#Author: persist
#Abstract: how *.rc files in android ramdisk.img work

# Grammar of *.rc files

	Basis elements in *.rc files:
	
	>* Action
	>* Service
	>* Command
	>* Option


# Action

	Execute commands when some conditions are met.
	
	Format:
```	 
on <action_name>
	<command>
	<command>
	...
```

# Service

	Fork init process to start a service.

	Format:
```
service <service_name> <service_file_path>
	<option>
	<option>
	...
```

# Command

	Do some real works, such as starting services.
	
	Some common commands:
	
	>* trigger <action_name>
	>* class_start <service_class_name>
	>* start <service_name>
	>* stop <service_name>
	>* setprop <name> <value>
	>* mkdir <path>
	>* symlink <target> <sym_link>
	>* write <path> <string>
	>* exec <COMMAND>
	>* export <name> <name>
	>* loglevel <level>

# Option

	Set some features of services.

	Some command options:
	
	>* disable
	>* oneshot
	>* user <username>
	>* group <group_name>
	>* class <class_name>
	>* onrestart <action>
	>* socket <socket_name>
	>* critical
	

# init.rc

## Action early-init

	>* set oom_adj of init and its children
	>* set security context of /adb_keys
	>* mount debugfs
	>* create symlink /lib -> system/lib
	>* start service ueventd
	
	SO, what is service ueventd?


