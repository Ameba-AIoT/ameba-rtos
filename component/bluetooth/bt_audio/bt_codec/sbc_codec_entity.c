/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <rtk_bt_common.h>
#include <sbc_codec_entity.h>
#include <bt_audio_debug.h>

#define SBC_DEFAULT_DECODE_CHANNEL_NUM 2
#define SBC_DEFAULT_DECODE_FAST_FILTER_BUFFERS 27

static rtk_bt_sbc_codec_t sbc_codec_t = {0};
static bool priv_decode_flag = false;
static bool priv_encode_flag = false;
static SBC_ENC_PARAMS priv_encode_params = {0};
static uint8_t encoded_data[4096 + 16] = {0};
static OI_CODEC_SBC_DECODER_CONTEXT priv_decode_context = {0};
static uint32_t context_data[(((sizeof(OI_INT32) * SBC_MAX_BLOCKS * SBC_DEFAULT_DECODE_CHANNEL_NUM * SBC_MAX_BANDS) \
												   + (sizeof(SBC_BUFFER_T) * SBC_MAX_CHANNELS * SBC_MAX_BANDS * SBC_DEFAULT_DECODE_FAST_FILTER_BUFFERS) \
					+ (sizeof(OI_UINT32) - 1)) / sizeof(OI_UINT32))] = {0};
static int16_t pcm_data[SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS] = {0}; // default is one frame each time, cause bt_audio_intf.c will parse the frame number firstly and process one by one

static uint16_t caculate_sbc_frame_size(uint16_t blocks, uint8_t channel_mode, uint16_t subbands, uint16_t bitpool)
{
	uint16_t frame_size = 0;
	uint16_t channels = 0;

	if (channel_mode == 0) {
		channels = 1;
	} else {
		channels = 2;
	}
	/* stereo */
	if (channel_mode == 0x2) {
		frame_size = 4 + (4 * subbands * channels) / 8;
		float temp = (float)(blocks * bitpool) / 8;
		frame_size += (int)(temp + 0.5);
	} else if (channel_mode == 0x3) { /* joint_stereo */
		frame_size = 4 + (4 * subbands * channels) / 8;
		float temp = (float)(subbands + blocks * bitpool) / 8;
		frame_size += (int)(temp + 0.5);
	} else { /* mono and dual channel */
		frame_size = 4 + (4 * subbands * channels) / 8;
		float temp = (float)(blocks * channels * bitpool) / 8;
		frame_size += (int)(temp + 0.5);
	}

	return frame_size;
}

static bool bt_stack_sbc_decoder_init(sbc_mode_t mode, OI_CODEC_SBC_DECODER_CONTEXT *pcontext)
{
	OI_STATUS status = OI_STATUS_SUCCESS;

	if (!pcontext) {
		BT_LOGE("%s : input pcontext is NULL \r\n", __func__);
		return false;
	}
	// note: we always request stereo output, even for mono input
	status = OI_CODEC_SBC_DecoderReset(pcontext, context_data, sizeof(context_data), SBC_DEFAULT_DECODE_CHANNEL_NUM, SBC_DEFAULT_DECODE_CHANNEL_NUM, FALSE,
									   (bool)mode);
	if (status != OI_STATUS_SUCCESS) {
		BT_LOGE("%s : error during reset %d \r\n", __func__, status);
		return false;
	}
	// btstack_sbc_plc_init(&pstate->plc_state);

	return true;
}

