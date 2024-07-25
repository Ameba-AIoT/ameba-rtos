/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "RTRecordTest"
//#define LOG_NDEBUG 0

#include "ameba_soc.h"
#include "audio/audio_control.h"
#include "audio/audio_record.h"
#include "audio/audio_service.h"
#include "audio/audio_track.h"
#include "common/audio_errnos.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"

#define DUMP_FRAME            96000
#define RECORD_TIME_SECONDS   10
#define REF_PLAY_SECONDS      3
#define MAX_CHANNEL_COUNT     8
/*when dump, d2 needs invalidate, lite doesn't need invalidate*/
#define NEED_CACHE_INVALIDATE 1
#define DUMP_BUFFER           0
//to test ppm between system clock and audio clock, please remember
//to use pll for audio record in ameba_audio_hw_usrcfg.h.
#define TEST_TIMESTAMP        0
//if running in mixer architecture, please set 1 here.
//if running in passthrough architecture, please set 0 here.
#define TEST_MIXER_ARCH       1

#define EXAMPLE_AUDIO_DEBUG(fmt, args...)    printf("=> D/AudioRecordExample:[%s]: " fmt "\n", __func__, ## args)
#define EXAMPLE_AUDIO_ERROR(fmt, args...)    printf("=> E/AudioRecordExample:[%s]: " fmt "\n", __func__, ## args)

#define  RTAUDIO_RECORD_DEBUG_HEAP_BEGIN() \
	unsigned int heap_start;\
	unsigned int heap_end;\
	unsigned int heap_min_ever_free;\
	EXAMPLE_AUDIO_DEBUG("[Mem] mem debug info init \n");\
	heap_start = rtos_mem_get_free_heap_size()

#define  RTAUDIO_RECORD_DEBUG_HEAP_END() \
    heap_end = rtos_mem_get_free_heap_size();\
	heap_min_ever_free = rtos_mem_get_minimum_ever_free_heap_size();\
	EXAMPLE_AUDIO_DEBUG("[Mem] start (0x%x), end (0x%x), \n", heap_start, heap_end);\
	EXAMPLE_AUDIO_DEBUG(" diff (%d), peak (%d) \n", heap_start - heap_end, heap_start - heap_min_ever_free)

static struct RTAudioRecord *g_audio_record = NULL;
static unsigned int  g_only_record = 0;
static unsigned int  g_noirq_test = 0;
static unsigned int  g_test_ref = 0;
static unsigned int  g_pressure_test = 0;
static unsigned int  g_record_rate = 16000;
static unsigned int  g_record_channel = 2;
static unsigned int  g_record_mode = 0;
static unsigned int  g_record_format = 16;
static unsigned int  g_record_bytes_one_time = 8192;
static unsigned int  g_record_mic_category = RTDEVICE_IN_MIC;
static unsigned int  g_record_channel_src[MAX_CHANNEL_COUNT] = {RTAUDIO_AMIC1};
static unsigned int  Record_Sample(void);
static          void Play_Sample(unsigned int channels, unsigned int rate);

static u16 sine_48000[96] = {
	0X0000,	0X0000, 0X10B5, 0X10B5, 0X2120, 0X2120, 0X30FB, 0X30FB, 0X3FFF, 0X3FFF, 0X4DEB, 0X4DEB,
	0X5A82,	0X5A82, 0X658C, 0X658C, 0X6ED9, 0X6ED9, 0X7641, 0X7641, 0X7BA3,	0X7BA3, 0X7EE7, 0X7EE7,
	0X7FFF, 0X7FFF, 0X7EE7, 0X7EE7, 0X7BA3, 0X7BA3, 0X7641, 0X7641,	0X6ED9,	0X6ED9, 0X658C, 0X658C,
	0X5A82,	0X5A82, 0X4DEB, 0X4DEB, 0X3FFF, 0X3FFF, 0X30FB, 0X30FB, 0X2120, 0X2120, 0X10B5, 0X10B5,
	0X0000,	0X0000, 0XEF4A, 0XEF4A, 0XDEDF, 0XDEDF, 0XCF04, 0XCF04, 0XC000, 0XC000,	0XB214,	0XB214,
	0XA57D,	0XA57D, 0X9A73, 0X9A73,	0X9126,	0X9126, 0X89BE, 0X89BE,	0X845C,	0X845C, 0X8118, 0X8118,
	0X8000,	0X8000,	0X8118,	0X8118,	0X845C,	0X845C, 0X89BE, 0X89BE, 0X9126, 0X9126, 0X9A73, 0X9A73,
	0XA57D, 0XA57D, 0XB214,	0XB214,	0XC000, 0XC000,	0XCF04, 0XCF04, 0XDEDF, 0XDEDF, 0XEF4A,	0XEF4A
};

