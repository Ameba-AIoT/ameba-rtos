#include <math.h>

#include "ameba_soc.h"
#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_track.h"
#include "audio/audio_service.h"
#include "common/audio_errnos.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"

#define PLAY_SECONDS           200
#define TWO_CHANNEL_FRAMES     96
//For some bad performance system, please set it 0.because this may take some CPU.
#define SINE_GEN_EVERY_TIME    1
#define TEST_EQ                0
#define DUMP_ENABLE            0
#define NEED_CACHE_INVALIDATE  0
#define DUMP_FRAME             192000
#define TEST_DELAY             0
#define TEST_DELAY_GPIO        _PB_8
#define M_PI                   3.14159265358979323846
#define LITTLEFS_RAW           0
//to test ppm between system clock and audio clock, please remember
//to use pll for audio playback in ameba_audio_hw_usrcfg.h.
#define TEST_TIMESTAMP         0

#define EXAMPLE_AUDIO_DEBUG(fmt, args...)    printf("=> D/AudioTrackExample:[%s]: " fmt "\n", __func__, ## args)
#define EXAMPLE_AUDIO_ERROR(fmt, args...)    printf("=> E/AudioTrackExample:[%s]: " fmt "\n", __func__, ## args)

#define  RTAUDIO_TRACK_DEBUG_HEAP_BEGIN() \
	unsigned int heap_start;\
	unsigned int heap_end;\
	unsigned int heap_min_ever_free;\
	printf("=> I/AudioTrackExample:[Mem] mem debug info init \n");\
	heap_start = rtos_mem_get_free_heap_size()

#define  RTAUDIO_TRACK_DEBUG_HEAP_END() \
    heap_end = rtos_mem_get_free_heap_size();\
	heap_min_ever_free = rtos_mem_get_minimum_ever_free_heap_size();\
	printf("=> I/AudioTrackExample:[Mem] start (0x%x), end (0x%x), \n", heap_start, heap_end);\
	printf("=> I/AudioTrackExample: diff (%d), peak (%d) \n", heap_start - heap_end, heap_start - heap_min_ever_free)

static uint32_t  g_track_rate = 16000;
static uint32_t  g_track_channel = 2;
static uint32_t  g_track_format = 16;
static uint32_t  g_write_frames_one_time = 1024;

/* pcm frequency in Hz */
static double    g_freq = 1000;
static uint32_t  g_generate_cnt = 0;

static struct RTAudioTrack *g_audio_track = NULL;

int32_t g_gain = 15;
#if LITTLEFS_RAW
#include <fcntl.h>
static int s_lfs_fd = 0;
static char *s_lfs_name = "lfs://0-20khz-48kfs.raw";
#endif

#if TEST_DELAY
static void set_gpio_state(uint32_t pin, bool state)
{
	GPIO_InitTypeDef gpio_initstruct_temp;
	gpio_initstruct_temp.GPIO_Pin = pin;
	gpio_initstruct_temp.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&gpio_initstruct_temp);

	if (state == true) {
		EXAMPLE_AUDIO_DEBUG("gpio enable");
		GPIO_WriteBit(pin, 1);
	} else {
		EXAMPLE_AUDIO_DEBUG("gpio disable");
		GPIO_WriteBit(pin, 0);
	}
}
#endif

static void generate_sine(int8_t *buffer, uint32_t count, uint32_t rate, uint32_t channels, uint32_t bits, double *_phase)
{
	static double max_phase = 2. * M_PI;
	double phase = *_phase;
	double step = max_phase * g_freq / (double)rate;
	uint8_t *samples[channels];
	int32_t steps[channels];
	uint32_t chn;
	int32_t format_bits = bits;
	uint32_t maxval = (1 << (format_bits - 1)) - 1;
	/* bytes per sample */
	int32_t bps = format_bits / 8;
	int32_t to_unsigned = 0;

	for (chn = 0; chn < channels; chn++) {
		steps[chn] = bps * channels;
		samples[chn] = (uint8_t *)(buffer + chn * bps);
	}

	while (count-- > 0) {
		int32_t res, i;
		res = sin(phase) * maxval;
		if (to_unsigned) {
			res ^= 1U << (format_bits - 1);
		}
		for (chn = 0; chn < channels; chn++) {
			{
				for (i = 0; i < bps; i++) {
					*(samples[chn] + i) = (res >>  i * 8) & 0xff;
				}
			}
			samples[chn] += steps[chn];
		}
		phase += step;
		if (phase >= max_phase) {
			phase -= max_phase;
		}
	}
	*_phase = phase;
}

