#Author: persist
#Abstract: explain how to implement specific android image based live mode

------------------------------------------

# **Stage 0**

	*Remove useless install menu entries, and set timeout 0*

------------------------------------------

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

## Edit isolinux/isolinux.cfg

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

## Edit boot/grub/grub.cfg
```
set timeout=0
set default=0

menuentry 'Android-x86 6.0-r3 Live' --class android-x86 {
	search --file --no-floppy --set=root /system.sfs
	linux /kernel root=/dev/ram0 quiet DATA=
	initrd /initrd.img
}

```


------------------------------------------

# **Stage 1**

	*Simplify init shell script to run android on live directly*

------------------------------------------

# Edit init

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

------------------------------------------

# **Stage 2**

	*Support light OS image structure*

------------------------------------------

# Structure

## iso
```
.
├── boot
│   └── grub
│       ├── efi.img
│       └── grub.cfg
├── efi
│   └── boot
│       ├── bootia32.efi
│       ├── BOOTx64.EFI
│       └── grubx64.efi
├── initrd.img
├── isolinux
│   ├── android-x86.png
│   ├── isolinux.bin
│   ├── isolinux.cfg
│   └── vesamenu.c32
└── kernel
```

## initrd+kernel
```
.
├── initrd.img
└── kernel
```

# Make initrd.img

## Merge old initrd.img and ramdisk.img
	old-initrd is the mountpoint of old initrd, and old-ramdisk is the mountpoint of old ramdisk.img

``` bash
$ cp -r old-ramdisk new-initrd
$ cp -r old-initrd/bin new-initrd/
$ cp -r old-initrd/lib new-initrd/
$ cp -r old-initrd/scripts new-initrd/
$ cp -r old-initrd/sbin/* new-initrd/sbin/
```

## Create new initrd script rdinit in new-initrd/

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

error()
{
	echo $*
	return 1
}

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