static int GetFormatForBits(void)
{
	int format;
	switch (g_record_format) {
	case 16:
		format = RTAUDIO_FORMAT_PCM_16_BIT;
		break;
	case 24:
		format = RTAUDIO_FORMAT_PCM_24_BIT;
		break;
	case 32:
		format = RTAUDIO_FORMAT_PCM_32_BIT;
		break;
	default:
		EXAMPLE_AUDIO_ERROR("format:%d not supportd \n", g_record_format);
		return RTAUDIO_FORMAT_INVALID;
	}

	return format;
}

static unsigned int Record_Sample()
{
	char *buffer;
	char *dump_buffer;
	unsigned int size;
	ssize_t size_read;
#if DUMP_BUFFER
	int dumped_size = 0;
#endif
	uint32_t flags = RTAUDIO_OUTPUT_FLAG_NONE;
	uint32_t record_flags = RTAUDIO_INPUT_FLAG_NONE;
	int64_t bytes_read = 0;
	unsigned int frames = 0;
	int format = GetFormatForBits();
	int64_t record_size = (int64_t)g_record_rate * (int64_t)RECORD_TIME_SECONDS * (int64_t)g_record_channel * (int64_t)g_record_format / (int64_t)8;
	int track_buf_size = 4096;

	if (format == (int)RTAUDIO_FORMAT_INVALID) {
		EXAMPLE_AUDIO_ERROR("invalid record format bits:%u \n", g_record_format);
		return 0;
	}

	if (g_noirq_test) {
		flags |= RTAUDIO_OUTPUT_FLAG_NOIRQ;
		record_flags |= RTAUDIO_INPUT_FLAG_NOIRQ;
	}

	struct RTAudioRecord *audio_record;
	audio_record = RTAudioRecord_Create();
	if (!audio_record) {
		EXAMPLE_AUDIO_ERROR("record create failed \n");
		return 0;
	}

	RTAudioRecordConfig record_config;
	record_config.sample_rate = g_record_rate;
	record_config.format = format;
	record_config.channel_count = g_record_channel;
	record_config.device = g_record_mic_category;
	record_config.buffer_bytes = 0; //0 means using default period bytes
	if (g_noirq_test) {
		record_config.buffer_bytes = g_record_bytes_one_time;
	}
	RTAudioRecord_Init(audio_record, &record_config, flags);

	struct RTAudioTrack *audio_track;
	if (!g_only_record) {
		RTAudioService_Init();
		audio_track = RTAudioTrack_Create();
		if (!audio_track) {
			EXAMPLE_AUDIO_ERROR("new RTAudioTrack failed, destroy record \n");
			RTAudioRecord_Destroy(audio_record);
			return 0;
		}

		track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, g_record_rate, format, g_record_channel) * 4;

		RTAudioTrackConfig  track_config;
		track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
		track_config.sample_rate = g_record_rate;
		track_config.format = format;
		track_config.channel_count = g_record_channel;
		if (g_noirq_test) {
			track_config.buffer_bytes = g_record_bytes_one_time;
		} else {
			track_config.buffer_bytes = track_buf_size;
		}
		RTAudioTrack_Init(audio_track, &track_config, flags);
	}

	RTAudioRecord_Start(audio_record);
	if (!g_only_record) {
		RTAudioTrack_Start(audio_track);
	}
	g_audio_record = audio_record;

	for (unsigned int i = 0; i < MAX_CHANNEL_COUNT; i++) {
		RTAudioControl_SetChannelMicCategory(i, g_record_channel_src[i]);
	}

	RTAudioControl_SetCaptureVolume(4, 0x2f);
	RTAudioControl_SetMicBstGain(RTAUDIO_AMIC2, MICBST_GAIN_30DB);

	switch (g_record_mode) {
	case 0:
		RTAudioRecord_SetParameters(audio_record, "cap_mode=no_afe_pure_data");
		break;
	case 1:
		RTAudioRecord_SetParameters(audio_record, "cap_mode=no_afe_all_data");
		break;
	default:
		break;
	}

	if (g_noirq_test) {
		RTAudioControl_AdjustPLLClock(g_record_rate, 0, RTAUDIO_PLL_AUTO);
	}

	size = g_record_bytes_one_time;

	buffer = (char *) malloc(size);
	if (!buffer) {
		free(buffer);
		EXAMPLE_AUDIO_ERROR("failed to malloc buffer \n");
		return 0;
	}

