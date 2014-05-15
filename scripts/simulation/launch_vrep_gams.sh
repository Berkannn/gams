#!/bin/bash
WAIT=3600
MADARA_DEBUG=0

xterm -hold -e ./gams_controller -l $MADARA_DEBUG -i 0 -w $WAIT -p vrep -mf scripts/simulation/madara_init_0.mf &
sleep 1
xterm -hold -e ./gams_controller -l $MADARA_DEBUG -i 1 -w $WAIT -p vrep -mf scripts/simulation/madara_init_1.mf &
sleep 1
xterm -hold -e ./gams_controller -l $MADARA_DEBUG -i 2 -w $WAIT -p vrep -mf scripts/simulation/madara_init_2.mf &
