/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include "platform_autoconf.h"
#include <bt_debug.h>
#include <ameba_soc.h>
#include <bt_audio_track_api.h>
#include <bt_audio_debug.h>
#include <audio/audio_track.h>
#include <audio/audio_service.h>
#include <audio/audio_control.h>

#define AUDIO_SP_CTRL0                          0x4100D004

void *rtk_bt_audio_track_init(uint32_t channels,
							  uint32_t rate,
							  uint32_t format,
							  uint32_t period_size,
							  uint32_t period_count,
							  uint32_t duration)
{
	(void)period_size;
	(void)period_count;
	struct AudioTrack *audio_track = NULL;
	int track_buf_size = 4096;
#if defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	uint32_t track_start_threshold = 0;
#endif
	uint32_t flags = AUDIO_OUTPUT_FLAG_NONE;
	uint32_t bits = 16;

	AudioService_Init();
	audio_track = AudioTrack_Create();
	if (!audio_track) {
		BT_LOGE("%s :new AudioTrack failed", __func__);
		return NULL;
	}
	/* audio_config */
	{
		switch (format) {
		case BT_AUDIO_FORMAT_PCM_8_BIT:
			bits = 8;
			break;
		case BT_AUDIO_FORMAT_PCM_16_BIT:
			bits = 16;
			break;
		case BT_AUDIO_FORMAT_PCM_32_BIT:
			bits = 32;
			break;
		default:
			bits = 16;
			break;
		}
		/* mixer period size is config within ameba_audio_mixer_usrcfg.cpp */
		/* passthrough has no period size concept */
		if (duration) {
			track_buf_size = (duration * ((rate / 1000) * channels * (bits / 8)) / 1000) * 6;
		} else {
			track_buf_size = AudioTrack_GetMinBufferBytes(audio_track, AUDIO_CATEGORY_MEDIA, rate, format, channels) * 3;
			//track_buf_size = 22608;
		}
		AudioTrackConfig  track_config;
		track_config.category_type = AUDIO_CATEGORY_MEDIA;
		track_config.sample_rate = rate;
		track_config.format = format;
		track_config.channel_count = channels;
		track_config.buffer_bytes = track_buf_size;
		AudioTrack_Init(audio_track, &track_config, flags);

#if defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
		/* set audio play start buffer size to match play latency and play fluency */
		if (duration) {
			int32_t thresholdBytes = (duration * ((rate / 1000) * channels * (bits / 8)) / 1000) * 3;
			AudioTrack_SetStartThresholdBytes(audio_track, thresholdBytes);
		} else {
			AudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);
		}
		track_start_threshold = AudioTrack_GetStartThresholdBytes(audio_track);
		BT_LOGA("%s: get start threshold:%d\r\n", __func__, (int)track_start_threshold);
#endif
		BT_LOGE("%s sample_rate %d, channel_count %d, track_buf_size %d \r\n", __func__,
				(int)rate,
				(int)channels,
				(int)track_buf_size);
	}

	return (void *)audio_track;
}

uint16_t rtk_bt_audio_track_deinit(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_Pause(audio_track);
	AudioTrack_Stop(audio_track);
	AudioTrack_Destroy(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_stop(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_Stop(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_pause(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_Pause(audio_track);
	AudioTrack_Flush(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_pause_without_flush(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_Pause(audio_track);

	return 0;
}
uint16_t rtk_bt_audio_track_resume(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_Start(audio_track);

	return 0;
}

int32_t rtk_bt_audio_track_play(void *track_hdl, void *buffer, uint32_t size)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	return AudioTrack_Write(audio_track, buffer, size, true);
}

uint16_t rtk_bt_audio_track_set_hardware_volume(float left_volume, float right_volume)
{
	return AudioControl_SetHardwareVolume(left_volume, right_volume);
}

uint16_t rtk_bt_audio_track_get_volume(float *left_volume, float *right_volume)
{
	if (left_volume == NULL || right_volume == NULL) {
		BT_LOGE("%s: left_volume or right_volume is NULL pointer", __func__);
		return 1;
	}
	return AudioControl_GetHardwareVolume(left_volume, right_volume);
}

uint16_t rtk_bt_audio_track_set_software_volume(void *track_hdl, float left_volume, float right_volume)
{
	struct AudioTrack *audio_track = NULL;
	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}

	return AudioTrack_SetVolume(audio_track, left_volume, right_volume);
}

uint16_t rtk_bt_audio_track_start(void *track_hdl, float left_volume, float right_volume)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	if (AudioTrack_Start(audio_track)) {
		BT_LOGE("audio track start fail!! ");
		return 1;
	}

	if (rtk_bt_audio_track_set_software_volume(audio_track, left_volume, right_volume)) {
		BT_LOGE("audio track set software volume fail!! ");
		return 1;
	}

#if defined(CONFIG_AMEBALITE) && CONFIG_AMEBALITE
#if defined(CONFIG_AUDIO_PASSTHROUGH) && CONFIG_AUDIO_PASSTHROUGH
	{
		char string_value[20];
		sprintf(string_value, "%s%d", "amp_pin=", (int)_PA_12);
		AudioTrack_SetParameters(audio_track, string_value);
	}
#elif defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	AudioControl_SetAmplifierEnPin(_PA_12);
#endif
#endif
	return 0;
}

uint16_t rtk_bt_audio_track_delay_start(void *track_hdl, bool ctrl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_SetParameters(audio_track, ctrl == true ? "delay_start=1" : "delay_start=0");

	return 0;
}

/* this api should be invoked when enable rtk_bt_audio_track_delay_start */
uint16_t rtk_bt_audio_track_hw_start(void)
{
	(*((uint32_t *)AUDIO_SP_CTRL0)) &= ~SP_BIT_TX_DISABLE;
	(*((uint32_t *)AUDIO_SP_CTRL0)) |= SP_BIT_START_TX;

	return 0;
}

void rtk_bt_audio_track_set_mute(bool muted)
{
	//true means mute dac, false means unmute dac.
	if (0 != AudioControl_SetPlaybackMute(muted)) {
		BT_LOGE("audio track mute fail!! ");
	}
}

bool rtk_bt_audio_track_get_muted(void)
{
	return AudioControl_GetPlaybackMute();
}

uint16_t rtk_bt_audio_track_set_sample_rate(void *track_hdl, uint32_t sample_rate)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_SetSampleRate(audio_track, sample_rate);

	return 0;
}

uint32_t rtk_bt_audio_track_get_sample_rate(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 0;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	return AudioTrack_GetSampleRate(audio_track);
}

uint16_t rtk_bt_audio_track_set_channel_count(void *track_hdl, uint32_t channel_count)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	AudioTrack_SetChannelCount(audio_track, channel_count);

	return 0;
}

uint32_t rtk_bt_audio_track_get_channel_count(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 0;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	return AudioTrack_GetChannelCount(audio_track);
}

int32_t rtk_bt_audio_track_get_start_thresholdbytes(void *track_hdl)
{
	struct AudioTrack *audio_track = NULL;

	if (!track_hdl) {
		BT_LOGE("%s: audio track is NULL", __func__);
		return 0;
	} else {
		audio_track = (struct AudioTrack *)track_hdl;
	}
	return AudioTrack_GetStartThresholdBytes(audio_track);
}

long rtk_bt_audio_track_get_buffer_size(void)
{
	return 0;
}
