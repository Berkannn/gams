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
 * 3. The names Carnegie Mellon University, "SEI and/or Software
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
 *      INSTITUTE MATERIAL IS FURNISHED ON AN AS-IS BASIS. CARNEGIE MELLON
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
 * @file Search_Area.cpp
 * @author Anton Dukeman <anton.dukeman@gmail.com>
 *
 * Search Area is a collection of regions, possibly with priority
 **/

#include <sstream>
using std::stringstream;
#include <string>
#include "gams/utility/Search_Area.h"

gams::utility::Search_Area::Search_Area ()
{
}

gams::utility::Search_Area::Search_Area (const Prioritized_Region& region)
{
  regions_.push_back (region);
}

gams::utility::Search_Area::~Search_Area ()
{
}

void
gams::utility::Search_Area::operator= (const Search_Area & rhs)
{
  if (this != &rhs)
  {
    this->regions_ = rhs.regions_;
    this->union_ = rhs.union_;
  }
}

inline void
gams::utility::Search_Area::add_prioritized_region (const Prioritized_Region& r)
{
  regions_.push_back (r);
}

const vector<gams::utility::Prioritized_Region>&
gams::utility::Search_Area::get_regions () const
{
  return regions_;
}

bool
gams::utility::Search_Area::is_in_search_area (const GPS_Position & p) const
{
  bool ret = false;
  for (unsigned int i = 0; i < regions_.size(); ++i)
    ret = (ret || regions_[i].is_in_region (p));
  return ret;
}

string
gams::utility::Search_Area::to_string () const
{
  stringstream buffer;
  buffer << "Num regions: " << regions_.size () << endl;
  for (unsigned int i = 0; i < regions_.size (); ++i)
    buffer << "Region " << i << ": " << regions_[i].to_string () << endl;
  return buffer.str();
}

void
gams::utility::Search_Area::to_container (
  Madara::Knowledge_Engine::Containers::String_Array & target) const
{
#warning gams::utility::Search_Area::to_container NYI
}

void
gams::utility::Search_Area::from_container (
  Madara::Knowledge_Engine::Containers::String_Array & target)
{
#warning gams::utility::Search_Area::from_container NYI
//  regions_.resize (target.size ());
//  for (unsigned int i = 0; i < target.size (); ++i)
//    points[i] = GPS_Position::from_string (target[i]);
//  calculate_bounding_box ();
}

void
gams::utility::Search_Area::calculate_bounding_box ()
{
#warning gams::utility::Search_Area::calculate_bounding_box NYI
//  min_lat_ = min_lon_ = min_alt_ = DBL_MAX;
//  max_lat_ = max_lon_ = max_alt_ = -DBL_MAX;
//  for (unsigned int i = 0; i < .size(); ++i)
//  {
//    min_lat_ = (min_lat_ > points[i].lat) ? points[i].lat : min_lat_;
//    min_lon_ = (min_lon_ > points[i].lon) ? points[i].lon : min_lon_;
//    min_alt_ = (min_alt_ > points[i].alt) ? points[i].alt : min_alt_;
//
//    max_lat_ = (max_lat_ < points[i].lat) ? points[i].lat : max_lat_;
//    max_lon_ = (max_lon_ < points[i].lon) ? points[i].lon : max_lon_;
//    max_alt_ = (max_alt_ < points[i].alt) ? points[i].alt : max_alt_;
//  }
}

gams::utility::Search_Area
gams::utility::parse_search_area (
  Madara::Knowledge_Engine::Knowledge_Base& knowledge,
  const unsigned int search_area_id)
{
  Search_Area ret;

  // get size of search_area in number of regions
  char expr[50];
  sprintf (expr, "search_area.%u.size", search_area_id);
  const unsigned int num_regions = knowledge.get (expr).to_integer ();

  // parse each region
  for (unsigned int i = 0; i < num_regions; ++i)
  {
    // get region id
    sprintf (expr, "search_area.%u.%u", search_area_id, i);
    unsigned int region_id;
    sscanf (knowledge.get (expr).to_string ().c_str (), "region.%u",
      &region_id);

    // get prioritized region and add to search area
    ret.add_prioritized_region (
      parse_prioritized_region (knowledge, region_id));
  }

  return ret;
}
