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
#include "Null_Platform.h"
#include "gams/loggers/Global_Logger.h"

gams::platforms::Base_Platform *
gams::platforms::Null_Platform_Factory::create (
  const Madara::Knowledge_Vector & /*args*/,
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
{
  Base_Platform * result (0);
  
  if (knowledge && sensors && platforms && self)
  {
    result = new Null_Platform (knowledge, sensors, platforms, self);
  }

  return result;
}

gams::platforms::Null_Platform::Null_Platform (
  Madara::Knowledge_Engine::Knowledge_Base * knowledge,
  variables::Sensors * sensors,
  variables::Platforms * platforms,
  variables::Self * self)
  : Base_Platform (knowledge, sensors, self)
{
  if (platforms && knowledge)
  {
    (*platforms)[get_id ()].init_vars (*knowledge, get_id ());
    status_ = (*platforms)[get_id ()];
  }
}

gams::platforms::Null_Platform::~Null_Platform ()
{
}

void
gams::platforms::Null_Platform::operator= (const Null_Platform & rhs)
{
  if (this != &rhs)
  {
    platforms::Base_Platform * dest = dynamic_cast <platforms::Base_Platform *> (this);
    const platforms::Base_Platform * source =
      dynamic_cast <const platforms::Base_Platform *> (&rhs);

    *dest = *source;
  }
}
 
int
gams::platforms::Null_Platform::analyze (void)
{ 
  return 0;
}

std::string
gams::platforms::Null_Platform::get_id () const
{
  return "null";
}

std::string
gams::platforms::Null_Platform::get_name () const
{
  return "Null";
}

double
gams::platforms::Null_Platform::get_accuracy () const
{
  return 0.0;
}

double
gams::platforms::Null_Platform::get_move_speed () const
{
  return 0.0;
}

int
gams::platforms::Null_Platform::home (void)
{
  return 0;
}

int
gams::platforms::Null_Platform::land (void)
{
  return 0;
}

int
gams::platforms::Null_Platform::move (const utility::Position & /*position*/,
  const double & /*epsilon*/)
{
  return 0;
}

int
gams::platforms::Null_Platform::sense (void)
{
  return 0;
}

void
gams::platforms::Null_Platform::set_move_speed (const double& /*speed*/)
{
}

int
gams::platforms::Null_Platform::takeoff (void)
{
  return 0;
}
