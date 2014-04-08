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
 * @file Algorithm_Factory.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the algorithm factory
 **/

#ifndef   _GAMS_ALGORITHM_FACTORY_H_
#define   _GAMS_ALGORITHM_FACTORY_H_

#include "gams/algorithms/Base_Algorithm.h"
#include "gams/variables/Platform.h"
#include "gams/variables/Self.h"
#include "gams/variables/Sensor.h"
#include "gams/variables/Device.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

namespace gams
{
  namespace algorithms
  {
    class GAMS_Export Factory
    {
    public:
      /**
       * Constructor
       * @param  knowledge  the knowledge base
       * @param  sensors    variables for all sensors
       * @param  platform   the current platform
       * @param  self       self-referencing variables
       * @param  devices    devices of the swarm
       **/
      Factory (Madara::Knowledge_Engine::Knowledge_Base * knowledge = 0,
               variables::Sensors * sensors = 0,
               platforms::Base * platform = 0,
               variables::Self * self = 0,
               variables::Devices * devices = 0);

      /**
       * Destructor
       **/
      ~Factory ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Factory & rhs);

      /**
       * Creates a platform
       * @param  type   type of platform to create
       * @return  the new platform
       **/
      Base * create (const std::string & type);
      
      /**
       * Sets the knowledge base
       * @param  knowledge    the knowledge base to use
       **/
      void set_knowledge (Madara::Knowledge_Engine::Knowledge_Base * knowledge);
      
      /**
       * Sets the map of sensor names to sensor information
       * @param  sensors      map of sensor names to sensor information
       **/
      void set_sensors (variables::Sensors * sensors);
      
      /**
       * Sets the map of platform names to platform information
       * @param  algorithms   map of platform names to platform information
       **/
      void set_platform (platforms::Base * platform);
      
      /**
       * Sets self-referencing variables
       * @param  self       self-referencing variables
       **/
      void set_self (variables::Self * self);
      
      /**
       * Sets list of devices participating in swarm
       * @param  devices    devices in the swarm
       **/
      void set_devices (variables::Devices * devices);
      
    private:

      /// knowledge base containing variables
      Madara::Knowledge_Engine::Knowledge_Base * knowledge_;

      /// sensor variables
      variables::Sensors * sensors_;

      /// platform variables
      platforms::Base * platform_;

      /// self-referencing variables
      variables::Self * self_;

      /// list of devices participating in the swarm
      variables::Devices * devices_;
    };
  }
}

#endif // _GAMS_ALGORITHM_FACTORY_H_