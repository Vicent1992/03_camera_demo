
#include "nv12_process.h"

NV12_PATH::NV12_PATH(struct Video* p) 
	: StreamPUBase("NV12_PATH", true, true)
	, buffer_index(0)
{ 
	func_trace("NV12_PATH\n");
	video = p; 
}

NV12_PATH::~NV12_PATH() {
	func_trace("~NV12_PATH\n");
}

bool NV12_PATH::processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf) {

#ifdef SAVE_NV12_FILE
    static FILE* f_tmp = NULL;
    static char filename[80] = {0};
    static int cmd_cnt = 0;
	
    snprintf(filename, sizeof(filename), 
    		"/mnt/sdcard/nv12_%d_%d.yuv", 
    		inBuf->getWidth(), inBuf->getHeight());
    f_tmp = fopen(filename, "a+");
    fwrite(inBuf->getVirtAddr(), 1, inBuf->getDataSize(), f_tmp);
    fflush(f_tmp);
    fclose(f_tmp);
    f_tmp = NULL;

	func_trace("NV12_PATH %s\n", filename);
#endif
	
	(buffer_index >= NV12_BUFFER_ALLOC_NUM-1)?
					 (buffer_index = 0):(buffer_index++);
	func_trace("NV12_PATH buffer_index %d\n", buffer_index);
    return true;
}


int prepare_nv12_path(struct Video* video)
{
	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++) {
		memset (&video->nv12_buffer[i], 0, sizeof(struct ion_manage));
		video->nv12_buffer[i].client = -1;
		video->nv12_buffer[i].fd = -1;
		if (ion_manage_alloc(&video->nv12_buffer[i], video->width, video->height, 3, 2)) {
			printf("isp_video nv12_path ion alloc fail!\n");
			return -1;
		}
	}
	if (video->hal->mpath.get()) {
		video->hal->nv12_path = shared_ptr<NV12_PATH>(new NV12_PATH(video));
		if (hal_add_path(video->hal->mpath, 
				video->hal->nv12_path, 
				video->mpfrmFmt, 
				NV12_BUFFER_ALLOC_NUM, NULL))
		return -1;
	}
	return 0;
}

int release_nv12_path(struct Video* video)
{
	if (video->hal->mpath.get()) {
		hal_remove_path(video->hal->mpath, video->hal->nv12_path);
		video->hal->nv12_path.reset();
	}

	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++)
		ion_manage_free(&video->nv12_buffer[i]);

	return 0;
}

