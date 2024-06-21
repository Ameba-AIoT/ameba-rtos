/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

/* -------------------------------- Includes -------------------------------- *//* -------------------------------- Includes -------------------------------- */
#include <platform_autoconf.h>
#include <bt_api_config.h>
#include <osif.h>
#include <dlist.h>
#include <ameba_soc.h>
#include <rtk_bt_common.h>
#include <bt_audio_intf.h>
#include <bt_audio_config.h>
#include <bt_audio_codec_wrapper.h>
#include <bt_audio_debug.h>
#include <bt_audio_sync.h>

/* -------------------------------- Defines --------------------------------- */
#define RTK_BT_AUDIO_STREAM_HANDLE_TASK_EXIT    0xFF

/* ----------------------------- static struct ------------------------------ */
struct bt_audio_intf_priv {
	uint32_t type;                    /*!< Indicate bt codec type */
	uint16_t max_track_num;           /*!< Indicate max tracek num */
	uint16_t max_record_num;          /*!< Indicate max record num */
	uint16_t curr_track_num;          /*!< Indicate current tracek num */
	uint16_t curr_record_num;         /*!< Indicate current record num */
	struct list_head track_list;      /*!< Audio track list */
	struct list_head record_list;     /*!< Audio record list */
};
struct bt_audio_codec_priv {
	uint32_t type;                    /*!< Indicate bt codec type */
	uint16_t max_entity_num;          /*!< Indicate max codec entity num */
	uint16_t curr_entity_num;         /*!< Indicate current codec entity num */
	struct list_head entity_list;     /*!< Codec entity list */
};
static struct bt_audio_intf_priv bt_audio_intf_private_table[] = {
	{RTK_BT_AUDIO_CODEC_SBC, 1, 1, 0, 0, {0}, {0}},
	{RTK_BT_AUDIO_CODEC_mSBC, 1, 1, 0, 0, {0}, {0}},
	{RTK_BT_AUDIO_CODEC_AAC, 1, 1, 0, 0, {0}, {0}},
	{RTK_BT_AUDIO_CODEC_LC3, 2, 2, 0, 0, {0}, {0}},
	{RTK_BT_AUDIO_CODEC_CVSD, 1, 1, 0, 0, {0}, {0}},
	{RTK_BT_AUDIO_CODEC_MAX, 0, 0, 0, 0, {0}, {0}},
};
static struct bt_audio_codec_priv bt_audio_codec_priv_table[] = {
	{RTK_BT_AUDIO_CODEC_SBC, 1, 0, {0}},
	{RTK_BT_AUDIO_CODEC_mSBC, 1, 0, {0}},
	{RTK_BT_AUDIO_CODEC_AAC, 1, 0, {0}},
	{RTK_BT_AUDIO_CODEC_LC3, 2, 0, {0}},
	{RTK_BT_AUDIO_CODEC_CVSD, 1, 0, {0}},
	{RTK_BT_AUDIO_CODEC_MAX, 0, 0, {0}},
};
static void *bt_audio_intf_priv_mutex = NULL;
/* ---------------------------- Global Variables ---------------------------- */
static uint8_t                      bt_audio_init_flag = 0;
static uint8_t                      audio_handle_task_running = 0;
static int8_t                       audio_stream_enqueue_num = 0;
static void                         *audio_stream_handle_task_sem = NULL;
static void                         *audio_stream_handle_task_hdl = NULL;
static void                         *audio_stream_q = NULL;
#if defined(AUDIO_RENDER_BUFFER_FLAG) && AUDIO_RENDER_BUFFER_FLAG
static uint8_t render_buffer_flag = 0;
#endif

static struct bt_audio_intf_priv *get_audio_intf_priv_data(uint32_t type)
{
	uint8_t i = 0;
	struct bt_audio_intf_priv *priv = NULL;
	/* foreach bt_audio_intf_private_table */
	while (1) {
		if (bt_audio_intf_private_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		} else if (bt_audio_intf_private_table[i].type == type) {
			priv = &bt_audio_intf_private_table[i];
			break;
		}
		i++;
	}

	return priv;
}

static struct bt_audio_codec_priv *get_audio_codec_priv_data(uint32_t type)
{
	uint8_t i = 0;
	struct bt_audio_codec_priv *priv = NULL;
	/* foreach bt_audio_codec_priv_table */
	while (1) {
		if (bt_audio_codec_priv_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		} else if (bt_audio_codec_priv_table[i].type == type) {
			priv = &bt_audio_codec_priv_table[i];
			break;
		}
		i++;
	}

	return priv;
}

static bool check_audio_track(struct bt_audio_intf_priv *priv, rtk_bt_audio_track_t *ptrack)
{
	struct list_head *plist = NULL;
	bool ret = false;

	if (!priv || !ptrack) {
		return false;
	}
	osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL);
	/* foreach priv */
	if (list_empty(&priv->track_list)) {
		goto exit;
	}
	plist = priv->track_list.next;
	while (plist != &priv->track_list) {
		if ((rtk_bt_audio_track_t *)plist == ptrack) {
			ret = true;
		}
		plist = plist->next;
	}

exit:
	osif_mutex_give(bt_audio_intf_priv_mutex);
	return ret;
}

static bool check_audio_record(struct bt_audio_intf_priv *priv, rtk_bt_audio_record_t *precord)
{
	struct list_head *plist = NULL;
	bool ret = false;

	if (!priv || !precord) {
		return false;
	}
	osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL);
	/* foreach priv */
	if (list_empty(&priv->record_list)) {
		goto exit;
	}
	plist = priv->record_list.next;
	while (plist != &priv->record_list) {
		if ((rtk_bt_audio_record_t *)plist == precord) {
			ret = true;
		}
		plist = plist->next;
	}

exit:
	osif_mutex_give(bt_audio_intf_priv_mutex);
	return ret;
}

static bool check_codec_entity(struct bt_audio_codec_priv *priv, void *pentity)
{
	struct list_head *plist = NULL;
	bool ret = false;

	if (!priv || !pentity) {
		return false;
	}
	osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL);
	/* foreach priv */
	if (list_empty(&priv->entity_list)) {
		goto exit;
	}
	plist = priv->entity_list.next;
	while (plist != &priv->entity_list) {
		if ((void *)plist == pentity) {
			ret = true;
		}
		plist = plist->next;
	}

exit:
	osif_mutex_give(bt_audio_intf_priv_mutex);
	return ret;
}

