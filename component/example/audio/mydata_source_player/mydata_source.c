#define LOG_TAG "MyDataSource"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mydata_source.h"

#include "common/audio_errnos.h"

/* prepare delay simulation */
//#define MDS_PREPARE_DELAY_TEST

/* source read unsmooth simulation */
//#define MDS_READ_UNSMOOTH_TEST

/* unknown length source simulation */
//#define MDS_UNKNOWN_LENGTH_TEST

#define MDS_SLEEP_TIME_MS 10

#ifdef MDS_PREPARE_DELAY_TEST
int kMDSPrepareDelayTimeMs = 3000;
volatile int g_prepared = 0;
volatile bool g_prepare_thread_alive = 0;
void MyDataSource_PrepareTestThread(void *Data);
#endif

#ifdef MDS_READ_UNSMOOTH_TEST
#define MDS_READ_RETRY_NUM 200
int kMDSReadCount = 0;
int kMDSReadRetry = 0;
#endif

#ifdef MDS_UNKNOWN_LENGTH_TEST
int kMDSLengthIncreaseTotalTimeMs = 60000;
volatile int g_source_total_length = 0;
volatile bool g_length_increase_thread_alive = 0;
void MyDataSource_UnknownLengthTestThread(void *Data);
#endif

void MyDataSource_WaitLoopExit();


// ----------------------------------------------------------------------
// MyDataSource
RTDataSource *MyDataSource_Create(char *data, int length)
{
	printf("MyDataSource_Create(%p, %d)\n", data, length);
	if (!data) {
		printf("invalid source or ring_buffer\n");
		return NULL;
	}

	MyDataSource *data_source = calloc(1, sizeof(MyDataSource));
	if (!data_source) {
		printf("fail to alloc MyDataSource.\n");
		return NULL;
	}

	data_source->base.CheckPrepared = MyDataSource_CheckPrepared;
	data_source->base.ReadAt = MyDataSource_ReadAt;
	data_source->base.GetLength = MyDataSource_GetLength;

	data_source->data = data;
	data_source->data_length = length;
	data_source->alive = 1;

#ifdef MDS_UNKNOWN_LENGTH_TEST
	data_source->data_length = 0;
	data_source->unknown_data_length = 1;
	g_source_total_length = length;
	g_length_increase_thread_alive = 0;
	printf("g_source_total_length: %d, last_data_gained: %d\n", g_source_total_length, data_source->last_data_gained);
	rtos_task_create(NULL, "MyDataSource_UnknownLengthTestThread", MyDataSource_UnknownLengthTestThread, (void *)data_source, 2048 * 4, 0);
#endif

	printf("length: %d, unknown_data_length: %d\n", data_source->data_length, data_source->unknown_data_length);

#ifdef MDS_PREPARE_DELAY_TEST
	g_prepared = 0;
	g_prepare_thread_alive = 0;
	rtos_task_create(NULL, "MyDataSource_PrepareTestThread", MyDataSource_PrepareTestThread, (void *)data_source, 2048 * 4, 0);
#endif

	return (RTDataSource *)data_source;
}

void MyDataSource_Destroy(MyDataSource *source)
{
	if (!source) {
		return;
	}

	printf("MyDataSource_Destroy\n");

	source->alive = 0;

	MyDataSource_WaitLoopExit();

	free((void *)source);
	source = NULL;
}

int32_t MyDataSource_CheckPrepared(const RTDataSource *source)
{
	if (!source) {
		return AUDIO_ERR_NO_INIT;
	}

#ifdef MDS_PREPARE_DELAY_TEST
	if (!g_prepared) {
		return AUDIO_ERR_NO_INIT;
	}
#endif

	MyDataSource *data_source = (MyDataSource *)source;

	return data_source->data ? AUDIO_OK : AUDIO_ERR_NO_INIT;
}

