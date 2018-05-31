#ifndef __H264_ENCODER_H_
#define __H264_ENCODER_H_


#include "encoder_base.h"
#include "mpp_common.h"
#include "debug.h"



// A encoder which call the mpp interface directly.
// Not thread-safe.
class H264Encoder : public BaseVideoEncoder {
 public:
#ifndef MPP_PACKET_FLAG_INTRA
#define MPP_PACKET_FLAG_INTRA (0x00000008)
#endif
  typedef struct {
    MppCodingType video_type;
    MppCtx ctx;
    MppApi* mpi;
    MppPacket packet;
    MppFrame frame;
    MppBuffer osd_data;
  } RkMppEncContext;

  H264Encoder();
  ~H264Encoder();

  virtual int InitConfig(MediaConfig& config);
  // Change configs which are not contained in sps/pps.
  int CheckConfigChange();
  // Encode the raw srcbuf to dstbuf
  virtual int EncodeOneFrame(IonBuffer* src, IonBuffer* dst, IonBuffer* mv);
  // Control before encoding.
  int EncodeControl(int cmd, void* param);
  inline void GetExtraData(void*& extra_data, size_t& extra_data_size) {
    extra_data = extra_data_;
    extra_data_size = extra_data_size_;
  }

 private:
  RkMppEncContext mpp_enc_ctx_;
  void* extra_data_;
  size_t extra_data_size_;
};

#endif

