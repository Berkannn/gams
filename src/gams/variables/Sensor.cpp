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
#include "Sensor.h"

#include <float.h>
#include <sstream>
using std::stringstream;
#include <iostream>
using std::cout;

typedef  Madara::Knowledge_Record::Integer  Integer;


gams::variables::Sensor::Sensor (const string& name,
  Madara::Knowledge_Engine::Knowledge_Base* knowledge,
  const double& range, const utility::GPS_Position& origin) :
  knowledge_ (knowledge), name_ (name)
{
  init_vars ();

  if (range != -1.0)
    range_ = range;
  if (origin.lat != DBL_MAX)
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
    this->covered_ = rhs.covered_;
    this->origin_ = rhs.origin_;
    this->knowledge_ = rhs.knowledge_;
    this->name_ = rhs.name_;
  }
}

void
gams::variables::Sensor::set_range (const double& range)
{
  range_ = range;
}

void
gams::variables::Sensor::set_origin (const utility::GPS_Position& origin)
{
  origin.to_container (origin_);
}

gams::utility::GPS_Position
gams::variables::Sensor::get_origin ()
{
  utility::GPS_Position origin;
  origin.from_container (origin_);
  return origin;
}

double
gams::variables::Sensor::get_value (const utility::GPS_Position& pos)
{
  return get_value (get_index_from_gps (pos));
}

double
gams::variables::Sensor::get_value (const utility::Position& pos)
{
  return covered_[index_pos_to_index (pos)].to_double ();
}

void
gams::variables::Sensor::set_value (const utility::GPS_Position& pos,
  const double& val, const bool bcast)
{
  set_value (get_index_from_gps (pos), val, bcast);
}

void
gams::variables::Sensor::set_value (const utility::Position& pos, const double& val,
  const bool bcast)
{
  string idx = index_pos_to_index (pos);
  covered_.set (idx, val,
    Madara::Knowledge_Engine::Knowledge_Update_Settings (bcast));
}

gams::utility::Position
gams::variables::Sensor::get_index_from_gps (const utility::GPS_Position& pos)
{
  utility::GPS_Position origin;
  origin.from_container (origin_);
  utility::Position idx = pos.to_position (origin);
  double range = range_.to_double ();
  idx.x = (int)((idx.x + range / 2) / range);
  idx.y = (int)((idx.y + range / 2) / range);

  return idx;
}

gams::utility::GPS_Position
gams::variables::Sensor::get_gps_from_index (const utility::Position& idx)
{
  double range = range_.to_double ();
  utility::Position meters (int(idx.x) * range, int(idx.y) * range, int(idx.z));
  utility::GPS_Position origin;
  origin.from_container (origin_);
  utility::GPS_Position ret = meters.to_gps_position (origin);
  return ret;
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
  covered_.set_name (prefix + ".covered", *knowledge_);
  origin_.set_name (prefix + ".origin", *knowledge_, 3);
}

string
gams::variables::Sensor::index_pos_to_index (const utility::Position& pos) const
{
  stringstream buffer;
  buffer << (int)(pos.x) << "x" << (int)(pos.y);

  return buffer.str ();
}
