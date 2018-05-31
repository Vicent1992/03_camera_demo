#include "rk_camhal_option.h"

struct rk_cams_dev_info g_test_cam_infos;

int init_camerahal(struct Video* video)
{
	func_func();

    video->hal = new hal();

  	memset(&g_test_cam_infos, 0, sizeof(g_test_cam_infos));
	CamHwItf::getCameraInfos(&g_test_cam_infos);

    video->hal->dev = getCamHwItf(&g_test_cam_infos.isp_dev);
	if (!video->hal->dev.get()) {
	   printf(">>>>>>>>>>>>>>>getCamHwItf no memory!\n");
	   return -1;
	}
	
	func_trace("g_test_cam_infos num_camers %d\n", g_test_cam_infos.num_camers);

	for (int i = 0; i < g_test_cam_infos.num_camers; i++) {
        if (g_test_cam_infos.cam[i]->type == RK_CAM_ATTACHED_TO_ISP) {
            func_trace("connected isp camera name %s,input id %d\n",
                   				g_test_cam_infos.cam[i]->name, 
                   				g_test_cam_infos.cam[i]->index);

            if (video->hal->dev->initHw(g_test_cam_infos.cam[i]->index) == false) {
                printf("video init fail!\n");
                return -1;
            }
            break;
        }
    }

	alloc_camera_ionbuff(video);
	get_camera_mp_path(video);
	set_camera_fps(video);
}


int deinit_camerahal(struct Video* video)
{
	func_func();

    if (video->hal->dev.get()) {
		video->hal->dev->deInitHw();
    }
	video->hal->dev.reset();
	if (video->hal) {
		delete video->hal;
		video->hal = NULL;
	}
	free(video);
}

int alloc_camera_ionbuff(struct Video* video)
{
	video->hal->bufAlloc =
        shared_ptr<IonCameraBufferAllocator>(new IonCameraBufferAllocator());
    if (!video->hal->bufAlloc.get()) {
        printf("new IonCameraBufferAllocator failed!\n");
        return -1;
    }
}

int get_camera_mp_path(struct Video* video)
{

	video->hal->mpath = video->hal->dev->getPath(CamHwItf::MP);
    if (video->hal->mpath.get() == NULL) {
        printf("%s:path doesn't exist!\n", __func__);
        return -1;
    }

    if (video->hal->mpath->prepare(video->mpfrmFmt, video->mp_buffer_num, 
					*(video->hal->bufAlloc.get()), false, 0) == false) {
        printf("mp prepare faild!\n");
        return -1;
    }

    func_trace("mp: width = %4d,height = %4d\n", 
    			video->mpfrmFmt.frmSize.width,
           		video->mpfrmFmt.frmSize.height);
}

int start_camera_mp_path(struct Video* video)
{
    if (!video->hal->mpath->start()) {
        printf("mpath start failed!\n");
        return -1;
    }
}

int stop_camera_mp_path(struct Video* video)
{
    if (video->hal->mpath.get()) {
        video->hal->mpath->stop();
        video->hal->mpath->releaseBuffers();
    }
}

int set_camera_fps(struct Video* video)
{
	int ret = 0;
    HAL_FPS_INFO_t fps;

    fps.numerator = 1;
    fps.denominator = video->fps;
    func_trace("fps : %d/%d\n", fps.numerator, fps.denominator);

    ret = video->hal->dev->setFps(fps);
    if (!ret) {
        printf("set_camera_fps faild\n");
    }
    return ret;
}

