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

/**
 * @file VREP_Boat_EXT.cpp
 * @author Cormac O'Meadhra <cormac.omeadhra@gmail.com>
 *
 * This file contains the definition of the VREP_Boat_EXT simulator class
 */

#ifdef _GAMS_VREP_ // only compile this if we are simulating in VREP

#include "gams/platforms/vrep/VREP_Boat_EXT.h"

#include <iostream>
#include <cmath>
#include <cstring>

#include "madara/knowledge_engine/containers/Double_Vector.h"
#include "gams/utility/Angular_Velocity.h"
#include "gams/utility/Orientation.h"

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

using std::endl;
using std::cout;
using std::string;

const std::string gams::platforms::VREP_Boat_EXT::DEFAULT_BOAT_EXT_MODEL (
  (getenv ("GAMS_ROOT") == 0) ? 
  "" : // if GAMS_ROOT is not defined, then just leave this as empty string
  (string (getenv ("GAMS_ROOT")) + "/resources/vrep/boat_ext.ttm")
  );

gams::platforms::Base_Platform *
gams::platforms::VREP_Boat_EXT_Factory::create (
        const Madara::Knowledge_Vector & args,
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MINOR,
    "entering gams::platforms::VREP_Boat_EXT_Factory::create\n");

  Base_Platform * result (0);

  if (knowledge && sensors && platforms && self)
  {
    if (knowledge->get_num_transports () == 0)
    {
      Madara::Transport::QoS_Transport_Settings settings;

      settings.type = Madara::Transport::MULTICAST;
      settings.hosts.push_back ("239.255.0.1:4150");

      knowledge_->attach_transport ("", settings);
      knowledge_->activate_transport ();

      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_MINOR,
         "gams::platforms::VREP_Boat_EXT_Factory::create:" \
        " no transports found, attaching multicast\n");
    }

    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
       "gams::platforms::VREP_Boat_EXT_Factory::create:" \
      " creating VREP_Boat_EXT object\n");

    // specify the model file
    string model_file;
    simxUChar is_client_side; // file is on server
    if (args.size () >= 1)
    {
      model_file = args[0].to_string ();
      is_client_side = 1;
    }
    else
    {
      model_file = VREP_Boat_EXT::DEFAULT_BOAT_EXT_MODEL;
      is_client_side = 0;
    }

    result = new VREP_Boat_EXT (model_file, is_client_side, knowledge, sensors, 
      platforms, self);
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
       "gams::platforms::VREP_Boat_EXT_Factory::create:" \
      " invalid knowledge, sensors, platforms, or self\n");
  }

  if (result == 0)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
       "gams::platforms::VREP_Boat_EXT_Factory::create:" \
      " error creating VREP_Boat_EXT object\n");
  }

  return result;
}

gams::platforms::VREP_Boat_EXT::VREP_Boat_EXT (
  std::string model_file, 
  simxUChar is_client_side, 
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self) :
  VREP_Base (knowledge, sensors, self), threader_ (*knowledge), 
  updater_thread_ (0)
{
  if (platforms && knowledge)
  {
    (*platforms)[get_id ()].init_vars (*knowledge, get_id ());
    status_ = (*platforms)[get_id ()];
  }

  self_->device.desired_altitude = 0.05;
  add_model_to_environment (model_file, is_client_side);
  set_initial_position ();
  get_target_handle ();
  wait_for_go ();

  //run thread at 100Hz
  updater_thread_ = new Motor_Updater (client_id_, node_id_);
  threader_.run (100.0, "Motor updater", updater_thread_);
}

gams::platforms::VREP_Boat_EXT::~VREP_Boat_EXT()
{
  threader_.terminate("Motor updater"); 
  delete updater_thread_;
}

int
gams::platforms::VREP_Boat_EXT::sense ()
{
  // get angular velocity
  simxFloat curr_arr[3];
  simxGetObjectVelocity (client_id_, node_id_,  NULL, curr_arr,
    simx_opmode_oneshot_wait);

  utility::Angular_Velocity vrep_angv;
  array_to_position (curr_arr, vrep_angv);

  // set angular velocity in madara
  vrep_angv.to_container (self_->device.angular_velocity);

  //Get orientation
  simxGetObjectOrientation(client_id_, node_id_, sim_handle_parent, curr_arr,
    simx_opmode_oneshot_wait);

  utility::Orientation orientation;
  array_to_position (curr_arr, orientation);

  // set angular velocity in madara
  orientation.to_container (self_->device.orientation);
  return 0;
}

