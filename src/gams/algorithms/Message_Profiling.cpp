/**
 * Copyright (c) 2015 Carnegie Mellon University. All Rights Reserved.
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
 * 3. The names "Carnegie Mellon University," "SEI" and/or "Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN "AS-IS" BASIS. CARNEGIE MELLON
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

#include "gams/algorithms/Message_Profiling.h"

#include <iostream>
#include <sstream>

using std::stringstream;
using std::string;
using std::map;
using std::cerr;
using std::endl;

const string gams::algorithms::Message_Profiling::key_prefix_ = "message_profiling";

gams::algorithms::Base_Algorithm *
gams::algorithms::Message_Profiling_Factory::create (
  const Madara::Knowledge_Vector & args,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base_Platform * platform,
  variables::Sensors * sensors,
  variables::Self * self,
  variables::Devices * /*devices*/)
{
  Base_Algorithm * result (0);

  // set defaults
  Madara::Knowledge_Record send_size (Madara::Knowledge_Record::Integer (100));

  if (knowledge && sensors && self)
  {
    if (args.size () >= 1)
      send_size = args[0];

    //if (send_size.is_integer_type ())
      result = new Message_Profiling (send_size, knowledge, platform, sensors, self);
  }

  return result;
}

gams::algorithms::Message_Profiling::Message_Profiling (
  const Madara::Knowledge_Record& send, 
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  platforms::Base * platform,
  variables::Sensors * sensors,
  variables::Self * self)
  : Base_Algorithm (knowledge, platform, sensors, self), 
    send_size_ (20)
{
  status_.init_vars (*knowledge, "message_profiling", self->id.to_integer ());
  status_.init_variable_values ();

  // attach filter
  //knowledge->close_transport ();

  Madara::Transport::QoS_Transport_Settings settings;

//  settings.hosts.push_back (send.to_string ());
//  settings.type = Madara::Transport::BROADCAST;

//  const std::string default_broadcast ("128.237.127.255:15000");
//  settings.hosts.push_back (default_broadcast);
//  settings.type = Madara::Transport::BROADCAST;

  const std::string default_multicast ("239.255.0.1:4150");
  settings.hosts.push_back (default_multicast);
  settings.type = Madara::Transport::MULTICAST;

  settings.add_receive_filter (&filter_);

  local_knowledge_ = new Madara::Knowledge_Engine::Knowledge_Base (
    knowledge_->get_id (), settings);

  // setup containers
  const size_t size = send.to_integer ();
  const string key = key_prefix_ + "." +
    knowledge_->get (".id").to_string ();
  data_.set_name (key + ".data", *local_knowledge_);
  data_ = string (size - 1, 'a'); // set value, will never change
  //count_.set_name (key + ".count", *knowledge);
}

gams::algorithms::Message_Profiling::~Message_Profiling ()
{
  delete local_knowledge_;
  local_knowledge_ = 0;

//  for (map<string, Message_Filter::Message_Data>::const_reference map_item : 
//    filter_.msg_map)
  for (map<string, size_t>::const_reference map_item : filter_.msg_map)
  {
//    size_t first = map_item.second.first;
//    size_t last = map_item.second.last;
//    size_t expected = last - first + 1;
//    size_t found = 0;
//    for (size_t i = first; i <= last; ++i)
//      if (map_item.second.present[i])
//        ++found;
//
//    double percent_missing = (expected - found) / double(expected);

    const string prefix = key_prefix_ + "." + map_item.first + ".";
    //knowledge_->set(prefix + "first", 
    //  Madara::Knowledge_Record::Integer (map_item.second.first));
    //knowledge_->set(prefix + "last", 
    //  Madara::Knowledge_Record::Integer (map_item.second.last));
    //knowledge_->set(prefix + "missing", percent_missing);
    knowledge_->set(prefix + "count", 
      Madara::Knowledge_Record::Integer (map_item.second));
  }
}

void
gams::algorithms::Message_Profiling::init_filtered_transport (
  Madara::Transport::QoS_Transport_Settings settings)
{
  settings.add_receive_filter (&filter_);
  local_knowledge_->attach_transport (knowledge_->get_id (), settings);
}

void
gams::algorithms::Message_Profiling::operator= (
  const Message_Profiling & rhs)
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
gams::algorithms::Message_Profiling::analyze (void)
{
  return OK;
}

int
gams::algorithms::Message_Profiling::execute (void)
{
  ++executions_;

  data_.modify ();
  //count_ = executions_;
  local_knowledge_->send_modifieds ();

  return 0;
}


int
gams::algorithms::Message_Profiling::plan (void)
{
  return 0;
}

gams::algorithms::Message_Profiling::Message_Filter::~Message_Filter ()
{
}

void
gams::algorithms::Message_Profiling::Message_Filter::filter (
  Madara::Knowledge_Map& /*records*/, 
  const Madara::Transport::Transport_Context& transport_context, 
  Madara::Knowledge_Engine::Variables& /*var*/)
{
  const string origin = transport_context.get_originator ();
  if (msg_map.find (origin) == msg_map.end ())
    msg_map[origin] = 0;
  size_t& data = msg_map[origin];
  ++data;

//  // get data struct
//  const string origin = transport_context.get_originator ();
//  Message_Data& data = msg_map[origin];
//
//  // loop through each update
//  for (Madara::Knowledge_Map::const_iterator iter = records.begin();
//	      iter != records.end(); ++iter)
//  {
//    Madara::Knowledge_Map::const_reference update = *iter;
//
//    // we only care about specific messages
//    if (update.second.is_integer_type () && 
//        update.first.find (key_prefix_) == 0 && 
//        update.first.find ("count") != std::string::npos)
//    {
//      // get msg number
//      size_t msg_num = update.second.to_integer ();
//
//      // is this the first?
//      if (data.first == -1)
//      {
//        data.first = msg_num;
//        data.last = msg_num;
//      }
//      // is this a previously missing message?
//      else if (msg_num < data.last)
//      {
//        // is this earlier than first?
//        if (msg_num < data.first)
//          data.first = msg_num;
//      }
//      else // this could only be a new message past last
//      {
//        data.last = msg_num;
//      }
//
//      if (data.present.size() <= msg_num)
//        data.present.reserve (msg_num * 2);
//      data.present [msg_num] = true;
//    }
//  }
}

string
gams::algorithms::Message_Profiling::Message_Filter::missing_messages_string ()
  const
{
//  stringstream ret_val;
//  for (map<string, Message_Data>::const_iterator iter = msg_map.begin();
//	      iter != msg_map.end(); ++iter)
//  {
//    map<string, Message_Data>::const_reference ref = *iter;
//    ret_val << ref.first << ": ";
//    for (size_t i = ref.second.first + 1; i < ref.second.last; ++i)
//    {
//      if(!ref.second.present[i])
//        ret_val << i << ",";
//    }
//    ret_val << endl;
//  }
//  return ret_val.str();
  return "";
}
