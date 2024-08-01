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
#include "platform_stdlib.h"
#include "basic_types.h"
#include "hardware/audio/audio_hw_manager.h"

#define PLAY_SECONDS 5
#define FRAMES_MULTIPLY_CHANNELS 96

u32 sine32_48k[FRAMES_MULTIPLY_CHANNELS] = {
	0X00000000,	0X00000000, 0X10B50000, 0X10B50000, 0X21200000, 0X21200000, 0X30FB0000, 0X30FB0000, 0X3FFF0000, 0X3FFF0000, 0X4DEB0000, 0X4DEB0000,
	0X5A820000,	0X5A820000, 0X658C0000, 0X658C0000, 0X6ED90000, 0X6ED90000, 0X76410000, 0X76410000, 0X7BA30000,	0X7BA30000, 0X7EE70000, 0X7EE70000,
	0X7FFF0000, 0X7FFF0000, 0X7EE70000, 0X7EE70000, 0X7BA30000, 0X7BA30000, 0X76410000, 0X76410000,	0X6ED90000,	0X6ED90000, 0X658C0000, 0X658C0000,
	0X5A820000,	0X5A820000, 0X4DEB0000, 0X4DEB0000, 0X3FFF0000, 0X3FFF0000, 0X30FB0000, 0X30FB0000, 0X21200000, 0X21200000, 0X10B50000, 0X10B50000,
	0X00000000,	0X00000000, 0XEF4A0000, 0XEF4A0000, 0XDEDF0000, 0XDEDF0000, 0XCF040000, 0XCF040000, 0XC0000000, 0XC0000000,	0XB2140000,	0XB2140000,
	0XA57D0000,	0XA57D0000, 0X9A730000, 0X9A730000,	0X91260000,	0X91260000, 0X89BE0000, 0X89BE0000,	0X845C0000,	0X845C0000, 0X81180000, 0X81180000,
	0X80000000,	0X80000000,	0X81180000,	0X81180000,	0X845C0000,	0X845C0000, 0X89BE0000, 0X89BE0000, 0X91260000, 0X91260000, 0X9A730000, 0X9A730000,
	0XA57D0000, 0XA57D0000, 0XB2140000,	0XB2140000,	0XC0000000, 0XC0000000,	0XCF040000, 0XCF040000, 0XDEDF0000, 0XDEDF0000, 0XEF4A0000,	0XEF4A0000
};

u16 sine_48k[FRAMES_MULTIPLY_CHANNELS] = {
	0X0000,	0X0000, 0X10B5, 0X10B5, 0X2120, 0X2120, 0X30FB, 0X30FB, 0X3FFF, 0X3FFF, 0X4DEB, 0X4DEB,
	0X5A82,	0X5A82, 0X658C, 0X658C, 0X6ED9, 0X6ED9, 0X7641, 0X7641, 0X7BA3,	0X7BA3, 0X7EE7, 0X7EE7,
	0X7FFF, 0X7FFF, 0X7EE7, 0X7EE7, 0X7BA3, 0X7BA3, 0X7641, 0X7641,	0X6ED9,	0X6ED9, 0X658C, 0X658C,
	0X5A82,	0X5A82, 0X4DEB, 0X4DEB, 0X3FFF, 0X3FFF, 0X30FB, 0X30FB, 0X2120, 0X2120, 0X10B5, 0X10B5,
	0X0000,	0X0000, 0XEF4A, 0XEF4A, 0XDEDF, 0XDEDF, 0XCF04, 0XCF04, 0XC000, 0XC000,	0XB214,	0XB214,
	0XA57D,	0XA57D, 0X9A73, 0X9A73,	0X9126,	0X9126, 0X89BE, 0X89BE,	0X845C,	0X845C, 0X8118, 0X8118,
	0X8000,	0X8000,	0X8118,	0X8118,	0X845C,	0X845C, 0X89BE, 0X89BE, 0X9126, 0X9126, 0X9A73, 0X9A73,
	0XA57D, 0XA57D, 0XB214,	0XB214,	0XC000, 0XC000,	0XCF04, 0XCF04, 0XDEDF, 0XDEDF, 0XEF4A,	0XEF4A
};

static void PlaySample(uint32_t channels, uint32_t rate, uint32_t bits);

