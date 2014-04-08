#!/bin/bash

echo "Building with ARM_ROOT=$ARM_ROOT"

ROOT=$HOME

echo "ROOT start as $ROOT"

if [ ! -z $1 ]; then
    ROOT=$1
    echo "Root directory is set to $ROOT"
fi    


export MADARA_ROOT=$ROOT/madara
export ACE_ROOT=$ROOT/ace/ACE_wrappers

echo "MADARA will be built in $MADARA_ROOT"
echo "ACE will be built in $ACE_ROOT"

rm -rf $MADARA_ROOT
rm -rf $ACE_ROOT

cd $ROOT

echo "Checking out ACE to $ACE_ROOT"

#current version of ACE
svn co -r 95000 svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE ace

#known good version of ACE at 95000
#svn co -r 95000 svn://svn.dre.vanderbilt.edu/DOC/Middleware/sets-anon/ACE ace

echo "Checking out MADARA to $MADARA_ROOT"

svn co https://madara.googlecode.com/svn/trunk/ madara

echo "Configuring ACE for ARM"

cd $ACE_ROOT

echo "#include \"ace/config-linux.h\"" > $ACE_ROOT/ace/config.h

echo "CROSS_COMPILE=$ARM_ROOT/bin/arm-none-linux-gnueabi-" > $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "no_hidden_visibility=1" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo versioned_so=0 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo OCFLAGS=-Os >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo cross_compile=1 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo ARM=1 >> $ACE_ROOT/include/makeinclude/platform_macros.GNU
echo "include \$(ACE_ROOT)/include/makeinclude/platform_linux.GNU" >> $ACE_ROOT/include/makeinclude/platform_macros.GNU

echo "Generating ACE makefiles and building"
cd $ACE_ROOT/ace
perl $ACE_ROOT/bin/mwc.pl -type gnuace ace.mwc
make 

echo "Generating MADARA makefiles and building"
cd $MADARA_ROOT
perl $ACE_ROOT/bin/mwc.pl -type gnuace MADARA.mwc
make tests=1

