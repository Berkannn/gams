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
 * 3. The names �Carnegie Mellon University,� "SEI� and/or �Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN �AS-IS� BASIS. CARNEGIE MELLON
 *      UNIVERSITY MAKES NO WARRANTIES OF ANY KIND, EITHER EXPRESSED OR
 *      IMPLIED, AS TO ANY MATTER INCLUDING, BUT NOT LIMITED TO, WARRANTY OF
 *      FITNESS FOR PURPOSE OR MERCHANTABILITY, EXCLUSIVITY, OR RESULTS
 *      OBTAINED FROM USE OF THE MATERIAL. CARNEGIE MELLON UNIVERSITY DOES
 *      NOT MAKE ANY WARRANTY OF ANY KIND WITH RESPECT TO FREEDOM FROM PATENT,
 *      TRADEMARK, OR COPYRIGHT INFRINGEMENT.
 * 
 *      This material has been approved for public release and unlimited
 *      distribution.
 */

/**
 * @file Follow.cpp
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * This file contains the definition of the Follow algorithm
 */

#include "gams/algorithms/Follow.h"

#include <sstream>

#include "gams/algorithms/GPS_Position.h"

using std::stringstream;

gams::algorithms::Follow::Follow (
  const Madara::Knowledge_Record& id,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge = 0,
  platforms::Base * platform = 0,
  variables::Sensors * sensors = 0,
  variables::Self * self = 0) :
  Base (knowledge, platform, sensors, self)
{
  stringstream location_string;
  location_string << "device" << id.to_integer ();
  location_string << ".location";
  target_location_.set_name (location_string.str (), *knowledge);
}

gams::algorithms::Follow::~Follow ()
{
}

void
gams::algorithms::Follow::operator= (const Follow & rhs)
{
  if (this != &rhs)
  {
    this->Base::operator= (rhs);
    this->target_location_ = rhs.target_location_;
    this->next_position_ = rhs.next_position_;
    this->previous_locations_ = rhs.previous_locations_;
  }
}

int
gams::algorithms::Follow::analyze (void)
{
  utility::GPS_Position current;
  current.from_container (target_location_);
  previous_locations_.push (current);
}
      
int
gams::algorithms::Follow::execute (void)
{
  platform_->move (next_position_);
}

int
gams::algorithms::Follow::plan (void)
{
  if (previous_locations_.size () > 5)
  {
    next_position_ = previous_locations_.front ();
    previous_locations_.pop ();
  }
}
