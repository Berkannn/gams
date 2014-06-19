#!/bin/bash

# local vars
CORES=10 # number of build jobs
ARM_COMPILE_PREFIX=$HOME/dev/drone-rk/toolchain/arm-2009q1/bin/arm-none-linux-gnueabi-

# setup
cd $GAMS_ROOT
rm -rf drone_build
mkdir drone_build

# build ACE
cd $ACE_ROOT

echo "#include \"ace/config-linux.h\"" > $ACE_ROOT/ace/config.h
echo "CROSS_COMPILE=$ARM_COMPILE_PREFIX" > $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "no_hidden_visibility=1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo versioned_so=0 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo OCFLAGS=-Os >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo cross_compile=1 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo ARM=1 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

cd $ACE_ROOT/ace
perl $ACE_ROOT/bin/mwc.pl -type gnuace ace.mwc
make realclean
make -j $CORES
cp libACE.so $GAMS_ROOT/drone_build

# build MADARA
cd $MADARA_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace MADARA.mwc
make realclean
make -j $CORES
cp libMADARA.so $GAMS_ROOT/drone_build

# build GAMS
cd $GAMS_ROOT
mwc.pl -type gnuace gams.mwc
make realclean
make -j $CORES
cp libGAMS.so drone_build
cp gams_controller drone_build
