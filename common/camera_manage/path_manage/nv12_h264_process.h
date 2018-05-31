#ifndef __NV12_H264_PROCESS_H_
#define __NV12_H264_PROCESS_H_

#include "base_process.h"


int prepare_nv12_h264_path(struct Video* video);
int release_nv12_h264_path(struct Video* video);


class NV12_H264_PATH : public StreamPUBase
{
	int buffer_index;

	int h264_file_fd;
	void* h264_extra_data;
	size_t h264_extra_data_size;

    struct Video* video;
public:
    NV12_H264_PATH(struct Video* p);
    ~NV12_H264_PATH();
    bool processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf); 
};


#endif

