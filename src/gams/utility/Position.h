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
 * @file Position.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a utility class for working with position
 **/

#ifndef   _GAMS_UTILITY_GPS_H_
#define   _GAMS_UTILITY_GPS_H_

#include <vector>

#include "gams/GAMS_Export.h"
#include "madara/knowledge_engine/containers/Double_Vector.h"
#include "madara/knowledge_engine/containers/Native_Double_Vector.h"

namespace gams
{
  namespace utility
  {
    class GAMS_Export Position
    {
    public:
      /**
       * Constructor
       * @param  init_x    the x axis coordinate (e.g. latitude)
       * @param  init_y    the y axis coordinate (e.g. longitude)
       * @param  init_z    the z axis coordinate (e.g. altitude)
       **/
      Position (
        double init_x = 0.0, double init_y = 0.0, double init_z = 0.0);

      /**
       * Destructor
       **/
      ~Position ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Position & rhs);
      
      /**
       * Helper function for converting the position to a string
       * @param delimiter characters to insert between position components
       **/
      std::string to_string (const std::string & delimiter = ",") const;

      /**
       * Helper function for copying values to a MADARA double array
       * @param target     target container to copy values to
       **/
      void to_container (
        Madara::Knowledge_Engine::Containers::Double_Array & target) const;
      
      /**
       * Helper function for copying values to a MADARA double array
       * @param source     source container to copy values from
       **/
      void from_container (
        Madara::Knowledge_Engine::Containers::Double_Array & source);
      
      /**
       * Helper function for copying values to a MADARA double array
       * @param target     target container to copy values to
       **/
      void to_container (
        Madara::Knowledge_Engine::Containers::Native_Double_Array & target)
        const;
      
      /**
       * Helper function for copying values to a MADARA double array
       * @param source     source container to copy values from
       **/
      void from_container (
        Madara::Knowledge_Engine::Containers::Native_Double_Array & source);

      /// the x coordinate (e.g. latitude)
      double x;

      /// the y coordinate (e.g. longitude)
      double y;

      /// the z coordinate (e.g. altitude)
      double z;
    };
  }
}

#endif // _GAMS_UTILITY_GPS_H_