#if DUMP_BUFFER
	dump_buffer = (char *) malloc(DUMP_FRAME * g_record_channel * g_record_format / 8);
	if (!dump_buffer) {
		free(dump_buffer);
		EXAMPLE_AUDIO_ERROR("failed to malloc dump_buffer \n");
		return 0;
	}
#endif

	EXAMPLE_AUDIO_DEBUG("Capturing sample: %u ch, %u hz, record bytes one time:%d, dump_buffer:%p \n", g_record_channel, g_record_rate,
						g_record_bytes_one_time, dump_buffer);
	do {
		size_read = RTAudioRecord_Read(audio_record, buffer, size, true);
		if ((unsigned int)size_read != size) {
			EXAMPLE_AUDIO_DEBUG("opps size wanted:%d, size actually read:%d \n", size, size_read);
		}

#if DUMP_BUFFER
		if (dumped_size + size <= DUMP_FRAME * g_record_channel * g_record_format / 8) {
			memcpy(dump_buffer + dumped_size, buffer, size);
#if NEED_CACHE_INVALIDATE
			DCache_Invalidate((u32)(dump_buffer + dumped_size), size);
#endif
			dumped_size += size;
		}
#endif
		//drop first 100ms data of record, and instead send 0, because record need some time to be stable, it's normal.
		if (!g_only_record && bytes_read >= 100 * g_record_rate * g_record_channel * g_record_format / 8 / 1000) {
			RTAudioTrack_Write(audio_track, buffer, size, true);
		} else if (!g_only_record && !g_noirq_test) {
#if !TEST_MIXER_ARCH
			memset(buffer, 0, size);
			RTAudioTrack_Write(audio_track, buffer, size, true);
			//To give another 0 buf at beginning, in case of xrun. For real case, please add ringbuffer between record and track.
			RTAudioTrack_Write(audio_track, buffer, size, true);
#endif
		}

		bytes_read += size;

		if (!g_pressure_test) {
			if (bytes_read >= record_size) {
				break;
			}
		}

	} while (1);

	frames = bytes_read / (g_record_channel * g_record_format / 8);

	free(buffer);
	buffer = NULL;

#if DUMP_BUFFER
	free(dump_buffer);
	dump_buffer = NULL;
#endif

	RTAudioRecord_Stop(audio_record);
	RTAudioRecord_Destroy(audio_record);
	if (!g_only_record) {
		RTAudioTrack_Stop(audio_track);
		RTAudioTrack_Destroy(audio_track);
	}

	return frames;
}

static void Play_Sample(unsigned int channels, unsigned int rate)
{
	struct RTAudioTrack *audio_track;
	int track_buf_size = 4096;
	unsigned int frames_played = 0;
	unsigned int play_frame_size = rate * REF_PLAY_SECONDS;
	uint32_t flags = RTAUDIO_OUTPUT_FLAG_NONE;

	RTAudioService_Init();
	audio_track = RTAudioTrack_Create();
	if (!audio_track) {
		EXAMPLE_AUDIO_ERROR("new RTAudioTrack failed");
		return;
	}

	track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, rate, RTAUDIO_FORMAT_PCM_16_BIT, channels) * 4;
	RTAudioTrackConfig  track_config;
	track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = rate;
	track_config.format = RTAUDIO_FORMAT_PCM_16_BIT;
	track_config.channel_count = channels;
	track_config.buffer_bytes = track_buf_size;
	RTAudioTrack_Init(audio_track, &track_config, flags);

	RTAudioTrack_Start(audio_track);

	ssize_t size = 96 * 2;
	while (1) {

		RTAudioTrack_Write(audio_track, (u8 *)sine_48000, size, true);
		frames_played += size / 4;

		if (frames_played >= play_frame_size) {
			break;
		}
	}

	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

}

