#define LOG_TAG "SpeechC"

#include "log/log.h"
#include "osal_c/osal_mem.h"

#include <stdlib.h>
#include "cJSON/cJSON.h"

#include "ameba_soc.h"

#include "audio/audio_record.h"
#include "audio/audio_control.h"

#include "speech_mind.h"
#include "speech_config.h"

#include "vfs.h"

#define PARAM_FILE "vfs://param.json"
#define PARAM_FILE_SIZE_MAX 128

static SpeechConfig g_config;
static void SpeechConfig_setDefaultParam(void)
{
    g_config.kws_config.key_words_num = 2;
    int key_words_length[2];
    key_words_length[0] = strlen("ni-hao-xiao-qiang") + 1;
    key_words_length[1] = strlen("xiao-qiang-xiao-qiang") + 1;
    int total_length = key_words_length[0] + key_words_length[1];
    MEDIA_LOGD("total_length %d", total_length);

    g_config.kws_config.key_words_length = (int *)osal_malloc(total_length * sizeof(int));
    g_config.kws_config.key_words_total = (char *)osal_malloc(total_length * sizeof(char));
    memcpy(g_config.kws_config.key_words_total, "ni-hao-xiao-qiang", key_words_length[0]);
    memcpy(g_config.kws_config.key_words_total + key_words_length[0], "xiao-qiang-xiao-qiang", key_words_length[1]);
    MEDIA_LOGD("%s %s %s", __FUNCTION__, g_config.kws_config.key_words_total, g_config.kws_config.key_words_total + key_words_length[0]);

    g_config.kws_config.key_words_length[0] = key_words_length[0];
    g_config.kws_config.key_words_length[1] = key_words_length[1];

    g_config.kws_config.thresholds_num = 2;
    g_config.kws_config.thresholds = (float *)osal_malloc(g_config.kws_config.thresholds_num * sizeof(float));
    g_config.kws_config.thresholds[0] = 0.1;
    g_config.kws_config.thresholds[1] = 0.1;
}

void SpeechConfig_save(void)
{
    cJSON *root;
    FILE *param_file = fopen(PARAM_FILE, "r+");
    if (param_file) {
        char tmp[PARAM_FILE_SIZE_MAX];
        size_t res = fread(tmp, PARAM_FILE_SIZE_MAX, 1, param_file);
        if (res < PARAM_FILE_SIZE_MAX) {
            MEDIA_LOGE("read error");
            fclose(param_file);
            return;
        }

        root = cJSON_Parse(tmp);
        if (!root) {
            MEDIA_LOGE("parse error");
            fclose(param_file);
            return;
        }

        cJSON *key_words_obj = cJSON_GetObjectItem(root, "key_words");
        if (key_words_obj) {
            cJSON_DeleteItemFromObject(root, "key_words");
        }
        cJSON *thresholds_obj = cJSON_GetObjectItem(root, "thresholds");
        if (thresholds_obj) {
            cJSON_DeleteItemFromObject(root, "thresholds");
        }
        fclose(param_file);
    } else {
        MEDIA_LOGE("%s is not exit, create new one.", PARAM_FILE);
        root = cJSON_CreateObject();
    }

    char **key_words = (char **)osal_malloc(g_config.kws_config.key_words_num * sizeof(char *));
    char *temp_key_words = g_config.kws_config.key_words_total;
    for (int i = 0; i < g_config.kws_config.key_words_num; i++) {
        key_words[i] = (char *)osal_malloc(g_config.kws_config.key_words_length[i] * sizeof(char));
        memcpy(key_words[i], temp_key_words, g_config.kws_config.key_words_length[i]);
        temp_key_words += g_config.kws_config.key_words_length[i];
    }
    cJSON *key_words_array = cJSON_CreateStringArray((const char *const *)key_words, g_config.kws_config.key_words_num);
    cJSON_AddItemToObject(root, "key_words", key_words_array);

    for (int i = 0; i < g_config.kws_config.key_words_num; i++) {
        osal_free(key_words[i]);
    }
    osal_free(key_words);

    double *thresholds = (double *)osal_malloc(g_config.kws_config.thresholds_num * sizeof(double));
    for (int i = 0; i < g_config.kws_config.thresholds_num; i++) {
        thresholds[i] = g_config.kws_config.thresholds[i];
    }
    cJSON *thresholds_array = cJSON_CreateDoubleArray(thresholds, g_config.kws_config.thresholds_num);
    cJSON_AddItemToObject(root, "thresholds", thresholds_array);

    osal_free(thresholds);

    char *updated_json = cJSON_Print(root);
    updated_json[strlen(updated_json) + 1] = '\0';

    MEDIA_LOGD("new json %s", updated_json);

    cJSON_Delete(root);

    param_file = fopen(PARAM_FILE, "w");
    if (!param_file) {
        MEDIA_LOGE("%s is not exist.", PARAM_FILE);
        return;
    }
    size_t res = fwrite(updated_json, strlen(updated_json) + 1, 1, param_file);
    if (res < strlen(updated_json) + 1) {
        MEDIA_LOGE("write fail");
    }
    fclose(param_file);
    osal_free(updated_json);
}

