#Author: persist
#Abstract: what android init does

# Main Services in *.rc


| service		| class		| disabled	| oneshot	| path					|
| :---			| :---		| :---:		| :---:		| :---					|
| ueventd		| core		|		|		| /sbin/ueventd				|
| logd			| core		|		|		| /system/bin/logd			|
| logd-reinit		| 		| true		| true		| /system/bin/logd			|
| healthd		| core		|		|		| /sbin/healthd				|
| console		| core		| true		|		| /system/bin/sh			|
| adbd			| core		| true		|		| /sbin/adbd				|
| lmkd			| core		|		|		| /system/bin/lmkd			|
| servicemanager	| core		|		|		| /system/bin/servicemanager		|
| vold			| core		|		|		| /system/bin/vold			|
| surfaceflinger	| core		|		|		| /system/bin/surfacefinger		|
| bootanim		| core		| true		| true		| /system/bin/bootanimation		|
| debuggerd		| main		|		|		| /system/bin/debuggerd			|
| debuggerd		| main		|		|		| /system/bin/debuggerd64		|
| ril-daemon		| main		|		|		| /system/bin/rild			|
| drm			| main		|		|		| /system/bin/drmserver			|
| media			| main		|		|		| /system/bin/media			|
| installd		| main		|		|		| /system/bin/installd			|
| flash_recovery	| main		| 		| true		| /system/bin/install-recovery.sh	|
| racoon		| main		| true		| true		| /system/bin/racoon			|
| mtpd			| main		| true		| true		| /system/bin/mtpd			|
| keystore		| main		|		|		| /system/bin/kerstore			|
| dumpstate		| main		| true		| true		| /system/bin/dumpstate			|
| mdnsd			| main		| true		| true		| /system/bin/mdnsd			|
| uncrypt		| main		| true		| true		| /system/bin/uncrypt			|
| pre-recovery		| main		| true		| true		| /system/bin/uncrypt			|
| zygote		| main		|		|		| /system/bin/app_process64		|
| zygote_secondary	| main		| 		|		| /system/bin/app_process32		|
| dhcpcd_wlan0		| main		| true		| true		| /system/bin/dhcpcd			|
| iprenew_wlan0		| main		| true		| true		| /system/bin/dhcpcd			|
| dhcpcd_eth0		| main		| true		| true		| /system/bin/dhcpcd			|
| iprenew_wlan0		| main		| true		| true		
| /system/bin/dhcpcd			|
| dhcpcd_bt-pan		| main		| true		| true		| /system/bin/dhcpcd			|
| nativebridge		| main		| true		| true		| /system/bin/enable_nativebridge	|
| btattache		| main		| true		| true		| /system/bin/btattach			|
| bluetoothd		| main		| true		| true		| /system/bin/bluetoothd		|
| bluetoothd-snoop	| main		| true		| true		| /system/bin/bluetoothd-snoop		|
| perfprofd		| late_start	| 		| true		| /system/bin/perfprofd			|
| logcatd		| late_start	| true		|		| /system/bin/logcat			|
| gatekeeperd		| late_start	|		|		| /system/bin/gatekeeperd		|
| powerbtnd		| late_start	| 		|		| /system/bin/powerbtnd			|
| logcat		| debug		|		|		| /system/bin/logcat			|
| wacom-input		| 		| true		| true		| /system/bin/wacom-input		|
| tablet-mode		| 		| true		| true		| /system/bin/tablet-mode		|
| su-daemon		|		|		| true		| /system/xbin/su			|
| defaultcrypto		| 		| true		| true		| /system/bin/vdc			|
| encrypt		| 		| true		| true		| /system/bin/vdc			|


# Main Actions

## early-init

	start service(s): 
	>* ueventd


## init

	mount cgroup


## late-init

	trigger actions: 
	>* early-fs
	>* fs
	>* post-fs
	>* load_system_props_action
	>* post-fs-data
	>* load_persist_props_action
	>* firmware_mounts_complete
	>* early-boot
	>* boot


# Lower Actions

## early-fs


## fs

	mount all devices in fstab


## post-fs

	start service(s): 
	>* logd
	
	exec /system/bin/logwrapper


## post-fs-data

	start service(s): 
	>* logd
	>* vold


## early-boot


## boot

	start service class(es): 
	>* core


