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
#include "Printer_Platform.h"


gams::platforms::Printer_Platform::Printer_Platform (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  variables::Sensors * sensors,
  variables::Platforms & platforms,
  variables::Self & self)
  : Base (&knowledge, sensors), self_ (&self)
{
  platforms["printer"].init_vars (knowledge, "printer");
  status_ = platforms["printer"];
}

gams::platforms::Printer_Platform::~Printer_Platform ()
{
}

void
gams::platforms::Printer_Platform::operator= (const Printer_Platform & rhs)
{
  if (this != &rhs)
  {
    this->sensors_ = rhs.sensors_;
    this->status_ = rhs.status_;
    this->self_ = rhs.self_;
  }
}

void
gams::platforms::Printer_Platform::get_sensors (variables::Sensor_Names & sensors)
{
  bool needs_change (false);

  if (sensors.size () != 1)
  {
    needs_change = true;
    sensors.resize (1);
  }
  else
  {
    if (sensors[0] != "thermal")
        needs_change = true;
  }

  if (needs_change)
  {
    sensors[0] = "thermal";
  }
}


int
gams::platforms::Printer_Platform::move (double x, double y, double z)
{
  std::cerr << "  platform.move (" << x << ", " << y << ", " << z << ")\n";

  if (sensors_)
    std::cerr << "    platform.sensors_ is set.\n";
  else
    std::cerr << "    ERROR: platform.sensors_ is not set.\n";
  
  if (self_)
    std::cerr << "    platform.self_ is set.\n";
  else
    std::cerr << "    ERROR: platform.self_ is not set.\n";
  
  status_.waiting = 0;
  status_.moving = 1;

  std::cerr << "    platform.status_.ok == " << *status_.ok << "\n";
  std::cerr << "    platform.status_.waiting == " << *status_.waiting << "\n";
  std::cerr << "    platform.status_.deadlocked == "
    << *status_.deadlocked << "\n";
  std::cerr << "    platform.status_.failed == "
    << *status_.failed << "\n";
  std::cerr << "    platform.status_.moving == "
    << *status_.moving << "\n";
  std::cerr << "    platform.status_.reduced_sensing == "
    << *status_.reduced_sensing << "\n";
  std::cerr << "    platform.status_.reduced_movement == "
    << *status_.reduced_movement << "\n";
  std::cerr << "    platform.status_.communication_available == "
    << *status_.communication_available << "\n";
  std::cerr << "    platform.status_.sensors_available == "
    << *status_.sensors_available << "\n";
  std::cerr << "    platform.status_.movement_available == "
    << *status_.movement_available << "\n";
  std::cerr << "    platform.status_.gps_spoofed == "
    << *status_.gps_spoofed << "\n";
  
  return 0;
}
      
int
gams::platforms::Printer_Platform::sense (void)
{
  std::cerr << "platform.sense ()\n";

  if (sensors_)
    std::cerr << "  platform.sensors_ is set.\n";
  else
    std::cerr << "  ERROR: platform.sensors_ is not set.\n";
  
  if (self_)
    std::cerr << "  platform.self_ is set.\n";
  else
    std::cerr << "  ERROR: platform.self_ is not set.\n";
  
  std::cerr << "  platform.status_.ok == " << *status_.ok << "\n";
  std::cerr << "  platform.status_.waiting == " << *status_.waiting << "\n";
  std::cerr << "  platform.status_.deadlocked == "
    << *status_.deadlocked << "\n";
  std::cerr << "  platform.status_.failed == "
    << *status_.failed << "\n";
  std::cerr << "  platform.status_.moving == "
    << *status_.moving << "\n";
  std::cerr << "  platform.status_.reduced_sensing == "
    << *status_.reduced_sensing << "\n";
  std::cerr << "  platform.status_.reduced_movement == "
    << *status_.reduced_movement << "\n";
  std::cerr << "  platform.status_.communication_available == "
    << *status_.communication_available << "\n";
  std::cerr << "  platform.status_.sensors_available == "
    << *status_.sensors_available << "\n";
  std::cerr << "  platform.status_.movement_available == "
    << *status_.movement_available << "\n";
  std::cerr << "  platform.status_.gps_spoofed == "
    << *status_.gps_spoofed << "\n";
  
  return 0;
}
      
int
gams::platforms::Printer_Platform::analyze (void)
{
  std::cerr << "platform.analyze ()\n";

  if (sensors_)
    std::cerr << "  platform.sensors_ is set.\n";
  else
    std::cerr << "  ERROR: platform.sensors_ is not set.\n";
  
  if (self_)
    std::cerr << "  platform.self_ is set.\n";
  else
    std::cerr << "  ERROR: platform.self_ is not set.\n";
  
  status_.moving = 0;
  status_.communication_available = 1;
  status_.movement_available = 1;
  status_.sensors_available = 1;
  status_.waiting = 1;

  std::cerr << "  platform.status_.ok == " << *status_.ok << "\n";
  std::cerr << "  platform.status_.waiting == " << *status_.waiting << "\n";
  std::cerr << "  platform.status_.deadlocked == "
    << *status_.deadlocked << "\n";
  std::cerr << "  platform.status_.failed == "
    << *status_.failed << "\n";
  std::cerr << "  platform.status_.moving == "
    << *status_.moving << "\n";
  std::cerr << "  platform.status_.reduced_sensing == "
    << *status_.reduced_sensing << "\n";
  std::cerr << "  platform.status_.reduced_movement == "
    << *status_.reduced_movement << "\n";
  std::cerr << "  platform.status_.communication_available == "
    << *status_.communication_available << "\n";
  std::cerr << "  platform.status_.sensors_available == "
    << *status_.sensors_available << "\n";
  std::cerr << "  platform.status_.movement_available == "
    << *status_.movement_available << "\n";
  std::cerr << "  platform.status_.gps_spoofed == "
    << *status_.gps_spoofed << "\n";
  
  return 0;
}
