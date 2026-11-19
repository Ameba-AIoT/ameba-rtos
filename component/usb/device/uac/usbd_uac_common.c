/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* Shared by the UAC 1.0 and UAC 2.0 drivers, one of which is built per image */
#include "usbd_uac_priv.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif

/* Private defines -----------------------------------------------------------*/

#if USBD_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME     (10U * 1000U)
#define USBD_UAC_DEBUG_STEP_TIME     100U
#endif
/* Period of the warning usbd_uac_stream_deinit() logs while a ring user does not leave */
#define USBD_UAC_USER_EXIT_WARN_MS   1000U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static inline void usbd_uac_count_read_data(u32 audio_len);
static int usbd_uac_ring_read(usbd_uac_stream_t *stream, u8 *buffer, u32 size, u32 *copy_len, u16 *packet_index, u32 *zero_pkt_flag);
static u32 usbd_uac_read_ring(usbd_uac_stream_t *stream, u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag);
static u16 usbd_uac_get_ring_node_count(u8 speed, u8 is_record);
static int usbd_uac_stream_init(usbd_uac_stream_t *stream, u8 speed);
static int usbd_uac_private_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg);
static int usbd_uac_dma_buf_alloc(usbd_uac_stream_t *stream);
static void usbd_uac_dma_buf_free(void);
#if USBD_UAC_DEBUG
static void usbd_uac_status_dump_thread(void *param);
#endif
#ifdef CONFIG_USBD_UAC_RECORD
static void usbd_uac_isoc_in_transmit_complete(usb_dev_t *dev);
static int usbd_uac_record_packetizer_init(u8 speed, u16 mps);
static void usbd_uac_record_packetizer_deinit(void);
static void usbd_uac_record_packetizer_reset(void);
static void usbd_uac_record_packetizer_next(void);
#endif

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static const u8 usbd_uac_host_volume_levels[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static const s16 usbd_uac_driver_volumes[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};
/* Both volume tables are indexed together by usbd_uac_volume_linear_interpolation() */
_Static_assert((sizeof(usbd_uac_host_volume_levels) / sizeof(usbd_uac_host_volume_levels[0])) == (sizeof(usbd_uac_driver_volumes) / sizeof(
				   usbd_uac_driver_volumes[0])),
			   "UAC volume tables differ in length");

/* Exported variables --------------------------------------------------------*/

/* UAC Device */
usbd_uac_dev_t usbd_uac_dev;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Update or manage the count of audio data bytes
  * @param  audio_len: Data len had been wrote to the audiotrack
  * @retval void
  */
static inline void usbd_uac_count_read_data(u32 audio_len)
{
#if USBD_UAC_DEBUG
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	cdev->read_data_len += audio_len;
#else
	UNUSED(audio_len);
#endif
}

/**
  * @brief  Read data from a USB audio ring buffer
  * @param  stream: Stream to read
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  copy_len: Accumulated copy length
  * @param  packet_index: Accumulated packet count, the bit index in zero_pkt_flag
  * @param  zero_pkt_flag: Optional bitmap of filler packets (bit N = packet N)
  * @retval Status
  */
static int usbd_uac_ring_read(usbd_uac_stream_t *stream, u8 *buffer, u32 size,
							  u32 *copy_len, u16 *packet_index, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *ring = &(stream->ring);
	u32 read_len;
	u8 valid = 0;

	do {
		/* Exit: 1) enough data obtained; 2) next packet cannot fit */
		if ((*copy_len >= size) || ((*copy_len + stream->max_packet_size) > size)) {
			return RTK_SUCCESS;
		}

		/* The OUT ISR drops the head of a full ring, possibly on another SMP core */
		usb_os_enter_critical();
		read_len = usb_ringbuf_remove_head(ring, buffer + *copy_len, size - *copy_len, &valid);
		usb_os_exit_critical();
		if (read_len > 0) {
			if (valid == 0U) {
				/* A filler node carries no payload, the ring hands back the stale bytes of the
				   slot: play it as silence */
				usb_os_memset((void *)(buffer + *copy_len), 0U, read_len);
				/* zero_pkt_flag is a 32-bit bitmap; cap at 32 pkts (shift >= 32 is UB). */
				if ((zero_pkt_flag != NULL) && (*packet_index < 32U)) {
					*zero_pkt_flag |= (1U << (*packet_index));
				}
			}
			*copy_len += read_len;
			(*packet_index)++;
		}
	} while (usb_ringbuf_is_empty(ring) == 0);

	if (*copy_len >= size) {
		return RTK_SUCCESS;
	}

	return -RTK_ERR_IO;
}

/**
  * @brief  Read the OUT ring buffer, refer to usbd_uac_read()
  * @note   Called by usbd_uac_read() between its gate updates, so the ring and the semaphore
  *         stay valid for the whole call.
  * @param  stream: Playback stream
  * @param  buffer: Destination buffer
  * @param  size: Destination capacity
  * @param  time_out_ms: Wait time, 0 for no wait
  * @param  zero_pkt_flag: Optional bitmap of filler packets
  * @retval Copied length in bytes
  */
static u32 usbd_uac_read_ring(usbd_uac_stream_t *stream, u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	u32 copy_len = 0;
	u16 packet_index = 0;
	u8 ring_empty;

	if (stream->streaming == 0) {
		return copy_len;
	}

	if (time_out_ms == 0) {
		if (usb_ringbuf_is_empty(&(stream->ring))) {
			return copy_len;
		}

		usbd_uac_ring_read(stream, buffer, size, &copy_len, &packet_index, zero_pkt_flag);
	} else {
		do {
			/* Test the ring and announce the wait under the lock the producer tests the flag with,
			   refer to usbd_uac_wake_reader(), or a node queued in between on the other core is
			   not signalled */
			usb_os_enter_critical();
			ring_empty = (usb_ringbuf_is_empty(&(stream->ring)) != 0) ? 1U : 0U;
			if (ring_empty != 0U) {
				stream->read_wait_sema = 1U;
			}
			usb_os_exit_critical();

			if (ring_empty != 0U) {
				/* is_closing: the buffer teardown waits for this call to leave */
				if ((stream->sema_valid == 0) || (stream->is_closing != 0U)) {
					stream->read_wait_sema = 0;
					break;
				}
				if (rtos_sema_take(stream->sema, time_out_ms) != RTK_SUCCESS) {
					stream->read_wait_sema = 0;
					break;
				}
				stream->read_wait_sema = 0;
				if (stream->is_closing != 0U) {
					break;
				}
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if ((usbd_uac_ring_read(stream, buffer, size, &copy_len, &packet_index, zero_pkt_flag) == RTK_SUCCESS)
					|| (copy_len > 0)) {
					break;
				}
			}
		} while (stream->streaming != 0);
	}

	return copy_len;
}

/**
  * @brief  Get the ring buffer node count of a direction at a speed
  * @note   play_buf_count / record_buf_count count milliseconds of audio, a node holds one packet
  * @param  speed: USB connection speed
  * @param  is_record: 1 for the record (ISOC IN, record_buf_count) ring, 0 for the playback (ISOC OUT, play_buf_count) one
  * @retval Number of ring nodes
  */
static u16 usbd_uac_get_ring_node_count(u8 speed, u8 is_record)
{
	const usbd_uac_dev_t *cdev = &usbd_uac_dev;
	u16 cnt;

	if (is_record != 0U) {
		cnt = (cdev->cfg->record_buf_count != 0U) ? cdev->cfg->record_buf_count : USBD_UAC_DEFAULT_RECORD_BUF_COUNT;
	} else {
		cnt = (cdev->cfg->play_buf_count != 0U) ? cdev->cfg->play_buf_count : USBD_UAC_DEFAULT_PLAY_BUF_COUNT;
	}

	return (u16)((cnt * (u32)usbd_uac_get_packets_per_second(speed)) / USBD_UAC_ONE_KHZ);
}

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Start the next record packet, sizing it from the fractional frame accumulator
  * @note   Ref UAC 2.0 2.3.1.1 and USB 2.0 5.12.4.2: an asynchronous source sends, in every
  *         packet period, the nominal number of audio frames or one more, so that the long
  *         term average matches the sampling rate, e.g. 5 or 6 frames per microframe at 44.1kHz.
  * @retval void
  */
static void usbd_uac_record_packetizer_next(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_record_packetizer_t *packetizer = &(cdev->record_packetizer);

	packetizer->fill = 0U;
	packetizer->target = packetizer->base_len;
	packetizer->acc += packetizer->rate_rem;
	if (packetizer->acc >= packetizer->packets_per_second) {
		packetizer->acc -= packetizer->packets_per_second;
		packetizer->target = (u16)(packetizer->target + packetizer->frame_len);
	}
}

/**
  * @brief  Restart the record packetizer, dropping a partly assembled packet
  * @retval void
  */
static void usbd_uac_record_packetizer_reset(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->record_packetizer.acc = 0U;
	usbd_uac_record_packetizer_next();
}

/**
  * @brief  Set up the record packetizer for the record format at the connection speed
  * @param  speed: USB connection speed
  * @param  mps: ISOC IN max packet size, which holds the nominal packet plus one frame
  * @retval RTK_SUCCESS, or -RTK_ERR_NOMEM when the packet buffer can not be allocated
  */
static int usbd_uac_record_packetizer_init(u8 speed, u16 mps)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_record_packetizer_t *packetizer = &(cdev->record_packetizer);

	usbd_uac_record_packetizer_deinit();

	packetizer->buf = (u8 *)usb_os_malloc(mps);
	if (packetizer->buf == NULL) {
		return -RTK_ERR_NOMEM;
	}

	packetizer->packets_per_second = usbd_uac_get_packets_per_second(speed);
	packetizer->frame_len = (u8)(USBD_UAC_RECORD_CHANNEL_COUNT * USBD_UAC_RECORD_BYTE_WIDTH);
	packetizer->base_len = (u16)((USBD_UAC_RECORD_SAMPLING_FREQ / packetizer->packets_per_second) * packetizer->frame_len);
	packetizer->rate_rem = USBD_UAC_RECORD_SAMPLING_FREQ % packetizer->packets_per_second;
	usbd_uac_record_packetizer_reset();

	return RTK_SUCCESS;
}

/**
  * @brief  Release the record packetizer buffer
  * @retval void
  */
static void usbd_uac_record_packetizer_deinit(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_record_packetizer_t *packetizer = &(cdev->record_packetizer);

	if (packetizer->buf != NULL) {
		usb_os_mfree((void *)packetizer->buf);
	}
	usb_os_memset((void *)packetizer, 0, sizeof(*packetizer));
}

/**
  * @brief  Handle completion of an ISOC IN transfer for the record engine, then retrigger
  *         if more data is queued.
  * @note   ISR context.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_uac_isoc_in_transmit_complete(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->record_ep.xfer_state = 0U;

	if (cdev->record_stream.streaming != 0) {
		usbd_uac_isoc_in_transmit_next(dev);
	}
}
#endif /* CONFIG_USBD_UAC_RECORD */

/**
  * @brief  Initialize a stream: size and allocate its ring for the connection speed
  * @param  stream: Stream to initialize
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_uac_stream_init(usbd_uac_stream_t *stream, u8 speed)
{
	u8 is_record = (stream == &(usbd_uac_dev.record_stream)) ? 1U : 0U;
	u16 mps;
	u8 cnt;

	/* The record format is fixed at build time, the playback node fits every declared format */
#ifdef CONFIG_USBD_UAC_RECORD
	mps = (is_record != 0U) ? usbd_uac_get_record_max_packet_size(speed) : usbd_uac_get_play_max_packet_size(speed);
#else
	mps = usbd_uac_get_play_max_packet_size(speed);
#endif
	/* 1 to USBD_UAC_RING_NODE_COUNT_MAX nodes: a buffer count of 0 picks the non-zero default and
	   usbd_uac_version_init() caps it so that the nodes of a high speed ms still fit a u8 */
	cnt = (u8)usbd_uac_get_ring_node_count(speed, is_record);
	/* Read by usbd_uac_get_read_frame_time_in_us() only, the ring does not depend on it */
	stream->binterval = usbd_uac_get_binterval(speed);

	/* The ring only depends on the speed: keep it while it fits, so a format switch of the
	   host never reallocates it */
	if ((stream->sema_valid != 0U) && (stream->max_packet_size == mps) && (stream->ring_node_count == cnt)) {
		return RTK_SUCCESS;
	}

	usbd_uac_stream_deinit(stream);
	stream->max_packet_size = mps;
	stream->ring_node_count = cnt;

#ifdef CONFIG_USBD_UAC_RECORD
	/* Before the ring, so that its failure leaves nothing to release */
	if ((is_record != 0U) && (usbd_uac_record_packetizer_init(speed, mps) != RTK_SUCCESS)) {
		return -RTK_ERR_NOMEM;
	}
#endif

	if (usb_ringbuf_manager_init(&(stream->ring), stream->ring_node_count, stream->max_packet_size, 1) != RTK_SUCCESS) {
#ifdef CONFIG_USBD_UAC_RECORD
		if (is_record != 0U) {
			usbd_uac_record_packetizer_deinit();
		}
#endif
		return -RTK_ERR_NOMEM;
	}

	if (stream->sema == NULL) {
		if (rtos_sema_create(&(stream->sema), 0U, 1U) != RTK_SUCCESS) {
			usb_ringbuf_manager_deinit(&(stream->ring));
#ifdef CONFIG_USBD_UAC_RECORD
			if (is_record != 0U) {
				usbd_uac_record_packetizer_deinit();
			}
#endif
			return -RTK_ERR_NOMEM;
		}
	}
	stream->read_wait_sema = 0;
	/* Every ring resource exists from here on: let usbd_uac_read() / usbd_uac_transmit_data() in.
	   Published under the lock, which orders the ring writes above before the flags for
	   usbd_uac_isoc_in_endpoint_open() in ISR context on the other core */
	usb_os_enter_critical();
	stream->sema_valid = 1;
	stream->is_closing = 0U;
	usb_os_exit_critical();
	stream->sof_count = 0;
	stream->data_count = 0;
	stream->packet_count = 0;
	stream->last_packet_len = 0;

	return RTK_SUCCESS;
}

#if USBD_UAC_DEBUG
/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbd_uac_status_dump_thread(void *param)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);
	u32 loop_idx = 0;
	u32 max_loop = USBD_UAC_DEBUG_LOOP_TIME / USBD_UAC_DEBUG_STEP_TIME;

	UNUSED(param);

	cdev->dump_thread_running = 1;
	while (cdev->dump_thread_running != 0) {
		loop_idx = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB Dump RX %d/TO %d/OW %d-%d/zp %d/pkt %d/copylen %d/%d\n",
				 stream->packet_count,
				 stream->timeout_count,
				 stream->overwrite_count,
				 stream->filler_overwrite_count,
				 stream->filler_count,
				 usb_ringbuf_get_count(&(stream->ring)),
				 cdev->read_data_len,
				 stream->streaming
				);
		do {
			if (0 == cdev->dump_thread_running) {
				break;
			}
			rtos_time_delay_ms(USBD_UAC_DEBUG_STEP_TIME); //split to many times
		} while (loop_idx++ < max_loop); // 100 * 100
	}

	rtos_task_delete(NULL);
}
#endif

/**
  * @brief  Allocate the DMA buffer of one stream
  * @note   One packet, sized to the HS MPS, which also covers the DWORD aligned FS MPS the controller
  *         pads a 1023 byte packet to. It lives from init to deinit, the ring is rebuilt without it.
  * @param  stream: Stream to allocate the buffer of
  * @retval Status
  */
static int usbd_uac_dma_buf_alloc(usbd_uac_stream_t *stream)
{
	stream->dma_buf = (u8 *)usb_os_malloc(USBD_UAC_HS_ISOC_MPS);
	if (stream->dma_buf == NULL) {
		return -RTK_ERR_NOMEM;
	}

	/* The controller DMA and the cache maintenance work on whole cache lines */
	if (!USB_IS_MEM_DMA_ALIGNED(stream->dma_buf)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "DMA buf unaligned\n");
		usb_os_mfree((void *)stream->dma_buf);
		stream->dma_buf = NULL;
		return -RTK_ERR_NOMEM;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Release the DMA buffers of both streams
  * @note   Only once no class callback can run any more, the ISR arms the endpoints with them.
  *         Safe to call repeatedly and after a partial init.
  * @retval void
  */
static void usbd_uac_dma_buf_free(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	usb_os_mfree((void *)cdev->play_stream.dma_buf);
	cdev->play_stream.dma_buf = NULL;
#ifdef CONFIG_USBD_UAC_RECORD
	usb_os_mfree((void *)cdev->record_stream.dma_buf);
	cdev->record_stream.dma_buf = NULL;
#endif
}

/**
  * @brief  Initialize UAC device, shared by the standalone and the composite initializer
  * @param  cb: Callback functions for audio events
  * @param  cfg: Class configuration
  * @retval Status
  */
static int usbd_uac_private_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	int ret = RTK_SUCCESS;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *play_ep = &cdev->play_ep;
	usb_ep_info_t *info;
#ifdef CONFIG_USBD_UAC_RECORD
	usbd_ep_t *record_ep = &cdev->record_ep;
#endif

	/* A second init would leak the DMA buffers and register the driver twice. cb is set by a
	   successful init and cleared by deinit or a failed init, so it tells an initialized class.
	   Tested first: nothing of the running instance may be touched. */
	if (cdev->cb != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Already init\n");
		return -RTK_ERR_BUSY;
	}

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	/* No H2D request is waiting for its data stage yet. The device context is a static object,
	   so a re-init after deinit must not inherit a stale pending flag. */
	cdev->ctrl_req_pending = 0U;

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return -RTK_ERR_BADARG;
	}

	/* Every endpoint the descriptors declare needs its address, with the right direction bit */
	if (((cfg->play_ep_addr & USB_REQ_DIR_MASK) != USB_H2D) || (usbd_uac_is_valid_endpoint_number(cfg->play_ep_addr) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x\n", cfg->play_ep_addr);
		return -RTK_ERR_BADARG;
	}
#ifdef CONFIG_USBD_UAC_RECORD
	if (((cfg->record_ep_addr & USB_REQ_DIR_MASK) != USB_D2H) || (usbd_uac_is_valid_endpoint_number(cfg->record_ep_addr) == 0U)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x\n", cfg->record_ep_addr);
		return -RTK_ERR_BADARG;
	}
#endif

	/* Checks of the UAC version built, e.g. the ring depth limit */
	ret = usbd_uac_version_init(cfg);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	/* Published before anything that reads it: the descriptor patch path, the endpoint setup
	   below and the ISOC buffer sizing all resolve their options through cdev->cfg. */
	cdev->cfg = cfg;

	/* The core owns the string table, so the same call works in standalone and composite
	   mode. 0 means no iInterface string. */
	cdev->interface_string_index = usbd_add_string(cfg->itf_str);

	cdev->play_volume = (cfg->volume_default != 0U) ? cfg->volume_default : USBD_UAC_DEFAULT_VOLUME;
	cdev->play_mute = (cfg->mute_default != 0U) ? 1U : 0U;
	/* Non-zero default rate: the first SET_INTERFACE(alt=1) fires format_changed
	 * with all three fields set even if the host skips SET_CUR SAMPLING_FREQ
	 * (single discrete rate), which would otherwise leave playback silent. */
	cdev->play_sampling_freq = USBD_UAC_SAMPLING_FREQ_48K;
#ifdef CONFIG_USBD_UAC_RECORD
	cdev->record_sampling_freq = USBD_UAC_RECORD_SAMPLING_FREQ;
	usbd_uac_stream_deinit(&(cdev->record_stream));
#endif
	usbd_uac_stream_deinit(&(cdev->play_stream));

	ret = usbd_uac_dma_buf_alloc(&(cdev->play_stream));
#ifdef CONFIG_USBD_UAC_RECORD
	if (ret == RTK_SUCCESS) {
		ret = usbd_uac_dma_buf_alloc(&(cdev->record_stream));
	}
#endif
	if (ret != RTK_SUCCESS) {
		usbd_uac_dma_buf_free();
		return ret;
	}

	/* bInterval is set for the negotiated speed by set_config(), the speed is unknown before */
	info = &play_ep->info;
	info->addr = cfg->play_ep_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = usbd_uac_get_binterval(USB_SPEED_FULL);
	play_ep->xfer_buf_len = USBD_UAC_HS_ISOC_MPS;
#ifdef CONFIG_USBD_UAC_RECORD
	info = &record_ep->info;
	info->addr = cfg->record_ep_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = usbd_uac_get_binterval(USB_SPEED_FULL);
#endif

	cdev->cb = cb;

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			/* Not registered yet, so nothing can use the buffers: undo what this init allocated */
			cdev->cb = NULL;
			usbd_uac_dma_buf_free();
			return ret;
		}
	}

#if USBD_UAC_DEBUG
	rtos_task_t task_dump;
	if (rtos_task_create(&task_dump, ((const char *)"usbd_uac_status_dump_thread"), usbd_uac_status_dump_thread, NULL, 1024U, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Convert a 16-bit little-endian control word to a signed value
  * @note   wCUR of a volume control is a two's complement 1/256 dB value; going through s32 keeps
  *         the conversion defined for words above 0x7FFF (MISRA C:2012 Rule 10.3)
  * @param  buf: The two received bytes
  * @retval Signed volume
  */
s16 usbd_uac_le16_to_s16(const u8 *buf)
{
	s32 value = (s32)buf[0] | ((s32)buf[1] << 8);

	if (value > 0x7FFF) {
		value -= 0x10000;
	}

	return (s16)value;
}

/**
  * @brief  Calculate the PC's intended volume level based on the UAC driver value
  * @param  target_y: The volume level from the UAC driver to be interpolated
  * @retval The intended volume level on the Windows PC corresponding to the UAC driver value,
  *         USBD_UAC_VOLUME_ERR_VAL outside the mapping table
  */
u8 usbd_uac_volume_linear_interpolation(s16 target_y)
{
	const u8 *x_points = usbd_uac_host_volume_levels;
	const s16 *y_points = usbd_uac_driver_volumes;
	u8 num_points = (u8)(sizeof(usbd_uac_host_volume_levels) / sizeof(usbd_uac_host_volume_levels[0]));
	u8 i;
	u8 x0;
	u8 x1;
	s16 y0;
	s16 y1;
	s32 numerator;
	s32 denominator;
	s32 x;

	if (target_y > 190 || target_y < -190) {
		return USBD_UAC_VOLUME_ERR_VAL;
	}

	for (i = 0; i < num_points - 1; i++) {
		y0 = y_points[i];
		y1 = y_points[i + 1];
		x0 = x_points[i];
		x1 = x_points[i + 1];

		if ((y0 <= target_y && target_y <= y1) || (y1 <= target_y && target_y <= y0)) {
			numerator = (s32)(target_y - y0) * (x1 - x0);
			denominator = (y1 - y0);
			x = (numerator + denominator / 2) / denominator + x0;
			if (x < 0) {
				x = 0;
			}
			if (x > 100) {
				x = 100;
			}
			return (u8)x;
		}
	}

	return USBD_UAC_VOLUME_ERR_VAL;
}

/**
  * @brief  Check the number of an application supplied endpoint address
  * @note   USB_EP_NUM() keeps the reserved bits D6..D4, so a reserved address is out of range as well
  * @param  ep_addr: Endpoint address, direction bit included
  * @retval 1 for a non-control endpoint the controller has, else 0
  */
u8 usbd_uac_is_valid_endpoint_number(u8 ep_addr)
{
	u8 ep_num = USB_EP_NUM(ep_addr);

	return ((ep_num != 0U) && (ep_num < USB_MAX_ENDPOINTS)) ? 1U : 0U;
}

/**
  * @brief  Accept the data stage of an H2D class or vendor request
  * @note   Ref USB 2.0 8.5.3.1 and 9.2.7: the status stage reports whether the request was
  *         carried out. The core only hands a data stage to ep0_data_out() in the Configured
  *         state and ACKs it otherwise, and a parameter block of the wrong size can not be
  *         applied, so both are refused here and the core stalls the request.
  * @param  dev: USB device instance
  * @param  req: USB request
  * @param  expected_length: Size of the parameter block of the addressed control
  * @retval Status
  */
int usbd_uac_receive_control_data(usb_dev_t *dev, const usb_setup_req_t *req, u16 expected_length)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = -RTK_ERR_BADARG;

	if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wLength == expected_length) &&
		((u32)req->wLength <= ep0_out->xfer_buf_len)) {
		usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
		cdev->ctrl_req_pending = 1U;
		ep0_out->xfer_len = req->wLength;
		/* Propagate a submit failure so that the core stalls EP0 instead of leaving the host
		   waiting out the data stage, ref USB 2.0 8.5.3.4 */
		ret = usbd_ep_receive(dev, ep0_out);
		if (ret != RTK_SUCCESS) {
			/* The data stage never started, so no EP0 OUT completion will arrive to consume the
			   stashed request. Drop it, else the next unrelated request's data stage would be
			   applied as this one's payload. */
			cdev->ctrl_req_pending = 0U;
		}
	}

	return ret;
}

