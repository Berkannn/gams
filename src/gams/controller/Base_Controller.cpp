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

#include "Base_Controller.h"
#include "ace/High_Res_Timer.h"
#include "ace/OS_NS_sys_time.h"
#include "madara/utility/Utility.h"
#include "gams/platforms/Platform_Factory.h"
#include "gams/algorithms/Algorithm_Factory.h"

typedef  Madara::Knowledge_Record::Integer  Integer;

gams::controller::Base::Base (
  Madara::Knowledge_Engine::Knowledge_Base & knowledge)
  : knowledge_ (knowledge), platform_ (0), algorithm_ (0)
{
}

gams::controller::Base::~Base ()
{

}

void
gams::controller::Base::init_vars (
  const Integer & id,
  const Integer & processes)
{
  // initialize the devices, swarm, and self variables
  variables::init_vars (devices_, knowledge_, processes);
  swarm_.init_vars (knowledge_);
  self_.init_vars (knowledge_, id);
}

void
gams::controller::Base::init_platform (
  const std::string & platform)
{
  // initialize the platform
  
  if (platform == "")
  {
    std::cerr << "Platform is empty. Here is a list of supported platforms.";
    std::cerr << "\n\n";
    std::cerr << "SUPPORTED PLATFORMS:\n";
    std::cerr << "  drone-rk\n";
    std::cerr << "  vrep\n";
  }
  else
  {
    delete platform_;
    platforms::Factory factory (&knowledge_, &sensors_, &platforms_, &self_);
    platform_ = factory.create (platform);

    if (algorithm_)
      algorithm_->set_platform (platform_);
  }
}

void
gams::controller::Base::init_algorithm (
  const std::string & algorithm)
{
  // initialize the algorithm
  
  if (algorithm == "")
  {
    std::cerr << "Algorithm is empty. Here is a list of supported algorithms.";
    std::cerr << "\n\n";
    std::cerr << "SUPPORTED ALGORITHMS:\n";
    std::cerr << "  bridge | bridging\n";
    std::cerr << "  random area coverage\n";
    std::cerr << "  priotized area coverage\n";
  }
  else
  {
    delete algorithm_;
    algorithms::Factory factory (&knowledge_, &sensors_,
      platform_, &self_, &devices_);
    algorithm_ = factory.create (algorithm);
  }
}

int
gams::controller::Base::monitor (void)
{
  return platform_->sense ();
}

int
gams::controller::Base::analyze (void)
{
  int return_value (0);

  if (platform_)
    return_value |= platform_->analyze ();

  if (algorithm_)
    return_value |= algorithm_->analyze ();

  return return_value;
}

int
gams::controller::Base::plan (void)
{
  int return_value (0);
  
  if (algorithm_)
    return_value |= algorithm_->plan ();

  return return_value;
}

int
gams::controller::Base::execute (void)
{
  int return_value (0);
  
  if (algorithm_)
    return_value |= algorithm_->execute ();

  return return_value;
}

int
gams::controller::Base::run (double period, double max_runtime)
{
  // return value
  int return_value (0);

  // loop every period until a max run time has been reached
  ACE_Time_Value current = ACE_OS::gettimeofday ();  
  ACE_Time_Value max_wait, sleep_time, next_epoch;
  ACE_Time_Value poll_frequency, last = current;
  
  if (period > 0)
  {
    max_wait.set (max_runtime);
    max_wait = current + max_wait;
    
    poll_frequency.set (period);
    next_epoch = current + poll_frequency;

    while (current < max_wait)
    {
      // return value should be last return value of mape loop
      return_value = 0;
      return_value |= monitor ();
      return_value |= analyze ();
      return_value |= plan ();
      return_value |= execute ();
    
      knowledge_.send_modifieds ();

      current = ACE_OS::gettimeofday ();

      if (current < next_epoch)
      {
        Madara::Utility::sleep (next_epoch - current);  
      }

      next_epoch = next_epoch + poll_frequency;
    }
  }

  return return_value;
}

