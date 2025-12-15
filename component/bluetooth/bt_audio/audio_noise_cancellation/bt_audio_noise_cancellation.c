/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <bt_debug.h>
#include <osif.h>
#include "ameba_soc.h"
#include "bt_audio_noise_cancellation.h"
#include <aivoice_interface.h>
#include <aivoice_afe_config.h>
#include <string.h>
#include "ringbuffer.h"
#include "rtk_bt_common.h"
#include "speex/speex_resampler.h"

#define RESAMPLE_CHANNEL 1
#define RESAMPLE_IN_SAMPLE_RATE 16000
#define RESAMPLE_OUT_SAMPLE_RATE 8000

struct aivoice {
	const struct rtk_aivoice_iface *iface;
	void *handle;
	uint32_t codec_index;
	SpeexResamplerState *presampler;
	unsigned int frame_bytes_per_channel;
	void *afe_out_8k_buffer;
	RingBuffer *afe_rbuffer;
};

static struct aivoice gaivoice = {
	.iface = NULL,
	.handle = NULL,
	.codec_index = RTK_BT_AUDIO_CODEC_MAX,
	.presampler = NULL,
	.frame_bytes_per_channel = 0,
	.afe_out_8k_buffer = NULL,
	.afe_rbuffer = NULL,
};

static uint8_t record_channel = 0;

static int aivoice_callback_process(void *userdata,
									enum aivoice_out_event_type event_type,
									const void *msg, int len)
{

	(void)userdata;
	(void)len;
	struct aivoice_evout_afe *afe_out;
	unsigned int frame_bytes_8k = gaivoice.frame_bytes_per_channel / 2;

	switch (event_type) {
	case AIVOICE_EVOUT_AFE:
		afe_out = (struct aivoice_evout_afe *)msg;

		if (gaivoice.codec_index == RTK_BT_AUDIO_CODEC_CVSD) {
			int error;
			spx_uint32_t in_len_frames = gaivoice.frame_bytes_per_channel / (16 / 8);
			spx_uint32_t out_len_frames = in_len_frames / 2;
			/* do resample from 16k to 8k */
			error = speex_resampler_process_int(gaivoice.presampler, 0, (spx_int16_t *)afe_out->data, &in_len_frames, (spx_int16_t *)gaivoice.afe_out_8k_buffer,
												&out_len_frames);
			if (error != RESAMPLER_ERR_SUCCESS) {
				BT_LOGE("[BT AUDIO] speex_resampler_process_int fail 0x%x \r\n", error);
				break;
			}
			if (RingBuffer_Space(gaivoice.afe_rbuffer) >= frame_bytes_8k) {
				RingBuffer_Write(gaivoice.afe_rbuffer, (uint8_t *)gaivoice.afe_out_8k_buffer, frame_bytes_8k);
			} else {
				BT_LOGE("[BT AUDIO] gaivoice.afe_rbuffer size is not enough(8k) \r\n");
			}
		} else {
			if (RingBuffer_Space(gaivoice.afe_rbuffer) >= gaivoice.frame_bytes_per_channel) {
				RingBuffer_Write(gaivoice.afe_rbuffer, (uint8_t *)afe_out->data, gaivoice.frame_bytes_per_channel);
			} else {
				BT_LOGE("[BT AUDIO] gaivoice.afe_rbuffer size is not enough(16k) \r\n");
			}
		}
		break;

	default:
		break;
	}

	return 0;
}

uint32_t rtk_bt_audio_noise_cancellation_data_get(void *buffer, uint32_t size)
{
	uint32_t read_size = 0;
	if (gaivoice.afe_rbuffer) {
		if (RingBuffer_Available(gaivoice.afe_rbuffer) >= size) {
			RingBuffer_Read(gaivoice.afe_rbuffer, buffer, size);
			read_size = size;
		}
	}

	return read_size;
}