static void do_audio_track_write(rtk_bt_audio_track_t *track, uint8_t *data, uint16_t size)
{
	int32_t write_bytes;

	write_bytes = rtk_bt_audio_track_play(track->audio_track_hdl, (void *)data, size);
	/* empty buffer */
	if (write_bytes > 0) {
		track->trans_bytes += write_bytes;
	} else if (write_bytes == -32) { // OSAL_ERR_DEAD_OBJECT
		rtk_bt_audio_handle_xrun(track, data, size);
	}
}

static void do_audio_sync_flow(rtk_bt_audio_track_t *track, uint8_t packet_index, uint32_t ts_us, uint8_t **ppdata, uint32_t *pdata_size)
{
	uint16_t ret = 1;

	if (track->pre_drop_cnt_left) {
		uint32_t cnt_drop;
		/* RTAUDIO_FORMAT_PCM_16_BIT -> 2bytes */
		cnt_drop = track->pre_drop_cnt_left;
		if (cnt_drop > *pdata_size) {
			track->pre_drop_cnt_left = cnt_drop - *pdata_size;
			BT_LOGA("[BT AUDIO] drop %d data to speed up audio track rendering, left %d \r\n", (int)*pdata_size, (int)track->pre_drop_cnt_left);
			*pdata_size = 0;
		} else {
			*ppdata += cnt_drop; // offset
			*pdata_size -= cnt_drop;
			track->pre_drop_cnt_left = 0;
			BT_LOGA("[BT AUDIO] drop %d data to speed up audio track rendering \r\n", (int)cnt_drop);
			do_audio_track_write(track, *ppdata, *pdata_size);
		}
	} else {
		if (track->audio_sync_flag) {
			ret = rtk_bt_audio_presentation_compensation(track, ts_us, ppdata, pdata_size);
			if (!ret) {
				do_audio_track_write(track, *ppdata, *pdata_size);
			}
		}
		/* first frame */
		if (packet_index == 0) {
			track->prev_ts_us = ts_us;
		}
	}
}

static void bt_audio_parsing_recv_stream(uint32_t type, rtk_bt_audio_track_t *track, void *entity, uint8_t *data, uint16_t size, uint32_t ts_us)
{
	struct bt_audio_intf_priv *p_intf_priv = NULL;
	struct bt_audio_codec_priv *p_codec_priv = NULL;
	struct dec_codec_buffer *pdecode_frame_buffer = NULL;
	struct audio_param param = {0};
	struct audio_param private_param = {0};
	uint32_t frame_size = 0; //param frame_size contained within data
	uint8_t frame_num = 0;
	uint32_t handle_media_frame_num = 0;
	uint8_t codec_header_flag = 0;
	uint32_t pcm_data_size;
	uint16_t err = RTK_BT_AUDIO_FAIL;

	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		goto exit;
	}
	p_intf_priv = get_audio_intf_priv_data(type);
	if (!p_intf_priv) {
		BT_LOGE("[BT AUDIO] audio intf illegal codec type %d \r\n", (int)type);
		goto exit;
	}
	p_codec_priv = get_audio_codec_priv_data(type);
	if (!p_codec_priv) {
		BT_LOGE("[BT AUDIO] codec intf illegal codec type %d \r\n", (int)type);
		goto exit;
	}
	if (!check_audio_track(p_intf_priv, track)) {
		BT_LOGE("[BT AUDIO] %s Track handle not match \r\n", __func__);
		goto exit;
	}
	if (!check_codec_entity(p_codec_priv, entity)) {
		BT_LOGE("[BT AUDIO] Codec entity not match \r\n");
		goto exit;
	}
	while (size) {
		/* param frame number and frame size */
		if (bt_audio_handle_media_data_packet(entity, data, size, &frame_size, &frame_num, &codec_header_flag, &param)) {
			DBG_BAD("%s: media packet dismatch codec type %d ! \r\n", __func__, (int)type);
			goto exit;
		}
		DBG_BAD("%s: frame_size %d, frame_num %d, %d ! \r\n", __func__, (int)frame_size, (int)frame_num, (int)type);
		if (!frame_num) {
			break;
		}
		/* decoding */
		for (uint8_t i = 0; i < frame_num; i++) {
			pdecode_frame_buffer = bt_audio_get_decode_buffer(entity);
			if (pdecode_frame_buffer == NULL) {
				BT_LOGE("[BT_AUDIO] %d fail to get decode buffer ! \r\n", (int)type);
				continue;
			}
			err = rtk_bt_audio_decode_data((void *)entity, pdecode_frame_buffer, &data[i * frame_size + codec_header_flag], frame_size, &pcm_data_size, (void *)&param);
			if (err) {
				BT_LOGE("[BT_AUDIO] %d decode fail ! \r\n", (int)type);
				bt_audio_free_decode_buffer(entity, pdecode_frame_buffer);
				continue;
			}
			pdecode_frame_buffer->actual_write_size = pcm_data_size;
			pdecode_frame_buffer->type = type;
			private_param.channels = track->channels;
			private_param.channel_allocation = track->channel_allocation;
			private_param.rate = track->rate;
			private_param.bits = track->bits;
			if (memcmp((void *)&private_param, (void *)&param, sizeof(struct audio_param))) {
				/* change channel number or frequence */
				BT_LOGE("[BT AUDIO] received audio format mismatch \r\n");
				BT_LOGE("[BT AUDIO] private_param channels=%d,channel_allocation=%d,rate=%d,bits=%d\r\n",
						(int)private_param.channels,
						(int)private_param.channel_allocation,
						(int)private_param.rate,
						(int)private_param.bits);
				BT_LOGE("[BT AUDIO] param channels=%d,channel_allocation=%d,rate=%d,bits=%d\r\n",
						(int)param.channels,
						(int)param.channel_allocation,
						(int)param.rate,
						(int)param.bits);
			} else {
				if (track->pcm_data_cb) {
					track->pcm_data_cb(pdecode_frame_buffer->pbuffer, pcm_data_size, (void *)entity, (void *)track);
				}
				if (track->audio_track_hdl) {
					if (!track->audio_sync_flag) {
						do_audio_track_write(track, (uint8_t *)pdecode_frame_buffer->pbuffer, (uint16_t)pcm_data_size);
					} else {
						do_audio_sync_flow(track, i, ts_us, (uint8_t **)&pdecode_frame_buffer->pbuffer, &pcm_data_size);
					}
				}
			}
			/* free buffer */
			bt_audio_free_decode_buffer(entity, pdecode_frame_buffer);
		}
		data += frame_size * frame_num;
		size -= frame_size * frame_num;
		handle_media_frame_num += frame_num;
	}

