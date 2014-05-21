#!/bin/bash
WAIT=3600
MADARA_DEBUG=0

# launch drone controllers
xterm -hold -e "./gams_controller -i 0 -p vrep -mf scripts/simulation/madara_init_0.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 1 -p vrep -mf scripts/simulation/madara_init_1.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 2 -p vrep -mf scripts/simulation/madara_init_2.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
./dynamic_simulation -n 3 --north_east "40.443412,-79.93954" --south_west "40.442775,-79.940967"
