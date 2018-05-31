#ifndef __JPEG_ENCODER_H
#define __JPEG_ENCODER_H

#include <unistd.h>

#include "encoder_base.h"
#include "ion_buffer.h"
#include "vpu.h"
#include "mpp_common.h"
#include "debug.h"

class MPPJpegEncoder : public BaseEncoder {
 public:
  MPPJpegEncoder();
  virtual ~MPPJpegEncoder();
  virtual int InitConfig(MediaConfig& config) override;
  int Encode(IonBuffer* src, IonBuffer* dst);

 private:
  struct vpu_encode* venc_;
};

#endif  // MPP_JPEG_ENCODER_H