exit:
	DBG_BAD("%s: Complete Frame num %d ! \r\n", __func__, handle_media_frame_num);
}

static void rtk_bt_audio_stream_handle_thread(void *ctx)
{
	(void)ctx;
	T_AUDIO_STREAM_MSG stream_msg;
	PAUDIO_CODEC_ENTITY pentity = NULL;
	rtk_bt_audio_track_t *track = NULL;
	uint32_t flags = 0;

	audio_handle_task_running = 1;
	osif_sem_give(audio_stream_handle_task_sem);

	while (audio_handle_task_running) {
		if (true == osif_msg_recv(audio_stream_q, &stream_msg, 0xffffffffUL)) {
			switch (stream_msg.type) {
			case RTK_BT_AUDIO_STREAM_HANDLE_TASK_EXIT:
				audio_handle_task_running = 0;
				break;

			default:
#if defined(AUDIO_RENDER_BUFFER_FLAG) && AUDIO_RENDER_BUFFER_FLAG
				if (render_buffer_flag) {
					while (audio_stream_enqueue_num < AUDIO_RENDER_BUFFER_SIZE) {
						osif_delay(2);
					}
					render_buffer_flag = 0;
					BT_LOGE("buffer %d numbers \r\n", audio_stream_enqueue_num);
				} else {
					if (audio_stream_enqueue_num == 1) {
						render_buffer_flag = 1;
						BT_LOGE("1 number active buffer flag \r\n");
					}
				}
#endif
				pentity = (PAUDIO_CODEC_ENTITY)stream_msg.entity;
				track = (rtk_bt_audio_track_t *)stream_msg.track;
				bt_audio_parsing_recv_stream(stream_msg.type, stream_msg.track, stream_msg.entity, stream_msg.data, stream_msg.size, stream_msg.ts_us);
				if (stream_msg.size) {
					osif_mem_free(stream_msg.data);
				}
				break;
			}
			flags = osif_lock();
			audio_stream_enqueue_num --;
			if (RTK_BT_AUDIO_STREAM_HANDLE_TASK_EXIT != stream_msg.type) {
				pentity->track_num --;
				track->track_num --;
			}
			osif_unlock(flags);
		}
	}
	/* free all mallocated audio stream buffer enqueued */
	while (audio_stream_enqueue_num > 0) {
		if (true == osif_msg_recv(audio_stream_q, &stream_msg, 0xffffffffUL)) {
			if (stream_msg.size) {
				osif_mem_free(stream_msg.data);
			}
			flags = osif_lock();
			audio_stream_enqueue_num --;
			if (RTK_BT_AUDIO_STREAM_HANDLE_TASK_EXIT != stream_msg.type) {
				pentity = (PAUDIO_CODEC_ENTITY)stream_msg.entity;
				track = (rtk_bt_audio_track_t *)stream_msg.track;
				pentity->track_num --;
				track->track_num --;
			}
			osif_unlock(flags);
		}
	}

	BT_LOGA("[BT AUDIO] bt audio stream handle task exit\r\n");
	osif_sem_give(audio_stream_handle_task_sem);
	osif_task_delete(NULL);
}

static uint16_t bt_audio_msg_send(uint32_t type, rtk_bt_audio_track_t *track, void *entity, void *pdata, uint16_t size, uint32_t ts_us)
{
	static uint32_t stream_overflow_cout = 0;
	T_AUDIO_STREAM_MSG stream_msg;
	uint32_t flags = 0;

	stream_msg.data = pdata;
	stream_msg.size = size;
	stream_msg.type = type;
	stream_msg.track = track;
	stream_msg.entity = entity;
	stream_msg.ts_us = ts_us;

	while (audio_stream_enqueue_num >= AUDIO_STREAM_MSG_QUEUE_SIZE) {
		// BT_LOGE("%s:enqueue stream data too fast %d %d %d ! \r\n", __func__, (int)audio_stream_enqueue_num, (int)stream_overflow_cout, (int)type);
		osif_delay(5);
		stream_overflow_cout ++;
	}
	stream_overflow_cout = 0;
	if (false == osif_msg_send(audio_stream_q, &stream_msg, 0)) {
		return RTK_BT_AUDIO_FAIL;
	}
	flags = osif_lock();
	audio_stream_enqueue_num ++;
	osif_unlock(flags);

	return RTK_BT_AUDIO_OK;
}

static uint16_t bt_audio_app_data_handle_init(void)
{
	if (false == osif_sem_create(&audio_stream_handle_task_sem, 0, 1)) {
		goto failed;
	}
	if (false == osif_msg_queue_create(&audio_stream_q, AUDIO_STREAM_MSG_QUEUE_SIZE, sizeof(T_AUDIO_STREAM_MSG))) {
		goto failed;
	}
#if defined(AUDIO_RENDER_BUFFER_FLAG) && AUDIO_RENDER_BUFFER_FLAG
	render_buffer_flag = 1;
#endif
	audio_stream_enqueue_num = 0;
	audio_handle_task_running = 0;
	if (false == osif_task_create(&audio_stream_handle_task_hdl, "bt_audio_stream_task", rtk_bt_audio_stream_handle_thread, NULL,
								  AUDIO_STREAM_TASK_STACK_SIZE, AUDIO_STREAM_TASK_PRIORITY)) {
		goto failed;
	}
	/* Wait until audio stream handle task is running */
	if (false == osif_sem_take(audio_stream_handle_task_sem, 0xffffffffUL)) {
		goto failed;
	}

	return RTK_BT_AUDIO_OK;

failed:
	if (audio_stream_handle_task_hdl) {
		osif_task_delete(audio_stream_handle_task_hdl);
	}
	if (audio_stream_handle_task_sem) {
		osif_sem_delete(audio_stream_handle_task_sem);
	}
	if (audio_stream_q) {
		osif_msg_queue_delete(audio_stream_q);
	}

	return RTK_BT_AUDIO_FAIL;
}

