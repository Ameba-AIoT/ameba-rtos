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

#define LOG_TAG "HalCaptureTest"
//#define LOG_NDEBUG 0

#include "platform_stdlib.h"
#include "basic_types.h"
#include "ameba_soc.h"
#include "hardware/audio/audio_hw_manager.h"

#define RECORD_SECONDS 5

static uint32_t CaptureSample(uint32_t channels, uint32_t rate, uint32_t record_bytes_one_time);

void AudioHalCaptureTestApp(char **argv)
{
	uint32_t channels = 2;
	uint32_t rate = 16000;
	uint32_t frames;
	uint32_t record_bytes_one_time = 4096;

	/* parse command line arguments */
	while (*argv) {
		if (strcmp(*argv, "-c") == 0) {
			argv++;
			if (*argv) {
				channels = atoi(*argv);
			}
		} else if (strcmp(*argv, "-r") == 0) {
			argv++;
			if (*argv) {
				rate = atoi(*argv);
			}
		} else if (strcmp(*argv, "-b") == 0) {
			argv++;
			if (*argv) {
				record_bytes_one_time = atoi(*argv);
			}
		}
		if (*argv) {
			argv++;
		}
	}

	frames = CaptureSample(channels, rate, record_bytes_one_time);

	printf("captured %u frames \n", frames);

	return;
}

