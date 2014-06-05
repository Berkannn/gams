#!/bin/bash
TIME=3600
MADARA_DEBUG=0
PERIOD=0.75
AREA=small
NUM=5
SIM=local_pheremone

# launch drone controllers
xterm -hold -e "$GAMS_ROOT/gams_controller -i 0 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_0.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "$GAMS_ROOT/gams_controller -i 1 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_1.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "$GAMS_ROOT/gams_controller -i 2 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_2.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "$GAMS_ROOT/gams_controller -i 3 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_3.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "$GAMS_ROOT/gams_controller -i 4 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_4.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
$GAMS_ROOT/dynamic_simulation -p -n $NUM -mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf
