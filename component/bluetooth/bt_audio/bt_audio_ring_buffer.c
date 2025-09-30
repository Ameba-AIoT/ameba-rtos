/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

/* -------------------------------- Includes -------------------------------- */
#include <stdio.h>
#include <bt_debug.h>
#include <osif.h>
#include <bt_audio_config.h>
#include <string.h>
#include <bt_audio_ring_buffer.h>

/* -------------------------------- Defines --------------------------------- */

/* ----------------------------- static struct ------------------------------ */
typedef struct {
	uint8_t buffer[BT_AUDIO_RINGBUFFER_SIZE];
	uint32_t size;
	bool used;
} bt_audio_buffer;
/* ---------------------------- Global Variables ---------------------------- */
static bt_audio_buffer static_storage[BT_AUDIO_STATIC_RINGBUFFER_NUM] = {0};

static uint32_t bt_audio_min(uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}

// static uint32_t bt_audio_max(uint32_t a, uint32_t b)
// {
//  return (a > b) ? a : b;
// }

uint16_t bt_audio_ring_buffer_init(bt_audio_ring_buffer_t *ring_buffer, uint32_t storage_size)
{
	bool find = false;

	if (!ring_buffer || ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has been initialized \r\n", __func__);
		return 1;
	}
	memset((void *)ring_buffer, 0, sizeof(bt_audio_ring_buffer_t));
	if (storage_size > BT_AUDIO_RINGBUFFER_SIZE) {
		BT_LOGE("%s: storage_size %d is larger than BT_AUDIO_RINGBUFFER_SIZE %d \r\n", __func__, storage_size, BT_AUDIO_RINGBUFFER_SIZE);
		return 1;
	}
	for (uint8_t i = 0; i < BT_AUDIO_STATIC_RINGBUFFER_NUM; i ++) {
		if (!static_storage[i].used) {
			static_storage[i].used = true;
			static_storage[i].size = storage_size;
			ring_buffer->storage = static_storage[i].buffer;
			find = true;
			break;
		}
	}
	if (!find) {
		BT_LOGE("%s: ring_buffer->storage allocate fail \r\n", __func__);
		return 1;
	}
	osif_mutex_create(&ring_buffer->mtx);
	if (!ring_buffer->mtx) {
		BT_LOGE("%s: create mtx fail \r\n", __func__);
		memset((void *)ring_buffer->storage, 0, sizeof(bt_audio_buffer));
		return 1;
	}
	ring_buffer->size = storage_size;
	ring_buffer->last_read_index = 0;
	ring_buffer->last_written_index = 0;
	ring_buffer->full = 0;
	ring_buffer->used = true;
	BT_LOGA("%s: ring_buff alloc successfully size is %d \r\n", __func__, storage_size);

	return 0;
}

uint16_t bt_audio_ring_buffer_deinit(bt_audio_ring_buffer_t *ring_buffer)
{
	if (!ring_buffer || !ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has not been initialized \r\n", __func__);
		return 1;
	}
	ring_buffer->used = false;
	if (ring_buffer->storage) {
		memset((void *)ring_buffer->storage, 0, sizeof(bt_audio_buffer));
	}
	if (ring_buffer->mtx) {
		osif_mutex_delete(ring_buffer->mtx);
	}
	memset((void *)ring_buffer, 0, sizeof(bt_audio_ring_buffer_t));

	return 0;
}

uint32_t bt_audio_ring_buffer_bytes_read_available(bt_audio_ring_buffer_t *ring_buffer)
{
	int diff = 0;

	if (!ring_buffer || !ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has not been initialized \r\n", __func__);
		return 0;
	}
	if (ring_buffer->full) {
		printf("[BT Audio] ring buffer is full \r\n");
		return ring_buffer->size;
	}
	osif_mutex_take(ring_buffer->mtx, BT_TIMEOUT_FOREVER);
	diff = ring_buffer->last_written_index - ring_buffer->last_read_index;
	if (diff >= 0) {
		osif_mutex_give(ring_buffer->mtx);
		return diff;
	}
	osif_mutex_give(ring_buffer->mtx);
	return diff + ring_buffer->size;
}

bool bt_audio_ring_buffer_empty(bt_audio_ring_buffer_t *ring_buffer)
{
	if (!ring_buffer || !ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has not been initialized \r\n", __func__);
		return 0;
	}
	return (bt_audio_ring_buffer_bytes_read_available(ring_buffer) == 0);
}

