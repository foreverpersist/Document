#Author: persist
#Abstract: how to convert a directory to *.img

# Create image

	Create a raw image of the required size (use seek to creates a sparse file, which saves space).
	
``` bash
$ dd if=/dev/null of=example.img bs=1M seek=10240
```

# Create filesystem

	Create a specific filesystem on the image.

``` bash
$ mkfs.ext4 -F example.img
```


# Mount

	Mount it to specific directory for writting.

``` bash
$ mkdir /mnt/example
$ mount -t ext4 -o loop example.img /mnt/example
```


# Write

	Write files to the mountpoint.

``` bash
$ cp example_file /mnt/example
$ cp -r example_dir /mnt/example  
```


# Umount

	Umount the mountpoint to save changes.

``` bash
$ mount /mnt/example
```