void AudioHalRenderTestApp(char **argv)
{
	uint32_t rate = 48000;
	uint32_t num_channels = 2;
	uint32_t bits_per_sample = 16;

	while (*argv) {
		if (strcmp(*argv, "-b") == 0) {
			argv++;
			if (*argv) {
				bits_per_sample = atoi(*argv);
			}
		}
		if (*argv) {
			argv++;
		}
	}

	PlaySample(num_channels, rate, bits_per_sample);
}

static void PlaySample(uint32_t channels, uint32_t rate, uint32_t bits)
{

	struct AudioHwManager        *audio_manager;
	struct AudioHwCard           *audio_card;
	struct AudioHwCardDescriptor *audio_card_desc;
	struct AudioHwStreamOut      *audio_stream_out;
	struct AudioHwPort            audio_port;
	enum AudioHwFormat            format;
	uint32_t                      frames_played = 0;
	uint32_t                      play_frame_size = rate * PLAY_SECONDS;

	printf("play sample channels:%lu, rate:%lu, bits=%lu \n", channels, rate, bits);

	switch (bits) {
	case 16:
		format = AUDIO_HW_FORMAT_PCM_16_BIT;
		break;
	case 32:
		format = AUDIO_HW_FORMAT_PCM_32_BIT;
		break;
	default:
		break;
	}

	audio_port.role = AUDIO_HW_PORT_ROLE_OUT;
	audio_port.port_index = 0;
	audio_port.flags.out_flags = AUDIO_HW_OUTPUT_FLAG_NONE;
	audio_port.supported_devices = AUDIO_HW_DEVICE_NONE;
	audio_port.devices_num = 0;

	audio_manager = CreateAudioHwManager();
	if (audio_manager == NULL) {
		printf("get audio hw manager failed \n");
		return;
	}

	int32_t cards_size = audio_manager->GetCardsCount(audio_manager);
	struct AudioHwCardDescriptor *card_descs = audio_manager->GetCards(audio_manager);

	if (cards_size < 0) {
		printf("no card in hal \n");
		return;
	}

	for (int32_t index = 0; index < cards_size; index++) {
		struct AudioHwCardDescriptor *desc = &card_descs[index];
		for (uint32_t port = 0; (desc != NULL && port < desc->port_num); port++) {
			printf("check for audio port \n");
			if (desc->ports[port].role == AUDIO_HW_PORT_ROLE_OUT &&
				(audio_card = audio_manager->OpenCard(audio_manager, desc))) {
				audio_port = desc->ports[port];
				audio_card_desc = desc;
				break;
			}
		}
	}
	if (audio_card == NULL) {
		printf("open audio card failed \n");
		return;
	}

	struct AudioHwConfig audio_config;
	audio_config.sample_rate = rate;
	audio_config.channel_count = channels;
	audio_config.format = format;
	struct AudioHwPathDescriptor path_desc;
	path_desc.port_index = audio_port.port_index;
	path_desc.devices = AUDIO_HW_DEVICE_OUT_SPEAKER;
	path_desc.flags = AUDIO_HW_INPUT_FLAG_NONE;
	audio_stream_out = audio_card->CreateStreamOut(audio_card, &path_desc, &audio_config);
	if (audio_stream_out == NULL) {
		printf("create audio stream out failed \n");
		return;
	}

	ssize_t size = 0;

	while (1) {
		if (format == AUDIO_HW_FORMAT_PCM_32_BIT) {
			size = FRAMES_MULTIPLY_CHANNELS * bits / 8;
			audio_stream_out->Write(audio_stream_out, (u8 *)sine32_48k, size, true);
		} else if (format == AUDIO_HW_FORMAT_PCM_16_BIT) {
			size = FRAMES_MULTIPLY_CHANNELS * bits / 8;
			audio_stream_out->Write(audio_stream_out, (u8 *)sine_48k, size, true);
		}
		frames_played += size / (channels * bits / 8);
		if (frames_played >= play_frame_size) {
			break;
		}
	}

	audio_card->DestroyStreamOut(audio_card, audio_stream_out);

	if (audio_manager != NULL) {
		audio_manager->CloseCard(audio_manager, audio_card, audio_card_desc);
		DestoryAudioHwManager(audio_manager);
		audio_card = NULL;
		audio_manager = NULL;
		audio_card_desc = NULL;
	}
}

u32 CmdAudioHalRenderTest(u16 argc, unsigned char **argv)
{
	(void) argc;
	AudioHalRenderTestApp((char **)argv);
	return _TRUE;
}
