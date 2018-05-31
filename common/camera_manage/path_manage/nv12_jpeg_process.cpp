#include "nv12_jpeg_process.h"

#include "ion_option.h"


NV12_JPEG_PATH::NV12_JPEG_PATH(struct Video* p)
	:StreamPUBase("NV12_JPEG_PATH", true, true)
	, buffer_index(0)
{ 
	func_trace("NV12_JPEG_PATH\n");
	video = p; 
}

NV12_JPEG_PATH::~NV12_JPEG_PATH()
{
	func_trace("~NV12_JPEG_PATH\n");
}

bool NV12_JPEG_PATH::processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf) {
 
	BufferData input_data;
	BufferData output_data;
	
	input_data.vir_addr_ = inBuf->getVirtAddr();
	input_data.ion_data_.fd_ = inBuf->getFd();
	input_data.ion_data_.handle_ = (ion_user_handle_t)inBuf->getHandle();
	input_data.mem_size_ = inBuf->getDataSize();

	output_data.vir_addr_ = video->jpeg_buffer[buffer_index].buffer;
	output_data.ion_data_.fd_ = video->jpeg_buffer[buffer_index].fd;
	output_data.ion_data_.handle_ = video->jpeg_buffer[buffer_index].handle;
	output_data.mem_size_ = video->jpeg_buffer[buffer_index].size;
	
	IonBuffer input_buf(input_data);
	IonBuffer output_buf(output_data);

	JpegEncConfig& jconfig = video->medio_config.jpeg_config;
	jconfig.width = video->width;
	jconfig.height = video->height;
	jconfig.fmt = PIX_FMT_NV12;
	jconfig.qp = 10;  // best quality [1-10]

	MPPJpegEncoder* encoder = new MPPJpegEncoder();
	if (encoder->InitConfig(video->medio_config))
		  assert(0);
	int encode_ret = encoder->Encode(&input_buf, &output_buf);
	assert(!encode_ret);
	assert(output_buf.GetValidDataSize() > 0);
	encoder->unref();

#ifdef SAVE_JPEG_FILE 
	char path[80] = {0};
	int write_size;
	int jpeg_file_fd;

    snprintf(path, sizeof(path), "/mnt/sdcard/nv12_%d_%d.jpg", 
								 video->width, video->height);

	printf("vicent----------------jpeg_file_path %s\n", path);

    jpeg_file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
    if (jpeg_file_fd < 0) {
      printf("open jpg failed, errno: %d\n", errno);
      assert(0);
    }
    write_size = write(jpeg_file_fd, 
    				   video->jpeg_buffer[buffer_index].buffer, 
    				   output_buf.GetValidDataSize());

    close(jpeg_file_fd);
    if (write_size != output_buf.GetValidDataSize()) {
      printf("sd card may be full filled\n");
      unlink(path);
    }
#endif

	(buffer_index >= NV12_BUFFER_ALLOC_NUM-1)?
					 (buffer_index = 0):(buffer_index++);
	printf("NV12_JPEG_PATH buffer_index %d\n", buffer_index);
    return true;
}

int prepare_nv12_jpeg_path(struct Video* video)
{
	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++) {
		memset (&video->jpeg_buffer[i], 0, sizeof(struct ion_manage));
		video->jpeg_buffer[i].client = -1;
		video->jpeg_buffer[i].fd = -1;
		if (ion_manage_alloc(&video->jpeg_buffer[i], video->width, video->height, 3, 2)) {
			printf("isp_video nv12_path ion alloc fail!\n");
			return -1;
		}
	}

	if (video->hal->mpath.get()) {
		video->hal->nv12_jpeg_path = 
				shared_ptr<NV12_JPEG_PATH>(new NV12_JPEG_PATH(video));
		if (hal_add_path(video->hal->mpath, 
				video->hal->nv12_jpeg_path, 
				video->mpfrmFmt, 
				NV12_BUFFER_ALLOC_NUM, NULL))
		return -1;
	}
	return 0;
}

int release_nv12_jpeg_path(struct Video* video)
{
	if (video->hal->mpath.get()) {
		hal_remove_path(video->hal->mpath, video->hal->nv12_jpeg_path);
		video->hal->nv12_jpeg_path.reset();
	}

	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++)
		ion_manage_free(&video->jpeg_buffer[i]);

	return 0;
}