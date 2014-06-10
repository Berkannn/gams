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
#include "gams/algorithms/Algorithm_Factory.h"
#include "gams/algorithms/Land.h"
#include "gams/algorithms/Move.h"
#include "gams/algorithms/Printer_Algorithm.h"
#include "gams/algorithms/Uniform_Random_Area_Coverage.h"
#include "gams/algorithms/Uniform_Random_Edge_Coverage.h"
#include "gams/algorithms/Priority_Weighted_Random_Area_Coverage.h"
#include "gams/algorithms/Local_Pheremone_Area_Coverage.h"
#include "gams/algorithms/Snake_Area_Coverage.h"
#include "gams/algorithms/Min_Time_Area_Coverage.h"
#include "gams/algorithms/Formation_Flying.h"
#include "gams/algorithms/Takeoff.h"

#include <iostream>
using std::cout;
using std::endl;

gams::algorithms::Factory::Factory (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  platforms::Base * platform,
  variables::Self * self,
  variables::Devices * devices)
: knowledge_ (knowledge), sensors_ (sensors),
  platform_ (platform), self_ (self),
  devices_ (devices)
{
}

gams::algorithms::Factory::~Factory ()
{
}

gams::algorithms::Base *
gams::algorithms::Factory::create (const std::string & type,
  const Madara::Knowledge_Record & arg1,
  const Madara::Knowledge_Record & arg2,
  const Madara::Knowledge_Record & arg3,
  const Madara::Knowledge_Record & arg4,
  const Madara::Knowledge_Record & arg5)
{
  gams::algorithms::Base * result = 0;

  if (type == "debug" || type == "print" || type == "printer")
  {
    if (knowledge_ && sensors_ && self_)
      result = new Printer_Algorithm (knowledge_, platform_, sensors_, self_);
  }
  else if (type == "uniform random area coverage" || type == "urac")
  {
    if (knowledge_ && sensors_ && self_)
      result = new Uniform_Random_Area_Coverage (
        arg1 /* region id */,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "uniform random edge coverage" || type == "urec")
  {
    if (knowledge_ && sensors_ && self_)
      result = new Uniform_Random_Edge_Coverage (
        arg1 /* region id*/,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "priority weighted random area coverage" || type == "pwrac")
  {
    if (knowledge_ && sensors_ && self_)
      result = new Priority_Weighted_Random_Area_Coverage (
        arg1 /* search area id*/,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "snake" || type == "sac")
  {
    if (knowledge_ && sensors_ && platform_ && self_)
      result = new Snake_Area_Coverage (
        arg1 /* region id*/,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "local pheremone")
  {
    if (knowledge_ && sensors_ && self_)
      result = new Local_Pheremone_Area_Coverage (
        arg1 /* search area id*/,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "min time")
  {
    if (knowledge_ && sensors_ && self_)
      result = new Min_Time_Area_Coverage (
        arg1 /* search area id*/,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "formation")
  {
    if (knowledge_ && sensors_ && platform_ && self_)
      result = new Formation_Flying (
        arg1 /* target */, arg2 /* offset */, arg3 /* destination */,
        arg4 /* members */, arg5 /* modifier */,
        knowledge_, platform_, sensors_, self_);
  }
  else if (type == "takeoff")
  {
    if (knowledge_ && sensors_ && platform_ && self_)
      result = new Takeoff (knowledge_, platform_, sensors_, self_);
  }
  else if (type == "land")
  {
    if (knowledge_ && sensors_ && platform_ && self_)
      result = new Land (knowledge_, platform_, sensors_, self_);
  }
  else if (type == "move")
  {
    if (knowledge_ && sensors_ && platform_ && self_ && arg1.is_string_type ())
    {
      // if an integer is passed as arg2, then it's the number of executions 
      if (arg2.is_integer_type ())
      {
        result = new Move (arg1.to_string (), arg2.to_integer (), -1,
          knowledge_, platform_, sensors_, self_);
      }
      else if (arg2.type () == Madara::Knowledge_Record::DOUBLE)
      {
        result = new Move (arg1.to_string (), 0, arg2.to_double (),
          knowledge_, platform_, sensors_, self_);
      }
      else if (arg2.type () == Madara::Knowledge_Record::UNCREATED)
      {
        utility::Position target;
        target.from_container (self_->device.dest);

        result = new Move (arg1.to_string (), target,
          knowledge_, platform_, sensors_, self_);
      }
    }
  }

  return result;
}

void
gams::algorithms::Factory::set_knowledge (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge)
{
  knowledge_ = knowledge;
}
      

void
gams::algorithms::Factory::set_sensors (variables::Sensors * sensors)
{
  sensors_ = sensors;
}
      

void
gams::algorithms::Factory::set_platform (platforms::Base * platform)
{
  platform_ = platform;
}
      

void
gams::algorithms::Factory::set_self (variables::Self * self)
{
  self_ = self;
}

void
gams::algorithms::Factory::set_devices (variables::Devices * devices)
{
  devices_ = devices;
}
