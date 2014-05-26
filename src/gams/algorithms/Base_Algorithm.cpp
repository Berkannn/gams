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

#include "Base_Algorithm.h"
#include "gams/utility/Region.h"

gams::algorithms::Base::Base (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self,
  variables::Devices * devices)
  : knowledge_ (knowledge), platform_ (platform),
    sensors_ (sensors), self_ (self), devices_ (devices),
    executions_ (0)
{
}

gams::algorithms::Base::~Base ()
{
}

void
gams::algorithms::Base::operator= (const Base & rhs)
{
  if (this != &rhs)
  {
    this->knowledge_ = rhs.knowledge_;
    this->platform_ = rhs.platform_;
    this->sensors_ = rhs.sensors_;
    this->self_ = rhs.self_;
    this->status_ = rhs.status_;
  }
}

void
gams::algorithms::Base::set_sensors (variables::Sensors * sensors)
{
  sensors_ = sensors;
}

void
gams::algorithms::Base::set_platform (platforms::Base * platform)
{
  platform_ = platform;
}
   
void
gams::algorithms::Base::set_self (variables::Self * self)
{
  self_ = self;
}
   
void
gams::algorithms::Base::set_devices (variables::Devices * devices)
{
  devices_ = devices;
}

gams::utility::Region
gams::algorithms::Base::parse_region ()
{
  // get region from knowledge base
  //std::string region = knowledge_->get ("device.{.id}.command.1").to_string ();
  int id = knowledge_->get (".id").to_integer ();
  char expr[50];
  sprintf(expr, "device.%d.command.1", id);
  std::string region = knowledge_->get (expr).to_string ();
  
  // parse vertices
  std::vector<gams::utility::Position> vertices;
  char expression[50];
  sprintf (expression, "%s.type", region.c_str ());
  int region_type = knowledge_->get (expression).to_integer ();
  switch (region_type)
  {
    case 0: // arbitrary convex polygon
    {
      sprintf (expression, "%s.size", region.c_str ());
      const int num_vertices = knowledge_->get (expression).to_integer ();
      for (int i = 1; i <= num_vertices; ++i) // get the vertices
      {
        sprintf (expression, "%s.%d", region.c_str (), i);
        utility::Position pos;
        sscanf (knowledge_->get (expression).to_string ().c_str (),
          "%lf,%lf,%lf", &pos.x, &pos.y, &pos.z);
        vertices.push_back (pos);
      }
      break;
    }
    default:
      std::cerr << "invalid region type: " << region_type << endl;
  }

  return gams::utility::Region (vertices);
}
