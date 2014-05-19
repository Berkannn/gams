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
#include "Snake_Area_Coverage.h"

#include <cmath>

gams::algorithms::Snake_Area_Coverage::Snake_Area_Coverage (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base (knowledge, platform, sensors, self)
{
  status_.init_vars (*knowledge, "sac");

  // get region information
  std::vector<utility::Position> vertices = parse_region ();
  const int num_edges = vertices.size ();
  for(int i = 0; i < num_edges; ++i)
    cout << "vertex_" << i << ": " << vertices[i].to_string() << endl;

  // find longest edge
  int longest_edge = 0;
  double max_dist = vertices[0].distance (vertices[1]);
  for(int i = 1; i < num_edges; ++i)
  {
    double dist = vertices[i].distance (vertices[(i + 1) % num_edges]);
    if(dist > max_dist)
    {
      max_dist = dist;
      longest_edge = i;
    }
  }
  cout << "longest edge: " << longest_edge << endl;

  // starting points are vertices of longest edge
  waypoints_.push_back (vertices[longest_edge]);
  waypoints_.push_back (vertices[(longest_edge + 1) % num_edges]);

  // determine shift direction
  const double shift = 1.0; // TODO: update for sensor range
  double m_0;
  const utility::Position & p_0 = vertices[longest_edge];
  const utility::Position & p_1 = vertices[(longest_edge + 1) % num_edges];
  if (p_0.slope_2d (p_1, m_0))
  {
    // we have a non-vertical line, so find new line
    const double delta_b = -1 * shift * pow (pow (m_0, 2) + 1, 0.5);

    // loop until snaked through entire polygon
    int intercept_idx = -1; // arbitrary non-zero value so we enter loop
    unsigned int loop = 0; // loop counter
    while(intercept_idx != 0)
    {
      ++loop;
      cout << "loop: " << loop << endl;

      // find intercept
      intercept_idx = 0;
      utility::Position intercepts[2];
      for(int i = 1; i < num_edges && intercept_idx < 2; ++i)
      {
        // check current vertex for intercept
        int cur_vertex = (longest_edge + i) % num_edges;
        cout << "checking edge: " << cur_vertex << endl;
        const utility::Position & p_n_0 = vertices[cur_vertex];
        const utility::Position & p_n_1 = vertices[(cur_vertex + 1) % num_edges];
        cout << "\tp_n_0: " << p_n_0.to_string() << endl;
        cout << "\tp_n_1: " << p_n_1.to_string() << endl;

        // non-vertical line
        double m_n;
        if (p_n_0.slope_2d(p_n_1, m_n))
        {
          cout << "\tnot a vertical line" << endl;
          const double y_w = (m_n * p_0.y - m_n * m_0 * p_0.x +
            m_n * loop * delta_b - m_0 * p_n_0.y - m_0 * m_n * p_n_0.x) /
            (m_n - m_0);
          const double x_w = (y_w - p_0.y + m_0 * p_0.x - loop * delta_b) / 
            m_0;
          cout << "\t\tx_w: " << x_w << endl;
          cout << "\t\ty_w: " << y_w << endl;
          utility::Position check;
          check.x = x_w;
          check.y = y_w;
          if (p_n_0.is_between_2d(p_n_1, check))
          {
            cout << "\t\tfound intercept" << endl;
            check.z = 1.5;
            intercepts[intercept_idx++] = check;
          }
          else
            cout << "\t\tnot an intercept" << endl;
        }
      }

      // found intercepts, now which one do we go to first?
      if (intercept_idx > 1)
      {
        const utility::Position & prev = waypoints_[waypoints_.size () - 1];
        if (prev.distance_2d (intercepts[0]) > prev.distance_2d (intercepts[1]))
        {
          waypoints_.push_back (intercepts[1]);
          waypoints_.push_back (intercepts[0]);
        }
        else
        {
          waypoints_.push_back (intercepts[0]);
          waypoints_.push_back (intercepts[1]);
        }
      }
      else if (intercept_idx > 0)
      {
        waypoints_.push_back (intercepts[0]);
      }
    }
  }
  else // we have a vertical line, so just shift the x coord
  {
  }

  for(int i = 0; i < waypoints_.size(); ++i)
    cout << "waypoint_" << i << ": " << waypoints_[i].to_string() << endl;
}

gams::algorithms::Snake_Area_Coverage::~Snake_Area_Coverage ()
{
}

void
gams::algorithms::Snake_Area_Coverage::operator= (
  const Snake_Area_Coverage & rhs)
{
  if (this != &rhs)
  {
    this->platform_ = rhs.platform_;
    this->sensors_ = rhs.sensors_;
    this->self_ = rhs.self_;
    this->status_ = rhs.status_;
  }
}


int
gams::algorithms::Snake_Area_Coverage::analyze (void)
{
  platform_->get_position (current_position_);

  current_position_.to_container (self_->device.location);

  return 0;
}
      

int
gams::algorithms::Snake_Area_Coverage::execute (void)
{
  platform_->move (waypoints_[cur_waypoint_]);
  return 0;
}


int
gams::algorithms::Snake_Area_Coverage::plan (void)
{
  // generate new next position if necessary
  if (current_position_.approximately_equal(waypoints_[cur_waypoint_], 
    platform_->get_position_accuracy ()))
  {
    cur_waypoint_ = (cur_waypoint_ + 1) % waypoints_.size();
    cout << "current waypoint: (" << cur_waypoint_ << ") " << 
      waypoints_[cur_waypoint_].to_string () << endl;
  }

  return 0;
}