static uint16_t bt_audio_app_data_handle_deinit(void)
{
	/* indicate bt audio stream handle task to kill itself */
	if (bt_audio_msg_send(RTK_BT_AUDIO_STREAM_HANDLE_TASK_EXIT, NULL, NULL, NULL, 0, 0)) {
		return RTK_BT_AUDIO_FAIL;
	}

	if (false == osif_sem_take(audio_stream_handle_task_sem, 0xffffffffUL)) {
		return RTK_BT_AUDIO_FAIL;
	}
	osif_sem_delete(audio_stream_handle_task_sem);
	osif_msg_queue_delete(audio_stream_q);
	audio_stream_handle_task_sem = NULL;
	audio_stream_handle_task_hdl = NULL;
	audio_stream_q = NULL;

	return 0;
}

static uint16_t bt_audio_codec_init(rtk_bt_audio_codec_conf_t *paudio_codec_conf, PAUDIO_CODEC_ENTITY pentity)
{
	uint16_t err = RTK_BT_AUDIO_FAIL;
	uint32_t codec_index = 0;

	if (!paudio_codec_conf || !pentity) {
		BT_LOGE("[BT_AUDIO] audio configuration or entity is empty \r\n");
		return err;
	}
	codec_index = paudio_codec_conf->codec_index;

#if defined(CONFIG_BT_AUDIO_CODEC_SBC) && CONFIG_BT_AUDIO_CODEC_SBC
	/* sbc codec */
	if (RTK_BT_AUDIO_CODEC_SBC == codec_index) {
		BT_LOGE("[BT_AUDIO] sbc codec init  \r\n");
		err = bt_audio_register_codec(RTK_BT_AUDIO_CODEC_SBC, paudio_codec_conf->param, paudio_codec_conf->param_len, pentity);
	}
#endif
#if defined(CONFIG_BT_AUDIO_CODEC_AAC) && CONFIG_BT_AUDIO_CODEC_AAC
	/* aac codec */
	if (RTK_BT_AUDIO_CODEC_AAC == codec_index) {
		BT_LOGE("[BT_AUDIO] aac codec init  \r\n");
		err = bt_audio_register_codec(RTK_BT_AUDIO_CODEC_AAC, paudio_codec_conf->param, paudio_codec_conf->param_len, pentity);
	}
#endif
#if defined(CONFIG_BT_AUDIO_CODEC_LC3) && CONFIG_BT_AUDIO_CODEC_LC3
	/* lc3 codec */
	if (RTK_BT_AUDIO_CODEC_LC3 == codec_index) {
		BT_LOGE("[BT_AUDIO] lc3 codec init  \r\n");
		err = bt_audio_register_codec(RTK_BT_AUDIO_CODEC_LC3, paudio_codec_conf->param, paudio_codec_conf->param_len, pentity);
	}
#endif
#if defined(CONFIG_BT_AUDIO_CODEC_CVSD) && CONFIG_BT_AUDIO_CODEC_CVSD
	/* cvsd codec */
	if (RTK_BT_AUDIO_CODEC_CVSD == codec_index) {
		BT_LOGE("[BT_AUDIO] cvsd codec init  \r\n");
		err = bt_audio_register_codec(RTK_BT_AUDIO_CODEC_CVSD, paudio_codec_conf->param, paudio_codec_conf->param_len, pentity);
	}
#endif

	return err;
}

static uint16_t bt_audio_codec_deinit(uint32_t codec_index, PAUDIO_CODEC_ENTITY pentity)
{
	uint16_t err = RTK_BT_AUDIO_FAIL;

	if (!pentity) {
		BT_LOGE("[BT_AUDIO] Codec entity is empty \r\n");
		return err;
	}
#if defined(CONFIG_BT_AUDIO_CODEC_SBC) && CONFIG_BT_AUDIO_CODEC_SBC
	/* sbc codec */
	if (RTK_BT_AUDIO_CODEC_SBC == codec_index) {
		BT_LOGE("[BT_AUDIO] sbc codec deinit  \r\n");
		err = bt_audio_unregister_codec(RTK_BT_AUDIO_CODEC_SBC, pentity);
	}
#endif
#if defined(CONFIG_BT_AUDIO_CODEC_AAC) && CONFIG_BT_AUDIO_CODEC_AAC
	/* aac codec */
	if (RTK_BT_AUDIO_CODEC_AAC == codec_index) {
		BT_LOGE("[BT_AUDIO] aac codec deinit  \r\n");
		err = bt_audio_unregister_codec(RTK_BT_AUDIO_CODEC_AAC, pentity);
	}
#endif
#if defined(CONFIG_BT_AUDIO_CODEC_LC3) && CONFIG_BT_AUDIO_CODEC_LC3
	/* lc3 codec */
	if (RTK_BT_AUDIO_CODEC_LC3 == codec_index) {
		BT_LOGE("[BT_AUDIO] lc3 codec deinit  \r\n");
		err = bt_audio_unregister_codec(RTK_BT_AUDIO_CODEC_LC3, pentity);
	}
#endif
#if defined(CONFIG_BT_AUDIO_CODEC_CVSD) && CONFIG_BT_AUDIO_CODEC_CVSD
	/* cvsd codec */
	if (RTK_BT_AUDIO_CODEC_CVSD == codec_index) {
		BT_LOGE("[BT_AUDIO] cvsd codec deinit  \r\n");
		err = bt_audio_unregister_codec(RTK_BT_AUDIO_CODEC_CVSD, pentity);
	}
#endif

	return err;
}

struct enc_codec_buffer *rtk_bt_audio_data_encode(uint32_t type, void *entity, int16_t *pdata, uint32_t len)
{
	struct bt_audio_codec_priv *priv = NULL;
	struct enc_codec_buffer *pencoder_buffer = NULL;
	PAUDIO_CODEC_ENTITY pentity = (PAUDIO_CODEC_ENTITY)entity;
	uint8_t frame_num = 0;
	uint32_t actual_len = 0;
	uint32_t flags = 0;

	if (!entity) {
		BT_LOGE("%s: empty entity handle \r\n", __func__);
		return NULL;
	} else {
		flags = osif_lock();
		pentity->encode_num ++;
		osif_unlock(flags);
	}
	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		goto exit;
	}
	priv = get_audio_codec_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		goto exit;
	}
	if (!check_codec_entity(priv, entity)) {
		BT_LOGE("[BT AUDIO] %s Track handle not match \r\n", __func__);
		goto exit;
	}
	pencoder_buffer = bt_audio_get_encode_buffer((PAUDIO_CODEC_ENTITY)entity);
	if (!pencoder_buffer) {
		BT_LOGE("[BT_AUDIO] get audio encode buffer fail \r\n");
		goto exit;
	}
	if (bt_audio_encode_process_data((PAUDIO_CODEC_ENTITY)entity, pencoder_buffer, pdata, len, &frame_num, &actual_len)) {
		BT_LOGE("[BT_AUDIO] bt_audio_encode_process_data fail \r\n");
		bt_audio_free_encode_buffer((PAUDIO_CODEC_ENTITY)entity, pencoder_buffer);
		pencoder_buffer = NULL;
	}

