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
#include "Random_Edge_Coverage.h"


gams::algorithms::Random_Edge_Coverage::Random_Edge_Coverage (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base (knowledge, platform, sensors, self)
{
  status_.init_vars (*knowledge, "rac");

  // get bounding box from madara
  northwest_corner_.x = knowledge->get (".rac.bounding.nw.lat").to_double ();
  northwest_corner_.y = knowledge->get (".rac.bounding.nw.long").to_double ();
  northwest_corner_.z = 0;
  southeast_corner_.x = knowledge->get (".rac.bounding.se.lat").to_double ();
  southeast_corner_.y = knowledge->get (".rac.bounding.se.long").to_double ();
  southeast_corner_.z = 0;

  // generate random position
  srand (time (NULL));
  target_side_ = (Side) (rand () % 4);
  generate_new_position ();
}

gams::algorithms::Random_Edge_Coverage::~Random_Edge_Coverage ()
{
}

void
gams::algorithms::Random_Edge_Coverage::operator= (const Random_Edge_Coverage & rhs)
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
gams::algorithms::Random_Edge_Coverage::analyze (void)
{
  this->platform_->get_sensors (sensor_names_);

  platform_->get_position (current_position_);

  current_position_.to_container (self_->device.location);

  return 0;
}


int
gams::algorithms::Random_Edge_Coverage::execute (void)
{
  platform_->move(next_position_);
  return 0;
}


int
gams::algorithms::Random_Edge_Coverage::plan (void)
{
  // generate new next position if necessary
  if(current_position_.approximately_equal(next_position_, 0.25))
  {
    target_side_ = (Side) ((target_side_ + (rand () % 3)) % 4);
    generate_new_position ();
  }
  else

  return 0;
}

void
gams::algorithms::Random_Edge_Coverage::generate_new_position ()
{
  if(target_side_ == EAST || target_side_ == WEST)
  {
    double max = northwest_corner_.x, min = southeast_corner_.x;
    next_position_.x = min + (max - min) * ((double) rand ()) / RAND_MAX;
  }
  else
  {
    if(target_side_ == NORTH)
      next_position_.x = northwest_corner_.x;
    else // target_side_ == SOUTH
      next_position_.x = southeast_corner_.x;
  }

  // generate longitude
  utility::Position ret;
  if(target_side_ == NORTH || target_side_ == SOUTH)
  {
    double min = northwest_corner_.y, max = southeast_corner_.y;
    next_position_.y = min + (max - min) * ((double) rand ()) / RAND_MAX;
  }
  else
  {
    if(target_side_ == EAST)
      next_position_.y = southeast_corner_.y;
    else // target_side_ == WEST
      next_position_.y = northwest_corner_.y;
  }

  next_position_.z = 0.5;
}
