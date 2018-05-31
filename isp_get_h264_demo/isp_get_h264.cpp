#include <signal.h>

#include "isp_get_h264.h"
#include "pthread_option.h"

struct Video* video = NULL;


static void video_record_wait(struct Video* video)
{
    pthread_mutex_lock(&video->record_mutex);
    if (video->pthread_run)
        pthread_cond_wait(&video->record_cond, &video->record_mutex);
    pthread_mutex_unlock(&video->record_mutex);
}

static void video_record_timedwait(struct Video* video)
{
    struct timespec timeout;
    struct timeval now;
    gettimeofday(&now, NULL);
    timeout.tv_sec = now.tv_sec + 1;
    timeout.tv_nsec = now.tv_usec * 1000;

    pthread_mutex_lock(&video->record_mutex);
    if (video->pthread_run)
        pthread_cond_timedwait(&video->record_cond, &video->record_mutex, &timeout);
    pthread_mutex_unlock(&video->record_mutex);
}

void video_record_signal(struct Video* video)
{
    pthread_mutex_lock(&video->record_mutex);
    pthread_cond_signal(&video->record_cond);
    pthread_mutex_unlock(&video->record_mutex);
}

void SignalHandler(int sig)
{
	func_trace("-------------- SignalHandler\n");
    video->pthread_run = 0;
}

void* Threadloop(void* arg) 
{
	struct Video *video = (struct Video *)arg;

	init_camerahal(video);

	prepare_nv12_h264_path(video);
	start_camera_mp_path(video);

	while(video->pthread_run)
	{
		func_trace("Threadloop \n");
		video_record_timedwait(video);
	}
	stop_camera_mp_path(video);
	release_nv12_h264_path(video);
	deinit_camerahal(video);
}


int main(int argc, char *argv[])
{
	int thread_idx = 0;
	pthread_t tid = 0;
	
	video = (struct Video*)calloc(1, sizeof(struct Video));
	if (!video) {
		printf(">>>>>>>>>>>>>>>calloc struct Video no memory!\n");
	}

    video->fps = 30;
    video->width = 1920;
    video->height = 1080;

	video->encoder_width  =video->width;
	video->encoder_height =video->height;
	video->encoder_fps    =video->fps;

	if (argc >= 2) {
	    fprintf(stderr, "Usage: %s [-w width] [-h height] [-fps fps] ", argv[0]);
	    fprintf(stderr, "          [-ew enc_width] [-eh enc_height] \n");
	    fprintf(stderr, "          [-efps enc_fps] \n");
		argv += 1;
	    while (*argv) {
	        if (strcmp(*argv, "-w") == 0) {
	            argv++;
	            if (*argv)
	                video->width = atoi(*argv);
	        } else if (strcmp(*argv, "-h") == 0) {
	            argv++;
	            if (*argv)
	                video->height = atoi(*argv);
	        } else if (strcmp(*argv, "-fps") == 0) {
	            argv++;
	            if (*argv)
	                video->fps = atoi(*argv);
	        } else if (strcmp(*argv, "-ew") == 0) {
	            argv++;
	            if (*argv)
	                video->encoder_width = atoi(*argv);
	        } else if (strcmp(*argv, "-eh") == 0) {
	            argv++;
	            if (*argv)
	                video->encoder_height = atoi(*argv);
	        }
	        if (*argv)
	            argv++;
	    }    
	}

	video->mp_buffer_num = 2;
	video->mpfrmFmt.fps = video->fps;
    video->mpfrmFmt.frmSize.width = video->width;
    video->mpfrmFmt.frmSize.height = video->height;
    video->mpfrmFmt.frmFmt = HAL_FRMAE_FMT_NV12;
	video->mpfrmFmt.colorSpace = HAL_COLORSPACE_SMPTE170M;

	video->sp_buffer_num = 0;
	video->spfrmFmt.fps = video->fps;
    video->spfrmFmt.frmSize.width = 640;
    video->spfrmFmt.frmSize.height = 360;
    video->spfrmFmt.frmFmt = HAL_FRMAE_FMT_NV12;
	video->spfrmFmt.colorSpace = HAL_COLORSPACE_SMPTE170M;

    video->pthread_run = 1;
	
    pthread_mutex_init(&video->record_mutex, NULL);
    pthread_cond_init(&video->record_cond, NULL);

	signal(SIGINT, SignalHandler);
	StartThread(tid, Threadloop, (void *)video);
	StopThread(tid);

	pthread_mutex_destroy(&video->record_mutex);
	pthread_cond_destroy(&video->record_cond);

    return 0;
}

