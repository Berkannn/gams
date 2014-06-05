#!/bin/bash
TIME=3600
MADARA_DEBUG=0
PERIOD=0.5
NUM=5
SIM=rotation_formation
AREA=small

# launch drone controllers
xterm -hold -e "./gams_controller -i 0 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_0.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 1 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_1.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 2 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_2.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 3 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_3.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "./gams_controller -i 4 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf scripts/simulation/$SIM/madara_init_4.mf scripts/simulation/areas/${AREA}.mf scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
./dynamic_simulation -n $NUM -mf scripts/simulation/areas/${AREA}.mf
