#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include "pthread_option.h"
#include "ion_option.h"
#include "h264_encoder.h"

#define PTHREAD_NUM 10

static int looping = 1;

MediaConfig config;
struct ion_manage nv12_ion_buffer;
struct ion_manage h264_ion_buffer;


void SignalHandler(int sig)
{
    looping = 0;
}

void AllocBuffer(int w, int h)
{
    memset(&nv12_ion_buffer, 0 , sizeof(ion_manage));
    memset(&h264_ion_buffer, 0 , sizeof(ion_manage));

    nv12_ion_buffer.client = -1;
    nv12_ion_buffer.fd = -1;
    h264_ion_buffer.client = -1;
    h264_ion_buffer.fd = -1;

    if (ion_manage_alloc(&nv12_ion_buffer, w, h, 3, 2) == -1)
      assert(0);
    if (ion_manage_alloc(&h264_ion_buffer, w, h, 3, 2) == -1)
      assert(0);

}

void FreeBuffer()
{
    ion_manage_free(&nv12_ion_buffer);
    ion_manage_free(&h264_ion_buffer);

}

					  
void Nv12EncH264(int width,
					int height,
					IonBuffer *input_buf, 
					IonBuffer *dst_buf)
{
	char path[64] = {0};
	int nv12_file_fd;
	int h264_file_fd;
	int write_size, read_size;
  	int frame_size = (width * height * 3) >> 1;
	int encode_ret;

	void* h264_extra_data = nullptr;
	size_t h264_extra_data_size = 0;
	VideoConfig& vconfig = config.video_config;

	vconfig.width = width;
	vconfig.height = height;
	vconfig.fmt = PIX_FMT_NV12;
	vconfig.bit_rate = width * height * 7;
	if (vconfig.bit_rate > 1000000) {
		vconfig.bit_rate /= 1000000;
		vconfig.bit_rate *= 1000000;
	}
	vconfig.frame_rate = 30;
	vconfig.level = 51;
	vconfig.gop_size = vconfig.frame_rate;
	vconfig.profile = 100;
	vconfig.quality = MPP_ENC_RC_QUALITY_BEST;
	vconfig.qp_init = 26;
	vconfig.qp_step = 8;
	vconfig.qp_min = 4;
	vconfig.qp_max = 48;
	vconfig.rc_mode = MPP_ENC_RC_MODE_CBR;

	H264Encoder *h264_encoder = new H264Encoder();
	if (h264_encoder->InitConfig(config))
		assert(0);

	snprintf(path, sizeof(path), "/mnt/sdcard/nv12_1920_1080.yuv");
	nv12_file_fd = open(path, O_RDONLY);
	assert(nv12_file_fd);
	snprintf(path, sizeof(path), "/mnt/sdcard/nv12_1920_1080.h264");
	h264_file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
	assert(h264_file_fd);
	
	h264_encoder->GetExtraData(h264_extra_data, h264_extra_data_size);
  	write_size = write(h264_file_fd, h264_extra_data, h264_extra_data_size);
	assert(write_size == h264_extra_data_size);

	while((read_size = read(nv12_file_fd, nv12_ion_buffer.buffer, 
														frame_size)) > 0) {
		assert(read_size == frame_size);
		encode_ret = h264_encoder->EncodeOneFrame(input_buf, dst_buf, nullptr);
	    assert(!encode_ret);
	    assert(dst_buf->GetValidDataSize() > 0);
		if (dst_buf->GetUserFlag() & MPP_PACKET_FLAG_INTRA)
			printf("Got I frame\n");
		else
			printf("Got P frame\n");
	    write_size = write(h264_file_fd, h264_ion_buffer.buffer, dst_buf->GetValidDataSize());
	    assert(write_size == dst_buf->GetValidDataSize());
	}
	close(nv12_file_fd);
	close(h264_file_fd);
	h264_encoder->unref();
}

void* Threadloop(void* arg) 
{
	char path[64] = {0};
	int size;
	int *resolution = (int *)arg;
	int width, height;
	BufferData input_data;
	BufferData dst_data;

	width = (int)resolution[0];
	height = (int)resolution[1];

	AllocBuffer(width, height);

	input_data.vir_addr_ = nv12_ion_buffer.buffer;
	input_data.ion_data_.fd_ = nv12_ion_buffer.fd;
	input_data.ion_data_.handle_ = nv12_ion_buffer.handle;
	input_data.mem_size_ = nv12_ion_buffer.size;

	dst_data.vir_addr_ = h264_ion_buffer.buffer;
	dst_data.ion_data_.fd_ = h264_ion_buffer.fd;
	dst_data.ion_data_.handle_ = h264_ion_buffer.handle;
	dst_data.mem_size_ = h264_ion_buffer.size;

	IonBuffer input_buf(input_data);
	IonBuffer dst_buf(dst_data);

	Nv12EncH264(width, height, &input_buf, &dst_buf);

	FreeBuffer();
}

int main(int argc, char **argv)
{
	int thread_idx = 0;
	pthread_t tid = 0;
	int resolution[2] = {1920,1080};

	signal(SIGINT, SignalHandler);

	StartThread(tid, Threadloop, (void *)resolution);
	StopThread(tid);

	return 0;
}