/*in order to test input data, directly play it out through spk*/
static uint32_t CaptureSample(uint32_t channels, uint32_t rate, uint32_t record_bytes_one_time)
{
	char        *buffer;
	uint32_t     size;
	uint32_t     bytes_read = 0;
	uint32_t     frames = 0;
	uint32_t     bits = 16;
	int32_t      format = AUDIO_HW_FORMAT_PCM_16_BIT;
	int32_t      ret;
	uint32_t     record_size = rate * RECORD_SECONDS * channels * bits / 8;

	struct AudioHwManager        *audio_manager = NULL;

	struct AudioHwCard           *audio_card_in = NULL;
	struct AudioHwCardDescriptor *audio_card_in_desc = NULL;
	struct AudioHwStreamIn       *audio_stream_in = NULL;
	struct AudioHwPort            audio_port_in;

	struct AudioHwCard           *audio_card_out = NULL;
	struct AudioHwCardDescriptor *audio_card_out_desc = NULL;
	struct AudioHwStreamOut      *audio_stream_out = NULL;
	struct AudioHwPort            audio_port_out;

	/*init audio manager*/
	audio_manager = CreateAudioHwManager();
	if (audio_manager == NULL) {
		printf("get audio hw manager failed \n");
		goto end;
	}

	/*get all cards*/
	int32_t cards_size = audio_manager->GetCardsCount(audio_manager);
	struct AudioHwCardDescriptor *card_descs = audio_manager->GetCards(audio_manager);

	if (cards_size < 0) {
		printf("no card in AudioHwManager \n");
		goto end;
	}

	/*create audio_config for both input & output*/
	struct AudioHwConfig audio_config;
	audio_config.sample_rate = rate;
	audio_config.channel_count = channels;
	audio_config.format = format;

	/**************************************************************************************************
	 * **************************************output init*********************************************
	 * ************************************************************************************************/
	/*init input AudioHwPort*/
	audio_port_in.role = AUDIO_HW_PORT_ROLE_IN;
	audio_port_in.port_index = 0;
	audio_port_in.flags.in_flags = AUDIO_HW_INPUT_FLAG_NONE;
	audio_port_in.supported_devices = AUDIO_HW_DEVICE_NONE;
	audio_port_in.devices_num = 0;

	/*get audio card for input*/
	for (int32_t index = 0; index < cards_size; index++) {
		struct AudioHwCardDescriptor *desc = &card_descs[index];
		for (uint32_t port = 0; (desc != NULL && port < desc->port_num); port++) {
			if (desc->ports[port].role == AUDIO_HW_PORT_ROLE_IN &&
				(audio_card_in = audio_manager->OpenCard(audio_manager, desc))) {
				audio_port_in = desc->ports[port];
				audio_card_in_desc = desc;
				break;
			}
		}
	}

	if (audio_card_in == NULL) {
		printf("open audio card failed \n");
		goto end;
	}

	/*create audio input*/
	struct AudioHwPathDescriptor path_desc_in;
	path_desc_in.port_index = audio_port_in.port_index;
	path_desc_in.devices = AUDIO_HW_DEVICE_IN_MIC;
	path_desc_in.flags = AUDIO_HW_INPUT_FLAG_NONE;
	audio_stream_in = audio_card_in->CreateStreamIn(audio_card_in, &path_desc_in, &audio_config);
	if (audio_stream_in == NULL) {
		printf("create audio stream in failed \n");
		goto end;
	}

	/**************************************************************************************************
	 * **************************************input init**********************************************
	 * ************************************************************************************************/
	/*init audio_port_out*/
	audio_port_out.role = AUDIO_HW_PORT_ROLE_OUT;
	audio_port_out.port_index = 0;
	audio_port_out.flags.out_flags = AUDIO_HW_OUTPUT_FLAG_NONE;
	audio_port_out.supported_devices = AUDIO_HW_DEVICE_NONE;
	audio_port_out.devices_num = 0;
	/*get out card*/
	for (int32_t index = 0; index < cards_size; index++) {
		struct AudioHwCardDescriptor *desc = &card_descs[index];
		for (uint32_t port = 0; (desc != NULL && port < desc->port_num); port++) {
			if (desc->ports[port].role == AUDIO_HW_PORT_ROLE_OUT &&
				(audio_card_out = audio_manager->OpenCard(audio_manager, desc))) {
				audio_port_out = desc->ports[port];
				audio_card_out_desc = desc;
				break;
			}
		}
	}
	if (audio_card_out == NULL) {
		printf("open audio card failed \n");
		goto end;
	}

	/*create stream out*/
	struct AudioHwPathDescriptor path_desc_out;
	path_desc_out.port_index = audio_port_out.port_index;
	path_desc_out.devices = AUDIO_HW_DEVICE_OUT_SPEAKER;
	path_desc_out.flags = AUDIO_HW_OUTPUT_FLAG_NONE;
	audio_stream_out = audio_card_out->CreateStreamOut(audio_card_out, &path_desc_out, &audio_config);
	if (audio_stream_out == NULL) {
		printf("create audio stream out failed \n");
		goto end;
	}

	/**************************************************************************************************
	 * ************************************input=>output test******************************************
	 * ************************************************************************************************/
	size = record_bytes_one_time;

	buffer = (char *) malloc(size);
	if (!buffer) {
		goto end;
	}

	printf("capturing sample: %u ch, %u hz\n", channels, rate);
	do {
		audio_stream_in->Read(audio_stream_in, buffer, size);
		//drop first 100ms data of record, because record need some time to be stable, it's normal.
		if (bytes_read >= 100 * rate * channels * bits / 8 / 1000) {
			audio_stream_out->Write(audio_stream_out, buffer, size, true);
		} else {
			memset(buffer, 0, size);
			audio_stream_out->Write(audio_stream_out, buffer, size, true);
			audio_stream_out->Write(audio_stream_out, buffer, size, true);
		}
		bytes_read += size;
	} while (bytes_read < record_size);

	frames = bytes_read / (channels * bits / 8);

end:
	if (buffer != NULL) {
		free(buffer);
	}

	if (audio_stream_in != NULL) {
		audio_card_in->DestroyStreamIn(audio_card_in, audio_stream_in);
		audio_stream_in = NULL;
	}

	if (audio_stream_out != NULL) {
		audio_card_out->DestroyStreamOut(audio_card_out, audio_stream_out);
		audio_stream_out = NULL;
	}

	if (audio_manager != NULL) {
		audio_manager->CloseCard(audio_manager, audio_card_in, audio_card_in_desc);
		audio_card_in = NULL;
		audio_card_in_desc = NULL;
		audio_manager->CloseCard(audio_manager, audio_card_out, audio_card_out_desc);
		audio_card_out = NULL;
		audio_card_out_desc = NULL;
		DestoryAudioHwManager(audio_manager);
		audio_manager = NULL;
	}

	return frames;
}

u32 CmdAudioHalCaptureTest(u16 argc, unsigned char **argv)
{
	(void) argc;
	AudioHalCaptureTestApp((char **)argv);
	return _TRUE;
}
