#!/bin/bash

xterm -e ./gams_controller -i 0 -p vrep -a "random edge coverage" -mf scripts/simulation/madara_init_0.mf &
sleep 1
xterm -e ./gams_controller -i 1 -p vrep -a "random edge coverage" -mf scripts/simulation/madara_init_1.mf &
sleep 1
xterm -e ./gams_controller -i 2 -p vrep -a "random edge coverage" -mf scripts/simulation/madara_init_2.mf &
