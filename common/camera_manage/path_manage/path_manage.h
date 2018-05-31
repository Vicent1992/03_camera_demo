
#ifndef __PU_MANAGE_H__
#define __PU_MANAGE_H__

#include "base_process.h"

int hal_add_path(shared_ptr<CamHwItf::PathBase> pre, shared_ptr<StreamPUBase> next,
               frm_info_t &frmFmt, unsigned int num, shared_ptr<CameraBufferAllocator> allocator);
int hal_add_path(shared_ptr<StreamPUBase> pre, shared_ptr<StreamPUBase> next,
               frm_info_t &frmFmt, unsigned int num, shared_ptr<CameraBufferAllocator> allocator);
void hal_remove_path(shared_ptr<CamHwItf::PathBase> pre, shared_ptr<StreamPUBase> next);
void hal_remove_path(shared_ptr<StreamPUBase> pre, shared_ptr<StreamPUBase> next);

#endif
