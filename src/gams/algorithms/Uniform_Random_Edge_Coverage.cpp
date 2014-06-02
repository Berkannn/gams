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
#include "Uniform_Random_Edge_Coverage.h"

gams::algorithms::Uniform_Random_Edge_Coverage::Uniform_Random_Edge_Coverage (
  const Madara::Knowledge_Record& region_id,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self) :
  Base (knowledge, platform, sensors, self),
  region_ (utility::parse_region (*knowledge, region_id.to_integer ())),
  init_ (false)
{
  status_.init_vars (*knowledge, "urec");
}

gams::algorithms::Uniform_Random_Edge_Coverage::~Uniform_Random_Edge_Coverage ()
{
}

void
gams::algorithms::Uniform_Random_Edge_Coverage::operator= (
  const Uniform_Random_Edge_Coverage & rhs)
{
  if (this != &rhs)
  {
    this->platform_ = rhs.platform_;
    this->sensors_ = rhs.sensors_;
    this->self_ = rhs.self_;
    this->status_ = rhs.status_;
  }
}


int
gams::algorithms::Uniform_Random_Edge_Coverage::analyze (void)
{
  return 0;
}


int
gams::algorithms::Uniform_Random_Edge_Coverage::execute (void)
{
  platform_->move(next_position_);
  return 0;
}


int
gams::algorithms::Uniform_Random_Edge_Coverage::plan (void)
{
  // generate new next position if necessary
  utility::GPS_Position current;
  current.from_container (self_->device.location);
  if (!init_ || current.approximately_equal(next_position_,
    platform_->get_position_accuracy ()))
  {
    init_ = true;
    generate_new_position();
  }

  return 0;
}

void
gams::algorithms::Uniform_Random_Edge_Coverage::generate_new_position ()
{
  // select new edge
  int num_edges = region_.points.size();
  int target_edge = rand() % num_edges;

  // get endpoints
  const utility::GPS_Position & pos_1 = region_.points[target_edge];
  const utility::GPS_Position & pos_2 = 
    region_.points[(target_edge + 1) % num_edges];

  // get random point on line
  double delta_lat = pos_2.lat - pos_1.lat;
  double delta_lon = pos_2.lon - pos_1.lon;
  if (delta_lon == 0) // north/south line
  {
    const double & min = pos_1.lat < pos_2.lat ? pos_1.lat : pos_2.lat;
    const double & max = pos_1.lat > pos_2.lat ? pos_1.lat : pos_2.lat;
    next_position_.lat = Madara::Utility::rand_double(min, max);
    next_position_.lon = pos_1.lon;
  }
  else if (delta_lat == 0) // east/west line
  {
    const double & min = pos_1.lon < pos_2.lon ? pos_1.lon : pos_2.lon;
    const double & max = pos_1.lon > pos_2.lon ? pos_1.lon : pos_2.lon;
    next_position_.lon = Madara::Utility::rand_double(min, max);
    next_position_.lat = pos_1.lat;
  }
  else // other arbitrary line
  {
    const double slope = (pos_2.lon - pos_1.lon) / (pos_2.lat - pos_1.lat);
    next_position_.lat = Madara::Utility::rand_double(pos_1.lat, pos_2.lat);
    next_position_.lon = slope * (next_position_.lat - pos_1.lat) + pos_1.lon;
  }

  // fill in altitude on waypoint
  next_position_.alt = self_->device.location[2];

  cout << "next_position: " << next_position_.to_string () << endl;
}
