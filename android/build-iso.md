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
$ mkisofs -o android-x86.iso -r -J -T -V "Android x86 Build" -b isolinux/isolinux.bin -c isolinux/boot.cat -no-emul-boot -boot-load-size 4 -boot-info-table -eltorito-alt-boot  -e boot/grub/efi.img  -no-emul-boot .
$ isohybrid android-x86.iso
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

``` bash
$ file android-x86.iso
```

	Rerun qemu with option '-vga std', vga is a universal display dirver which can support most graphics cards.