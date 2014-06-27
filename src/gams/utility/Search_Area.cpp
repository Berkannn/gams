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
using std::string;
#include <algorithm>
using std::max;

#include "gams/utility/Search_Area.h"

gams::utility::Search_Area::Search_Area ()
{
}

gams::utility::Search_Area::Search_Area (const Prioritized_Region& region)
{
  regions_.push_back (region);
  calculate_bounding_box ();
}

gams::utility::Search_Area::Search_Area (
  const vector<Prioritized_Region>& regions) :
  regions_ (regions)
{
  calculate_bounding_box ();
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
    this->min_lat_ = rhs.min_lat_;
    this->max_lat_ = rhs.max_lat_;
    this->min_lon_ = rhs.min_lon_;
    this->max_lon_ = rhs.max_lon_;
    this->min_alt_ = rhs.min_alt_;
    this->max_alt_ = rhs.max_alt_;
  }
}

inline void
gams::utility::Search_Area::add_prioritized_region (const Prioritized_Region& r)
{
  regions_.push_back (r);

  // modify bounding box
  min_lat_ = (min_lat_ > r.min_lat_) ? r.min_lat_ : min_lat_;
  min_lon_ = (min_lon_ > r.min_lon_) ? r.min_lon_ : min_lon_;
  min_alt_ = (min_alt_ > r.min_alt_) ? r.min_alt_ : min_alt_;

  max_lat_ = (max_lat_ < r.max_lat_) ? r.max_lat_ : max_lat_;
  max_lon_ = (max_lon_ < r.max_lon_) ? r.max_lon_ : max_lon_;
  max_alt_ = (max_alt_ < r.max_alt_) ? r.max_alt_ : max_alt_;
}

const vector<gams::utility::Prioritized_Region>&
gams::utility::Search_Area::get_regions () const
{
  return regions_;
}

unsigned int
gams::utility::Search_Area::get_priority (const GPS_Position& pos) const
{
  unsigned int priority = 0;
  for (vector<Prioritized_Region>::const_iterator it = regions_.begin ();
    it != regions_.end (); ++it)
  {
    if (it->is_in_region (pos))
      priority = max (priority, it->priority);
  }

  return priority;
}

bool
gams::utility::Search_Area::is_in_search_area (const GPS_Position & p) const
{
  for (unsigned int i = 0; i < regions_.size(); ++i)
    if (regions_[i].is_in_region (p))
      return true;
  return false;
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
gams::utility::Search_Area::calculate_bounding_box ()
{
  min_lat_ = min_lon_ = min_alt_ = DBL_MAX;
  max_lat_ = max_lon_ = max_alt_ = -DBL_MAX;
  for (unsigned int i = 0; i < regions_.size (); ++i)
  {
    min_lat_ = (min_lat_ > regions_[i].min_lat_) ? regions_[i].min_lat_ : min_lat_;
    min_lon_ = (min_lon_ > regions_[i].min_lon_) ? regions_[i].min_lon_ : min_lon_;
    min_alt_ = (min_alt_ > regions_[i].min_alt_) ? regions_[i].min_alt_ : min_alt_;

    max_lat_ = (max_lat_ < regions_[i].max_lat_) ? regions_[i].max_lat_ : max_lat_;
    max_lon_ = (max_lon_ < regions_[i].max_lon_) ? regions_[i].max_lon_ : max_lon_;
    max_alt_ = (max_alt_ < regions_[i].max_alt_) ? regions_[i].max_alt_ : max_alt_;
  }
}

gams::utility::Search_Area
gams::utility::parse_search_area (
  Madara::Knowledge_Engine::Knowledge_Base& knowledge,
  const string& search_area_id)
{
  Search_Area ret;

  // get size of search_area in number of regions
  char expr[512];
  sprintf (expr, "%s.size", search_area_id.c_str ());
  const unsigned int num_regions = knowledge.get (expr).to_integer ();

  // parse each region
  for (unsigned int i = 0; i < num_regions; ++i)
  {
    // get prioritized region and add to search area
    sprintf (expr, "%s.%u", search_area_id.c_str (), i);
    ret.add_prioritized_region (
      parse_prioritized_region (knowledge, knowledge.get (expr).to_string ()));
  }

  return ret;
}
