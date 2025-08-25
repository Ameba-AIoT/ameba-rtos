#include <math.h>

#include "serial_api.h"
#include "serial_ex_api.h"
#include "PinNames.h"
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "cJSON/cJSON.h"
#include "audio/audio_record.h"
#include "audio/audio_track.h"
#include "audio/audio_control.h"
#include "platform_stdlib.h"
#include "basic_types.h"

#include "audio/audio_control.h"
#include "audio/audio_equalizer.h"
#include "audio/audio_track.h"
#include "audio/audio_service.h"

#include "audio_uart_handler.h"

#define USING_CMD      1
#define EXAMPLE_AUDIO_DEBUG(fmt, args...)    printf("=> D/AudioUart:[%s]: " fmt "\n", __func__, ## args)
#define EXAMPLE_AUDIO_ERROR(fmt, args...)    printf("=> E/AudioUart:[%s]: " fmt "\n", __func__, ## args)

#define  RTAUDIO_UART_DEBUG_HEAP_BEGIN() \
	unsigned int heap_start;\
	unsigned int heap_end;\
	unsigned int heap_min_ever_free;\
	EXAMPLE_AUDIO_DEBUG("[Mem] mem debug info init");\
	heap_start = rtos_mem_get_free_heap_size()

#define  RTAUDIO_UART_DEBUG_HEAP_END() \
    heap_end = rtos_mem_get_free_heap_size();\
	heap_min_ever_free = rtos_mem_get_minimum_ever_free_heap_size();\
	EXAMPLE_AUDIO_DEBUG("[Mem] start (0x%x), end (0x%x), ", heap_start, heap_end);\
	EXAMPLE_AUDIO_DEBUG(" diff (%d), peak (%d)", heap_start - heap_end, heap_start - heap_min_ever_free)


static int32_t        g_receive_msg_start_cnt = 0;
static int32_t        g_receive_msg_end_cnt = 0;
static unsigned char *g_receive_msg_data = NULL;
static int32_t        g_receive_data_size = 0;
static rtos_sema_t    g_receive_data_sema;
static rtos_sema_t    g_send_data_sema;
static rtos_mutex_t   g_send_data_mutex;
static serial_t       g_serial_obj;
static int32_t        g_receive_cnt = 0;
static int32_t        g_max_receive_cnt = 0x7fffffff;

struct audio_uart_msg {
	int32_t item;
	char *item_str;
};

typedef struct msg_attrib_s {
	int32_t type;
} msg_attrib_t;

const struct audio_uart_msg eq_msg_type[] = {
	{AUDIO_UART_MSG_ACK, 		     "ack"		    },
	{AUDIO_UART_MSG_ERROR, 	         "error"		},
	{AUDIO_UART_MSG_SET_FREQUENCY,   "setfrequency"	},
	{AUDIO_UART_MSG_SET_GAIN, 		 "setgain"		},
	{AUDIO_UART_MSG_SETQFACTOR, 	 "setqfactor"   },
	{AUDIO_UART_MSG_SETFILTERTYPE,	 "setfiltertype"},
	{AUDIO_UART_MSG_QUERY,           "query"        },
};

static bool audio_uart_check_cjson(unsigned char *inbuf, int32_t len)
{
	char p[AUDIO_UART_RECEIVE_DATA_BUFLEN], *p1;
	int32_t left = 0, right = 0;

	memcpy(p, inbuf, len);
	p1 = p;

	while (*p1 != '\0') {
		if (*p1 == '{') {
			left++;
		}

		if (*p1 == '}') {
			right++;
		}
		p1++;

		if (left == right) {
			break;
		}
	}

	if (left == right && (left != 0)) {
		return TRUE;
	}
	return FALSE;
}

static void audio_uart_send_data(char *pstr, int32_t len)
{
#if AUDIO_UART_USE_DMA_TX
	int32_t ret = 0;
	rtos_sema_take(g_send_data_sema, RTOS_MAX_TIMEOUT);

	ret = serial_send_stream_dma(&g_serial_obj, pstr, len);
	if (ret != 0) {
		printf("%s Error(%ld)\n", __FUNCTION__, ret);
	}
#else
	while (len) {
		//printf("send through uart \n");
		serial_putc(&g_serial_obj, *pstr);
		pstr++;
		len--;
	}
#endif
}

