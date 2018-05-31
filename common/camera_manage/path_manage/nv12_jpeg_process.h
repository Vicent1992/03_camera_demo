#ifndef __NV12_JPEG_PROCESS_H_
#define __NV12_JPEG_PROCESS_H_

#include "base_process.h"


int prepare_nv12_jpeg_path(struct Video* video);
int release_nv12_jpeg_path(struct Video* video);


class NV12_JPEG_PATH : public StreamPUBase
{
	int buffer_index;
    struct Video* video;
public:
    NV12_JPEG_PATH(struct Video* p);
    ~NV12_JPEG_PATH();
    bool processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf); 
};


#endif

