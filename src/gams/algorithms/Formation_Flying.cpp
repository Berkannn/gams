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

#include "gams/algorithms/Formation_Flying.h"

#include <cmath>
#include <string>
using std::string;
using std::stringstream;
#include <set>
using std::set;

#include "gams/utility/Position.h"

gams::algorithms::Formation_Flying::Formation_Flying (
  const Madara::Knowledge_Record & head_id,
  const Madara::Knowledge_Record & offset,
  const Madara::Knowledge_Record & destination,
  const Madara::Knowledge_Record & members,
  const Madara::Knowledge_Record & modifier,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base (knowledge, platform, sensors, self), modifier_ (NONE),
    need_to_move_ (false), phi_dir_(DBL_MAX)
{
  status_.init_vars (*knowledge, "formation");

  // get head information
  head_ = (head_id.to_integer () == self->id.to_integer ());

  // set madara containers
  stringstream in_formation_str;
  in_formation_str << "formation." << head_id.to_integer ();
  in_formation_str << "." << *(self->id) << ".ready";
  in_formation_.set_name (in_formation_str.str (), *knowledge);

  stringstream formation_ready_str;
  formation_ready_str << "formation." << head_id.to_integer ();
  formation_ready_str << ".flying";
  formation_ready_.set_name (formation_ready_str.str (), *knowledge);

  stringstream head_location_str;
  head_location_str << "device." << head_id.to_integer () << ".location";
  head_location_.set_name (head_location_str.str (), *knowledge, 3);

  // parse offset
  if (!head_)
    sscanf (offset.to_string ().c_str (), "%lf,%lf,%lf", &rho_, &phi_, &z_);

  // parse destination
  sscanf (destination.to_string (). c_str(), "%lf,%lf,%lf",
    &destination_.lat, &destination_.lon, &destination_.alt);

  // parse modifier
  string mod = modifier.to_string ();
  if (mod == "rotate")
  {
    modifier_ = ROTATE;
  }

  // construct wait for in formation string
  if (head_)
  {
    // parse members
    char* mem_string = new char[members.to_string ().length () + 1];
    const char* idx = mem_string;
    strcpy (mem_string, members.to_string ().c_str ());
    int num_members, member;
    sscanf (idx, "%d,%*s", &num_members);
    idx = strchr (idx, ',') + 1;
    set<int> members;
    for (int i = 0; i < num_members - 1; ++i)
    {
      sscanf (idx, "%d,%*s", &member);
      if (member != self->id.to_integer ())
        members.insert (member);
      idx = strchr (idx, ',') + 1;
    }
    sscanf (idx, "%d", &member);
    if (member != self->id.to_integer ())
      members.insert (member);
    delete [] mem_string;
  
    // construct actual string
    std::stringstream formation_expr;
    set<int>::iterator it = members.begin ();
    formation_expr << "formation." << self->id.to_integer ();
    formation_expr << "." << *it << ".ready ";
    ++it;
    for (; it != members.end (); ++it)
    {
      formation_expr << " && formation." << head_id.to_integer ();
      formation_expr << "." << *it << ".ready ";
    }
    compiled_formation_ = knowledge_->compile (formation_expr.str ());
  }

  /**
   * These values are used because they were found to produce simulations
   * that looked good in VREP. These will be parameterized when we perform real
   * world experiments, but for now will remain hardcoded.
   */
  // update speed if necessary
  if (modifier_ == ROTATE)
  {
    if (!head_)
      platform->set_move_speed (platform->get_move_speed () * 1.5);
    else // head_
      platform->set_move_speed (platform->get_move_speed () * 0.2);
  }
  else
  {
    if (head_)
      platform->set_move_speed (platform->get_move_speed () * 0.6);
  }
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
  // split logic by role
  if (head_)
  {
    // head considers itself in formation when everybody else gets in formation
    if (in_formation_ == 0)
      in_formation_ = knowledge_->evaluate (compiled_formation_).to_integer ();
    // everybody is in formation, so inform we are ready to move
    else if (formation_ready_ == 0)
      formation_ready_ = 1;
  }
  else // follower
  {
    if (in_formation_ == 0) // if not yet in formation...
    {
      if (phi_dir_ == DBL_MAX)
      {
        // calculate offset
        utility::GPS_Position start;
        start.from_container (head_location_);
        start.direction_to (destination_, phi_dir_);
      }

      utility::GPS_Position location;
      location.from_container (self_->device.location);

      // check if in formation
      if (location.approximately_equal (next_position_,
        platform_->get_gps_accuracy ()))
      {
        in_formation_ = 1; // inform in formation
      }
    }
  }
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
  // increment executions, only used by rotation formation for now
  ++executions_;

  need_to_move_ = false;
  if (head_)
  {
    // head only has to wait for everybody, and then move to destination
    if (formation_ready_ == 1)
    {
      next_position_ = destination_;
      need_to_move_ = true;
    }
  }
  else // !head_
  {
    switch (modifier_)
    {
      /**
       * Rotation formation keys off of head location at all times
       */
      case ROTATE:
      {
        const double OMEGA = M_PI / 30;
        double angle = -phi_ + phi_dir_ + executions_ * OMEGA;
        utility::Position offset (rho_ * sin (angle), rho_ * cos (angle), z_);

        utility::GPS_Position reference;
        reference.from_container (head_location_);
        next_position_ = offset.to_gps_position (reference);
        
        need_to_move_ = true;

        break;
      }

      /**
       * Default formation sticks with head, until close to destination
       */
      default: // case NONE
      {
        // calculate formation location
        double angle = -phi_ + phi_dir_;
        utility::GPS_Position ref_location;
        ref_location.from_container (head_location_);
        utility::Position offset (rho_ * sin (angle), rho_ * cos (angle), z_);

        // hold position until everybody is ready
        if (formation_ready_ == 0)
        {
          next_position_ = offset.to_gps_position (ref_location);
        }
        else // we are moving or already at destination
        {
          double dist = ref_location.distance_to (destination_);
          // TODO: tune the movement parameter
          if (dist > platform_->get_move_speed ())
          {
            // predict where the reference device will be
            dist = platform_->get_move_speed () * 1.5;
            utility::Position direction (dist * sin (phi_dir_), dist * cos (phi_dir_));
            utility::GPS_Position predicted = direction.to_gps_position (ref_location);
            next_position_ = offset.to_gps_position (predicted);
          }
          else // close enough, just go to final location
          {
            next_position_ = offset.to_gps_position (destination_);
          }
        }
        need_to_move_ = true;
      }
    }
  }
  return 0;
}
