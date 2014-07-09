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
 * @file Drone_RK.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the Drone-RK platform class
 **/

#ifndef   _GAMS_PLATFORM_DRONE_RK_H_
#define   _GAMS_PLATFORM_DRONE_RK_H_

#include "gams/variables/Self.h"
#include "gams/variables/Sensor.h"
#include "gams/variables/Platform.h"
#include "gams/platforms/Base_Platform.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

namespace gams
{
  namespace platforms
  {
    class GAMS_Export Drone_RK : public Base
    {
    public:
      /**
       * Constructor
       * @param  knowledge  knowledge base
       * @param  sensors    map of sensor names to sensor information
       * @param  platforms  map of platform names to platform information
       * @param  self       device variables that describe self state
       **/
      Drone_RK (
        Madara::Knowledge_Engine::Knowledge_Base & knowledge,
        variables::Sensors * sensors,
        variables::Platforms & platforms,
        variables::Self & self);

      /**
       * Destructor
       **/
      ~Drone_RK ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Drone_RK & rhs);
      
      /**
       * Instructs the device to return home
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int home (void);
      
      /**
       * Instructs the platform to land
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int land (void);
      
      /**
       * Moves the platform to a position
       * @param   position  the coordinate to move to
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int move (const utility::Position & position);
      
      /**
       * Polls the sensor environment for useful information
       * @return number of sensors updated/used
       **/
      virtual int sense (void);
      
      /**
       * Instructs the platform to take off
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int takeoff (void);
      
      /**
       * Analyzes platform information
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int analyze (void);

      /**
       * Get the position accuracy in meters
       * @return position accuracy
       **/
      virtual double get_gps_accuracy () const;

      /**
       * Fills a list of sensor names with sensors available on the platform
       **/
      virtual void get_sensors (variables::Sensor_Names & sensors);
      
      /**
       * Obtains the current position
       * @param  position  after the call, filled with the current position
       **/
      virtual void get_position (utility::Position & position);

    protected:

      /// current position
      utility::Position position_;

      /// flag for drone being airborne
      bool airborne_;
    };
  }
}

#endif // _GAMS_PLATFORM_DRONE_RK_H_
