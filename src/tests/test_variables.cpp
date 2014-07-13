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
 * @file test_variables.cpp
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Tests the functionality of gams::variables classes
 **/

#include "gams/utility/Position.h"
using gams::utility::Position;
#include "gams/utility/GPS_Position.h"
using gams::utility::GPS_Position;
#include "gams/variables/Sensor.h"
using gams::variables::Sensor;

#include <string>
using std::string;
#include <iostream>
using std::cout;
using std::endl;
#include <assert.h>
#include <vector>
using std::vector;

void
testing_output (const string& str, const unsigned int& tabs = 0)
{
  for (unsigned int i = 0; i < tabs; ++i)
    cout << "\t";
  cout << "testing " << str << "..." << endl;
}

void
test_Sensor ()
{
  testing_output ("gams::variables::Sensor");

  // init knowledge base to use
  std::string host ("");
  const std::string default_multicast ("239.255.0.1:4150");
  Madara::Transport::QoS_Transport_Settings settings;
  settings.type = Madara::Transport::MULTICAST;
  if (settings.hosts.size () == 0)
  {
    settings.hosts.push_back (default_multicast);
  }
  Madara::Knowledge_Engine::Knowledge_Base test (host, settings);

  // test constructor
  testing_output ("constructor", 1);
  const string name ("coverage");
  const double range = 1.0;
  const GPS_Position origin (40, -80);
  Sensor s (name, &test, range, origin);
  assert (s.get_name () == name);
  assert (s.get_origin () == origin);
  assert (s.get_range () == range);

  // Test get/set value
  testing_output ("get/set", 1);
  double value = 1;
  Position origin_idx;
  s.set_value (origin, value);
  assert (s.get_value (origin) == value);
  assert (s.get_value (origin_idx) == value);
  GPS_Position g1 (origin);
  g1.latitude (40.01);
  value = 2;
  s.set_value (g1, value);
  Position p1 = s.get_index_from_gps (g1);
  GPS_Position g2 = s.get_gps_from_index (p1);
  assert (s.get_value (g1) == value);
  assert (s.get_value (p1) == value);
  assert (s.get_value (g2) == value);
}

int
main (int argc, char ** argv)
{
  test_Sensor ();
  return 0;
}
