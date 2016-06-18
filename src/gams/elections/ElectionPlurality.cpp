/**
* Copyright (c) 2016 Carnegie Mellon University. All Rights Reserved.
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

#include <algorithm>

#include "ElectionPlurality.h"
#include "gams/loggers/GlobalLogger.h"
#include "madara/knowledge/containers/Integer.h"

namespace knowledge = madara::knowledge;
namespace containers = knowledge::containers;

gams::elections::ElectionPluralityFactory::ElectionPluralityFactory ()
{
}

gams::elections::ElectionPluralityFactory::~ElectionPluralityFactory ()
{
}

gams::elections::ElectionBase *
gams::elections::ElectionPluralityFactory::create (
const std::string & election_prefix,
const std::string & agent_prefix,
madara::knowledge::KnowledgeBase * knowledge)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::elections::ElectionPluralityFactory:" \
    " creating election from %s\n", election_prefix.c_str ());

  return new ElectionPlurality (election_prefix, agent_prefix, knowledge);
}

gams::elections::ElectionPlurality::ElectionPlurality (
  const std::string & election_prefix,
  const std::string & agent_prefix,
  madara::knowledge::KnowledgeBase * knowledge)
  : ElectionBase (election_prefix, agent_prefix, knowledge)
{
}

/**
* Constructor
**/
gams::elections::ElectionPlurality::~ElectionPlurality ()
{

}

gams::elections::CandidateList
gams::elections::ElectionPlurality::get_leaders (int num_leaders)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::elections::ElectionPlurality:get_leaders" \
    " getting leaders from %s\n", election_prefix_.c_str ());

  CandidateList leaders;

  if (knowledge_)
  {
    madara::knowledge::ContextGuard guard (*knowledge_);

    std::vector <std::string> keys;
    votes_.keys (keys);
  }

  return leaders;
}

std::string
gams::elections::ElectionPlurality::get_leader (void)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::elections::ElectionPlurality:get_leader" \
    " getting leader from %s\n", election_prefix_.c_str ());

  std::string leader;
  CandidateList leaders = get_leaders ();

  if (leaders.size () > 0)
  {
    leader = leaders[0];
  }

  return leader;
}
