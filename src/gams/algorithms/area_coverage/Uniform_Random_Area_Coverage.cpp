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

#include "gams/algorithms/area_coverage/Uniform_Random_Area_Coverage.h"

using std::string;

gams::algorithms::Base_Algorithm *
gams::algorithms::area_coverage::Uniform_Random_Area_Coverage_Factory::create (
  const Madara::Knowledge_Vector & args,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base_Platform * platform,
  variables::Sensors * sensors,
  variables::Self * self,
  variables::Devices * devices)
{
  Base_Algorithm * result (0);
  
  if (knowledge && sensors && self && devices)
  {
    if (args.size () >= 1)
    {
      if (args[0].is_string_type ())
      {
        if (args.size () == 2)
        {
          if (args[1].is_double_type () || args[1].is_integer_type ())
          {
            result = new area_coverage::Uniform_Random_Area_Coverage (
              args[0].to_string () /* search area id*/,
              ACE_Time_Value (args[1].to_double ()) /* exec time */,
              knowledge, platform, sensors, self, devices);
          }
          else
          {
            madara_logger_ptr_log (gams::loggers::global_logger.get (),
              gams::loggers::LOG_ERROR,
               "gams::algorithms::Uniform_Random_Area_Coverage_Factory::create:" \
              " invalid second arg, expected double\n");
          }
        }
        else
        {
          result = new area_coverage::Uniform_Random_Area_Coverage (
            args[0].to_string () /* search area id*/,
            ACE_Time_Value (0.0) /* run forever */,
            knowledge, platform, sensors, self, devices);
        }
      }
      else
      {
        madara_logger_ptr_log (gams::loggers::global_logger.get (),
          gams::loggers::LOG_ERROR,
           "gams::algorithms::Uniform_Random_Area_Coverage_Factory::create:" \
          " invalid first arg, expected string\n");
      }
    }
    else
    {
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_ERROR,
         "gams::algorithms::Uniform_Random_Area_Coverage_Factory::create:" \
        " expected 1 or 2 args\n");
    }
  }
  else
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
       "gams::algorithms::Uniform_Random_Area_Coverage_Factory::create:" \
      " invalid knowledge, sensors, self, or devices parameters\n");
  }

  if (result == 0)
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_ERROR,
       "gams::algorithms::Uniform_Random_Area_Coverage_Factory::create:" \
      " unknown error creating algorithm\n");
  }

  return result;
}

gams::algorithms::area_coverage::Uniform_Random_Area_Coverage::
  Uniform_Random_Area_Coverage (
  const string& search_area_id,
  const ACE_Time_Value& e_time,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base_Platform * platform,
  variables::Sensors * sensors,
  variables::Self * self,
  variables::Devices * devices) :
  Base_Area_Coverage (knowledge, platform, sensors, self, devices, e_time)
{
  // init status vars
  status_.init_vars (*knowledge, "urac", self->id.to_integer ());
  status_.init_variable_values ();

  // get region to cover
  utility::Search_Area search;
  search.from_container (*knowledge, search_area_id);
  region_ = search.get_convex_hull ();

  // generate initial waypoint
  generate_new_position();
}

gams::algorithms::area_coverage::Uniform_Random_Area_Coverage::
  ~Uniform_Random_Area_Coverage ()
{
}

void
gams::algorithms::area_coverage::Uniform_Random_Area_Coverage::operator= (
  const Uniform_Random_Area_Coverage & rhs)
{
  if (this != &rhs)
  {
    this->Base_Area_Coverage::operator= (rhs);
    this->region_ = rhs.region_;
  }
}

/**
 * Random positions are generated in the bounding rectangle of the region and
 * checked if it is actually in the region. In theory this could never happen
 * when given any arbitrary region. The average selection time is 
 * area(bounding_box) / area(region). In practice, this finds a valid position
 * quickly enough to not affect the system.
 */
void
gams::algorithms::area_coverage::Uniform_Random_Area_Coverage::
  generate_new_position ()
{
  do
  {
    next_position_.latitude (Madara::Utility::rand_double (region_.min_lat_,
      region_.max_lat_));
    next_position_.longitude (Madara::Utility::rand_double (region_.min_lon_,
      region_.max_lon_));
    next_position_.altitude (Madara::Utility::rand_double (region_.min_alt_,
      region_.max_alt_));
  }
  while (!region_.contains (next_position_));

  // found an acceptable position, so set it as next
  utility::GPS_Position current;
  current.from_container (self_->device.location);
  next_position_.altitude (current.altitude ()); // TODO: update when altitude is handled
}
