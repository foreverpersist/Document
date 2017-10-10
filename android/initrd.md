#Author: persist
#Abstract: explain what initrd.img does

# Function

	Create initramdisk filesystem, and install or run android.
	>* mount real filesystem: mount ramdisk.img to /, and mount  system.sfs/system.img to system.
	>* Decide what to do by $INSTALL: install android to disk with the guide of install.img, or just run android on live.
	

# Structure

```
.
├── android
├── bin
│   ├── busybox
│   ├── ld-linux.so.2
│   └── lndir
├── hd
├── init
├── iso
├── lib
│   ├── ld-linux.so.2 -> /bin/ld-linux.so.2
│   ├── libcrypt.so.1
│   ├── libc.so.6
│   ├── libdl.so.2
│   ├── libm.so.6
│   ├── libntfs-3g.so.31
│   ├── libpthread.so.0
│   └── librt.so.1
├── mnt
├── proc
├── sbin
│   └── mount.ntfs-3g
├── scripts
│   ├── 00-ver
│   ├── 0-auto-detect
│   ├── 1-install
│   ├── 2-mount
│   ├── 3-tslib
│   └── 4-dpi
├── sfs
├── sys
└── tmp
```


# Pack

``` bash
$ find . | cpio --quiet -H newc -o | gzip -9 -n > ../initrd.img
```


# Unpack

``` bash
$ zcat ../initrd.img | cpio -iud
```

# Key

## init

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
# 配置LOG
if [ -n "$DEBUG" ]; then
	LOG=/tmp/log
	set -x
else
	LOG=/dev/null
	test -e "$LOG" || busybox mknod $LOG c 1 3
fi
exec 2>> $LOG

# early boot
# 检查tty环境,若无tty,开启cttyhack启动一个tty重新执行此脚本
if test x"$HAS_CTTY" != x"Yes"; then
	# initialise /proc and /sys
	# 挂载proc和sys
	busybox mount -t proc proc /proc
	busybox mount -t sysfs sys /sys
	# let busybox install all applets as symlinks
	# 安装busybox，为其内部各种工具(如ls,mkdir)创建软连接(指向busybox)
	busybox --install -s
	# spawn shells on tty 2 and 3 if debug or installer
	# initialise /dev (first time)
	# 初始化mdev(在嵌入式场景下替代linux原生的udev)
	mkdir -p /dev/block
	echo /sbin/mdev > /proc/sys/kernel/hotplug
	mdev -s
	# re-run this script with a controlling tty
	# 调用cttyhack重现执行此脚本
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

check_root()
{
	if [ "`dirname $1`" = "/dev" ]; then
		# 若/dev不存在,则失败
		[ -e $1 ] || return 1
		blk=`basename $1`
		# 若/dev/block/$blk不存在, 将/dev/$blk连接到/dev/block
		[ ! -e /dev/block/$blk ] && ln $1 /dev/block
		dev=/dev/block/$blk
	else
		dev=$1
	fi
	# 以只读方式挂载/$dev到/mnt,若挂载失败，则失败
	try_mount ro $dev /mnt || return 1
	# 若iso非空且/mnt/$iso存在
	if [ -n "$iso" -a -e /mnt/$iso ]; then
		# 将/mnt移动到/iso
		mount --move /mnt /iso
		# 将/iso/$iso移动到/mnt/iso
		# 至此,/iso为整体内容挂载点,/mnt/iso为$iso挂载点
		mkdir /mnt/iso
		mount -o loop /iso/$iso /mnt/iso
		SRC=iso
	fi
	# 从/mnt/$SRC/$RAMDISK或/dev/$RAMDISK提取RAMDISK，
	# 两处均提取不到，则失败
	if [ -e /mnt/$SRC/$RAMDISK ]; then
		zcat /mnt/$SRC/$RAMDISK | cpio -id > /dev/null
	elif [ -b /dev/$RAMDISK ]; then
		zcat /dev/$RAMDISK | cpio -id > /dev/null
	else
		return 1
	fi
	# *unknown*
	# 提取SYSTEM挂载到/android/system
	# 来源可以是system.sfs,system.img,system,/mnt,其中
	# system.img,system会挂载为读写模式(为何区别对待???)
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
	# 创建目录/android/mnt
	mkdir -p mnt
	echo " found at $1"
	# mke2fs用于建立ext2,ext3文件系统
	rm /sbin/mke2fs
	# 清除命令hash表
	hash -r
}

remount_rw()
{
	# "foo" as mount source is given to workaround a Busybox bug with NFS
	# - as it's ignored anyways it shouldn't harm for other filesystems.
	mount -o remount,rw foo /mnt
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

# 设置SRC,若SRC空且BOOT_IMAGE非空，则SRC为包含BOOT_IMAGE的目录
[ -z "$SRC" -a -n "$BOOT_IMAGE" ] && SRC=`dirname $BOOT_IMAGE`
# 设置RAMDISK,若RAMDISK空，则RAMDISK为ramdisk.img；RAMDISK非空，则
# RAMDISK为去掉/dev/前缀后的值
[ -z "$RAMDISK" ] && RAMDISK=ramdisk.img || RAMDISK=${RAMDISK##/dev/}

# /proc/cmdline内容是isolinux.cfg传入的参数(append之后)
for c in `cat /proc/cmdline`; do
	case $c in
		iso-scan/filename=*)
			#执行eval `iso=*`这种形式的命令
			eval `echo $c | cut -b1-3,18-`
			;;
		*)
			;;
	esac
done
# 挂载tmpfs到/android
mount -t tmpfs tmpfs /android
cd /android
while :; do
	# ROOT空，则ROOT为/dev/[hmsv][dmr][0-9a-z]*
	for device in ${ROOT:-/dev/[hmsv][dmr][0-9a-z]*}; do
		# check_root成功，则跳出两层循环
		check_root $device && break 2
		# (check_root失败)检查/mnt,若为挂载点,则取消挂载
		mountpoint -q /mnt && umount /mnt
	done
	sleep 1
	echo -n .
done

# 当前工作目录在/android

# 将mnt/$SRC连接到/src
ln -s mnt/$SRC /src
# 将android/system 连接到/
ln -s android/system /
# 将../system/lib/firmware和../system/lib/modules连接到/lib
ln -s ../system/lib/firmware ../system/lib/modules /lib

if [ -n "$INSTALL" ]; then
	zcat /src/install.img | ( cd /; cpio -iud > /dev/null )
fi

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
# 加载并执行/scripts和/src/scripts下脚本
for s in `ls /scripts/* /src/scripts/*`; do
	test -e "$s" && source $s
done

# A target should provide its detect_hardware function.
# On success, return 0 with the following values set.
# return 1 if it wants to use auto_detect
[ "$AUTO" != "1" ] && detect_hardware && FOUND=1

#[ -n "$INSTALL" ] && do_install

# 加载$EXTMOD中模块
load_modules
# 挂载data.img(实际为空,挂载tmpfs到data)
mount_data
# 挂载sdcard(实际为空)
mount_sdcard
# 安装pointercal???
setup_tslib
# 用ro.sf.lcd_density=$DPI替换default.prop属性
setup_dpi
# *unknown*
# ???
post_detect


# 是否设置向导
[ "$SETUPWIZARD" = "0" ] && echo "ro.setupwizard.mode=DISABLED" >> default.prop

[ -n "$DEBUG" ] && SWITCH=${SWITCH:-chroot}

# We must disable mdev before switching to Android
# since it conflicts with Android's init
# 关闭mdev
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
