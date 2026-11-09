/**
 * @file      bt_audio_ring_buffer.h
 * @brief     BT Audio ring buffer type and function definition.
 * @copyright Copyright (c) 2021. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __BT_AUDIO_RING_BUFFER_H__
#define __BT_AUDIO_RING_BUFFER_H__

#ifdef __cplusplus
extern "C"
{
#endif

#define BT_AUDIO_RINGBUFFER_PRE_SIZE 1920 * 15  /*!< Base capacity of a ring buffer storage. Unit: bytes. */
#define BT_AUDIO_STATIC_RINGBUFFER_NUM 2        /*!< Number of statically allocated ring buffer storages. */
/**
 * @def       BT_AUDIO_RINGBUFFER_SIZE
 * @brief     Capacity of a ring buffer storage. Unit: bytes.
 *            The extra 9600 bytes cover a 50 ms presentation delay of 48 kHz, 2 channel, 16 bit audio.
 */
#define BT_AUDIO_RINGBUFFER_SIZE (BT_AUDIO_RINGBUFFER_PRE_SIZE + 9600)

/**
 * @typedef   bt_audio_ring_buffer_t
 * @brief     BT Audio ring buffer backed by a statically allocated storage.
 */
typedef struct bt_audio_ring_buffer {
	void *mtx;                      /*!< Mutex protecting the read and write indexes. */
	uint8_t *storage;               /*!< Storage assigned from the static pool at initialization. */
	uint32_t size;                  /*!< Capacity of the storage in use. Unit: bytes. */
	uint32_t last_read_index;       /*!< Offset of the next byte to read. Unit: bytes. */
	uint32_t last_written_index;    /*!< Offset of the next byte to write. Unit: bytes. */
	uint8_t full;                   /*!< Whether the buffer holds size bytes of unread data.
                                     *   1: full.
                                     *   0: not full. */
	bool used;                      /*!< Whether the ring buffer is initialized.
                                     *   - true: initialized.
                                     *   - false: not initialized. */
} bt_audio_ring_buffer_t;

/********************************* Functions Declaration *******************************/
/**
 * @defgroup  bt_audio_ring_buffer BT Audio Ring Buffer Related APIs
 * @brief     BT Audio ring buffer APIs.
 * @ingroup   bt_audio_group
 * @{
 */

/**
 * @brief     Initialize a ring buffer and assign it a storage from the static pool.
 * @param[in] ring_buffer: Ring buffer to initialize.
 *                         Must not be NULL.
 * @param[in] storage_size: Capacity of the ring buffer.
 *                          Unit: bytes.
 * @return
 *            - 0: Succeed.
 *            - 1: Failed.
 * @note
 *            - The structure must be zero-initialized before the first call.
 *            - Fails when storage_size is larger than @ref BT_AUDIO_RINGBUFFER_SIZE or when all @ref BT_AUDIO_STATIC_RINGBUFFER_NUM storages are in use.
 */
uint16_t bt_audio_ring_buffer_init(bt_audio_ring_buffer_t *ring_buffer, uint32_t storage_size);

/**
 * @brief     Deinitialize a ring buffer and release its storage and mutex.
 * @param[in] ring_buffer: Ring buffer to deinitialize.
 *                         Must not be NULL.
 * @return
 *            - 0: Succeed.
 *            - 1: Failed because the ring buffer is NULL or not initialized.
 */
uint16_t bt_audio_ring_buffer_deinit(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @brief     Get the number of bytes stored in the ring buffer that can be read.
 * @param[in] ring_buffer: Ring buffer.
 *                         Must not be NULL.
 * @return
 *            - Number of bytes available for reading. 0 if the ring buffer is NULL or not initialized.
 */
uint32_t bt_audio_ring_buffer_bytes_read_available(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @brief     Check whether the ring buffer holds no data to read.
 * @param[in] ring_buffer: Ring buffer.
 *                         Must not be NULL.
 * @return
 *            - true: No data is available for reading.
 *            - false: Data is available for reading, or the ring buffer is NULL or not initialized.
 */
bool bt_audio_ring_buffer_empty(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @brief     Get the free space of the ring buffer that can be written.
 * @param[in] ring_buffer: Ring buffer.
 *                         Must not be NULL.
 * @return
 *            - Number of bytes available for writing. 0 if the ring buffer is NULL or not initialized.
 */
uint32_t bt_audio_ring_buffer_bytes_write_available(bt_audio_ring_buffer_t *ring_buffer);

/**
 * @brief     Write data into the ring buffer.
 * @param[in] ring_buffer: Ring buffer.
 *                         Must not be NULL.
 * @param[in] data: Data to write.
 *                  Must not be NULL.
 * @param[in] data_length: Length of data.
 *                         Unit: bytes.
 *                         Must be non-zero.
 * @return
 *            - 0: Succeed.
 *            - 1: Failed because of an invalid parameter or not enough free space.
 * @note
 *            - The data is written only when the free space is not less than data_length.
 */
uint16_t bt_audio_ring_buffer_write(bt_audio_ring_buffer_t *ring_buffer, uint8_t *data, uint32_t data_length);

/**
 * @brief     Read data from the ring buffer and remove it from the ring buffer.
 * @param[in] ring_buffer: Ring buffer.
 *                         Must not be NULL.
 * @param[out] p_out: Buffer that receives the data.
 *                    Must not be NULL.
 * @param[in] data_length: Maximum number of bytes to read.
 *                         Unit: bytes.
 * @return
 *            - Number of bytes read. 0 if no data is available.
 */
uint32_t bt_audio_ring_buffer_read(bt_audio_ring_buffer_t *ring_buffer, uint8_t *p_out, uint32_t data_length);

/**
 * @brief     Discard data from the ring buffer and zero the discarded bytes.
 * @param[in] ring_buffer: Ring buffer.
 *                         Must not be NULL.
 * @param[in] data_length: Maximum number of bytes to discard.
 *                         Unit: bytes.
 * @return
 *            - Number of bytes discarded. 0 if no data is available.
 */
uint32_t bt_audio_ring_buffer_clear(bt_audio_ring_buffer_t *ring_buffer, uint32_t data_length);

#ifdef __cplusplus
}
#endif

#endif /* __BT_AUDIO_RING_BUFFER_H__ */
