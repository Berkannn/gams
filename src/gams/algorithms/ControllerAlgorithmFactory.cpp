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

#include "madara/utility/Utility.h"

#include "gams/loggers/GlobalLogger.h"

#include "gams/algorithms/ControllerAlgorithmFactory.h"
#include "gams/algorithms/Land.h"
#include "gams/algorithms/Move.h"
#include "gams/algorithms/DebugAlgorithm.h"
#include "gams/algorithms/NullAlgorithm.h"
#include "gams/algorithms/FormationFlying.h"
#include "gams/algorithms/FormationCoverage.h"
#include "gams/algorithms/FormationSync.h"
#include "gams/algorithms/ZoneCoverage.h"
#include "gams/algorithms/Takeoff.h"
#include "gams/algorithms/Follow.h"
#include "gams/algorithms/MessageProfiling.h"
#include "gams/algorithms/Executive.h"
#include "gams/algorithms/Wait.h"
#include "gams/algorithms/PerformanceProfiling.h"
#include "gams/algorithms/GroupBarrier.h"
#include "gams/algorithms/PatrollingGame.h"

#include "gams/algorithms/area_coverage/UniformRandomAreaCoverage.h"
#include "gams/algorithms/area_coverage/UniformRandomEdgeCoverage.h"
#include "gams/algorithms/area_coverage/PriorityWeightedRandomAreaCoverage.h"
#include "gams/algorithms/area_coverage/LocalPheremoneAreaCoverage.h"
#include "gams/algorithms/area_coverage/SnakeAreaCoverage.h"
#include "gams/algorithms/area_coverage/MinTimeAreaCoverage.h"
#include "gams/algorithms/area_coverage/PrioritizedMinTimeAreaCoverage.h"
#include "gams/algorithms/area_coverage/PerimeterPatrol.h"
#include "gams/algorithms/area_coverage/WaypointsCoverage.h"

#include "gams/loggers/GlobalLogger.h"

#include <iostream>

namespace algorithms = gams::algorithms;
namespace variables = gams::variables;
namespace platforms = gams::platforms;

algorithms::ControllerAlgorithmFactory::ControllerAlgorithmFactory (
  madara::knowledge::KnowledgeBase * knowledge,
  variables::Sensors * sensors,
  platforms::BasePlatform * platform,
  variables::Self * self,
  variables::Agents * agents)
: agents_ (agents), knowledge_ (knowledge), platform_ (platform),
  self_ (self), sensors_ (sensors)
{
  initialize_default_mappings ();
}

algorithms::ControllerAlgorithmFactory::~ControllerAlgorithmFactory ()
{
}

void
algorithms::ControllerAlgorithmFactory::add (
  const std::vector <std::string> & aliases,
  AlgorithmFactory * factory)
{
  for (size_t i = 0; i < aliases.size (); ++i)
  {
    std::string alias (aliases[i]);
    madara::utility::lower (alias);

    factory->set_agents (agents_);
    factory->set_knowledge (knowledge_);
    factory->set_platform (platform_);
    factory->set_self (self_);
    factory->set_sensors (sensors_);

    factory_map_[alias] = factory;
  }
}

