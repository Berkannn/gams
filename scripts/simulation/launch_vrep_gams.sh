#!/bin/bash
WAIT=3600

xterm -hold -e ./gams_controller -i 0 -w $WAIT -p vrep -a "random edge coverage" -mf scripts/simulation/madara_init_0.mf &
sleep 1
xterm -hold -e ./gams_controller -i 1 -w $WAIT -p vrep -a "random edge coverage" -mf scripts/simulation/madara_init_1.mf &
sleep 1
xterm -hold -e ./gams_controller -i 2 -w $WAIT -p vrep -a "random edge coverage" -mf scripts/simulation/madara_init_2.mf &