void
gams::platforms::VREP_Boat_EXT::add_model_to_environment (
  const std::string& file, const simxUChar client_side)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_ERROR,
    "gams::platforms::VREP_Boat_EXT::add_model_to_environment(" \
    "%s, %u)\n", file.c_str (), client_side);
  if (simxLoadModel (client_id_, file.c_str (), client_side, &node_id_,
    simx_opmode_oneshot_wait) != simx_error_noerror)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
       "gams::platforms::VREP_Boat_EXT::add_model_to_environment:" \
      " error loading model in vrep\n");
    exit (-1);
  }

  if (node_id_ < 0)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
       "gams::platforms::VREP_Boat_EXT::add_model_to_environment:" \
      " invalid handle id\n");
    exit (-1);
  }
}

std::string
gams::platforms::VREP_Boat_EXT::get_id () const
{
  return "vrep_boat_ext";
}

std::string
gams::platforms::VREP_Boat_EXT::get_name () const
{
  return "VREP Boat EXT";
}

void
gams::platforms::VREP_Boat_EXT::get_target_handle ()
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
gams::platforms::VREP_Boat_EXT::set_initial_position () const
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
  pos[2] = (simxFloat) 0.3;

  // send set object position command
  simxSetObjectPosition (client_id_, node_id_, sim_handle_parent, pos,
    simx_opmode_oneshot_wait);
}

int
gams::platforms::VREP_Boat_EXT::move (const utility::Position & /*position*/,
  const double & /*epsilon*/)
{
  return OK;
}

gams::platforms::VREP_Boat_EXT::Motor_Updater::Motor_Updater (simxInt c, 
  simxInt n) :
  lknowledge_(0), left_motor_speed_ (0), right_motor_speed_ (0), 
  client_id_ (c), node_id_ (n)
{
}

void
gams::platforms::VREP_Boat_EXT::Motor_Updater::init (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
  lknowledge_ = &knowledge;
  //Get boat name to create motor signals
  simxInt num_handles;
  simxInt * handles;
  simxInt num_names;
  simxChar * names;
  
  //VREP remote API has no function to directly retrieve an object
  //name. Instead all object names are retrieved and the correct
  //name is matached with the handle of the correct object 
  simxGetObjectGroupData(client_id_, sim_appobj_object_type, 0, 
    &num_handles, &handles, 
    NULL, NULL, 
    NULL, NULL, 
    &num_names, &names, 
    simx_opmode_oneshot_wait);

  //Compare each handle to the stored handle
  simxChar* boat_name_cstr;
  for(int i = 0; i < num_handles; i++)
  {
    boat_name_cstr = names;
    names += strlen (boat_name_cstr) + 1;
    if(handles[i] == node_id_)
      break;  
  }

  //Extract boat number from name
  //Boats are numbered as follows: no_number, #0, #1, ... 
  std::string boat_name (boat_name_cstr);
  size_t pos = boat_name.find("#");
  int id;
  if(pos == std::string::npos)
    id = 0;
  else
    id = atoi( boat_name.substr(pos+1).c_str() ) + 1;

  //Generate signal names
  std::stringstream lss;
  lss << "left_motor_" << id;
  left_motor_sig_ = lss.str();

  std::stringstream rss;
  rss << "right_motor_" << id;
  right_motor_sig_ = rss.str();
}

void
gams::platforms::VREP_Boat_EXT::Motor_Updater::run()
{
  //Query knowledge base for motor speeds
  left_motor_speed_ = lknowledge_->get (left_motor_sig_).to_double ();
  right_motor_speed_ = lknowledge_->get (right_motor_sig_).to_double ();
            
  //Pass motor speeds to vrep
  simxSetFloatSignal(client_id_, left_motor_sig_.c_str(), left_motor_speed_, 
    simx_opmode_oneshot);
  simxSetFloatSignal(client_id_, right_motor_sig_.c_str(), right_motor_speed_, 
    simx_opmode_oneshot);
}

void
gams::platforms::VREP_Boat_EXT::Motor_Updater::cleanup()
{
}

#endif // _GAMS_VREP_
