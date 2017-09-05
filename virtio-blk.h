/*
 * Dedicated thread for virtio-blk I/O processing
 * Each process in VM(Virtual Machine) holds a thread instance
 *
 * Author:
 *   persist <1554706732@qq.com>
 *
 */

#ifndef HW_PROCESSPLANE_VIRTIO_BLK_H
#define HW_PROCESSPLANE_VIRTIO_BLK_H

typedef struct VirtIOBlockProcessPlane VirtIOBlockProcessPlane;

void virtio_blk_process_plane_create(VirtIOBlockProcessPlane *plane, pid_t pid, char* name);

void virtio_blk_process_plane_destory(VirtIOBlockProcessPlane *s);

void virtio_blk_process_plane_notify(VirtIOBlockProcessPlane *s);

void virtio_blk_process_plane_start(VirtIOBlockProcessPlane *s);

void virtio_blk_process_plane_stop(VirtIOBlockProcessPlane *s);

#endif /* HW_PROCESSPLANE_VIRTIO_BLK_H */
