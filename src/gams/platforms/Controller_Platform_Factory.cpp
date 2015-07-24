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

#include "madara/utility/Utility.h"

#include "Controller_Platform_Factory.h"
#include "Debug_Platform.h"
#include "Null_Platform.h"

#ifdef _GAMS_DRONERK_
#include "dronerk/Drone_RK.h"
#endif

#ifdef _GAMS_VREP_
#include "gams/platforms/vrep/VREP_UAV.h"
#include "gams/platforms/vrep/VREP_Ant.h"
#include "gams/platforms/vrep/VREP_Summit.h"
#include "gams/platforms/vrep/VREP_Boat.h"
#endif

#ifdef _GAMS_ROS_
#include "gams/platforms/ros/ROS_P3DX.h"
#endif 

#include "gams/loggers/Global_Logger.h"

#include <string>

namespace platforms = gams::platforms;
namespace variables = gams::variables;


platforms::Controller_Platform_Factory::Controller_Platform_Factory (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
: knowledge_ (knowledge), platforms_ (platforms), self_ (self),
  sensors_ (sensors)
{
  initialize_default_mappings ();
}

platforms::Controller_Platform_Factory::~Controller_Platform_Factory ()
{
}

void
platforms::Controller_Platform_Factory::initialize_default_mappings (void)
{
  std::vector <std::string> aliases;
  
  // the debug platform
  aliases.resize (3);
  aliases[0] = "debug";
  aliases[1] = "print";
  aliases[2] = "printer";

  add (aliases, new Debug_Platform_Factory ());
  
  // the null platform
  aliases.resize (1);
  aliases[0] = "null";

  add (aliases, new Null_Platform_Factory ());
  
  // VREP Platforms
#ifdef _GAMS_VREP_
  
  // the VREP Ant platform
  aliases.resize (2);
  aliases[0] = "vrep-ant";
  aliases[1] = "vrep_ant";

  add (aliases, new VREP_Ant_Factory ());
  
  // the VREP Ant platform
  aliases.resize (2);
  aliases[0] = "vrep-uav";
  aliases[1] = "vrep_uav";

  add (aliases, new VREP_UAV_Factory ());

  // the VREP Summit platform
  aliases.resize (2);
  aliases[0] = "vrep-summit";
  aliases[1] = "vrep_summit";

  add (aliases, new VREP_Summit_Factory ());
  
 // the VREP Boat platform
  aliases.resize (2);
  aliases[0] = "vrep-boat";
  aliases[1] = "vrep_boat";

  add (aliases, new VREP_Boat_Factory ());
#endif

  // ROS Platforms
#ifdef _GAMS_ROS_
  // the ROS P3DX platform
  aliases.resize (2);
  aliases[0] = "ros_p3dx";
  aliases[1] = "ros-p3dx";

  add (aliases, new ROS_P3DX_Factory ());
#endif
}

void
platforms::Controller_Platform_Factory::add (
  const std::vector <std::string> & aliases,
  Platform_Factory * factory)
{
  for (size_t i = 0; i < aliases.size (); ++i)
  {
    std::string alias (aliases[i]);
    Madara::Utility::lower (alias);

    factory->set_knowledge (knowledge_);
    factory->set_self (self_);
    factory->set_sensors (sensors_);
    factory->set_platforms (platforms_);

    factory_map_[alias] = factory;
  }
}

platforms::Base_Platform *
platforms::Controller_Platform_Factory::create (
  const std::string & type,
  const Madara::Knowledge_Vector & args)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MINOR,
    "gams::platforms::Controller_Platform_Factory::create(" \
    "%s,...)\n", type.c_str ());

  Base_Platform * result (0);

  if (type != "")
  {
    Factory_Map::iterator it = factory_map_.find (type);
    if (it != factory_map_.end ())
    {
      result = it->second->create (args, knowledge_, sensors_, platforms_, 
        self_);
    }
    else
    {
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_ALWAYS,
        "gams::platforms::Controller_Platform_Factory::create:" \
        " could not find \"%s\" platform.\n", type.c_str ());
    }
  }

  return result;
}

void
platforms::Controller_Platform_Factory::set_knowledge (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge)
{
  knowledge_ = knowledge;
}

void
platforms::Controller_Platform_Factory::set_platforms (
  variables::Platforms * platforms)
{
  platforms_ = platforms;
}

void
platforms::Controller_Platform_Factory::set_self (variables::Self * self)
{
  self_ = self;
}

void
platforms::Controller_Platform_Factory::set_sensors (
  variables::Sensors * sensors)
{
  sensors_ = sensors;
}
