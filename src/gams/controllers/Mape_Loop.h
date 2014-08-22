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
 **/

/**
 * @file Mape_Loop.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the MAPE loop used to autonomously
 * control a UAS node.
 **/

#ifndef   _GAMS_LOOP_H_
#define   _GAMS_LOOP_H_

#include "gams/GAMS_Export.h"
#include "gams/variables/Device.h"
#include "gams/variables/Swarm.h"
#include "gams/variables/Self.h"
#include "gams/variables/Sensor.h"
#include "gams/algorithms/Base_Algorithm.h"
#include "gams/platforms/Base_Platform.h"
#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/knowledge_engine/containers/Double.h"
#include "madara/knowledge_engine/containers/String.h"
#include "madara/knowledge_engine/containers/Double_Vector.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

namespace gams
{
  namespace controllers
  {
    class GAMS_Export Mape_Loop
    {
    public:
      /**
       * Constructor
       * @param   knowledge   The knowledge base to reference and mutate
       **/
      Mape_Loop (Madara::Knowledge_Engine::Knowledge_Base & knowledge);

      /**
       * Destructor
       **/
      ~Mape_Loop ();

      /**
       * Defines the MAPE loop
       **/
      void define_mape (const std::string & loop =
        "monitor (); analyze (); plan (); execute ()");
      
      /**
       * Defines the monitor function (the M of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       * @param  func   the function to call
       **/
      void define_monitor (
        Madara::Knowledge_Record (*func) (
          Madara::Knowledge_Engine::Function_Arguments &,
          Madara::Knowledge_Engine::Variables &));
      
      /**
       * Defines the analyze function (the A of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       * @param  func   the function to call
       **/
      void define_analyze (
        Madara::Knowledge_Record (*func) (
          Madara::Knowledge_Engine::Function_Arguments &,
          Madara::Knowledge_Engine::Variables &));
      
      /**
       * Defines the plan function (the P of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       * @param  func   the function to call
       **/
      void define_plan (
        Madara::Knowledge_Record (*func) (
          Madara::Knowledge_Engine::Function_Arguments &,
          Madara::Knowledge_Engine::Variables &));
      
      /**
       * Defines the execute function (the E of MAPE). This function should
       * return a 0 unless the MAPE loop should stop.
       * @param  func   the function to call
       **/
      void define_execute (
        Madara::Knowledge_Record (*func) (
          Madara::Knowledge_Engine::Function_Arguments &,
          Madara::Knowledge_Engine::Variables &));

      /**
       * Initializes global variable containers
       * @param   knowledge  the knowledge base to reference
       * @param   id         node identifier
       * @param   processes  processes
       **/
      void init_vars (Madara::Knowledge_Engine::Knowledge_Base & knowledge,
        const Madara::Knowledge_Record::Integer & id = 0,
        const Madara::Knowledge_Record::Integer & processes = -1);

      /**
       * Runs one iteration of the MAPE loop
       * @param  period       time between executions of the loop
       * @param  max_runtime  maximum runtime within the MAPE loop
       * @return  the result of the MAPE loop
       **/
      Madara::Knowledge_Record run (double period = 0.5,
        double max_runtime = -1);

    protected:

      /// Containers for device-related variables
      variables::Devices devices_;

      /// knowledge base
      Madara::Knowledge_Engine::Knowledge_Base & knowledge_;

      /// Compiled MAPE Mape_Loop
      Madara::Knowledge_Engine::Compiled_Expression mape_loop_;

      /// Containers for self-referencing variables
      variables::Self self_;

      /// Containers for sensor information
      variables::Sensors sensors_;

      /// Containers for swarm-related variables
      variables::Swarm swarm_;
    };
  }
}

#endif // _GAMS_LOOP_H_
