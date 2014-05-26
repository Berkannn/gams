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
#include <sstream>
#include <string>
#include "Region.h"

gams::utility::Region::Region ()
{
}

gams::utility::Region::Region (const std::vector <Position> & init_points)
: points (init_points)
{
  calculate_bounding_box ();
}

gams::utility::Region::~Region ()
{
}

void
gams::utility::Region::operator= (const Region & rhs)
{
  if (this != &rhs)
  {
    this->points = rhs.points;
  }
}

bool
gams::utility::Region::is_in_region (const Position & p) const
{
  // check if in bounding box
  if (p.x < min_x_ || p.x > max_x_ ||
      p.y < min_y_ || p.y > max_y_ ||
      p.z < min_z_ || p.z > max_z_)
  {
    return false;
  }

  // check if point in polygon code from 
  // http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
  unsigned int i, j;
  bool ret = false;
  for (i = 0, j = points.size(); i < points.size(); j = i++)
  {
    if ( ((points[i].y > p.y) != (points[j].y > p.y)) &&
         (p.x < (points[j].x - points[i].x) * (p.y - points[i].y) / 
                (points[j].y - points[i].y) + 
                 points[i].x) )
    {
      ret = !ret;
    }
  }

  return ret;
}

gams::utility::Region
gams::utility::Region::get_bounding_box () const
{
  Region ret;

  Position p;
  p.x = min_x_; p.y = min_y_; p.z = 0;
  ret.points.push_back (p);
  p.x = min_x_; p.y = max_y_; p.z = 0;
  ret.points.push_back (p);
  p.x = max_x_; p.y = max_y_; p.z = 0;
  ret.points.push_back (p);
  p.x = max_x_; p.y = min_y_; p.z = 0;
  ret.points.push_back (p);

  ret.min_x_ = this->min_x_;
  ret.max_x_ = this->max_x_;
  ret.min_y_ = this->min_y_;
  ret.max_y_ = this->max_y_;
  ret.min_z_ = this->min_z_;
  ret.max_z_ = this->max_z_;

  return ret;
}

std::string
gams::utility::Region::to_string (const std::string & delimiter) const
{
  std::stringstream buffer;

  if (points.size () > 0)
  {
    buffer << points[0].to_string ();
    for (unsigned int i = 1; i < points.size (); ++i)
    {
      buffer << delimiter;
      buffer << points[i].to_string ();
    }
  }

  return buffer.str ();
}

void
gams::utility::Region::to_container (
  Madara::Knowledge_Engine::Containers::String_Array & target) const
{
  for (unsigned int i = 0; i < points.size (); ++i)
    target.set (i, points[i].to_string ());
}

void
gams::utility::Region::from_container (
  Madara::Knowledge_Engine::Containers::String_Array & target)
{
  points.resize (target.size ());
  for (unsigned int i = 0; i < target.size (); ++i)
    points[i] = Position::from_string (target[i]);
  calculate_bounding_box ();
}

void
gams::utility::Region::calculate_bounding_box ()
{
  min_x_ = min_y_ = min_z_ = DBL_MAX;
  max_x_ = max_y_ = max_z_ = -DBL_MAX;
  for (unsigned int i = 0; i < points.size(); ++i)
  {
    min_x_ = (min_x_ > points[i].x) ? points[i].x : min_x_;
    min_y_ = (min_y_ > points[i].y) ? points[i].y : min_y_;
    min_z_ = (min_z_ > points[i].z) ? points[i].z : min_z_;

    max_x_ = (max_x_ < points[i].x) ? points[i].x : max_x_;
    max_y_ = (max_y_ < points[i].y) ? points[i].y : max_y_;
    max_z_ = (max_z_ < points[i].z) ? points[i].z : max_z_;
  }
}
