project(isp_get_jpeg)
cmake_minimum_required(VERSION 2.8.12)

set(TARGET_NAME isp_get_jpeg)

set(SRC_FILES
    isp_get_jpeg.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/path_manage/nv12_jpeg_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/path_manage/path_manage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/rockchip_isp_camhal/rk_camhal_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/ion_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/ion_buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/pthread_manage/pthread_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/video_object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/vpu.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/mpp_common.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/encoder_base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/encoder_packet.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/jpeg_encoder.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
	-DSUPPORT_ENCODER \
	-DSUPPORT_ION \
	-DLINUX \
	-DENABLE_ASSERT \
	-std=c++11")

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/debug/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/pthread_manage/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/path_manage/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/rockchip_isp_camhal
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/
	${CMAKE_CURRENT_SOURCE_DIR}/../../../out/system/include/
)

add_executable(${TARGET_NAME} ${SRC_FILES})
target_link_libraries(${TARGET_NAME} ion cam_hal avformat avcodec avutil mpp pthread)

install(TARGETS ${TARGET_NAME} DESTINATION bin)
