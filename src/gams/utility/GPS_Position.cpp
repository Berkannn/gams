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

#define _USE_MATH_DEFINES
#include <cmath>
#include <sstream>
using std::stringstream;

#include "gams/utility/GPS_Position.h"
#include "gams/utility/Position.h"

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

void
gams::utility::GPS_Position::operator= (const GPS_Position & rhs)
{
  if (this != &rhs)
  {
    this->lat = rhs.lat;
    this->lon = rhs.lon;
    this->alt = rhs.alt;
  }
}

bool
gams::utility::GPS_Position::operator== (const GPS_Position & rhs) const
{
  return this->lat == rhs.lat && this->lon == rhs.lon && this->alt == rhs.alt;
}

bool
gams::utility::GPS_Position::operator== (
  const Madara::Knowledge_Engine::Containers::Double_Array & rhs) const
{
  return rhs.size () == 3 && 
    this->lat == rhs[0] && this->lon == rhs[1] && this->alt == rhs[2];
}

bool
gams::utility::GPS_Position::operator== (
  const Madara::Knowledge_Engine::Containers::Native_Double_Array & rhs) const
{
  return rhs.size () == 3 && 
    this->lat == rhs[0] && this->lon == rhs[1] && this->alt == rhs[2];
}

bool
gams::utility::GPS_Position::operator!= (const GPS_Position & rhs) const
{
  return !(*this == rhs);
}

bool
gams::utility::GPS_Position::operator!= (
  const Madara::Knowledge_Engine::Containers::Double_Array & rhs) const
{
  return !(*this == rhs);
}

bool
gams::utility::GPS_Position::operator!= (
  const Madara::Knowledge_Engine::Containers::Native_Double_Array & rhs) const
{
  return !(*this == rhs);
}

bool
gams::utility::GPS_Position::approximately_equal (const GPS_Position & rhs,
  const double & epsilon) const
{
  return this->distance_to(rhs) <= epsilon;
}

void
gams::utility::GPS_Position::direction_to (const GPS_Position& rhs, 
  double& phi) const
{
  /**
   * Use rhumb line
   * phi = lat, lambda = long
   * Taken from: http://www.movable-type.co.uk/scripts/latlong.html
   */
  double del_psi = log (
    tan (M_PI / 4 + DEG_TO_RAD (rhs.lat) / 2) /
    tan (M_PI / 4 + DEG_TO_RAD (this->lat) / 2));

  double del_lambda = this->lon - rhs.lon;
  if (fabs (del_lambda) > M_PI)
  {
    del_lambda =
      (del_lambda > 0) ? (-2 * M_PI - del_lambda) : (2 * M_PI + del_lambda);
  }

  phi = atan2 (del_lambda, del_psi); // theta from the website
}

double
gams::utility::GPS_Position::distance_to (const GPS_Position & rhs) const
{
  /**
   * We make the assumption that the curvature of the Earth is insignificant
   * over the distances we will be covering.
   */
  const double EARTH_RADIUS = 6371000.0;
  const double EARTH_CIRCUMFERENCE = 2 * EARTH_RADIUS * M_PI;

  // calculate north/south distance
  const double ns_dif = EARTH_CIRCUMFERENCE * (this->lat - rhs.lat) / 360.0;

  // calculate east/west distance
  const double r_prime = EARTH_RADIUS * cos (DEG_TO_RAD (this->lat));
  const double circumference = 2 * r_prime * M_PI;
  const double ew_dif = circumference * (this->lon - rhs.lon) / 360.0;

  // calculate altitude difference
  const double alt_dif = this->alt - rhs.alt;

  // use distance formula
  const double dist =
    sqrt (pow (ns_dif, 2.0) + pow (ew_dif, 2.0) + pow (alt_dif, 2.0));
  return dist;
}

std::string
gams::utility::GPS_Position::to_string (const std::string & delimiter,
  const unsigned int precision) const
{
  stringstream buffer;
  buffer << std::setprecision(precision);
  buffer << lat;
  buffer << delimiter;
  buffer << lon;
  buffer << delimiter;
  buffer << alt;

  return buffer.str ();
}

gams::utility::Position
gams::utility::GPS_Position::to_position (const GPS_Position& ref) const
{
  /**
   * Make the assumption that the Earth is a perfect sphere and that the 
   * curvature of the Earth over the two points is insignificant
   */
  Position ret;

  // assume the Earth is a perfect sphere
  const double EARTH_RADIUS = 6371000.0;
  const double EARTH_CIRCUMFERENCE = 2 * EARTH_RADIUS * M_PI;

  // convert the latitude/x coordinates
  // VREP uses y for latitude
  ret.x = (this->lat - ref.lat) / 360.0 * EARTH_CIRCUMFERENCE;
  
  // assume the meters/degree longitude is constant throughout environment
  // convert the longitude/y coordinates
  // VREP uses x for longitude
  double r_prime = EARTH_RADIUS * cos (DEG_TO_RAD (this->lat));
  double circumference = 2 * r_prime * M_PI;
  ret.y = (this->lon - ref.lon) / 360.0 * circumference;

  // keep same altitude
  ret.z = this->alt - ref.alt;

  return ret;
}

gams::utility::GPS_Position
gams::utility::GPS_Position::from_string (const std::string & s)
{
  GPS_Position temp;
  sscanf (s.c_str (), "%lf%*s%lf%*s%lf", &temp.lat, &temp.lon, &temp.alt);
  return temp;
}

void
gams::utility::GPS_Position::to_container (
  Madara::Knowledge_Engine::Containers::Double_Array & target) const
{
  target.set (0, lat);
  target.set (1, lon);
  target.set (2, alt);
}

void
gams::utility::GPS_Position::from_container (
  Madara::Knowledge_Engine::Containers::Double_Array & source)
{
  if (source.size () >= 3)
  {
    lat = source[0];
    lon = source[1];
    alt = source[2];
  }
}

void
gams::utility::GPS_Position::to_container (
  Madara::Knowledge_Engine::Containers::Native_Double_Array & target) const
{
  target.set (0, lat);
  target.set (1, lon);
  target.set (2, alt);
}

void
gams::utility::GPS_Position::from_container (
  Madara::Knowledge_Engine::Containers::Native_Double_Array & source)
{
  if (source.size () >= 3)
  {
    lat = source[0];
    lon = source[1];
    alt = source[2];
  }
}
