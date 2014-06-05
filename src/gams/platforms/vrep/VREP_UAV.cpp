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
 * 3. The names “Carnegie Mellon University,” "SEI” and/or “Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN “AS-IS” BASIS. CARNEGIE MELLON
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

#ifdef _GAMS_VREP_

#include "VREP_UAV.h"

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

#include <iostream>
using std::endl;
using std::cout;
using std::string;
#include <cmath>

#include "gams/variables/Sensor.h"

#include "madara/knowledge_engine/containers/Double_Vector.h"

gams::platforms::VREP_UAV::VREP_UAV (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  variables::Sensors * sensors,
  variables::Platforms & platforms,
  variables::Self & self)
  : Base (&knowledge, sensors, self), airborne_ (false),
    move_speed_ (1)
{
  platforms["vrep_uav"].init_vars (knowledge, "vrep_uav");

  // grab coverage sensor
  variables::Sensors::iterator it = sensors->find ("coverage");
  if (it == sensors->end ()) // create coverage sensor
  {
    // get origin
    utility::GPS_Position origin;
    Madara::Knowledge_Engine::Containers::Native_Double_Array origin_container;
    origin_container.set_name ("sensor.coverage.origin", knowledge, 3);
    origin.from_container (origin_container);

    // establish sensor
    variables::Sensor* coverage_sensor =
      new variables::Sensor ("coverage", &knowledge, 1.0, origin);
    (*sensors)["coverage"] = coverage_sensor;
  }
  sensors_["coverage"] = (*sensors)["coverage"];

  // get client id
  client_id_ = simxStart(knowledge.get(".vrep_host").to_string ().c_str (),
    knowledge.get(".vrep_port").to_integer(), true, true, 2000, 5);
  if (client_id_ == -1)
  {
    cerr << "couldn't connect to vrep" << endl;
    exit (-1);
  }

  // get vrep environment data
  string sw = knowledge.get (".vrep_sw_position").to_string ();
  sscanf(sw.c_str (), "%lf,%lf", &sw_position_.lat, &sw_position_.lon);

  // init quadrotor in env
  string modelFile (getenv("GAMS_ROOT"));
  modelFile += "/resources/vrep/Quadricopter_NoCamera.ttm";
  node_id_ = -1;
  if (simxLoadModel (client_id_,modelFile.c_str (), 0, &node_id_,
    simx_opmode_oneshot_wait) != simx_error_noerror)
  {
    cerr << "error loading VREP_UAV model in vrep" << endl;
    exit (-1);
  }

  // set initial position if necessary
  int id = self.id.to_integer ();
  if (knowledge.get (".set_initial").to_integer ())
  {
    utility::Position obj_coord;
    obj_coord.x = knowledge.get (".initial_lat").to_double ();
    obj_coord.y = knowledge.get (".initial_lon").to_double ();
    obj_coord.z = id + 1; // TODO: remove when collision avoidance is added

    // do we need to convert from gps first?
    simxFloat pos[3];
    if (knowledge.get (".init_using_gps").to_integer () == 1)
    {
      utility::GPS_Position gps (obj_coord.x, obj_coord.y, obj_coord.z);
      utility::Position vrep_coord;
      gps_to_vrep (gps, vrep_coord);
      pos[0] = vrep_coord.x;
      pos[1] = vrep_coord.y;
      pos[2] = vrep_coord.z;
    }
    else
    {
      pos[0] = obj_coord.x;
      pos[1] = obj_coord.y;
      pos[2] = obj_coord.z;
    }

    // send set object position command
    simxSetObjectPosition (client_id_, node_id_, sim_handle_parent, pos,
      simx_opmode_oneshot_wait);
  }

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

  //find the target sub-object of the base sub-object
  node_target_ = -1;
  simxGetObjectChild (client_id_, nodeBase, 0,
    &node_target_, simx_opmode_oneshot_wait);

  // sync with other nodes; wait for all processes to get up
  std::stringstream buffer, init_string;
  init_string << "S";
  init_string << id;
  init_string << ".init";

  buffer << "(" << init_string.str () << " = 1)";
  buffer << " && begin_sim";
  std::string expression = buffer.str ();
  Madara::Knowledge_Engine::Wait_Settings wait_settings;
  wait_settings.send_list [init_string.str ()] = true;
  Madara::Knowledge_Engine::Compiled_Expression compiled;
  compiled = knowledge.compile (expression);
  knowledge.wait (compiled, wait_settings);
}

gams::platforms::VREP_UAV::~VREP_UAV ()
{
  simxInt childId = 0;
  while (childId != -1)
  {
    simxInt retVal = simxGetObjectChild (client_id_, node_id_, 0, &childId,
      simx_opmode_oneshot_wait);
    if (retVal != simx_error_noerror)
      cerr << "error getting child of node: " << node_id_ << endl;

    if(childId != -1)
    {
      retVal = simxRemoveObject (client_id_, childId, simx_opmode_oneshot_wait);
      if(retVal != simx_error_noerror)
        cerr << "error removing child id " << childId << endl;
    }
  }

  if (simxRemoveObject (client_id_, node_id_, simx_opmode_oneshot_wait)
    != simx_error_noerror)
  {
    cerr << "error deleting node " << node_id_ << endl;
  }
}

void
gams::platforms::VREP_UAV::operator= (const VREP_UAV & rhs)
{
  if (this != &rhs)
  {
    platforms::Base * dest = dynamic_cast <platforms::Base *> (this);
    const platforms::Base * source =
      dynamic_cast <const platforms::Base *> (&rhs);

    *dest = *source;
  }
}

