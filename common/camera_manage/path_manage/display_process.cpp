#include "display_process.h"

extern "C" {
#include "rk_fb/rk_fb.h"
#include "rk_rga/rk_rga.h"
}

#define WIN_W 1280
#define WIN_H 720

void ui_fill_colorkey(void)
{
    struct win * ui_win;
    struct color_key color_key;
    unsigned short rgb565_data;
    unsigned short *ui_buff;
    int i;
    int w, h;
	
#define COLOR_KEY_R 0x0
#define COLOR_KEY_G 0x0
#define COLOR_KEY_B 0x1

    ui_win = rk_fb_getuiwin();
    ui_buff = (unsigned short *)ui_win->buffer;

    /* enable and set color key */
    color_key.enable = 1;
    color_key.red = (COLOR_KEY_R & 0x1f) << 3;
    color_key.green = (COLOR_KEY_G & 0x3f) << 2;
    color_key.blue = (COLOR_KEY_B & 0x1f) << 3;
    rk_fb_set_color_key(color_key);

    rk_fb_get_out_device(&w, &h);

    /* set ui win color key */
    rgb565_data = (COLOR_KEY_R & 0x1f) << 11 | ((COLOR_KEY_G & 0x3f) << 5) | (COLOR_KEY_B & 0x1f);
    for (i = 0; i < w * h; i ++) {
        ui_buff[i] = rgb565_data;
    }
}


DISPALY_PATH::DISPALY_PATH(struct Video* p) 
	: StreamPUBase("DISPALY_PATH", true, true) 
	, buffer_index(0)
{ 
	func_trace("DISPALY_PATH\n");
	video = p;
	disp_rga_fd = rk_rga_open();
	rk_fb_init(FB_FORMAT_RGB_565);
	ui_fill_colorkey();
}

DISPALY_PATH::~DISPALY_PATH() {
	func_trace("~DISPALY_PATH\n");
	rk_fb_deinit();
    rk_rga_close(disp_rga_fd);
}

bool DISPALY_PATH::processFrame(shared_ptr<BufferBase> inBuf,
                      shared_ptr<BufferBase> outBuf) 
{                  
	int src_w, src_h, src_fd;
	int dst_w, dst_h, dst_fd;
	int src_fmt, dst_fmt;
	int x, y;
    struct win* video_win;

	x = 0;
	y = 0;

	src_w   = inBuf->getWidth();
	src_h   = inBuf->getHeight();
	src_fd  = inBuf->getFd();
	src_fmt = RGA_FORMAT_YCBCR_420_SP;

	dst_w   = video->display_buffer[buffer_index].width;
	dst_h   = video->display_buffer[buffer_index].height;
	dst_fd  = video->display_buffer[buffer_index].fd;
	dst_fmt = RGA_FORMAT_YCBCR_420_SP;

	if (rk_rga_ionfd_to_ionfd_scal(disp_rga_fd,
								src_fd, src_w, src_h, src_fmt,
								dst_fd, dst_w, dst_h, dst_fmt,
								x, y, dst_w, dst_h, src_w, src_h))
		printf("%s: %d failed!\n", __func__, __LINE__);
	
	func_trace("1 %d %d %d %d\n", src_w, src_h, dst_w, dst_h);

	src_w   = video->display_buffer[buffer_index].width;
	src_h   = video->display_buffer[buffer_index].height;
	src_fd  = video->display_buffer[buffer_index].fd;
	src_fmt = RGA_FORMAT_YCBCR_420_SP;

    video_win = rk_fb_getvideowin();
	dst_w   = video_win->video_ion.width;
	dst_h   = video_win->video_ion.height;
	dst_fd  = video_win->video_ion.fd;
	dst_fmt = RGA_FORMAT_YCBCR_420_SP;

    if (rk_rga_ionfd_to_ionfd_rotate(disp_rga_fd,
                                 src_fd, src_w, src_h, src_fmt, src_w, src_h,
                                 dst_fd, dst_w, dst_h, dst_fmt, 90))
		printf("%s: %d failed!\n", __func__, __LINE__);

	func_trace("2 %d %d %d %d\n", src_w, src_h, dst_w, dst_h);

	rk_fb_video_disp(video_win);

	(buffer_index >= NV12_BUFFER_ALLOC_NUM-1)?
					 (buffer_index = 0):(buffer_index++);
	printf("NV12_JPEG_PATH buffer_index %d\n", buffer_index);

    return true;
}


int prepare_display_path(struct Video* video)
{
	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++) {
		memset (&video->display_buffer[i], 0, sizeof(struct ion_manage));
		video->display_buffer[i].client = -1;
		video->display_buffer[i].fd = -1;
		if (ion_manage_alloc(&video->display_buffer[i], WIN_W, WIN_H, 3, 2)) {
			printf("isp_video nv12_path ion alloc fail!\n");
			return -1;
		}
	}
	if (video->hal->mpath.get()) {
		video->hal->display_path = shared_ptr<DISPALY_PATH>(new DISPALY_PATH(video));
		if (hal_add_path(video->hal->mpath, 
				video->hal->display_path, 
				video->mpfrmFmt, 
				NV12_BUFFER_ALLOC_NUM, NULL))
		return -1;
	}
	return 0;
}

int release_display_path(struct Video* video)
{
	if (video->hal->mpath.get()) {
		hal_remove_path(video->hal->mpath, video->hal->display_path);
		video->hal->display_path.reset();
	}

	for (int i = 0; i < NV12_BUFFER_ALLOC_NUM; i++)
		ion_manage_free(&video->display_buffer[i]);

	return 0;
}

