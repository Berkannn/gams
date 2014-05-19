#!/bin/bash

SCENE=`pwd`/scripts/simulation/floor_10x10.ttt
cd $VREP_ROOT
./vrep.sh $SCENE > /dev/null &
