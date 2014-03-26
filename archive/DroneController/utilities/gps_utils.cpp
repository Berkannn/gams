/*********************************************************************
* Usage of this software requires acceptance of the SMASH-CMU License,
* which can be found at the following URL:
*
* https://code.google.com/p/smash-cmu/wiki/License
*********************************************************************/

#include "gps_utils.h"
#include <math.h>

// Haversin function.
#define HAVERSIN(x) pow(sin((x)/2), 2)

// The radius of the Earth in meters.
static const double EARTH_RADIUS = 6371000.0;

static const double DEGREES_IN_CIRCUMFERENCE = 360.0;
static const double EARTH_POLES_PERIMETER = 40008000.0;
static const double EARTH_EQUATORIAL_PERIMETER = 40075160.0;

///////////////////////////////////////////////////////////////////////////////
// Calculate the distance between two coordinate pairs.
///////////////////////////////////////////////////////////////////////////////
double SMASH::Utilities::gps_coordinates_distance (double lat1, double long1, 
  double lat2, double long2)
{
  // Turn the latitudes into radians.
  lat1 = DEG_TO_RAD(lat1);
  lat2 = DEG_TO_RAD(lat2);

  // Get the difference between our two points then convert the difference into 
  // radians.
  double lat_diff = DEG_TO_RAD(lat2 - lat1);
  double long_diff = DEG_TO_RAD(long2 - long1);

  // Calculate the distance in meters via trigonometry, using the cosines law 
  // formula.
  double distanceInMeters = acos(sin(lat1)*sin(lat2) + 
                                 cos(lat1)*cos(lat2) *
                                 cos(long_diff)) * EARTH_RADIUS;

  // Return the distance.
  return distanceInMeters;
}

///////////////////////////////////////////////////////////////////////////////
// Calculate the distance between two coordinate pairs.
///////////////////////////////////////////////////////////////////////////////
double gps_coordinates_distanceHarv (double lat1, double long1, 
  double lat2, double long2)
{
  // Turn the latitudes into radians.
  lat1 = DEG_TO_RAD(lat1);
  lat2 = DEG_TO_RAD(lat2);

  // Get the difference between our two points then convert the difference into 
  // radians.
  double lat_diff = DEG_TO_RAD(lat2 - lat1);
  double long_diff = DEG_TO_RAD(long2 - long1);

  // Calculate the distance in meters via trigonometry, using the haversine 
  // formula.
  double pointsHaversin = HAVERSIN(lat_diff) + 
                          (HAVERSIN(long_diff) * cos(lat1) * cos(lat2));
  double c = 2 * atan2(sqrt(pointsHaversin), sqrt(1 - pointsHaversin));
  double distanceInMeters = EARTH_RADIUS * c;

  // Return the distance.
  return distanceInMeters;
}

///////////////////////////////////////////////////////////////////////////////
// Calculates the latitude and longitude of a point given as a distance in 
// meters in x and y from a reference point.
///////////////////////////////////////////////////////////////////////////////
SMASH::Utilities::Position SMASH::Utilities::getLatAndLong(double x, double y, 
  SMASH::Utilities::Position referencePoint)
{
  // Get the deltas from reference point.
  double perimeterAtRefLatitude =  EARTH_EQUATORIAL_PERIMETER * 
    cos(DEG_TO_RAD(referencePoint.latitude));
  double deltaLongitude = x * DEGREES_IN_CIRCUMFERENCE / perimeterAtRefLatitude;
  double deltaLatitude  = y * DEGREES_IN_CIRCUMFERENCE / EARTH_POLES_PERIMETER;

  // Get the lat and long.
  double latitude = deltaLatitude + referencePoint.latitude;
  double longitude = deltaLongitude + referencePoint.longitude;

  // Turn this into a Position and return it.
  SMASH::Utilities::Position result;
  result.latitude = latitude;
  result.longitude = longitude;
  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Turns coordinates into cartesian values.
///////////////////////////////////////////////////////////////////////////////
SMASH::Utilities::CartesianPosition SMASH::Utilities::getCartesianCoordinates(
  SMASH::Utilities::Position coordinates, 
  SMASH::Utilities::Position referencePoint)
{
  // Calculate the difference between our and the reference points.
  double deltaLatitude = coordinates.latitude - referencePoint.latitude;
  double deltaLongitude = coordinates.longitude - referencePoint.longitude;
    
  // Actually do the conversion.
  double perimeterAtRefLatitude = EARTH_EQUATORIAL_PERIMETER * 
    cos(DEG_TO_RAD(referencePoint.latitude));
  double resultX = perimeterAtRefLatitude*deltaLongitude/DEGREES_IN_CIRCUMFERENCE;
  double resultY = EARTH_POLES_PERIMETER*deltaLatitude/DEGREES_IN_CIRCUMFERENCE;

  // Return it as a pair.
  SMASH::Utilities::CartesianPosition cartesianCoordinates(resultX, resultY);
  return cartesianCoordinates;
}