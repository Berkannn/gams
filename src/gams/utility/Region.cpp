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
 * @file Region.cpp
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a utility class for working with regions
 **/

#include <sstream>
using std::stringstream;
#include <string>
using std::string;
#include <vector>
using std::vector;
#include <cmath>

#include "Region.h"

gams::utility::Region::Region (const std::vector <GPS_Position> & init_points) :
  points (init_points)
{
  calculate_bounding_box ();
}

gams::utility::Region::~Region ()
{
}

void
gams::utility::Region::operator= (const Region& rhs)
{
  if (this != &rhs)
  {
    this->points = rhs.points;
  }
}

bool
gams::utility::Region::is_in_region (const GPS_Position & p) const
{
  // check if in bounding box
  if (p.lat < min_lat_ || p.lat > max_lat_ ||
      p.lon < min_lon_ || p.lon > max_lon_)
  {
    return false;
  }

  // check if point in polygon code from 
  // http://www.ecse.rpi.edu/Homepages/wrf/Research/Short_Notes/pnpoly.html
  unsigned int i, j;
  bool ret = false;
  for (i = 0, j = points.size() - 1; i < points.size(); j = i++)
  {
    if ( ((points[i].lon > p.lon) != (points[j].lon > p.lon)) &&
         (p.lat < (points[j].lat - points[i].lat) * (p.lon - points[i].lon) / 
                (points[j].lon - points[i].lon) + 
                 points[i].lat) )
    {
      ret = !ret;
    }
  }

  // check if this is a vertex point
  if (!ret)
  {
    for (unsigned int i = 0; i < points.size() && !ret; ++i)
      ret = (points[i] == p);
  }

  // TODO: add check for border point

  return ret;
}

double
gams::utility::Region::distance (const GPS_Position& p) const
{
  // if point is in region, then the distance is 0
  if (is_in_region (p))
    return 0;

  // convert to cartesian coords with equirectangular projection
  const GPS_Position sw (min_lat_, min_lon_);
  vector<Position> local_points;
  for (size_t i = 0; i < points.size (); ++i)
    local_points.push_back (points[i].to_position (sw));
  Position local_p = p.to_position (sw);

  // else we check for distance from each edge
  double min_dist = DBL_MAX;
  for (size_t i = 0; i < local_points.size (); ++i)
  {
    const size_t i_1 = (i + 1) % local_points.size();
    double dist = local_points[i].distance_to_2d (local_points[i_1], local_p);
    if (dist < min_dist)
      min_dist = dist;
  }

  return min_dist;
}

bool
gams::utility::Region::is_in_region (const Position & p,
  const GPS_Position& ref) const
{
  // convert to GPS_Position and then just use other is_in_region
  return is_in_region (p.to_gps_position (ref));
}

gams::utility::Region
gams::utility::Region::get_bounding_box () const
{
  Region ret;

  GPS_Position p;
  p.lat = min_lat_; p.lon = min_lon_; p.alt = 0;
  ret.points.push_back (p);
  p.lat = min_lat_; p.lon = max_lon_; p.alt = 0;
  ret.points.push_back (p);
  p.lat = max_lat_; p.lon = max_lon_; p.alt = 0;
  ret.points.push_back (p);
  p.lat = max_lat_; p.lon = min_lon_; p.alt = 0;
  ret.points.push_back (p);

  ret.min_lat_ = this->min_lat_;
  ret.max_lat_ = this->max_lat_;
  ret.min_lon_ = this->min_lon_;
  ret.max_lon_ = this->max_lon_;
  ret.min_alt_ = this->min_alt_;
  ret.max_alt_ = this->max_alt_;

  return ret;
}

double
gams::utility::Region::get_area () const
{
  if (points.size() < 3)
    return 0; // degenerate polygon

  // see http://geomalgorithms.com/a01-_area.html
  // Convert all units to cartesian
  vector<Position> cart_points;
  for (unsigned int i = 0; i < points.size(); ++i)
  {
    Position p = points[i].to_position (points[0]);
    cart_points.push_back (p);
  }

  // perform calculations with cartesian points
  double area = 0.0;
  unsigned int i, j, k;
  unsigned int num_points = cart_points.size ();
  for (i = 1, j = 2, k = 0; i < num_points; ++i, ++j, ++k)
  {
    area += cart_points[i].x *
      (cart_points[j % num_points].y - cart_points[k].y);
  }
  area += cart_points[0].x * (cart_points[1].y - cart_points[num_points - 1].y);
  return fabs(area / 2);
}

string
gams::utility::Region::to_string (const string & delimiter) const
{
  stringstream buffer;

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
    points[i] = GPS_Position::from_string (target[i]);
  calculate_bounding_box ();
}

void
gams::utility::Region::calculate_bounding_box ()
{
  min_lat_ = min_lon_ = min_alt_ = DBL_MAX;
  max_lat_ = max_lon_ = max_alt_ = -DBL_MAX;
  for (unsigned int i = 0; i < points.size(); ++i)
  {
    min_lat_ = (min_lat_ > points[i].lat) ? points[i].lat : min_lat_;
    min_lon_ = (min_lon_ > points[i].lon) ? points[i].lon : min_lon_;
    min_alt_ = (min_alt_ > points[i].alt) ? points[i].alt : min_alt_;

    max_lat_ = (max_lat_ < points[i].lat) ? points[i].lat : max_lat_;
    max_lon_ = (max_lon_ < points[i].lon) ? points[i].lon : max_lon_;
    max_alt_ = (max_alt_ < points[i].alt) ? points[i].alt : max_alt_;
  }
}

gams::utility::Region
gams::utility::parse_region (
  Madara::Knowledge_Engine::Knowledge_Base& knowledge,
  const string& search_id)
{
  // get region id
  int region_id;
  sscanf (search_id.c_str (), "region.%u", &region_id);

  // parse vertices
  std::vector<gams::utility::GPS_Position> vertices;
  char expression[50];
  sprintf (expression, "region.%u.type", region_id);
  int region_type = knowledge.get (expression).to_integer ();
  switch (region_type)
  {
    case 0: // arbitrary convex polygon
    {
      sprintf (expression, "region.%u.size", region_id);
      const unsigned int num_vertices = knowledge.get (expression).to_integer ();
      for (unsigned int i = 0; i < num_vertices; ++i) // get the vertices
      {
        sprintf (expression, "region.%u.%u", region_id, i);
        utility::GPS_Position pos;
        sscanf (knowledge.get (expression).to_string ().c_str (),
          "%lf,%lf,%lf", &pos.lat, &pos.lon, &pos.alt);
        vertices.push_back (pos);
      }
      break;
    }
    default:
      std::cerr << "invalid region type: " << region_type << endl;
  }

  return Region(vertices);
}
