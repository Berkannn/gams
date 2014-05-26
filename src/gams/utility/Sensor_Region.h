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
 * @file Sensor_Region.h
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Tracks sensors data over points in a region
 **/

#ifndef  _GAMS_UTILITY_SENSOR_REGION_H_
#define  _GAMS_UTILITY_SENSOR_REGION_H_

#include "gams/utility/Region.h"
#include "gams/GAMS_Export.h"
#include "gams/utility/Position.h"

#include "madara/knowledge_engine/containers/Vector_N.h"
using Madara::Knowledge_Engine::Containers::Array_N;

#include <string>

namespace gams
{
  namespace utility
  {
    class GAMS_Export Sensor_Region : Region
    {
    public:
      /**
       * Constructor
       * @param  init_points  the vertices of the region
       **/
      Sensor_Region (std::string name,
        Madara::Knowledge_Engine::Knowledge_Base& knowledge,
        const double& delta, 
        const std::vector <Position> & init_points = std::vector<Position> ());

      /**
       * Destructor
       **/
      ~Sensor_Region ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Sensor_Region& rhs);

      /**
       * Get sensor value from array
       * @param loc location to get sensor value for
       * @return value of sensor at specified location
       **/
      double operator[] (const Array_N::Index& loc) const;

      /**
       * Get sensor value from array
       * @param lat latitude of requested location
       * @param lon longitude of requested location
       * @return value of sensor at requested location
       **/
      double get (const double& x, const double& y) const;

      /**
       * Set sensor value at location
       * @param loc location to set value at
       * @param val value to set for location
       **/
      void set (const Array_N::Index& loc, const double& val);

      /**
       * Set sensor value at location
       * @param lat  latitude of location to set value at
       * @param lon  longitude of location to set value at
       * @param val  value to set for location
       **/
      void set (const double& x, const double& y, const double& val);
      
      /**
       * Helper function for converting the position to a string
       * @param delimiter characters to insert between position components
       **/
      std::string to_string (const std::string& delimiter = ":") const;

    protected:
      /**
       * discretize the region
       **/
      Array_N::Index get_array_indices (const double& x, const double& y)
        const;

      /// discretization constant
      const double delta_;

      /// sensor value of point
      Array_N sensor_values_;
    }; // class Sensor_Region
  } // namespace utility
} // namespace gams

#endif // _GAMS_UTILITY_REGION_H_
