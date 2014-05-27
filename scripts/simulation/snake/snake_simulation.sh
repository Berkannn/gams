#!/bin/bash
TIME=3600
MADARA_DEBUG=0
PERIOD=1
AREA=20

# launch drone controllers
xterm -hold -e "./gams_controller -i 0 -n 1 -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/snake/madara_init_0.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
./dynamic_simulation -n 1 --north_east "20,20" --south_west "0,0"
