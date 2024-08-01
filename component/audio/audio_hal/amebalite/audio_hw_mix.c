#include "audio_hw_mix.h"

static int16_t Transform32To16(int32_t one_sample)
{
	if ((one_sample >> 15) ^ (one_sample >> 31)) {
		one_sample = 0x7FFF ^ (one_sample >> 31);
	}
	return one_sample;
}

void ProcessMix(const void *buffer, int16_t *raw_out, size_t frame_count)
{
	size_t i;
	int32_t left_tmp;
	int32_t right_tmp;
	int32_t left_mix_right;
	const int16_t *raw_in = (int16_t *)buffer;
	for (i = 0 ; i < frame_count; i++) {
		left_tmp = (int32_t) * raw_in;
		raw_in++;
		right_tmp = (int32_t) * raw_in;
		raw_in++;
		left_mix_right = left_tmp + right_tmp;
		*raw_out = Transform32To16(left_mix_right);
		raw_out++;
	}

}