exit:
	flags = osif_lock();
	pentity->encode_num --;
	osif_unlock(flags);
	return pencoder_buffer;
}

uint16_t rtk_bt_audio_free_encode_buffer(uint32_t type, void *entity, struct enc_codec_buffer *pbuffer)
{
	struct bt_audio_codec_priv *priv = NULL;

	if (!entity) {
		BT_LOGE("%s: empty entity handle \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	priv = get_audio_codec_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return RTK_BT_AUDIO_FAIL;
	}
	if (!check_codec_entity(priv, entity)) {
		BT_LOGE("[BT AUDIO] %s Track handle not match \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}

	return bt_audio_free_encode_buffer((PAUDIO_CODEC_ENTITY)entity, pbuffer);
}

/* this api will be invoked by rtk_bt_audio_recved_data_in */
/* app can modify this api to use vendor decoder */
uint16_t rtk_bt_audio_decode_data(void *entity, void *pparam, uint8_t *data, uint32_t length, uint32_t *ppcm_data_size, void *paudio_param)
{
	uint16_t ret = RTK_BT_AUDIO_FAIL;
	PAUDIO_CODEC_ENTITY pentity = (PAUDIO_CODEC_ENTITY)entity;
	uint32_t flags = 0;

	if (!pparam || !pentity) {
		BT_LOGE("[BT_AUDIO] pparam is NULL ! \r\n");
		return RTK_BT_AUDIO_FAIL;
	} else {
		flags = osif_lock();
		pentity->decode_num ++;
		osif_unlock(flags);
	}
	ret = bt_audio_decode_process_data((PAUDIO_CODEC_ENTITY)entity, (struct dec_codec_buffer *)pparam, data, length, ppcm_data_size,
									   (struct audio_param *)paudio_param);
	flags = osif_lock();
	pentity->decode_num --;
	osif_unlock(flags);

	return ret;
}

uint16_t rtk_bt_audio_recvd_data_in(uint32_t type, rtk_bt_audio_track_t *track, void *entity, uint8_t *pdata, uint32_t len, uint32_t ts_us)
{
	uint8_t *pdata_buffer = NULL;
	struct bt_audio_intf_priv *p_intf_priv = NULL;
	struct bt_audio_codec_priv *p_codec_priv = NULL;
	PAUDIO_CODEC_ENTITY pentity = (PAUDIO_CODEC_ENTITY)entity;
	uint32_t flags = 0;

	if (!track || !pentity) {
		return RTK_BT_AUDIO_FAIL;
	} else {
		flags = osif_lock();
		track->track_num ++;
		pentity->track_num ++;
		osif_unlock(flags);
	}
	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		goto exit;
	}
	p_intf_priv = get_audio_intf_priv_data(type);
	if (!p_intf_priv) {
		BT_LOGE("[BT AUDIO] intf priv illegal codec type %d \r\n", (int)type);
		goto exit;
	}
	p_codec_priv = get_audio_codec_priv_data(type);
	if (!p_codec_priv) {
		BT_LOGE("[BT AUDIO] codec priv illegal codec type %d \r\n", (int)type);
		goto exit;
	}
	if (!check_audio_track(p_intf_priv, track)) {
		BT_LOGE("[BT AUDIO] %s Track handle not match \r\n", __func__);
		goto exit;
	}
	if (!check_codec_entity(p_codec_priv, pentity)) {
		BT_LOGE("[BT AUDIO] %s Codec entity not match \r\n", __func__);
		goto exit;
	}
	/* memcpying data if receiving data from ipc helps to free ipc resources */
	pdata_buffer = osif_mem_alloc(RAM_TYPE_DATA_ON, len);
	if (!pdata_buffer) {
		goto exit;
	} else {
		memset((void *)pdata_buffer, 0, len);
	}
	memcpy((void *)pdata_buffer, (void *)pdata, len);
	if (ts_us && !track->audio_sync_flag) {
		track->audio_sync_flag = true;
	}
	if (bt_audio_msg_send(type, track, pentity, pdata_buffer, len, ts_us)) {
		osif_mem_free(pdata_buffer);
		goto exit;
	}
	return RTK_BT_AUDIO_OK;

exit:
	flags = osif_lock();
	track->track_num --;
	pentity->track_num --;
	osif_unlock(flags);
	return RTK_BT_AUDIO_FAIL;
}

int rtk_bt_audio_record_data_get(uint32_t type, rtk_bt_audio_record_t *record, void *entity, void *buffer, int size, bool blocking)
{
	struct bt_audio_intf_priv *priv = NULL;
	PAUDIO_CODEC_ENTITY pentity = (PAUDIO_CODEC_ENTITY)entity;
	int readsize = 0;
	uint32_t flags = 0;

	if (!record || !buffer || !pentity) {
		BT_LOGE("%s: empty record or buffer \r\n", __func__);
		return 0;
	} else {
		flags = osif_lock();
		record->record_num ++;
		pentity->record_num ++;
		osif_unlock(flags);
	}
	if (!record->audio_record_hdl) {
		BT_LOGE("%s: record hdl null \r\n", __func__);
		goto exit;
	}
	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		goto exit;
	}
	priv = get_audio_intf_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		goto exit;
	}
	if (!check_audio_record(priv, record)) {
		BT_LOGE("[BT AUDIO] record handle not match \r\n");
		goto exit;
	}
	readsize = rtk_bt_audio_record_read(record->audio_record_hdl, buffer, size, blocking);

exit:
	flags = osif_lock();
	record->record_num --;
	pentity->record_num --;
	osif_unlock(flags);
	return readsize;
}

rtk_bt_audio_track_t *rtk_bt_get_audio_track(void *timer, uint32_t type)
{
	struct bt_audio_intf_priv *priv = NULL;
	rtk_bt_audio_track_t *ptrack = NULL;
	struct list_head *plist = NULL;

	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		return ptrack;
	}
	priv = get_audio_intf_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return ptrack;
	}
	if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
		BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
		return ptrack;
	}
	/* foreach priv */
	if (list_empty(&priv->track_list)) {
		goto exit;
	}
	plist = priv->track_list.next;
	while (plist != &priv->track_list) {
		rtk_bt_audio_track_t *track_tmp = (rtk_bt_audio_track_t *)plist;
		if (track_tmp->audio_delay_start_timer == timer) {
			ptrack = track_tmp;
			goto exit;
		}
		plist = plist->next;
	}

