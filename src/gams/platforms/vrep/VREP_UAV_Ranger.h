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
 * @file VREP_UAV_Ranger.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the VREP_UAV_Ranger simulator uav class
 **/

#ifndef   _GAMS_PLATFORM_VREP_UAV_RANGER_H_
#define   _GAMS_PLATFORM_VREP_UAV_RANGER_H_

#include "gams/platforms/Platform_Factory.h"
#include "gams/platforms/vrep/VREP_Base.h"
#include "gams/platforms/vrep/VREP_Aerial_Base.h"
#include "gams/variables/Self.h"
#include "gams/variables/Sensor.h"
#include "gams/variables/Platform_Status.h"
#include "gams/utility/GPS_Position.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

extern "C" {
#include "extApi.h"
}


#ifdef _GAMS_VREP_

namespace gams
{
  namespace platforms
  {
    class GAMS_Export Has_Range_Sensor
    {
    public:
      virtual double get_range() const = 0;
    };

    class GAMS_Export VREP_UAV_Ranger : public VREP_Aerial_Base, public Has_Range_Sensor
    {
    protected:
      /**
       * Constructor
       * @param  knowledge  knowledge base
       * @param  sensors    map of sensor names to sensor information
       * @param  platforms  map of platform names to platform information
       * @param  self       device variables that describe self state
       **/
      VREP_UAV_Ranger (
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self);
    public:
      /**
       * Gets the unique identifier of the platform. This should be an
       * alphanumeric identifier that can be used as part of a MADARA
       * variable (e.g. vrep_ant, autonomous_snake, etc.)
       **/
      virtual std::string get_id () const;

      /**
       * Gets the name of the platform
       **/
      virtual std::string get_name () const;

      /**
       * Gets the filename of the VREP model (includ e.ttm extension)
       * Assumed to be stored in ${GAMS_ROOT}/resources/vrep/
       **/
      virtual std::string get_model_filename () const;

      virtual double get_range () const;

    protected:
      /**
       * Get node target handle
       */
      virtual void get_target_handle ();

      void get_laser_sensor_handle ();

      simxInt laser_sensor_;
    public:
      friend class VREP_UAV_Ranger_Factory;
    }; // class VREP_UAV_Ranger

    /**
     * A factory class for creating VREP UAV platforms
     **/
    class GAMS_Export VREP_UAV_Ranger_Factory : public VREP_Aerial_Base_Factory
    {
      virtual VREP_UAV_Ranger * create (
        const Madara::Knowledge_Vector & args,
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self);

    protected:
      /**
       * Override to construct a new object of the derived type
       **/
      virtual VREP_UAV_Ranger * make_new (
        const Madara::Knowledge_Vector & /*args*/,
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self);
    };
  } // namespace platform
} // namespace gams

#endif // _GAMS_VREP_

#endif // _GAMS_PLATFORM_VREP_UAV_Ranger_H_
