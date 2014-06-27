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

/**
 * @file test_utility.cpp
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Tests the functionality of gams::utility classes
 **/

#include "gams/utility/Position.h"
using gams::utility::Position;
#include "gams/utility/GPS_Position.h"
using gams::utility::GPS_Position;
#include "gams/utility/Region.h"
using gams::utility::Region;

#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
#include <assert.h>
#include <vector>
using std::vector;

#define _USING_MATH_DEFINES
#include <cmath>

void
testing_output (const string& str, const unsigned int& tabs = 0)
{
  for (unsigned int i = 0; i < tabs; ++i)
    cout << "\t";
  cout << "testing " << str << "..." << endl;
}

// TODO: fill out remaining Position function tests
void
test_Position ()
{
  testing_output ("gams::utility::Position");

  // test constructor
  testing_output ("constructor", 1);
  const Position p (1,2,3);
  assert (p.x == 1);
  assert (p.y == 2);
  assert (p.z == 3);

  // test assignment operator
  testing_output ("assignment operator", 1);
  Position p2 = p;
  assert (p2.x == 1);
  assert (p2.y == 2);
  assert (p2.z == 3);
  
  // equality operator
  testing_output ("equality operator", 1);
  assert (p == p2);

  // inequality operator
  testing_output ("inequality operator", 1);
  assert (!(p != p2));

  // approximately equal
  testing_output ("approximately_equal", 1);
  p2.z = 4;
  assert (!(p.approximately_equal (p2, 0.9)));
  assert (p.approximately_equal (p2, 1));
  p2.y = 3;
  assert (!(p.approximately_equal (p2, 1.4)));
  assert (p.approximately_equal (p2, 1.5));
  p2.x = 2;
  assert (!(p.approximately_equal (p2, 1.7)));
  assert (p.approximately_equal (p2, 1.8));

  // direction to
  testing_output ("direction_to", 1);
  double phi, theta;
  p2.z = p.z;
  p.direction_to (p2, phi, theta);
  assert (phi == M_PI / 4);
  assert (theta == M_PI / 2);

  // distance
  testing_output ("distance", 1);
  p2 = p;
  ++p2.z;
  ++p2.y;
  ++p2.x;
  assert (p.distance_to (p2) == pow (3.0, 0.5));

  // distance_2d
  testing_output ("distance_2d", 1);
  assert (p.distance_to_2d (p2) == pow (2.0, 0.5));

  // test to_string
  testing_output ("to_string", 1);
  assert (p.to_string () == "1,2,3");

  // test from_string
  testing_output ("to_string", 1);
  Position p3 = Position::from_string (p.to_string ());
  assert (p == p3);
}

// TODO: fill out remaining GPS_Position function tests
void
test_GPS_Position ()
{
  testing_output ("gams::utility::GPS_Position");

  // test constructor
  testing_output ("constructor", 1);
  GPS_Position p (40.442775, -79.940967);
  GPS_Position p2 (40.443412, -79.93954);
  assert (p.lat == 40.442775 && p.lon == -79.940967 && p.alt == 0.0);
  assert (p2.lat == 40.443412 && p2.lon == -79.93954 && p2.alt == 0.0);

  // test assignment operator
  testing_output ("assignment operator", 1);
  GPS_Position p3 = p;
  assert (p3.lat == p.lat && p3.lon == p.lon && p3.alt == p.alt);

  // test equality operator
  testing_output ("equality operator", 1);
  assert (p3 == p);
  assert (!(p2 == p));

  // test inequality operator
  testing_output ("inequality operator", 1);
  assert (!(p3 != p));
  assert (p2 != p);

  // test approximately equal
  testing_output ("approximately_equal", 1);
  assert (p3.approximately_equal (p, 0.1));
  p3.lat = 40.442776;
  assert (!p3.approximately_equal (p, 0.1));
  assert (p3.approximately_equal (p, 1.0));
}

// TODO: fill out remaining Region function tests
void
test_Region ()
{
  testing_output ("gams::utility::Region");

  // test constructor
  testing_output ("constructor using to_string", 1);
  vector<GPS_Position> points;
  GPS_Position p (40.443273, -79.939951);
  points.push_back (p);
  p.lat = 40.443116; p.lon = -79.939973;
  points.push_back (p);
  p.lat = 40.443085; p.lon = -79.940313;
  points.push_back (p);
  p.lat = 40.443285; p.lon = -79.940242;
  points.push_back (p);
  Region r (points);
  string expected = "40.443273,-79.939951,0:" \
    "40.443116,-79.939973,0:40.443085,-79.940313,0:" \
    "40.443285,-79.940242,0";
  assert (r.to_string () == expected);

  // test is_in_region
  testing_output ("is_in_region", 1);
  assert (r.is_in_region (p));
  GPS_Position p2 (40.4432, -79.9401);
  assert (r.is_in_region (p2));
  GPS_Position empty;
  assert (!r.is_in_region (empty));

  // bounding box
  testing_output ("get_bounding_box", 1);
  Region bound = r.get_bounding_box ();
  expected = "40.443085,-79.940313,0:" \
    "40.443085,-79.939951,0:40.443285,-79.939951,0:" \
    "40.443285,-79.940313,0";
  assert (r.get_bounding_box ().to_string () == expected);

  // get area
  testing_output ("get_area", 1);
  assert (r.get_area () < 525.8 && r.get_area() > 525.7);
  assert (bound.get_area () < 681.3 && bound.get_area() > 681.2);
}

int
main ()
{
  test_Position ();
  test_GPS_Position ();
  test_Region ();
  return 0;
}
