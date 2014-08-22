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
 * 3. The names �Carnegie Mellon University,� "SEI� and/or �Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN �AS-IS� BASIS. CARNEGIE MELLON
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

#include "gams/variables/Sensor.h"

#include <float.h>
#include <sstream>
#include <vector>
#include <string>
#include <cmath>

using std::string;
using std::stringstream;
using std::vector;

typedef  Madara::Knowledge_Record::Integer  Integer;

gams::variables::Sensor::Sensor () :
  knowledge_ (0), name_ ("")
{
}

gams::variables::Sensor::Sensor (const string & name,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  const double & range, const utility::GPS_Position & origin) :
  knowledge_ (knowledge), name_ (name)
{
  init_vars ();

  if (range_ == 0.0 && range != 0.0)
    range_ = range;
  if (origin.latitude () != DBL_MAX)
    origin.to_container (origin_);
}

gams::variables::Sensor::~Sensor ()
{
}

void
gams::variables::Sensor::operator= (const Sensor & rhs)
{
  if (this != &rhs)
  {
    this->range_ = rhs.range_;
    this->value_ = rhs.value_;
    this->origin_ = rhs.origin_;
    this->knowledge_ = rhs.knowledge_;
    this->name_ = rhs.name_;
  }
}

set<gams::utility::Position>
gams::variables::Sensor::discretize (
  const utility::Region & region)
{
  set<utility::Position> ret_val;

  // find northern most point
  utility::GPS_Position northern = region.vertices[0];
  for (size_t i = 1; i < region.vertices.size (); ++i)
    if (northern.latitude () < region.vertices[i].latitude ())
      northern = region.vertices[i];
  const int max_x = get_index_from_gps (northern).x;

  // find southern most point
  utility::GPS_Position southern = region.vertices[0];
  for (size_t i = 1; i < region.vertices.size (); ++i)
    if (southern.latitude () > region.vertices[i].latitude ())
      southern = region.vertices[i];
  const int min_x = get_index_from_gps (southern).x;

  // find west most point 
  utility::GPS_Position start;
  start.longitude (DBL_MAX);
  for (size_t i = 0; i < region.vertices.size (); ++i)
    if (start.longitude () > region.vertices[i].longitude ())
      start = region.vertices[i];

  // find valid corresponding position
  utility::Position start_index = get_index_from_gps (start);
  if (!region.contains (get_gps_from_index (start_index)))
  {
    ++start_index.y; // go one east...
    utility::Position check = start_index;
    while ((!region.contains (get_gps_from_index (check))) &&
      check.x <= max_x)
    {
      ++check.x; // ... and start looking north for position in region
    }

    // if we still haven't found a good position...
    if (!region.contains (get_gps_from_index (check)))
    {
      // ...start looking south
      check = start_index;
      while ((!region.contains (get_gps_from_index (check))) &&
        check.x >= min_x)
      {
        --check.x;
      }
    }

    // by now we must have found a starting position, so update start_index
    start_index = check;
  }

  // find east most point
  utility::GPS_Position eastern = region.vertices[0];
  for (size_t i = 1; i < region.vertices.size (); ++i)
    if (eastern.longitude () < region.vertices[i].longitude ())
      eastern = region.vertices[i];
  const int max_y = get_index_from_gps (eastern).y;

  // move east each iteration
  while (start_index.y < max_y)
  {
    // check north
    for (utility::Position pos = start_index; pos.x <= max_x; ++pos.x)
      if (region.contains (get_gps_from_index (pos)))
        ret_val.insert (pos);
  
    // check south
    for (utility::Position pos = start_index; pos.x >= min_x; --pos.x)
      if (region.contains (get_gps_from_index (pos)))
        ret_val.insert (pos);

    ++start_index.y;
  }

  return ret_val;
}

set<gams::utility::Position>
gams::variables::Sensor::discretize (
  const utility::Search_Area & search)
{
  set<utility::Position> ret_val;
  const vector<utility::Prioritized_Region>& regions = search.get_regions ();
  for (size_t i = 0; i < regions.size (); ++i)
  {
    set<utility::Position> to_add = discretize (regions[i]);
    ret_val.insert (to_add.begin (), to_add.end ());
  }
  return ret_val;
}