uint16_t rtk_bt_audio_noise_cancellation_new(uint32_t codec_index, uint32_t channels)
{
	if (gaivoice.iface) {
		BT_LOGE("[BT AUDIO] aivoice ns is alreadly exist \r\n");
		return 1;
	}

	const struct rtk_aivoice_iface *aivoice = &aivoice_iface_afe_v1;
	struct aivoice_config config;
	memset(&config, 0, sizeof(config));
	struct afe_config afe_param = {0};

	switch (channels) {
	case 2: {
		struct afe_config temp_afe = AFE_CONFIG_COM_DEFAULT_1MIC();
		memcpy((void *)&afe_param, (void *)&temp_afe, sizeof(struct afe_config));
	}
	break;
	case 3: {
		struct afe_config temp_afe = AFE_CONFIG_COM_DEFAULT_2MIC50MM();
		memcpy((void *)&afe_param, (void *)&temp_afe, sizeof(struct afe_config));
	}
	break;
	default:
		BT_LOGE("[BT AUDIO] rtk_bt_audio_noise_cancellation_new invalid channels %d \r\n", (int)channels);
		return 1;
	}
	config.afe = &afe_param;
	gaivoice.frame_bytes_per_channel = afe_param.frame_size * sizeof(short);
	if (codec_index == RTK_BT_AUDIO_CODEC_CVSD) {
		int error;
		gaivoice.afe_out_8k_buffer = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, gaivoice.frame_bytes_per_channel / 2);
		if (!gaivoice.afe_out_8k_buffer) {
			BT_LOGE("[BT AUDIO] Unable to allocate afe_out_8k_buffer \r\n");
			return 1;
		}
		/* init resample instanse */
		gaivoice.presampler = speex_resampler_init(
								  RESAMPLE_CHANNEL,
								  RESAMPLE_IN_SAMPLE_RATE,
								  RESAMPLE_OUT_SAMPLE_RATE,
								  1,
								  &error);
		if (error != RESAMPLER_ERR_SUCCESS) {
			BT_LOGE("[BT AUDIO] Unable to create resampler \r\n");
			goto fail;
		}
	}
	gaivoice.afe_rbuffer = RingBuffer_Create(NULL, gaivoice.frame_bytes_per_channel * 4, LOCAL_RINGBUFF, 1);
	if (!gaivoice.afe_rbuffer) {
		BT_LOGE("[BT AUDIO] create ringbuffer failed \r\n");
		goto fail;
	}

	void *handle = aivoice->create(&config);
	if (!handle) {
		BT_LOGE("[BT AUDIO] create aivoice instance failed \r\n");
		goto fail;
	}

	rtk_aivoice_register_callback(handle, aivoice_callback_process, NULL);
	gaivoice.codec_index = codec_index;
	gaivoice.iface = aivoice;
	gaivoice.handle = handle;
	record_channel = channels;
	BT_LOGA("[BT AUDIO] aivoice instance is created, channels is %d \r\n", (int)channels);

	return 0;
fail:
	if (gaivoice.afe_rbuffer) {
		RingBuffer_Destroy(gaivoice.afe_rbuffer);
		gaivoice.afe_rbuffer = NULL;
	}
	if (codec_index == RTK_BT_AUDIO_CODEC_CVSD) {
		if (gaivoice.afe_out_8k_buffer) {
			osif_mem_free(gaivoice.afe_out_8k_buffer);
			gaivoice.afe_out_8k_buffer = NULL;
		}
		if (gaivoice.presampler) {
			speex_resampler_destroy(gaivoice.presampler);
			gaivoice.presampler = NULL;
		}
		gaivoice.codec_index = RTK_BT_AUDIO_CODEC_MAX;
	}

	return 1;
}

int rtk_bt_audio_noise_cancellation_feed(void *data, uint32_t bytes)
{
	if (bytes != gaivoice.frame_bytes_per_channel * record_channel) {
		BT_LOGE("[BT AUDIO] feed size error. expect=%d, real=%d \r\n",
				gaivoice.frame_bytes_per_channel * record_channel, bytes);
		return -1;
	}

	return gaivoice.iface->feed(gaivoice.handle, (char *)data, bytes);
}

uint16_t rtk_bt_audio_noise_cancellation_destroy(void)
{
	BT_LOGA("[BT AUDIO] entry %s \r\n", __func__);
	record_channel = 0;
	if (gaivoice.iface) {
		gaivoice.iface->destroy(gaivoice.handle);
		gaivoice.iface = NULL;
		gaivoice.handle = NULL;
		if (gaivoice.afe_rbuffer) {
			RingBuffer_Destroy(gaivoice.afe_rbuffer);
			gaivoice.afe_rbuffer = NULL;
		}
		if (gaivoice.codec_index == RTK_BT_AUDIO_CODEC_CVSD) {
			if (gaivoice.afe_out_8k_buffer) {
				osif_mem_free(gaivoice.afe_out_8k_buffer);
				gaivoice.afe_out_8k_buffer = NULL;
			}
			if (gaivoice.presampler) {
				speex_resampler_destroy(gaivoice.presampler);
				gaivoice.presampler = NULL;
			}
			gaivoice.codec_index = RTK_BT_AUDIO_CODEC_MAX;
		}
	}
	BT_LOGE("[BT AUDIO] aivoice instance is destroyed \r\n");

	return 0;
}