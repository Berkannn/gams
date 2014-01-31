/*********************************************************************
 * Usage of this software requires acceptance of the SMASH-CMU License,
 * which can be found at the following URL:
 *
 * https://code.google.com/p/smash-cmu/wiki/License
 *********************************************************************/

/*********************************************************************
 * BridgeAlgorithm.h - Declares the structures and methods for the algorithm
 * that creates a bridge and decides which drone should move where.
 *********************************************************************/

#ifndef _LINE_BRIDGE_ALGORITHM_H
#define _LINE_BRIDGE_ALGORITHM_H

#include <map>
#include "utilities/Position.h"
#include "BridgeAlgorithm.h"

namespace SMASH
{
    namespace Bridge
    {
        /**
         * @brief Represents a simple bridge algorithm.
         **/
        class LineBridgeAlgorithm : public virtual BridgeAlgorithm
        {
            virtual SMASH::Utilities::Position* getPositionInBridge(int myId, 
                                                            double commRange, 
                                                            SMASH::Utilities::Position sourcePosition, 
                                                            SMASH::Utilities::Position sinkPosition, 
                                                            std::map<int, SMASH::Utilities::Position> availableDronePositions);
        };
    }
}

#endif