/**
  * @brief  Wake the reader blocked in usbd_uac_read_ring() once the OUT ring holds a node
  * @note   ISR context. The wait flag and the ring are tested in the critical section the reader
  *         tests the ring and raises the flag in, so a node queued on one core while the reader on
  *         the other core is going to sleep is never missed. The give does not block and stays in
  *         the lock, the one usbd_uac_stream_deinit() clears sema_valid in before deleting
  *         the semaphore.
  * @param  stream: Playback stream
  * @retval void
  */
void usbd_uac_wake_reader(usbd_uac_stream_t *stream)
{
	usb_os_enter_critical();
	if ((stream->sema_valid != 0U) && (stream->read_wait_sema != 0U) &&
		(usb_ringbuf_is_empty(&(stream->ring)) == 0)) {
		rtos_sema_give(stream->sema);
	}
	usb_os_exit_critical();
}

/**
  * @brief  Size the playback ring for the connection speed, then arm the ISOC OUT endpoint
  * @param  void
  * @retval Status
  */
int usbd_uac_start_play(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);
	int ret;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play start\n");
#endif

	/* cdev->dev is assigned in set_config, so it is NULL until the host has enumerated us. The ring
	   sizing needs dev_speed, which is only known after the speed negotiation: the app retries. */
	if (cdev->dev == NULL) {
		return -RTK_ERR_IO;
	}

	/* Stopped until the endpoint is armed again below */
	stream->streaming = 0;

	/* Allocates the ring on the first call only, refer to usbd_uac_stream_init() */
	ret = usbd_uac_stream_init(stream, cdev->dev->dev_speed);
	if (ret == RTK_SUCCESS) {
		ret = usbd_uac_play_endpoint_arm();
	}

	return ret;
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbd_uac_stop_play(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play stop\n");
#endif
	cdev->play_stream.streaming = 0;
}

/**
  * @brief  Read UAC data
  * @param  buffer: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @param  zero_pkt_flag: Optional bitmap of filler packets, refer to usbd_uac.h
  * @retval copy len
  */
u32 usbd_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);
	usb_dev_t *dev = cdev->dev;
	u32 copy_len;

	if (zero_pkt_flag != NULL) {
		*zero_pkt_flag = 0;
	}

	if (dev == NULL || dev->is_ready == 0) {
		return 0U;
	}

	/* Counted under the same lock the buffer teardown closes the gate with, so the teardown
	   either sees this caller or this caller sees is_closing: never both missed */
	usb_os_enter_critical();
	if ((stream->is_closing != 0U) || (stream->sema_valid == 0U)) {
		usb_os_exit_critical();
		return 0U;
	}
	stream->user_count++;
	usb_os_exit_critical();

	copy_len = usbd_uac_read_ring(stream, buffer, size, time_out_ms, zero_pkt_flag);

	usb_os_enter_critical();
	stream->user_count--;
	usb_os_exit_critical();

	usbd_uac_count_read_data(copy_len);
	return copy_len;
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval read frame cnt
  */
u32 usbd_uac_get_read_frame_cnt(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);

	return usb_ringbuf_get_count(&(stream->ring));
}

/**
  * @brief  Push a node to a ring buffer, dropping the oldest one if it is full.
  * @note   ISR context for the OUT ring, task context for the IN ring. The other side pops the
  *         head, possibly on another SMP core, so the head drop and the tail add are serialized
  *         with it. The ring is released only after streaming was cleared under the same lock,
  *         refer to usbd_uac_stream_deinit(), so nothing is pushed once it is 0.
  * @param  stream: Stream whose ring is pushed
  * @param  buf: node payload, NULL for a filler node
  * @param  len: node length
  * @param  valid: 1 for real data, 0 for a filler node
  * @retval 1 if the oldest node was dropped, 0 otherwise
  */
