#!/bin/bash
# Build the required libraries for the ARM target Parrot AR.Drone 2.0
#
# There are several expected environment variables
#   $ARM_PREFIX   - full path prefix of arm compiler
#                   ex. ~/arm-none-linux-gnueabi-
#                   requires that ${ARM_PREFIX}g++ and ${ARM_PREFIX}strip are
#                   available
#   $CORES        - number of build jobs to launch with make
#   $ACE_ROOT     - location of local copy of ACE subversion repository from
#                   svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE
#                   Revision 97777 from Jun 6, 2014 is known to work
#   $MADARA_ROOT  - location of local copy of MADARA git repository from
#                   http://madara.googlecode.com/svn/trunk/
#   $GAMS_ROOT    - location of this GAMS git repository

# echo build information
echo "Using ARM prefix of $ARM_PREFIX with $CORES build jobs"
echo "MADARA will be built from $MADARA_ROOT"
echo "ACE will be built from $ACE_ROOT"
echo "GAMS will be built from $GAMS_ROOT"
echo ""

# prepare destination directory
cd $GAMS_ROOT
DRONE_DIR=$GAMS_ROOT/drone_files
rm -rf $DRONE_DIR
mkdir $DRONE_DIR

# building ACE
echo "Configuring ACE for ARM"

echo "#include \"ace/config-linux.h\"" > $ACE_ROOT/ace/config.h
echo "CROSS_COMPILE=$ARM_PREFIX" > $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "no_hidden_visibility=1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo versioned_so=0 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo OCFLAGS=-Os >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo cross_compile=1 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo ARM=1 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

echo "Building ACE"
cd $ACE_ROOT/ace
perl $ACE_ROOT/bin/mwc.pl -type gnuace ace.mwc
make realclean
make -j $CORES
cp libACE.so $DRONE_DIR

# build MADARA
echo ""
echo "Building MADARA"
cd $MADARA_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace MADARA.mwc
make realclean
make tests=0 -j $CORES
cp libMADARA.so $DRONE_DIR

# build GAMS
echo ""
echo "Build GAMS"
cd $GAMS_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace gams.mwc 
make realclean
make dronerk=1 -j $CORES
cp gams_controller $DRONE_DIR

# strip libraries and binaries
echo "Strip files"
${ARM_PREFIX}strip $DRONE_DIR/*
