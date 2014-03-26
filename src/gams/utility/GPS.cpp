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
#include "GPS.h"

double
gams::utility::get_distance (
      double lat1, double long1,
      double lat2, double long2)
{
  /**
   * We use Haversine here, though it is known to be less accurate than
   * Vincenty's formula. Unfortunately, Vincenty's formula is iterative
   * and can be time consuming. Harversine is less accurate but easier to
   * code and much quicker.
   *
   * From literature reviews, Haversine can be inaccurate up to .3% and
   * this varies with altitude since Haversine assumes a perfect sphere
   * and Earth simply isn't one (it's actually an ellipsoid with an irregular
   * surface).
   **/

  // Convert the coordinates into radians for haversine method
  lat1 = DEGREES_TO_RADIANS (lat1);
  lat2 = DEGREES_TO_RADIANS (lat2);

  // get difference in radians
  double lat_rad_diff = DEGREES_TO_RADIANS (lat2 - lat1);
  double long_rad_diff = DEGREES_TO_RADIANS (long2 - long1);

  // Here is the meat of the Haversine formula
  double a =
    sin (lat_rad_diff / 2) * sin (lat_rad_diff / 2) + (
      sin (long_rad_diff / 2) * sin (long_rad_diff / 2) *
        cos (lat1) * cos (lat2));

  double c = 2 * atan2(sqrt (a), sqrt (1 - a));

  // Return Earth's radius 
  return 6371000 * c;
}