u8 usbd_uac_ring_push(usbd_uac_stream_t *stream, const u8 *buf, u32 len, u8 valid)
{
	usb_ringbuf_manager_t *ring = &(stream->ring);
	u8 tmp[1];
	u8 dropped = 0U;

	usb_os_enter_critical();
	if (stream->streaming != 0U) {
		if (usb_ringbuf_is_full(ring) != 0) {
			usb_ringbuf_remove_head(ring, tmp, 1, NULL);
			dropped = 1U;
		}
		usb_ringbuf_add_tail(ring, buf, len, valid);
	}
	usb_os_exit_critical();

	return dropped;
}

/**
  * @brief  Append a filler node to the OUT ring buffer when the host stops sending data.
  * @note   ISR context (invoked from SOF path).
  */
void usbd_uac_append_filler(usbd_uac_stream_t *stream)
{
	if (usbd_uac_ring_push(stream, NULL, stream->last_packet_len, 0U) != 0U) {
#if USBD_UAC_DEBUG
		stream->filler_overwrite_count++;
#endif
	}
#if USBD_UAC_DEBUG
	stream->filler_count++;
#endif

	usbd_uac_wake_reader(stream);
}

/**
  * @brief  Release a stream, waiting for every task still using it
  * @param  stream: Stream to release
  * @retval void
  */
void usbd_uac_stream_deinit(usbd_uac_stream_t *stream)
{
	u32 wait_ms;

	/* Close the gates first, under the lock the ISRs test streaming with before touching the
	   ring and the task APIs test is_closing with, refer to usbd_uac_ring_push() */
	usb_os_enter_critical();
	stream->streaming = 0;
	stream->is_closing = 1U;
	usb_os_exit_critical();

	stream->max_packet_size = 0;
	stream->ring_node_count = 0;
	stream->sof_count = 0;
	stream->data_count = 0;
	stream->packet_count = 0;
	stream->last_packet_len = 0;

	if (stream->sema_valid != 0) {
		/* Cleared under the lock the ISR gives the semaphore in, refer to usbd_uac_wake_reader(): once
		   it is 0 no ISR gives it any more, so it can be deleted after the users below are gone */
		usb_os_enter_critical();
		stream->sema_valid = 0;
		usb_os_exit_critical();

		/* Wait for every task still inside usbd_uac_read() or usbd_uac_transmit_data() on this
		   ring to leave. Not bounded: freeing the semaphore or the ring under such a task would be
		   a use after free, so a starved one is reported once per USBD_UAC_USER_EXIT_WARN_MS
		   instead. Sleep rather than spin, the woken task may have a lower priority. */
		wait_ms = 0U;
		while (stream->user_count != 0U) {
			rtos_sema_give(stream->sema);
			usb_os_sleep_ms(1U);
			wait_ms++;
			if (wait_ms >= USBD_UAC_USER_EXIT_WARN_MS) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wait ring user exit\n");
				wait_ms = 0U;
			}
		}
		rtos_sema_delete(stream->sema);
		stream->sema = NULL;
	}

	usb_ringbuf_manager_deinit(&(stream->ring));

#ifdef CONFIG_USBD_UAC_RECORD
	/* No usbd_uac_transmit_data() caller is left, refer to the wait above */
	if (stream == &(usbd_uac_dev.record_stream)) {
		usbd_uac_record_packetizer_deinit();
	}
#endif
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @param  len: received length
  * @retval Status
  */
int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);
	usbd_ep_t *play_ep = &cdev->play_ep;
	int ret;
	u8 queued = 0U;

	UNUSED(dev);

	/* Return non-zero if EP is not owned by UAC, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->play_ep_addr) {
		return -RTK_ERR_BADARG;
	}

	if (stream->streaming != 0) {
		if (len > stream->max_packet_size) {
			/* Longer than a ring node, which is sized for the widest declared format, refer to
			   usbd_uac_get_play_max_packet_size(): the host broke its wMaxPacketSize. Drop it rather than queue a
			   truncated packet. */
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		} else if (len != 0U) { /* A ZLP carries no audio */
			stream->last_packet_len = (u16)len;
			stream->packet_count ++;
			stream->data_count ++;

#if USBD_UAC_DEBUG
			u32 rx_new_tick = usb_os_get_timestamp_us();
			/* 1.5 packet periods: 1500us for a 1ms packet, 185us for a 125us one */
			u32 step = (usbd_uac_get_packets_per_second(cdev->dev->dev_speed) == USBD_UAC_ONE_KHZ) ? 1500U : 185U;
			if ((stream->last_packet_time_us > 0) && ((rx_new_tick - stream->last_packet_time_us) > step)) {
				stream->timeout_count++;
			}
			stream->last_packet_time_us = rx_new_tick;
#endif

			if (usbd_uac_ring_push(stream, stream->dma_buf, len, 1U) != 0U) {
#if USBD_UAC_DEBUG
				stream->overwrite_count++;
#endif
			}
			queued = 1U;
		}
	}

	/* Re-arm before waking the reader. Only fails once the device is no longer ready, the OUT EP then
	   stays NAKing until the next start_play() re-arms it: record it, there is nothing to retry in an
	   ISR. The return value only tells the composite dispatcher the EP is owned. */
	play_ep->xfer_buf = stream->dma_buf;
	play_ep->xfer_len = stream->max_packet_size;
	ret = usbd_ep_receive(dev, play_ep);
	if (ret != RTK_SUCCESS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	if (queued != 0U) {
		usbd_uac_wake_reader(stream);
	}

	return RTK_SUCCESS;
}

/**
  * @brief  USB attach status change
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Detach: stop playback and record and wake up the read thread blocked in usbd_uac_read.
		   ISR context here: only rtos_sema_give is allowed (no malloc/sema_take). */
		usbd_uac_stop_play();
