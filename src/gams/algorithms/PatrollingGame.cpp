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
 * @file PatrollingGame.h
 * @author Philip Cooksey <pcooksey@andrew.cmu.edu>
 **/


#include "gams/algorithms/PatrollingGame.h"
#include "gams/algorithms/ControllerAlgorithmFactory.h"
#include "madara/knowledge/containers/StringVector.h"
#include "madara/utility/Utility.h"

#include <sstream>
#include <string>
#include <iostream>
#include <cmath>

#include "gams/algorithms/AlgorithmFactory.h"
#include "gams/utility/ArgumentParser.h"
#include "gams/utility/CartesianFrame.h"

namespace engine = madara::knowledge;
namespace containers = engine::containers;

using namespace gams::utility;

typedef madara::knowledge::KnowledgeRecord::Integer  Integer;

gams::algorithms::BaseAlgorithm *
gams::algorithms::PatrollingGameFactory::create (
const madara::knowledge::KnowledgeMap & args,
madara::knowledge::KnowledgeBase * knowledge,
platforms::BasePlatform * platform,
variables::Sensors * sensors,
variables::Self * self,
variables::Agents * agents)
{
  BaseAlgorithm * result (0);

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::PatrollingGameFactory:" \
    " entered create with %u args\n", args.size ());

  if (knowledge && sensors && platform && self)
  {
    std::string protectors = "protectors";
    std::string assets = "assets";
    std::string enemies = "enemies";
    std::string formation = "attack";

    ArgumentParser argp(args);

    for(ArgumentParser::const_iterator i = argp.begin();
         i != argp.end(); i.next())
    {
      std::string name(i.name());
      if(name.size() <= 0)
        continue;
      switch(name[0])
      {
      case 'a':
        if("assets" == name)
        {
          assets = i.value().to_string();
          break;
        }
        goto unknown;
      case 'e':
        if("enemies" == name)
        {
          enemies = i.value().to_string();
          break;
        }
        goto unknown;
      case 'f':
        if("formation" == name)
        {
          formation = i.value().to_string();
          break;
        }
        goto unknown;
      case 'p':
        if("protectors" == name)
        {
          protectors = i.value().to_string();
          break;
        }
        goto unknown;
      default:
      unknown:
        madara_logger_ptr_log (gams::loggers::global_logger.get (),
          gams::loggers::LOG_MAJOR,
          "gams::algorithms::PatrollingGameFactory:" \
          " argument unknown: %s -> %s\n",
          name.c_str(), i.value().to_string().c_str());
        continue;
      }
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_MAJOR,
        "gams::algorithms::PatrollingGameFactory:" \
        " argument: %s -> %s\n", name.c_str(), i.value().to_string().c_str());
    }

    result = new PatrollingGame (
      protectors, assets, enemies, formation,
      knowledge, platform, sensors, self);
  }

  return result;
}

gams::algorithms::PatrollingGame::PatrollingGame (
  const std::string &protectors,
  const std::string &assets,
  const std::string &enemies,
  const std::string &formation,
  madara::knowledge::KnowledgeBase * knowledge,
  platforms::BasePlatform * platform,
  variables::Sensors * sensors,
  variables::Self * self) :
  BaseAlgorithm (knowledge, platform, sensors, self),
  protector_group_(protectors), asset_group_(assets), enemy_group_(enemies),
  protectors_(get_group(protectors)), assets_(get_group(assets)),
  enemies_(get_group(enemies)),
  formation_(formation),
  index_(get_index()),
  form_func_(get_form_func(formation)),
  next_loc_(), old_enemy_loc_()
{
  status_.init_vars (*knowledge, "patrolling_game", self->id.to_integer ());
  status_.init_variable_values ();

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::PatrollingGame::constructor:" \
    " Creating algorithm with args: ...\n" \
    "   protectors -> %s\n" \
    "   assets -> %s\n" \
    "   enemies -> %s\n" \
    "   formation -> %s\n",
    protectors.c_str(), assets.c_str(), enemies.c_str(),
    formation.c_str()
    );

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_TRACE,
    "gams::algorithms::PatrollingGame::constructor:" \
    " protectors list size: %i\n",
    protectors_.size());

  update_arrays(assets_, asset_loc_cont_);

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_TRACE,
    "gams::algorithms::PatrollingGame::constructor:" \
    " assets list size: %i; asset loc array size: %i\n",
    assets_.size(), asset_loc_cont_.size());

  update_arrays(enemies_, enemy_loc_cont_);

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_TRACE,
    "gams::algorithms::PatrollingGame::constructor:" \
    " enemy list size: %i; enemy loc array size: %i\n",
    assets_.size(), asset_loc_cont_.size());

  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_TRACE,
    "gams::algorithms::PatrollingGame::constructor:" \
    " index: %i\n", index_);
}


gams::algorithms::PatrollingGame::~PatrollingGame ()
{
}

void
gams::algorithms::PatrollingGame::operator= (const PatrollingGame & rhs)
{
  if (this != &rhs)
  {
    this->BaseAlgorithm::operator= (rhs);
    this->protectors_ = rhs.protectors_;
    this->assets_ = rhs.assets_;
    this->enemies_ = rhs.enemies_;
    this->formation_ = rhs.formation_;
    this->form_func_ = rhs.form_func_;
  }
}

