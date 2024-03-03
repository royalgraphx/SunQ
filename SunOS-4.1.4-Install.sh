#!/bin/bash
#
# Copyright (c) 2024 RoyalGraphX.
# BSD 3-Clause License
#

clear

qemu-system-sparc -name "SunOS 4.1.4 SPARCStation 5" \
   -machine SS-5 \
   -smp 1,sockets=1,cores=1,threads=1 \
   -m 64 \
   -boot d \
   -net nic,netdev=net0,model=lance \
   -netdev vmnet-bridged,id=net0,ifname=en0 \
   -drive format=raw,file=harddrives/sunos414.img,bus=0,unit=3,media=disk \
   -device scsi-cd,channel=0,scsi-id=6,id=cdrom,drive=cdrom,physical_block_size=512 \
   -drive if=none,file=installers/SunOS-v4.1.4.iso,media=cdrom,id=cdrom \
   -vga tcx \
   -monitor unix:qemu-monitor-socket,server,nowait \
   -serial stdio \