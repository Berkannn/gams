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
using std::stringstream;
#include <set>
using std::set;

gams::algorithms::Formation_Flying::Formation_Flying (
  const Madara::Knowledge_Record & head_id,
  const Madara::Knowledge_Record & offset,
  const Madara::Knowledge_Record & destination,
  const Madara::Knowledge_Record & members,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base (knowledge, platform, sensors, self), need_to_move_ (false),
    phi_dir_(DBL_MAX)
{
  status_.init_vars (*knowledge, "formation");

  // get head information
  head_id_ = head_id.to_integer ();
  head_ = (head_id_ == self->id.to_integer ());

  // set madara containers
  stringstream in_formation_str;
  in_formation_str << "formation." << head_id.to_integer ();
  in_formation_str << "." << self->id.to_integer () << ".ready";
  in_formation_.set_name (in_formation_str.str (), *knowledge);

  stringstream head_location_str;
  head_location_str << "device." << head_id.to_integer () << ".location";
  head_location_.set_name (head_location_str.str (), *knowledge);

  // parse offset
  if (!head_)
    sscanf (offset.to_string ().c_str (), "%lf,%lf,%lf", &rho_, &phi_, &z_);

  // parse destination
  sscanf (destination.to_string (). c_str(), "%lf,%lf,%lf",
    &destination_.x, &destination_.y, &destination_.z);

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
    if (in_formation_ == 0)
      in_formation_ = knowledge_->evaluate (compiled_formation_).to_integer ();
  }
  else // follower
  {
    if (in_formation_ == 0)
    {
      // calculate offset
      utility::Position start;
      get_head_coords (start.x, start.y, start.z);
      double temp;
      start.direction_to (destination_, phi_dir_, temp);
 
      utility::Position location;
      location.from_container (self_->device.location);

      if (location.approximately_equal (next_position_,
        platform_->get_position_accuracy ()))
      {
        in_formation_ = 1;
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
  need_to_move_ = false;
  if (head_)
  {
    if (in_formation_ == 1)
    {
      next_position_ = destination_;
      need_to_move_ = true;
    }
  }
  else
  {
    // get head location
    // TODO: use madara containers
    double x, y, z;
    get_head_coords(x, y, z);

    // get into position
    double angle = phi_ + phi_dir_;
    next_position_.x = x + rho_ * cos (angle); // latitude
    next_position_.y = y + rho_ * sin (angle); // longitude
    next_position_.z = z + z_;
//    next_position_.x = head_location_[0] + rho_ * sin (phi_); // latitude
//    next_position_.y = head_location_[1] + rho_ * cos (phi_); // longitude
//    next_position_.z = head_location_[2] + z_;
    need_to_move_ = true;
  }
  return 0;
}

void
gams::algorithms::Formation_Flying::get_head_coords (double& x, double& y,
  double& z)
{
  stringstream loc;
  loc << "device." << head_id_ << ".location";
  string location = knowledge_->get (loc.str ()).to_string ();
  sscanf (location.c_str (), "%lf, %lf, %lf", &x, &y, &z);
}
