/**
* Copyright (c) 2016 Carnegie Mellon University. All Rights Reserved.
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

#ifndef   _GAMS_AUCTIONS_AUCTION_FACTORY_REPOSITORY_INL_
#define   _GAMS_AUCTIONS_AUCTION_FACTORY_REPOSITORY_INL_

#include "madara/knowledge/containers/Integer.h"

#include "gams/auctions/AuctionFactoryRepository.h"
#include "gams/auctions/AuctionMaximumBid.h"
#include "gams/auctions/AuctionMinimumBid.h"

inline void
gams::auctions::AuctionFactoryRepository::add (const std::string & type,
  AuctionFactory * factory)
{
  if (factory)
  {
    factory->set_knowledge (knowledge_);
    factory_map_[type] = factory;
  }
}

inline gams::auctions::AuctionBase *
gams::auctions::AuctionFactoryRepository::create (const std::string & type)
{
  gams::auctions::AuctionBase * result (0);
  AuctionFactoryMap::iterator found = factory_map_.find (type);

  if (found != factory_map_.end ())
  {
    result = found->second->create ("", knowledge_);
  }

  return result;
}

inline void
gams::auctions::AuctionFactoryRepository::init (void)
{
  // create appropriate default factories

  // set the knowledge base for each factory
  for (AuctionFactoryMap::iterator i = factory_map_.begin ();
    i != factory_map_.end (); ++i)
  {
    i->second->set_knowledge (knowledge_);
  }
}

inline void
gams::auctions::AuctionFactoryRepository::set_knowledge (
madara::knowledge::KnowledgeBase * knowledge)
{
  knowledge_ = knowledge;
}

#endif // _GAMS_AUCTIONS_AUCTION_FACTORY_REPOSITORY_INL_