void SpeechConfig_load(void)
{
    g_config.afe = 1;
    g_config.rpc = 1;
    g_config.amplifier_volume = 0.8f;

    FILE *param_file = fopen(PARAM_FILE, "r+");
    if (!param_file) {
        MEDIA_LOGE("%s is not exist. creat a new one", PARAM_FILE);
        SpeechConfig_setDefaultParam();
        SpeechConfig_save();
        return;
    }

    char tmp[PARAM_FILE_SIZE_MAX];
    size_t res = fread(tmp, PARAM_FILE_SIZE_MAX, 1, param_file);
    if (res < PARAM_FILE_SIZE_MAX) {
        tmp[res] = '\0';
    }

    cJSON *root = cJSON_Parse(tmp);
    if (!root) {
        MEDIA_LOGE("parse error");
        fclose(param_file);
        return;
    }

    cJSON *amplifier_volume = cJSON_GetObjectItem(root, "amplifier_volume");
    if (!amplifier_volume) {
        MEDIA_LOGE("parse volume fail.");
        g_config.amplifier_volume = 0.8f;
    } else {
        g_config.amplifier_volume = (float)amplifier_volume->valuedouble;
        MEDIA_LOGD("amplifier_volume is %d.", (int)(g_config.amplifier_volume * 10));
    }

    cJSON *key_words_obj = cJSON_GetObjectItem(root, "key_words");
    if (!key_words_obj) {
        MEDIA_LOGE("parse key_words fail");
        fclose(param_file);
        return;
    } else {
        MEDIA_LOGD("parse key_words success");
        int key_words_nums = cJSON_GetArraySize(key_words_obj);
        g_config.kws_config.key_words_num = key_words_nums;
        MEDIA_LOGD("parse key_words_nums %d", key_words_nums);
        int total_key_words_length = 0;
        g_config.kws_config.key_words_length = (int *)osal_malloc(g_config.kws_config.key_words_num * sizeof(int));

        for (int i = 0; i < key_words_nums; i++) {
            cJSON *key_word_obj = cJSON_GetArrayItem(key_words_obj, i);
            if (cJSON_IsString(key_word_obj) && (key_word_obj->valuestring != NULL)) {
                g_config.kws_config.key_words_length[i] = strlen(key_word_obj->valuestring) + 1;
                total_key_words_length += g_config.kws_config.key_words_length[i];
            }
        }

        g_config.kws_config.key_words_total = (char *)osal_malloc(total_key_words_length * sizeof(char));
        char *temp_key_words = g_config.kws_config.key_words_total;
        for (int i = 0; i < key_words_nums; i++) {
            cJSON *key_word_obj = cJSON_GetArrayItem(key_words_obj, i);
            if (cJSON_IsString(key_word_obj) && (key_word_obj->valuestring != NULL)) {
                memcpy(temp_key_words, key_word_obj->valuestring, g_config.kws_config.key_words_length[i]);
                temp_key_words += g_config.kws_config.key_words_length[i];
                MEDIA_LOGD("g_config.kws_config.key_words_length[%d] %d", i, g_config.kws_config.key_words_length[i]);
            }
        }
    }

    cJSON *thresholds_obj = cJSON_GetObjectItem(root, "thresholds");
    if (!thresholds_obj) {
        MEDIA_LOGE("parse thresholds fail");
        fclose(param_file);
        return;
    } else {
        MEDIA_LOGD("parse thresholds success");
        int thresholds_num = cJSON_GetArraySize(thresholds_obj);
        g_config.kws_config.thresholds_num = thresholds_num;
        g_config.kws_config.thresholds = (float *)osal_malloc(g_config.kws_config.thresholds_num * sizeof(float));
        for (int i = 0; i < thresholds_num; i++) {
            cJSON *threshold_obj = cJSON_GetArrayItem(thresholds_obj, i);
            g_config.kws_config.thresholds[i] = threshold_obj->valuedouble;
            MEDIA_LOGD("parse g_config.kws_config.thresholds[%d] %f", i, g_config.kws_config.thresholds[i]);
        }
    }

    cJSON_Delete(root);
    fclose(param_file);
    MEDIA_LOGD("parse finish");
}