exit:
	osif_mutex_give(bt_audio_intf_priv_mutex);
	return ptrack;
}

rtk_bt_audio_track_t *rtk_bt_audio_track_add(uint32_t type, float left_volume, float right_volume, uint32_t channels, uint32_t rate, uint32_t format,
											 uint32_t duration, pcm_data_cb cb, bool play_flag)
{
	struct bt_audio_intf_priv *priv = NULL;
	rtk_bt_audio_track_t *ptrack = NULL;

	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		return NULL;
	}
	priv = get_audio_intf_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return NULL;
	}
#if defined(CONFIG_AUDIO_PASSTHROUGH) && CONFIG_AUDIO_PASSTHROUGH
	if (priv->curr_track_num > 0) {
		BT_LOGE("[BT AUDIO] Audio Framework Passthrough only allow one audio track \r\n");
		return NULL;
	}
	BT_LOGE("[BT AUDIO] Audio Framework Passthrough try to add one audio track \r\n");
#elif defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	if (priv->curr_track_num == priv->max_track_num) {
		BT_LOGE("[BT AUDIO] Audio Framework Mixer has no more remaing track num, curr audio track num is %d \r\n", (int)priv->curr_track_num);
		return NULL;
	}
	BT_LOGE("[BT AUDIO] Audio Framework Mixer try to add one audio track \r\n");
#else
	BT_LOGE("[BT AUDIO] Audio Framework need to be enable for bt audio(Mixer or Passthrough) \r\n");

	return NULL;
#endif
	ptrack = (rtk_bt_audio_track_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_audio_track_t));
	if (!ptrack) {
		BT_LOGE("[BT AUDIO] Allocate bt audio track handle fail \r\n");
		return NULL;
	} else {
		memset((void *)ptrack, 0, sizeof(rtk_bt_audio_track_t));
		INIT_LIST_HEAD(&ptrack->list);
	}
	BT_LOGE("[BT AUDIO] audio track init audio channels %d, rate %d, format %d, left_volume %.2f, right_volume %.2f ! \r\n",
			(int)channels,
			(int)rate,
			(int)format,
			left_volume,
			right_volume);
	if (cb) {
		ptrack->pcm_data_cb = cb;
		BT_LOGE("[BT AUDIO] pcm data callback is registered \r\n");
	} else {
		ptrack->pcm_data_cb = NULL;
		BT_LOGE("[BT AUDIO] pcm data callback is NULL \r\n");
	}
	osif_mutex_create(&ptrack->audio_sync_mutex);
	if (!ptrack->audio_sync_mutex) {
		BT_LOGE("[BT AUDIO] ptrack->audio_sync_mutex create failed!\r\n");
		osif_mem_free(ptrack);
		return NULL;
	}
	if (play_flag) {
		ptrack->audio_track_hdl = rtk_bt_audio_track_init((uint32_t)channels, (uint32_t)rate, (uint32_t)format, 1024, 0, duration);
		if (!ptrack->audio_track_hdl) {
			BT_LOGE("[BT AUDIO] rtk_bt_audio_track_init fail \r\n");
			osif_mutex_delete(ptrack->audio_sync_mutex);
			osif_mem_free(ptrack);
			return NULL;
		}
		BT_LOGE("[BT AUDIO] Complete allocating audio track \r\n");
	} else {
		ptrack->audio_track_hdl = NULL;
		BT_LOGE("[BT AUDIO] audio_track_hdl is NULL \r\n");
	}
	ptrack->channels = channels;
	ptrack->rate = rate;
	ptrack->format = format;
	switch (format) {
	case BT_AUDIO_FORMAT_PCM_8_BIT:
		ptrack->bits = 8;
		break;
	case BT_AUDIO_FORMAT_PCM_16_BIT:
		ptrack->bits = 16;
		break;
	case BT_AUDIO_FORMAT_PCM_32_BIT:
		ptrack->bits = 32;
		break;
	default:
		ptrack->bits = 16;
		break;
	}
	if (channels == 1) {
		if (left_volume == 0) {
			ptrack->channel_allocation = 2;
		} else if (right_volume == 0) {
			ptrack->channel_allocation = 1;
		} else {
			BT_LOGE("[BT AUDIO] Wrong volume allocation \r\n");
		}
	} else if (channels == 2) {
		ptrack->channel_allocation = 3;
	}
	if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
		BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
		rtk_bt_audio_track_deinit(ptrack->audio_track_hdl);
		osif_mutex_delete(ptrack->audio_sync_mutex);
		osif_mem_free(ptrack);
		return NULL;
	}
	list_add_tail(&ptrack->list, &priv->track_list);
	priv->curr_track_num ++;
	osif_mutex_give(bt_audio_intf_priv_mutex);
	if (ptrack->audio_track_hdl) {
		rtk_bt_audio_track_start(ptrack->audio_track_hdl, left_volume, right_volume);
	}

	return ptrack;
}

rtk_bt_audio_record_t *rtk_bt_audio_record_add(uint32_t type, uint32_t channels, uint32_t rate, uint32_t buffer_bytes)
{
	struct bt_audio_intf_priv *priv = NULL;
	rtk_bt_audio_record_t *precord = NULL;

	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		return NULL;
	}
	priv = get_audio_intf_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return NULL;
	}
#if defined(CONFIG_AUDIO_PASSTHROUGH) && CONFIG_AUDIO_PASSTHROUGH
	if (priv->curr_record_num > 0) {
		BT_LOGE("[BT AUDIO] Audio Framework Passthrough only allow one audio record \r\n");
		return NULL;
	}
	BT_LOGE("[BT AUDIO] Audio Framework Passthrough try to add one audio record \r\n");
#elif defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	if (priv->curr_record_num == priv->max_record_num) {
		BT_LOGE("[BT AUDIO] Audio Framework Mixer has no more remaing record num, curr audio record num is %d \r\n", (int)priv->curr_record_num);
		return NULL;
	}
	BT_LOGE("[BT AUDIO] Audio Framework Mixer try to add one audio record \r\n");
#else
	BT_LOGE("[BT AUDIO] Audio Framework need to be enable for bt audio(Mixer or Passthrough) \r\n");

	return NULL;