void test_eq(struct RTAudioEqualizer **audio_equalizer)
{

	int16_t band_level = 0;
	int32_t center_freq = 0;
	int32_t qfactor = 0;
	int16_t band_index = 0;
	int32_t *freq_range = NULL;

	*audio_equalizer = RTAudioEqualizer_Create();
	RTAudioEqualizer_Init(*audio_equalizer, 0, 0);
	int16_t bands = RTAudioEqualizer_GetNumberOfBands(*audio_equalizer);
	EXAMPLE_AUDIO_DEBUG("total bands:%d", bands);

	int16_t *range = RTAudioEqualizer_GetBandLevelRange(*audio_equalizer);
	EXAMPLE_AUDIO_DEBUG("band range:(%d, %d)", *range, *(range + 1));
	free(range);
	range = NULL;

	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 0, 40);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 1, 90);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 2, 180);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 3, 380);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 4, 760);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 5, 1000);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 6, 3020);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 7, 6010);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 8, 12010);
	RTAudioEqualizer_SetCenterFreq(*audio_equalizer, 9, 18010);

	/*band level range from -1500->1500(-15db->15db)*/
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 0, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 1, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 2, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 3, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 4, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 5, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 6, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 7, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 8, g_gain);
	RTAudioEqualizer_SetBandLevel(*audio_equalizer, 9, g_gain);

	RTAudioEqualizer_SetQfactor(*audio_equalizer, 0, 80);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 1, 96);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 2, 96);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 3, 70);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 4, 96);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 5, 90);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 6, 97);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 7, 66);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 8, 280);
	RTAudioEqualizer_SetQfactor(*audio_equalizer, 9, 99);

	RTAudioEqualizer_SetEnabled(*audio_equalizer, true);

	for (; band_index < bands; band_index++) {
		band_level = RTAudioEqualizer_GetBandLevel(*audio_equalizer, band_index);
		EXAMPLE_AUDIO_DEBUG("band %d level:%d", band_index, band_level);

		center_freq = RTAudioEqualizer_GetCenterFreq(*audio_equalizer, band_index);
		EXAMPLE_AUDIO_DEBUG("band:%d, center freq:%ld", band_index, center_freq);

		qfactor = RTAudioEqualizer_GetQfactor(*audio_equalizer, band_index);
		EXAMPLE_AUDIO_DEBUG("band:%d, qfactor:%ld", band_index, qfactor);
		free(freq_range);
		freq_range = NULL;
	}

}

void end_test_eq(struct RTAudioEqualizer **audio_equalizer)
{
	RTAudioEqualizer_SetEnabled(*audio_equalizer, false);
	RTAudioEqualizer_Destroy(*audio_equalizer);
}

