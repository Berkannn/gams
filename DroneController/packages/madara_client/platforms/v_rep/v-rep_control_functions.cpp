/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

#pragma once

#ifdef V_REP


#include "platforms/platform.h"
#include "movement/platform_movement.h"
#include "sensors/platform_sensors.h"

#include "utilities/CommonMadaraVariables.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include <string>

// NOTE: We are using a hack here, assuming that an external Main module will set this KB to the common KB used by the system.
Madara::Knowledge_Engine::Knowledge_Base* m_sim_knowledge;

// Define the ids for the internal expressions.
enum VRepMadaraExpressionId 
{
    // Expression to send a movement command.
	VE_SEND_MOVE_COMMAND,
};

// Map of Madara expressions.
static std::map<VRepMadaraExpressionId, Madara::Knowledge_Engine::Compiled_Expression> m_expressions;

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Internal functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
static void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge);

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// General Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_platform()
{
    //// By default we identify ourselves by the hostname set in our OS.
    //std::string g_host ("");

    //// By default, we use the multicast port 239.255.0.1.:4150
    //const std::string DEFAULT_MULTICAST_ADDRESS ("239.255.0.1:4150");

    //// Used for updating various transport settings
    //Madara::Transport::Settings g_settings;

    //// Define the transport.
    //g_settings.hosts_.resize (1);
    //g_settings.hosts_[0] = DEFAULT_MULTICAST_ADDRESS;
    //g_settings.type = Madara::Transport::MULTICAST;
    //g_settings.id = 1000;
    //
    //// Create the knowledge base.
    //m_knowledge = new Madara::Knowledge_Engine::Knowledge_Base(g_host, g_settings);

    compileExpressions(m_sim_knowledge);

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Compiles all expressions to be used by this class.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void compileExpressions(Madara::Knowledge_Engine::Knowledge_Base* knowledge)
{
    // Expression to update the list of available drone positions, simply calls the predefined function.
    m_expressions[VE_SEND_MOVE_COMMAND] = knowledge->compile(
        // Simulator variables sent to V-Rep to simulate movements.
        "("
            MS_SIM_DEVICES_PREFIX "{.id}" + std::string(MV_MOVEMENT_REQUESTED) + "=" + MV_MOVEMENT_REQUESTED + ";"
            MS_SIM_DEVICES_PREFIX "{.id}" + std::string(MV_MOVEMENT_TARGET_LAT) + "=" + MV_MOVEMENT_TARGET_LAT + ";"
            MS_SIM_DEVICES_PREFIX "{.id}" + std::string(MV_MOVEMENT_TARGET_LON) + "=" + MV_MOVEMENT_TARGET_LON + ";"
        ")"
    );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Movement Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_control_functions()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void takeoff()
{
	
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void land()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_up()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_down()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_left()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_right()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_forward()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_backward()
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void move_to_location(double lat, double lon)
{
    // We will assume that lat and lon have already been loaded in the local Madara variables.
    m_sim_knowledge->evaluate(m_expressions[VE_SEND_MOVE_COMMAND]);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sensor Functions.
////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool init_sensor_functions()
{
	return true;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_thermal(double buffer[8][8])
{

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Gets the GPS coordinates from the simulator and gives them back to the 
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_gps(struct madara_gps * ret)
{
    // Get the latitude and longitude that the simulator set for this drone, in variables with the sim prefix.
    double latitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_PREFIX MV_DEVICE_LAT("{"MV_MY_ID"}"))).to_double();
    double longitude = m_sim_knowledge->get(m_sim_knowledge->expand_statement(MS_SIM_PREFIX MV_DEVICE_LON("{"MV_MY_ID"}"))).to_double();

    //std::cout << "Lat " << latitude << ", Long " << longitude << " from: " << std::string(MS_SIM_PREFIX MV_DEVICE_LAT("{"MV_MY_ID"}")) << std::endl;
    //m_sim_knowledge->print_knowledge();

    // Set the values in the return structure.
	ret->latitude = latitude;
	ret->longitude = longitude;
	ret->num_sats = 10;             // Just because it should be really exact with the simulator.
}

#endif