static bool bt_stack_sbc_encoder_init(SBC_ENC_PARAMS *p_params, rtk_bt_sbc_encode_t *p_encode_t)
{

	if (!p_params || !p_encode_t) {
		BT_LOGE("%s : input parameters is NULL \r\n", __func__);
		return false;
	}
	if (p_encode_t->sbc_pkt_num > SBC_MAX_NUM_FRAME) {
		BT_LOGE("%s : sbc packet number %d is overflow, max is %d \r\n", __func__, p_encode_t->sbc_pkt_num, SBC_MAX_NUM_FRAME);
		return false;
	}
	p_params->sbc_mode = p_encode_t->sbc_enc_mode;
	p_params->s16ChannelMode = p_encode_t->channel_mode;
	p_params->s16NumOfSubBands = p_encode_t->subbands;
	p_params->s16NumOfBlocks = p_encode_t->blocks;
	p_params->s16AllocationMethod = (uint8_t)p_encode_t->alloc_method;
	switch (p_encode_t->sample_rate) {
	case 16000:
		p_params->s16SamplingFreq = SBC_sf16000;
		break;
	case 32000:
		p_params->s16SamplingFreq = SBC_sf32000;
		break;
	case 44100:
		p_params->s16SamplingFreq = SBC_sf44100;
		break;
	case 48000:
		p_params->s16SamplingFreq = SBC_sf48000;
		break;
	default:
		p_params->s16SamplingFreq = 0;
		break;
	}
	p_params->u16BitRate = 229;
	memset((void *)encoded_data, 0, 4096 + 16);
	p_params->pu8Packet = encoded_data;
	p_params->u8NumPacketToEncode = p_encode_t->sbc_pkt_num;
	SBC_Encoder_Init(p_params);
	p_encode_t->bitpool = p_params->s16BitPool;
	p_encode_t->frame_size = caculate_sbc_frame_size(p_params->s16NumOfBlocks, p_params->s16ChannelMode, p_params->s16NumOfSubBands, p_params->s16BitPool);
	BT_LOGA("frame_size is %d \r\n", p_encode_t->frame_size);

	return true;
}

static uint16_t sbc_codec_init(void *pentity, void *param)
{
	(void)pentity;
	rtk_bt_sbc_codec_t *psbc_codec_t = (rtk_bt_sbc_codec_t *)param;

	if (!psbc_codec_t) {
		BT_LOGE("%s : No rtk_bt_sbc_codec_t configure \r\n", __func__);
		return 1;
	}
	/* init encode component */
	if (!priv_encode_flag) {
		if (bt_stack_sbc_encoder_init(&priv_encode_params, &psbc_codec_t->encoder_t) == false) {
			BT_LOGE("%s : bt_stack_sbc_encoder_init error \r\n", __func__);
			return 1;
		}
		priv_encode_flag = true;
	}
	/* init decode component */
	if (!priv_decode_flag) {
		if (bt_stack_sbc_decoder_init((sbc_mode_t)psbc_codec_t->decoder_t.sbc_dec_mode, &priv_decode_context) == false) {
			BT_LOGE("%s : bt_stack_sbc_decoder_init error \r\n", __func__);
			return 1;
		}
		priv_decode_flag = true;
	}
	memcpy((void *)&sbc_codec_t, (void *)psbc_codec_t, sizeof(rtk_bt_sbc_codec_t));

	return 0;
}

extern void SBC_Encoder_Update_Encode_Pktnum(uint8_t pkt_num);

static uint16_t sbc_codec_update(void *pentity, void *param)
{
	(void)pentity;
	rtk_bt_sbc_codec_t *psbc_codec_t = (rtk_bt_sbc_codec_t *)param;

	if (!psbc_codec_t) {
		BT_LOGE("%s : No rtk_bt_sbc_codec_t configure \r\n", __func__);
		return 1;
	}
	/* update encode component */
	if (priv_encode_flag) {
		SBC_Encoder_Update_Encode_Pktnum(psbc_codec_t->encoder_t.sbc_pkt_num);
	}
	memcpy((void *)&sbc_codec_t, (void *)psbc_codec_t, sizeof(rtk_bt_sbc_codec_t));

	return 0;
}

static uint16_t sbc_codec_deinit(void *pentity)
{
	(void)pentity;

	priv_encode_flag = false;
	memset((void *)&priv_encode_params, 0, sizeof(SBC_ENC_PARAMS));
	priv_decode_flag = false;
	memset((void *)&priv_decode_context, 0, sizeof(OI_CODEC_SBC_DECODER_CONTEXT));
	/* pbluedroid_decode_state = NULL; */
	memset((void *)&sbc_codec_t, 0, sizeof(rtk_bt_sbc_codec_t));

	return 0;
}

