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
    this->battery_remaining_ = device.battery_remaining_;
    this->bridge_id_ = device.bridge_id_;
    this->coverage_type_ = device.coverage_type_;
    this->is_mobile_ = device.is_mobile_;
    this->location_ = device.location_;
    this->min_alt_ = device.min_alt_;
    this->next_coverage_type_ = device.next_coverage_type_;
    this->search_area_id_ = device.search_area_id_;
    this->temperature_ = device.temperature_;
  }
}


void
gams::variables::Device::init_vars (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  const Integer & id)
{
  // create the device name string identifier ('device.{id}')
  std::stringstream buffer;
  buffer << "device.";
  buffer << id;
  std::string device_name (buffer.str ());

  // initialize the variable containers
  min_alt_.set_name (device_name + ".min_alt", knowledge);
  location_.set_name (device_name + ".location", knowledge, 3);
  is_mobile_.set_name (device_name + ".mobile", knowledge);
  battery_remaining_.set_name (device_name + ".battery", knowledge);
  bridge_id_.set_name (device_name + ".bridge_id", knowledge);
  coverage_type_.set_name (device_name + ".area_coverage_type", knowledge);
  next_coverage_type_.set_name (device_name + ".next_area_coverage_type",
    knowledge);
  search_area_id_.set_name (device_name + ".search_area_id", knowledge);

  // environment variables
  temperature_.set_name (device_name + ".temperature", knowledge);
}

void gams::variables::init_vars (Devices & devices,
      Madara::Knowledge_Engine::Knowledge_Base & knowledge,
      const Madara::Knowledge_Record::Integer & processes)
{
  devices.resize ((size_t) processes);

  for (Integer i = 0; i < processes; ++i)
  {
    devices[i].init_vars (knowledge, i);
  }
}