SpeechConfig *SpeechConfig_getSpeechConfig(void)
{
    return &g_config;
}

float SpeechConfig_getAmplifierVolume(void)
{
    return g_config.amplifier_volume;
}

void speech_mind_config_handler(uint16_t argc, uint8_t *argv[])
{
    MEDIA_LOGD("-------cmd param--------");
    (void) argc;
    while (*argv) {
        if (strcmp((const char *)*argv, "-k") == 0) {
            MEDIA_LOGD("-------key words param--------");
            argv++;
            if (*argv) {
                g_config.kws_config.key_words_num = atoi((const char *)*argv);
                MEDIA_LOGD("key words num %d", g_config.kws_config.key_words_num);
            }
            int total_key_words_length = 0;
            for (int i = 0; i < g_config.kws_config.key_words_num; i++) {
                argv++;
                if (*argv) {
                    total_key_words_length += (strlen((const char *)*argv) + 1);
                }
            }
            MEDIA_LOGD("total key words length %d", total_key_words_length);
            for (int i = 0; i < g_config.kws_config.key_words_num; i++) {
                argv--;
            }
            if (g_config.kws_config.key_words_total) {
                osal_free(g_config.kws_config.key_words_total);
            }
            if (g_config.kws_config.key_words_length) {
                osal_free(g_config.kws_config.key_words_length);
            }
            g_config.kws_config.key_words_total = (char *)osal_malloc((total_key_words_length) * sizeof(char));
            g_config.kws_config.key_words_length = (int *)osal_malloc((total_key_words_length) * sizeof(int));
            char *temp = g_config.kws_config.key_words_total;
            for (int i = 0; i < g_config.kws_config.key_words_num; i++) {
                argv++;
                g_config.kws_config.key_words_length[i] = (strlen((const char *)*argv) + 1);
                memcpy(temp, *argv, strlen((const char *)*argv) + 1);
                temp += strlen((const char *)*argv) + 1;
                MEDIA_LOGD("[%d] key words is %s", i, *argv);
            }
        }
        if (strcmp((const char *)*argv, "-t") == 0) {
            MEDIA_LOGD("-------thresholds param--------");
            argv++;
            if (*argv) {
                g_config.kws_config.thresholds_num = atoi((const char *)*argv);
                MEDIA_LOGD("thresholds num %d", g_config.kws_config.thresholds_num);
                if (g_config.kws_config.thresholds_num != g_config.kws_config.key_words_num) {
                    MEDIA_LOGE("thresholds num err");
                    MEDIA_LOGE("thresholds num is %d, but key words num is %d", g_config.kws_config.thresholds_num, g_config.kws_config.key_words_num);
                    if (g_config.kws_config.key_words_total) {
                        osal_free(g_config.kws_config.key_words_total);
                    }
                    if (g_config.kws_config.key_words_length) {
                        osal_free(g_config.kws_config.key_words_length);
                    }
                }
                if (g_config.kws_config.thresholds) {
                    osal_free(g_config.kws_config.thresholds);
                }
                g_config.kws_config.thresholds = (float *)osal_malloc(g_config.kws_config.thresholds_num * sizeof(float));
                for (int i = 0; i < g_config.kws_config.thresholds_num; i++) {
                    argv++;
                    g_config.kws_config.thresholds[i] = atof((const char *)*argv);
                    MEDIA_LOGD("g_config.kws_config.thresholds[%d] is %f", i, g_config.kws_config.thresholds[i]);
                }
            }
        }
        if (*argv) {
            argv++;
        }
    }
}

u32 speech_mind_config_test(u16 argc, u8 *argv[])
{
    MEDIA_LOGV("%s Enter\n", __FUNCTION__);
    speech_mind_config_handler(argc, argv);
    SpeechConfig_save();

    SpeechMind_stop();
    SpeechMind_start();

    return 1;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE speech_test_cmd_table[] = {
    {
        (const u8 *)"speech_mind_config",  1, speech_mind_config_test, (const u8 *)"\t speech_mind_config -k keywords_num keywords1 keywords2 -t threshold_num threshold1 threshold2\n"
    },
};