double
gams::variables::Sensor::get_discretization () const
{
  return sqrt (2.0 * pow(get_range (), 2.0));
}

gams::utility::GPS_Position
gams::variables::Sensor::get_gps_from_index (
  const utility::Position & idx)
{
  const double discretize = get_discretization ();
  utility::Position meters (
    int(idx.x) * discretize, int(idx.y) * discretize, int(idx.z));
  utility::GPS_Position origin;
  origin.from_container (origin_);
  utility::GPS_Position ret =
    utility::GPS_Position::to_gps_position (meters, origin);
  return ret;
}

gams::utility::Position
gams::variables::Sensor::get_index_from_gps (
  const utility::GPS_Position & pos)
{
  utility::GPS_Position origin;
  origin.from_container (origin_);
  utility::Position idx = pos.to_position (origin);
  const double discretize = get_discretization ();
  idx.x = (int)((idx.x + discretize / 2) / discretize);
  idx.y = (int)((idx.y + discretize / 2) / discretize);

  return idx;
}

string
gams::variables::Sensor::get_name () const
{
  return name_;
}

gams::utility::GPS_Position
gams::variables::Sensor::get_origin ()
{
  utility::GPS_Position origin;
  origin.from_container (origin_);
  return origin;
}

double
gams::variables::Sensor::get_range () const
{
  return range_.to_double ();
}

double
gams::variables::Sensor::get_value (const utility::GPS_Position & pos)
{
  return get_value (get_index_from_gps (pos));
}

double
gams::variables::Sensor::get_value (const utility::Position & pos)
{
  return value_[index_pos_to_index (pos)].to_double ();
}

void
gams::variables::Sensor::set_origin (const utility::GPS_Position & origin)
{
  origin.to_container (origin_);
}

void
gams::variables::Sensor::set_range (const double & range)
{
  range_ = range;
}

void
gams::variables::Sensor::set_value (const utility::GPS_Position & pos,
  const double & val,
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  set_value (get_index_from_gps (pos), val, settings);
}

void
gams::variables::Sensor::set_value (const utility::Position & pos,
  const double & val,
  const Madara::Knowledge_Engine::Knowledge_Update_Settings & settings)
{
  string idx = index_pos_to_index (pos);
  value_.set (idx, val, settings);
}

string
gams::variables::Sensor::index_pos_to_index (
  const utility::Position & pos) const
{
  stringstream buffer;
  buffer << (int)(pos.x) << "x" << (int)(pos.y);

  return buffer.str ();
}

void
gams::variables::Sensor::init_vars ()
{
  // sensor information is prefixed by sensor.<name_>
  std::string prefix ("sensor");
  prefix += ".";
  prefix += name_;

  // initialize the variable containers
  range_.set_name (prefix + ".range", *knowledge_);
  value_.set_name (prefix + ".covered", *knowledge_);
  origin_.set_name (prefix + ".origin", *knowledge_, 3);
}

void
gams::variables::Sensor::init_vars (const string & name,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  const double & range, const utility::GPS_Position & origin)
{
  name_ = name;
  knowledge_ = knowledge;

  /**
   * We only want to update range if it has not yet been set. This could result 
   * in inconsistencies if multiple agents try to set different ranges. In the 
   * best case, there is a round of sensor values that are inconsistent. 
   * Depending on the actual data that is being collected, this might not be an
   * issue.
   */
  if (range_ != 0.0)
    range_ = range;

  /**
   * Similar to range, origin could be set incorrectly by multiple agents. The
   * best option would be to have the controller set it when initializing the
   * system.
   **/
  utility::GPS_Position cur_origin;
  cur_origin.from_container (origin_);
  if (cur_origin.latitude () != 0.0 && origin.latitude () != DBL_MAX)
    origin.to_container (origin_);

  init_vars ();
}
