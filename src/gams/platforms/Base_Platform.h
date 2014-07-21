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
 * @file Base.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains the definition of the base platform class
 **/

#ifndef   _GAMS_PLATFORM_BASE_H_
#define   _GAMS_PLATFORM_BASE_H_

#include <string>

#include "gams/variables/Self.h"
#include "gams/variables/Sensor.h"
#include "gams/variables/Platform.h"
#include "gams/utility/GPS_Position.h"
#include "madara/knowledge_engine/Knowledge_Base.h"

namespace gams
{
  namespace platforms
  {
    /**
     * Possible platform statuses, as returnable by analyze ()
     **/
    enum Status
    {
      OK  = 1,
      WAITING = 2,
      DEADLOCKED = 4,
      FAILED = 8,
      MOVING = 16,
      REDUCED_SENSING_AVAILABLE = 128,
      REDUCED_MOVEMENT_AVAILABLE = 256,
      COMMUNICATION_AVAILABLE = 512,
      SENSORS_AVAILABLE = 1024,
      MOVEMENT_AVAILABLE = 2048
    };

    class GAMS_Export Base
    {
    public:
      /**
       * Constructor
       * @param  knowledge  context containing variables and values
       * @param  sensors  map of sensor names to sensor information
       **/
      Base (Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Self & self);

      /**
       * Destructor
       **/
      virtual ~Base ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Base & rhs);

      /**
       * Analyzes platform information
       * @return bitmask status of the platform. @see Status.
       **/
      virtual int analyze (void) = 0;

      /**
       * Get the position accuracy in meters
       * @return position accuracy
       **/
      virtual double get_gps_accuracy () const = 0;

      /**
       * Get GPS position
       * @return GPS location of platform
       */
      utility::GPS_Position get_gps_position ();

      /**
       * Get sensor radius
       * @return minimum radius of all available sensors for this platform
       */
      virtual double get_min_sensor_range () const;

      /**
       * Get move speed
       **/
      virtual double get_move_speed () const;

      /**
       * Get a sensor
       * @param sensor  identifier of sensor to get
       * @return Sensor object
       */
      virtual const variables::Sensor& get_sensor (const std::string& name) const;

      /**
       * Fills a list of sensor names with sensors available on the platform
       * @param  sensors   list of sensors to fill
       **/
      virtual void get_sensor_names (variables::Sensor_Names & sensors) const;

      /**
       * Instructs the device to return home
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int home (void);

      /**
       * Instructs the device to land
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int land (void) = 0;

      /**
       * Moves the platform to a position
       * @param   position  the coordinates to move to
       * @param   epsilon   approximation value
       * @return 1 if moving toward position, 0 if arrived, negative if error
       **/
      virtual int move (const utility::GPS_Position & position,
        const double & epsilon = 0.1);
      
      /**
       * Pauses movement, keeps source and dest at current values
       **/
      virtual void pause_move (void);

      /**
       * Polls the sensor environment for useful information
       * @return number of sensors updated/used
       **/
      virtual int sense (void) = 0;
      
      /**
       * Sets the knowledge base to use for the platform
       * @param  rhs  the new knowledge base to use
       **/
      void set_knowledge (Madara::Knowledge_Engine::Knowledge_Base * rhs);
      
      /**
       * Set move speed
       * @param speed new speed in meters/second
       **/
      virtual void set_move_speed (const double& speed);
      
      /**
       * Sets the map of sensor names to sensor information
       * @param  sensors      map of sensor names to sensor information
       **/
      virtual void set_sensors (variables::Sensors * sensors);
      
      /**
       * Stops movement, resetting source and dest to current location
       **/
      virtual void stop_move (void);

      /**
       * Instructs the device to take off
       * @return 1 if moving, 2 if arrived, 0 if error
       **/
      virtual int takeoff (void) = 0;
      
    protected:
      /// movement speed for platform in meters/second
      double move_speed_;

      /// provides access to variables and values
      Madara::Knowledge_Engine::Knowledge_Base * knowledge_;

      /// provides access to self state
      variables::Self self_;

      /// provides access to a sensor
      variables::Sensors * sensors_;

      /// provides access to status information for this platform
      variables::Platform status_;
    };
  }
}

#endif // _GAMS_PLATFORM_BASE_H_
