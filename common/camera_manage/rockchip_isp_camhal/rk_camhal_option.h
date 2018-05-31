#ifndef __RK_CAMHAL_OPTION_H_
#define __RK_CAMHAL_OPTION_H_

#include "path_manage.h"

int init_camerahal(struct Video* video);
int deinit_camerahal(struct Video* video);

int alloc_camera_ionbuff(struct Video* video);
int get_camera_mp_path(struct Video* video);
int start_camera_mp_path(struct Video* video);
int stop_camera_mp_path(struct Video* video);

int set_camera_fps(struct Video* video);


#endif