void play_sample(uint32_t channels, uint32_t rate, uint32_t bits, uint32_t period_size)
{
	struct RTAudioTrack *audio_track;

	uint64_t frames_written = 0;
	uint32_t frame_size = channels * bits / 8;
	int32_t track_buf_size = 4096;
	uint32_t format;
	uint32_t track_start_threshold = 0;
	uint64_t play_frame_size = (uint64_t)rate * (uint64_t)PLAY_SECONDS;

	RTAudioTimestamp tstamp;
	uint32_t frames_played = 0;
	uint64_t frames_played_at_us = 0;
	int64_t now_us = 0;

	uint32_t sine_frames_count = period_size;
	int8_t sine_buf[sine_frames_count * frame_size];
	double phase = 0;

#if DUMP_ENABLE
	int8_t *dump_buffer;
#endif

#if TEST_DELAY
	bool water_level_us_print = true;
	uint32_t start_us = 0;
#endif

	EXAMPLE_AUDIO_DEBUG("play sample channels:%lu, rate:%lu,bits=%lu,period_size=%lu", channels, rate, bits, period_size);

	switch (bits) {
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
		break;
	}

#if DUMP_ENABLE
	dump_buffer = (int8_t *) malloc(DUMP_FRAME * frame_size);
	if (!dump_buffer) {
		free(dump_buffer);
		EXAMPLE_AUDIO_DEBUG("failed to malloc dump_buffer \n");
		return;
	}
	EXAMPLE_AUDIO_DEBUG("audio track dump_buffer:%p", dump_buffer);
#endif

	audio_track = RTAudioTrack_Create();
	if (!audio_track) {
		EXAMPLE_AUDIO_ERROR("error: new RTAudioTrack failed");
		return;
	}

	track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, rate, format, channels) * 4;
	RTAudioTrackConfig  track_config;
	track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = rate;
	track_config.format = format;
	track_config.channel_count = channels;
	track_config.buffer_bytes = track_buf_size;
	RTAudioTrack_Init(audio_track, &track_config, RTAUDIO_OUTPUT_FLAG_NONE);

	EXAMPLE_AUDIO_DEBUG("track buf size:%ld", track_buf_size);

#if TEST_EQ
	struct RTAudioEqualizer *audio_equalizer;
	test_eq(&audio_equalizer);
#endif

	/*set dac hardware volume, if you want to test this demo's sine from oscilloscope, better set 0.956*/
#if TEST_DELAY
	RTAudioControl_SetHardwareVolume(0.956, 0.956);
#endif
	/*for mixer version, this mean sw volume, for passthrough version, sw volume is not supported*/
	RTAudioTrack_SetVolume(audio_track, 1.0, 1.0);

	RTAudioTrack_SetStartThresholdBytes(audio_track, track_buf_size);
	track_start_threshold = RTAudioTrack_GetStartThresholdBytes(audio_track);
	EXAMPLE_AUDIO_DEBUG("get start threshold:%lu", track_start_threshold);

	ssize_t size = sine_frames_count * channels  * bits / 8;

	if (RTAudioTrack_Start(audio_track) != AUDIO_OK) {
		EXAMPLE_AUDIO_ERROR("error: audio track start fail");
		return;
	}

	g_audio_track = audio_track;

	while (1) {

#if TEST_DELAY
		if (frames_written == 0) {
			set_gpio_state(TEST_DELAY_GPIO, 0);
			set_gpio_state(TEST_DELAY_GPIO, 1);
			start_us = DTimestamp_Get();
		}
#endif


#if SINE_GEN_EVERY_TIME
		generate_sine(sine_buf, sine_frames_count, rate, channels, bits, &phase);
		if (g_generate_cnt >= 1000 && g_generate_cnt % 1000 == 0) {
			g_freq -= 1000;
		} else if (g_generate_cnt >= 500 && g_generate_cnt % 500 == 0) {
			g_freq += 1000;
		}
#else
		if (g_generate_cnt < 1) {
			generate_sine(sine_buf, sine_frames_count, rate, channels, bits, &phase);
		}
#endif
		g_generate_cnt ++;

#if TEST_DELAY
		if (frames_written + sine_frames_count >= track_start_threshold / frame_size && water_level_us_print) {
			EXAMPLE_AUDIO_DEBUG("water level achieved, takes %lu us", DTimestamp_Get() - start_us);
			water_level_us_print = false;
		}
#endif

#if LITTLEFS_RAW
		if (s_lfs_fd > 0) {
			int32_t bytes_read = fread(sine_buf, 1, size, (FILE *)s_lfs_fd);
			EXAMPLE_AUDIO_DEBUG("fread from file(0x%x). size:%d, bytes_read:%ld", s_lfs_fd, size, bytes_read);
		}
#endif
		RTAudioTrack_Write(audio_track, (u8 *)sine_buf, size, true);

#if DUMP_ENABLE
		if (frames_written  * frame_size + size <= DUMP_FRAME * frame_size) {
			memcpy(dump_buffer + frames_written  * frame_size, sine_buf, size);
#if NEED_CACHE_INVALIDATE
			DCache_Invalidate((u32)(dump_buffer + frames_written  * frame_size), size);
#endif
		}
#endif

		now_us = rtos_time_get_current_system_time_ms() * 1000;
		if (RTAudioTrack_GetTimestamp(audio_track, &tstamp) == AUDIO_OK) {
			EXAMPLE_AUDIO_DEBUG("timestamp position:%lld, sec:%lld, nsec:%ld", tstamp.position, tstamp.time.tv_sec, tstamp.time.tv_nsec);
			frames_played = tstamp.position;
			frames_played_at_us = tstamp.time.tv_sec * 1000000LL + tstamp.time.tv_nsec / 1000;
		}

		frames_written += (uint64_t)(size / frame_size);
		if (frames_written >= play_frame_size) {
			EXAMPLE_AUDIO_DEBUG("frames_written:%llu, play_frame_size:%llu", frames_written, play_frame_size);
			break;
		}
	}

	int64_t duration_us = frames_played * 1000000LL / rate + now_us - frames_played_at_us;
	int64_t frames_written_us = frames_written * 1000000LL / rate;
	uint32_t wait_ms = (frames_written_us - duration_us) / 1000;

	if (wait_ms > 100) {
		//ignore too big latency.
		wait_ms = 100;
	}

	rtos_time_delay_ms(wait_ms);

	RTAudioTrack_Pause(audio_track);
	RTAudioTrack_Flush(audio_track);
	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

