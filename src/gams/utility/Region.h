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
 * @file Region.h
 * @author James Edmondson <jedmondson@gmail.com>
 *
 * This file contains a utility class for working with position
 **/

#ifndef  _GAMS_UTILITY_REGION_H_
#define  _GAMS_UTILITY_REGION_H_

#include <vector>

#include "gams/GAMS_Export.h"
#include "madara/knowledge_engine/containers/String_Vector.h"
#include "gams/utility/Position.h"

namespace gams
{
  namespace utility
  {
    class GAMS_Export Region
    {
    public:
      /**
       * Constructor
       * @param  init_points  the vertices of the region
       **/
      Region (const std::vector <Position> & init_points);

      /**
       * Destructor
       **/
      ~Region ();

      /**
       * Assignment operator
       * @param  rhs   values to copy
       **/
      void operator= (const Region & rhs);
      
      /**
       * Determine if position is in region
       * @param   p   point to check if in region
       * @return  true if point is in region or on border, false otherwise
       **/
      bool is_in_region (const Position & p) const;

      /**
       * Get bounding box
       * @return Region object corresponding to bounding box
       **/
      Region get_bounding_box () const;

      /**
       * Helper function for converting the position to a string
       * @param delimiter characters to insert between position components
       **/
      std::string to_string (const std::string & delimiter = ":") const;

      /**
       * Helper function for copying values to a MADARA double array
       * @param target     target container to copy values to
       **/
      void to_container (
        Madara::Knowledge_Engine::Containers::String_Array & target) const;
      
      /**
       * Helper function for copying values from a MADARA double array
       * @param target     target container to copy values from
       **/
      void from_container (
        Madara::Knowledge_Engine::Containers::String_Array & target);

      /// the vertices of the region
      std::vector <Position> points;

      /// bounding box
      double min_x_, max_x_;
      double min_y_, max_y_;
      double min_z_, max_z_;

    protected:
      /**
       * Default constructor should only be called by the class
       **/
      Region ();

      /**
       * populate bounding box values
       **/
      void calculate_bounding_box ();
    }; // class Region
  } // namespace utility
} // namespace gams

#endif // _GAMS_UTILITY_REGION_H_
