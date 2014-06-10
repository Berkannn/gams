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
 * 3. The names Carnegie Mellon University, "SEI and/or Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN AS-IS BASIS. CARNEGIE MELLON
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
 * @file Local_Pheremone_Area_Coverage.h
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 **/

#ifndef _GAMS_ALGORITHMS_PHEREMONE_AREA_COVERAGE_H_
#define _GAMS_ALGORITHMS_PHEREMONE_AREA_COVERAGE_H_

#include "gams/algorithms/Base_Algorithm.h"
#include "gams/utility/Search_Area.h"
#include "gams/utility/GPS_Position.h"

namespace gams
{
  namespace algorithms
  {
    class GAMS_Export Local_Pheremone_Area_Coverage : public Base
    {
    public:
      /**
       * Constructor
       * @param  knowledge    the context containing variables and values
       * @param  platform     the underlying platform the algorithm will use
       * @param  sensors      map of sensor names to sensor information
       * @param  self         self-referencing variables
       **/
      Local_Pheremone_Area_Coverage (
        const Madara::Knowledge_Record& search_id, 
        Madara::Knowledge_Engine::Knowledge_Base * knowledge = 0,
        platforms::Base * platform = 0, variables::Sensors * sensors = 0,
        variables::Self * self = 0);

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Local_Pheremone_Area_Coverage & rhs);
      
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
      /// generate new next position
      void generate_new_position ();

      /// next position
      utility::GPS_Position next_position_;

      /// Search Area to cover
      utility::Search_Area search_area_;

      /// digital pheremone
      variables::Sensor pheremone_;

      /// number of executions of this algorithm
      unsigned int executions_;
    };
  } // namespace algorithms
} // namespace gams

#endif //_GAMS_ALGORITHMS_PHEREMONE_AREA_COVERAGE_H_