static int32_t audio_uart_eq_query(struct RTAudioEqualizer *audio_equalizer)
{
	uint32_t eq_bands = 10;
	cJSON *root;
	cJSON *cj_band[eq_bands];
	char *msg_js = NULL;

	uint32_t gain[eq_bands];
	uint32_t frequency[eq_bands];
	uint32_t qfactor[eq_bands];
	uint32_t type[eq_bands];
	//get above values from eq module.

	for (uint32_t i = 0; i < eq_bands; i++) {
		gain[i] = RTAudioEqualizer_GetBandLevel(audio_equalizer, i);
		frequency[i] = RTAudioEqualizer_GetCenterFreq(audio_equalizer, i);
		qfactor[i] = RTAudioEqualizer_GetQfactor(audio_equalizer, i);
		type[i] = 0;
	}

	if ((root = cJSON_CreateObject()) == NULL) {
		return -1;
	}

	cJSON_AddStringToObject(root, "type", "query");

	for (uint32_t i = 0; i < eq_bands; i++) {
		char object_name[6];
		sprintf(object_name, "band%ld", (int32_t)i);
		cj_band[i] = cJSON_AddObjectToObject(root, object_name);
		cJSON_AddNumberToObject(cj_band[i], "centerfrequency", frequency[i]);
		cJSON_AddNumberToObject(cj_band[i], "gain", gain[i] / 100);
		cJSON_AddDoubleToObject(cj_band[i], "qfactor", (double)qfactor[i] / (double)100);
		cJSON_AddNumberToObject(cj_band[i], "filtertype", type[i]);
	}

	msg_js = cJSON_Print(root);

	cJSON_Delete(root);

	EXAMPLE_AUDIO_DEBUG("%s, %s\n", __func__, msg_js);
	rtos_mutex_take(g_send_data_mutex, MUTEX_WAIT_TIMEOUT);
	audio_uart_send_data(msg_js, strlen(msg_js));
	rtos_mutex_give(g_send_data_mutex);
	rtos_mem_free(msg_js);

	return 0;
}

static int32_t audio_uart_ack_reply(int32_t opt)
{
	cJSON *msg_obj;
	char *msg_js = NULL;

	if ((msg_obj = cJSON_CreateObject()) == NULL) {
		return -1;
	}

	if (opt >= 0) {
		cJSON_AddStringToObject(msg_obj, "type", "ack");
	} else {
		cJSON_AddStringToObject(msg_obj, "type", "error");
	}

	msg_js = cJSON_Print(msg_obj);
	cJSON_Delete(msg_obj);

	EXAMPLE_AUDIO_DEBUG("%s, %s\n", __func__, msg_js);
	rtos_mutex_take(g_send_data_mutex, MUTEX_WAIT_TIMEOUT);
	audio_uart_send_data(msg_js, strlen(msg_js));
	EXAMPLE_AUDIO_DEBUG("%s, send ack done(%d)\n", __func__, strlen(msg_js));
	rtos_mutex_give(g_send_data_mutex);

	rtos_mem_free(msg_js);
	return 0;
}

