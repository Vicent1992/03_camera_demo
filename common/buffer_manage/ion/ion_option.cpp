#include <assert.h>
#include <execinfo.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <ion/ion.h>

#include "ion_option.h"

static void fatal_dump(void)
{
    void*array[30];
    size_t size;
    char**strings;
    size_t i;

    printf ("<<<<<<--- ION FATAL DUMP --->>>>>>\n");

    size = backtrace(array, 30);
    strings = (char**)backtrace_symbols(array, size);
    for (i = 0; i < size; i++)
        printf ("%d : %s \n", i, strings[i]);
    free (strings);
}

static int ion_alloc_buf(struct ion_manage* ion_info)
{
    int ret = ion_alloc(ion_info->client, ion_info->size, 0,
                        ION_HEAP_TYPE_DMA_MASK, 0, &ion_info->handle);
    if (ret < 0) {
        ion_manage_free(ion_info);
        printf("ion_alloc failed!\n");
        return -1;
    }
    ret = ion_share(ion_info->client, ion_info->handle, &ion_info->fd);
    if (ret < 0) {
        ion_manage_free(ion_info);
        printf("ion_share failed!\n");
        return -1;
    }
    ion_get_phys(ion_info->client, ion_info->handle, &ion_info->phys);
    ion_info->buffer = mmap(NULL, ion_info->size, PROT_READ | PROT_WRITE,
                             MAP_SHARED | MAP_LOCKED, ion_info->fd, 0);
    if (!ion_info->buffer) {
        ion_manage_free(ion_info);
        printf("%s mmap failed!\n", __func__);
        return -1;
    }
    printf("--- ion alloc, get fd: %d\n", ion_info->fd);
    return 0;
}

int ion_alloc_rational(struct ion_manage* ion_info,
                             int width,
                             int height,
                             int num,
                             int den)
{
    if (ion_info->client >= 0) {
        printf("warning: video_ion client has been already opened\n");
        fatal_dump();
        return -1;
    }
    ion_info->client = ion_open();
    if (ion_info->client < 0) {
        printf("%s:open /dev/ion failed!\n", __func__);
        return -1;
    }

    ion_info->width = width;
    ion_info->height = height;
    ion_info->size = ((width + 15) & ~15) * ((height + 15) & ~15) * num / den;
    return ion_alloc_buf(ion_info);
}

int ion_manage_alloc(struct ion_manage* ion_info, 
							int width, 
							int height, 
							int num, 
							int den)
{
    return ion_alloc_rational(ion_info, width, height, num, den);
}

int ion_manage_free(struct ion_manage* ion_info)
{
    int ret = 0;

    if (ion_info->buffer) {
        munmap(ion_info->buffer, ion_info->size);
        ion_info->buffer = NULL;
    }

    if (ion_info->fd >= 0) {
        printf("--- ion free, release fd: %d\n", ion_info->fd);
        close(ion_info->fd);
        ion_info->fd = -1;
    }

    if (ion_info->client >= 0) {
        if (ion_info->handle) {
            ret = ion_free(ion_info->client, ion_info->handle);
            if (ret)
                printf("ion_free failed!\n");
            ion_info->handle = 0;
        }

        ion_close(ion_info->client);
        ion_info->client = -1;
    }

    memset(ion_info, 0, sizeof(struct ion_manage));
    ion_info->client = -1;
    ion_info->fd = -1;

    return ret;
}
