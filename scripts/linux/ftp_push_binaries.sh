#!/bin/bash
# copies files from localhost to drone at IP
#
# build_ar_drone.sh will build the necessary files and place them in the 
# location that this script expects

# bring in other variables
source $GAMS_ROOT/scripts/linux/common.sh

# setup ip address
IP=192.168.1.1
echo "IP start as $IP"

if [ ! -z $1 ]; then
    IP=$1
    echo "FTP IP is set to $IP"
fi

# copy scripts file
cd $GAMS_ROOT/scripts/linux
ftp -n -v $IP << END_SCRIPTS_FTP

binary

delete swarmSetup.sh
delete hosts

put swarmSetup.sh
put hosts

quit

END_SCRIPTS_FTP

# mark scripts as executable
telnet $IP << END_SCRIPTS_TELNET

cd data/video
chmod a+x swarmSetup.sh 
cp hosts /etc
exit

END_SCRIPTS_TELNET

# copy madara to drone
cd $DRONE_DIR
ftp -n -v $IP << END_MADARA_FTP

binary

delete libMADARA.so 
delete network_profiler 
delete test_file_rebroadcasts 
delete test_fragmentation 
delete test_rebroadcast_ring
delete test_reasoning_throughput 
delete test_udp 
delete test_broadcast 
delete profile_architecture
delete madara_version

put libMADARA.so 
put network_profiler 
put test_file_rebroadcasts 
put test_fragmentation 
put test_rebroadcast_ring
put test_reasoning_throughput 
put test_udp 
put test_broadcast 
put profile_architecture
put madara_version

quit

END_MADARA_FTP

# set madara files as executable
telnet $IP << END_MADARA_TELNET

cd /data/video
chmod a+x network_profiler 
chmod a+x test_file_rebroadcasts 
chmod a+x test_fragmentation 
chmod a+x test_rebroadcast_ring
chmod a+x test_reasoning_throughput 
chmod a+x test_udp 
chmod a+x test_broadcast 
chmod a+x profile_architecture
chmod a+x madara_version
exit

END_MADARA_TELNET

# copy ace to drone
ftp -n -v $IP << END_ACE_FTP

binary
delete libACE.so
put libACE.so
quit

END_ACE_FTP

exit 0