#if DUMP_ENABLE
	if (dump_buffer) {
		free(dump_buffer);
	}
#endif

#if TEST_EQ
	end_test_eq(&audio_equalizer);
	audio_equalizer = NULL;
#endif

	audio_track = NULL;
}

void example_audio_track_thread(void *param)
{
	EXAMPLE_AUDIO_DEBUG("Audio track demo begin");
	(void) param;

	RTAUDIO_TRACK_DEBUG_HEAP_BEGIN();
	g_generate_cnt = 0;
#if LITTLEFS_RAW
	s_lfs_fd = (int)fopen(s_lfs_name, "r");
	if (s_lfs_fd <= 0) {
		EXAMPLE_AUDIO_ERROR("fopen lfs fail.");
	}
	EXAMPLE_AUDIO_ERROR("fopen lfs 0x%x.", s_lfs_fd);

	fseek((FILE *)s_lfs_fd, 0L, SEEK_END);
	int64_t length = ftell((FILE *)s_lfs_fd);
	EXAMPLE_AUDIO_DEBUG("file length:%lld", length);
	fseek((FILE *)s_lfs_fd, 0, SEEK_SET);

#endif

	//user should set sdk/component/soc/**/usrcfg/include/ameba_audio_hw_usrcfg.h's AUDIO_HW_AMPLIFIER_PIN to make sure amp is enabled.
	RTAudioService_Init();

#if TEST_DELAY
	rtos_time_delay_ms(5 * RTOS_TICK_RATE_HZ);
#endif

	play_sample(g_track_channel, g_track_rate, g_track_format, g_write_frames_one_time);
	rtos_time_delay_ms(2 * RTOS_TICK_RATE_HZ);

#if LITTLEFS_RAW
	if (s_lfs_fd > 0) {
		EXAMPLE_AUDIO_DEBUG("close file.");
		fclose((FILE *)s_lfs_fd);
		s_lfs_fd = NULL;
	}
#endif
	RTAUDIO_TRACK_DEBUG_HEAP_END();

	rtos_task_delete(NULL);
}

#if TEST_TIMESTAMP
int64_t last_frames_played_ns = 0;
int64_t last_frames_played_at_ns = 0;
int64_t last_phase_played_ns = 0;
int64_t last_phase_played_at_ns = 0;
int32_t ppm_test_cnt = 0;

