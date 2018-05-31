#include "encoder_packet.h"

EncodedPacket::EncodedPacket() 
{
	av_init_packet(&av_pkt);
	type = PACKET_TYPE_COUNT;
	memset(&time_val, 0, sizeof(struct timeval));
	audio_index = 0;
	is_phy_buf = false;
	memset(&lbr_time_val, 0, sizeof(lbr_time_val));
}

EncodedPacket::~EncodedPacket() 
{
	// printf("pkt: %p, free av_pkt: %d\n", this, av_pkt.size);
	av_packet_unref(&av_pkt);
}

int EncodedPacket::copy_av_packet() 
{
	assert(is_phy_buf);
	AVPacket pkt = av_pkt;
	if (0 != av_copy_packet(&av_pkt, &pkt)) {
		printf("in %s, av_copy_packet failed!\n", __func__);
		return -1;
	}
	is_phy_buf = false;
	return 0;
}