static void audio_uart_process_receive_data(msg_attrib_t *pattrib, struct RTAudioEqualizer *audio_equalizer)
{
	unsigned char tempbuf[AUDIO_UART_RECEIVE_DATA_BUFLEN];
	cJSON *root, *typeobj;
	int32_t opt = -1;
	int32_t datasize = g_receive_data_size;

	memset(tempbuf, 0x00, AUDIO_UART_RECEIVE_DATA_BUFLEN);
	memcpy(tempbuf, g_receive_msg_data, datasize);
	tempbuf[datasize++] = '\0';
	g_receive_data_size = 0;

	EXAMPLE_AUDIO_DEBUG("%s, msg(%ld): %s\n", __func__, datasize, tempbuf);

	if (datasize == 0) {
		EXAMPLE_AUDIO_DEBUG("null data\n");
		return;
	}

	if (audio_uart_check_cjson(tempbuf, datasize) == FALSE) {
		EXAMPLE_AUDIO_DEBUG("invaild json %s\n", tempbuf);
		return;
	}

	if ((root = cJSON_Parse((const char *)tempbuf)) != NULL) {
		if ((typeobj = cJSON_GetObjectItem(root, "type")) != NULL) {
			EXAMPLE_AUDIO_DEBUG("type: %s\n", typeobj->valuestring);
			for (uint32_t i = 0; i < sizeof(eq_msg_type) / sizeof(struct audio_uart_msg); i++) {
				if (!strcmp(typeobj->valuestring, eq_msg_type[i].item_str)) {
					pattrib->type = eq_msg_type[i].item;
					break;
				}
			}
		}
	}

	switch (pattrib->type) {
	case AUDIO_UART_MSG_QUERY:
		opt = audio_uart_eq_query(audio_equalizer);
		if (opt != 0) {
			audio_uart_ack_reply(opt);
		}
		break;
	case AUDIO_UART_MSG_SET_FREQUENCY: {
		cJSON *cj_band_id = cJSON_GetObjectItem(root, "id");
		cJSON *cj_frequency = cJSON_GetObjectItem(root, "centerfrequency");

		uint32_t band_id = cj_band_id->valueint;
		uint32_t frequency = cj_frequency->valueint;

		RTAudioEqualizer_SetCenterFreq(audio_equalizer, band_id, frequency);
	}
	audio_uart_ack_reply(0);
	break;
	case AUDIO_UART_MSG_SET_GAIN: {
		cJSON *cj_band_id = cJSON_GetObjectItem(root, "id");
		cJSON *cj_gain = cJSON_GetObjectItem(root, "gain");
		uint32_t band_id = cj_band_id->valueint;
		uint32_t gain = cj_gain->valueint;

		RTAudioEqualizer_SetBandLevel(audio_equalizer, band_id, gain * 100);
	}
	audio_uart_ack_reply(0);
	break;
	case AUDIO_UART_MSG_SETQFACTOR: {
		cJSON *cj_band_id = cJSON_GetObjectItem(root, "id");
		cJSON *cj_qfactor = cJSON_GetObjectItem(root, "qfactor");
		uint32_t band_id = cj_band_id->valueint;
		double q_factor = cj_qfactor->valuedouble;

		RTAudioEqualizer_SetQfactor(audio_equalizer, band_id, (uint32_t)(q_factor * (double)100));
	}
	audio_uart_ack_reply(0);
	break;
	case AUDIO_UART_MSG_SETFILTERTYPE: {
		cJSON *cj_band_id = cJSON_GetObjectItem(root, "id");
		cJSON *cj_filter_type = cJSON_GetObjectItem(root, "filtertype");
		uint32_t band_id = cj_band_id->valueint;
		uint32_t filter_type = cj_filter_type->valueint;

		//set type here.do nothing.
		(void) band_id;
		(void) filter_type;
	}
	audio_uart_ack_reply(0);
	break;
	default:
		EXAMPLE_AUDIO_DEBUG("%s, unsupport type\n", __func__);
		audio_uart_ack_reply(opt);
	}

	cJSON_Delete(root);
}

static void audio_uart_receive_irq(uint32_t id, SerialIrq event)
{
	serial_t    *sobj = (serial_t *)id;
	unsigned char rc = 0;

	if (event == RxIrq) {
		rc = serial_getc(sobj);
		if (rc == '{') {
			g_receive_msg_start_cnt++;
		} else if (rc == '}') {
			g_receive_msg_end_cnt++;
		}

		g_receive_msg_data[g_receive_data_size++] = rc;

		if ((g_receive_msg_start_cnt != 0) && g_receive_msg_start_cnt == g_receive_msg_end_cnt) {
			g_receive_msg_start_cnt = 0;
			g_receive_msg_end_cnt = 0;
			g_receive_msg_data[g_receive_data_size++] = '\0';
			rtos_sema_give(g_receive_data_sema);
			return;
		}
	}
}

static void uart_dma_tx_done(uint32_t id)
{
	(void) id;
	rtos_sema_give(g_send_data_sema);
}

static void audio_uart_init_uart(void)
{
	g_serial_obj.uart_idx = AUDIO_UART_INDEX;
	serial_init(&g_serial_obj, AUDIO_UART_TX_PIN, AUDIO_UART_RX_PIN);
	serial_baud(&g_serial_obj, AUDIO_UART_BAUDRATE);
	serial_format(&g_serial_obj, 8, ParityNone, 1);

	serial_rx_fifo_level(&g_serial_obj, FifoLvHalf);
	serial_set_flow_control(&g_serial_obj, FlowControlNone, 0, 0);
	serial_irq_handler(&g_serial_obj, audio_uart_receive_irq, (uint32_t)&g_serial_obj);
	serial_irq_set(&g_serial_obj, RxIrq, ENABLE);
#if AUDIO_UART_USE_DMA_TX
	serial_send_comp_handler(&g_serial_obj, (void *)uart_dma_tx_done, (uint32_t) &g_serial_obj);
#endif
}