void example_audio_counter_time(void *param)
{
	rtos_time_delay_ms(2 * RTOS_TICK_RATE_HZ);
	EXAMPLE_AUDIO_DEBUG("Audio track time begin");
	(void) param;

	RTAudioTimestamp tstamp;
	int32_t frames_played = 0;
	int64_t frames_played_ns = 0;
	int64_t frames_played_at_ns = 0;

	int64_t phase_played_ns = 0;
	int64_t phase_played_at_ns = 0;

	while (1) {
		if (!ppm_test_cnt) {
			rtos_time_delay_ms(1 * RTOS_TICK_RATE_HZ);
		} else {
			rtos_time_delay_ms(10 * RTOS_TICK_RATE_HZ);
		}

		if (RTAudioTrack_GetTimestamp(g_audio_track, &tstamp) == AUDIO_OK) {
			frames_played = tstamp.position;
			frames_played_at_ns = tstamp.time.tv_sec * 1000000000LL + tstamp.time.tv_nsec;
			frames_played_ns = (int64_t)((double)frames_played / (double)g_track_rate * (double)1000000000);
		}

		if (RTAudioTrack_GetPresentTime(g_audio_track, &phase_played_at_ns, &phase_played_ns) != AUDIO_OK) {
			EXAMPLE_AUDIO_ERROR("get present time fail");
		}

		if (ppm_test_cnt) {
			EXAMPLE_AUDIO_DEBUG("ppm:%.16f frames_played:%ld, frames_played_ns:%lld, frames_played_at_ns:%lld, last_frames_played_ns:%lld, last_frames_played_at_ns:%lld",
								(double)(frames_played_ns - last_frames_played_ns  - (frames_played_at_ns - last_frames_played_at_ns)) / (double)(
									frames_played_at_ns - last_frames_played_at_ns) * (double)1000000,
								frames_played, frames_played_ns, frames_played_at_ns, last_frames_played_ns, last_frames_played_at_ns);

			EXAMPLE_AUDIO_DEBUG("phase ppm:%.16f phase_played_ns:%lld, phase_played_at_ns:%lld, last_phase_played_ns:%lld, last_phase_played_at_ns:%lld",
								(double)(phase_played_ns - last_phase_played_ns  - (phase_played_at_ns - last_phase_played_at_ns)) / (double)(phase_played_at_ns - last_phase_played_at_ns) *
								(double)1000000,
								phase_played_ns, phase_played_at_ns, last_phase_played_ns, last_phase_played_at_ns);
		}

		last_frames_played_ns = frames_played_ns;
		last_frames_played_at_ns = frames_played_at_ns;

		last_phase_played_ns = phase_played_ns;
		last_phase_played_at_ns = phase_played_at_ns;

		ppm_test_cnt ++;
	}

	rtos_task_delete(NULL);
}
#endif

void example_audio_track(char **argv)
{
	/* parse command line arguments */
	while (*argv) {
		if (strcmp(*argv, "-c") == 0) {
			argv++;
			if (*argv) {
				g_track_channel = atoi(*argv);
			}
		} else if (strcmp(*argv, "-r") == 0) {
			argv++;
			if (*argv) {
				g_track_rate = atoi(*argv);
			}
		} else if (strcmp(*argv, "-b") == 0) {
			argv++;
			if (*argv) {
				g_write_frames_one_time = atoi(*argv);
			}
		} else if (strcmp(*argv, "-f") == 0) {
			argv++;
			if (*argv) {
				g_track_format = atoi(*argv);
			}
		} else if (strcmp(*argv, "-s") == 0) {
			argv++;
			if (*argv) {
				g_freq = atoi(*argv);
			}
		} else if (strcmp(*argv, "-g") == 0) {
			argv++;
			if (*argv) {
				g_gain = atoi(*argv);
			}
		}
		if (*argv) {
			argv++;
		}
	}

	if (rtos_task_create(NULL, ((const char *)"example_audio_track_thread"), example_audio_track_thread, NULL, 8192 * 4, 1) != SUCCESS) {
		EXAMPLE_AUDIO_ERROR("error: rtos_task_create(example_audio_track_thread) failed");
	}

#if TEST_TIMESTAMP
	if (rtos_task_create(NULL, ((const char *)"example_audio_counter_time"), example_audio_counter_time, NULL, 8192 * 4, 1) != SUCCESS) {
		EXAMPLE_AUDIO_ERROR("error: rtos_task_create(example_audio_counter_time) failed");
	}
#endif
}

u32 example_track_test(u16 argc, unsigned char **argv)

{
	(void) argc;
	example_audio_track((char **)argv);
	return _TRUE;
}