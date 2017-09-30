#Author: persist
#Abstract: explain what system.sfs(system.img) does

# Function

	It is /system partition of android system.


# Structure

```
.
├── app
├── bin
├── build.prop
├── etc
├── fonts
├── framework
├── lib
├── lib64
├── lost+found
├── media
├── priv-app
├── usr
├── vendor
└── xbin
```


# Pack

	``` bash
	$ make_ext4fs -T -1 -S file_contexts -L system -l 1342177280 -a system system.img system system
	$ mksquashfs system.img system.sfs
	```

# Unpack

	``` bash
	$ unsquashfs system.sfs
	$ mkdir system
	$ mount -o loop squashfs-root/system.img system
	```

# Key

## app

## priv-app

## framework