ssize_t MyDataSource_ReadAt(const RTDataSource *source, off_t offset, void *data, size_t size)
{
	if (!source || !data || !size) {
		printf("ReadAt invalid param, source: %p, data: %p, size: %d\n", source, data, size);
		return (ssize_t)AUDIO_ERR_INVALID_OPERATION;
	}

	//printf("MyDataSource_ReadAt offset: %d, size: %d\n", offset, size);

	MyDataSource *data_source = (MyDataSource *)source;

	if (offset >= data_source->data_length) {
		if (data_source->unknown_data_length && !data_source->last_data_gained) {
			//printf("ReadAt offset(%d) beyond unknown length data, now data_length(%d)\n", offset, data_source->data_length);
			return (ssize_t)RTDATA_SOURCE_READ_AGAIN;
		}
		printf("ReadAt offset(%d) beyond data length(%d), unknown_length(%d), source(%p), data(%p)\n",
			   offset,
			   data_source->data_length, data_source->unknown_data_length,
			   data_source, data);
		return (ssize_t)RTDATA_SOURCE_EOF;
	}

	if ((data_source->data_length - offset) < size) {
		//printf("free size %d is smaller than read size %d, so change read size\n", data_source->data_length - offset, size);
		size = data_source->data_length - offset;
	}

#ifdef MDS_READ_UNSMOOTH_TEST
	if (kMDSReadCount % 10 == 0) {
		kMDSReadRetry++;
		if (kMDSReadRetry == MDS_READ_RETRY_NUM) {
			kMDSReadRetry = 0;
			kMDSReadCount++;
		}
		printf("read again %d-%d\n", kMDSReadCount, kMDSReadRetry);
		return (ssize_t)RTDATA_SOURCE_READ_AGAIN;
	}
#endif

	//printf("memcpy %p, %p, %d\n", data, data_source->data + offset, (size_t)size);
	memcpy(data, data_source->data + offset, (size_t)size);

#ifdef MDS_READ_UNSMOOTH_TEST
	kMDSReadCount++;
#endif

	return size;
}

int32_t MyDataSource_GetLength(const RTDataSource *source, off_t *size)
{
	if (!source) {
		return RTDATA_SOURCE_FAIL;
	}

	MyDataSource *data_source = (MyDataSource *)source;
	*size = data_source->data_length;

	if (data_source->unknown_data_length && !data_source->last_data_gained) {
		return RTDATA_SOURCE_UNKNOWN_LENGTH;
	}

	return 0;
}


// ----------------------------------------------------------------------
// Private Interfaces
void MyDataSource_WaitLoopExit()
{
	int count = 10 * 1000 / MDS_SLEEP_TIME_MS; //wait 3s

#ifdef MDS_UNKNOWN_LENGTH_TEST
#endif
	while (count) {
		int exit = 1;
#ifdef MDS_PREPARE_DELAY_TEST
		if (g_prepare_thread_alive) {
			exit = 0;
		}
#endif
#ifdef MDS_PREPARE_DELAY_TEST
		if (g_length_increase_thread_alive) {
			exit = 0;
		}
#endif
		if (exit) {
			break;
		}

		rtos_time_delay_ms(MDS_SLEEP_TIME_MS);
		//printf("wait task exit time=%d.\n", count);
		count--;
	}
}

#ifdef MDS_PREPARE_DELAY_TEST
void MyDataSource_PrepareTestThread(void *Data)
{
	MyDataSource *data_source = (MyDataSource *)Data;
	printf("[MyDataSource_PrepareTestThread] start\n");
	g_prepare_thread_alive = 1;

	unsigned int count = kMDSPrepareDelayTimeMs / MDS_SLEEP_TIME_MS;
	do {
		rtos_time_delay_ms(MDS_SLEEP_TIME_MS);
		count--;
	} while (count && data_source->alive);

	g_prepared = 1;
	g_prepare_thread_alive = 0;
	printf("[MyDataSource_PrepareTestThread] exit\n");
	rtos_task_delete(NULL);
}
#endif

#ifdef MDS_UNKNOWN_LENGTH_TEST
void MyDataSource_UnknownLengthTestThread(void *Data)
{
	MyDataSource *data_source = (MyDataSource *)Data;
	printf("[MyDataSource_UnknownLengthTestThread] start, g_source_total_length: %d\n", g_source_total_length);

	g_length_increase_thread_alive = 1;
	int count = kMDSLengthIncreaseTotalTimeMs / MDS_SLEEP_TIME_MS;
	int block_length = g_source_total_length / count;
	int delta = g_source_total_length % count;

	printf("count(%d), block_length(%d), delta(%d), data_source->data_length(%d)\n", count, block_length, delta, data_source->data_length);
	do {
		data_source->data_length += block_length;
		count--;
		rtos_time_delay_ms(MDS_SLEEP_TIME_MS);
	} while (count > 0 && data_source->alive);

	data_source->data_length += delta;
	data_source->last_data_gained = 1;
	g_length_increase_thread_alive = 0;
	printf("[MyDataSource_UnknownLengthTestThread] exit, total data_length: %d\n", data_source->data_length);
	rtos_task_delete(NULL);
}
#endif
