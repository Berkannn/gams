#!/bin/bash
# Build the required libraries for GAMS
#
# There are several expected environment variables
#   $CORES        - number of build jobs to launch with make
#   $ACE_ROOT     - location of local copy of ACE subversion repository from
#                   svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE
#                   Revision 97777 from Jun 6, 2014 is known to work.
#   $MADARA_ROOT  - location of local copy of MADARA git repository from
#                   http://madara.googlecode.com/svn/trunk/
#   $GAMS_ROOT    - location of this GAMS git repository

TESTS=0

if [ "$#" = 1 ]; then
  TESTS=1
fi

# echo build information
echo "Using $CORES build jobs"
echo "MADARA will be built from $MADARA_ROOT"
echo "ACE will be built from $ACE_ROOT"
echo "GAMS will be built from $GAMS_ROOT"
echo "TESTS has been set to $TESTS"
echo ""

# build ACE
echo "Building ACE"
echo "#include \"ace/config-linux.h\"" > $ACE_ROOT/ace/config.h
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" > $ACE_ROOT/include/makeinclude/platform_macros.GNU
cd $ACE_ROOT/ace
perl $ACE_ROOT/bin/mwc.pl -type gnuace ace.mwc
make realclean -j $CORES
make -j $CORES

# build MADARA
echo "Building MADARA"
cd $MADARA_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace -features tests=$TESTS MADARA.mwc
make realclean -j $CORES
make tests=$TESTS -j $CORES

# build GAMS
echo "Building GAMS"
cd $GAMS_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace -features tests=$TESTS gams.mwc
make realclean -j $CORES
make vrep=1 tests=$TESTS -j $CORES

