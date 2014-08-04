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

/**
 * @file Base_Controller.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the preferred base controller for a GAMS control loop
 **/

#ifndef   _GAMS_BASE_CONTROLLER_H_
#define   _GAMS_BASE_CONTROLLER_H_

#include "gams/GAMS_Export.h"
#include "gams/variables/Device.h"
#include "gams/variables/Swarm.h"
#include "gams/variables/Self.h"
#include "gams/variables/Sensor.h"
#include "gams/variables/Algorithm.h"
#include "gams/variables/Platform.h"
#include "gams/algorithms/Base_Algorithm.h"
#include "gams/platforms/Base_Platform.h"

#ifdef _GAMS_JAVA_
#include <jni.h>
#endif

namespace gams
{
  namespace controllers
  {
    class GAMS_Export Base
    {
    public:
      /**
       * Constructor
       * @param   knowledge   The knowledge base to reference and mutate
       **/
      Base (Madara::Knowledge_Engine::Knowledge_Base & knowledge);

      /**
       * Destructor
       **/
      ~Base ();

      /**
       * Defines the monitor function (the M of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       **/
      virtual int monitor (void);

      /**
       * Analyzes the system to determine if platform or algorithm changes
       * are necessary. This function should
       * return a 0 unless the MAPE loop should stop.
       **/
      virtual int system_analyze (void);

      /**
       * Defines the analyze function (the A of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       **/
      virtual int analyze (void);

      /**
       * Defines the plan function (the P of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       **/
      virtual int plan (void);

      /**
       * Defines the execute function (the E of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       **/
      virtual int execute (void);
     
      /**
       * Runs one iteration of the MAPE loop
       * @param  period       time between executions of the loop
       * @param  max_runtime  maximum runtime within the MAPE loop
       * @return  the result of the MAPE loop
       **/
      int run (double period = 0.5, double max_runtime = -1);

      /**
       * Adds an accent algorithm
       * @param  algorithm   the name of the accent algorithm to add
       * @param  args        vector of knowledge record arguments
       **/
      void init_accent (const std::string & algorithm,
        const Madara::Knowledge_Vector & args = Madara::Knowledge_Vector ());

      /**
       * Clears all accent algorithms
       **/
      void clear_accents (void);

      /**
       * Initializes an algorithm
       * @param  algorithm   the name of the algorithm to run
       * @param  args        vector of knowledge record arguments
       **/
      void init_algorithm (const std::string & algorithm,
        const Madara::Knowledge_Vector & args = Madara::Knowledge_Vector ());
 
      /**
       * Initializes the controller with a user-provided algorithm. This
       * algorithm's memory will be maintained by the controller. DO NOT
       * DELETE THIS POINTER.
       * @param  algorithm   the algorithm to use
       **/
      void init_algorithm (algorithms::Base * algorithm);

      /**
       * Initializes the platform
       * @param  platform   the name of the platform the controller is using
       * @param  args        vector of knowledge record arguments
       **/
      void init_platform (const std::string & platform,
        const Madara::Knowledge_Vector & args = Madara::Knowledge_Vector ());
       
      /**
       * Initializes the controller with a user-provided platform. This
       * platform's memory will be maintained by the controller. DO NOT
       * DELETE THIS POINTER.
       * @param  platform   the platform to use
       **/
      void init_platform (platforms::Base * platform);
           
#ifdef _GAMS_JAVA_
      /**
       * Initializes a Java-based algorithm
       * @param  algorithm  the java-based algorithm to use
       **/
      void init_algorithm (jobject algorithm);
      
      /**
       * Initializes a Java-based platform
       * @param  platform  the java-based platform to use
       **/
      void init_platform (jobject platform);
#endif

      /**
       * Initializes global variable containers
       * @param   id         node identifier
       * @param   processes  processes
       **/
      void init_vars (const Madara::Knowledge_Record::Integer & id = 0,
        const Madara::Knowledge_Record::Integer & processes = -1);
      
      /**
       * Initializes containers and knowledge base in a platform
       * This is usually the first thing a developer should do with
       * a user-defined platform.
       * @param   platform   the platform to initialize
       **/
      void init_vars (platforms::Base & platform);
      
      /**
       * Initializes containers and knowledge base in an algorithm.
       * This is usually the first thing a developer should do with
       * a user-defined algorithm.
       * @param   algorithm   the algorithm to initialize
       **/
      void init_vars (algorithms::Base & algorithm);

    protected:

      /// accents on the primary algorithm
      algorithms::Algorithms accents_;

      /// algorithm to perform
      algorithms::Base * algorithm_;

      /// Containers for algorithm information
      variables::Algorithms algorithms_;
      
      /// Containers for device-related variables
      variables::Devices devices_;

      /// knowledge base
      Madara::Knowledge_Engine::Knowledge_Base & knowledge_;

      /// Platform on which the controller is running
      platforms::Base * platform_;

      /// Containers for platform information
      variables::Platforms platforms_;

      /// Containers for self-referencing variables
      variables::Self self_;

      /// Containers for sensor information
      variables::Sensors sensors_;

      /// Containers for swarm-related variables
      variables::Swarm swarm_;
    };
  }
}

#endif // _GAMS_BASE_CONTROLLER_H_