void algorithms::ControllerAlgorithmFactory::initialize_default_mappings (
  void)
{
  std::vector <std::string> aliases;
  
  // the debug algorithm
  aliases.resize (3);
  aliases[0] = "debug";
  aliases[1] = "print";
  aliases[2] = "printer";

  add (aliases, new DebugAlgorithmFactory ());

  // the follow algorithm
  aliases.resize (1);
  aliases[0] = "follow";

  add (aliases, new FollowFactory ());

  // the formation coverage algorithm
  aliases.resize (1);
  aliases[0] = "formation coverage";

  add (aliases, new FormationCoverageFactory ());

  // the formation algorithm
  aliases.resize (1);
  aliases[0] = "formation";

  add (aliases, new FormationFlyingFactory ());
  
  // the move algorithm
  aliases.resize (1);
  aliases[0] = "move";

  add (aliases, new MoveFactory ());
  
  // the null algorithm
  aliases.resize (1);
  aliases[0] = "null";

  add (aliases, new NullAlgorithmFactory ());
  
  // the takeoff algorithm
  aliases.resize (1);
  aliases[0] = "takeoff";

  add (aliases, new TakeoffFactory ());
  
  // the local pheromone coverage algorithm
  aliases.resize (1);
  aliases[0] = "local pheremone";

  add (aliases, new area_coverage::LocalPheremoneAreaCoverageFactory ());
  
  // the minimum time coverage algorithm
  aliases.resize (2);
  aliases[0] = "min time";
  aliases[1] = "mtac";

  add (aliases, new area_coverage::MinTimeAreaCoverageFactory ());
  
  // the minimum time coverage algorithm
  aliases.resize (2);
  aliases[0] = "perimeter patrol";
  aliases[1] = "ppac";

  add (aliases, new area_coverage::PerimeterPatrolFactory ());
  
  // the priority-weighted coverage algorithm
  aliases.resize (2);
  aliases[0] = "priority weighted random area coverage";
  aliases[1] = "pwrac";

  add (aliases, new area_coverage::PriorityWeightedRandomAreaCoverageFactory ());
  
  // the snake area coverage algorithm
  aliases.resize (2);
  aliases[0] = "snake";
  aliases[1] = "sac";

  add (aliases, new area_coverage::SnakeAreaCoverageFactory ());
  
  // the uniform random area coverage algorithm
  aliases.resize (2);
  aliases[0] = "uniform random area coverage";
  aliases[1] = "urac";

  add (aliases, new area_coverage::UniformRandomAreaCoverageFactory ());
  
  // the uniform random edge coverage algorithm
  aliases.resize (2);
  aliases[0] = "uniform random edge coverage";
  aliases[1] = "urec";

  add (aliases, new area_coverage::UniformRandomEdgeCoverageFactory ());

  // the prioritized min time area coverage
  aliases.resize (2);
  aliases[0] = "prioritized min time area coverage";
  aliases[1] = "pmtac";

  add (aliases, new area_coverage::PrioritizedMinTimeAreaCoverageFactory ());

  // the message profiling algorithm
  aliases.resize (1);
  aliases[0] = "message profiling";

  add (aliases, new MessageProfilingFactory ());

  // the waypoints coverage algorithm
  aliases.resize (1);
  aliases[0] = "waypoints";

  add (aliases, new area_coverage::WaypointsCoverageFactory ());

  // the executive
  aliases.resize (1);
  aliases[0] = "executive";

  add (aliases, new ExecutiveFactory ());

  // the wait
  aliases.resize (1);
  aliases[0] = "wait";

  add (aliases, new WaitFactory ());

  // the performance profiler
  aliases.resize (1);
  aliases[0] = "performance profiling";

  add (aliases, new PerformanceProfilingFactory ());

  // the performance profiler
  aliases.resize (1);
  aliases[0] = "formation sync";

  add (aliases, new FormationSyncFactory ());

  // the performance profiler
  aliases.resize (1);
  aliases[0] = "barrier";

  add (aliases, new GroupBarrierFactory ());

  // zone coverage
  aliases.resize (1);
  aliases[0] = "zone coverage";

  add (aliases, new ZoneCoverageFactory ());
  
  // Testing TestingDemo
  aliases.resize(1);
  aliases[0] = "patrolling game";
  
  add(aliases, new PatrollingGameFactory ());

}

algorithms::BaseAlgorithm *
algorithms::ControllerAlgorithmFactory::create (
  const std::string & type,
  const madara::knowledge::KnowledgeMap & args)
{
  algorithms::BaseAlgorithm * result = 0;

  // the user is going to expect this kind of error to be printed immediately
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::ControllerAlgorithmFactory::create:" \
    " creating \"%s\" algorithm.\n", type.c_str ());

  
  if (type != "")
  {
    FactoryMap::iterator it = factory_map_.find (type);
    if (it != factory_map_.end ())
    {
      result = it->second->create (args, knowledge_, platform_,
        sensors_, self_, agents_);
    }
    else
    {
      // the user is going to expect this kind of error to be printed immediately
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_ALWAYS,
        "gams::algorithms::ControllerAlgorithmFactory::create:" \
        " could not find \"%s\" algorithm.\n", type.c_str ());
    }
  }

  return result;
}

void
algorithms::ControllerAlgorithmFactory::set_agents (
  variables::Agents * agents)
{
  agents_ = agents;
}

void
algorithms::ControllerAlgorithmFactory::set_knowledge (
  madara::knowledge::KnowledgeBase * knowledge)
{
  knowledge_ = knowledge;
}

void
algorithms::ControllerAlgorithmFactory::set_platform (
  platforms::BasePlatform * platform)
{
  platform_ = platform;
}

void
algorithms::ControllerAlgorithmFactory::set_self (
  variables::Self * self)
{
  self_ = self;
}

void
algorithms::ControllerAlgorithmFactory::set_sensors (
  variables::Sensors * sensors)
{
  sensors_ = sensors;
}