check_mount()
{
	if [ "`dirname $1`" = "/dev" ]; then
		[ -e $1 ] || return 1
		blk=`basename $1`
		[ ! -e /dev/block/$blk ] && ln $1 /dev/block
		dev=/dev/block/$blk
	else
		dev=$1
	fi
	# dev=$1
	dir=/mnt/dev$dev_index
	
	if [ ! -d $dir ]; then
		mkdir $dir
	fi
	
	try_mount ro $dev $dir || return 1

	blank=1
	if [ -n "$iso" -a -e $dir/$iso ]; then
		mkdir /iso
		mount --move $dir /iso
		mkdir $dir/iso
		mount -o loop /iso/$iso $dir/iso
		SRC=iso
	fi
	
	# check system.sfs/system.img
	if [ -e $dir/$SRC/system.sfs ]; then
		[ $blank != 0 ] && blank=0
		mkdir /sfs
		mount -o loop,noatime $dir/$SRC/system.sfs /sfs
		mount_label /sfs/system.img system
	elif [ -e $dir/$SRC/system.img ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/system.img system
	elif [ -d $dir/$SRC/system ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mountpoint -q system || mount --bind $dir/$SRC/system system
	elif [ -e $dir/build.prop ]; then
		[ $blank != 0 ] && blank=0
		mountpoint -q system || mount --bind $dir system
	fi
	
	# check userdata.img
	if [ -e $dir/$SRC/$DATA ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/$DATA data
	fi
	
	# check cache.img
	if [ -e $dir/$SRC/$CACHE ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/$CACHE cache
	fi
	
	# check sdcard.img
	if [ -e $dir/$SRC/$SDCARD ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/$SDCARD sdcard
	fi
	
	if [ $blank != 0 ]; then
		umount $dir
		# check initrd.img
		if [ -e $dir/$SRC/initrd.img ]; then
			mount_label $dev sdcard
		fi

	else
		dev_index=$((dev_index+1))
	fi
	echo " found at $1"
	rm /sbin/mke2fs
	hash -r
}

remount_rw()
{
	# "foo" as mount source is given to workaround a Busybox bug with NFS
	# - as it's ignored anyways it shouldn't harm for other filesystems.
	mount -o remount,rw foo $dir
}

# usage: mount_label dev label
mount_label()
{
	mountpoint -q $2 && echo "$2 has been mounted" && return
	if [ ! -e $2 ]
	then
		mkdir $2
	fi
	mount -o loop,noatime $1 $2
	echo "mount -o loop,noatime $1 $2 : $?"
	sleep 10
}

debug_shell()
{
	if [ -x system/bin/sh ]; then
		echo Running MirBSD Korn Shell...
		USER="($1)" system/bin/sh -l 2>&1
	else
		echo Running busybox ash...
		sh 2>&1
	fi
}

echo -n Detecting Android-x86...

[ -z "$SRC" -a -n "$BOOT_IMAGE" ] && SRC=`dirname $BOOT_IMAGE`
# [ -z "$RAMDISK" ] && RAMDISK=ramdisk.img
[ -z "$DATA" ] && DATA=userdata.img 
[ -z "$CACHE" ] && CACHE=cache.img
[ -z "$SDCARD" ] && SDCARD=sdcard.img


for c in `cat /proc/cmdline`; do
	case $c in
		iso-scan/filename=*)
			eval `echo $c | cut -b1-3,18-`
			;;
		*)
			;;
	esac
done

mkdir /mnt

# dev_index: mountpoint index under /mnt
dev_index=0

# search block devices
echo "..."
blkid > tmp
while read line
do
	# $line is like "/dev/sda: LABEL=label UUID=uuid TYPE=type"
	dev=${line%%:*}
	label=`expr "$line" : '.*LABEL="\([^"]*\)"'`

	case $label in
		system)
			# this is a device consist of system.img, mount it to system
			mount_label $dev system
			;;
		data)
			# this is a device consist of userdata.img, mount it to data
			mount_label $dev data
			;;
		cache)
			# this is a device consist of cache.img, mount it to cache
			mount_label $dev sdcard
			;;
		sdcard)
			# this is a device consist of sdcard.img, mount it to sdcard
			mount_label $dev sdcard
			;;
		*)
			# this is a unknown device, there are many cases
			# 1) system.sfs/system.img [data.img] [cache.img] [sdcard.img] others
			# 2) sdcard.img
			check_mount $dev
			;;
	esac
done < tmp

rm tmp


ln -s mnt/dev$dev_index/$SRC /src
ln -s android/system /
ln -s ../system/lib/firmware ../system/lib/modules /lib

