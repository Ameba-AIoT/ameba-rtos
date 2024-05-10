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

static unsigned int Capture_Sample(unsigned int channels, unsigned int rate, unsigned int record_bytes_one_time);

void AudioHwCaptureTestApp(char **argv)
{
	unsigned int channels = 2;
	unsigned int rate = 16000;
	unsigned int frames;
	unsigned int record_bytes_one_time = 4096;

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

	frames = Capture_Sample(channels, rate, record_bytes_one_time);

	printf("Captured %u frames \n", frames);

	return;
}

/*in order to test capture data, directly play it out through spk*/
static unsigned int Capture_Sample(unsigned int channels, unsigned int rate, unsigned int record_bytes_one_time)
{
	char *buffer;
	unsigned int size;
	unsigned int bytes_read = 0;
	unsigned int frames = 0;
	unsigned int bits = 16;
	int format = AUDIO_HW_FORMAT_PCM_16_BIT;
	int32_t ret;
	unsigned int record_size = rate * RECORD_SECONDS * channels * bits / 8;

	struct AudioHwManager *audio_manager = NULL;
	struct AudioHwAdapter *cap_audio_adapter = NULL;
	struct AudioHwCapture *audio_capture = NULL;
	struct AudioHwAdapterDescriptor *cap_adapter_desc = NULL;
	struct AudioHwPort capture_port;
	struct AudioHwRender *audio_render = NULL;
	struct AudioHwAdapter *out_audio_adapter = NULL;
	struct AudioHwAdapterDescriptor *out_adapter_desc = NULL;
	struct AudioHwPort render_port;

	/*init audio manager*/
	audio_manager = GetAudioHwManagerFuncs();
	if (audio_manager == NULL) {
		printf("Get AudioHwManager failed \n");
		goto end;
	}

	/*get all adapters*/
	int32_t adapters_size = -1;
	struct AudioHwAdapterDescriptor *descs = NULL;
	audio_manager->GetAllAdapters(audio_manager, &descs, &adapters_size);
	if (adapters_size < 0) {
		printf("No adapter in AudioHwManager \n");
		goto end;
	}

	/*create param for both cap & playback*/
	struct AudioHwSampleAttributes param;
	param.sample_rate = rate;
	param.channel_count = channels;
	param.format = format;
	param.interleaved = false;

	/**************************************************************************************************
	 * **************************************playback init*********************************************
	 * ************************************************************************************************/
	/*init cap AudioHwPort*/
	capture_port.dir = AUDIO_HW_PORT_IN;
	capture_port.port_id = 0;
	capture_port.port_name = NULL;

	/*get audio adapter for cap*/
	for (int index = 0; index < adapters_size; index++) {
		struct AudioHwAdapterDescriptor *desc = &descs[index];
		for (unsigned int port = 0; (desc != NULL && port < desc->port_num); port++) {
			if (desc->ports[port].dir == AUDIO_HW_PORT_IN &&
				(audio_manager->LoadAdapter(audio_manager, desc, &cap_audio_adapter)) == 0) {
				capture_port = desc->ports[port];
				cap_adapter_desc = desc;
				break;
			}
		}
	}

	if (cap_audio_adapter == NULL) {
		printf("Load audio adapter failed \n");
		goto end;
	}

	/*create audio capture*/
	struct AudioHwDeviceDescriptor cap_device_desc;
	cap_device_desc.port_id = capture_port.port_id;
	cap_device_desc.pins = AUDIO_HW_PIN_IN_MIC;
	cap_device_desc.desc = NULL;
	ret = cap_audio_adapter->CreateCapture(cap_audio_adapter, &cap_device_desc, &param, AUDIO_HW_INPUT_FLAG_NONE, &audio_capture);
	if (ret != 0 || audio_capture == NULL) {
		printf("Create audio Capture failed \n");
		goto end;
	}

	/**************************************************************************************************
	 * **************************************capture init**********************************************
	 * ************************************************************************************************/
	/*init render_port*/
	render_port.dir = AUDIO_HW_PORT_OUT;
	render_port.port_id = 0;
	render_port.port_name = NULL;

	/*get out adapter*/
	for (int index = 0; index < adapters_size; index++) {
		struct AudioHwAdapterDescriptor *desc = &descs[index];
		for (unsigned int port = 0; (desc != NULL && port < desc->port_num); port++) {
			if (desc->ports[port].dir == AUDIO_HW_PORT_OUT &&
				(audio_manager->LoadAdapter(audio_manager, desc, &out_audio_adapter)) == 0) {
				render_port = desc->ports[port];
				out_adapter_desc = desc;
				break;
			}
		}
	}
	if (out_audio_adapter == NULL) {
		printf("Load audio adapter failed \n");
		goto end;
	}

	/*create Render*/
	struct AudioHwDeviceDescriptor device_desc;
	device_desc.port_id = render_port.port_id;
	device_desc.pins = AUDIO_HW_PIN_OUT_SPEAKER;
	device_desc.desc = NULL;
	ret = out_audio_adapter->CreateRender(out_audio_adapter, &device_desc, &param, AUDIO_HW_OUTPUT_FLAG_NONE, &audio_render);
	if (ret != 0 || audio_render == NULL) {
		printf("Create audio render failed \n");
		goto end;
	}

	/**************************************************************************************************
	 * ************************************cap=>playback test******************************************
	 * ************************************************************************************************/
	size = record_bytes_one_time;

	buffer = (char *) malloc(size);
	if (!buffer) {
		goto end;
	}

	printf("Capturing sample: %u ch, %u hz\n", channels, rate);
	do {
		audio_capture->Read(audio_capture, buffer, size);
		//drop first 100ms data of record, because record need some time to be stable, it's normal.
		if (bytes_read >= 100 * rate * channels * bits / 8 / 1000) {
			audio_render->Write(audio_render, buffer, size, true);
		} else {
			memset(buffer, 0, size);
			audio_render->Write(audio_render, buffer, size, true);
			audio_render->Write(audio_render, buffer, size, true);
		}
		bytes_read += size;
	} while (bytes_read < record_size);

	frames = bytes_read / (channels * bits / 8);

end:
	if (buffer != NULL) {
		free(buffer);
	}

	if (audio_capture != NULL) {
		cap_audio_adapter->DestroyCapture(cap_audio_adapter, audio_capture);
		audio_capture = NULL;
	}

	if (audio_render != NULL) {
		out_audio_adapter->DestroyRender(out_audio_adapter, audio_render);
		audio_render = NULL;
	}

	if (audio_manager != NULL) {
		audio_manager->UnloadAdapter(audio_manager, cap_adapter_desc, cap_audio_adapter);
		cap_audio_adapter = NULL;
		cap_adapter_desc = NULL;
		audio_manager->UnloadAdapter(audio_manager, out_adapter_desc, out_audio_adapter);
		out_audio_adapter = NULL;
		out_adapter_desc = NULL;
		DestoryAudioHwManager(audio_manager);
		audio_manager = NULL;
	}

	return frames;
}

u32 CmdAudioHalCaptureTest(u16 argc, unsigned char **argv)
{
	(void) argc;
	AudioHwCaptureTestApp((char **)argv);
	return _TRUE;
}
