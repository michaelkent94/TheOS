#/bin/bash
set -e
. ./build.sh
 
mkdir -p isodir
mkdir -p isodir/boot
mkdir -p isodir/boot/grub
 
cp sysroot/boot/theos.kernel isodir/boot/theos.kernel
cat > isodir/boot/grub/grub.cfg << EOF
menuentry "TheOS" {
	multiboot /boot/theos.kernel
}
EOF
grub-mkrescue -o theos.iso isodir
