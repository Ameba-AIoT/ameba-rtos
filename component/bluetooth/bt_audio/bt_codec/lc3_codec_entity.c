/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <osif.h>
#include <stdio.h>
#include <string.h>
#include <rtk_bt_common.h>
#include <lc3_codec_entity.h>
#include <bt_audio_debug.h>
#include "lc3.h"
#include "section_config.h"

#define BIT_PER_SAMPLE_DECODE 16
#define BIT_PER_SAMPLE_ENCODE 16

// static int byte_count_max_dec = 800;

typedef struct {
	lc3_encoder_t enc[2];
	lc3_decoder_t dec[2];
} lc3_codec_t;

static struct audio_sample_option le_audio_sample_rate_tab[] = {
	{0x01, 8000, true},
	{0x02, 11000, false},
	{0x03, 16000, true},
	{0x04, 22000, false},
	{0x05, 24000, true},
	{0x06, 32000, true},
	{0x07, 44100, true}, // 0x07 -> 44100, 44100 param is equal 48000 in lc3
	{0x08, 48000, true},
	{0x09, 88000, false},
	{0x0A, 96000, false},
	{0x0B, 176000, false},
	{0x0C, 192000, false},
	{0x0D, 384000, false},
	{0xFF, 0, false}
};

static int get_sample_rate(uint8_t sample_index)
{
	uint8_t i = 0;
	int sample_rate = 0;
	/* foreach event table */
	while (1) {
		if (le_audio_sample_rate_tab[i].sample_rate_index == sample_index) {
			if (!le_audio_sample_rate_tab[i].support_flag) {
				BT_LOGE("%s: !!!! lc3 codec not support srate %d \r\n", __func__, le_audio_sample_rate_tab[i].sample_rate);
			} else {
				sample_rate = le_audio_sample_rate_tab[i].sample_rate;
			}
			break;
		}
		if (le_audio_sample_rate_tab[i].sample_rate_index == 0xFF) {
			BT_LOGE("%s: cannot find matched sample rate \r\n", __func__);
			break;
		}
		i++;
	}

	return sample_rate;
}

static uint32_t get_lea_chnl_num(uint32_t audio_channel_allocation)
{
	uint32_t channels = 0;

	//count_bits_1
	while (audio_channel_allocation) {
		channels ++ ;
		audio_channel_allocation &= (audio_channel_allocation - 1);
	}

	//BT_LOGE("%s audio_channel_allocation = 0x%x, channels=%d\r\n",__func__,(unsigned int)audio_channel_allocation,channels);
	return channels;
}

static bool private_lc3_codec_init(PAUDIO_CODEC_ENTITY p_entity, rtk_bt_le_audio_lc3_cfg_t *p_le_audio_codec_cfg)
{
	lc3_codec_t *p_codec_t = (lc3_codec_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(lc3_codec_t));
	if (!p_codec_t) {
		goto fail;
	}
	if (!p_le_audio_codec_cfg) {
		DBG_BAD("%s : lc3 decoder param is NULL \r\n", __func__);
		goto fail;
	}

	p_entity->lc3.sample_rate = get_sample_rate(p_le_audio_codec_cfg->sample_frequency);
	if (!p_entity->lc3.sample_rate) {
		DBG_BAD("%s : lc3 sample rate invalid \r\n", __func__);
		goto fail;
	}
	/* units: us */
	if (p_le_audio_codec_cfg->frame_duration) {
		p_entity->lc3.frame_duration = 10000;
	} else {
		p_entity->lc3.frame_duration = 7500;
	}
	p_entity->lc3.codec_frame_blocks_per_sdu = p_le_audio_codec_cfg->codec_frame_blocks_per_sdu;
	p_entity->lc3.audio_channel_allocation = p_le_audio_codec_cfg->audio_channel_allocation;
	p_entity->lc3.channels = get_lea_chnl_num(p_entity->lc3.audio_channel_allocation);
	p_entity->lc3.compress_bytes = p_le_audio_codec_cfg->compress_bytes;
	p_entity->lc3.bits_per_audio_sample_enc = 16;
	p_entity->lc3.bits_per_audio_sample_dec = 16;
	/* setup encoder handle */
	for (uint32_t ich = 0; ich < p_entity->lc3.channels; ich ++) {
		void *p_mem = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, lc3_encoder_size(p_entity->lc3.frame_duration, p_entity->lc3.sample_rate));
		if (!p_mem) {
			goto fail;
		}
		p_codec_t->enc[ich] = lc3_setup_encoder(p_entity->lc3.frame_duration, p_entity->lc3.sample_rate, p_entity->lc3.sample_rate, p_mem);
		if (!p_codec_t->enc[ich]) {
			osif_mem_free(p_mem);
			goto fail;
		}
	}
	/* setup decoder handle */
	for (uint32_t ich = 0; ich < p_entity->lc3.channels; ich ++) {
		void *p_mem = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, lc3_decoder_size(p_entity->lc3.frame_duration, p_entity->lc3.sample_rate));
		if (!p_mem) {
			goto fail;
		}
		p_codec_t->dec[ich] = lc3_setup_decoder(p_entity->lc3.frame_duration, p_entity->lc3.sample_rate, p_entity->lc3.sample_rate, p_mem);
		if (!p_codec_t->dec[ich]) {
			osif_mem_free(p_mem);
			goto fail;
		}
	}
	p_entity->lc3.p_lc3_t = (void *)p_codec_t;

	return _SUCCESS;

