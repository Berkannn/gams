#!/bin/bash

# launch drone controllers
for (( i=0; i<$NUM; i++ ))
do
  xterm -hold -e "$GAMS_ROOT/gams_controller -i $i -n $NUM -p vrep --loop-time $TIME --period $PERIOD --madara-file $GAMS_ROOT/scripts/simulation/$SIM/madara_init_${i}.mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf $GAMS_ROOT/scripts/simulation/madara_init_common.mf -l $MADARA_DEBUG" &
done

# launch simulation controller
$GAMS_ROOT/dynamic_simulation -p -n $NUM -mf $GAMS_ROOT/scripts/simulation/areas/${AREA}.mf