uint32_t bt_audio_ring_buffer_bytes_write_available(bt_audio_ring_buffer_t *ring_buffer)
{
	if (!ring_buffer || !ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has not been initialized \r\n", __func__);
		return 0;
	}
	return ring_buffer->size - bt_audio_ring_buffer_bytes_read_available(ring_buffer);
}

uint16_t bt_audio_ring_buffer_write(bt_audio_ring_buffer_t *ring_buffer, uint8_t *data, uint32_t data_length)
{
	unsigned int bytes_until_end = 0;
	unsigned int bytes_to_copy = 0;

	if (!ring_buffer || !ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has not been initialized \r\n", __func__);
		return 1;
	}
	if (bt_audio_ring_buffer_bytes_write_available(ring_buffer) < data_length) {
		printf("[BT Audio] ring buffer free length %d is not enough for data length %d \r\n", (int)bt_audio_ring_buffer_bytes_write_available(ring_buffer),
			   (int)data_length);
		return 1;
	}
	BT_LOGD("write data %d, ava %d \r\n", data_length, bt_audio_ring_buffer_bytes_write_available(ring_buffer));
	/* check data_length */
	if (data_length == 0) {
		printf("[BT Audio] bt audio ring buffer wirte fail, data length is 0 \r\n");
		return 1;
	}
	osif_mutex_take(ring_buffer->mtx, BT_TIMEOUT_FOREVER);
	/* copy first chunk */
	bytes_until_end = ring_buffer->size - ring_buffer->last_written_index;
	bytes_to_copy = bt_audio_min(bytes_until_end, data_length);
	BT_LOGD("bytes_to_copy %d last write index %d \r\n", bytes_to_copy, ring_buffer->last_written_index);
	memcpy((void *)&ring_buffer->storage[ring_buffer->last_written_index],
		   (void *)data, bytes_to_copy);
	data_length -= bytes_to_copy;
	data += bytes_to_copy;
	/* update last written index */
	ring_buffer->last_written_index += bytes_to_copy;
	if (ring_buffer->last_written_index == ring_buffer->size) {
		ring_buffer->last_written_index = 0;
	}
	/* copy second chunk */
	if (data_length != 0) {
		memcpy((void *)&ring_buffer->storage[0], (void *)data, data_length);
		ring_buffer->last_written_index += data_length;
	}
	/* mark buffer as full */
	if (ring_buffer->last_written_index == ring_buffer->last_read_index) {
		ring_buffer->full = 1;
	}
	BT_LOGD("last write index %d \r\n", ring_buffer->last_written_index);
	osif_mutex_give(ring_buffer->mtx);

	return 0;
}

uint32_t bt_audio_ring_buffer_read(bt_audio_ring_buffer_t *ring_buffer, uint8_t *p_out, uint32_t data_length)
{
	uint32_t length = 0;
	unsigned int bytes_until_end = 0;
	unsigned int bytes_to_copy = 0;

	if (!ring_buffer || !ring_buffer->used) {
		BT_LOGE("%s: ring_buff is NULL or has not been initialized \r\n", __func__);
		return 0;
	}
	/* limit data to get and report */
	length = bt_audio_min(data_length, bt_audio_ring_buffer_bytes_read_available(ring_buffer));
	/* simplify logic below by asserting data_length > 0 */
	if (length == 0) {
		printf("[BT Audio] There is no data in bt audio ring buffer \r\n");
		return 0;
	}
	osif_mutex_take(ring_buffer->mtx, BT_TIMEOUT_FOREVER);
	/* copy first chunk */
	bytes_until_end = ring_buffer->size - ring_buffer->last_read_index;
	bytes_to_copy = bt_audio_min(bytes_until_end, length);
	memcpy((void *)p_out, (void *)&ring_buffer->storage[ring_buffer->last_read_index],
		   bytes_to_copy);
	length -= bytes_to_copy;
	p_out += bytes_to_copy;
	/* update last read index */
	ring_buffer->last_read_index += bytes_to_copy;
	if (ring_buffer->last_read_index == ring_buffer->size) {
		ring_buffer->last_read_index = 0;
	}
	// copy second chunk
	if (length != 0) {
		(void)memcpy(p_out, &ring_buffer->storage[0], length);
		ring_buffer->last_read_index += length;
	}
	// clear full flag
	ring_buffer->full = 0;
	osif_mutex_give(ring_buffer->mtx);

	return (bytes_to_copy + length);
}