static uint16_t sbc_decoder_process_data(void *pentity, uint8_t *data, uint32_t size, struct dec_codec_buffer *decode_buffer, uint32_t *ppcm_size,
										 struct audio_param *paudio_param)
{
	(void)pentity;
	(void)paudio_param;
	OI_STATUS status;
	const OI_BYTE *frame_data = data;
	int16_t *pcm_data_pointer = decode_buffer->pbuffer;
	OI_UINT32 frame_data_len = size;
	uint32_t pcmBytes, availPcmBytes;

	availPcmBytes = sizeof(pcm_data);
	// DBG_BAD("%s : Enter frame size %d \r\n", __func__, size);
	while (frame_data_len && availPcmBytes) {
		pcmBytes = availPcmBytes;
		status = OI_CODEC_SBC_DecodeFrame(&priv_decode_context,
										  &frame_data,
										  &frame_data_len,
										  pcm_data_pointer,
										  &pcmBytes);
		/* Handle decoding result. */
		if (status != OI_STATUS_SUCCESS) {
			BT_LOGE("%s: Decoding failure: %d \r\n", __func__, status);
			break;
		}
		availPcmBytes -= pcmBytes;
		pcm_data_pointer += pcmBytes / 2;
	}
	*ppcm_size = sizeof(pcm_data) - availPcmBytes;

	return (uint16_t)status;
}

static uint16_t sbc_encoder_process_data(void *pentity, int16_t *data, uint32_t size, struct enc_codec_buffer *pencoder_buffer, uint8_t *p_frame_num,
										 uint32_t *p_actual_len)
{
	(void)pentity;

	if (!priv_encode_flag) {
		BT_LOGE("%s : encode component need initialization firstly \r\n", __func__);
		return 1;
	}
	if (!data) {
		BT_LOGE("%s : input data is NULL \r\n", __func__);
		return 1;
	}
	if (size > ((SBC_MAX_NUM_FRAME * SBC_MAX_NUM_OF_BLOCKS * SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS) * 2)) {
		BT_LOGE("%s : input length max is %d in bytes \r\n", __func__,
				(int)(SBC_MAX_NUM_FRAME * SBC_MAX_NUM_OF_BLOCKS * SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS));
		return 1;
	}
	memset((void *)priv_encode_params.as16PcmBuffer, 0, (SBC_MAX_NUM_FRAME * SBC_MAX_NUM_OF_BLOCKS * SBC_MAX_NUM_OF_CHANNELS * SBC_MAX_NUM_OF_SUBBANDS));
	memcpy((void *)priv_encode_params.as16PcmBuffer, (void *)data, size);
	SBC_Encoder(&priv_encode_params);
	*p_actual_len = (uint32_t)priv_encode_params.u16PacketLength;
	*p_frame_num = (uint8_t)priv_encode_params.u8NumPacketToEncode;
	pencoder_buffer->pbuffer = priv_encode_params.pu8Packet;
	pencoder_buffer->frame_num = *p_frame_num;
	pencoder_buffer->frame_size = *p_actual_len;

	return 0;
}

static int get_bit16(uint16_t bitmap, int position)
{
	return (bitmap >> position) & 1;
}

static uint8_t read_sbc_header(uint8_t *packet, int size, int *offset, avdtp_sbc_codec_header_t *sbc_header)
{
	int sbc_header_len = 12; // without crc
	int pos = *offset;

	if (packet[pos] == 0x9c) {
		DBG_BAD("%s: No Sbc Header \r\n", __func__);
		return 0;
	}

	if (size - pos < sbc_header_len) {
		BT_LOGE("%s: Not enough data to read SBC header, expected %d, received %d\n", __func__, sbc_header_len, size - pos);
		return 0;
	}
	sbc_header->fragmentation = get_bit16(packet[pos], 7);
	sbc_header->starting_packet = get_bit16(packet[pos], 6);
	sbc_header->last_packet = get_bit16(packet[pos], 5);
	sbc_header->num_frames = packet[pos] & 0x0f;
	pos++;
	*offset = pos;

	DBG_BAD("%s: fragmentation %d, starting_packet %d, last_packet %d, num_frames %d \r\n", __func__, sbc_header->fragmentation,
			sbc_header->starting_packet,
			sbc_header->last_packet,
			sbc_header->num_frames);

	return 1;
}

