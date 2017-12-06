#Author: persist
#Abstract: explain how to make android x86 iso image with mkisofs

# Structure

```
.
├── boot
│   └── grub
│       ├── efi.img
│       ├── grub.cfg
│       └── grub.cfg
├── efi
│   └── boot
│       ├── bootia32.efi
│       ├── BOOTx64.EFI
│       └── grubx64.efi 
├── initrd.img
├── install.img
├── isolinux
│   ├── android-x86.png
│   ├── isolinux.bin
│   ├── isolinux.cfg (remove it, then generate it automatically)
│   └── vesamenu.c32
├── kernel
├── myiso.iso
├── ramdisk.img
└── system.sfs
```


# Pack

``` bash
$ genisoimage -vJURT -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -eltorit0-alt-boot -e boot/grub/efi.img -no-emul-boot -input-charset utf-8 -V "Android-x86 LiveCD" -o android-x86.iso iso/
$ isohybrid --uefi android-x86.iso
```


# Run with qemu

``` bash
$ qemu-system-x86 -smp 2 -m 4096 -enable-kvm -vga std android-x86.iso
```


# Note 

	Check iso image information with tool 'dumpet' or 'file'.

``` bash
$ dumpet -i android-x86.iso
```

	dumpet command's output is like: 

```
	Validation Entry:
	Header Indicator: 0x01 (Validation Entry)
	PlatformId: 0x00 (80x86)
	ID: ""
	Checksum: 0x55aa
	Key bytes: 0x55aa
Boot Catalog Default Entry:
	Entry is bootable
	Boot Media emulation type: no emulation
	Media load segment: 0x0 (0000:7c00)
	System type: 0 (0x00)
	Load Sectors: 4 (0x0004)
	Load LBA: 43 (0x0000002b)
Section Header Entry:
	Header Indicator: 0x91 (Final Section Header Entry)
	PlatformId: 0xef (EFI)
	Section Entries: 1
	ID: ""
Boot Catalog Section Entry:
	Entry is bootable
	Boot Media emulation type: no emulation
	Media load address: 0 (0x0000)
	System type: 0 (0x00)
	Load Sectors: 6144 (0x1800)
	Load LBA: 55 (0x00000037)
```

``` bash
$ file android-x86.iso
```

	file command's output is like: 

```
android-x86.iso: x86 boot sector
```
	Rerun qemu with option '-vga std', vga is a universal display dirver which can support most graphics cards.
