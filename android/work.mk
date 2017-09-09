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


# Build android-x86 image android-x86.iso
``` bash
$ mkisofs -o .././output//android-x86.iso -r -J -T -V "Android x86 Build" -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -eltorito-alt-boot  -e boot/grub/efi.img  -no-emul-boot .
```


# Run
$ qemu-system-x86_64 -smp 2 -m 4096 -enable-kvm  ./output//test.img -cdrom ./output//android-x86.iso  -vga std -net nic -net user
