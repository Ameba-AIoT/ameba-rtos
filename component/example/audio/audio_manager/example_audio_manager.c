#include "ameba_soc.h"
#include "os_wrapper.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "audio/audio_type.h"
#include "audio/audio_manager.h"
#include "example_audio_manager.h"

#define EXAMPLE_MANAGER_DEBUG(fmt, args...)    printf("=> D/AudioManagerExample:[%s]: " fmt "\n", __func__, ## args)
#define EXAMPLE_MANAGER_ERROR(fmt, args...)    printf("=> E/AudioManagerExample:[%s]: " fmt "\n", __func__, ## args)

#define  RTAUDIO_MANAGER_DEBUG_HEAP_BEGIN() \
	unsigned int heap_start;\
	unsigned int heap_end;\
	unsigned int heap_min_ever_free;\
	EXAMPLE_MANAGER_DEBUG("[Mem] mem debug info init \n");\
	heap_start = rtos_mem_get_free_heap_size()

#define  RTAUDIO_MANAGER_DEBUG_HEAP_END() \
	heap_end = rtos_mem_get_free_heap_size();\
	heap_min_ever_free = rtos_mem_get_minimum_ever_free_heap_size();\
	EXAMPLE_MANAGER_DEBUG("[Mem] start (0x%x), end (0x%x), \n", heap_start, heap_end);\
	EXAMPLE_MANAGER_DEBUG(" diff (%d), peak (%d) \n", heap_start - heap_end, heap_start - heap_min_ever_free)

#define PRESSURE_TEST 0
static uint32_t  g_rate = 44100;
static uint32_t  g_channels = 2;
static uint32_t  g_format = 16;

void example_audio_manager_thread(void *param)
{
	(void) param;
	EXAMPLE_MANAGER_DEBUG("Audio manager demo begin.");

	uint32_t format;
	switch (g_format) {
	case 16:
		format = RTAUDIO_FORMAT_PCM_16_BIT;
		break;
	case 24:
		format = RTAUDIO_FORMAT_PCM_8_24_BIT;
		break;
	case 32:
		format = RTAUDIO_FORMAT_PCM_32_BIT;
		break;
	default:
		EXAMPLE_MANAGER_ERROR("unsupported format:%ld", format);
		return;
	}

#if PRESSURE_TEST
	while (1) {
		RTAUDIO_MANAGER_DEBUG_HEAP_BEGIN();
#endif
		struct RTAudioManager *audio_manager = RTAudioManager_GetInstance();
		if (!audio_manager) {
			EXAMPLE_MANAGER_ERROR("create audio manager fail.");
			goto error_handle;
		}

		struct RTAudioPatchConfig sources[1];
		sources[0].sample_rate = g_rate;
		sources[0].channel_count = g_channels;
		sources[0].format = format;
		sources[0].type = RTAUDIO_PATCH_NODE_DEVICE;
		sources[0].node.device = RTDEVICE_IN_I2S;

		struct RTAudioPatchConfig sinks[1];
		sinks[0].sample_rate = g_rate;
		sinks[0].channel_count = g_channels;
		sinks[0].format = format;
		sinks[0].type = RTAUDIO_PATCH_NODE_DEVICE;
		sinks[0].node.device = RTDEVICE_OUT_SPEAKER;

		int32_t patch_index = RTAudioManager_CreateAudioPatch(audio_manager, 1, sources, 1, sinks);

#if PRESSURE_TEST
		rtos_time_delay_ms(100000);

		RTAudioManager_ReleaseAudioPatch(audio_manager, patch_index);

		rtos_time_delay_ms(1000);

		patch_index = RTAudioManager_CreateAudioPatch(audio_manager, 1, sources, 1, sinks);

		rtos_time_delay_ms(100000);

		RTAudioManager_ReleaseAudioPatch(audio_manager, patch_index);

		RTAudioManager_Destroy();

		RTAUDIO_MANAGER_DEBUG_HEAP_END();
	}
#else
		while (1) {
			EXAMPLE_MANAGER_DEBUG("patch:%ld, LOOP I2S IN -> I2S OUT \n", patch_index);
			rtos_time_delay_ms(10000);
		}
#endif

error_handle:
	rtos_task_delete(NULL);
}

#if USING_CMD
void example_audio_manager(char **argv)
{
	/* parse command line arguments */
	while (*argv) {
		if (strcmp(*argv, "-c") == 0) {
			argv++;
			if (*argv) {
				g_channels = atoi(*argv);
			}
		} else if (strcmp(*argv, "-r") == 0) {
			argv++;
			if (*argv) {
				g_rate = atoi(*argv);
			}
		} else if (strcmp(*argv, "-f") == 0) {
			argv++;
			if (*argv) {
				g_format = atoi(*argv);
			}
		}
		if (*argv) {
			argv++;
		}
	}
	if (rtos_task_create(NULL, ((const char *)"example_audio_manager_thread"), example_audio_manager_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		EXAMPLE_MANAGER_ERROR("error: rtos_task_create(example_audio_manager_thread) failed");
	}
}

u32 example_manager_test(u16 argc, unsigned char **argv)
{
	(void) argc;
	example_audio_manager((char **)argv);
	return TRUE;
}

#else
void example_manager(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_audio_manager_thread"), example_audio_manager_thread, NULL, 2048 * 4, 1) != RTK_SUCCESS) {
		EXAMPLE_MANAGER_ERROR("error: rtos_task_create(example_audio_manager_thread) failed");
	}
}
#endif
