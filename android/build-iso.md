#Author: persist
#Abstract: explain how to make android x86 iso image with mkisofs

# Structure

```
.
├── boot
│   ├── grub
│   │   ├── efi.img
│   │   ├── grub.cfg
│   │   └── TRANS.TBL
│   └── TRANS.TBL
├── efi
│   ├── boot
│   │   ├── bootia32.efi
│   │   ├── BOOTx64.EFI
│   │   ├── grubx64.efi
│   │   └── TRANS.TBL
│   └── TRANS.TBL
├── initrd.img
├── install.img
├── isolinux
│   ├── android-x86.png
│   ├── isolinux.bin
│   ├── isolinux.cfg (remove it, then generate it automatically)
│   ├── TRANS.TBL
│   └── vesamenu.c32
├── kernel
├── myiso.iso
├── ramdisk.img
├── system.sfs
└── TRANS.TBL
```


# Pack

``` bash
$ mkisofs -o myiso.iso -r -J -T -V "Android x86 Build" -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -eltorito-alt-boot  -e boot/grub/efi.img  -no-emul-boot .
```


# Run with qemu

``` bash
$ qemu-system-x86 -smp2 -m 4096 -enable-kvm -cdrom myiso.iso
```


# Note 

	Check iso image information with tool 'dumpet' or 'file'.

``` bash
$ dumpet -i myiso.iso
```

``` bash
$ file myiso.iso
```

	Rerun qemu with option '-vga std', vga is a universal display dirver which can support most graphics cards.