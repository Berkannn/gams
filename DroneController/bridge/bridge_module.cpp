/********************************************************************* 
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * bridge_module.cpp - Defines the manager for bulding a bridge.
 *********************************************************************/

#include <vector>
#include <map>
#include <math.h>
#include "utilities/CommonMadaraVariables.h"
#include "bridge_module.h"
#include "LineBridgeAlgorithm.h"

using namespace SMASH::Bridge;
using namespace SMASH::Utilities;

#define DEFAULT_MAX_COMM_DISTANCE_METERS      4.0                   // The default max range for the radio the bridge will be using to transfer data, in meters.
#define DEFAULT_BRIDGE_ALGORITHM              MO_BRIDGE_ALGO_LINE   // The default bridge algorithm.

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Madara Variable Definitions
////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions.
#define MF_MAIN_LOGIC				"bridge_doBridgeBuilding"					// Function that checks if there is bridge building to be done, and does it.
#define MF_UPDATE_AVAILABLE_DRONES	"bridge_updateAvailableDrones"				// Function that checks the amount and positions of drones ready for bridging.
#define MF_SETUP_BRIDGE_PROCESSING  "bridge_setupBridgeProcessing"              // Function to setup initial variables to calculate bridge info.
#define MF_FIND_NEXT_POS_FOR_BRIDGE "bridge_findPositionInBridge"				// Function that finds and sets the position in the bridge for this drone, if any.
#define MF_TURN_OFF_BRIDGE_REQUEST	"bridge_turnOffBridgeRequest"				// Function to turn off a bridge request locally.
#define MF_BRIDGE_LOCATION_REACHED  "bridge_bridgeLocationReached"              // Function to check if the bridge location we are assigned to has been reached.

// Internal variables.
#define MV_AVAILABLE_DRONES_AMOUNT	".bridge.devices.available.total"			    // The amount of available drones.
#define MV_AVAILABLE_DRONES_IDS		".bridge.devices.available.ids"			        // Array of the ids of the available drones.
#define MV_AVAILABLE_DRONES_LAT	    ".bridge.devices.available.latitudes"		    // Array of the lat part of the position of the drones indicated by MV_AVAILABLE_DRONES_IDS.
#define MV_AVAILABLE_DRONES_LON	    ".bridge.devices.available.longitudes"		    // Array of the long part of the position of the drones indicated by MV_AVAILABLE_DRONES_IDS.
#define MV_CURR_BRIDGE_ID	        ".bridge.curr_bridge_id"		                // Indicates the id of the bridge we are currently part of.
#define MV_BRIDGE_CHECKED(bridgeId) ".bridge." + std::string(bridgeId) + ".checked" // Indicates that we already checked if we had to be part of this bridge.
#define MV_MOVING_TO_BRIDGE         ".bridge.moving_to_bridge"                      // 1 if we are moving towards a bridge.
#define MV_BRIDGE_LOC_LAT           ".bridge.location.latitude"
#define MV_BRIDGE_LOC_LON           ".bridge.location.longitude"

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private variables.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define the ids for the expressions that will exist to accomplish different aspects of bridge building.
enum BridgeMadaraExpressionId 
{
    // Expression to call function to update the positions of the drones available for a bridge.
  BE_FIND_AVAILABLE_DRONES_POSITIONS,
};

// Map of Madara expressions used in bridge building.
static std::map<BridgeMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

// Positions used for the bridge.
static Position* m_sinkPosition;
static Position* m_sourcePosition;

