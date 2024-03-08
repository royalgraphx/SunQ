#!/bin/bash

echo "HDSpawn (SunQ Edition)"
echo "Bash Version 1.0"
echo

read -p "Disk format (default: raw, qcow2): " disk_format
disk_format=${disk_format:-raw}

read -p "Name of the disk image to create: " disk_name

read -p "Size of the disk image (with suffixes k, M, G, T, P, E): " disk_size

echo
echo "Creating disk image..."

# Run qemu-img command to create disk image
qemu-img create -f "$disk_format" "harddrives/$disk_name.img" "$disk_size"
