/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * MadaraSystemController.h - Class that simulates a System Controller.
 *********************************************************************/

#pragma once

#ifndef _MADARA_SYSTEM_CONTROLLER_H
#define _MADARA_SYSTEM_CONTROLLER_H

#include "madara/knowledge_engine/Knowledge_Base.h"
#include "Position.h"

#include <vector>
#include <string>

using std::string;

// Class that simulates the Madara controller of the system (though it also acts as a bridge between
// the drone information given by the simualted drones and the Madara knowledge base).
class MadaraController
{
private:
    // The controller's id.
    int m_id;

    // Whether human detection should be on or off: 1 is on, 0 is off.
    int m_humanDetection;

    // The actual knowledge base.
    Madara::Knowledge_Engine::Knowledge_Base* m_knowledge;

    // A counter for the regions created.
    int m_regionId;
public:
    MadaraController(int id, std::string platform);
    ~MadaraController();

    // Commands to take off and land the swarm.
    void sendTakeoffCommand();
    void sendLandCommand();

    // Sets basic parameters.
    void setParamNumDrones(const int& numberOfDrones);
    void setParamCommRange(const double& commRange);
    void setParamMinAltitude(const double& minAltitude);
    void setParamHeightDiff(const double& heightDiff);
    void setParamCoverageTracking(const int& enabled, const int& writeToFile);
    void setParamThermalSensorAngle(const double& thermalSensorAngle);

    // This will disseminate all parameters previously set.
    void disseminateParameters();

    // Bridge methods.
    void setupBridgeRequest(int bridgeId, SMASH::Utilities::Region startRegion, 
                                          SMASH::Utilities::Region endRegion, std::string bridgeAlgorithm = "");

    // Area coverage methods.
    void setNewSearchArea(int searchAreaId, SMASH::Utilities::Region& areaBoundaries);
    void setPrioritizedSearchParameters(const double& defaultPriority, const std::string& prioritizedAreas);
    void requestAreaCoverage(std::vector<int> droneIds, int searchAreaId, string searchAlgorithm, int wait, double lineWidth, 
                             std::string humanDetectionAlgorithm);

    // Information getter methods.
    std::vector<SMASH::Utilities::Position> getCurrentLocations();
    std::vector<SMASH::Utilities::Position> getCurrentThermals();

    void printKnowledge();
};

#endif
