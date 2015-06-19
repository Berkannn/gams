/**
 * Copyright (c) 2014 Carnegie Mellon University. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following acknowledgments and disclaimers.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 
 * 3. The names "Carnegie Mellon University," "SEI" and/or "Software
 *    Engineering Institute" shall not be used to endorse or promote products
 *    derived from this software without prior written permission. For written
 *    permission, please contact permission@sei.cmu.edu.
 * 
 * 4. Products derived from this software may not be called "SEI" nor may "SEI"
 *    appear in their names without prior written permission of
 *    permission@sei.cmu.edu.
 * 
 * 5. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 * 
 *      This material is based upon work funded and supported by the Department
 *      of Defense under Contract No. FA8721-05-C-0003 with Carnegie Mellon
 *      University for the operation of the Software Engineering Institute, a
 *      federally funded research and development center. Any opinions,
 *      findings and conclusions or recommendations expressed in this material
 *      are those of the author(s) and do not necessarily reflect the views of
 *      the United States Department of Defense.
 * 
 *      NO WARRANTY. THIS CARNEGIE MELLON UNIVERSITY AND SOFTWARE ENGINEERING
 *      INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
 *      UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *      IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *      FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *      OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *      NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *      TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *      This material has been approved for public release and unlimited
 *      distribution.
 **/

#ifdef _GAMS_VREP_ // only compile this if we are simulating in VREP

#include "VREP_UAV_Ranger.h"


#include <iostream>
#include <cmath>

#include "madara/knowledge_engine/containers/Double_Vector.h"

#include "gams/variables/Sensor.h"

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)
using std::endl;
using std::cout;
using std::string;

gams::platforms::VREP_UAV_Ranger *
gams::platforms::VREP_UAV_Ranger_Factory::make_new (
  const Madara::Knowledge_Vector & /*args*/,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
{
  return new VREP_UAV_Ranger(knowledge, sensors, platforms, self);
}

gams::platforms::VREP_UAV_Ranger *
gams::platforms::VREP_UAV_Ranger_Factory::create (
        const Madara::Knowledge_Vector & args,
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self)
{
  VREP_UAV_Ranger * result (dynamic_cast<VREP_UAV_Ranger*>(
     VREP_Aerial_Base_Factory::create(args, knowledge, sensors, platforms, self)));
  if(result)
    result->get_laser_sensor_handle();
  return result;
}

gams::platforms::VREP_UAV_Ranger::VREP_UAV_Ranger (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
  : VREP_Aerial_Base (knowledge, sensors, platforms, self)
{ }

std::string gams::platforms::VREP_UAV_Ranger::get_id () const
{
  return "vrep_uav_ranger";
}

std::string gams::platforms::VREP_UAV_Ranger::get_name () const
{
  return "VREP UAV Ranger";
}

std::string gams::platforms::VREP_UAV_Ranger::get_model_filename () const
{
  return "Quadricopter_Laser.ttm";
}

double gams::platforms::VREP_UAV_Ranger::get_range() const
{
  simxUChar detectionState;
  simxFloat detectedPoint[3];
  simxInt ret = simxReadProximitySensor(client_id_, laser_sensor_,
    &detectionState, detectedPoint, NULL, NULL, simx_opmode_oneshot_wait);
  if(detectionState == 0)
    return -2;
  else
    return sqrt(detectedPoint[0] * detectedPoint[0] +
                detectedPoint[1] * detectedPoint[1] +
                detectedPoint[2] * detectedPoint[2]);
}

void
gams::platforms::VREP_UAV_Ranger::get_target_handle ()
{
  //find the dummy base sub-object
  simxInt handlesCount = 0,*handles = NULL;
  simxInt parentsCount = 0,*parents = NULL;
  simxGetObjectGroupData (client_id_, sim_object_dummy_type, 2, &handlesCount,
    &handles, &parentsCount, &parents, NULL, NULL, NULL, NULL,
    simx_opmode_oneshot_wait);

  // find node base
  simxInt nodeBase = -1;
  for(simxInt i = 0; i < handlesCount; ++i)
  {
    if(parents[i] == node_id_)
    {
      nodeBase = handles[i];
      break;
    }
  }

  // find the target sub-object of the base sub-object
  node_target_ = -1;
  simxGetObjectChild (client_id_, nodeBase, 0, &node_target_,
    simx_opmode_oneshot_wait);

  if (node_target_ < 0)
  {
    GAMS_DEBUG (gams::utility::LOG_EMERGENCY, (LM_DEBUG, 
      DLINFO "gams::platforms::VREP_UAV_Ranger::get_target_handle:" \
      " invalid target handle id\n"));
  }
}

void
gams::platforms::VREP_UAV_Ranger::get_laser_sensor_handle ()
{
  //find the dummy base sub-object
  simxInt handlesCount = 0,*handles = NULL;
  simxInt parentsCount = 0,*parents = NULL;
  simxGetObjectGroupData (client_id_, sim_object_proximitysensor_type, 2, &handlesCount,
    &handles, &parentsCount, &parents, NULL, NULL, NULL, NULL,
    simx_opmode_oneshot_wait);

  // find sensor
  for(simxInt i = 0; i < handlesCount; ++i)
  {
    if(parents[i] == node_id_)
    {
      laser_sensor_ = handles[i];
      break;
    }
  }
}

#endif // _GAMS_VREP_
