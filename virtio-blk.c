/*
 * Dedicated thread for virtio-blk I/O processing
 * Each process in VM(Virtual Machine) holds a thread instance
 *
 *
 * Author:
 *   persist <1554706732@qq.com>
 *
 */

#include "qemu/osdep.h"
#include "qapi/error.h"
#include "trace.h"
#include "qemu/iov.h"
#include "qemu/thread.h"
#include "qemu/error-report.h"
#include "hw/virtio/virtio-access.h"
#include "sysemu/block-backend.h"
#include "hw/virtio/virtio-blk.h"
#include "virtio-blk.h"
#include "block/aio.h"
#include "hw/virtio/virtio-bus.h"
#include "qom/object_interfaces.h"

struct VirtIOBlockProcessPlane {
    /* thread is running */
    bool running;
    /* pid of the process in VM */
    pid_t pid;
    /* thread name */
    char *name;
    /* thread instance */
    QemuThread *process_thread;
    /* current req */
    VirtIOBlockReq *req;
    /* multiple req buffer */
    MultiReqBuffer *mrb;
    /*thread mutex*/
    QemuMutex *mutex;
    /* thread cond related to mutex*/
    QemuCond *cond;
};

static void *virtio_blk_process_plane_run(void *arg)
{
    VirtIOBlockProcessPlane *s = arg;
    
    do{
        qemu_mutex_lock(s->mutex);
        if(s->running) {
            qemu_cond_wait(s->cond, s->mutex);
            if(s->running && s->req && s->mrb) {
                //handle request
                virtio_blk_data_plane_handle_vq(s->req, s->mrb);
            }
        }
        qemu_mutex_unlock(s->mutex);
    } while(s->running);

    return NULL;
}

void virtio_blk_process_plane_create(VirtIOBlockProcessPlane *plane, pid_t pid, char* name)
{
    VirtIOBlockProcessPlane *s;

    *dataplane = NULL;

    if (pid < 0) {
            fprintf(stderr, "pid is invalid\n");
            return;
    }

    s = g_malloc0(sizeof(VirtIOBlockDataPlane));
    s->pid = pid;
    s->name = name;
    s->req = NULL;
    s->mrb = g_malloc0(sizeof(MultiReqBuffer));
    s->mutext = g_malloc0(sizeof(QemuMutex));
    s->cond = g_malloc0(sizeof(QemuCond));
    qemu_mutex_init(s->mutex);
    qemu_cond_init(s->cond);

    QemuThread t;
    qemu_thread_create(&t, name, virtio_blk_process_plane_run, s, QEMU_THREAD_JOINABLE);
    virtio_blk_process_plane_start(s);

    *dataplane = s;
}

/* Context: QEMU global mutex held */
void virtio_blk_process_plane_destroy(VirtIOBlockProcessPlane *s)
{
    if (!s) {
        return;
    }
    g_free(s->mrb);
    qemu_mutex_destory(s->mutex);
    qemu_cond_destory(s->cond);
    g_free(s->mutex);
    g_free(s->cond);
}


static void virtio_blk_data_plane_handle_vq(VirtIORequest *req, MultiReqBuffer *mrb)
{

    virtio_blk_handle_request(req, mrb);
}

int virtio_blk_data_plane_start(VirtIOBlockProcessPlane *s)
{
    s->running = true;
}

void virtio_blk_data_plane_stop(VirtIOBlockProcessPlane *s)
{
    s->running= false;
}