#endif
	precord = (rtk_bt_audio_record_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_bt_audio_record_t));
	if (!precord) {
		BT_LOGE("[BT AUDIO] Allocate bt audio record handle fail \r\n");
		return NULL;
	} else {
		memset((void *)precord, 0, sizeof(rtk_bt_audio_record_t));
		INIT_LIST_HEAD(&precord->list);
	}
	BT_LOGE("[BT AUDIO] audio record init channels %d, rate %d , buffer_bytes %d! \r\n", (int)channels, (int)rate, (uint32_t)buffer_bytes);
	precord->audio_record_hdl = rtk_bt_audio_record_init((uint32_t)channels, (uint32_t)rate, (uint32_t)buffer_bytes);
	if (!precord->audio_record_hdl) {
		BT_LOGE("[BT AUDIO] rtk_bt_audio_record_init fail \r\n");
		osif_mem_free(precord);
		return NULL;
	} else {
		precord->channels = channels;
		precord->rate = rate;
		precord->buffer_bytes = buffer_bytes;
		precord->record_num = 0;
		if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
			BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
			rtk_bt_audio_record_deinit(precord->audio_record_hdl);
			osif_mem_free(precord);
			return NULL;
		}
		list_add_tail(&precord->list, &priv->record_list);
		priv->curr_record_num ++;
		osif_mutex_give(bt_audio_intf_priv_mutex);
	}
	rtk_bt_audio_record_set_parameters(precord->audio_record_hdl, "ch0_sel_amic=1");
	rtk_bt_audio_record_set_capture_volume(channels, 0x2f);
	rtk_bt_audio_record_set_mic_bst_gain(RTK_BT_AUDIO_AMIC1, RTK_BT_AUDIO_MICBST_GAIN_40DB);

	return precord;
}

uint16_t rtk_bt_audio_track_del(uint32_t type, rtk_bt_audio_track_t *ptrack)
{
	struct bt_audio_intf_priv *priv = NULL;

	if (!ptrack) {
		BT_LOGE("[BT_AUDIO] Ptrack is NULL \r\n");
		return RTK_BT_AUDIO_FAIL;
	}
	while (ptrack->track_num) {
		BT_LOGE("[BT AUDIO] track num %d \r\n", (int)ptrack->track_num);
		osif_delay(5);
	}
	priv = get_audio_intf_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return RTK_BT_AUDIO_FAIL;
	}
	if (!check_audio_track(priv, ptrack)) {
		BT_LOGE("[BT AUDIO] %s Track handle not match \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	if (ptrack->audio_track_hdl) {
		rtk_bt_audio_track_stop(ptrack->audio_track_hdl);
		rtk_bt_audio_track_deinit(ptrack->audio_track_hdl);
	} else if (ptrack->pcm_data_cb) {
		ptrack->pcm_data_cb = NULL;
	}
	if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
		BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	list_del(&ptrack->list);
	priv->curr_track_num --;
	if (ptrack->audio_delay_start_timer) {
		osif_timer_delete(&ptrack->audio_delay_start_timer);
	}
	if (ptrack->audio_sync_mutex) {
		osif_mutex_delete(ptrack->audio_sync_mutex);
	}
	osif_mem_free(ptrack);
	osif_mutex_give(bt_audio_intf_priv_mutex);

	return RTK_BT_AUDIO_OK;
}

uint16_t rtk_bt_audio_track_del_all(void)
{
	uint8_t i = 0;
	struct bt_audio_intf_priv *priv = NULL;
	rtk_bt_audio_track_t *ptrack = NULL;
	struct list_head *plist = NULL;

	while (1) {
		if (bt_audio_intf_private_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		}
		priv = &bt_audio_intf_private_table[i];
		if (list_empty(&priv->track_list)) {
			i ++;
			continue;
		}
		plist = priv->track_list.next;
		while (plist != &priv->track_list) {
			ptrack = (rtk_bt_audio_track_t *)plist;
			rtk_bt_audio_track_del(priv->type, ptrack);
			plist = priv->track_list.next;
		}
		i ++;
	}

	return RTK_BT_AUDIO_OK;
}

uint16_t rtk_bt_audio_record_del(uint32_t type, rtk_bt_audio_record_t *precord)
{
	struct bt_audio_intf_priv *priv = NULL;

	if (!precord) {
		BT_LOGE("[BT_AUDIO] Precord is NULL \r\n");
		return RTK_BT_AUDIO_FAIL;
	}
	while (precord->record_num) {
		BT_LOGE("[BT AUDIO] record num %d \r\n", (int)precord->record_num);
		osif_delay(5);
	}
	priv = get_audio_intf_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return RTK_BT_AUDIO_FAIL;
	}
	if (!check_audio_record(priv, precord)) {
		BT_LOGE("[BT AUDIO] Record handle not match \r\n");
		return RTK_BT_AUDIO_FAIL;
	}
	rtk_bt_audio_record_stop(precord->audio_record_hdl);
	rtk_bt_audio_record_deinit(precord->audio_record_hdl);
	if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
		BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	list_del(&precord->list);
	priv->curr_record_num --;
	osif_mem_free(precord);
	osif_mutex_give(bt_audio_intf_priv_mutex);

	return RTK_BT_AUDIO_OK;
}

uint16_t rtk_bt_audio_record_del_all(void)
{
	uint8_t i = 0;
	struct bt_audio_intf_priv *priv = NULL;
	rtk_bt_audio_record_t *precord = NULL;
	struct list_head *plist = NULL;

	while (1) {
		if (bt_audio_intf_private_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		}
		priv = &bt_audio_intf_private_table[i];
		if (list_empty(&priv->record_list)) {
			i ++;
			continue;
		}
		plist = priv->record_list.next;
		while (plist != &priv->record_list) {
			precord = (rtk_bt_audio_record_t *)plist;
			rtk_bt_audio_record_del(priv->type, precord);
			plist = priv->record_list.next;
		}
		i ++;
	}

	return RTK_BT_AUDIO_OK;
}

