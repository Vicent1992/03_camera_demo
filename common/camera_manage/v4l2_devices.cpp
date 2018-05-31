#include "v4l2_devices.h" 
#include "debug.h"

void enum_cam_format(struct Cam_Device *cam_dev) 
{
	cam_dev->fmtdesc.index = 0;
	cam_dev->fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;


	while (ioctl(cam_dev->cam_fd, VIDIOC_ENUM_FMT, &cam_dev->fmtdesc) == 0) 
	{ 
      	func_trace(" camid [%d] support format[%x] \n", 
			       cam_dev->cam_id, cam_dev->fmtdesc.pixelformat);
		cam_dev->fmtdesc.index++;
	}

	cam_dev->pixelformat_num = cam_dev->fmtdesc.index;
}

void enum_cam_framesize(struct Cam_Device *cam_dev) 
{ 
	cam_dev->frmsize.index = 0;
	cam_dev->frmsize.pixel_format = V4L2_PIX_FMT_NV21;
  
	while (ioctl(cam_dev->cam_fd, VIDIOC_ENUM_FRAMESIZES, &cam_dev->frmsize) == 0)
	{ 
      	func_trace("  camid [%d] support framesize[%dx%d] \n", 
			       cam_dev->cam_id, 
			       cam_dev->frmsize.discrete.width,
			       cam_dev->frmsize.discrete.height);
		cam_dev->frmsize.index++;
	}
}

int get_cam_format(struct Cam_Device *cam_dev) 
{
	int ret = 0;

	memset(&cam_dev->v4l2Fmt, 0, sizeof(cam_dev->v4l2Fmt));

	cam_dev->v4l2Fmt.type = cam_dev->formattype;


	ret = ioctl(cam_dev->cam_fd, VIDIOC_G_FMT, &cam_dev->v4l2Fmt);
	if (ret < 0) {
		printf("[%s]:VIDIOC_G_FMT failed failed: %s\n", __func__, strerror(errno));
		return ret;
	}

	func_trace("camid [%d] resolution= [%dx%d],format.fmt.raw_data[9]=%d\n",
			   cam_dev->cam_id, 
			   cam_dev->v4l2Fmt.fmt.pix.width, 
			   cam_dev->v4l2Fmt.fmt.pix.height,
			   cam_dev->v4l2Fmt.fmt.raw_data[9]);

	return 0;
}

int set_cam_format(struct Cam_Device *cam_dev) 
{
	int ret = 0;

	memset(&cam_dev->v4l2Fmt, 0, sizeof(cam_dev->v4l2Fmt));

	cam_dev->v4l2Fmt.fmt.pix.width = cam_dev->width;
	cam_dev->v4l2Fmt.fmt.pix.height = cam_dev->height;
	cam_dev->v4l2Fmt.type = cam_dev->formattype;

	cam_dev->v4l2Fmt.fmt.pix.bytesperline = cam_dev->width;
	cam_dev->v4l2Fmt.fmt.pix.pixelformat = cam_dev->pixelformat;
	cam_dev->v4l2Fmt.fmt.pix.colorspace = cam_dev->color_space;
	cam_dev->v4l2Fmt.fmt.pix.sizeimage = (cam_dev->width * cam_dev->height * 3) >> 1;

	ret = ioctl(cam_dev->cam_fd, VIDIOC_S_FMT, &cam_dev->v4l2Fmt);
	if (ret < 0) {
		printf("[%s]:VIDIOC_S_FMT failed failed: %s\n", __func__, strerror(errno));
		return ret;
	}
	func_trace("camid [%d] resolution= [%dx%d],format.fmt.raw_data[9]=%d\n",
			   cam_dev->cam_id, 
			   cam_dev->v4l2Fmt.fmt.pix.width, 
			   cam_dev->v4l2Fmt.fmt.pix.height,
			   cam_dev->v4l2Fmt.fmt.raw_data[9]);

	return 0;
}

void try_cam_format(struct Cam_Device *cam_dev)
{
	struct v4l2_format fmt;

	cam_dev->v4l2Fmt.type = cam_dev->formattype; 

	cam_dev->v4l2Fmt.fmt.pix.pixelformat = cam_dev->pixelformat;
	
	if (ioctl(cam_dev->cam_fd, VIDIOC_TRY_FMT,&cam_dev->v4l2Fmt) == 0) {
		func_trace(" support format %x!\n", cam_dev->pixelformat);
	} else {
		func_trace(" not support format %x!\n", cam_dev->pixelformat);
	}
}


int get_cam_capability(struct Cam_Device *cam_dev)
{
	if (ioctl(cam_dev->cam_fd, VIDIOC_QUERYCAP, &cam_dev->cap)) 
	{
    	printf("[%s]:cam[%d] VIDIOC_QUERYCAP failed!\n", __func__, cam_dev->cam_id);
    	return -1;
	}
	func_trace("cam[%d] businfo:%s\n",cam_dev->cam_id, cam_dev->cap.bus_info);

	if (strstr((char*)cam_dev->cap.bus_info, "isp"))
		cam_dev->cam_type = CAM_TYPE_ISP;
	else if (strstr((char*)cam_dev->cap.bus_info, "cif"))
		cam_dev->cam_type = CAM_TYPE_CIF;
	else if (strstr((char*)cam_dev->cap.bus_info, "usb"))
		cam_dev->cam_type = CAM_TYPE_USB;
	
    return 0;
}