static int read_frame_header(uint8_t *packet, int size, int *offset, sbc_frame_header_t *sbc_frame_header)
{
	int sbc_frame_header_len = 4; // without join RFA and scale factors
	int pos = *offset;

	if (size - pos < sbc_frame_header_len) {
		BT_LOGE("%s: Not enough data to read SBC frame header, expected %d, received %d\n", __func__, sbc_frame_header_len, size - pos);
		return 1;
	}
	sbc_frame_header->syncword = packet[pos];
	sbc_frame_header->sampling_frequency = (packet[pos + 1] >> 6) & 3;
	sbc_frame_header->blocks = (packet[pos + 1] >> 4) & 3;
	sbc_frame_header->channel_mode = (packet[pos + 1] >> 2) & 3;
	sbc_frame_header->allocation_method = (packet[pos + 1] >> 1) & 1;
	sbc_frame_header->subbands = packet[pos + 1] & 1;
	sbc_frame_header->bitpool = packet[pos + 2];
	sbc_frame_header->crc_check = packet[pos + 3];
	pos += 4;
	*offset = pos;

	DBG_BAD("%s: syncword %d, sampling_frequency %d, blocks %d, channel_mode %d, allocation_method %d, subbands %d, bitpool %d, crc_check %d \r\n",
			__func__, sbc_frame_header->syncword,
			sbc_frame_header->sampling_frequency,
			sbc_frame_header->blocks,
			sbc_frame_header->channel_mode,
			sbc_frame_header->allocation_method,
			sbc_frame_header->subbands,
			sbc_frame_header->bitpool,
			sbc_frame_header->crc_check);

	return 0;
}

static uint16_t sbc_audio_handle_media_data_packet(void *pentity, uint8_t *packet, uint16_t size, uint32_t *pframe_size, uint8_t *pframe_num,
												   uint8_t *pcodec_header_flag, struct audio_param *paudio_param)
{
	(void)pentity;
	avdtp_sbc_codec_header_t sbc_header = {0};
	sbc_frame_header_t sbc_frame_header = {0};
	uint32_t sbc_frame_size = 0;
	int pos = 0;

	/* decode sbc header */
	*pcodec_header_flag = read_sbc_header(packet, size, &pos, &sbc_header);

	/* decode sbc frame header */
	if (read_frame_header(packet, size - pos, &pos, &sbc_frame_header)) {
		BT_LOGE("%s: read sbc frame header fail \r\n", __func__);
		return 1;
	}

	/* calculate sbc frame size */
	{
		uint16_t blocks, channels, subbands;
		/* param blocks*/
		switch (sbc_frame_header.blocks) {
		case 0: {
			blocks = 4;
		}
		break;
		case 1: {
			blocks = 8;
		}
		break;
		case 2: {
			blocks = 12;
		}
		break;
		case 3: {
			blocks = 16;
		}
		break;
		default:
			break;
		}
		/* param channels*/
		if (sbc_frame_header.channel_mode == 0) {
			channels = 1;
		} else {
			channels = 2;
		}
		/* param subbands*/
		if (sbc_frame_header.subbands == 0) {
			subbands = 4;
		} else {
			subbands = 8;
		}
		/* P audio parameter */
		paudio_param->channels = channels;
		if (channels == 1) {
			paudio_param->channel_allocation = 1;
		} else {
			paudio_param->channel_allocation = 3;
		}
		if (sbc_frame_header.sampling_frequency == 0) {
			paudio_param->rate = 16000;
		} else if (sbc_frame_header.sampling_frequency == 1) {
			paudio_param->rate = 32000;
		} else if (sbc_frame_header.sampling_frequency == 2) {
			paudio_param->rate = 44100;
		} else if (sbc_frame_header.sampling_frequency == 3) {
			paudio_param->rate = 48000;
		}
		paudio_param->bits = 16;
		sbc_frame_size = caculate_sbc_frame_size(blocks, sbc_frame_header.channel_mode, subbands, sbc_frame_header.bitpool);
	}
	if (*pcodec_header_flag) {
		/* cause sbc_frame_size maybe float */
		*pframe_size = (uint32_t)(((size - 1) / sbc_header.num_frames)) > sbc_frame_size ? (uint32_t)(((size - 1) / sbc_header.num_frames)) : sbc_frame_size;
		*pframe_num = sbc_header.num_frames;
	} else {
		*pframe_size = sbc_frame_size;
		*pframe_num = 1;
	}

	return 0;
}