void *rtk_bt_audio_codec_add(rtk_bt_audio_codec_conf_t *paudio_codec_conf)
{
	struct bt_audio_codec_priv *priv = NULL;
	PAUDIO_CODEC_ENTITY pentity = NULL;
	uint32_t type = 0;

	if (!paudio_codec_conf) {
		BT_LOGE("[BT AUDIO] paudio_codec_conf is null \r\n");
		return NULL;
	}
	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] BT audio has not be initialized \r\n");
		return NULL;
	}
	type = paudio_codec_conf->codec_index;
	priv = get_audio_codec_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return NULL;
	}
	if (priv->curr_entity_num == priv->max_entity_num) {
		BT_LOGE("[BT AUDIO] Has no more remaing entity num, curr codec entity num is %d \r\n", (int)priv->curr_entity_num);
		return NULL;
	}
	BT_LOGE("[BT AUDIO] BT codec try to add one codec \r\n");
	pentity = (PAUDIO_CODEC_ENTITY)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(AUDIO_CODEC_ENTITY));
	if (!pentity) {
		BT_LOGE("[BT AUDIO] Allocate bt audio codec entity fail \r\n");
		return NULL;
	} else {
		memset((void *)pentity, 0, sizeof(AUDIO_CODEC_ENTITY));
		INIT_LIST_HEAD(&pentity->list);
	}
	BT_LOGE("[BT AUDIO] add codec type %d ! \r\n", (int)type);
	/* init codec entity */
	if (bt_audio_codec_init(paudio_codec_conf, pentity)) {
		BT_LOGE("[BT AUDIO] Fail to add codec type%d ! \r\n", (int)type);
		osif_mem_free(pentity);
		return NULL;
	}
	if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
		BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
		osif_mem_free(pentity);
		return NULL;
	}
	list_add_tail(&pentity->list, &priv->entity_list);
	priv->curr_entity_num ++;
	osif_mutex_give(bt_audio_intf_priv_mutex);

	return (void *)pentity;
}

uint16_t rtk_bt_audio_codec_remove(uint32_t type, void *pentity)
{
	struct bt_audio_codec_priv *priv = NULL;
	PAUDIO_CODEC_ENTITY entity = NULL;

	priv = get_audio_codec_priv_data(type);
	if (!priv) {
		BT_LOGE("[BT AUDIO] Illegal codec type %d \r\n", (int)type);
		return RTK_BT_AUDIO_FAIL;
	}
	if (!pentity) {
		BT_LOGE("[BT AUDIO] Pentity is null \r\n");
		return RTK_BT_AUDIO_FAIL;
	}
	entity = (PAUDIO_CODEC_ENTITY)pentity;
	while (entity->encode_num || entity->decode_num || entity->track_num || entity->record_num) {
		BT_LOGE("[BT AUDIO] tarck num %d record num %d \r\n", (int)entity->track_num, (int)entity->record_num);
		osif_delay(5);
	}
	if (!check_codec_entity(priv, entity)) {
		BT_LOGE("[BT AUDIO] Codec entity not match \r\n");
		return RTK_BT_AUDIO_FAIL;
	}
	bt_audio_codec_deinit(type, entity);
	if (!osif_mutex_take(bt_audio_intf_priv_mutex, 0xFFFFFFFFUL)) {
		BT_LOGE("[BT_AUDIO] %s get mutex failed \r\n", __func__);
		return RTK_BT_AUDIO_FAIL;
	}
	list_del(&entity->list);
	priv->curr_entity_num --;
	osif_mem_free(entity);
	osif_mutex_give(bt_audio_intf_priv_mutex);

	return RTK_BT_AUDIO_OK;
}

uint16_t rtk_bt_audio_codec_remove_all(void)
{
	uint8_t i = 0;
	struct bt_audio_codec_priv *priv = NULL;
	PAUDIO_CODEC_ENTITY entity = NULL;
	struct list_head *plist = NULL;

	while (1) {
		if (bt_audio_codec_priv_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		}
		priv = &bt_audio_codec_priv_table[i];
		if (list_empty(&priv->entity_list)) {
			i ++;
			continue;
		}
		plist = priv->entity_list.next;
		while (plist != &priv->entity_list) {
			entity = (PAUDIO_CODEC_ENTITY)plist;
			rtk_bt_audio_codec_remove(priv->type, entity);
			plist = priv->entity_list.next;
		}
		i ++;
	}

	return RTK_BT_AUDIO_OK;
}

uint16_t rtk_bt_audio_init(void)
{
	uint16_t err = RTK_BT_AUDIO_FAIL;
	uint8_t i = 0;

	/* judge whether already initialized */
	if (bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] Already be initialized \r\n");
		return RTK_BT_AUDIO_OK;
	}
	/* init audio stream handle task */
	err = bt_audio_app_data_handle_init();
	if (err) {
		return err;
	}
	/* foreach bt_audio_intf_private_table and init list */
	while (1) {
		if (bt_audio_intf_private_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		}
		INIT_LIST_HEAD(&bt_audio_intf_private_table[i].track_list);
		INIT_LIST_HEAD(&bt_audio_intf_private_table[i].record_list);
		i++;
	}
	/* foreach bt_audio_intf_private_table and init list */
	i = 0;
	while (1) {
		if (bt_audio_codec_priv_table[i].type == RTK_BT_AUDIO_CODEC_MAX) {
			break;
		}
		INIT_LIST_HEAD(&bt_audio_codec_priv_table[i].entity_list);
		i++;
	}
	if (false == osif_mutex_create(&bt_audio_intf_priv_mutex)) {
		BT_LOGE("[BT AUDIO] audio intf priv mutex create fail \r\n");
		err = RTK_BT_AUDIO_FAIL;
	}
	bt_audio_init_flag = 1;

	return err;
}

uint16_t rtk_bt_audio_deinit(void)
{
	uint16_t err = RTK_BT_AUDIO_FAIL;

	/* judge whether already initialized */
	if (!bt_audio_init_flag) {
		BT_LOGE("[BT_AUDIO] No need to do deinit \r\n");
		return RTK_BT_AUDIO_FAIL;
	} else {
		bt_audio_init_flag = 0;
	}
	/* delete all audio track */
	rtk_bt_audio_track_del_all();
	/* delete all audio record */
	rtk_bt_audio_record_del_all();
	/* delete all codec entity */
	rtk_bt_audio_codec_remove_all();
	/* deinit audio stream handle task */
	err = bt_audio_app_data_handle_deinit();
	if (err) {
		BT_LOGE("[BT_AUDIO] bt_audio_app_data_handle_deinit fail \r\n");
		return err;
	}
	osif_mutex_delete(bt_audio_intf_priv_mutex);
	bt_audio_intf_priv_mutex = NULL;
	BT_LOGA("[BT_AUDIO] deinit complete ! \r\n");

	return err;
}
