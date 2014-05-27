#!/bin/bash
TIME=3600
MADARA_DEBUG=0
PERIOD=1
SIM_TYPE=$1
AREA=$2

# launch drone controllers
xterm -hold -e "./gams_controller -i 0 -n 3 -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM_TYPE/madara_init_0.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 1 -n 3 -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM_TYPE/madara_init_1.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 2 -n 3 -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM_TYPE/madara_init_2.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
./dynamic_simulation -n 3 --north_east "20,20" --south_west "0,0"
