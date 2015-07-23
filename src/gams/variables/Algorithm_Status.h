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

/**
 * @file Self.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the self-referencing MADARA variables
 **/

#ifndef   _GAMS_VARIABLES_ALGORITHM_H_
#define   _GAMS_VARIABLES_ALGORITHM_H_

#include <string>
#include <vector>
#include <map>

#include "gams/GAMS_Export.h"
#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "Device.h"


namespace gams
{
  namespace variables
  {
    class GAMS_Export Algorithm_Status
    {
    public:
      /**
       * Constructor
       **/
      Algorithm_Status ();

      /**
       * Destructor
       **/
      ~Algorithm_Status ();

      /**
       * Assignment operator
       * @param  rhs   value to copy
       **/
      void operator= (const Algorithm_Status & rhs);

      /**
       * Initializes variable containers
       * @param   knowledge  the knowledge base that houses the variables
       * @param   new_name   the name of the algorithm
       * @param   i          the device id
       **/
      void init_vars (Madara::Knowledge_Engine::Knowledge_Base & knowledge,
        const std::string & new_name, int i);
      
      /**
       * Initializes variable containers
       * @param   knowledge  the variable context
       * @param   new_name   the name of the algorithm
       * @param   i          the device id
       **/
      void init_vars (Madara::Knowledge_Engine::Variables & knowledge,
        const std::string & new_name, int i);

      /**
       * Initialize variable values
       */
      void init_variable_values ();

      /// the device id
      int id;

      /// the name of the algorithm
      std::string name;
      
      /// the device-specific variables
      //Device device;
      
      /// status flag for deadlocked
      Madara::Knowledge_Engine::Containers::Integer deadlocked;

      /// status flag for failed
      Madara::Knowledge_Engine::Containers::Integer failed;

      /// status flag for ok
      Madara::Knowledge_Engine::Containers::Integer ok;
      
      /// status flag for ok
      Madara::Knowledge_Engine::Containers::Integer paused;
      
      /// status flag for unknown
      Madara::Knowledge_Engine::Containers::Integer unknown;

      /// status flag for waiting
      Madara::Knowledge_Engine::Containers::Integer waiting;

      /// status flag for finished
      Madara::Knowledge_Engine::Containers::Integer finished;

    protected:
      /**
       * Get prefix for variables
       */
      std::string make_variable_prefix () const;
    };
    
    /// deprecated typedef. Please use Algorithm_Status instead.
    typedef Algorithm_Status Algorithm;

    /// a map of sensor names to the sensor information
    typedef  std::map <std::string, Algorithm_Status>   Algorithms;
    
    /// a typedef for convenience and legibility
    typedef  Algorithms   Algorithm_Statuses;

    /// a list of sensor names
    typedef  std::vector <std::string>        Algorithm_Names;
  }
}

#endif // _GAMS_VARIABLES_ALGORITHM_H_
