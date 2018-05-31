#include "v4l2_devices.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
    int ret = 0;
    char dev[20] = {0};
	struct Cam_Device cam_dev;

	if (argc < 2) {
        fprintf(stderr, "Usage: %s  [-id cam_id] [-w width] [-h height]\n", argv[0]);
        return 1;
    }

	cam_dev.cam_id = 0;
	cam_dev.width = 1920;
	cam_dev.height = 1080;
	cam_dev.fps = 30;
	cam_dev.pixelformat = V4L2_PIX_FMT_NV12;
	cam_dev.formattype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	cam_dev.color_space = COLORSPACE_SMPTE170M;
	cam_dev.rqbuf_cnt = RQ_BUFFER_NUM;
  	memset(&cam_dev.cap, 0, sizeof(cam_dev.cap));
  	memset(&cam_dev.fmtdesc, 0, sizeof(cam_dev.fmtdesc));
  	memset(&cam_dev.frmsize, 0, sizeof(cam_dev.frmsize));
  	memset(&cam_dev.v4l2Fmt, 0, sizeof(cam_dev.v4l2Fmt));

	argv += 1;
    while (*argv) {
        if (strcmp(*argv, "-id") == 0) {
            argv++;
            if (*argv)
                cam_dev.cam_id = atoi(*argv);
        } else if (strcmp(*argv, "-w") == 0) {
            argv++;
            if (*argv)
                cam_dev.width = atoi(*argv);
        } else if (strcmp(*argv, "-h") == 0) {
            argv++;
            if (*argv)
                cam_dev.height = atoi(*argv);
        } else if (strcmp(*argv, "-fps") == 0) {
            argv++;
            if (*argv)
                cam_dev.fps = atoi(*argv);
        } 
        if (*argv)
            argv++;
    }

	printf("cam_id %d width %d height %d\n", 
		   					cam_dev.cam_id, 
		   					cam_dev.width, 
		   					cam_dev.height);

	ret = open_cam_dev(&cam_dev);
	if (ret < 0) {
		printf("open_cam_dev err!");
		goto out1;
	}

    ret = get_cam_capability(&cam_dev);
	if (ret < 0) {
		printf("get_cam_capability err!");
		goto out1;
	}

	enum_cam_format(&cam_dev);
	enum_cam_framesize(&cam_dev);
	
	ret = get_cam_format(&cam_dev);
	ret = set_cam_format(&cam_dev);
	try_cam_format(&cam_dev);

	ret = request_buff(&cam_dev);
	if (ret < 0) {
		printf("request_buff err!");
		goto out2;
	}
	ret = stream_on(&cam_dev);
	if (ret < 0) {
		printf("stream_on err!");
		goto out3;
	}

	ret = capture_camera(&cam_dev);
	if (ret < 0) {
		printf("capture_camera err!");
		goto out4;
	}

	stream_off(&cam_dev);
	unrequest_buff(&cam_dev);
	close_cam_dev(&cam_dev);
	return 0;

out4:
	stream_off(&cam_dev);
out3:
	unrequest_buff(&cam_dev);
out2:
	close_cam_dev(&cam_dev);
out1:
	return 0;
}