if [ -x system/bin/ln -a \( -n "$DEBUG" -o -n "$BUSYBOX" \) ]; then
	mv /bin /lib .
	sed -i 's|\( PATH.*\)|\1:/bin|' init.environ.rc
	rm /sbin/modprobe
	busybox mv /sbin/* sbin
	rmdir /sbin
	ln -s android/bin android/lib android/sbin /
	hash -r
fi

# load scripts
for s in `ls /scripts/* /src/scripts/*`; do
	test -e "$s" && source $s
done

# A target should provide its detect_hardware function.
# On success, return 0 with the following values set.
# return 1 if it wants to use auto_detect
[ "$AUTO" != "1" ] && detect_hardware && FOUND=1


load_modules
# mount_data
mountpoint -q data || mount -t tmpfs tmpfs data
# mount_sdcard
mountpoint -q sdcard || mount -t tmpfs tmpfs sdcard
setup_tslib
setup_dpi
post_detect

[ "$SETUPWIZARD" != "1" ] && echo "ro.setupwizard.mode=DISABLED" >> default.prop

# We must disable mdev before switching to Android
# since it conflicts with Android's init
echo > /proc/sys/kernel/hotplug

export ANDROID_ROOT=/system

chmod a+rw /dev/tty
/init

# avoid kernel panic
while :; do
	echo
	echo '	Android-x86 console shell. Use only in emergencies.'
	echo
	debug_shell fatal-err
done
```
	
## Set kernel option: rdinit=/rdini

### iso
	append option in isolinux/isolinux.cfg

```

label livem
	menu label Live CD - ^Run Android-x86 without installation
	kernel /kernel
	append rdinit=/rdinit initrd=/initrd.img root=/dev/ram0 
```

### initrd+kernel 
	run qemu with option: -append "rdinit=/rdinit root=dev/ram0"

# Run

	Mount /system partition from external disk

## iso

``` bash
$ qemu-system-x86_64 -smp 2 -m 4096 -enable-kvm -vga std android-x86.iso -drive file=system.sfs
```

## initrd+kernel

``` bash
$ qemu-system-x86_64 -smp 2 -m 4096 -enable-kvm -vga std -kernel kernel -initrd initrd.img -append "rdinit=/rdinit root=/dev/ram0 quiet" -drive file=system.sfs 
```

# Note

	You can also mount other partitions (/data, /cache, /sdcard) from external disks.

	/data:		-drive file=userdata.img
	/cache:		-drive file=cache.img
	/sdcard:	-drive file=sdcard.img

	Drive filename does not matter, you can use any filename you like. Drives are recognized by their volume names, Only "system", "data", "cache", "sdcard" is supportted, any volume name else will be seen as "sdcard".


------------------------------------------

# **Stage 3**

	*Simplify system image to remove useless apks, append `containerd` and `runc`*

------------------------------------------


# Remove all applications in app/

``` bash
$ rm -rf app/*
```

# Remove most applications in priv-app

	just keep `PackageInstaller/`, `SettingsProvider/`, `StatementService/`, `SystemUI/`?

# Append containerd into bin/

	Extract bin/{containerd, container-shim, ctr, service}. Extract localtime in etc/ and create directory etc whose directory tree is like following structure.

```
etc
├── containerd
│   └── config.toml
├── init.d
└── localtime
```

	Then edit etc/containerd/config.toml.

```
state = "/system/run/containerd"
root = "/system/var/lib/containerd"
snapshotter = "io.containerd.snapshotter.v1.overlayfs"
differ = "io.containerd.differ.v1.base-diff"
subreaper = false

[grpc]
  address = "/system/run/containerd/containerd.sock"
  uid = 0
  gid = 0

[debug]
  address = "/system/run/containerd/debug.sock"
  level = "info"

[metrics]
  address = ""

[plugins.linux]
  runtime = "runc"
  shim_debug = true

```

	Copy bin/* and etc/* to system/

``` bash
$ cp bin/* system/bin/
$ cp -r etc/* system/etc/
```

# Append runc into bin/

	Download runc.amd64 from https://github.com/opencontainers/runc/releases. Then copy it to bin/ in system.img. (Only runc which is compiled statically can work on anroid)

``` bash
$ cp runc.amd64 system/bin/runc
```


------------------------------------------

# **Stage 4**

	*Build runtime environment for runc*

------------------------------------------

# Support Namespace

	IPC Namespace is not supported by android, because android use binder for process communication.

	So, only 5 kinds of namespaces is supported on android. Just select them in kernel config file.
>* PID
>* UTS
>* USER
>* NET
>* MNT

# Support Cgroup

	Just select following cgroup subsystems in kernel config file.
>* blkio
>* cpu
>* cpuacct
>* cpuset
>* devices
>* freezer
>* memory

	Then mount them on android by modifing init.rc. (Subsystem `cpu`, `cpuacct`, `cpuset`, `memory` have already been mounted)

```
    # Create cgroup mount point for blkio
    mkdir /sys/fs/cgroup/blkio 0750 root system
    mount cgroup none /sys/fs/cgroup/blkio blkio
    
    # Create cgroup mount point for devices
    mkdir /sys/fs/cgroup/devices 0750 root system
    mount cgroup none /sys/fs/cgroup/devices devices
    
    # Create cgroup mount point for freezer
    mkdir /sys/fs/cgroup/freezer 0750 root system
    mount cgroup none /sys/fs/cgroup/freezer freezer
```

# Support pivot_root

	pivot_root, which runc uses normally will not work on a ramfs or tmpfs root filesystem. We can work around this by two means.

## Create new tmpfs as rootfs, then use switch_root

	Change iso structure, and edit init shell script. Finally, generate new initrd.img to replace the old one in light OS image structure.

### Pack ramdisk
	
	Suppose that `ramdisk/` is the folder of **native** ramdisk, pack it to ramdisk.img.

```
$ cd ramdisk/
$ find . | cpio --quiet -H newc -o | gzip -9 -n > ../ramdisk.img
```

### Put ramdisk.img into initrd 

	Suppose that `initrd/` is the folder of **native** initrd, copy ramdisk.img to `initrd/`.

```
$ cp ramdisk.img initrd/
$ cd initrd/
$ find . | cpio --quiet -H newc -o | gzip -9 -n > ../initrd.img
```

### Edit init shell script in initrd

	edit init shell script based on the `initrd` script which is used in previous light OS image structure.

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

error()
{
        echo $*
        return 1
}

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

check_mount()
{
        if [ "`dirname $1`" = "/dev" ]; then
                [ -e $1 ] || return 1
                blk=`basename $1`
                [ ! -e /dev/block/$blk ] && ln $1 /dev/block
                dev=/dev/block/$blk
        else
		dev=$1
	fi
	# dev=$1
	dir=mnt/dev$dev_index
	
	if [ ! -d $dir ]; then
		mkdir $dir
	fi
	
	try_mount ro $dev $dir || return 1
	 sleep 3
	blank=1
	if [ -n "$iso" -a -e $dir/$iso ]; then
		mkdir /iso
		mount --move $dir /iso
		mkdir $dir/iso
		mount -o loop /iso/$iso $dir/iso
		SRC=iso
	fi
	
	# check system.sfs/system.img
	if [ -e $dir/$SRC/system.sfs ]; then
		[ $blank != 0 ] && blank=0
		mkdir /sfs
		mount -o loop,noatime $dir/$SRC/system.sfs /sfs
		mount_label /sfs/system.img system
	elif [ -e $dir/$SRC/system.img ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/system.img system
	elif [ -d $dir/$SRC/system ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mountpoint -q system || mount --bind $dir/$SRC/system system
	elif [ -e $dir/build.prop ]; then
		[ $blank != 0 ] && blank=0
		mountpoint -q system || mount --bind $dir system
	fi
	
	# check userdata.img
	if [ -e $dir/$SRC/$DATA ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/$DATA data
	fi
	
	# check cache.img
	if [ -e $dir/$SRC/$CACHE ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/$CACHE cache
	fi
	
	# check sdcard.img
	if [ -e $dir/$SRC/$SDCARD ]; then
		[ $blank != 0 ] && blank=0 && remount_rw
		mount_label $dir/$SRC/$SDCARD sdcard
	fi
	
	if [ $blank != 0 ]; then
		# check initrd.img
		if [  -e $dir/$SRC/initrd.img ]; then
			# it is a iso img
			umount $dir
		else
			umount $dir
			mount_label $dev sdcard
		fi

	else
		dev_index=$((dev_index+1))
	fi
	echo " found at $1"
	rm /sbin/mke2fs
	hash -r
}

remount_rw()
{
	# "foo" as mount source is given to workaround a Busybox bug with NFS
	# - as it's ignored anyways it shouldn't harm for other filesystems.
	mount -o remount,rw foo $dir
}

# usage: mount_label dev label
mount_label()
{
	mountpoint -q $2 && echo "$2 has been mounted" && return
	if [ ! -e $2 ]
	then
		mkdir $2
	fi
	mount -o loop,noatime $1 $2
}

debug_shell()
{
	if [ -x system/bin/sh ]; then
		echo Running MirBSD Korn Shell...
		USER="($1)" system/bin/sh -l 2>&1 || sh 2>&1
	else
		echo Running busybox ash...
		sh 2>&1
	fi
}

echo -n Detecting Android-x86...

[ -z "$SRC" -a -n "$BOOT_IMAGE" ] && SRC=`dirname $BOOT_IMAGE`
[ -z "$RAMDISK" ] && RAMDISK=ramdisk.img || RAMDISK=${RAMDISK##/dev/}
[ -z "$DATA" ] && DATA=userdata.img 
[ -z "$CACHE" ] && CACHE=cache.img
[ -z "$SDCARD" ] && SDCARD=sdcard.img


for c in `cat /proc/cmdline`; do
	case $c in
		iso-scan/filename=*)
			eval `echo $c | cut -b1-3,18-`
			;;
		*)
			;;
	esac
done

# create tmpfs rootfs for runc
mount -t tmpfs rootfs /android
cd /android

# check whether ramdisk is in /
if [ -e /$RAMDISK ] 
then
	zcat /$RAMDISK | cpio -iud > /dev/null
fi

mkdir mnt

# dev_index: mountpoint index under /mnt
dev_index=0

# search block devices
blkid > tmp
while read line
do
	# $line is like "/dev/sda: LABEL=labek UUID=uuid TYPE=type"
	dev=${line%%:*}
	label=`expr "$line" : '.*LABEL="\([^"]*\)"'`

	case $label in
		system)
			# this is a device consist of system.img, mount it to system
			mount_label $dev system
			;;
		data)
			# this is a device consist of userdata.img, mount it to data
			mount_label $dev data
			;;
		cache)
			# this is a device consist of cache.img, mount it to cache
			mount_label $dev sdcard
			;;
		sdcard)
			# this is a device consist of sdcard.img, mount it to sdcard
			mount_label $dev sdcard
			;;
		*)
			# this is a unknown device, there are many cases
			# 1) system.sfs/system.img [data.img] [cache.img] [sdcard.img] others
			# 2) sdcard.img
			check_mount $dev
			;;
	esac
done < tmp

rm tmp


ln -s mnt/dev$dev_index/$SRC /src
ln -s android/system /
ln -s ../system/lib/firmware ../system/lib/modules /lib

if [ -x system/bin/ln -a \( -n "$DEBUG" -o -n "$BUSYBOX" \) ]; then
	mv /bin /lib .
	sed -i 's|\( PATH.*\)|\1:/bin|' init.environ.rc
	rm /sbin/modprobe
	busybox mv /sbin/* sbin
	rmdir /sbin
	ln -s android/bin android/lib android/sbin /
	hash -r
fi

# load scripts
for s in `ls /scripts/* /src/scripts/*`; do
	test -e "$s" && source $s
done

# A target should provide its detect_hardware function.
# On success, return 0 with the following values set.
# return 1 if it wants to use auto_detect
[ "$AUTO" != "1" ] && detect_hardware && FOUND=1


load_modules
#mount_data
mountpoint -q data || mount -t tmpfs tmpfs data
#mount_sdcard
mountpoint -q sdcard || mount -t tmpfs tmpfs sdcard
setup_tslib
setup_dpi
post_detect

[ "$SETUPWIZARD" != "1" ] && echo "ro.setupwizard.mode=DISABLED" >> default.prop

# We must disable mdev before switching to Android
# since it conflicts with Android's init
echo > /proc/sys/kernel/hotplug

export ANDROID_ROOT=/system

chmod a+x /system/bin/su
 
exec ${SWITCH:-switch_root} /android /init

# avoid kernel panic
while :; do
	echo
	echo '	Android-x86 console shell. Use only in emergencies.'
	echo
	debug_shell fatal-err
done
```

## Run runc with option: --no-pivot

``` bash
$ runc run --no-pivot hello
```

# Support devpts multiple instances

	Just select devpts multiple instances support in kernel config file.

```
CONFIG_DEVPTS_MULTIPLE_INSTANCES=y
```

------------------------------------------

# **Stage 5**

	*Optimize android boot time*

------------------------------------------