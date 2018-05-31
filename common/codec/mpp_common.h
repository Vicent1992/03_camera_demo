#ifndef __MPP_COMMON_H
#define __MPP_COMMON_H

#include "encoder_base.h"
#include "ion_buffer.h"
#include "vpu.h"

MppFrameFormat ConvertToMppPixFmt(const PixelFormat& fmt);
PixelFormat ConvertToPixFmt(const MppFrameFormat& mfmt);


#endif