static void RecordTask(void *param)
{
	unsigned int frames;
	(void) param;

	RTAUDIO_RECORD_DEBUG_HEAP_BEGIN();
	frames = Record_Sample();
	rtos_time_delay_ms(2 * RTOS_TICK_RATE_HZ);
	RTAUDIO_RECORD_DEBUG_HEAP_END();

	printf("Recorded %u frames \n", frames);
	rtos_task_delete(NULL);
}

static void PlayTask(void *Data)
{
	(void) Data;
	unsigned int channels = 2;
	unsigned int rate = 48000;

	Play_Sample(channels, rate);

	rtos_task_delete(NULL);
}

#if TEST_TIMESTAMP
int64_t last_frames_captured_ns = 0;
int64_t last_frames_captured_at_ns = 0;
int64_t last_phase_captured_ns = 0;
int64_t last_phase_captured_at_ns = 0;
int32_t ppm_test_cnt = 0;

void example_audio_counter_time(void *param)
{
	rtos_time_delay_ms(2 * RTOS_TICK_RATE_HZ);
	EXAMPLE_AUDIO_DEBUG("Audio record time begin");
	(void) param;

	RTAudioTimestamp tstamp;
	int32_t frames_captured = 0;
	int64_t frames_captured_ns = 0;
	int64_t frames_captured_at_ns = 0;

	int64_t phase_captured_ns = 0;
	int64_t phase_captured_at_ns = 0;

	while (1) {
		if (!ppm_test_cnt) {
			rtos_time_delay_ms(1 * RTOS_TICK_RATE_HZ);
		} else {
			rtos_time_delay_ms(10 * RTOS_TICK_RATE_HZ);
		}

		if (RTAudioRecord_GetTimestamp(g_audio_record, &tstamp) == AUDIO_OK) {
			frames_captured = tstamp.position;
			frames_captured_at_ns = tstamp.time.tv_sec * 1000000000LL + tstamp.time.tv_nsec;
			frames_captured_ns = (int64_t)((double)frames_captured / (double)g_record_rate * (double)1000000000);
		}

		if (RTAudioRecord_GetPresentTime(g_audio_record, &phase_captured_at_ns, &phase_captured_ns) != AUDIO_OK) {
			EXAMPLE_AUDIO_ERROR("get present time fail");
		}

		if (ppm_test_cnt) {
			EXAMPLE_AUDIO_DEBUG("ppm:%.16f frames_captured:%ld, frames_captured_ns:%lld, frames_captured_at_ns:%lld, last_frames_captured_ns:%lld, last_frames_captured_at_ns:%lld",
								(double)(frames_captured_ns - last_frames_captured_ns  - (frames_captured_at_ns - last_frames_captured_at_ns)) / (double)(
									frames_captured_at_ns - last_frames_captured_at_ns) * (double)1000000,
								frames_captured, frames_captured_ns, frames_captured_at_ns, last_frames_captured_ns, last_frames_captured_at_ns);

			EXAMPLE_AUDIO_DEBUG("phase ppm:%.16f phase_captured_ns:%lld, phase_captured_at_ns:%lld, last_phase_captured_ns:%lld, last_phase_captured_at_ns:%lld",
								(double)(phase_captured_ns - last_phase_captured_ns  - (phase_captured_at_ns - last_phase_captured_at_ns)) / (double)(
									phase_captured_at_ns - last_phase_captured_at_ns) * (double)1000000,
								phase_captured_ns, phase_captured_at_ns, last_phase_captured_ns, last_phase_captured_at_ns);
		}

		last_frames_captured_ns = frames_captured_ns;
		last_frames_captured_at_ns = frames_captured_at_ns;

		last_phase_captured_ns = phase_captured_ns;
		last_phase_captured_at_ns = phase_captured_at_ns;

		ppm_test_cnt ++;
	}

	rtos_task_delete(NULL);
}
#endif

