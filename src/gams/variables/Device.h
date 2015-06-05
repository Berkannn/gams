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
 * @file Device.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the device-prefixed MADARA variables
 **/

#ifndef   _GAMS_VARIABLES_DEVICES_H_
#define   _GAMS_VARIABLES_DEVICES_H_

#include <vector>
#include <string>

#include "gams/GAMS_Export.h"
#include "madara/knowledge_engine/containers/Integer.h"
#include "madara/knowledge_engine/containers/Double.h"
#include "madara/knowledge_engine/containers/String.h"
#include "madara/knowledge_engine/containers/Native_Double_Vector.h"
#include "madara/knowledge_engine/containers/Vector.h"
#include "madara/knowledge_engine/Knowledge_Base.h"
#include "Accent_Status.h"

namespace gams
{
  namespace variables
  {
    class GAMS_Export Device
    {
    public:
      /**
       * Constructor
       **/
      Device ();

      /**
       * Destructor
       **/
      ~Device ();

      /**
       * Assignment operator
       * @param  device   device to copy
       **/
      void operator= (const Device & device);

      /**
       * Initializes variable containers
       * @param   knowledge  the variable context
       * @param   id         node identifier
       **/
      void init_vars (Madara::Knowledge_Engine::Knowledge_Base & knowledge,
        const Madara::Knowledge_Record::Integer& id);
      
      /**
       * Initializes variable containers
       * @param   knowledge  the variable context
       * @param   id         node identifier
       **/
      void init_vars (Madara::Knowledge_Engine::Variables & knowledge,
        const Madara::Knowledge_Record::Integer& id);
      
      /// the battery indicator for this device
      Madara::Knowledge_Engine::Containers::Integer battery_remaining;

      /// indicator for whether or not the device is busy with a mission
      Madara::Knowledge_Engine::Containers::Integer bridge_id;

      /// device specific command
      Madara::Knowledge_Engine::Containers::String command;

      /// number of arguments for command
      Madara::Knowledge_Engine::Containers::Vector command_args;
      
      /// device specific command
      Madara::Knowledge_Engine::Containers::String coverage_type;

      /// desired altitude in meters
      Madara::Knowledge_Engine::Containers::Double desired_altitude;

      /// the destination location
      Madara::Knowledge_Engine::Containers::Native_Double_Array dest;
      
      /// the home location
      Madara::Knowledge_Engine::Containers::Native_Double_Array home;

      /// the mobility indicator for this device (true if mobile)
      Madara::Knowledge_Engine::Containers::Integer is_mobile;

      /// the location, usually encoded in GPS, for this device
      Madara::Knowledge_Engine::Containers::Native_Double_Array location;
      
      /// the minimum altitude for this device
      Madara::Knowledge_Engine::Containers::Double min_alt;
      
      /// indicator for next type of area coverage requested (queue like)
      Madara::Knowledge_Engine::Containers::String next_coverage_type;

      /// indicator for next assigned search area id
      Madara::Knowledge_Engine::Containers::Integer search_area_id;

      /// the source location
      Madara::Knowledge_Engine::Containers::Native_Double_Array source;
      
      /// indicator for temperature
      Madara::Knowledge_Engine::Containers::Double temperature;

      /// container for accents
      Accent_Statuses accents;

    protected:
      /**
       * Create device/local device name
       * @param id  id of device as string
       * @return device variable name
       */
      static std::string make_variable_name (
        const Madara::Knowledge_Record::Integer& id);

      /**
       * Set variable settings
       */
      void init_variable_settings ();
    };

    /**
     * An array of devices
     **/
    typedef std::vector <Device>   Devices;
    
    /**
      * Initializes a self containers
      * @param   variables  the variables to initialize
      * @param   knowledge  the knowledge base that houses the variables
      * @param   processes  the number of processes in the device swarm
      **/
    GAMS_Export void init_vars (Devices & variables,
      Madara::Knowledge_Engine::Knowledge_Base & knowledge,
      const Madara::Knowledge_Record::Integer& processes);
  }
}

#endif // _GAMS_VARIABLES_DEVICES_H_
