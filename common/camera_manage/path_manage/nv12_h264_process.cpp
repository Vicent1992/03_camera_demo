#include "nv12_h264_process.h"

#include "ion_option.h"


NV12_H264_PATH::NV12_H264_PATH(struct Video* p)
	:StreamPUBase("NV12_H264_PATH", true, true)
	, buffer_index(0)
	, h264_file_fd(0)
	, h264_extra_data_size(0)
	, h264_extra_data(NULL)
{ 
	char path[64] = {0};
	int write_size;
	func_trace("NV12_H264_PATH\n");

	video = p; 

	snprintf(path, sizeof(path), "/mnt/sdcard/nv12_%d_%d.h264",
						video->encoder_width, video->encoder_height);
	h264_file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
	assert(h264_file_fd);

	VideoConfig& vconfig = video->medio_config.video_config;

	vconfig.width = video->encoder_width;
	vconfig.height = video->encoder_height;
	vconfig.fmt = PIX_FMT_NV12;
	vconfig.bit_rate = vconfig.width * vconfig.height * 7;
	if (vconfig.bit_rate > 1000000) {
		vconfig.bit_rate /= 1000000;
		vconfig.bit_rate *= 1000000;
	}
	vconfig.frame_rate = video->encoder_fps;
	vconfig.level = 51;
	vconfig.gop_size = vconfig.frame_rate;
	vconfig.profile = 100;
	vconfig.quality = MPP_ENC_RC_QUALITY_BEST;
	vconfig.qp_init = 26;
	vconfig.qp_step = 8;
	vconfig.qp_min = 4;
	vconfig.qp_max = 48;
	vconfig.rc_mode = MPP_ENC_RC_MODE_CBR;

	video->h264_encoder = new H264Encoder();
	if (video->h264_encoder->InitConfig(video->medio_config))
		assert(0);
	video->h264_encoder->GetExtraData(h264_extra_data, h264_extra_data_size);
  	write_size = write(h264_file_fd, h264_extra_data, h264_extra_data_size);
	assert(write_size == h264_extra_data_size);

}

NV12_H264_PATH::~NV12_H264_PATH()
{
	func_trace("~NV12_H264_PATH\n");

	close(h264_file_fd);
	video->h264_encoder->unref();
}

bool NV12_H264_PATH::processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf) {
 
	int write_size;
	int encode_ret;

	BufferData input_data;
	BufferData output_data;
	
	input_data.vir_addr_ = inBuf->getVirtAddr();
	input_data.ion_data_.fd_ = inBuf->getFd();
	input_data.ion_data_.handle_ = (ion_user_handle_t)inBuf->getHandle();
	input_data.mem_size_ = inBuf->getDataSize();

	output_data.vir_addr_ = video->h264_buffer[buffer_index].buffer;
	output_data.ion_data_.fd_ = video->h264_buffer[buffer_index].fd;
	output_data.ion_data_.handle_ = video->h264_buffer[buffer_index].handle;
	output_data.mem_size_ = video->h264_buffer[buffer_index].size;
	
	IonBuffer input_buf(input_data);
	IonBuffer output_buf(output_data);

	encode_ret = video->h264_encoder->EncodeOneFrame(&input_buf, &output_buf, nullptr);
    assert(!encode_ret);
    assert(output_buf.GetValidDataSize() > 0);
	if (output_buf.GetUserFlag() & MPP_PACKET_FLAG_INTRA)
		printf("Got I frame\n");
	else
		printf("Got P frame\n");
    write_size = write(h264_file_fd, video->h264_buffer[buffer_index].buffer, 
    									      output_buf.GetValidDataSize());
    assert(write_size == output_buf.GetValidDataSize());

	(buffer_index >= NV12_BUFFER_ALLOC_NUM-1)?
					 (buffer_index = 0):(buffer_index++);
	printf("NV12_H264_PATH buffer_index %d\n", buffer_index);
    return true;
}

int prepare_nv12_h264_path(struct Video* video)
{
	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++) {
		memset (&video->h264_buffer[i], 0, sizeof(struct ion_manage));
		video->h264_buffer[i].client = -1;
		video->h264_buffer[i].fd = -1;
		if (ion_manage_alloc(&video->h264_buffer[i], video->width, video->height, 3, 2)) {
			printf("isp_video nv12_path ion alloc fail!\n");
			return -1;
		}
	}

	if (video->hal->mpath.get()) {
		video->hal->nv12_h264_path = 
				shared_ptr<NV12_H264_PATH>(new NV12_H264_PATH(video));
		if (hal_add_path(video->hal->mpath, 
				video->hal->nv12_h264_path, 
				video->mpfrmFmt, 
				NV12_BUFFER_ALLOC_NUM, NULL))
		return -1;
	}
	return 0;
}

int release_nv12_h264_path(struct Video* video)
{
	if (video->hal->mpath.get()) {
		hal_remove_path(video->hal->mpath, video->hal->nv12_h264_path);
		video->hal->nv12_h264_path.reset();
	}

	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++)
		ion_manage_free(&video->h264_buffer[i]);

	return 0;
}