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
 * @file VREP_Boat_EXT.h
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * This file contains the declaration of the VREP_Boat_EXT simulator autonomous boat class
 * which should be used when the boat is to be externally controlled by directly setting 
 * motor commands  
 **/

#ifndef   _GAMS_PLATFORM_VREP_BOAT_EXT_H_
#define   _GAMS_PLATFORM_VREP_BOAT_EXT_H_

#include "gams/platforms/Platform_Factory.h"
#include "gams/platforms/vrep/VREP_Base.h"

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
    class GAMS_Export VREP_Boat_EXT : public VREP_Base
    {
    public:
      /**
       * Constructor
       * @param  knowledge  knowledge base
       * @param  sensors    map of sensor names to sensor information
       * @param  platforms  map of platform names to platform information
       * @param  self       device variables that describe self state
       **/
      VREP_Boat_EXT (
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self);
     
      /**
       * Destructor
       **/
      ~VREP_Boat_EXT();
  
      /*
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
       * Override base class move 
       */
      int
      gams::platforms::VREP_Base::move (const utility::Position & position,
        const double & epsilon);

    protected:
      /**
       * Add model to environment
       */
      virtual void add_model_to_environment ();

      /**
       * Get target handle
       */
      virtual void get_target_handle ();
  
      /**
       * Set initial position
       */
      virtual void set_initial_position () const;
    /**
     * A class to run on a seperate thread to query the knowledge base and 
     * update motor commands
     **/
     class Motor_Updater: public threads::Base_Thread{
        
        public:
          void init(Madara::Knowledge_Engine::Knowledge_Base & knowledge)
          {
            lknowledge_ = knowledge;
            //Get boat name to create motor signals
            simxInt num_handles;
            simxInt * handles;
            simxInt num_names;
            simxChar * names;
            
            //VREP remote API has no function to directly retrieve an object
            //name. Instead all object names are retrieved and the correct
            //name is matached with the handle of the correct object 
            simxGetObjectGroupData(client_id_, sim_appobj_object_type, 0,
                                   num_handles, &handles, NULL, NULL,NULL, 
                                   NULL, num_names, &names, 
                                   simx_opmode_oneshot_wait);
            //Compare each handle to the stored handle
            int i = 0;
            for(i = 0;  i < num_handles; i++)
            {
              boat_name = names;
              names += boat_name.length() + 1;
              if(handles[i] == node_id_)
              {
                break;  
              }
            }
            //Extract boat number from name
            //Boats are numbered as follows: no_number, #0, #1, ... 
            ssize_t pos = boat_name.find("#");
            int id;
            if(pos == std::npos)
              id = 0;
            else
              id = atoi( boat_name.substr(pos+1).c_str() ) + 1;
            
            //Generate signal names
            std::stringstream lss;
            lss << "left_motor_" << id;
            left_motor_sig = lss.str();

            std::stringstream rss;
            rss << "right_motor_" << id;
            right_motor_sig = rss.str();

          }
          void run(void )
          {
            //Query knowledge base for motor speeds
            left_speed = lknowledge.get(left_motor_sig);
            right_speed = lknowledge.get(right_motor_sig);
            
            //Pass motor speeds to vrep
            simxSetFloatSignal(client_id_, left_motor_sig.c_str() ,  left_speed, simx_opmode_oneshot);
            simxSetFloatSignal(client_id_, right_motor_sig.c_str(), right_speed, simx_opmode_oneshot);
          }
          void cleanup(void){};
        private:
          Madara::Knowledge_Engine::Knowledge_Base lknowledge_;
          double left_motor_speed = 0;
          double right_motor_speed = 0;
          std::string right_motor_sig;
          std::string left_motor_sig;
          };
                        
      }
    }; // class VREP_Boat

    /**
     * A factory class for creating VREP Boat platforms
     **/
    class GAMS_Export VREP_Boat_EXT_Factory : public Platform_Factory
    {
    public:
        
      /**
       * Creates a VREP ant platform.
       * @param   args      no arguments are necessary for this platform
       * @param   knowledge the knowledge base. This will be set by the
       *                    controller in init_vars.
       * @param   sensors   the sensor info. This will be set by the
       *                    controller in init_vars.
       * @param   platforms status inform for all known devices. This
       *                    will be set by the controller in init_vars
       * @param   self      self-referencing variables. This will be
       *                    set by the controller in init_vars
       **/
      virtual Base_Platform * create (
        const Madara::Knowledge_Vector & args,
        Madara::Knowledge_Engine::Knowledge_Base * knowledge,
        variables::Sensors * sensors,
        variables::Platforms * platforms,
        variables::Self * self);
    };
    
  } // namespace platform
} // namespace gams

#endif // _GAMS_VREP_

#endif // _GAMS_PLATFORM_VREP_BOAT_EXT_H_