// Algorithm in use.
static BridgeAlgorithm* m_selectedBridgeAlgorithm;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Private function declarations.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge);
static Madara::Knowledge_Record madaraFindPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraTurnOffBridgeRequest (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables);
static Position* calculateMiddlePoint(Madara::Knowledge_Engine::Variables &variables, std::string regionId);
static std::map<int, Position> getAvailableDronesPositions(Madara::Knowledge_Engine::Variables &variables);
static Madara::Knowledge_Record madaraSetupBridgeProcessing(Madara::Knowledge_Engine::Function_Arguments &args,
  Madara::Knowledge_Engine::Variables &variables);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initializer, gets the refence to the knowledge base and compiles expressions.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Bridge::BridgeModule::initialize(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Defines internal and external functions.
    defineFunctions(knowledge);

    // Initialize some default and starting values, locally.
    knowledge.set(MV_COMM_RANGE, DEFAULT_MAX_COMM_DISTANCE_METERS,
                  Madara::Knowledge_Engine::Eval_Settings(true, true));
    knowledge.set(MV_BRIDGE_ALGO_REQUESTED, DEFAULT_BRIDGE_ALGORITHM,
                  Madara::Knowledge_Engine::Eval_Settings(true, true));

    // Registers all default expressions, to have them compiled for faster access.
    compileExpressions(knowledge);

    // Initializations.
    m_sourcePosition = NULL;
    m_sinkPosition = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Cleanup, not needed in this module.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SMASH::Bridge::BridgeModule::cleanup(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the main logic to be run. This returns a function call that can be included in another block of logic.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::string SMASH::Bridge::BridgeModule::get_core_function()
{
    return MF_MAIN_LOGIC "()";
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Registers functions with Madara.
// ASSUMPTION: Drone IDs are continuous, starting from 0.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void defineFunctions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Function that can be included in main loop of another method to introduce bridge building. 
    // Only does the actual bridge calculations if the command to do so is on.
    // Assumes that MV_USER_BRIDGE_REQUEST_ON triggers the bridge building logic.
    knowledge.define_function(MF_MAIN_LOGIC, 
        "("
            // Only used when there is a new bridge request.
            MV_BRIDGE_REQUESTED " => "
            "("
                "#print('New bridge request received.\n');"
                // We turn off the bridge request, but only locally.
                MF_TURN_OFF_BRIDGE_REQUEST "();"

                // If we are available and there are bridges to check, check if we can help.
                "(" MV_MOBILE("{" MV_MY_ID "}") " && (!" MV_BUSY("{" MV_MY_ID "}") ") && (" MV_TOTAL_BRIDGES " > 0)" ")"
                " => " 
                    "("
                        // Find out if we have to move somewhere to build the bridge.
                        // The position may be a step towards the bridge, or
                        // the final bridge location, depending on the algorithm.
                        "#print('Checking bridge request.\n');"
                        MF_SETUP_BRIDGE_PROCESSING "();"
                        MF_FIND_NEXT_POS_FOR_BRIDGE "();"
                    ");"
            ");"
            // Only used when we are in the process of moving towards a bridge.
            MV_MOVING_TO_BRIDGE " => "
            "("
                "#print('Moving to location in bridge.\n');"
                // If we reached our bridge location, record that and land.
                MV_REACHED_GPS_TARGET " => " 
                "("
                    "#print('Location in bridge reached.\n');"
                    MV_MOVING_TO_BRIDGE "= 0;"

                    // Tell command module to land.
                    "(" MV_MOVEMENT_REQUESTED "='" MO_LAND_CMD "');"
                ");"

                // If we have not reached our target, check if we have to 
                // recalculate (depending on the algorithm, this may or may 
                // not be necessary).
                "(!"MV_REACHED_GPS_TARGET") =>"
                "("
                    MF_FIND_NEXT_POS_FOR_BRIDGE "();"
                ")"
            ");"
        ")"
    );

    // Function to setup bridge request processing.
    knowledge.define_function(MF_SETUP_BRIDGE_PROCESSING, madaraSetupBridgeProcessing);

    // Function that actually performs bridge building for this drone.
    knowledge.define_function(MF_FIND_NEXT_POS_FOR_BRIDGE, madaraFindPositionInBridge);

    // Function that actually performs bridge building for this drone.
    knowledge.define_function(MF_TURN_OFF_BRIDGE_REQUEST, madaraTurnOffBridgeRequest);

    // Function to update the amound and positions of drones available for bridges.
    // @param .0    Bridge id, an integer indicating the bridge id for which we are finding available drones.
    knowledge.define_function(MF_UPDATE_AVAILABLE_DRONES, 
        // Set available drones to 0 and disregard its return (choose right).
        MV_AVAILABLE_DRONES_AMOUNT " = 0 ;>"

        // Loop over all the drones to find the available ones.
        ".i[0->" MV_TOTAL_DEVICES ")"
        "("
            // A drone is available if it is mobile and it is not busy, or if it is busy but busy helping with this same bridge.
            "(" MV_MOBILE("{.i}") " && (!" MV_BUSY("{.i}") " || "
                                        "(" MV_BUSY("{.i}") " && (" MV_BRIDGE_ID("{.i}") " == .0) )"
                                        ")"
            ")"
            " => "
            "("
                // If so, increase the amount of available drones, and store its id, x position, and y position in arrays.
                MV_AVAILABLE_DRONES_IDS "{" MV_AVAILABLE_DRONES_AMOUNT "} = .i;"
                MV_AVAILABLE_DRONES_LAT "{" MV_AVAILABLE_DRONES_AMOUNT "} = " MV_DEVICE_LAT("{.i}") ";"
                MV_AVAILABLE_DRONES_LON "{" MV_AVAILABLE_DRONES_AMOUNT "} = " MV_DEVICE_LON("{.i}") ";"
                "++" MV_AVAILABLE_DRONES_AMOUNT ";"
            ");"
        ");"
    );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base &knowledge)
{
    // Expression to update the list of available drone positions, simply calls the predefined function.
    m_expressions[BE_FIND_AVAILABLE_DRONES_POSITIONS] = knowledge.compile(
        MF_UPDATE_AVAILABLE_DRONES "(" MV_CURR_BRIDGE_ID ");"
    );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Method to turn off a bridge request locally.
 * Will be called from an external Madara function.
 * @return  Returns true (1) always.
 **/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraTurnOffBridgeRequest (Madara::Knowledge_Engine::Function_Arguments &args,
             Madara::Knowledge_Engine::Variables &variables)
{
    // Turn off the request by setting the global variable to 0 but only locally.
    variables.set(MV_BRIDGE_REQUESTED, (Madara::Knowledge_Record::Integer) 0.0, Madara::Knowledge_Engine::Eval_Settings(false, true));

    return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sets internally information about the sink and source for a particular bridge.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraSetSourceAndSinkPositions(Madara::Knowledge_Engine::Function_Arguments &args,
  Madara::Knowledge_Engine::Variables &variables)
{
  // Cleanup, if needed.
  if(m_sourcePosition != NULL)
  {
    delete m_sourcePosition;
  }

  if(m_sinkPosition != NULL)
  {
    delete m_sinkPosition;
  }

  // Simplify the source region for now, treating it as a point by calculating the middle point.
  std::string bridgeSourceRegionId = variables.get(variables.expand_statement(MV_BRIDGE_SOURCE_REGION_ID("{" MV_CURR_BRIDGE_ID "}"))).to_string();
  m_sourcePosition = calculateMiddlePoint(variables, bridgeSourceRegionId);

  // Simplify the sink region for now, treating it as a point by calculating the middle point.
  std::string bridgeSinkRegionId = variables.get(variables.expand_statement(MV_BRIDGE_SINK_REGION_ID("{" MV_CURR_BRIDGE_ID "}"))).to_string();
  m_sinkPosition = calculateMiddlePoint(variables, bridgeSinkRegionId);

  // Check if we couldn't find the required positions, returning immediately with false.
  if(m_sourcePosition == NULL || m_sinkPosition == NULL)
  {
    return Madara::Knowledge_Record(0.0);
  }

  return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraSetupBridgeProcessing(Madara::Knowledge_Engine::Function_Arguments &args,
  Madara::Knowledge_Engine::Variables &variables)
{
  // Store the current bridge ID in the base to be used by the next evaluations.
  variables.evaluate(MV_CURR_BRIDGE_ID " = " MV_BRIDGE_REQUESTED);

  // Setup the source and sink variables.
  madaraSetSourceAndSinkPositions(args, variables);

  // Reset the algorithm variable.
  if(m_selectedBridgeAlgorithm != NULL)
  {
    delete m_selectedBridgeAlgorithm;
  }
  m_selectedBridgeAlgorithm = NULL;

  // Select the appropriate algorithm.
  // NOTE: for now the default algorithm is used, as no scene supports setting and disseminating this value.
  std::string algorithmName = variables.get(MV_BRIDGE_ALGO_REQUESTED).to_string();
  if(algorithmName == MO_BRIDGE_ALGO_LINE)
  {
    m_selectedBridgeAlgorithm = new LineBridgeAlgorithm();
  }
  else
  {
    // TODO: Do something.
    return Madara::Knowledge_Record(0.0);
  }

  return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Calls the corresponding algorithm to find our potential position in the bridge,
 * and sets the corresponding variables to actually move there.
 * Will be called from an external Madara function.
 * @return  Returns true (1) if it can calculate the bridge, or false (0) if it couldn't find all required data.
 **/
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Madara::Knowledge_Record madaraFindPositionInBridge (Madara::Knowledge_Engine::Function_Arguments &args,
  Madara::Knowledge_Engine::Variables &variables)
{
  // Get the positions of all available drones.
  std::map<int, Position> availableDronePositions = getAvailableDronesPositions(variables);

  // Call the bridge algorithm to find out if I have to move to help with the bridge.
  int myId = (int) variables.get(MV_MY_ID).to_integer();
  double commRange = variables.get(MV_COMM_RANGE).to_double();
  Position* myNewPosition = m_selectedBridgeAlgorithm->getPositionInBridge(myId, commRange, *m_sourcePosition, *m_sinkPosition, availableDronePositions);

  // If I have to move, tell Madara that I am in bridging mode, and set my final destination.
  bool iHaveToGoToBridge = myNewPosition != NULL;
  if(iHaveToGoToBridge)
  {
    // Set all the command parameters, resetting the "reached" variable as we want to move to a new location.
    // TODO: having to manually reset the REACHED variable is a bit clumsy, could be improved, but this
    // is part of the Movement module.
    variables.set(MV_MOVEMENT_TARGET_LAT, myNewPosition->latitude);
    variables.set(MV_MOVEMENT_TARGET_LON, myNewPosition->longitude);
    variables.set(MV_MOVEMENT_REQUESTED, std::string(MO_MOVE_TO_GPS_CMD));
    variables.set(MV_REACHED_GPS_TARGET, Madara::Knowledge_Record::Integer(0));

    // Locally store our destination.
    variables.set(MV_BRIDGE_LOC_LAT, myNewPosition->latitude);
    variables.set(MV_BRIDGE_LOC_LON, myNewPosition->longitude);

    // Locally note that we are moving towards a bridge.
    variables.set(MV_MOVING_TO_BRIDGE, 1.0);

    // Update the drone status now that we are going to build a bridge.
    variables.set(variables.expand_statement(MV_BUSY("{" MV_MY_ID "}")), 1.0,
        Madara::Knowledge_Engine::Eval_Settings(true));
    variables.evaluate(variables.expand_statement(MV_BRIDGE_ID("{" MV_MY_ID "}  = " MV_BRIDGE_REQUESTED)));
  }

  // Cleanup.
  if(myNewPosition != NULL)
  {
    delete myNewPosition;
  }

  return Madara::Knowledge_Record(1.0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Obtains the positions of all the drones available to be part of the bridge.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
std::map<int, Position> getAvailableDronesPositions(Madara::Knowledge_Engine::Variables &variables)
{
    // Find all the available drones and their positions, called here to ensure atomicity and we have the most up to date data.
    variables.evaluate(m_expressions[BE_FIND_AVAILABLE_DRONES_POSITIONS], Madara::Knowledge_Engine::Eval_Settings(true, true));

    // Obtain the ids, and x and y parts of the available drone's positions from Madara.
    int availableDrones = (int) variables.get(MV_AVAILABLE_DRONES_AMOUNT).to_integer();
    std::vector<Madara::Knowledge_Record> availableDronesIds;
    std::vector<Madara::Knowledge_Record> availableDronesLats;
    std::vector<Madara::Knowledge_Record> availableDronesLongs;
    variables.to_vector(MV_AVAILABLE_DRONES_IDS, 0, availableDrones, availableDronesIds);
    variables.to_vector(MV_AVAILABLE_DRONES_LAT, 0, availableDrones, availableDronesLats);
    variables.to_vector(MV_AVAILABLE_DRONES_LON, 0, availableDrones, availableDronesLongs);

    // Move the ids and position coordinates from the three arrays into a combined map.
    std::map<int, Position> availableDronePositions;
    for(int i=0; i < availableDrones; i++)
    {
      Position currDronePos;
      int currDroneId = (int) availableDronesIds[i].to_integer();
      currDronePos.latitude = availableDronesLats[i].to_double();
      currDronePos.longitude = availableDronesLongs[i].to_double();
      availableDronePositions[currDroneId] = currDronePos;
    }

    return availableDronePositions;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Calculates the middle point of a given region.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
Position* calculateMiddlePoint(Madara::Knowledge_Engine::Variables &variables, std::string regionId)
{
    Position* middlePoint = NULL;

    // Calculate the mid point depending on the region type obtained from Madara.
    int regionType = (int) variables.get(MV_REGION_TYPE(regionId)).to_integer();   
    switch(regionType)
    {
        case 0: // Rectangle.
        {
            // Get the bounding box from Madara.
            double northWestLat = variables.get(MV_REGION_TOPLEFT_LAT(regionId)).to_double();
            double northWestLong = variables.get(MV_REGION_TOPLEFT_LON(regionId) ).to_double();
            double southEastLat = variables.get(MV_REGION_BOTRIGHT_LAT(regionId)).to_double();
            double southEastlong = variables.get(MV_REGION_BOTRIGHT_LON(regionId) ).to_double();

            // Just find the middle point of the diagonal between the two border points.
            middlePoint = new Position();
            middlePoint->latitude = northWestLat - fabs(northWestLat - southEastLat)/2.0;
            middlePoint->longitude = northWestLong + fabs(northWestLong - southEastlong)/2.0;
        }
    }

    return middlePoint;
}
