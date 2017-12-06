#Author: persist
#Abstract: explain how to custom kernel

# Target

>* support runc
>* support para-virtualization

## Support runc

### Namespace

	IPC Namespace is not supported by android, because android use binder for process communication.

>* PID
>* UTS
>* USER
>* NET
>* MNT

### Cgroup

>* blkio
>* cpu
>* cpuacct
>* cpuset
>* devices
>* freezer
>* memory

### Devpts

	Append devpts multiple instances support

```
CONFIG_DEVPTS_MULTIPLE_INSTANCES=y
```

## Support para-virtualization