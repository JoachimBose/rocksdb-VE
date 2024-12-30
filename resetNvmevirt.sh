#!/bin/bash

set -e

umount /dev/nvme0n1
#mke2fs -t ext4 -F /dev/nvme0n1
mkfs.xfs -f /dev/nvme0n1
mount -t auto /dev/nvme0n1 /local/nvmevirt/
