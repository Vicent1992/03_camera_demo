#ifndef __ION_OPTION_H__
#define __ION_OPTION_H__

#include "ion/ion.h"
#include "ion_buffer.h"

struct ion_manage {
    int client;
    int width;
    int height;
    void *buffer;
    int fd;
    ion_user_handle_t handle;
    size_t size;
    unsigned long phys;
};

int ion_manage_alloc(struct ion_manage* video_ion,
                             int width,
                             int height,
                             int num,
                             int den);
int ion_manage_free(struct ion_manage* video_ion);

#endif
