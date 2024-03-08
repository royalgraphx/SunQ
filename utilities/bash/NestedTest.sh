#!/bin/bash
#
# Copyright (c) 2024 RoyalGraphX.
# BSD 3-Clause License
#

clear

# Start qemu-system-sparc in the background
qemu-system-sparc -nodefaults \
   -machine SS-5 \
   -m 64 \
   -bios roms/ss5.bin \
   -drive format=raw,file=harddrives/sunos414.img,bus=0,unit=3,media=disk \
   -device scsi-cd,channel=0,scsi-id=6,id=cdrom,drive=cdrom,physical_block_size=512 \
   -drive if=none,file=installers/SunOS-v4.1.4.iso,media=cdrom,id=cdrom \
   -serial stdio \
   -monitor unix:qemu-monitor-socket,server,nowait &

# Monitor the creation of qemu-monitor-socket file
while [ ! -S "qemu-monitor-socket" ]; do
    sleep 1
done

# Execute socat when the file is found
socat -,echo=0,icanon=0 unix-connect:qemu-monitor-socket

# Check if qemu-system-sparc process is still running
while kill -0 $! 2>/dev/null; do
    sleep 1
done

# Clean up and exit when qemu-system-sparc process is terminated
clear
echo "qemu-system-sparc process terminated. Exiting..."
