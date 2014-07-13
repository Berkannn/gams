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
#include "Accent.h"

#include <string>
using std::string;

gams::variables::Accent::Accent ()
{
}

gams::variables::Accent::~Accent ()
{
}

void
gams::variables::Accent::operator= (const Accent & accent)
{
  if (this != &accent)
  {
    this->command = accent.command;
    this->command_args = accent.command_args;
  }
}

void
gams::variables::Accent::init_vars (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  const Madara::Knowledge_Record::Integer& id)
{
  // create the accent name string identifier ('accent.{id}')
  string accent_name (make_variable_name (id));
  string local_accent_name ("." + accent_name);

  // initialize the variable containers
  command_args.set_name (accent_name + ".command", knowledge);

  // init settings
  init_variable_settings ();
}

void
gams::variables::Accent::init_vars (
  Madara::Knowledge_Engine::Variables & knowledge,
  const Madara::Knowledge_Record::Integer& id)
{
  // create the accent name string identifier ('accent.{id}')
  string accent_name (make_variable_name (id));
  string local_accent_name ("." + accent_name);

  // initialize the variable containers
  command.set_name (accent_name + ".command", knowledge);
  command_args.set_name (accent_name + ".command", knowledge);

  // init settings
  init_variable_settings ();
}

void gams::variables::init_vars (Accents & variables,
  Madara::Knowledge_Engine::Knowledge_Base & knowledge,
  const Madara::Knowledge_Record::Integer& processes)
{
  Madara::Knowledge_Record::Integer limit = processes;
  if (processes >= 0)
  {
    variables.resize (processes);
  }
  else
  {
    limit = knowledge.get ("accent.size").to_integer ();
  }

  for (unsigned int i = 0; i < limit; ++i)
  {
    variables[i].init_vars (knowledge, i);
  }
}

string
gams::variables::Accent::make_variable_name (
  const Madara::Knowledge_Record::Integer& id)
{
  std::stringstream buffer;
  buffer << "accent.";
  buffer << id;
  return buffer.str ();
}

void
gams::variables::Accent::init_variable_settings ()
{
  // keep certain varaible changes as local only
  Madara::Knowledge_Engine::Knowledge_Update_Settings keep_local (true);
  command.set_settings (keep_local);
  command_args.set_settings (keep_local);
}
