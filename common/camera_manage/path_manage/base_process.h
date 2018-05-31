#ifndef __BASE_PROCESS_H_
#define __BASE_PROCESS_H_

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <pthread.h>
#include <pwd.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/prctl.h>
#include <unistd.h>

#include <CameraHal/CamCifDevHwItf.h>
#include <CameraHal/CamHalVersion.h>
#include <CameraHal/CamHwItf.h>
#include <CameraHal/CamIsp11DevHwItf.h>
#include <CameraHal/CamUSBDevHwItfImc.h>
#include <CameraHal/BufferBase.h>
#include <CameraHal/CameraIspTunning.h>
#include <CameraHal/linux/v4l2-controls.h>
#include <CameraHal/linux/media/rk-isp11-config.h>
#include <CameraHal/linux/media/v4l2-config_rockchip.h>
#include <CameraHal/IonCameraBuffer.h>
#include <CameraHal/StrmPUBase.h>
#include <CameraHal/cam_types.h>
#include <iostream>
#include <linux/videodev2.h>

#include "debug.h"
#include "ion_option.h"
#include "path_manage.h"
#include "nv12_process.h"
#include "nv12_h264_process.h"
#include "nv12_jpeg_process.h"
#include "display_process.h"

#ifdef SUPPORT_ENCODER
#include "jpeg_encoder.h"
#include "h264_encoder.h"
#endif

#define SAVE_NV12_FILE
#define SAVE_JPEG_FILE
#define SAVE_H264_FILE

#define NV12_BUFFER_ALLOC_NUM	4

class NV12_PATH;
class NV12_JPEG_PATH;
class NV12_H264_PATH;
class DISPALY_PATH;

struct hal {
    shared_ptr<CamHwItf> dev;
    shared_ptr<CamHwItf::PathBase> mpath;
    shared_ptr<CamHwItf::PathBase> spath;
    shared_ptr<IonCameraBufferAllocator> bufAlloc;

    shared_ptr<NV12_PATH> nv12_path;
    shared_ptr<NV12_JPEG_PATH> nv12_jpeg_path;
    shared_ptr<NV12_H264_PATH> nv12_h264_path;
    shared_ptr<DISPALY_PATH> display_path;
};


struct Video {
	int cam_type;

    int width;
    int height;
    int fps;

    struct hal* hal;
	
    int mp_buffer_num;
    frm_info_t mpfrmFmt;
    int sp_buffer_num;
    frm_info_t spfrmFmt;

    struct ion_manage nv12_buffer[NV12_BUFFER_ALLOC_NUM];
    struct ion_manage h264_buffer[NV12_BUFFER_ALLOC_NUM];
    struct ion_manage jpeg_buffer[NV12_BUFFER_ALLOC_NUM];
    struct ion_manage display_buffer[NV12_BUFFER_ALLOC_NUM];
	
#ifdef SUPPORT_ENCODER
    int encoder_width;
    int encoder_height;
    int encoder_fps;
	MediaConfig medio_config;
	H264Encoder *h264_encoder;
#endif
    volatile int pthread_run;

    pthread_mutex_t record_mutex;
    pthread_cond_t record_cond;
};

#endif

