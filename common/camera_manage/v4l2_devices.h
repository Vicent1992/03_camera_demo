#ifndef __V4L2_DEVICES_H__
#define __V4L2_DEVICES_H__

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define RQ_BUFFER_NUM 4

enum camera_type{
	CAM_TYPE_ISP = 0,
	CAM_TYPE_CIF,
	CAM_TYPE_USB,
};

enum COLORSPACE {
  COLORSPACE_SMPTE170M = 1,
  COLORSPACE_JPEG = 7,
};


struct Cam_Device {
    int cam_id;
    int cam_fd;
    int cam_type;
    int width;
    int height;
    int fps;
	int pixelformat;
	int pixelformat_num;
	int formattype;
	int color_space;
	int rqbuf_cnt;
	int rqbuf_len[RQ_BUFFER_NUM];
	void *rqbuf[RQ_BUFFER_NUM];

	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc; 
	struct v4l2_frmsizeenum frmsize;
	struct v4l2_format v4l2Fmt;
    //struct Cam_Hal* cam_hal;
};
	
int get_cam_capability(struct Cam_Device *cam_dev);
void enum_cam_format(struct Cam_Device *cam_dev);
void enum_cam_framesize(struct Cam_Device *cam_dev);
int get_cam_format(struct Cam_Device *cam_dev);
int set_cam_format(struct Cam_Device *cam_dev);
void try_cam_format(struct Cam_Device *cam_dev);

int request_buff(struct Cam_Device *cam_dev);
int unrequest_buff(struct Cam_Device *cam_dev);
int stream_on(struct Cam_Device *cam_dev);
int stream_off(struct Cam_Device *cam_dev);

int capture_camera(struct Cam_Device *cam_dev);

int open_cam_dev(struct Cam_Device *cam_dev);
int close_cam_dev(struct Cam_Device *cam_dev);



#endif
