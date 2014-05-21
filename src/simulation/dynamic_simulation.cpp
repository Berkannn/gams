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
using std::cerr;
using std::endl;
#include <cmath>
#include <string>
using std::string;
#include <vector>
using std::vector;

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
double period = 1.0;
double sim_time = 50.0;

// vrep connection information
string vrep_host = "127.0.0.1";
int vrep_port = 19905;

// string executable
string executable = "./gams_controller";

// SW corner of simulation
double sw_lat = 40.442824;
double sw_long = -79.940967;

// NE corner of simulation
double ne_lat = 40.44355;
double ne_long = -79.939626;

// Agent information
int num_agents = 0;

// place plants or not
bool plants = false;

// use gps coords
bool gps = false;

/**
 * Print out program usage
 **/
void print_usage (string prog_name)
{
  cerr << "Program summary for " << prog_name << ":" << endl;
  cerr << endl;
  cerr << "   Creates VREP simulation environment of specific size and" << endl;
  cerr << "   launches simulated gams agents" << endl;
  cerr << endl;
  cerr << "   [-g | --gps]" << endl;
  cerr << "       use gps coords (instead of vrep)" << endl;
  cerr << "   [-l | --log_level <number>]" << endl;
  cerr << "       MADARA log level, 1-10" << endl;
  cerr << "   [-n | --num_agents <number>]" << endl;
  cerr << "       number of agents that will be launched" << endl;
  cerr << "   [--north_east <coords>]" << endl;
  cerr << "       northeast corner coordinates, ex. \"40,-72\"" << endl;
  cerr << "   [-p | --plants]" << endl;
  cerr << "       place plants as position markers" << endl;
  cerr << "   [--south_west <coords>]" << endl;
  cerr << "       southeast corner coordinates, ex. \"40,-72\"" << endl;
  cerr << "   [-v | --vrep <ip_address> <port>]" << endl;
  cerr << "       vrep connection information" << endl;

  exit(0);
}

// handle command line arguments
void handle_arguments (int argc, char** argv)
{
  for (int i = 1; i < argc; ++i)
  {
    std::string arg1 (argv[i]);

    if (arg1 == "-g" || arg1 == "--gps")
    {
      gps = true;
    }
    else if (arg1 == "-l" || arg1 == "--log_level")
    {
      sscanf (argv[i + 1], "%d", &MADARA_debug_level);
      ++i;
    }
    else if (arg1 == "-n" || arg1 == "--num_agents")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
        sscanf(argv[i + 1], "%d", &num_agents);
      else
        print_usage (argv[0]);
      ++i;
    }
    else if (arg1 == "--north_east")
    {
      if (i + 1 < argc)
        sscanf (argv[i + 1], "%lf,%lf", &ne_lat, &ne_long);
      else
        print_usage (argv[0]);
      ++i;
    }
    else if (arg1 == "-p" || arg1 == "--plants")
    {
      plants = true;
    }
    else if (arg1 == "--south_west")
    {
      if (i + 1 < argc)
        sscanf (argv[i + 1], "%lf,%lf", &sw_lat, &sw_long);
      else
        print_usage (argv[0]);
      ++i;
    }
    else if (arg1 == "-v" || arg1 == "--vrep")
    {
      if (i + 1 < argc && argv[i + 1][0] != '-')
        vrep_host = argv[i + 1];
      else
        print_usage (argv[0]);
      ++i;

      if (i + 1 < argc && argv[i + 1][0] != '-')
      {
          sscanf(argv[i + 1], "%d", &vrep_port);
          ++i;
      }
    }
    else
    {
      print_usage (argv[0]);
    }
  }
}

/**
 * Get dimensions of environment using SW corner and NE corner
 * VREP uses y coord for N/S and x for E/W
 **/
void get_dimensions (double &max_x, double &max_y)
{
  if (gps)
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
  else // vrep
  {
    max_y = ne_lat - sw_lat;
    max_x = ne_long - sw_long;
  }
}

/**
 * Create environment in vrep
 * Add floors and plants as visible markers
 * @param client_id id for vrep connection
 **/
void create_environment (int client_id)
{
  // find environment parameters
  double max_x, max_y;
  get_dimensions (max_x, max_y);
  cout << "creating environment of size " << max_x << " x " << max_y << "...";
  int num_x = max_x / 20 + 2;
  int num_y = max_y / 20 + 2;

  // load floor models
  string model_file (getenv("VREP_ROOT"));
  const int floor_side = 20;
  model_file += "/models/infrastructure/floors/20mX20m floor.ttm";
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
  if(plants)
  {
    cout << "placing plants as markers...";
    model_file = getenv("VREP_ROOT");
    model_file += "/models/furniture/plants/indoorPlant.ttm";
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
    cout << "done" << endl;
  }
}

/**
 * Wait for all simulated control loops to sync, then start vrep
 **/
void start_simulator (const int & client_id,
  Madara::Knowledge_Engine::Knowledge_Base & knowledge)
{
  // wait for all processes to get up
  std::stringstream buffer;
  buffer << "S0.init";
  for (unsigned int i = 1; i < num_agents; ++i)
    buffer << " && S" << i << ".init";
  std::string expression = buffer.str ();
  Madara::Knowledge_Engine::Compiled_Expression compiled;
  compiled = knowledge.compile (expression);
  cout << "waiting for " << num_agents << " agent(s) to come online...";
  knowledge.wait(compiled);
  cout << "done" << endl;

  // start the simulation
  cout << "starting simulation...";
  simxStartSimulation (client_id, simx_opmode_oneshot_wait);
  cout << "done" << endl;

  // inform simulated control loops to begin
  cout << "informing agents to continue...";
  knowledge.set ("begin_sim", Madara::Knowledge_Record::Integer(1));
  cout << "done" << endl;
}

/**
 * entry point
 **/
int main (int argc, char ** argv)
{
  settings.type = Madara::Transport::MULTICAST;

  // handle all user arguments
  handle_arguments (argc, argv);

  // create knowledge base
  if (settings.hosts.size () == 0)
  {
    // setup default transport as multicast
    settings.hosts.push_back (default_multicast);
  }
  Madara::Knowledge_Engine::Knowledge_Base knowledge (host, settings);

  // connect to vrep
  cout << "connecting to vrep...";
  int client_id = 
    simxStart (vrep_host.c_str(), vrep_port, true, true, 2000, 5);
  if (client_id == -1)
  {
    cerr << "failure connecting to vrep" << endl;
    exit(-1);
  }
  cout << "done" << endl;

  // actual work
  create_environment (client_id);
  if(num_agents > 0)
    start_simulator (client_id, knowledge);

  // close connection to vrep
  cout << "closing vrep connection...";
  simxFinish(client_id);
  cout << "done" << endl;

  // exit
  return 0;
}
