
#ifndef __ENCODE_PACKET_H_
#define __ENCODE_PACKET_H_

#include "video_object.h"

extern "C" {
#include <libavcodec/avcodec.h>
}

typedef enum {
  VIDEO_PACKET,
  AUDIO_PACKET,
  SUBTITLE_PACKET,
  VIDEO_INFO_PACKET,
  PACKET_TYPE_COUNT
} PacketType;

class EncodedPacket : public video_object {
public:
	AVPacket av_pkt;
	PacketType type;
	bool is_phy_buf;
	union {
		struct timeval time_val;  // store the time for video_packet
		uint64_t audio_index;     // audio packet index
	};
	struct timeval lbr_time_val;
	EncodedPacket();
	~EncodedPacket();
	int copy_av_packet();
};

#endif