int
gams::algorithms::PatrollingGame::get_index() const
{
  std::stringstream to_find;
  to_find << "agent." << self_->id.to_integer();
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_TRACE,
    "gams::algorithms::PatrollingGame::get_index:" \
    " looking for: %s\n", to_find.str().c_str());
  for(int i = 0; i < protectors_.size(); ++i)
  {
    if(protectors_[i] == to_find.str())
      return i;
  }
  for(int i = 0; i < enemies_.size(); ++i)
  {
    if(enemies_[i] == to_find.str())
      return i;
  }
  return -1;
}

madara::knowledge::containers::StringVector
gams::algorithms::PatrollingGame::get_group(const std::string &name) const
{
  return madara::knowledge::containers::StringVector(
     "group." + name + ".members", *knowledge_);
}

void
gams::algorithms::PatrollingGame::update_arrays(
  const madara::knowledge::containers::StringVector &names,
  MadaraArrayVec &arrays) const
{
  arrays.clear();
  arrays.resize(names.size());
  for(int i = 0; i < names.size(); ++i)
  {
    arrays[i].set_name(names[i] + ".location", *knowledge_, 3);
  }
}

void
gams::algorithms::PatrollingGame::update_locs(
  const MadaraArrayVec &arrays,
  std::vector<Location> &locs) const
{
  if(locs.size() != arrays.size())
  {
    madara_logger_ptr_log (gams::loggers::global_logger.get (),
      gams::loggers::LOG_MAJOR,
      "gams::algorithms::PatrollingGame::update_locs:" \
      " resizing locs array\n");
    locs.resize(arrays.size(), Location(platform_->get_frame()));
  }
  for(int i = 0; i < arrays.size(); ++i)
  {
    if(arrays[i][0] != 0.0 && arrays[i][1] != 0.0)
    {
      locs[i].from_container<order::GPS>(arrays[i]);
      madara_logger_ptr_log (gams::loggers::global_logger.get (),
        gams::loggers::LOG_MAJOR,
        "gams::algorithms::PatrollingGame::update_locs:" \
        " read loc (%f, %f, %f) for #%i\n",
        locs[i].x(), locs[i].y(), locs[i].z(), i);
    }
  }
}

int
gams::algorithms::PatrollingGame::analyze (void)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::PatrollingGame::analyze:" \
    " entering analyze method\n");

  update_locs(asset_loc_cont_, asset_locs_);
  update_locs(enemy_loc_cont_, enemy_locs_);
  return OK;
}

int
gams::algorithms::PatrollingGame::execute (void)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::PatrollingGame::execute:" \
    " entering execute method\n");

  if(!next_loc_.is_invalid())
    platform_->move(next_loc_);
  return OK;
}

int
gams::algorithms::PatrollingGame::plan (void)
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::PatrollingGame::plan:" \
    " entering plan method\n");

  if(index_ >= 0)
    next_loc_ = ((this)->*(form_func_))();
  
  //Save old enemy position
  old_enemy_loc_ = enemy_locs_[0];
  return OK;
}

Location
gams::algorithms::PatrollingGame::attack_formation() const
{
  // We get location
  Location ret(platform_->get_frame());
  const Location &enemy_loc = enemy_locs_[0];
  
  if(old_enemy_loc_.x()!=INVAL_COORD)
  {
    double dx, dy, dz, distance;
    dx = enemy_loc.x()-old_enemy_loc_.x();
    dy = enemy_loc.y()-old_enemy_loc_.y();
    dz = enemy_loc.z()-old_enemy_loc_.z();
    
    distance = sqrt(dx*dx +dy*dy+dz*dz);
    
    ret.x(enemy_loc.x()+dx*10);
    ret.y(enemy_loc.y()+dy*10);
    ret.z(enemy_loc.z()+dz);
  }
  
  return ret;
}

Location
gams::algorithms::PatrollingGame::diving_attack() const
{
  madara_logger_ptr_log (gams::loggers::global_logger.get (),
    gams::loggers::LOG_MAJOR,
    "gams::algorithms::PatrollingGame::execute:" \
    " Diving Attack\n");
  // We get location
  Location ret(platform_->get_frame());
   // Check if we have assests and enemies
  if(asset_locs_.size() >= 1)
  {
    const Location &enemy_loc = asset_locs_[0];
    ret.x(enemy_loc.x());
    ret.y(enemy_loc.y());
    ret.z(enemy_loc.z());
  }
  return ret;
}

gams::algorithms::PatrollingGame::formation_func
gams::algorithms::PatrollingGame::get_form_func(const std::string &form_name)
{
  if(form_name.size() == 0)
    return &PatrollingGame::attack_formation;
  switch(form_name[0])
  {
  case 'a':
    if(form_name == "attack")
      return &PatrollingGame::attack_formation;
    goto unknown;
  case 'd':
    if(form_name == "diving")
      return &PatrollingGame::diving_attack;
    goto unknown;
  default:
  unknown:
    return &PatrollingGame::attack_formation;
  }
}
