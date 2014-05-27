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
 * @file Formation_Flying.h
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Declaration of Formation_Flying class
 **/

#ifndef   _GAMS_ALGORITHMS_FORMATION_FLYING_H_
#define   _GAMS_ALGORITHMS_FORMATION_FLYING_H_

#include "gams/variables/Sensor.h"
#include "gams/platforms/Base_Platform.h"
#include "gams/variables/Algorithm.h"
#include "gams/variables/Self.h"
#include "gams/algorithms/Base_Algorithm.h"

namespace gams
{
  namespace algorithms
  {
    class GAMS_Export Formation_Flying : public Base
    {
    public:
      /**
       * Constructor
       * @param  knowledge    the context containing variables and values
       * @param  platform     the underlying platform the algorithm will use
       * @param  sensors      map of sensor names to sensor information
       * @param  self         self-referencing variables
       **/
      Formation_Flying (
        const Madara::Knowledge_Record & target,
        const Madara::Knowledge_Record & offset,
        const Madara::Knowledge_Record & destination,
        const Madara::Knowledge_Record & members,
        Madara::Knowledge_Engine::Knowledge_Base * knowledge = 0,
        platforms::Base * platform = 0, variables::Sensors * sensors = 0,
        variables::Self * self = 0);

      /**
       * Destructor
       **/
      ~Formation_Flying ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Formation_Flying & rhs);
      
      /**
       * Analyzes environment, platform, or other information
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int analyze (void);
      
      /**
       * Plans the next execution of the algorithm
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int execute (void);

      /**
       * Plans the next execution of the algorithm
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int plan (void);
      
    protected:
      /// list of sensor names
      variables::Sensor_Names sensor_names_;

      /// next position
      utility::Position next_position_;

      /// do we need to move?
      bool need_to_move_;

      /// destination
      utility::Position destination_;

      /// planar distance formation offsets
      double rho_;

      /// angular formation offsets
      double phi_;

      /// directional angular formation offsets
      double phi_dir_;

      /// altitude formation offsets
      double z_;

      /// number of agents in formation; only head_id_ needs to know this
      unsigned int num_agents_;

      /// head location
      Madara::Knowledge_Engine::Containers::Native_Double_Array head_location_;

      /// head id
      int head_id_;

      /// am i the head?
      bool head_;

      /// am i in formation?
      Madara::Knowledge_Engine::Containers::Integer in_formation_;

      /// are we in formation?
      Madara::Knowledge_Engine::Containers::Integer formation_ready_;

      /// formation wait string
      Madara::Knowledge_Engine::Compiled_Expression compiled_formation_;
    };
  }
}

#endif // _GAMS_ALGORITHMS_FORMATION_FLYING_H_
