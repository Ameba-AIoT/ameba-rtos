/*
 * This example demonstrates how to use the aivoice api.
*/

#include "aivoice_interface.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>


#define AIVOICE_ENABLE_AFE_SSL (1)

#if AIVOICE_ENABLE_AFE_SSL
#include "cJSON/cJSON.h"
#endif

#if defined(USE_DTCM)
#define DRAM0 __attribute__((section(".dram0.data")))
#define DRAM1 __attribute__((section(".dram1.data")))
#else
#define DRAM0
#define DRAM1
#endif

#define DATASIZE_124K (100 * 1024)
char DRAM0 g_dtcm_buffer_124k_0[DATASIZE_124K];

extern unsigned char *get_test_wav(void);
extern unsigned int get_test_wav_len(void);

#if AIVOICE_ENABLE_AFE_SSL
static void aivoice_show_afe_ssl_message(struct aivoice_evout_afe *afe_out)
{
	if (afe_out->out_others_json) {
		char *json = afe_out->out_others_json;

		cJSON *root_obj = cJSON_Parse((char *)json);
		if (root_obj != NULL) {
			cJSON *ssl_angle_obj = cJSON_GetObjectItem(root_obj, "ssl_angle");
			if (ssl_angle_obj) {
				float angle = ssl_angle_obj->valuedouble;
				if (angle >= 0) {
					printf("[user] voice angle %.1f\n", angle);
				}
			}

			cJSON_Delete(root_obj);
		}
	}
}
#endif

static int aivoice_callback_process(void *userdata,
									enum aivoice_out_event_type event_type,
									const void *msg, int len)
{

	(void)userdata;
	struct aivoice_evout_vad *vad_out;
	struct aivoice_evout_afe *afe_out;

	switch (event_type) {
	case AIVOICE_EVOUT_VAD:
		vad_out = (struct aivoice_evout_vad *)msg;
		printf("[user] vad. status = %d, offset = %d\n", vad_out->status, vad_out->offset_ms);
		break;

	case AIVOICE_EVOUT_WAKEUP:
		printf("[user] wakeup. %.*s\n", len, (char *)msg);
		break;

	case AIVOICE_EVOUT_ASR_RESULT:
		printf("[user] asr. %.*s\n", len, (char *)msg);
		break;

	case AIVOICE_EVOUT_ASR_REC_TIMEOUT:
		printf("[usr] asr timeout\n");
		break;

	case AIVOICE_EVOUT_AFE:
		afe_out = (struct aivoice_evout_afe *)msg;

		// afe will output audio each frame.
		// in this example, we only print it once to make log clear
		static int afe_out_printed = false;
		if (!afe_out_printed) {
			afe_out_printed = true;
			printf("[user] afe output %d channels raw audio, others: %s\n",
				   afe_out->ch_num, afe_out->out_others_json ? afe_out->out_others_json : "null");
		}

#if AIVOICE_ENABLE_AFE_SSL
		aivoice_show_afe_ssl_message(afe_out);
#endif

		// process afe output raw audio as needed
		// xx
		break;

	default:
		break;
	}

	return 0;
}

/* entry function of this example */
void aivoice_algo_offline_example(void)
{
	/* step 1:
	 * Select the aivoice flow you want to use.
	 * Refer to the end of aivoice_interface.h to see which flows are supported.
	 */
	const struct rtk_aivoice_iface *aivoice = &aivoice_iface_full_flow_v1;
	rtk_aivoice_set_multi_kws_mode();

	/* step 2:
	 * Modify the default configure if needed.
	 * You can modify 0 or more configures of afe/vad/kws/...
	 */
	struct aivoice_config config;
	memset(&config, 0, sizeof(config));

	/*
	 * here we use afe_res_2mic50mm for example.
	 * you can change these configuratons according the afe resource you used.
	 * refer to aivoce_afe_config.h for details;
	 *
	 * afe_config.mic_array MUST match the afe resource you linked.
	 */
	struct afe_config afe_param = AFE_CONFIG_ASR_DEFAULT();
	afe_param.mic_array = AFE_LINEAR_2MIC_50MM;  // change this according to the linked afe resource.
	config.afe = &afe_param;

	// afe ssl related configurations
#if AIVOICE_ENABLE_AFE_SSL
	afe_param.enable_ssl     = true;
	afe_param.ssl_resolution = 10;
	afe_param.ssl_min_hz     = 300;
	afe_param.ssl_max_hz     = 3500;
#endif

	/*
	 * ONLY turn on these settings when you are sure about what you are doing.
	 * it is recommend to use the default configure,
	 * if you do not know the meaning of these configure parameters.
	 */
#if 0
	struct vad_config vad_param = VAD_CONFIG_DEFAULT();
	vad_param.left_margin = 300; // you can change the configure if needed
	config.vad = &vad_param;    // can be NULL

	struct kws_config kws_param = KWS_CONFIG_DEFAULT();
	config.kws = &kws_param;    // can be NULL

	struct asr_config asr_param = ASR_CONFIG_DEFAULT();
	config.asr = &asr_param;    // can be NULL

	struct aivoice_sdk_config aivoice_param = AIVOICE_SDK_CONFIG_DEFAULT();
	aivoice_param.no_cmd_timeout = 10;
	config.common = &aivoice_param; // can be NULL
#endif

	/* step 3:
	 * Create the aivoice instance.
	 */
	void *handle = aivoice->create(&config);
	if (!handle) {
		return;
	}

	/* step 4:
	 * Register a callback function.
	 * You may only receive some of the aivoice_out_event_type in this example,
	 * depending on the flow you use.
	 * */
	rtk_aivoice_register_callback(handle, aivoice_callback_process, NULL);

	/* when run on chips, we get online audio stream,
	 * here we use a fix audio.
	 * */
	const char *audio = (const char *)get_test_wav();
	int len = get_test_wav_len();
	int audio_offset = 44;
	int mics_num = 2;
	int afe_frame_bytes = (mics_num + afe_param.ref_num) * afe_param.frame_size * sizeof(short);
	while (audio_offset <= len - afe_frame_bytes) {
		/* step 5:
		 * Feed the audio to the aivoice instance.
		 * */
		aivoice->feed(handle,
					  (char *)audio + audio_offset,
					  afe_frame_bytes);

		audio_offset += afe_frame_bytes;
	}

	/* step 6:
	 * Destroy the aivoice instance */
	aivoice->destroy(handle);
}
