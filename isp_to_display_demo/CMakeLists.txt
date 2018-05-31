project(isp_to_display)
cmake_minimum_required(VERSION 2.8.12)

set(TARGET_NAME isp_to_display)

set(SRC_FILES
    isp_to_display.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/path_manage/display_process.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/path_manage/path_manage.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/rockchip_isp_camhal/rk_camhal_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/ion_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/pthread_manage/pthread_option.cpp
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
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
	${CMAKE_CURRENT_SOURCE_DIR}/../../../out/system/include/
)

add_executable(${TARGET_NAME} ${SRC_FILES})
target_link_libraries(${TARGET_NAME} ion rkfb rkrga cam_hal pthread)

install(TARGETS ${TARGET_NAME} DESTINATION bin)