fail:
	if (p_codec_t) {
		for (uint32_t ich = 0; ich < p_entity->lc3.channels; ich ++) {
			if (p_codec_t->enc[ich]) {
				osif_mem_free(p_codec_t->enc[ich]);
			}
			if (p_codec_t->dec[ich]) {
				osif_mem_free(p_codec_t->dec[ich]);
			}
		}
		osif_mem_free(p_codec_t);
	}
	p_entity->lc3.sample_rate = 0;
	p_entity->lc3.frame_duration = 0;
	p_entity->lc3.channels = 0;
	p_entity->lc3.bits_per_audio_sample_enc = 0;
	p_entity->lc3.bits_per_audio_sample_dec = 0;
	p_entity->lc3.codec_frame_blocks_per_sdu = 0;
	p_entity->lc3.audio_channel_allocation = 0;
	p_entity->lc3.compress_bytes = 0;

	return _FAIL;
}

static void private_lc3_codec_deinit(PAUDIO_CODEC_ENTITY p_entity)
{
	lc3_codec_t *p_codec_t = (lc3_codec_t *)p_entity->lc3.p_lc3_t;
	if (p_codec_t) {
		for (uint32_t ich = 0; ich < p_entity->lc3.channels; ich ++) {
			if (p_codec_t->enc[ich]) {
				osif_mem_free(p_codec_t->enc[ich]);
			}
			if (p_codec_t->dec[ich]) {
				osif_mem_free(p_codec_t->dec[ich]);
			}
		}
		osif_mem_free(p_codec_t);
	}
	p_entity->lc3.frame_duration = 0;
	p_entity->lc3.sample_rate = 0;
	p_entity->lc3.frame_duration = 0;
	p_entity->lc3.channels = 0;
	p_entity->lc3.codec_frame_blocks_per_sdu = 0;
	p_entity->lc3.audio_channel_allocation = 0;
	p_entity->lc3.compress_bytes = 0;
}

