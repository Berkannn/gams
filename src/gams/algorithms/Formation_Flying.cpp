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

/**
 * @file Formation_Flying.h
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Declaration of Formation_Flying class
 **/

#include "Formation_Flying.h"
#include <cmath>
#include <string>
using std::string;

gams::algorithms::Formation_Flying::Formation_Flying (
  const Madara::Knowledge_Record & head_id,
  const Madara::Knowledge_Record & offset,
  const Madara::Knowledge_Record & destination,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base (knowledge, platform, sensors, self), need_to_move_ (false),
    in_formation_ (false)
{
  status_.init_vars (*knowledge, "formation");

  // get head information
  head_ = (head_id.to_integer () == self->id.to_integer ());

  // set madara containers
  char name[50]; // largish magic number
  sprintf (name, "formation.%d", head_id.to_integer ());
  in_formation_.set_name (string(name), *knowledge);
  sprintf (name, "device.%d.location", head_id.to_integer ());
  head_location_.set_name (string(name), *knowledge, 3);

  // parse offset
  if (head_)
    num_agents_ = offset.to_integer ();
  else
    sscanf (offset.to_string ().c_str (), "%lf,%lf,%lf", &rho_, &phi_, &z_);

  // parse destination
  sscanf (destination.to_string (). c_str(), "%lf,%lf",
    &destination_.x, &destination_.y);
  destination_.z = 1.5;
}

gams::algorithms::Formation_Flying::~Formation_Flying ()
{
}

void
gams::algorithms::Formation_Flying::operator= (
  const Formation_Flying & rhs)
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
gams::algorithms::Formation_Flying::analyze (void)
{
  //this->platform_->get_sensors (sensor_names_);

  //platform_->get_position (current_position_);

  //current_position_.to_container (self_->device.location);

  return 0;
}

int
gams::algorithms::Formation_Flying::execute (void)
{
  if (need_to_move_)
    platform_->move (next_position_);
  return 0;
}

int
gams::algorithms::Formation_Flying::plan (void)
{
  need_to_move_ = false;
  if (head_)
  {
    if (in_formation_ == 0)
    {
      // analyze for members in formation
    }
    else
    {
      // move towards destination
    }
  }
  else
  {
    if (in_formation_ == 0)
    {
      // get into position
      next_position_.x = head_location_[0] + rho_ * sin (phi_); // latitude
      next_position_.y = head_location_[1] + rho_ * cos (phi_); // longitude
      next_position_.z = head_location_[2] + z_;
      need_to_move_ = true;
    }
    else
    {
      // move towards destination
    }
  }
  return 0;
}
