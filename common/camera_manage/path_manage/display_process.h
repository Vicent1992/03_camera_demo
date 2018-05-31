#ifndef __DISPALY_PROCESS_H_
#define __DISPALY_PROCESS_H_

#include "base_process.h"


int prepare_display_path(struct Video* video);
int release_display_path(struct Video* video);


class DISPALY_PATH : public StreamPUBase
{
	int buffer_index;
	int disp_rga_fd;
    struct Video* video;

public:
    DISPALY_PATH(struct Video* p);
    ~DISPALY_PATH();
    bool processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf); 
};


#endif