uint16_t lc3_codec_init(void *p_entity, void *param)
{
	/* 3rdparty codec init */
	if (private_lc3_codec_init((PAUDIO_CODEC_ENTITY)p_entity, (rtk_bt_le_audio_lc3_cfg_t *)param) == false) {
		DBG_BAD("%s : lc3_decoder_init errir \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}

	return RTK_BT_AUDIO_OK;
}

uint16_t lc3_codec_deinit(void *p_entity)
{
	private_lc3_codec_deinit((PAUDIO_CODEC_ENTITY)p_entity);

	return RTK_BT_AUDIO_OK;
}

TIMESENSITIVE_TEXT_SECTION
uint16_t lc3_decoder_process_data(void *p_entity, uint8_t *data, uint32_t size, struct dec_codec_buffer *decode_buffer, uint32_t *ppcm_size,
								  struct audio_param *paudio_param)
{
	(void)paudio_param;
	int frame_bytes, pcm_sbytes, frame_samples;
	PAUDIO_CODEC_ENTITY entity = (PAUDIO_CODEC_ENTITY)p_entity;
	lc3_codec_t *p_codec_t = (lc3_codec_t *)entity->lc3.p_lc3_t;
	enum lc3_pcm_format pcm_fmt = entity->lc3.bits_per_audio_sample_dec == 24 ? LC3_PCM_FORMAT_S24_3LE : LC3_PCM_FORMAT_S16;
	void *pout = NULL;

	/* caculate pcm per sample bytes */
	pcm_sbytes = entity->lc3.bits_per_audio_sample_dec / 8;
	/* caculate the number of PCM samples in a frame */
	frame_samples = lc3_frame_samples(entity->lc3.frame_duration, entity->lc3.sample_rate);
#if 0
	bit_rate = entity->lc3.compress_bytes;
	/* get size of encoded frames */
	frame_bytes = lc3_frame_bytes(entity->lc3.frame_duration, bit_rate / entity->lc3.channels);
#else
	frame_bytes = entity->lc3.compress_bytes;
#endif
	if ((frame_bytes * entity->lc3.channels) > size) {
		BT_LOGE("%s: input lc3 data size (%d) is not matched with lc3 framesize (%d) \r\n", __func__, (int)size, frame_bytes);
		return RTK_BT_AUDIO_FAIL;
	}
	/* allocate decode buffer */
	pout = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, entity->lc3.channels * frame_samples * pcm_sbytes);
	if (!pout) {
		BT_LOGE("%s: allocate decode buffer fail \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	/* --- Decoding loop --- */
	for (uint32_t ich = 0; ich < entity->lc3.channels; ich ++) {
		lc3_decode(p_codec_t->dec[ich],
				   data + ich * frame_bytes, frame_bytes,
				   pcm_fmt, (void *)((uint8_t *)pout + ich * pcm_sbytes), entity->lc3.channels);
	}
	decode_buffer->pbuffer = pout;
	decode_buffer->total_size = entity->lc3.channels * frame_samples * pcm_sbytes;
	decode_buffer->actual_write_size = entity->lc3.channels * pcm_sbytes * frame_samples;
	*ppcm_size = decode_buffer->actual_write_size;

	return RTK_BT_AUDIO_OK;
}

TIMESENSITIVE_TEXT_SECTION
uint16_t lc3_encoder_process_data(void *p_entity, int16_t *data, uint32_t size, struct enc_codec_buffer *pencoder_buffer, uint8_t *p_frame_num,
								  uint32_t *p_actual_len)
{
	int frame_bytes, pcm_sbytes, frame_samples;
	PAUDIO_CODEC_ENTITY entity = (PAUDIO_CODEC_ENTITY)p_entity;
	lc3_codec_t *p_codec_t = (lc3_codec_t *)entity->lc3.p_lc3_t;
	enum lc3_pcm_format pcm_fmt =
		entity->lc3.bits_per_audio_sample_enc == 32 ? LC3_PCM_FORMAT_S24 :
		entity->lc3.bits_per_audio_sample_enc == 24 ? LC3_PCM_FORMAT_S24_3LE : LC3_PCM_FORMAT_S16;
	void *pout = NULL;

	/* caculate pcm per sample bytes */
	pcm_sbytes = entity->lc3.bits_per_audio_sample_enc / 8;
	/* caculate the number of PCM samples in a frame */
	frame_samples = lc3_frame_samples(entity->lc3.frame_duration, entity->lc3.sample_rate);
#if 0
	bit_rate = entity->lc3.compress_bytes;
	/* get size of encoded frames */
	frame_bytes = lc3_frame_bytes(entity->lc3.frame_duration, bit_rate / entity->lc3.channels);
#else
	frame_bytes = entity->lc3.compress_bytes;
#endif
	if ((entity->lc3.channels * pcm_sbytes * frame_samples) > size) {
		BT_LOGE("%s: input pcm data size (%d) is not matched with lc3 required (%d) \r\n", __func__, (int)size,
				(int)(entity->lc3.channels * pcm_sbytes * frame_samples));
		return RTK_BT_AUDIO_FAIL;
	}
	/* allocate encode buffer */
	pout = (void *)osif_mem_alloc(RAM_TYPE_DATA_ON, entity->lc3.channels * frame_bytes);
	if (!pout) {
		BT_LOGE("%s: allocate encode buffer fail \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	/* --- Encoding loop --- */
	for (uint32_t ich = 0; ich < entity->lc3.channels; ich ++) {
		lc3_encode(p_codec_t->enc[ich], pcm_fmt, (void *)((uint8_t *)data + ich * pcm_sbytes), entity->lc3.channels,
				   frame_bytes, (void *)((uint8_t *)pout + ich * frame_bytes));
	}
	// osif_unlock(0);
	*p_actual_len = entity->lc3.channels * frame_bytes;
	*p_frame_num = 1;
	pencoder_buffer->pbuffer = (uint8_t *)pout;
	pencoder_buffer->frame_num = *p_frame_num;
	pencoder_buffer->frame_size = *p_actual_len;

	return RTK_BT_AUDIO_OK;
}

uint16_t le_audio_handle_media_data_packet(void *p_entity, uint8_t *packet, uint16_t size, uint32_t *pframe_size, uint8_t *pframe_num,
										   uint8_t *pcodec_header_flag, struct audio_param *paudio_param)
{
	(void)packet;
	PAUDIO_CODEC_ENTITY entity = (PAUDIO_CODEC_ENTITY)p_entity;

	*pframe_size = size;
	*pframe_num = entity->lc3.codec_frame_blocks_per_sdu;
	*pcodec_header_flag = 0;
	paudio_param->channels = get_lea_chnl_num(entity->lc3.audio_channel_allocation);
	paudio_param->channel_allocation = entity->lc3.audio_channel_allocation;
	paudio_param->rate = entity->lc3.sample_rate;
	paudio_param->bits = entity->lc3.bits_per_audio_sample_dec;

	return RTK_BT_AUDIO_OK;
}

struct dec_codec_buffer *lc3_decoder_buffer_get(void *p_entity)
{
	(void)p_entity;
	struct dec_codec_buffer *pdecoder_buffer = NULL;

	pdecoder_buffer = (struct dec_codec_buffer *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct dec_codec_buffer));
	if (pdecoder_buffer == NULL) {
		BT_LOGE("%s:: allocate pbuffer fial \r\n", __func__);
		return NULL;
	}
	pdecoder_buffer->pbuffer = NULL;
	pdecoder_buffer->total_size = 0;
	pdecoder_buffer->actual_write_size = 0;

	return pdecoder_buffer;
}

void lc3_free_decode_buffer(void *p_entity, struct dec_codec_buffer *pdecodec_buffer)
{
	(void)p_entity;
	/* release pcm buffer */
	osif_mem_free((void *)pdecodec_buffer->pbuffer);
	/* free codec buffer */
	osif_mem_free((void *)pdecodec_buffer);
}

struct enc_codec_buffer *lc3_encoder_buffer_get(void *p_entity)
{
	(void)p_entity;
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

void lc3_free_encode_buffer(void *p_entity, struct enc_codec_buffer *pencodec_buffer)
{
	(void)p_entity;

	osif_mem_free((void *)pencodec_buffer->pbuffer);
	osif_mem_free((void *)pencodec_buffer);
}

int lc3_decoder_num_samples_per_frame(void *p_entity)
{
	PAUDIO_CODEC_ENTITY entity = (PAUDIO_CODEC_ENTITY)p_entity;

	return entity->lc3.codec_frame_blocks_per_sdu;
}

int lc3_decoder_num_channels(void *p_entity)
{
	PAUDIO_CODEC_ENTITY entity = (PAUDIO_CODEC_ENTITY)p_entity;

	return entity->lc3.channels;
}

int lc3_decoder_sample_rate(void *p_entity)
{
	PAUDIO_CODEC_ENTITY entity = (PAUDIO_CODEC_ENTITY)p_entity;

	return entity->lc3.sample_rate;
}

uint16_t rtk_bt_audio_lc3_register(uint32_t type, PAUDIO_CODEC_ENTITY p_entity)
{
	uint16_t ret = RTK_BT_AUDIO_FAIL;
	uint32_t lock_flag;

	DBG_BAD("%s:Enter \r\n", __func__);
	if (p_entity == NULL) {
		DBG_BAD("%s:NULL entity pointer \r\n");
		return ret;
	}
	lock_flag = osif_lock();
	p_entity->type = type;
	p_entity->stream_in_num = 0;
	p_entity->init = lc3_codec_init;
	p_entity->deinit = lc3_codec_deinit;
	p_entity->bt_audio_handle_media_data_packet = le_audio_handle_media_data_packet;
	p_entity->encoding_func = lc3_encoder_process_data;
	p_entity->decoding_func = lc3_decoder_process_data;
	p_entity->get_decode_buffer = lc3_decoder_buffer_get;
	p_entity->free_decode_buffer = lc3_free_decode_buffer;
	p_entity->get_encode_buffer = lc3_encoder_buffer_get;
	p_entity->free_encode_buffer = lc3_free_encode_buffer;
	p_entity->decoder_num_samples_per_frame = lc3_decoder_num_samples_per_frame;
	p_entity->decoder_num_channels = lc3_decoder_num_channels;
	p_entity->decoder_sample_rate = lc3_decoder_sample_rate;
	p_entity->encode_lock = false;
	p_entity->decode_lock = false;
	memset((void *)&p_entity->lc3, 0, sizeof(p_entity->lc3));
	osif_unlock(lock_flag);
	ret = RTK_BT_AUDIO_OK;

	return ret;
}