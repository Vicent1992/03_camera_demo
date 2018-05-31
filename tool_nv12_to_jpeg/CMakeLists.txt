project(nv12_to_jpeg)
cmake_minimum_required(VERSION 2.8.12)

set(TARGET_NAME nv12_to_jpeg)

set(SRC_FILES
    ${TARGET_NAME}.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/pthread_manage/pthread_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/ion_option.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/ion_buffer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/video_object.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/vpu.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/mpp_common.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/encoder_base.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/encoder_packet.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/jpeg_encoder.cpp
)

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}/../common/debug/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/pthread_manage/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/buffer_manage/ion/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/
	${CMAKE_CURRENT_SOURCE_DIR}/../common/codec/encoder/
)

add_executable(${TARGET_NAME} ${SRC_FILES})
target_link_libraries(${TARGET_NAME} avformat avcodec avutil pthread ion mpp)

install(TARGETS ${TARGET_NAME} DESTINATION bin)
