#Author: persist
#Abstract: how to implement specific android image based live mode

# Prepare
```
.
├── boot
│   └── grub
│       ├── efi.img
│       └── grub.cfg
├── initrd.img
├── isolinux
│   ├── android-x86.png
│   ├── isolinux.bin
│   ├── isolinux.cfg
│   └── vesamenu.c32
├── kernel
├── ramdisk.img
└── system.sfs
```

# Remove useless install menu entries

## edit isolinux/isolinux.cfg
```
default vesamenu.c32
timeout 0
default livem

menu background android-x86.png
menu title Android-x86 Live & Installation CD 6.0-r3
menu color border 0 #ffffffff #00000000
menu color sel 7 #ffffff00 #ff000000
menu color title 0 #ffffffff #00000000
menu color tabmsg 0 #ffffffff #00000000
menu color unsel 0 #ffffffff #00000000
menu color hotsel 0 #ffffff00 #ff000000
menu color hotkey 7 #ffffff00 #00000000

label livem
	menu label Live CD - ^Run Android-x86 without installation
	kernel /kernel
	append initrd=/initrd.img root=/dev/ram0 quiet SRC= DATA=

```
## edit boot/grub/grub.cfg
```
set timeout=0
set default=0

menuentry 'Android-x86 6.0-r3 Live' --class android-x86 {
	search --file --no-floppy --set=root /system.sfs
	linux /kernel root=/dev/ram0 quiet DATA=
	initrd /initrd.img
}

```

# Make initrd - /

## Modify /

### edit init
```
#!/bin/busybox sh
#
# By Chih-Wei Huang <cwhuang@linux.org.tw>
# and Thorsten Glaser <tg@mirbsd.org>
#
# Last updated 2017/04/23
#
# License: GNU Public License
# We explicitely grant the right to use the scripts
# with Android-x86 project.
#

PATH=/sbin:/bin:/system/bin:/system/xbin; export PATH

# configure debugging output
LOG=/dev/null
test -e "$LOG" || busybox mknod $LOG c 1 3
exec 2>> $LOG

# early boot
if test x"$HAS_CTTY" != x"Yes"; then
	# initialise /proc and /sys
	busybox mount -t proc proc /proc
	busybox mount -t sysfs sys /sys
	# let busybox install all applets as symlinks
	busybox --install -s
	# initialise /dev (first time)
	mkdir -p /dev/block
	echo /sbin/mdev > /proc/sys/kernel/hotplug
	mdev -s
	# re-run this script with a controlling tty
	exec env HAS_CTTY=Yes setsid cttyhack /bin/sh "$0" "$@"
fi

# now running under a controlling tty; debug output from stderr into log file
# boot up Android

try_mount()
{
	RW=$1; shift
	if [ "${ROOT#*:/}" != "$ROOT" ]; then
		# for NFS roots, use nolock to avoid dependency to portmapper
		RW="nolock,$RW"
	fi
	# FIXME: any way to mount ntfs gracefully?
	mount -o $RW,noatime $@ || mount.ntfs-3g -o rw,force $@
}

check_root()
{
	if [ "`dirname $1`" = "/dev" ]; then
		[ -e $1 ] || return 1
		blk=`basename $1`
		[ ! -e /dev/block/$blk ] && ln $1 /dev/block
		dev=/dev/block/$blk
	else
		dev=$1
	fi
	try_mount ro $dev /mnt || return 1
	if [ -n "$iso" -a -e /mnt/$iso ]; then
		mount --move /mnt /iso
		mkdir /mnt/iso
		mount -o loop /iso/$iso /mnt/iso
		SRC=iso
	fi
	if [ -e /mnt/$SRC/$RAMDISK ]; then
		zcat /mnt/$SRC/$RAMDISK | cpio -id > /dev/null
	elif [ -b /dev/$RAMDISK ]; then
		zcat /dev/$RAMDISK | cpio -id > /dev/null
	else
		return 1
	fi
	if [ -e /mnt/$SRC/system.sfs ]; then
		mount -o loop,noatime /mnt/$SRC/system.sfs /sfs
		mount -o loop,noatime /sfs/system.img system
	elif [ -e /mnt/$SRC/system.img ]; then
		remount_rw
		mount -o loop,noatime /mnt/$SRC/system.img system
	elif [ -d /mnt/$SRC/system ]; then
		remount_rw
		mount --bind /mnt/$SRC/system system
	elif [ -e /mnt/build.prop ]; then
		mount --bind /mnt system
	else
		rm -rf *
		return 1
	fi
	mkdir -p mnt
	echo " found at $1"
	rm /sbin/mke2fs
	hash -r
}

remount_rw()
{
	# "foo" as mount source is given to workaround a Busybox bug with NFS
	# - as it's ignored anyways it shouldn't harm for other filesystems.
	mount -o remount,rw foo /mnt
}


echo -n Detecting Android-x86...

[ -z "$SRC" -a -n "$BOOT_IMAGE" ] && SRC=`dirname $BOOT_IMAGE`
[ -z "$RAMDISK" ] && RAMDISK=ramdisk.img || RAMDISK=${RAMDISK##/dev/}

mount -t tmpfs tmpfs /android
cd /android
while :; do
	for device in ${ROOT:-/dev/[hmsv][dmr][0-9a-z]*}; do
		check_root $device && break 2
		mountpoint -q /mnt && umount /mnt
	done
	sleep 1
	echo -n .
done

ln -s mnt/$SRC /src
ln -s android/system /
ln -s ../system/lib/firmware ../system/lib/modules /lib

# load scripts
for s in `ls /scripts/* /src/scripts/*`; do
	test -e "$s" && source $s
done

# A target should provide its detect_hardware function.
# On success, return 0 with the following values set.
# return 1 if it wants to use auto_detect
[ "$AUTO" != "1" ] && detect_hardware && FOUND=1

load_modules
mount_data
mount_sdcard
setup_tslib
setup_dpi
post_detect

[ "$SETUPWIZARD" = "0" ] && echo "ro.setupwizard.mode=DISABLED" >> default.prop

# We must disable mdev before switching to Android
# since it conflicts with Android's init
echo > /proc/sys/kernel/hotplug

export ANDROID_ROOT=/system

exec ${SWITCH:-switch_root} /android /init

# avoid kernel panic
while :; do
	echo
	echo '	Android-x86 console shell. Use only in emergencies.'
	echo
	debug_shell fatal-err
done

```

## Build initrd.img
``` bash
$ find . | cpio --quiet -H newc -o | gzip -9 -n > ../initrd.img
```

# Make ramdisk - /root

## Modify /root


## Build ramdisk image ramdisk.img
``` bash
$ find . | cpio --quiet -H newc -o | gzip -9 -n > ../ramdisk.img
```

# Make system - /system

## Modify /system


## Build system image system.img
``` bash
$ make_ext4fs -T file_contexts -l 1024M -a system system.img system/
$ mksquashfs system.img system.sfs

```

# Make kernel

## Modify kernel

## Build kernel

# Build android-x86 image android-x86.iso
``` bash
$ mkisofs -o .././output//android-x86.iso -r -J -T -V "Android x86 Build" -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -eltorito-alt-boot  -e boot/grub/efi.img  -no-emul-boot .
```


# Run
$ qemu-system-x86_64 -smp 2 -m 4096 -enable-kvm  ./output//test.img -cdrom ./output//android-x86.iso  -vga std -net nic -net user
