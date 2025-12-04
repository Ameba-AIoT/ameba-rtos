/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __BT_AUDIO_RING_BUFFER_H__
#define __BT_AUDIO_RING_BUFFER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define BT_AUDIO_RINGBUFFER_PRE_SIZE 1920 * 15
#define BT_AUDIO_RINGBUFFER_SIZE (BT_AUDIO_RINGBUFFER_PRE_SIZE + 9600) // 9600 is for 48k 2channels 16bits 50ms Presentation Delay
#define BT_AUDIO_STATIC_RINGBUFFER_NUM 2

/**
 * @typedef   bt_audio_ring_buffer_t
 * @brief     audio ringbuffer struct
 *            there is no need for mutex cause audio stream handle is dequeued by the same thread
 */
typedef struct bt_audio_ring_buffer {
	void *mtx;
	uint8_t *storage;
	uint32_t size;
	uint32_t last_read_index;
	uint32_t last_written_index;
	uint8_t full;
	bool used;
} bt_audio_ring_buffer_t;

/**
 * @fn        uint16_t bt_audio_ring_buffer_init(bt_audio_ring_buffer_t *ring_buffer, uint8_t *storage, uint32_t storage_size)
 * @brief     bt audio ring buffer initialization
 * @param[in] ring_buffer: ring buffer pointer
 * @param[in] storage_size: actual storage size
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t bt_audio_ring_buffer_init(bt_audio_ring_buffer_t *ring_buffer, uint32_t storage_size);

/**
 * @fn        uint16_t bt_audio_ring_buffer_deinit(bt_audio_ring_buffer_t *ring_buffer)
 * @brief     bt audio ring buffer deinit
 * @param[in] ring_buffer: ring buffer pointer
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t bt_audio_ring_buffer_deinit(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @fn        uint32_t bt_audio_ring_buffer_bytes_read_available(bt_audio_ring_buffer_t *ring_buffer)
 * @brief     get audio stream bytes need to be playback
 * @param[in] ring_buffer: ring buffer pointer
 * @return    available audio stream bytes size
 */
uint32_t bt_audio_ring_buffer_bytes_read_available(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @fn        bool bt_audio_ring_buffer_empty(bt_audio_ring_buffer_t *ring_buffer)
 * @brief     check whether bt audio ring buffer is empty(there is no memory free to write)
 * @param[in] ring_buffer: ring buffer pointer
 * @return
 *            - 0  : Not empty
 *            - 1  : Empty
 */
bool bt_audio_ring_buffer_empty(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @fn        uint32_t bt_audio_ring_buffer_bytes_write_available(bt_audio_ring_buffer_t *ring_buffer)
 * @brief     check whether bt audio ring buffer is free(there is memory free to write)
 * @param[in] ring_buffer: ring buffer pointer
 * @return    free audio stream bytes size
 */
uint32_t bt_audio_ring_buffer_bytes_write_available(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @fn        uint16_t bt_audio_ring_buffer_write(bt_audio_ring_buffer_t *ring_buffer, uint8_t *data, uint32_t data_length)
 * @brief     check whether bt audio ring buffer is empty(there is no memory free to write)
 * @param[in] ring_buffer: ring buffer pointer
 * @param[in] data: pointer to stream to be writed
 * @param[in] data_length: stream data length(unit 2 bytes)
 * @return
 *            - 0  : Succeed
 *            - others: Error code
 */
uint16_t bt_audio_ring_buffer_write(bt_audio_ring_buffer_t *ring_buffer, uint8_t *data, uint32_t data_length);

/**
 * @fn        uint32_t bt_audio_ring_buffer_read(bt_audio_ring_buffer_t *ring_buffer, uint8_t *p_out, uint32_t data_length)
 * @brief     read audio stream data from bt audio ring buffer
 * @param[in] ring_buffer: ring buffer pointer
 * @param[in] p_out: pointer to the output buffer
 * @param[in] data_length: stream data length(unit 2 bytes)
 * @param[in] number_of_bytes_read: actual read data length
 * @return
 *            - Read size
 */
uint32_t bt_audio_ring_buffer_read(bt_audio_ring_buffer_t *ring_buffer, uint8_t *p_out, uint32_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_RING_BUFFER_H__ */