#ifdef CONFIG_USBD_UAC_RECORD
		usbd_uac_stop_record();
#endif
		/* Same lock as usbd_uac_wake_reader(), the semaphore cannot be deleted under the give */
		usb_os_enter_critical();
		if ((cdev->play_stream.sema_valid != 0U) && (cdev->play_stream.read_wait_sema != 0U)) {
			rtos_sema_give(cdev->play_stream.sema);
		}
		usb_os_exit_critical();
	}

	/* cb is released by usbd_uac_deinit(), which may run concurrently with this ISR: read once */
	cb = cdev->cb;
	if ((cb != NULL) && (cb->status_changed != NULL)) {
		cb->status_changed(old_status, status);
	}
}

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Open the ISOC IN endpoint of the microphone AS interface
  * @note   Ref USB 2.0 9.4.10: the endpoints of an alternate setting become active on its
  *         SET_INTERFACE, a host may skip SET_CUR on a clock that offers a single rate.
  *         An IN transfer aborted by the re-init never completes, so xfer_state is cleared
  *         in the same critical section the record engine uses to submit.
  * @param  dev: USB device instance
  * @retval Status
  */
int usbd_uac_isoc_in_endpoint_open(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *record_ep = &cdev->record_ep;
	u16 mps;
	int ret;

	/* Size the EP from the format the descriptors report, not from the record ring buffer which
	   usbd_uac_start_record() may be rebuilding at the same time */
	mps = usbd_uac_get_record_max_packet_size(dev->dev_speed);

	usb_os_enter_critical();
	usbd_ep_deinit(dev, record_ep);
	record_ep->info.mps = mps;
	/* A failed open leaves the interface on the previous setting, refer to the SET_INTERFACE caller */
	ret = usbd_ep_init(dev, record_ep);
	record_ep->xfer_state = 0U;
	/* Audio queued while the host was not streaming is stale, start the new stream from now.
	   is_closing is raised under this lock before the ring is freed, sema_valid is not */
	if ((ret == RTK_SUCCESS) && (cdev->record_stream.is_closing == 0U) && (cdev->record_stream.sema_valid != 0U)) {
		usb_ringbuf_reset(&(cdev->record_stream.ring));
	}
	usb_os_exit_critical();

	return ret;
}

/**
  * @brief  Pop one packet from the record TX ring buffer and transmit it on the ISOC IN endpoint.
  * @note   ISR context (SOF trigger / IN completion retrigger) or task context (usbd_uac_transmit_data
  *         kick). Nothing is submitted while the mic streaming setting is not selected, the EP is
  *         then closed. usb_ringbuf_remove_head() returns 0 when the ring buffer is empty, which naturally
  *         results in a ZLP transmit so the endpoint keeps being serviced every interval.
  *         The xfer_state test-and-set, the head pop and the submit run in one critical section, so
  *         the ISR and the record task, possibly on another SMP core, can not both pop the head or
  *         both submit into the busy DMA buffer of the record stream.
  * @param  dev: USB device instance
  * @retval void
  */
void usbd_uac_isoc_in_transmit_next(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->record_stream);
	usbd_ep_t *record_ep = &cdev->record_ep;
	u32 len;

	usb_os_enter_critical();

	/* streaming is cleared under this lock before the ring is released, refer to
	   usbd_uac_stream_deinit(), so it is tested again here */
	if ((record_ep->xfer_state == 0U) && (stream->streaming != 0U) && (cdev->record_alt_setting != 0U)) {
		len = usb_ringbuf_remove_head(&(stream->ring), stream->dma_buf, stream->max_packet_size, NULL);

		record_ep->xfer_state = 1U;
		record_ep->xfer_buf = stream->dma_buf;
		record_ep->xfer_len = (u16)len;
		/* On failure no completion IRQ fires to clear xfer_state; clear it here. */
		if (usbd_ep_transmit(dev, record_ep) != RTK_SUCCESS) {
			record_ep->xfer_state = 0U;
		}
	}

	usb_os_exit_critical();
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @param  status: Transfer status
  * @retval Status
  */
int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	/* Return non-zero if EP is not owned by UAC, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->record_ep_addr) {
		return -RTK_ERR_BADARG;
	}

	if (status != RTK_SUCCESS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	usbd_uac_isoc_in_transmit_complete(dev);

	return RTK_SUCCESS;
}
#endif /* CONFIG_USBD_UAC_RECORD */

#ifdef CONFIG_USBD_COMPOSITE
/**
  * @brief  Initialize the UAC class as part of a composite device
  * @param  cb: Callback functions for audio events
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbd_composite_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		cdev->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_uac_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_uac_deinit();
		}
	}
	return ret;
}
#endif

/**
  * @brief  Initialize the UAC class as a standalone device
  * @param  cb: Callback functions for audio events
  * @param  cfg: Class configuration
  * @retval Status
  */
int usbd_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		cdev->from_composite = 0;
		ret = usbd_register_class(&usbd_uac_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse, as on the composite path */
			usbd_uac_deinit();
		}
	}
	return ret;
}

/**
  * @brief  DeInitialize UAC device
  * @param  void
  * @retval None
  */
void usbd_uac_deinit(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	/* Leave the core before anything is torn down. Composite: refused until
	   usbd_composite_deinit() has blocked the ISR and cleared the configuration of every
	   function. Standalone: refused in interrupt context. Either way nothing is released when
	   refused, and once accepted no class callback runs any more and every endpoint of this
	   class is deinitialized: ref USB 2.0 9.4.7, the transfers still pending are aborted. */
#ifdef CONFIG_USBD_COMPOSITE
	if (cdev->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_uac_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != RTK_SUCCESS) {
		return;
	}

#if USBD_UAC_DEBUG
	cdev->dump_thread_running = 0;
#endif

#ifdef CONFIG_USBD_UAC_RECORD
	usbd_uac_stream_deinit(&(cdev->record_stream));
#endif
	usbd_uac_stream_deinit(&(cdev->play_stream));

	/* Unregistered above: no class callback can run afterwards, so dropping the pending
	   control request here cannot race an EP0 OUT completion in ISR context, and no endpoint
	   is armed with the DMA buffers any more. */
	cdev->ctrl_req_pending = 0U;
	usbd_uac_dma_buf_free();

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}
}