static void audio_uart_create_eq(struct RTAudioEqualizer **audio_equalizer)
{

	int16_t band_level = 0;
	int32_t center_freq = 0;
	int32_t qfactor = 0;
	int16_t band_index = 0;

	RTAudioService_Init();

	*audio_equalizer = RTAudioEqualizer_Create();
	RTAudioEqualizer_Init(*audio_equalizer, 0, 0);
	int16_t bands = RTAudioEqualizer_GetNumberOfBands(*audio_equalizer);
	EXAMPLE_AUDIO_DEBUG("total bands:%d", bands);

	int16_t *range = RTAudioEqualizer_GetBandLevelRange(*audio_equalizer);
	EXAMPLE_AUDIO_DEBUG("band range:(%d, %d)", *range, *(range + 1));
	free(range);
	range = NULL;

	RTAudioEqualizer_SetEnabled(*audio_equalizer, true);

	for (; band_index < bands; band_index++) {
		band_level = RTAudioEqualizer_GetBandLevel(*audio_equalizer, band_index);
		EXAMPLE_AUDIO_DEBUG("band %d level:%d", band_index, band_level);

		center_freq = RTAudioEqualizer_GetCenterFreq(*audio_equalizer, band_index);
		EXAMPLE_AUDIO_DEBUG("band:%d, center freq:%ld", band_index, center_freq);

		qfactor = RTAudioEqualizer_GetQfactor(*audio_equalizer, band_index);
		EXAMPLE_AUDIO_DEBUG("band:%d, qfactor:%ld", band_index, qfactor);

	}

}

static void audio_uart_destroy_eq(struct RTAudioEqualizer **audio_equalizer)
{
	RTAudioEqualizer_SetEnabled(*audio_equalizer, false);
	RTAudioEqualizer_Destroy(*audio_equalizer);
}

static void audio_uart_task(void *param)
{
	rtos_time_delay_ms(3000);
	EXAMPLE_AUDIO_DEBUG("Audio uart demo begin");
	(void) param;

	RTAUDIO_UART_DEBUG_HEAP_BEGIN();

	g_receive_msg_data = (unsigned char *) calloc(AUDIO_UART_RECEIVE_DATA_BUFLEN, sizeof(unsigned char));

	struct RTAudioEqualizer *audio_equalizer;
	audio_uart_create_eq(&audio_equalizer);

#if AUDIO_UART_USE_DMA_TX
	rtos_sema_create(&g_send_data_sema, 1, RTOS_SEMA_MAX_COUNT);
#endif
	rtos_sema_create(&g_receive_data_sema, 0, RTOS_SEMA_MAX_COUNT);
	rtos_mutex_create(&g_send_data_mutex);

	audio_uart_init_uart();

	memset(g_receive_msg_data, 0, AUDIO_UART_RECEIVE_DATA_BUFLEN);

	msg_attrib_t pattrib = {0};
	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);

#if USING_CMD
	while (g_receive_cnt < g_max_receive_cnt) {
#else
	(void) g_max_receive_cnt;
	while (1) {
#endif
		rtos_sema_take(g_receive_data_sema, RTOS_MAX_TIMEOUT);
		audio_uart_process_receive_data(&pattrib, audio_equalizer);
		g_receive_cnt ++;
	}

	audio_uart_destroy_eq(&audio_equalizer);
	g_receive_cnt = 0;
	serial_free(&g_serial_obj);
	rtos_mutex_delete(g_send_data_mutex);
	rtos_sema_delete(g_send_data_sema);
	rtos_sema_delete(g_receive_data_sema);

	free(g_receive_msg_data);
	rtos_time_delay_ms(1000);
	RTAUDIO_UART_DEBUG_HEAP_END();

	rtos_task_delete(NULL);
}

void audio_uart_task_start(void)
{
	if (rtos_task_create(NULL, ((const char *)"audio_uart_task"), audio_uart_task, NULL, 8192 * 4, 1) != RTK_SUCCESS) {
		EXAMPLE_AUDIO_ERROR("error: rtos_task_create(audio_uart_task) failed");
	}
}

#if USING_CMD
uint32_t audio_uart_cmd(uint16_t argc, unsigned char **argv)
{
	(void) argc;
	(void) argv;
	audio_uart_task_start();
	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE uart_test_cmd_table[] = {
	{
		(const u8 *)"audio_uart",  1, audio_uart_cmd, (const u8 *)"\t audio_uart\n"
	},
};
#else
void app_example(void)
{
	audio_uart_task_start();
}
#endif
