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
#include "Device.h"

typedef  Madara::Knowledge_Record::Integer  Integer;


gams::variables::Device::Device ()
{
}

gams::variables::Device::~Device ()
{
}

void
gams::variables::Device::operator= (const Device & device)
{
  if (this != &device)
  {
    this->battery_remaining = device.battery_remaining;
    this->bridge_id = device.bridge_id;
    this->coverage_type = device.coverage_type;
    this->is_mobile = device.is_mobile;
    this->location = device.location;
    this->source = device.source;
    this->dest = device.dest;
    this->home = device.home;
    this->min_alt = device.min_alt;
    this->next_coverage_type = device.next_coverage_type;
    this->search_area_id = device.search_area_id;
    this->temperature = device.temperature;
    this->command = device.command;
    this->command_args = device.command_args;
  }
}


void
gams::variables::Device::init_vars (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  const Integer & id)
{
  Madara::Knowledge_Engine::Knowledge_Update_Settings keep_local (true);

  // create the device name string identifier ('device.{id}')
  std::stringstream buffer;
  buffer << "device.";
  buffer << id;
  std::string device_name (buffer.str ());

  std::stringstream local_buffer;
  local_buffer << ".device.";
  local_buffer << id;
  std::string local_device_name (local_buffer.str ());

  // initialize the variable containers
  min_alt.set_name (device_name + ".min_alt", knowledge);
  location.set_name (device_name + ".location", knowledge);
  is_mobile.set_name (device_name + ".mobile", knowledge);
  battery_remaining.set_name (device_name + ".battery", knowledge);
  bridge_id.set_name (device_name + ".bridge_id", knowledge);
  coverage_type.set_name (device_name + ".area_coverage_type", knowledge);
  next_coverage_type.set_name (device_name + ".next_area_coverage_type",
    knowledge);
  search_area_id.set_name (device_name + ".search_area_id", knowledge);
  command.set_name (device_name + ".command", knowledge);
  home.set_name (device_name + ".home", knowledge);
  source.set_name (device_name + ".source", knowledge);
  dest.set_name (device_name + ".dest", knowledge);
  command_args.set_name (device_name + ".command", knowledge);
  
  // keep certain varaible changes as local only
  command.set_settings (keep_local);
  command_args.set_settings (keep_local);

  // environment variables
  temperature.set_name (device_name + ".temperature", knowledge);
}

void
gams::variables::Device::init_vars (
  Madara::Knowledge_Engine::Variables & knowledge,
  const Integer & id)
{
  Madara::Knowledge_Engine::Knowledge_Update_Settings keep_local (true);

  // create the device name string identifier ('device.{id}')
  std::stringstream buffer;
  buffer << "device.";
  buffer << id;
  std::string device_name (buffer.str ());
  
  std::stringstream local_buffer;
  local_buffer << ".device.";
  local_buffer << id;
  std::string local_device_name (local_buffer.str ());

  // initialize the variable containers
  min_alt.set_name (device_name + ".min_alt", knowledge);
  location.set_name (device_name + ".location", knowledge, 3);
  is_mobile.set_name (device_name + ".mobile", knowledge);
  battery_remaining.set_name (device_name + ".battery", knowledge);
  bridge_id.set_name (device_name + ".bridge_id", knowledge);
  coverage_type.set_name (device_name + ".area_coverage_type", knowledge);
  next_coverage_type.set_name (device_name + ".next_area_coverage_type",
    knowledge);
  search_area_id.set_name (device_name + ".search_area_id", knowledge);
  command.set_name (device_name + ".command", knowledge);
  home.set_name (device_name + ".home", knowledge);
  source.set_name (device_name + ".source", knowledge);
  dest.set_name (device_name + ".dest", knowledge);
  command_args.set_name (device_name + ".command", knowledge);

  // keep certain varaible changes as local only
  command.set_settings (keep_local);
  command_args.set_settings (keep_local);

  // environment variables
  temperature.set_name (device_name + ".temperature", knowledge);
}

void gams::variables::init_vars (Devices & variables,
      Madara::Knowledge_Engine::Knowledge_Base & knowledge,
      const Madara::Knowledge_Record::Integer & processes)
{
  Integer limit = processes;
  if (processes >= 0)
  {
    variables.resize ((size_t) processes);
  }
  else
  {
    limit = knowledge.get ("device.size").to_integer ();
  }

  for (Integer i = 0; i < limit; ++i)
  {
    variables[i].init_vars (knowledge, i);
  }
}
