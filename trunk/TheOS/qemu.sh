#/bin/bash
set -e
. ./iso.sh
 
qemu-system-$(./target-triplet-to-arch.sh $HOST) -m 4096 -cdrom theos.iso
