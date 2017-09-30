#Author: persist
#Abstract: explain what ramdisk.img does

# Function

	It is root filesystem of android system.


# Structure

```
.
├── charger -> /sbin/healthd
├── data
├── default.prop
├── dev
├── file_contexts
├── fstab.android_x86_64
├── init
├── init.android_x86_64.rc
├── init.bluetooth.rc
├── init.environ.rc
├── init.rc
├── init.superuser.rc
├── init.trace.rc
├── init.usb.configfs.rc
├── init.usb.rc
├── init.zygote32.rc
├── init.zygote64_32.rc
├── oem
├── proc
├── property_contexts
├── sbin
│   ├── adbd
│   ├── healthd
│   ├── modprobe -> ../init
│   ├── ueventd -> ../init
│   ├── v86d
│   └── watchdogd -> ../init
├── seapp_contexts
├── selinux_version
├── sepolicy
├── service_contexts
├── sys
├── system
├── ueventd.android_x86_64.rc
└── ueventd.rc
```


# Pack

``` bash
$ find . | cpio --quiet -H newc -o | gzip -9 -n > ../ramdisk.img
```


# Unpack

``` bash
$ zcat ../ramdisk.img | cpio -iud
```


# Key

## *.rc

### init*.rc
>*init.rc
>*init.environ.rc
>*init.trace.rc
>*init.zygote64_32.rc
>*init_zygote32.rc
>*init.bluetooth.rc
>*init.usb.rc
>*init.usb.configs.rc

### ueventd*.rc
>* ueventd.rc
>* ueventd.android_x86_64.rc


### *_contexts
>* file_contexts
>* property_contexts
>* seapp_contexts
>* service_contexts


### sepolicy
