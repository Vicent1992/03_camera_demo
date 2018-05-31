#include "path_manage.h"

int hal_add_path(shared_ptr<CamHwItf::PathBase> pre,      shared_ptr<StreamPUBase> next,
               frm_info_t &frmFmt, unsigned int num, shared_ptr<CameraBufferAllocator> allocator)
{
    if (!pre.get() || !next.get()) {
        printf("%s: PU is NULL\n", __func__);
        return -1;
    }
    pre->addBufferNotifier(next.get());
    next->prepare(frmFmt, num, allocator);
    if (!next->start()) {
        printf("PU start failed!\n");
        return -1;
    }

    return 0;
}

int hal_add_path(shared_ptr<StreamPUBase> pre, shared_ptr<StreamPUBase> next,
               frm_info_t &frmFmt, unsigned int num, shared_ptr<CameraBufferAllocator> allocator)
{
    if (!pre.get() || !next.get()) {
        printf("%s: PU is NULL\n", __func__);
        return -1;
    }
    pre->addBufferNotifier(next.get());
    next->prepare(frmFmt, num, allocator);
    if (!next->start()) {
        printf("PU start failed!\n");
        return -1;
    }

    return 0;
}

void hal_remove_path(shared_ptr<CamHwItf::PathBase> pre, shared_ptr<StreamPUBase> next)
{
    if (!pre.get() || !next.get())
        return;

    pre->removeBufferNotifer(next.get());
    next->stop();
    next->releaseBuffers();
}

void hal_remove_path(shared_ptr<StreamPUBase> pre, shared_ptr<StreamPUBase> next)
{
    if (!pre.get() || !next.get())
        return;

    pre->removeBufferNotifer(next.get());
    next->stop();
    next->releaseBuffers();
}