static struct dec_codec_buffer *sbc_decoder_buffer_get(void *pentity)
{
	(void)pentity;
	struct dec_codec_buffer *pdecoder_buffer = NULL;

	pdecoder_buffer = (struct dec_codec_buffer *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct dec_codec_buffer));
	if (pdecoder_buffer == NULL) {
		BT_LOGE("%s: allocate pdecoder_buffer fail \r\n", __func__);
		return NULL;
	}
	memset((void *)pcm_data, 0, SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS);
	pdecoder_buffer->pbuffer = pcm_data;
	pdecoder_buffer->total_size = (uint32_t)(SBC_MAX_SAMPLES_PER_FRAME * SBC_MAX_CHANNELS);
	pdecoder_buffer->actual_write_size = 0;

	return pdecoder_buffer;
}

static void sbc_free_decode_buffer(void *pentity, struct dec_codec_buffer *pdecoder_buffer)
{
	(void)pentity;

	osif_mem_free((void *)pdecoder_buffer);
}

static struct enc_codec_buffer *sbc_encoder_buffer_get(void *pentity)
{
	(void)pentity;
	struct enc_codec_buffer *pencoder_buffer = NULL;

	pencoder_buffer = (struct enc_codec_buffer *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct enc_codec_buffer));
	if (pencoder_buffer == NULL) {
		BT_LOGE("%s: allocate pencoder_buffer fail \r\n", __func__);
		return NULL;
	}
	pencoder_buffer->pbuffer = NULL;
	pencoder_buffer->frame_num = 0;

	return pencoder_buffer;
}

static void sbc_free_encode_buffer(void *pentity, struct enc_codec_buffer *pencodec_buffer)
{
	(void)pentity;

	osif_mem_free((void *)pencodec_buffer);
}

static int sbc_decoder_num_samples_per_frame(void *pentity)
{
	(void)pentity;

	return priv_decode_context.common.frameInfo.nrof_blocks * priv_decode_context.common.frameInfo.nrof_subbands;
}

static int sbc_decoder_num_channels(void *pentity)
{
	(void)pentity;

	return priv_decode_context.common.frameInfo.nrof_channels;
}

static int sbc_decoder_sample_rate(void *pentity)
{
	(void)pentity;

	return priv_decode_context.common.frameInfo.frequency;
}

uint16_t rtk_bt_audio_sbc_register(uint32_t type, PAUDIO_CODEC_ENTITY p_entity)
{
	uint16_t ret = 1;
	uint32_t lock_flag;

	DBG_BAD("%s:Enter \r\n", __func__);
	if (p_entity == NULL) {
		BT_LOGE("%s:NULL entity pointer \r\n", __func__);
		return ret;
	}
	lock_flag = osif_lock();
	p_entity->type = type;
	p_entity->stream_in_num = 0;
	p_entity->init = sbc_codec_init;
	p_entity->update = sbc_codec_update;
	p_entity->deinit = sbc_codec_deinit;
	p_entity->bt_audio_handle_media_data_packet = sbc_audio_handle_media_data_packet;
	p_entity->encoding_func = sbc_encoder_process_data;
	p_entity->decoding_func = sbc_decoder_process_data;
	p_entity->get_decode_buffer = sbc_decoder_buffer_get;
	p_entity->free_decode_buffer = sbc_free_decode_buffer;
	p_entity->get_encode_buffer = sbc_encoder_buffer_get;
	p_entity->free_encode_buffer = sbc_free_encode_buffer;
	p_entity->decoder_num_samples_per_frame = sbc_decoder_num_samples_per_frame;
	p_entity->decoder_num_channels = sbc_decoder_num_channels;
	p_entity->decoder_sample_rate = sbc_decoder_sample_rate;
	p_entity->encode_lock = false;
	p_entity->decode_lock = false;
	memset((void *)&p_entity->sbc, 0, sizeof(p_entity->sbc));
	osif_unlock(lock_flag);
	ret = 0;

	return ret;
}