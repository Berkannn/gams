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
 * 3. The names Carnegie Mellon University, "SEI and/or Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN AS-IS BASIS. CARNEGIE MELLON
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
 * @file Priority_Weighted_Random_Area_Coverage.cpp
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Prioritized Random Area Coverage prioritizes certain regions of a search area
 * based on specified priorities
 **/

#include "gams/algorithms/Priority_Weighted_Random_Area_Coverage.h"

#include <iostream>
using std::cout;
using std::endl;

gams::algorithms::Priority_Weighted_Random_Area_Coverage::
Priority_Weighted_Random_Area_Coverage (
  const Madara::Knowledge_Record& search_id,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform, variables::Sensors * sensors,
  variables::Self * self) :
  Base (knowledge, platform, sensors, self),
  search_area_ (utility::parse_search_area (*knowledge, search_id.to_string ())),
  total_priority_ (0.0)
{
  // calculate total priority
  const vector<utility::Prioritized_Region>& regions =
    search_area_.get_regions ();
  for (unsigned int i = 0; i < regions.size (); ++i)
  {
    total_priority_ += regions[i].get_area () * regions[i].priority;
    priority_total_by_region_.push_back (total_priority_);
  }

  // generate first position to move
  generate_new_position ();
}

void
gams::algorithms::Priority_Weighted_Random_Area_Coverage::operator= (
  const Priority_Weighted_Random_Area_Coverage & rhs)
{
  if (this != &rhs)
  {
    this->next_position_ = rhs.next_position_;
    this->search_area_ = rhs.search_area_;
    this->priority_total_by_region_ = rhs.priority_total_by_region_;
    this->total_priority_ = rhs.total_priority_;
  }
}

int
gams::algorithms::Priority_Weighted_Random_Area_Coverage::analyze ()
{
  return 0;
}

int
gams::algorithms::Priority_Weighted_Random_Area_Coverage::execute ()
{
  platform_->move(next_position_);
  return 0;
}

int
gams::algorithms::Priority_Weighted_Random_Area_Coverage::plan ()
{
  // find new target if necessary
  utility::GPS_Position current;
  current.from_container (self_->device.location);
  if (current.approximately_equal(next_position_,
    platform_->get_position_accuracy ()))
  {
    generate_new_position();
  }

  return 0;
}

void
gams::algorithms::Priority_Weighted_Random_Area_Coverage::
  generate_new_position ()
{
  // select region
  double selected_rand = Madara::Utility::rand_double (0.0, total_priority_);
  const utility::Prioritized_Region* selected_region = 0;
  for (unsigned int i = 0; i < search_area_.get_regions ().size (); ++i)
  {
    if (priority_total_by_region_[i] > selected_rand)
    {
      selected_region = &((search_area_.get_regions ())[i]);
      break;
    }
  }

  // select point in region
  do
  {
    next_position_.lat = Madara::Utility::rand_double (selected_region->min_lat_,
      selected_region->max_lat_);
    next_position_.lon = Madara::Utility::rand_double (selected_region->min_lon_,
      selected_region->max_lon_);
    next_position_.alt = Madara::Utility::rand_double (selected_region->min_alt_,
      selected_region->max_alt_);
  }
  while (!selected_region->is_in_region (next_position_));

  // found an acceptable position, so set it as next
  utility::GPS_Position current;
  current.from_container (self_->device.location);
  next_position_.alt = current.alt; // TODO: update when altitude is handled
}