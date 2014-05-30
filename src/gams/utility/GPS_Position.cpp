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

#include <cmath>
#include <sstream>
#include "gams/utility/GPS_Position.h"
#include "gams/utility/Position.h"

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

gams::utility::GPS_Position::GPS_Position (
  double init_lat, double init_lon, double init_alt) :
  lat (init_lat), lon (init_lon), alt (init_alt)
{
}

gams::utility::GPS_Position::~GPS_Position ()
{
}

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
  cout << "GPS_Position::approximately_equal (*this = " << to_string () <<
    ", rhs = " << rhs.to_string () << ", epsilon = " << epsilon << ")" << endl;
  return this->distance_to(rhs) <= epsilon;
}

void
gams::utility::GPS_Position::direction_to (const GPS_Position& rhs, 
  double& phi) const
{
  // Convert to cartesian and use those
  Position p1 = this->to_position (*this);
  Position p2 = rhs.to_position (*this);
  double temp;
  p1.direction_to (p2, phi, temp);
}

double
gams::utility::GPS_Position::distance_to (const GPS_Position & rhs) const
{
  // assume the Earth is a perfect sphere
  const double EARTH_RADIUS = 6371000.0;
  const double EARTH_CIRCUMFERENCE = 2 * EARTH_RADIUS * M_PI;

  // convert the latitude/x coordinates
  // VREP uses y for latitude
  double delta_lat = (rhs.lat - this->lat) / 360.0 * EARTH_CIRCUMFERENCE;
  
  // assume the meters/degree longitude is constant throughout environment
  // convert the longitude/y coordinates
  // VREP uses x for longitude
  double r_prime = EARTH_RADIUS * cos (DEG_TO_RAD (rhs.lat));
  double circumference = 2 * r_prime * M_PI;
  double delta_lon = (rhs.lon - this->lon) / 360.0 * circumference;

  double sum = pow (delta_lat, 2.0) + pow (delta_lon, 2.0);
  double dist = pow (sum, 0.5);

  cout << "GPS_Position::distance_to (*this = " << to_string () << ", rhs = " <<
    rhs.to_string () << ") = " << dist << endl;

  return dist;
}

bool
gams::utility::GPS_Position::slope_2d (const GPS_Position & p, double & slope)
  const
{
  const Position p1 (this->lat, this->lon, this->alt);
  const Position p2 (p.lat, p.lon, p.alt);
  return p1.slope_2d (p2, slope);
}

bool
gams::utility::GPS_Position::is_between_2d (const GPS_Position & end,
  const GPS_Position & check) const
{
  const Position p (this->lat, this->lon, this->alt);
  const Position e (end.lat, end.lon, end.alt);
  const Position c (check.lat, check.lon, check.alt);
  return p.is_between_2d (e, c);
}

std::string
gams::utility::GPS_Position::to_string (const std::string & delimiter) const
{
  std::stringstream buffer;
  buffer << std::setprecision(17);
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
