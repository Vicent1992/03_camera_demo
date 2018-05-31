project(get_nv12)
cmake_minimum_required(VERSION 2.8.12)

set(TARGET_NAME get_nv12)

set(SRC_FILES
    ${TARGET_NAME}.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/v4l2_devices.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common/camera_manage/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/debug/
)

add_executable(${TARGET_NAME} ${SRC_FILES})
target_link_libraries(${TARGET_NAME})

install(TARGETS ${TARGET_NAME} DESTINATION bin)
