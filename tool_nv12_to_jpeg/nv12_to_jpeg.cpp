#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <assert.h>

#include "pthread_option.h"
#include "ion_option.h"
#include "jpeg_encoder.h"

#define PTHREAD_NUM 10

static int looping = 1;

MediaConfig config;
struct ion_manage nv12_ion_buffer;
struct ion_manage jpeg_ion_buffer;


void SignalHandler(int sig)
{
    looping = 0;
}

void AllocBuffer(int w, int h)
{
    memset(&nv12_ion_buffer, 0 , sizeof(ion_manage));
    memset(&jpeg_ion_buffer, 0 , sizeof(ion_manage));

    nv12_ion_buffer.client = -1;
    nv12_ion_buffer.fd = -1;
    jpeg_ion_buffer.client = -1;
    jpeg_ion_buffer.fd = -1;

    if (ion_manage_alloc(&nv12_ion_buffer, w, h, 3, 2) == -1)
      assert(0);
    if (ion_manage_alloc(&jpeg_ion_buffer, w, h, 3, 2) == -1)
      assert(0);

}

void FreeBuffer()
{
    ion_manage_free(&nv12_ion_buffer);
    ion_manage_free(&jpeg_ion_buffer);

}

void Nv12EncJpeg(int width,
					  int height,
					  IonBuffer *input_buf, 
					  IonBuffer *dst_buf)
{
	JpegEncConfig& jconfig = config.jpeg_config;
	jconfig.width = width;
	jconfig.height = height;
	jconfig.fmt = PIX_FMT_NV12;
	jconfig.qp = 10;  // best quality [1-10]

	MPPJpegEncoder* encoder = new MPPJpegEncoder();
	if (encoder->InitConfig(config))
		  assert(0);
	int encode_ret = encoder->Encode(input_buf, dst_buf);
	assert(!encode_ret);
	assert(dst_buf->GetValidDataSize() > 0);
	encoder->unref();
}

void ReadNv12File(int width, 
						int height, 
						struct ion_manage *nv12_buffer)
{
	char path[64] = {0};
	int read_size, write_size;
	int nv12_file_fd;
	int frame_size = width * height * 3 / 2;

	snprintf(path, sizeof(path), "/mnt/sdcard/nv12_%d_%d.yuv", width, height);
	printf("vicent----------------nv12_file_path %s\n", path);
	nv12_file_fd = open(path, O_RDONLY);
	if (nv12_file_fd < 0) {
		printf("open file %s error\n", path);
	}
	read_size = read(nv12_file_fd, nv12_buffer->buffer, frame_size);
	close(nv12_file_fd);

}
void WriteJpegFile(int width, 
						 int height, 
						 struct ion_manage *jpeg_buffer,
						 int jpeg_buffer_size)
{
	char path[64] = {0};
	int read_size, write_size;
	int jpeg_file_fd;
	int frame_size = width * height * 3 / 2;

    snprintf(path, sizeof(path), "/mnt/sdcard/nv12_%d_%d.jpg", width, height);
	printf("vicent----------------jpeg_file_path %s\n", path);
    jpeg_file_fd = open(path, O_WRONLY | O_CREAT | O_TRUNC);
    if (jpeg_file_fd < 0) {
      printf("open jpg failed, errno: %d\n", errno);
      assert(0);
    }
    write_size = write(jpeg_file_fd, jpeg_buffer->buffer, jpeg_buffer_size);
    close(jpeg_file_fd);
    if (write_size != jpeg_buffer_size) {
      printf("sd card may be full filled\n");
      unlink(path);
    }

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

	ReadNv12File(width, height, &nv12_ion_buffer);

	input_data.vir_addr_ = nv12_ion_buffer.buffer;
	input_data.ion_data_.fd_ = nv12_ion_buffer.fd;
	input_data.ion_data_.handle_ = nv12_ion_buffer.handle;
	input_data.mem_size_ = nv12_ion_buffer.size;

	dst_data.vir_addr_ = jpeg_ion_buffer.buffer;
	dst_data.ion_data_.fd_ = jpeg_ion_buffer.fd;
	dst_data.ion_data_.handle_ = jpeg_ion_buffer.handle;
	dst_data.mem_size_ = jpeg_ion_buffer.size;

	IonBuffer input_buf(input_data);
	IonBuffer dst_buf(dst_data);

	Nv12EncJpeg(width, height, &input_buf, &dst_buf);

	WriteJpegFile(width, height, &jpeg_ion_buffer, dst_buf.GetValidDataSize());

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



