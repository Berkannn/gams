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
 * 3. The names �Carnegie Mellon University,� "SEI� and/or �Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN �AS-IS� BASIS. CARNEGIE MELLON
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

#include "VREP_UAV.h"


#include <iostream>
#include <iomanip>
#include <cmath>

#include "madara/knowledge_engine/containers/Double_Vector.h"

#include "gams/variables/Sensor.h"

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)
using std::endl;
using std::cout;
using std::string;

gams::platforms::VREP_UAV::VREP_UAV (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
  : VREP_Base (knowledge, sensors, self)
{
  if (knowledge && sensors && platforms && self)
  {
    (*platforms)[get_id ()].init_vars (*knowledge, get_id ());
    status_ = (*platforms)[get_id ()];

    self->device.desired_altitude = self->id.to_integer () + 1;
    add_model_to_environment ();
    set_initial_position ();
    set_move_speed(knowledge_->get(".move_speed").to_double());
    get_target_handle ();
    wait_for_go ();
  }
}

// TODO: handle epsilon
int
gams::platforms::VREP_UAV::move (const utility::Position & position,
  const double & epsilon)
{
  /**
   * VREP_UAV requires iterative movements for proper movement
   */
  // update variables
  Base::move (position, epsilon);

  // check if not airborne and takeoff if appropriate
  if (!airborne_)
    takeoff ();

  // convert form gps reference frame to vrep reference frame
  simxFloat dest_arr[3];
  const utility::GPS_Position *dest_gps_pos = dynamic_cast<const utility::GPS_Position *>(&position);
  utility::Position dest_pos;
  if(dest_gps_pos != NULL)
  {
    gps_to_vrep (*dest_gps_pos, dest_pos);
    position_to_array (dest_pos, dest_arr);
  }
  else
  {
    dest_pos = position;
    position_to_array (position, dest_arr);
  }

  //set current position of node target
  simxFloat curr_arr[3];
  utility::Position vrep_pos;
  utility::GPS_Position gps_pos (*get_position ());
  gps_to_vrep (gps_pos, vrep_pos);
  position_to_array (vrep_pos, curr_arr);

  // get distance to target
  double distance_to_target = dest_pos.distance_to_2d (vrep_pos);

  // move quadrotor target closer to the desired position
  if(distance_to_target < epsilon)
    return 2;
  if(distance_to_target < move_speed_) // we can get to target in one step
  {
    curr_arr[0] = dest_arr[0];
    curr_arr[1] = dest_arr[1];
    curr_arr[2] = dest_arr[2];
  }
  else // we cannot reach target in this step
  {
    // how far do we have to go in each dimension
    double dist[3];
    for (int i = 0; i < 3; ++i)
      dist[i] = fabs (curr_arr[i] - dest_arr[i]);

    // update target position
    for (int i = 0; i < 3; ++i)
    {
      if(curr_arr[i] < dest_arr[i])
        curr_arr[i] += dist[i] * move_speed_ / distance_to_target;
      else
        curr_arr[i] -= dist[i] * move_speed_ / distance_to_target;
    }
  }

  // send movement command
  simxSetObjectPosition (client_id_, node_target_, -1, curr_arr,
                        simx_opmode_oneshot_wait);

  return 1;
}

void
gams::platforms::VREP_UAV::add_model_to_environment ()
{
  string modelFile (getenv ("GAMS_ROOT"));
  modelFile += "/resources/vrep/Quadricopter_NoCamera.ttm";
  long ret;
  if ((ret = simxLoadModel (client_id_, modelFile.c_str (), 0, &node_id_,
    simx_opmode_oneshot_wait)) != simx_error_noerror)
  {
    cerr << "error loading VREP_UAV model in vrep: " << std::hex << ret << std::dec << endl;
    exit (-1);
  }
  else
    cerr << "loaded VREP_UAV model in vrep" << endl;
}

std::string gams::platforms::VREP_UAV::get_id () const
{
  return "vrep_uav";
}

std::string gams::platforms::VREP_UAV::get_name () const
{
  return "VREP UAV";
}

void
gams::platforms::VREP_UAV::get_target_handle ()
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
}

void
gams::platforms::VREP_UAV::set_initial_position () const
{
  // get initial position
  simxFloat pos[3];
  if (knowledge_->get (".initial_lat").to_double () != 0)
  {
    // get gps coords
    utility::GPS_Position gps_coord;
    gps_coord.latitude (knowledge_->get (".initial_lat").to_double ());
    gps_coord.longitude (knowledge_->get (".initial_lon").to_double ());

    // convert to vrep
    utility::Position vrep_coord;
    gps_to_vrep (gps_coord, vrep_coord);
    position_to_array (vrep_coord, pos);
  }
  else
  {
    // get vrep coords
    pos[0] = knowledge_->get (".initial_x").to_double ();
    pos[1] = knowledge_->get (".initial_y").to_double ();
  }

  // send set object position command
  pos[2] = knowledge_->get (".initial_alt").to_double ();

  //pos[2] = self_->id.to_integer () + 2;
  simxSetObjectPosition (client_id_, node_id_, -1, pos,
    simx_opmode_oneshot_wait);
  //simxSetObjectPosition (client_id_, node_target_, -1, pos,
    //simx_opmode_oneshot_wait);
}

#endif // _GAMS_VREP_
