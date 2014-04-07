#!/bin/bash

arm-none-linux-gnueabi-strip $MADARA_ROOT/libMADARA.so
arm-none-linux-gnueabi-strip $MADARA_ROOT/network_profiler
arm-none-linux-gnueabi-strip $MADARA_ROOT/test_file_rebroadcasts
arm-none-linux-gnueabi-strip $MADARA_ROOT/test_fragmentation
arm-none-linux-gnueabi-strip $MADARA_ROOT/test_rebroadcast_ring
arm-none-linux-gnueabi-strip $MADARA_ROOT/test_reasoning_throughput
arm-none-linux-gnueabi-strip $MADARA_ROOT/test_udp
arm-none-linux-gnueabi-strip $MADARA_ROOT/test_broadcast
arm-none-linux-gnueabi-strip $MADARA_ROOT/profile_architecture
arm-none-linux-gnueabi-strip $MADARA_ROOT/madara_version
arm-none-linux-gnueabi-strip $ACE_ROOT/ace/libACE.so
