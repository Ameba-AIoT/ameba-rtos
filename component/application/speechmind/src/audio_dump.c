#define LOG_TAG "AudioDump"
#include "log/log.h"

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "vfs.h"
#include "osal_c/osal_thread.h"
#include "osal_c/osal_mem.h"
#include "osal_c/osal_time.h"

#include "common/audio_errnos.h"

#include "cutils/ring_buffer.h"

#include "pc_recorder.h"
#include "audio_dump.h"
#include "speech_utils.h"

#define SD_CARD_SAVE_TIMES 100

typedef struct AudioDump {
    bool save_thread_running;
    osal_thread_t* save_thread;
    ring_buffer *save_ring_buffer;
    enum audio_dump_type type;
} AudioDump;

static bool SDDumpTask(void *param)
{
    MEDIA_LOGD("SDDumpTask");
    AudioDump *handle = (AudioDump *)param;

    uint8_t *tmp_data = (uint8_t *)osal_malloc(DUMP_FRAME_BYTES * 32);
    int i = 0;
    FILE *save_file = NULL;
    char path[128] = {0};
    int64_t index = 0;

    while (handle->save_thread_running) {
        while (handle->save_ring_buffer->available(handle->save_ring_buffer) < DUMP_FRAME_BYTES* 32) {
            osal_msleep(5);
            if (!handle->save_thread_running) {
                break;
            }
        }
        handle->save_ring_buffer->read(handle->save_ring_buffer, (uint8_t *)tmp_data, DUMP_FRAME_BYTES* 32);

        if (i == 0) {
            memset(path, 128, 0);
            char *prefix = find_vfs_tag(VFS_REGION_1);
            snprintf(path, sizeof(path), "%s://dump_%lld.pcm", prefix, index);
            MEDIA_LOGD("save path %s", path);
            save_file = fopen(path, "w+");
            if (!save_file) {
                MEDIA_LOGE("save %s fail\n", path);
                break;
            }
            index++;
        }
        size_t res = fwrite(tmp_data, DUMP_FRAME_BYTES* 32, 1, save_file);
        if (res < DUMP_FRAME_BYTES* 32) {
            fclose(save_file);
            MEDIA_LOGE("write %s fail", path);
            break;
        }
        i++;
        if (i == SD_CARD_SAVE_TIMES) {
            fclose(save_file);
            i = 0;
        }
    }

    osal_free(tmp_data);
    return false;
}

static bool PcDumpTask(void *param)
{
    MEDIA_LOGD("PcDumpTask");
    AudioDump *handle = (AudioDump *)param;

    uint8_t *buffer0 = (uint8_t *)osal_malloc(RECORD_PAGE_SIZE);
    uint8_t *buffer1 = (uint8_t *)osal_malloc(RECORD_PAGE_SIZE);
    void* tmp_data;
    int index = 0;

    PCRecorder_init();

    while (handle->save_thread_running) {
        while (handle->save_ring_buffer->available(handle->save_ring_buffer) < RECORD_PAGE_SIZE) {
            osal_msleep(5);
            if (!handle->save_thread_running) {
                break;
            }
        }
        if (index % 2 == 0) {
            tmp_data = buffer0;
        } else {
            tmp_data = buffer1;
        }
        index++;
        handle->save_ring_buffer->read(handle->save_ring_buffer, (uint8_t *)tmp_data, RECORD_PAGE_SIZE);
        PCRecorder_dump(tmp_data);
    }

    PCRecorder_deInit();

    osal_free(buffer0);
    osal_free(buffer1);
    return false;
}

AudioDump* AudioDump_create(enum audio_dump_type type) {
    AudioDump* audio_dump = (AudioDump*)osal_malloc(sizeof(AudioDump));
    if (!audio_dump) {
        MEDIA_LOGE("malloc audio_dump fail\n");
        return NULL;
    }

    audio_dump->save_ring_buffer = ring_buffer_create(1 << 19, RINGBUFFER_LOCAL);
    if (!audio_dump->save_ring_buffer) {
        MEDIA_LOGE("create afe local ring buffer fail");
        osal_free(audio_dump);
        return NULL;
    }

    audio_dump->save_thread_running = false;
    audio_dump->type = type;
    return audio_dump;
}

int32_t AudioDump_start(AudioDump* audio_dump) {
    if (!audio_dump) {
        return AUDIO_ERR_INVALID_OPERATION;
    }
    audio_dump->save_thread_running = true;
    osal_thread_param param;
    param.priority = OSAL_THREAD_PRI_AUDIO;
    param.stack_size = 1024 * 4;
    param.joinable = false;
    param.name = (char *)"DumpTask";
    int res = 0;
    if (audio_dump->type == AUDIO_DUMP_SD_CARD) {
        res = osal_thread_create(&audio_dump->save_thread, SDDumpTask, audio_dump, &param);
    } else {
        res = osal_thread_create(&audio_dump->save_thread, PcDumpTask, audio_dump, &param);
    }
    if (res) {
        MEDIA_LOGE("create dump task fail");
        return AUDIO_ERR_NO_MEMORY;
    }
    return AUDIO_OK;
}

int32_t AudioDump_dump(AudioDump* audio_dump, void* data, int32_t length) {
    if (!audio_dump) {
        MEDIA_LOGE("manager is NULL");
        return AUDIO_ERR_INVALID_PARAM;
    }

    int space = audio_dump->save_ring_buffer->space(audio_dump->save_ring_buffer);
    if (space >= length) {
        audio_dump->save_ring_buffer->write(audio_dump->save_ring_buffer, data, length);
        return AUDIO_OK;
    } else {
        MEDIA_LOGE("space not would block");
        return AUDIO_ERR_WOULD_BLOCK;
    }
}

int32_t AudioDump_stop(AudioDump* audio_dump) {
    if (audio_dump->save_thread) {
        audio_dump->save_thread_running = false;
        osal_thread_request_exitAndWait(audio_dump->save_thread);
        audio_dump->save_thread = NULL;
    }
    return AUDIO_OK;
}

int32_t AudioDump_destroy(AudioDump* audio_dump) {
    if (audio_dump->save_thread) {
        AudioDump_stop(audio_dump);
    }

    if (audio_dump) {
        osal_free(audio_dump);
    }
    return AUDIO_OK;
}