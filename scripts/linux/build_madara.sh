#!/bin/bash

rm -rf $MADARA_ROOT

rm -rf $ACE_ROOT

echo "Checking out ACE to $ACE_ROOT"

svn co svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE ace

echo "Checking out MADARA to $MADARA_ROOT"

svn co https://madara.googlecode.com/svn/trunk/ madara


cd $ACE_ROOT/ace
echo "#include \"ace/config-linux.h\"" > config.h
cd $ACE_ROOT/include/makeinclude
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" > platform_macros.GNU
cd $ACE_ROOT/ace
perl $ACE_ROOT/bin/mwc.pl -type gnuace ace.mwc
make 

cd $MADARA_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace MADARA.mwc
make tests=1