void RTAudioRecordTestApp(char **argv)
{
	g_record_channel_src[0] = RTAUDIO_AMIC1;
	g_record_channel_src[1] = RTAUDIO_AMIC2;
	g_record_channel_src[2] = RTAUDIO_AMIC3;

	/* parse command line arguments */
	while (*argv) {
		if (strcmp(*argv, "-c") == 0) {
			argv++;
			if (*argv) {
				g_record_channel = atoi(*argv);
			}
		} else if (strcmp(*argv, "-r") == 0) {
			argv++;
			if (*argv) {
				g_record_rate = atoi(*argv);
			}
		} else if (strcmp(*argv, "-b") == 0) {
			argv++;
			if (*argv) {
				g_record_bytes_one_time = atoi(*argv);
			}
		} else if (strcmp(*argv, "-m") == 0) {
			argv++;
			if (*argv) {
				g_record_mode = atoi(*argv);
			}
		} else if (strcmp(*argv, "-f") == 0) {
			argv++;
			if (*argv) {
				g_record_format = atoi(*argv);
			}
		} else if (strcmp(*argv, "-or") == 0) {
			argv++;
			if (*argv) {
				g_only_record = atoi(*argv);
			}
		} else if (strcmp(*argv, "-noirq") == 0) {
			argv++;
			if (*argv) {
				g_noirq_test = atoi(*argv);
			}
		} else if (strcmp(*argv, "-ref") == 0) {
			argv++;
			if (*argv) {
				g_test_ref = atoi(*argv);
			}
		} else if (strcmp(*argv, "-pres") == 0) {
			argv++;
			if (*argv) {
				g_pressure_test = atoi(*argv);
			}
		} else if (strcmp(*argv, "-c0s") == 0) {
			argv++;
			if (*argv) {
				g_record_channel_src[0] = atoi(*argv);
			}
		} else if (strcmp(*argv, "-c1s") == 0) {
			argv++;
			if (*argv) {
				g_record_channel_src[1] = atoi(*argv);
			}
		} else if (strcmp(*argv, "-c2s") == 0) {
			argv++;
			if (*argv) {
				g_record_channel_src[2] = atoi(*argv);
			}
		} else if (strcmp(*argv, "-c3s") == 0) {
			argv++;
			if (*argv) {
				g_record_channel_src[3] = atoi(*argv);
			}
		} else if (strcmp(*argv, "-d") == 0) {
			argv++;
			if (*argv) {
				if (atoi(*argv) == 1) {
					g_record_mic_category = RTDEVICE_IN_DMIC_REF_AMIC;
				} else if (atoi(*argv) == 2) {
					g_record_mic_category = RTDEVICE_IN_I2S;
				} else {
					g_record_mic_category = RTDEVICE_IN_MIC;
				}
			}
		}
		if (*argv) {
			argv++;
		}
	}

	if (SUCCESS != rtos_task_create(NULL, ((const char *)"RecordTask"), RecordTask, NULL, 4096 * 4, 5)) {
		EXAMPLE_AUDIO_ERROR("%s rtos_task_create(RecordTask) failed \n", __FUNCTION__);
	}

	if (g_test_ref) {
		rtos_time_delay_ms(1000);//for test noise
		if (SUCCESS != rtos_task_create(NULL, (const char *const)"PlayTask", PlayTask, NULL, 4096 * 4, 5)) {
			EXAMPLE_AUDIO_ERROR("create PlayTask error \n");
		}
	}

#if TEST_TIMESTAMP
	if (rtos_task_create(NULL, ((const char *)"example_audio_counter_time"), example_audio_counter_time, NULL, 8192 * 4, 1) != SUCCESS) {
		EXAMPLE_AUDIO_ERROR("error: rtos_task_create(example_audio_counter_time) failed");
	}
#endif

	return;
}

u32 example_record_test(u16 argc, unsigned char **argv)

{
	(void) argc;
	RTAudioRecordTestApp((char **)argv);
	return _TRUE;
}
