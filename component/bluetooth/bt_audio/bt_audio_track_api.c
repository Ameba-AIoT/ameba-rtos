/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include "platform_autoconf.h"
#include <ameba_soc.h>
#include <bt_audio_track_api.h>
#include <bt_audio_debug.h>
#include <audio/audio_track.h>
#include <audio/audio_service.h>
#include <audio/audio_control.h>

void *rtk_bt_audio_track_init(uint32_t channels,
							  uint32_t rate,
							  uint32_t bits,
							  uint32_t period_size,
							  uint32_t period_count)
{
	(void)bits;
	(void)period_size;
	(void)period_count;
	struct RTAudioTrack *audio_track = NULL;
	int track_buf_size = 4096;
#if defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	uint32_t track_start_threshold = 0;
#endif
	uint32_t flags = RTAUDIO_OUTPUT_FLAG_NONE;

	RTAudioService_Init();
	audio_track = RTAudioTrack_Create();
	if (!audio_track) {
		printf("%s :new AudioTrack failed", __func__);
		return NULL;
	}
	/* audio_config */
	{
		/* mixer period size is config within ameba_audio_mixer_usrcfg.cpp */
		/* passthrough has no period size concept */
		track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, rate, RTAUDIO_FORMAT_PCM_16_BIT, channels) * 3;
		RTAudioTrackConfig  track_config;
		track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
		track_config.sample_rate = rate;
		track_config.format = RTAUDIO_FORMAT_PCM_16_BIT;
		track_config.channel_count = channels;
		track_config.buffer_bytes = track_buf_size;
		RTAudioTrack_Init(audio_track, &track_config, flags);

#if defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
		/* set audio play start buffer size to match play latency and play fluency */
		RTAudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);
		track_start_threshold = RTAudioTrack_GetStartThresholdBytes(audio_track);
		printf("%s get start threshold:%d\r\n", __func__, (int)track_start_threshold);
#endif
		printf("%s sample_rate %d, channel_cout %d, track_buf_size %d \r\n", __func__,
			   (int)rate,
			   (int)channels,
			   (int)track_buf_size);
	}

	return (void *)audio_track;
}

uint16_t rtk_bt_audio_track_deinit(void *track_hdl)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_Pause(audio_track);
	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_stop(void *track_hdl)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_Stop(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_pause(void *track_hdl)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_Pause(audio_track);
	RTAudioTrack_Flush(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_resume(void *track_hdl)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_Start(audio_track);

	return 0;
}

uint16_t rtk_bt_audio_track_play(void *track_hdl, void *buffer, uint16_t size)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_Write(audio_track, buffer, size, true);

	return 0;
}

uint16_t rtk_bt_audio_track_set_hardware_volume(float left_volume, float right_volume)
{
	return RTAudioControl_SetHardwareVolume(left_volume, right_volume);
}

uint16_t rtk_bt_audio_track_get_volume(float *left_volume, float *right_volume)
{
	if (left_volume == NULL || right_volume == NULL) {
		printf("%s: left_volume or right_volume is NULL pointer", __func__);
		return 1;
	}
	return RTAudioControl_GetHardwareVolume(left_volume, right_volume);
}

uint16_t rtk_bt_audio_track_set_software_volume(void *track_hdl, float left_volume, float right_volume)
{
	struct RTAudioTrack *audio_track = NULL;
	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}

	return RTAudioTrack_SetVolume(audio_track, left_volume, right_volume);
}

uint16_t rtk_bt_audio_track_start(void *track_hdl, float left_volume, float right_volume)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	if (RTAudioTrack_Start(audio_track)) {
		printf("audio track start fail!! ");
		return 1;
	}

	if (rtk_bt_audio_track_set_software_volume(audio_track, left_volume, right_volume)) {
		printf("audio track set software volume fail!! ");
		return 1;
	}

#if defined(CONFIG_AMEBALITE) && CONFIG_AMEBALITE
#if defined(CONFIG_AUDIO_PASSTHROUGH) && CONFIG_AUDIO_PASSTHROUGH
	{
		char string_value[20];
		sprintf(string_value, "%s%d", "amp_pin=", (int)_PA_12);
		RTAudioTrack_SetParameters(audio_track, string_value);
	}
#elif defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	RTAudioControl_SetAmplifierEnPin(_PA_12);
#endif
#endif
	return 0;
}

void rtk_bt_audio_track_set_mute(bool muted)
{
	//true means mute dac, false means unmute dac.
	if (0 != RTAudioControl_SetPlaybackMute(muted)) {
		printf("audio track mute fail!! ");
	}
}

bool rtk_bt_audio_track_get_muted(void)
{
	return RTAudioControl_GetPlaybackMute();
}

uint16_t rtk_bt_audio_track_set_sample_rate(void *track_hdl, uint32_t sample_rate)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_SetSampleRate(audio_track, sample_rate);

	return 0;
}

uint32_t rtk_bt_audio_track_get_sample_rate(void *track_hdl)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 0;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	return RTAudioTrack_GetSampleRate(audio_track);
}

uint16_t rtk_bt_audio_track_set_channel_count(void *track_hdl, uint32_t channel_count)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 1;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	RTAudioTrack_SetChannelCount(audio_track, channel_count);

	return 0;
}

uint32_t rtk_bt_audio_track_get_channel_count(void *track_hdl)
{
	struct RTAudioTrack *audio_track = NULL;

	if (!track_hdl) {
		printf("%s: audio track is NULL", __func__);
		return 0;
	} else {
		audio_track = (struct RTAudioTrack *)track_hdl;
	}
	return RTAudioTrack_GetChannelCount(audio_track);
}

long rtk_bt_audio_track_get_buffer_size(void)
{
	return 0;
}