/**
  * @brief  Return the current OUT ring buffer occupancy expressed in microseconds
  *         of audio at the negotiated bInterval. Callers use this to pace playback
  *         and detect underruns.
  */
u32 usbd_uac_get_read_frame_time_in_us(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->play_stream);
	u8 speed = (cdev->dev != NULL) ? cdev->dev->dev_speed : (u8)USB_SPEED_FULL;
	/* One node holds the packet of one packet period, refer to usbd_uac_get_packets_per_second() */
	u32 base_us = (USBD_UAC_ONE_KHZ * USBD_UAC_ONE_KHZ) / usbd_uac_get_packets_per_second(speed);

	return (u32)usb_ringbuf_get_count(&(stream->ring)) * base_us;
}

#ifdef CONFIG_USBD_UAC_RECORD
/**
  * @brief  Transmit UAC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbd_uac_transmit_data(const u8 *buf, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_stream_t *stream = &(cdev->record_stream);
	usbd_uac_record_packetizer_t *packetizer = &(cdev->record_packetizer);
	u32 off = 0U;
	u32 chunk;

	if ((buf == NULL) && (len != 0U)) {
		return -RTK_ERR_BADARG;
	}

	if (dev == NULL || dev->is_ready == 0) {
		return RTK_SUCCESS;
	}

	/* Counted under the same lock the buffer teardown closes the gate with, so the teardown
	   either sees this caller or this caller sees is_closing: never both missed */
	usb_os_enter_critical();
	if ((stream->is_closing != 0U) || (stream->sema_valid == 0U) || (stream->streaming == 0U)) {
		usb_os_exit_critical();
		return RTK_SUCCESS;
	}
	stream->user_count++;
	usb_os_exit_critical();

	/* The open gate implies a sized packetizer, refer to usbd_uac_stream_init(). Its target is
	   never 0: the record rate is 8kHz or above, one frame per microframe */
	/* Cut the stream into packets of the nominal size of each (micro)frame, any length is accepted
	   and nothing is truncated: a tail shorter than a packet waits in the packetizer for the next
	   call. Both ring operations are serialized with the ISR, which pops the head and retriggers
	   the IN EP. A ring full of unsent packets drops its oldest one, which is the expected overrun
	   handling of a live stream. */
	while (off < len) {
		chunk = MIN(len - off, (u32)packetizer->target - packetizer->fill);
		usb_os_memcpy((void *)(packetizer->buf + packetizer->fill), (const void *)(buf + off), chunk);
		packetizer->fill = (u16)(packetizer->fill + chunk);
		off += chunk;

		if (packetizer->fill == packetizer->target) {
			if (usbd_uac_ring_push(stream, packetizer->buf, packetizer->fill, 1U) != 0U) {
				RTK_LOGS(TAG, RTK_LOG_DEBUG, "EP%02x TX drop\n", cdev->cfg->record_ep_addr);
			}
			usbd_uac_record_packetizer_next();
		}
	}
	usbd_uac_isoc_in_transmit_next(dev);

	usb_os_enter_critical();
	stream->user_count--;
	usb_os_exit_critical();

	return RTK_SUCCESS;
}

/**
  * @brief  Size the record ring for the connection speed, then start the record (microphone) stream
  * @param  void
  * @retval Status
  */
int usbd_uac_start_record(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_stream_t *stream = &(cdev->record_stream);
	int ret;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
#endif

	/* Same precondition as usbd_uac_start_play(): attached, so that the speed is known */
	if ((cdev->cfg == NULL) || (cdev->dev == NULL)) {
		return -RTK_ERR_IO;
	}

	/* Stopped until the ring is reset below */
	stream->streaming = 0;

	/* Allocates the ring and the packetizer on the first call only, refer to usbd_uac_stream_init() */
	ret = usbd_uac_stream_init(stream, cdev->dev->dev_speed);
	if (ret != RTK_SUCCESS) {
		return ret;
	}

	/* Published as one unit: the record engine on the other core must not see streaming set
	   before the ring reset, nor pop the ring while it is reset */
	usb_os_enter_critical();
	usb_ringbuf_reset(&(stream->ring));
	usbd_uac_record_packetizer_reset();
	stream->sof_count = 0;
	stream->data_count = 0;
	stream->packet_count = 0;
	stream->streaming = 1;
	usb_os_exit_critical();

	return RTK_SUCCESS;
}

/**
  * @brief  Stop UAC device record (microphone)
  * @param  void
  * @retval void
  */
void usbd_uac_stop_record(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record stop\n");
#endif
	cdev->record_stream.streaming = 0;
}
#else
/**
  * @brief  Transmit UAC data
  * @note   Playback only build: no ISOC IN interface, enable CONFIG_USBD_UAC_RECORD to record.
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval -RTK_ERR_IO
  */
int usbd_uac_transmit_data(const u8 *buf, u32 len)
{
	UNUSED(buf);
	UNUSED(len);

	return -RTK_ERR_IO;
}

/**
  * @brief  Start UAC device record (microphone)
  * @note   Playback only build, refer to usbd_uac_transmit_data().
  * @param  void
  * @retval -RTK_ERR_IO
  */
int usbd_uac_start_record(void)
{
	return -RTK_ERR_IO;
}

/**
  * @brief  Stop UAC device record (microphone)
  * @note   Playback only build: nothing to stop.
  * @param  void
  * @retval void
  */
void usbd_uac_stop_record(void)
{
}
#endif /* CONFIG_USBD_UAC_RECORD */
