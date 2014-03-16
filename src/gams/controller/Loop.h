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
 * @file Loop.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the MAPE loop used to autonomously
 * control a UAS node.
 **/

#ifndef   _GAMS_LOOP_H_
#define   _GAMS_LOOP_H_

#include "gams/GAMS_Export.h"
#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/knowledge_engine/containers/Double.h"
#include "madara/knowledge_engine/containers/String.h"
#include "madara/knowledge_engine/containers/Double_Vector.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

namespace gams
{
  namespace controller
  {
    class GAMS_Export Loop
    {
    public:
      /**
       * Constructor
       * @param   knowledge   The knowledge base to reference and mutate
       **/
      Loop (Madara::Knowledge_Engine::Knowledge_Base & knowledge);

      /**
       * Destructor
       **/
      ~Loop ();

      /**
       * Initializes global variable containers
       * @param   id         node identifier
       * @param   processes  processes
       **/
      void init_vars (Madara::Knowledge_Engine::Knowledge_Base & knowledge,
        const Madara::Knowledge_Record::Integer & id,
        const Madara::Knowledge_Record::Integer & processes);

      /**
       * Defines the MAPE loop
       **/
      void define_mape (const std::string & loop =
        "monitor (); analyze (); plan (); execute ()");

      /**
       * Runs one iteration of the MAPE loop
       * @return  the result of the MAPE loop
       **/
      Madara::Knowledge_Record run (void);

    private:

      /// the minimum altitude for this device
      Madara::Knowledge_Engine::Containers::Double min_alt_;

      /// the location, usually encoded in GPS, for this device
      Madara::Knowledge_Engine::Containers::Double_Array location_;

      /// the mobility indicator for this device (true if mobile)
      Madara::Knowledge_Engine::Containers::Integer is_mobile_;

      /// the battery indicator for this device
      Madara::Knowledge_Engine::Containers::Integer battery_remaining_;

      /// indicator for whether or not the device is busy with a mission
      Madara::Knowledge_Engine::Containers::Integer bridge_id_;

      /// indicator for type of area coverage requested of the device
      Madara::Knowledge_Engine::Containers::String coverage_type_;

      /// indicator for next type of area coverage requested (queue like)
      Madara::Knowledge_Engine::Containers::String next_coverage_type_;

      /// indicator for next assigned search area id
      Madara::Knowledge_Engine::Containers::Integer search_area_id_;

      /// indicator for temperature
      Madara::Knowledge_Engine::Containers::Double temperature_;

      /// knowledge base
      Madara::Knowledge_Engine::Knowledge_Base & knowledge_;

      /// Compiled MAPE Loop
      Madara::Knowledge_Engine::Compiled_Expression mape_loop_;

    };
  }
}

#endif // _GAMS_LOOP_H_
