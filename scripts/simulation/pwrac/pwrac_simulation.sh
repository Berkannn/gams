#!/bin/bash
TIME=3600
MADARA_DEBUG=0
PERIOD=1
AREA=small
NUM=3
SIM=pwrac

# launch drone controllers
xterm -hold -e "./gams_controller -i 0 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_0.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 1 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_1.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 2 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_2.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
./dynamic_simulation -n $NUM -mf scripts/simulation/areas/${AREA}.mf
