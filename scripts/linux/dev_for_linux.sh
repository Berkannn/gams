#!/bin/bash

# quit on failure
set -e

# number of build jobs
CORES=8

# build ACE
cd $ACE_ROOT

echo "#include \"ace/config-linux.h\"" > $ACE_ROOT/ace/config.h
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" > $ACE_ROOT/include/makeinclude/platform_macros.GNU

cd $ACE_ROOT/ace
perl $ACE_ROOT/bin/mwc.pl -type gnuace ace.mwc
make realclean -j $CORES
make -j $CORES

# build MADARA
cd $MADARA_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace MADARA.mwc
make realclean -j $CORES
make -j $CORES

# build GAMS
cd $GAMS_ROOT
mwc.pl -type gnuace gams.mwc
make realclean -j $CORES
make -j $CORES
