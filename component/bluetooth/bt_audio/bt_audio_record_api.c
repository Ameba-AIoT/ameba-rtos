/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <stdio.h>
#include <bt_debug.h>
#include <bt_audio_record_api.h>
#include <bt_audio_debug.h>
#include "audio/audio_record.h"
#include "audio/audio_control.h"

void *rtk_bt_audio_record_init(uint32_t channels, uint32_t rate, uint32_t buffer_bytes, rtk_bt_audio_mic_type_t mic_type)
{
	uint32_t format = AUDIO_FORMAT_PCM_16_BIT;
	struct AudioRecord *audio_record = NULL;

	audio_record = AudioRecord_Create();
	if (!audio_record) {
		BT_LOGE("%s :new AudioRecord failed", __func__);
		return NULL;
	}
	/* record config */
	{
		AudioRecordConfig record_config;
		record_config.sample_rate = rate;
		record_config.format = format;
		record_config.channel_count = channels;
		if (RTK_BT_AUDIO_AMIC == mic_type) {
			record_config.device = DEVICE_IN_MIC;
		} else if (RTK_BT_AUDIO_DMIC == mic_type) {
			record_config.device = DEVICE_IN_DMIC_REF_AMIC;
		} else {
			BT_LOGE("%s :unknown record type 0x%02x \r\n", __func__, mic_type);
			AudioRecord_Stop(audio_record);
			AudioRecord_Destroy(audio_record);
			return NULL;
		}
		BT_LOGA("%s: mic type is %s \r\n", __func__, (RTK_BT_AUDIO_AMIC == mic_type) ? "AMIC" : "DMIC");
		// record_frame = buffer_bytes / channels / 2 bytes(16 bit pcm)
		record_config.buffer_bytes = buffer_bytes * 2; // 0 means default period bytes
		AudioRecord_Init(audio_record, &record_config, AUDIO_INPUT_FLAG_NONE);
		AudioRecord_Start(audio_record);
	}

	return (void *)audio_record;
}

uint16_t rtk_bt_audio_record_deinit(void *record_hdl)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 1;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	AudioRecord_Stop(audio_record);
	AudioRecord_Destroy(audio_record);

	return 0;
}

uint16_t rtk_bt_audio_record_start(void *record_hdl)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 1;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	if (AudioRecord_Start(audio_record)) {
		return 1;
	}

	return 0;
}

uint16_t rtk_bt_audio_record_stop(void *record_hdl)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 1;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	AudioRecord_Stop(audio_record);

	return 0;
}

uint16_t rtk_bt_audio_record_set_sample_rate(void *record_hdl, uint32_t sample_rate)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 1;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	if (AudioRecord_SetSampleRate(audio_record, sample_rate)) {
		return 1;
	}

	return 0;
}

uint32_t rtk_bt_audio_record_get_sample_rate(void *record_hdl)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 0;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	return AudioRecord_GetSampleRate(audio_record);
}

uint16_t rtk_bt_audio_record_set_channel_count(void *record_hdl, uint32_t channel_count)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 1;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	if (AudioRecord_SetChannelCount(audio_record, channel_count)) {
		return 1;
	}
	return 0;
}

uint32_t rtk_bt_audio_record_get_channel_count(void *record_hdl)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 0;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	return AudioRecord_GetChannelCount(audio_record);
}

uint16_t rtk_bt_audio_record_set_parameters(void *record_hdl, const char *strs)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 1;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}
	if (AudioRecord_SetParameters(audio_record, strs)) {
		return 1;
	}
	return 0;
}

uint16_t rtk_bt_audio_record_set_capture_volume(uint32_t channels, uint32_t volume)
{
	if (AudioControl_SetCaptureVolume(channels, volume)) {
		return 1;
	}
	return 0;
}

uint16_t rtk_bt_audio_record_set_channel_mic_category(uint32_t channel_num, uint32_t mic_category)
{
	if (AudioControl_SetChannelMicCategory(channel_num, mic_category)) {
		return 1;
	}
	return 0;
}

uint16_t rtk_bt_audio_record_set_mic_bst_gain(uint32_t mic_category, uint32_t gain)
{
	if (AudioControl_SetMicBstGain(mic_category, gain)) {
		return 1;
	}
	return 0;
}

int32_t rtk_bt_audio_record_get_mic_bst_gain(uint32_t mic_category)
{
	return AudioControl_GetMicBstGain(mic_category);
}

int rtk_bt_audio_record_read(void *record_hdl, void *buffer, int size, bool blocking)
{
	struct AudioRecord *audio_record = NULL;

	if (!record_hdl) {
		BT_LOGE("%s: audio record is NULL", __func__);
		return 0;
	} else {
		audio_record = (struct AudioRecord *)record_hdl;
	}

	return (AudioRecord_Read(audio_record, buffer, (size_t)size, blocking));
}

void rtk_bt_audio_record_set_mute(uint32_t channel, bool muted)
{
	if (0 != AudioControl_SetRecordMute(channel, muted)) {
		BT_LOGE("audio track mute fail!! ");
	}
}

bool rtk_bt_audio_record_get_muted(uint32_t channel)
{
	return AudioControl_GetRecordMute(channel);
}