## Action init

	>* create symlinks: 
	>>* /etc	->	system/etc
	>>* /d		->	sys/kernel/debug
	>>* vendor	->	system/vendor
	
	>* mount cgroup cpuacct system
	>* create cgroup memory mountpoint and initialize it
	>* mkdir /system, /data, /cache, config
	>* restorecon_recursive /mnt
	>* mkdir /mnt/*
	>* create symlinks
	>>* /sdcard				->	storage/self/primary
	>>* /mnt/runtime/default/self/primary	->	../../../mnt/user/0/primary
	
	>* mount cgroup memcg
	>* set some tunables about /proc/sys and /proc/cpu
	>* create cgroup mountpoing for process groups and initialize it
	>* set up initial cpusets for ActivityManager
	>* set some access permissions

	So, what is command restorecon_recursive?


## sys.boot_from_charger_mode=1

	>* stop services belong to class charger
	>* trigger action late-init

	So, what are services belong to class charger? What is action late-init?


## Action load_system_props_action

	>* load system properties

	So, what are system properties?


## Action load_persist_props_action

	>* load persist properties
	>* start service logd
	>* start service logd-reinit

	So, what are persist properties? What is service logd? What is service logd-reinit?

	
## Action firmware_mounts_complete

	>* remove /dev/.booting

	So, what is /dev/.booting?

## Action late-init

	>* trigger actions about filesystems
	>>* action early-fs
	>>* action fs
	>>* action post-fs

	>* trigger action load_system_props_action
	>* trigger action post-fs-data
	>* trigger action load_persist_props_action
	>* trigger action firmware_mounts_complete
	>* trigger action early-boot
	>* trigger action boot
	
	So, what is action early-fs? What is action fs? What is action post-fs? What is action post-fs-data? What is action early-boot? What is action boot?


## Action post-fs

	>* start service logd
	>* remount / with readonly access permission
	>* mount / shared
	>* mount default storage into root namespace
	>* restorecon_recursive /cache
	>* set some access permissions about /proc/* and /sys/fs/selinux/policy
	>* mkdir /cache/lost+found

	So, what is command restorecon_recursive?


## Action post-fs-data

	>* set access permission of /data
	>* restorecon_recursive /data
	>* mkdir /data/media to emulate internal storate area
	>* start service logd
	>* start service vold
	>* check the device encryption key
	>* start bootcharting by command bootchart_init
	>* carry over entropy form previous boot
	>* mkdir /data/*
	>* reload policy from /data/security
	>* set SELinux security contexts by command restorecon_recursive /data
	>* check timezone data in /data

	So, what is command restorecon_recursive?


## Action boot

	>* initialize basic network
	>* set RLIMIT_NICE
	>* set memory management
	>* tweak background writeout
	>* set access permissions for System Server and daemons
	>* define default initial receive windown size
	>* start services belong to class core

	So, what are services belong to class core?


## Action nonencrypted

	>* start services belong to class main
	>* start services belong to class late_start
	
	So, what are services belong to class main? What are services belnog to class late_start?


## vold.decrypt=trigger_default_encryption

	>* start service defaultcrypto
	
	So, what is service defaultcrypto?


## vold.decrypt=trigger_encryption

	>* start service surfaceflinger
	>* start service encrypt
	
	So, what is service surfacelinger? What is service encrypt?


## sys.init_log_level=*

	>* set loglevel


## Action charger

	>* start services belong to class charger
	
	So, what are services belong to classs charger?


## vold.decrpyt=trigger_reset_main

	>* reset services belong to class main
	
	So, what is command class_reset? What are services belong to class main?


## vold.decrypt=trigger_load_persist_props

	>* load persist properties
	>* start service logd
	>* start service logd-reinit
	
	So, what are persist properties? What is service logd? What is service logd-reinit?


## vold.decrypt=trigger_post_fs_data

	>* trigger action post-fs-data


## vold.decrypt=trigger_restart_min_framwork

	>* start services belong to class main
	
	So, what are services belong to class main?


## vold.decrypt=trigger_restart_framework

	>* start services belong to class main
	>* start services belong to class late_start
	
	So, what are services belong to class main? What are services belong to class late_start?


## vold.decrypt=trigger_restart_shutdown_framework

	>* reset services belong to late_start
	>* reset services belong to main
	
	So, what are services belong to class late_start? What are services belong to class main?


## sys.powerctl=*

	>* powerctl
	
	So, what is command powerctl?


## sys.sysctl.extra_free_kbytes=*

	>* write /proc/sys/vm/extra_free_kbytes


## sys.sysctl.tcp_def_init_rwnd=*

	>* write /proc/sys/net/ipv4/tcp_default_init_rwnd


## security.perf_harden=0

	>* write /proc/sys/kernel/perf_event_paranoid


## security.perf_harden=1

	>* write /proc/sys/kernel/perf_event_paranoid


## Service ueventd

	path:		/sbin/ueventd
	class:		core
	user/group:	root root
	critical: 	true
	seclabel:	u:r:ueventd:s0

## Service logd

	path:		/system/bin/logd
	class:		core
	user/group:	root {root, system}
	socket:		logd, logdr, logdw


## Service logd-reinit

	path:		/system/bin/logd --reinit
	class:		default
	diabled:	true
	oneshot:	true

## Service healthd

	path:		/sbin/healthd
	class:		core
	user/group:	root {root, system}
	critical:	true
	seclabel:	u:r:healthd:s0


## Service console

	path:		/system/bin/sh
	class:		core
	user/group:	shell {shell, log}
	console:	true
	disabled:	true
	seclabel:	u:r:shell:s0


## ro.debuggable=1

	>* start service console


## Service adbd

	path:		/sbin/adbd --root_seclabel=u:r:su:s0
	class:		core
	user/group:	root root
	socket:		adbd
	disabled:	true
	seclabel:	u:r:adbd:s0


## ro.kernel.qemu=1

	>* start service adbd


## Service lmkd

	path:		/system/bin/lmkd
	class:		core
	user/group:	root root
	socket:		lmkd
	critical:	true


## Service servicemanager

	path: 		/system/bin/servicemanager
	class:		core
	user/group:	system system
	critical:	true
	onrestart:	restart services {healthd, zygote, media, surfaceflinger, drm}


## Service vold

	path:		/system/bin/vold --blkid_context=u:r:blkid:s0 --blkid_untrusted_context=u:r:blkid_untrusted:s0 --fsck_context=u:r:fsck:s0 --fsck_untrusted_context=u:r:fsck_untrusted:s0
	class:		core
	user/group:	root root
	socket:		vold, cryptd


## Service netd

	path:		/system/bin/netd
	class:		main
	user/group:	root root
	socket:		netd, dnsproxyd, mdns, fwmarkd


## Service debuggerd

	path:		/system/bin/debuggerd
	class:		main
	user/group:	root root


## Service debuggerd64

	path:		/system/bin/debuggerd64
	class:		main
	user/group:	root root


## Service ril-daemon

	path:		/system/bin/rild
	class:		main
	user/group:	root {radio, cache, inet, misc, audio, log}
	socket:		rild, sap_uim_socket1, rild-debug

	
## Service surfaceflinger

	path:		/system/bin/surfaceflinger
	class:		core
	user/group:	system {graphics, drmrpc}
	onrestart:	restart service zygote


## Service drm

	path:		/system/bin/drmserver
	class:		main
	user/group:	drm {drm, system, inet, drmrpc}


## Service media

	path:		/system/bin/mediaserver
	class:		main
	user/group:	media {audio, camera, inet, net_bt_admin, net_bw_acct, drmrpc, mediadrm}


## Service defaultcrypto

	path:		system/bin/vdc --wait cryptfs mountdefaultencrypted
	class:		default
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service encrypt

	path:		/system/bin/vdc --wait cryptfs enablecrypto inplace default noui
	class:		default
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service bootanim

	path:		/system/bin/bootanimation
	class:		core
	user/group:	graphics {graphics, audio}
	disabled:	true
	oneshot:	true


## Service gatekeeperd

	path:		/system/bin/gatekeeperd /data/misc/gatekeeper
	class:		late_start
	user/group:	system root


## Service installd

	path:		/system/bin/installd
	class:		main
	user/group:	root root
	socket:		installd


## Service flash_recovery

	path:		/system/bin/install-recovery.sh
	class:		main
	user/group:	root root
	oneshot:	true


## Service racoon

	path:		/system/bin/racoon
	class:		main
	user/group:	root {vpn, net_admin, inet}
	socket:		racoon
	disabled:	true
	oneshot:	true


## Service mtpd

	path:		/system/bin/mtpd
	class:		main
	user/group:	vpn {vpn, net_admin, inet, net_raw}
	disabled:	true
	oneshot:	true


## Service keystore

	path:		/system/bin/keystore /data/misc/keystore
	class:		main
	user/group:	keystore {keystore, drmrpc}


## Service dumstate

	path:		/system/bin/dumpstate -s
	class:		main
	user/group:	root root
	socket:		dumpstate
	disabled:	true
	oneshot:	true


## Service mdnsd

	path:		/system/bin/mdnsd
	class:		main
	user/group:	mdnsr {inet, net_raw}
	socket:		mdnsd
	disabled:	true
	oneshot:	true


## Service uncrypt

	path:		/system/bin/uncrypt
	class:		main
	user/group:	root root
	disable:	true
	oneshot:	true


## Service pre-recovery

	path:		/system/bin/uncrypt --reboot
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service perfprofd

	path:		/system/xbin/perfprofd
	class:		late_start
	user/group:	root root
	oneshot:	true


## persist.logd.logpersistd=logcatd

	>* mkdir /data/misc/logd
	>* exec logd
	>* start service logcatd

	So, what is service logcatd?

	
## Service logcatd 

	path:		/system/bin/logcat -b all -v threadtime -v usec -v printable -D -f /data/misc/logd/logcat -r 1024 -n 256
	class:		late_start
	user/group:	logd log
	disabled:	true

# init.environ.rc

## Action init

	>* export variables
	>>* ANDROID_BOOTLOGD
	>>* ANDROID_ROOT
	>>* ANDROID_ASSETS
	>>* ANDROID_DATA
	>>* ANDROID_STORAGE
	>>* EXTERNAL_STORAGE
	>>* ASEC_MOUNTPOINT
	>>* BOOTCLASSPATH
	>>* SYSTEMSERVERCLASSPATH

# init.trace.rc

## Action boot

	>* set acces permissions about /sys/kernel/debug/tracing/*


# init.zygote32.rc

## Service zygote

	path:		/system/bin/app_process -Xzygote /system/bin --zygote --start-system-server
	class:		main
	user/group:	root root
	socket:		zygote
	onrestart:	restart services {media, netd}


# init.zygote64_32.rc

## Service zygote

	path:		/system/bin/app_process64 -Xzygote /system/bin --zygote --start-system-server --socket-name=zygote
	class:		main
	user/group:	root root
	socket:		zygote
	onrestart:	write {/sys/android_power/request_state, /sys/power/state}, restart services {media, netd}


## Service zygote_secondary

	path:		 /system/bin/app_process32 -Xzygote /system/bin --zygote --socket-name=zygote_secondary
	class:		main
	user/group:	root root
	socket:		zygote_secondary
	onrestart:	restart service zygote


# init.android_x86_64.rc

## Action early-init

	>* export variables
	>>* EGL_LOG_LEVEL
	>>* EGL_DRIVERS


## Action init

	>* create symlinks
	>>* /mnt/sdcard		->	/sdcard
	>>* /storage/sdcard0	->	/sdcard


## Action fs

	>* mount all devices in /fstab.${ro.hardware}


## Action post-fs

	>* exec /system/bin/logwrapper


## Action post-fs-data

	>* mkdir /data/misc/* /data/system
	>* set property vold.post_fs_data_done


## Action boot

	>* set properties
	>>* ro.radio.use-app
	>>* status.battery.state
	>>* status.battery.level
	>>* status.battery.level_raw
	>>* status.battery.level_scale
	>>* ro.config.sync
	>>* app.setupwizard.disable
	>>* ro.alarm.volume.adjustable
	>>* ro.simulated.phone
	>>* persist.sys.strictmode.visual
	>>* persist.sys.strictmode.diable
	>>* sys.media.vdec.drop


## Service wpa_supplicant

	path:		/system/bin/wpa_supplicant -c/data/misc/wifi/wpa_supplicant.conf -iwlan0 -Dnl80211 -O/data/misc/wifi/sockets -e/data/misc/wifi/entropy.bin -g@android:wpa_wlan0
	class:		main
	user/group:	root root
	socket:		wpa_wlan0
	disabled:	true


## Service dhcpcd_wlan0

	path:		/system/bin/dhcpcd -aABDKL
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service iprenew_wlan0

	path:		/system/bin/dhcpcd -n
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service dhcpcd_eth0

	path:		/system/bin/dhcpcd -ABDKL
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service iprenew_eth0

	path:		/system/bin/dhcpcd -n
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service dhcpcd_bt-pan

	path:		/system/bin/dhcpcd -BKLG
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service nativebridge

	path:		/system/bin/enable_nativebridge
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service powerbtnd

	path:		/system/bin/powerbtnd
	class:		late_start
	user/group:	root root


## Service logcat

	path:		/system/bin/logcat -v threadtime -f /data/log.txt
	class:		debug
	user/group:	root root


## Service btattach

	path:		/system/bin/btattach -Pbcm
	class:		main
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service wacom-input

	path:		/system/bin/wacom-input
	class:		default
	user/group:	root root
	disabled:	true
	oneshot:	true


## Service tablet-mode

	path:		/system/bin/tablet-mode
	class:		default
	user/group:	root root
	disabled:	true
	oneshot:	true


## init.svc.wpa_supplicant=stopped

	>* stop service dhcpcd


## system_init.startsurfaceflinger=0

	>* start service surfaceflinger
	>* stop service console


## sys.boot_completed=1

	>* exec /system/bin/logwrapper


## init.svc.bluetoothd=running

	>* exec /system/bin/logwrapper


## init.svc.bluetoothd=stopped

	>* exec /system/bin/logwrapper


## net.dns1=1

	>* exec /system/bin/logwrapper


## debug.logcat=1

	>* start services belong to debug


## persist.sys.nativebridge=1

	>* start service nativebridge


## persist.sys.nativebridge=0

	>* stop service nativebridge


## debug.egl.hw=0

	>* set property ro.kernel.qemu


# init.superuser.rc

## Service su_daemon

	path:		/system/xbin/su --daemon
	class:		default
	user/group:	root root
	seclabel:	u:r:sudaemon:s0
	oneshot:	true


## persist.sys.root_access=0

	>* stop service su_daemon


## persist.sys.root_access=2

	>* stop service su_daemon


## persist.sys.root_access=1

	>* start service su_daemon


## persist.sys.root_access=3

	>* start service su_daemon


## init.svc.su_daemon=running

	>* restorecon_recursive /dev/com.android.settings.daemon
	>* restorecon_recursive /dev/com.android.settings
	
	So, what is command restorecon_recursive?


# init.bluetooth.rc

## Action boot

	>* set access permissions about bluetooth files

## bluetooth.start=daemon

	>* set property bluetooth.start
	>* start service bluetoothd


## bluetooth.stop=daemon

	>* set property bluetooth.stop
	>* stop service bluetoothd


## bluetooth.start=snoop

	>* set property bluetooth.start
	>* start service bluetoothd-snoop


## bluetooth.stop=snoop

	>* set property bluetooth.stop
	>* stop service bluetoothd-snoop


## Service bluetoothd

	path: 		/system/bin/bluetoothd
	class:		main
	user/group:	root bluetooth
	disabled:	true
	oneshot:	true


## Service bluetoothd-snoop

	path:		/system/bin/bluetoothd-snoop
	class:		main
	user/group:	root nobody
	disabled:	true
	oneshot:	true


# init.usb.rc

## Action post-fs-data

	>* set access permissions about usb


## Action boot

	>* set property sys.usb.configfs


## sys.usb.config=none && sys.usb.configfs=0

	>* stop servcie adbd
	>* set property sys.usb.state


## sys.usb.config=adb && sys.usb.configfs=0

	>* start service adbd
	>* set property sys.usb.state


## sys.usb.config=accessory && sys.usb.configfs=0

	>* set property sys.usb.state


## sys.usb.config=accessory,adb && sys.usb.configfs=0

	>* start service adbd
	>* set property sys.usb.state


## sys.usb.config=audio_source && sys.usb.configfs=0

	>* set property sys.usb.state


## sys.usb.config=audio_source,adb && sys.usb.configfs=0

	>* start service adbd
	>* set property sys.usb.state


## sys.usb.config=accessory,audio_source && sys.usb.configfs=0

	>* set property sys.usb.state


## sys.usb.config=accessory,audio_source,adb && sys.usb.configfs=0

	>* start service adbd
	>* set property sys.usb.state


## sys.usb.typec.*=*

	>* write /sys/class/dual_role_usb/otg_default/*
	>* set property sys.usb.typec.state


## sys.usb.config=none && sys.usb.configfs=1

	>* stop service adbd
	>* set property sys.usb.state


## sys.usb.config=adb && sys.usb.configfs=1

	>* start service adbd


## sys.usb.ffs.ready=1 && sys.usb.config=adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=mtp && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=mtp,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=ptp && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=ptp,adb && sys.usb.configfs=1

	>* start service adbd


## sys.usb.ffs.ready=1 && sys.usb.config=ptp,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=accessory && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=accessory,adb && sys.usb.configfs=1

	>* start service adbd


## sys.usb.ffs.ready=1 && sys.usb.config=accessory,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=audio_source && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=audio_source,adb && sys.usb.configfs=1

	>* start service adbd


## sys.usb.ffs.ready=1 && sys.usb.config=audio_source,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=accessory,audio_source && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=accessory,audio_source,adb && sys.usb.configfs=1

	>* start service adbd


## sys.ubs.ffs.ready=1 && sys.usb.config=accessory,audio_source,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=midi && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=midi,adb && sys.usb.configfs=1

	>* start service adbd


## sys.usb.ffs.ready=1 && sys.usb.config=midi,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=rndis && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


## sys.usb.config=rndis,adb && sys.usb.configfs=1

	>* start service adbd


## sys.usb.ffs.ready=1 && sys.usb.config=rndis,adb && sys.usb.configfs=1

	>* modify /config/usb_gadget/g1/*
	>* set property sys.usb.state


# ueventd.rc & ueventd.android_x86_64.rc

	device	list
	
	device_name	permission	user	group
	/dev/*		0[0-9]{3}	user	group
	
	device_name	???	permission	user	group
	/sys/devices/*	???	0[0-9]{3}	user	group