int
gams::platforms::VREP_UAV::home (void)
{
  // check if home has been set
  if (self_.device.home.size () == 3)
  {
    // read the home position
    utility::GPS_Position position;
    position.from_container (self_.device.home);

    // move to home
    move (position);
  }

  return 0;
}

int
gams::platforms::VREP_UAV::land (void)
{
  if (airborne_)
  {
    airborne_ = false;

    // TODO: vrep land
  }

  return 0;
}

// TODO: handle epsilon
int
gams::platforms::VREP_UAV::move (const utility::GPS_Position & position,
  const double & /*epsilon*/)
{
  // check if not airborne and takeoff if appropriate
  if (!airborne_)
    takeoff ();

  // convert form gps reference frame to vrep reference frame
  simxFloat dest_arr[3];
  utility::Position dest_pos;
  gps_to_vrep (position, dest_pos);
  position_to_array (dest_pos, dest_arr);

  //set current position of node target
  simxFloat curr_arr[3];
  utility::Position vrep_pos;
  gps_to_vrep (position_, vrep_pos);
  position_to_array (vrep_pos, curr_arr);

  // get distance to target
  double distance_to_target = dest_pos.distance (vrep_pos);

  // move quadrotor target closer to the desired position
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
  simxSetObjectPosition (client_id_, node_target_, sim_handle_parent, curr_arr,
                        simx_opmode_oneshot_wait);

  return 0;
}

int
gams::platforms::VREP_UAV::sense (void)
{
  static unsigned int executions = 0;
  
  // get position
  simxFloat curr_arr[3];
  simxGetObjectPosition (client_id_, node_id_, sim_handle_parent, curr_arr,
                        simx_opmode_oneshot_wait);

  utility::Position vrep_pos;
  array_to_position (curr_arr, vrep_pos);
  vrep_to_gps (vrep_pos, position_);

  // set position in madara
  position_.to_container (self_.device.location);

  // set position on coverage map
  sensors_["coverage"]->set_value (position_, executions);

  ++executions;
  return 0;
}

int
gams::platforms::VREP_UAV::takeoff (void)
{
  if (!airborne_)
  {
    airborne_ = true;

    // TODO: vrep takeoff
  }

  return 0;
}

int
gams::platforms::VREP_UAV::analyze (void)
{
  return 0;
}

void
gams::platforms::VREP_UAV::get_sensors (variables::Sensor_Names & sensors)
{
  bool needs_change (false);

  if (sensors.size () != 1)
  {
    needs_change = true;
    sensors.resize (1);
  }
  else
  {
    if (sensors[0] != "thermal")
        needs_change = true;
  }

  if (needs_change)
  {
    sensors[0] = "thermal";
  }
}

void
gams::platforms::VREP_UAV::get_position (utility::GPS_Position & position)
{
  position = position_;
}

double
gams::platforms::VREP_UAV::get_move_speed ()
{
  return move_speed_;
}

void
gams::platforms::VREP_UAV::set_move_speed (const double& speed)
{
  move_speed_ = speed;
}

double
gams::platforms::VREP_UAV::get_position_accuracy () const
{
  return 1.0;
}

void 
gams::platforms::VREP_UAV::gps_to_vrep (const utility::GPS_Position & position,
  utility::Position & converted) const
{
  // assume the Earth is a perfect sphere
  const double EARTH_RADIUS = 6371000.0;
  const double EARTH_CIRCUMFERENCE = 2 * EARTH_RADIUS * M_PI;

  // convert the latitude/x coordinates
  converted.y = (position.lat - sw_position_.lat) / 360.0 * EARTH_CIRCUMFERENCE;
  
  // assume the meters/degree longitude is constant throughout environment
  // convert the longitude/y coordinates
  double r_prime = EARTH_RADIUS * cos (DEG_TO_RAD (sw_position_.lat));
  double circumference = 2 * r_prime * M_PI;
  converted.x = (position.lon - sw_position_.lon) / 360.0 * circumference;

  // do nothing to altitude
  converted.z = position.alt;
}

void 
gams::platforms::VREP_UAV::vrep_to_gps (const utility::Position & position,
  utility::GPS_Position & converted) const
{
  // assume the Earth is a perfect sphere
  const double EARTH_RADIUS = 6371000.0;
  const double EARTH_CIRCUMFERENCE = 2 * EARTH_RADIUS * M_PI;

  // convert the latitude/x coordinates
  // VREP uses y for latitude
  converted.lat = (360.0 * position.y / EARTH_CIRCUMFERENCE) + sw_position_.lat;
  
  // assume the meters/degree longitude is constant throughout environment
  // convert the longitude/y coordinates
  // VREP uses x for longitude
  double r_prime = EARTH_RADIUS * cos (DEG_TO_RAD (sw_position_.lat));
  double circumference = 2 * r_prime * M_PI;
  converted.lon = (360.0 * position.x / circumference) + sw_position_.lon;

  // do nothing to altitude
  converted.alt = position.z;
}

inline void
gams::platforms::VREP_UAV::position_to_array(const utility::Position & pos,
  simxFloat (&arr)[3]) const
{
  // have to swap x and y for vrep
  arr[0] = pos.y;
  arr[1] = pos.x;
  arr[2] = pos.z;
}

inline void
gams::platforms::VREP_UAV::array_to_position(const simxFloat (&arr)[3],
  utility::Position & pos) const
{
  // have to swap x and y for vrep
  pos.x = arr[1];
  pos.y = arr[0];
  pos.z = arr[2];
}

#endif // _GAMS_VREP_
