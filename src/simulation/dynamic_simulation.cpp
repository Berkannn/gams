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
 * @file dynamic_simulation.cpp
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Sets up a vrep simulation environment
 **/

#include "madara/knowledge_engine/Knowledge_Base.h"

extern "C" {
#include "extApi.h"
}

#include <iostream>
using std::cout;
using std::endl;
#include <cmath>
#include <string>
using std::string;

#define DEG_TO_RAD(x) ((x) * M_PI / 180.0)

// default transport settings
std::string host ("");
const std::string default_multicast ("239.255.0.1:4150");
Madara::Transport::QoS_Transport_Settings settings;

// create shortcuts to MADARA classes and namespaces
namespace engine = Madara::Knowledge_Engine;
typedef Madara::Knowledge_Record Record;
typedef Record::Integer Integer;

// controller variables
double period (1.0);
double sim_time (50.0);

// madara commands from a file
std::string madara_commands;

// number of agents to launch
int agents = 1;

// SW corner of simulation
double sw_lat = 40.442824;
double sw_long = -79.940967;

// NE corner of simulation
double ne_lat = 40.44355;
double ne_long = -79.939626;

// handle command line arguments
void handle_arguments (int argc, char ** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-a" || arg1 == "--num_agents")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> agents;
      }
      ++i;
    }
    else if (arg1 == "-m" || arg1 == "--madara_file")
    {
      madara_commands = "";
      while (i + 1 < argc && (argv[i + 1][0] != '-'))
      {
        std::string file_commands = Madara::Utility::file_to_string (argv[i + 1]);
        madara_commands += file_commands + ";";
        ++i;
      }
    }
    else if (arg1 == "-t" || arg1 == "--sim_time")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> sim_time;
      }

      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--period")
    {
      if (i + 1 < argc)
      {
        std::stringstream buffer (argv[i + 1]);
        buffer >> period;
      }
      ++i;
    }
    else if (arg1 == "-s" || arg1 == "--south_west")
    {
      if (i + 1 < argc)
        sscanf (argv[i + 1], "%lf,%lf", &sw_lat, &sw_long);
      ++i;
    }
    else if (arg1 == "-n" || arg1 == "--north_east")
    {
      if (i + 1 < argc)
        sscanf (argv[i + 1], "%lf,%lf", &ne_lat, &ne_long);
      ++i;
    }
    else
    {
      MADARA_DEBUG (MADARA_LOG_EMERGENCY, (LM_DEBUG, 
"\nProgram summary for %s:\n\n" \
"  Creates VREP simulation environment of specific size and launches\n" \
"  simulated gams agents\n\n" \
" [-a|--agents <number>]     number of agents to launch\n" \
" [-e|--terminal <exe>]     terminal executable for system\n" \
" [-m|--madara_file <file_name>] files with madara commands to read in\n" \
" [-n|--north_east <coords>]  northeast corner coordinates, ex. \"40,-72\"\n" \
" [-p|--period <number>]         time, in seconds, between control loop executions\n" \
" [-s|--south_west <coords>]  southeast corner coordinates, ex. \"40,-72\"\n" \
" [-t|--sim_time <number>]       time, in seconds, to run simulation\n" \
"\n",
        argv[0]));
      exit (0);
    }
  }
}

/**
 * Get dimensions of environment using SW corner and NE corner
 * VREP uses y coord for N/S and x for E/W
 **/
void 
get_dimensions (double &max_x, double &max_y)
{
  // assume the Earth is a perfect sphere
  const double EARTH_RADIUS = 6371000.0;
  const double EARTH_CIRCUMFERENCE = 2 * EARTH_RADIUS * M_PI;

  // get y/lat first
  max_y = (ne_lat - sw_lat) / 360.0 * EARTH_CIRCUMFERENCE;
  
  // assume the meters/degree longitude is constant throughout environment
  // convert the longitude/x coordinates
  double r_prime = EARTH_RADIUS * cos (DEG_TO_RAD (sw_lat));
  double circumference = 2 * r_prime * M_PI;
  max_x = (ne_long - sw_long) / 360.0 * circumference;
}

/**
 * entry point
 **/
int
main (int /*argc*/, char ** /*argv*/)
{
//  settings.type = Madara::Transport::MULTICAST;
//
//  // handle all user arguments
//  handle_arguments (argc, argv);
//  
//  if (settings.hosts.size () == 0)
//  {
//    // setup default transport as multicast
//    settings.hosts.push_back (default_multicast);
//  }
//
//  // create knowledge base and a control loop
//  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);
//  controller::Base loop (knowledge);
//
//  // initialize variables and function stubs
//  loop.init_vars (settings.id, processes);
//  
//  // read madara initialization
//  if (madara_commands != "")
//    knowledge.evaluate (madara_commands,
//      Madara::Knowledge_Engine::Eval_Settings(false, true));

  // connect to vrep
  string vrep_host = "127.0.0.1";
  int vrep_port = 19905, client_id;
  client_id = simxStart (vrep_host.c_str (), vrep_port, true, true, 2000, 5);
  if (client_id == -1)
  {
    cerr << "failure connecting to vrep" << endl;
    exit(-1);
  }

  // find environment parameters
  double max_x, max_y;
  get_dimensions (max_x, max_y);
  int num_x = max_x / 20 + 2;
  int num_y = max_y / 20 + 2;
  cout << "creating floor of size at least: " << max_x << " x " << 
    max_y << endl;

  // load floor models
  string model_file (getenv("VREP_ROOT"));
  const int floor_side = 20;
  model_file += "/models/infrastructure/floors/20mX20m floor.ttm";
  cout << "placing floor tiles...";
  for (int i = 0; i < (num_x * num_y); ++i)
  {
    // find where it should go
    simxFloat pos[3];
    pos[0] = (i / num_y) * floor_side;
    pos[1] = (i % num_y) * floor_side;
    pos[2] = 0;

    // load object
    int node_id;
    if (simxLoadModel (client_id, model_file.c_str (), 0, &node_id,
      simx_opmode_oneshot_wait) != simx_error_noerror)
    {
      cerr << "failure loading floor model" << endl;
      exit(0);
    }

    // move object
    simxSetObjectPosition (client_id, node_id, sim_handle_parent, pos,
      simx_opmode_oneshot_wait);
  }
  cout << "done" << endl;

  // load plants as position markers
  model_file = getenv("VREP_ROOT");
  model_file += "/models/furniture/plants/indoorPlant.ttm";
  cout << "placing plants as markers...";
  for (int i = 0; i < (num_x * num_y); ++i)
  {
    // find where it should go
    simxFloat pos[3];
    pos[0] = (i / num_y) * floor_side;
    pos[1] = (i % num_y) * floor_side;
    pos[2] = 0;

    // load object
    int node_id;
    if (simxLoadModel (client_id, model_file.c_str (), 0, &node_id,
      simx_opmode_oneshot_wait) != simx_error_noerror)
    {
      cerr << "failure loading plant model" << endl;
      exit(0);
    }

    // move object
    simxSetObjectPosition (client_id, node_id, sim_handle_parent, pos,
      simx_opmode_oneshot_wait);
  }

  // launch gams sims

  // wait for sync

  // start sim

  // print all knowledge values
  //knowledge.print ();

  return 0;
}