int request_buff(struct Cam_Device *cam_dev)
{
	int ret = 0;
	struct v4l2_requestbuffers v4l2rb;
	
	memset(&v4l2rb, 0, sizeof(v4l2rb));
    v4l2rb.type   = cam_dev->formattype; 
    v4l2rb.memory = V4L2_MEMORY_MMAP; 
    v4l2rb.count  = cam_dev->rqbuf_cnt; 
	
	ret = ioctl(cam_dev->cam_fd, VIDIOC_REQBUFS, &v4l2rb); 
    if (ret < 0) 
	{ 
        printf("[%s]:VIDIOC_REQBUFS failed: %s\n", __func__, strerror(errno)); 
		return ret;
    } 
	cam_dev->rqbuf_cnt = v4l2rb.count;
	func_trace("VIDIOC_REQBUFS buffer_cnt count: %d\n", cam_dev->rqbuf_cnt);

	//quebuf map qbuf
	struct v4l2_buffer buf;
	for (int i = 0; i < cam_dev->rqbuf_cnt; i++) 
	{  
        memset (&buf, 0, sizeof (struct v4l2_buffer)); 
		buf.type   = cam_dev->formattype; 
		buf.memory = V4L2_MEMORY_MMAP; 
		buf.index  = i; 
		
		ret = ioctl (cam_dev->cam_fd, VIDIOC_QUERYBUF, &buf); 
        if (ret < 0) 
		{ 
            printf("[%s]:VIDIOC_QUERYBUF Unable to query buffer (%s)\n", __func__, strerror(errno)); 
            return ret; 
        } 

 
        cam_dev->rqbuf[i] = mmap (0, buf.length, 
		                           PROT_READ | PROT_WRITE, 
		                           MAP_SHARED, 
		                           cam_dev->cam_fd, 
		                           buf.m.offset); 
		cam_dev->rqbuf_len[i] = buf.length;

		func_trace(" index: %d\n",  i);
		func_trace(" offset: %x\n",  buf.m.offset);
		func_trace(" mem: %p\n",  cam_dev->rqbuf[i]);
		func_trace(" len: %d\n",  cam_dev->rqbuf_len[i]);
 
        if (cam_dev->rqbuf[i] == MAP_FAILED) 
		{ 
			printf("[%s]:mmap Unable to map buffer (%s)\n", __func__, strerror(errno)); 
            return -1; 
        } 

        ret = ioctl(cam_dev->cam_fd, VIDIOC_QBUF, &buf); 
        if (ret < 0) 
		{ 
            printf("[%s]:VIDIOC_QBUF Failed\n", __func__); 
            return ret; 
        } 
	} 

}

int unrequest_buff(struct Cam_Device *cam_dev)
{
	int ret = 0;

	for (int i = 0; i < cam_dev->rqbuf_cnt; i++) 
	{
		ret = munmap(cam_dev->rqbuf[i], cam_dev->rqbuf_len[i]);
        if (ret < 0) 
            printf("[%s]:unrequest_buff index %d munmap failed\n", __func__, i); 
		cam_dev->rqbuf[i] = NULL;
	}

	return ret;
}

int stream_on(struct Cam_Device *cam_dev)
{
	int ret = 0;
	enum v4l2_buf_type type = (enum v4l2_buf_type)cam_dev->formattype; 

  	ret = ioctl (cam_dev->cam_fd, VIDIOC_STREAMON, &type); 
	if (ret < 0) 
	{ 
		printf("[%s]:VIDIOC_STREAMON: Unable to start capture: %s\n", __func__, strerror(errno)); 
		return ret; 
	} 

	return ret;
}

int stream_off(struct Cam_Device *cam_dev)
{
	int ret = 0;
	enum v4l2_buf_type type = (enum v4l2_buf_type)cam_dev->formattype; 

  	ret = ioctl (cam_dev->cam_fd, VIDIOC_STREAMOFF, &type); 
	if (ret < 0) 
	{ 
		printf("[%s]:VIDIOC_STREAMON: Unable to start capture: %s\n", __func__, strerror(errno)); 
		return ret; 
	} 
	return ret;
}

int capture_camera(struct Cam_Device *cam_dev)
{
	struct v4l2_buffer buf;

	void * pbuffer = NULL;
	int buf_size = 0;

	char fdstr[30];
	FILE *file_fd = NULL;
	int i,num;

	memset (&buf, 0, sizeof (v4l2_buffer));
	buf.type = cam_dev->formattype;
	buf.memory = V4L2_MEMORY_MMAP;

	if (-1 ==ioctl (cam_dev->cam_fd, VIDIOC_DQBUF, &buf) )
		return -1;

	assert (buf.index < cam_dev->rqbuf_cnt);


	func_trace("file length = %d\n",cam_dev->rqbuf_len[buf.index]);
	func_trace("file start = %p\n",cam_dev->rqbuf[buf.index]);
	
	pbuffer = cam_dev->rqbuf[buf.index];
	buf_size = cam_dev->rqbuf_len[buf.index];

	file_fd = fopen("/mnt/sdcard/save.yuv","w");
	fwrite(pbuffer, buf_size, 1, file_fd);
	fclose(file_fd);

	if (-1 == ioctl (cam_dev->cam_fd, VIDIOC_QBUF, &buf))
		return -1;

	return 0;
}


int open_cam_dev(struct Cam_Device *cam_dev)
{
    char dev[20] = {0};
	
    snprintf(dev, sizeof(dev), "/dev/video%d", cam_dev->cam_id);
    cam_dev->cam_fd = open(dev, O_RDWR);
    if (cam_dev->cam_fd <= 0) {
        printf("[%s]:open %s failed\n", __func__, dev);
        return -1;
    }
    return 0;
}
						  
int close_cam_dev(struct Cam_Device *cam_dev)
{
    close(cam_dev->cam_fd);
    return 0;
}
