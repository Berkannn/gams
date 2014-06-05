#!/bin/bash
TIME=3600
MADARA_DEBUG=0
PERIOD=1
AREA=small
NUM=3
SIM=pwrac

# launch drone controllers
xterm -hold -e "$GAMS_ROOT/gams_controller -i 0 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_0.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "$GAMS_ROOT/gams_controller -i 1 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_1.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
xterm -hold -e "$GAMS_ROOT/gams_controller -i 2 -n $NUM -p vrep -lt $TIME -pd $PERIOD -mf $GAMS_ROOT/scripts/simulation/$SIM/madara_init_2.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &

# launch simulation controller
$GAMS_ROOT/dynamic_simulation -n $NUM -mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf
