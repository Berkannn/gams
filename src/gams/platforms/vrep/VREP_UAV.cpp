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
#include "VREP_UAV.h"

#include <iostream>
using std::endl;
using std::cout;
using std::string;

#ifdef _GAMS_VREP_

gams::platforms::VREP_UAV::VREP_UAV (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  variables::Sensors * sensors,
  variables::Platforms & platforms,
  variables::Self & self)
  : Base (&knowledge, sensors), self_ (self), airborne_ (false)
{
  platforms["vrep_uav"].init_vars (knowledge, "vrep_uav");

  // get client id
  client_id_ = simxStart(knowledge.get(".vrep_host").to_string ().c_str (),
    knowledge.get(".vrep_port").to_integer(), true, true, 2000, 5);

  // init quadrotor in env
  string modelFile (getenv("VREP_ROOT"));
  modelFile += "/models/robots/mobile/Quadricopter.ttm";
  node_id_ = -1;
  if (simxLoadModel (client_id_,modelFile.c_str (), 0, &node_id_,
    simx_opmode_oneshot_wait) != simx_error_noerror)
  {
    // fail out some how
  }

  // set initial position if necessary
  if(knowledge.get (".set_initial").to_integer ())
  {
    simxFloat objCoord[3];
    objCoord[0] = knowledge.get (".initial_x").to_double ();
    objCoord[1] = knowledge.get (".initial_y").to_double ();
    // TODO: remove when collision avoidance is added
    objCoord[2] = knowledge.get (".id").to_integer () + 1;
    simxSetObjectPosition (client_id_, node_id_, sim_handle_parent, objCoord,
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

  // sync with other nodes
  int id = knowledge.get (".id").to_integer ();
  int processes = knowledge.get ("num_agents").to_integer ();

  // wait for all processes to get up
  std::stringstream buffer;
  buffer << "(S" << id << ".init = 1)";
  for (int i = 0; i < processes; ++i)
    buffer << " && S" << i << ".init";
  std::string expression = buffer.str ();
  Madara::Knowledge_Engine::Compiled_Expression compiled;
  compiled = knowledge.compile (expression);
  knowledge.wait (compiled);

  // start the simulation
  if(id == 0)
    simxStartSimulation (client_id_, simx_opmode_oneshot_wait);
}

gams::platforms::VREP_UAV::~VREP_UAV ()
{
//  simxInt childId = 0;
//  while(childId != -1)
//  {
//    simxInt retVal = simxGetObjectChild (client_id_,node_id_,0,&childId,
//      simx_opmode_oneshot_wait);
//    if (retVal != simx_error_noerror)
//      cout << "error getting child of node: " << node_id_ << endl;
//
//    if(childId != -1)
//    {
//      retVal = simxRemoveObject(client_id_,childId,simx_opmode_oneshot_wait);
//      if(retVal != simx_error_noerror)
//        cout << "error removing child id " << childId << endl;
//    }
//  }
//
//  if (simxRemoveObject(client_id_,node_id_,simx_opmode_oneshot_wait)
//    != simx_error_noerror)
//  {
//    cout << "error deleting node " << node_id_ << endl;
//  }
}

void
gams::platforms::VREP_UAV::operator= (const VREP_UAV & rhs)
{
  if (this != &rhs)
  {
    this->sensors_ = rhs.sensors_;
    this->status_ = rhs.status_;
    this->self_ = rhs.self_;
  }
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
gams::platforms::VREP_UAV::get_position (utility::Position & position)
{
  position = position_;
}

int
gams::platforms::VREP_UAV::home (void)
{
  // check if home has been set
  if (self_.device.home.size () == 3)
  {
    // read the home position
    utility::Position position;
    position.from_container (self_.device.home);

    // move to home
    move (position);
  }

  return 0;
}

void 
gams::platforms::VREP_UAV::coord_to_vrep(const utility::Position & position, simxFloat (&converted)[3])
{
  // TODO: fill out conversion
  return;
}


int
gams::platforms::VREP_UAV::move (const utility::Position & position)
{
  // check if not airborne and takeoff if appropriate
  if (!airborne_)
    takeoff ();

  // move to the position
  simxFloat destPos[3];
  destPos[0] = position.x;
  destPos[1] = position.y;
  destPos[2] = position.z;
  coord_to_vrep (position, destPos);

  //set current position of node target
  simxFloat currPos[3];
  currPos[0] = position_.x;
  currPos[1] = position_.y;
  currPos[2] = position_.z;

  //move target closer to the waypoint and return 1
  // TODO: modify for straight line movement
  bool at_destination = true;
  const double TARGET_INCR = 0.5;
  for (int i = 0;i < 3;++i)
  {
    if(currPos[i] < destPos[i] - TARGET_INCR)
    {
      currPos[i] += TARGET_INCR;
      at_destination = false;
    }
    else if(currPos[i] > destPos[i] + TARGET_INCR)
    {
      currPos[i] -= TARGET_INCR;
      at_destination = false;
    }
    else
      currPos[i] = destPos[i];
  }
  simxSetObjectPosition (client_id_, node_target_, sim_handle_parent, currPos,
                        simx_opmode_oneshot_wait);

  return 0;
}
      
int
gams::platforms::VREP_UAV::sense (void)
{
  // get position
  simxFloat currPos[3];
  simxGetObjectPosition (client_id_, node_id_, sim_handle_parent, currPos,
                        simx_opmode_oneshot_wait);
  position_.x = currPos[0];
  position_.y = currPos[1];
  position_.z = currPos[2];

  return 0;
}
      
int
gams::platforms::VREP_UAV::analyze (void)
{
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
gams::platforms::VREP_UAV::land (void)
{
  if (airborne_)
  {
    airborne_ = false;

    // TODO: vrep land
  }

  return 0;
}

#endif
