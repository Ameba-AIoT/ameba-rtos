#include "example_pc_recorder.h"

#ifdef CONFIG_AUDIO_MIXER
#include "audio/audio_service.h"
#endif

#if defined(CONFIG_MEDIA_PLAYER) && CONFIG_MEDIA_PLAYER
#include "media/rtplayer.h"
#endif

#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "rtw_wifi_constants.h"

#include <dlist.h>

extern int wifi_get_join_status(void);

#define PR_UART_TX			PA_25
#define PR_UART_RX			PA_24
#define PR_UART_USE_DMA_TX	1

#define PR_BAUDRATE			1500000
#define PC_BUFLEN			1024
#define RECORD_PAGE_NUM		2
#define RECORD_PAGE_SIZE	2048

#define MAX_URL_LEN			64
#define SERVER_PORT			80
#define LOG_TAG "PlayerTest"

static int start_cnt = 0;
static int end_cnt = 0;

unsigned char pc_buf[PC_BUFLEN];
char record_buf[RECORD_PAGE_NUM * RECORD_PAGE_SIZE]__attribute__((aligned(64)));
char tx_buf[RECORD_PAGE_SIZE];
int pc_datasize = 0;
rtos_sema_t pr_rx_sema;
rtos_sema_t pr_dma_tx_sema;
rtos_mutex_t pr_tx_mutex;
struct RTAudioRecord *audio_record = NULL;
rtos_task_t record_task;
rtos_task_t tx_task;
rtos_task_t playback_task;

volatile bool recorder_is_running = false;
volatile bool player_is_running = false;

struct list_head pr_url_list;

serial_t pr_sobj;
pr_adapter_t pr_adapter = {0};

struct url_item {
	char item_str[MAX_URL_LEN];
	struct list_head node;
};

#if defined(CONFIG_MEDIA_PLAYER) && CONFIG_MEDIA_PLAYER
enum PlayingStatus {
	IDLE,
	PLAYING,
	PAUSED,
	PLAYING_COMPLETED,
	REWIND_COMPLETE,
	STOPPED,
	RESET,
};

int g_pc_playing_status;
struct RTPlayer *g_pc_player;
#else
static unsigned short sine_48000[96] = {
	0X0000, 0X0000, 0X10B5, 0X10B5, 0X2120, 0X2120, 0X30FB, 0X30FB, 0X3FFF, 0X3FFF, 0X4DEB, 0X4DEB,
	0X5A82, 0X5A82, 0X658C, 0X658C, 0X6ED9, 0X6ED9, 0X7641, 0X7641, 0X7BA3, 0X7BA3, 0X7EE7, 0X7EE7,
	0X7FFF, 0X7FFF, 0X7EE7, 0X7EE7, 0X7BA3, 0X7BA3, 0X7641, 0X7641, 0X6ED9, 0X6ED9, 0X658C, 0X658C,
	0X5A82, 0X5A82, 0X4DEB, 0X4DEB, 0X3FFF, 0X3FFF, 0X30FB, 0X30FB, 0X2120, 0X2120, 0X10B5, 0X10B5,
	0X0000, 0X0000, 0XEF4A, 0XEF4A, 0XDEDF, 0XDEDF, 0XCF04, 0XCF04, 0XC000, 0XC000, 0XB214, 0XB214,
	0XA57D, 0XA57D, 0X9A73, 0X9A73, 0X9126, 0X9126, 0X89BE, 0X89BE, 0X845C, 0X845C, 0X8118, 0X8118,
	0X8000, 0X8000, 0X8118, 0X8118, 0X845C, 0X845C, 0X89BE, 0X89BE, 0X9126, 0X9126, 0X9A73, 0X9A73,
	0XA57D, 0XA57D, 0XB214, 0XB214, 0XC000, 0XC000, 0XCF04, 0XCF04, 0XDEDF, 0XDEDF, 0XEF4A, 0XEF4A
};
#endif

const struct pr_msg msg_type[] = {
	{PR_MSG_ACK, 		"ack"		},
	{PR_MSG_ERROR, 	    "error"		},
	{PR_MSG_CONFIG, 	"config"	},
	{PR_MSG_START, 		"start"		},
	{PR_MSG_STOP, 		"stop"		},
	{PR_MSG_DATA,		"data"		},
	{PR_MSG_QUERY,		"query"		},
	{PR_MSG_VOLUME,		"volume"	},
};

void pc_recorder_start(msg_attrib_t *pattrib);
void pc_playback_task(void *param);

#if defined(CONFIG_MEDIA_PLAYER) && CONFIG_MEDIA_PLAYER
void OnStateChangedPC(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int state)
{
	printf("OnStateChanged(%p %p), (%d)\n", listener, player, state);

	switch (state) {
	case RTPLAYER_PREPARED: { //entered for async prepare
		break;
	}

	case RTPLAYER_PLAYBACK_COMPLETE: { //eos received, then stop
		g_pc_playing_status = PLAYING_COMPLETED;
		break;
	}

	case RTPLAYER_STOPPED: { //stop received, then reset
		printf("start reset\n");
		g_pc_playing_status = STOPPED;
		break;
	}

	case RTPLAYER_PAUSED: { //pause received when do pause or start rewinding
		printf("paused\n");
		g_pc_playing_status = PAUSED;
		break;
	}

	case RTPLAYER_REWIND_COMPLETE: { //rewind done received, then start
		printf("rewind complete\n");
		g_pc_playing_status = REWIND_COMPLETE;
		break;
	}
	}
}

void OnInfoPC(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int info, int extra)
{
	(void) listener;
	(void) player;
	(void) extra;
	//printf("OnInfo (%p %p), (%d, %d)\n", listener, player, info, extra);

	switch (info) {
	case RTPLAYER_INFO_BUFFERING_START: {
		printf("RTPLAYER_INFO_BUFFERING_START\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_END: {
		printf("RTPLAYER_INFO_BUFFERING_END\n");
		break;
	}

	case RTPLAYER_INFO_BUFFERING_INFO_UPDATE: {
		//printf("RTPLAYER_INFO_BUFFERING_INFO_UPDATE %d\n", extra);
		break;
	}

	case RTPLAYER_INFO_NOT_REWINDABLE: {
		printf("RTPLAYER_INFO_NOT_REWINDABLE\n");
		break;
	}
	}
}

void OnErrorPC(const struct RTPlayerCallback *listener, const struct RTPlayer *player, int error, int extra)
{
	printf("OnError (%p %p), (%d, %d)\n", player, listener, error, extra);
}

void pc_StartPlay(struct RTPlayer *player, const char *url)
{
	if (player == NULL) {
		printf("start play fail, player is NULL!\n");
		return;
	}

	printf("start to play: %s\n", url);
	int32_t ret = 0;

	g_pc_playing_status = PLAYING;

	printf("SetSource\n");
	ret = RTPlayer_SetSource(player, url);
	if (ret) {
		printf("SetDataSource fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Prepare\n");
	ret = RTPlayer_Prepare(player);
	if (ret) {
		printf("prepare  fail:error=%d\n", (int)ret);
		return ;
	}

	printf("Start\n");
	ret = RTPlayer_Start(player);
	if (ret) {
		printf("start  fail:error=%d\n", (int)ret);
		return ;
	}

	int64_t duration = 0;
	RTPlayer_GetDuration(player, &duration);
	printf("duration is %lldms\n", duration);

	while (g_pc_playing_status == PLAYING) {
		rtos_time_delay_ms(1000);
	}

	if (g_pc_playing_status == PLAYING_COMPLETED) {
		printf("play complete, now stop.\n");
		RTPlayer_Stop(player);
	}

	while (g_pc_playing_status == PLAYING_COMPLETED) {
		rtos_time_delay_ms(1000);
	}

	if (g_pc_playing_status == STOPPED) {
		printf("play stopped, now reset.\n");
		RTPlayer_Reset(player);
	}

	printf("play %s done!!!!\n", url);
}


int pc_player_test(const char *url)
{
	struct RTPlayerCallback *callback = (struct RTPlayerCallback *)rtos_mem_malloc(sizeof(struct RTPlayerCallback));
	if (!callback) {
		printf("Calloc RTPlayerCallback fail.\n");
		return -1;
	}

	callback->OnRTPlayerStateChanged = OnStateChangedPC;
	callback->OnRTPlayerInfo = OnInfoPC;
	callback->OnRTPlayerError = OnErrorPC;

	g_pc_player = RTPlayer_Create();

	RTPlayer_SetCallback(g_pc_player, callback);

	pc_StartPlay(g_pc_player, url);

	rtos_mem_free(callback);
	RTPlayer_Destory(g_pc_player);
	g_pc_player = NULL;

	rtos_time_delay_ms(1000);

	printf("exit\n");
	return 0;
}
#endif

void pr_uart_send_string(char *pstr, int len)
{
#if PR_UART_USE_DMA_TX
	int32_t ret = 0;
	rtos_sema_take(pr_dma_tx_sema, RTOS_MAX_TIMEOUT);

	ret = serial_send_stream_dma(&pr_sobj, pstr, len);
	if (ret != 0) {
		printf("%s Error(%d)\n", __FUNCTION__, (int)ret);
	}
#else
	while (len) {
		serial_putc(&pr_sobj, *pstr);
		pstr++;
		len--;
	}
#endif
}

void pr_uart_irq(uint32_t id, SerialIrq event)
{
	serial_t    *sobj = (serial_t *)id;
	unsigned char rc = 0;

	//UART_TypeDef *UARTx = UART_DEV_TABLE[sobj->uart_idx].UARTx;

	if (event == RxIrq) {
		rc = serial_getc(sobj);
		if (rc == '{') {
			start_cnt++;
		} else if (rc == '}') {
			end_cnt++;
		}

		pc_buf[pc_datasize++] = rc;

		if ((start_cnt != 0) && start_cnt == end_cnt) {
			start_cnt = 0;
			end_cnt = 0;
			pc_buf[pc_datasize++] = '\0';
			rtos_sema_give(pr_rx_sema);
			return;
		}
	}
}

static void uart_dma_tx_done(uint32_t id)
{
	(void) id;
	rtos_sema_give(pr_dma_tx_sema);
	//printf("%s\n", __func__);
}

void pr_uart_init_mbed(void)
{
	pr_sobj.uart_idx = 0;
	serial_init(&pr_sobj, PR_UART_TX, PR_UART_RX);
	serial_baud(&pr_sobj, PR_BAUDRATE);
	serial_format(&pr_sobj, 8, ParityNone, 1);

	serial_rx_fifo_level(&pr_sobj, FifoLvHalf);
	serial_set_flow_control(&pr_sobj, FlowControlNone, 0, 0);
	serial_irq_handler(&pr_sobj, pr_uart_irq, (uint32_t)&pr_sobj);
	serial_irq_set(&pr_sobj, RxIrq, ENABLE);
#if PR_UART_USE_DMA_TX
	serial_send_comp_handler(&pr_sobj, (void *)uart_dma_tx_done, (uint32_t) &pr_sobj);
#endif
}

int pr_audiorecord_query(void)
{
	cJSON *msg_obj;
	char *msg_js = NULL;

	if ((msg_obj = cJSON_CreateObject()) == NULL) {
		return -1;
	}

	cJSON_AddStringToObject(msg_obj, "type", "query");
	cJSON_AddStringToObject(msg_obj, "version", PR_VERSION);

	switch (pr_adapter.record_status) {
	case RECORD_IDLE:
		cJSON_AddStringToObject(msg_obj, "status", "idle");
		break;
	case RECORD_BUSY:
		cJSON_AddStringToObject(msg_obj, "status", "busy");
		break;
	}

	msg_js = cJSON_Print(msg_obj);
	cJSON_Delete(msg_obj);

	printf("[PC RECORDER INFO] %s, %s\n", __func__, msg_js);
	rtos_mutex_take(pr_tx_mutex, MUTEX_WAIT_TIMEOUT);
	pr_uart_send_string(msg_js, strlen(msg_js));
	rtos_mutex_give(pr_tx_mutex);
	rtos_mem_free(msg_js);

	return 0;
}

int pr_audiorecord_config(msg_attrib_t *pattrib)
{
	printf("[PC RECORDER INFO] ================>Free Heap: %d\n", (int)rtos_mem_get_free_heap_size());

	pr_adapter.rx_cnt = 0;
	pr_adapter.tx_cnt = 0;

	audio_record = RTAudioRecord_Create();
	if (!audio_record) {
		printf("[PC RECORDER INFO] record create failed");
		return -1;
	}

	unsigned int device;

	switch (pattrib->device) {
	case 1:
		device = RTDEVICE_IN_MIC;
		break;
	case 2:
		device = RTDEVICE_IN_I2S;
		break;
	}

	RTAudioRecordConfig record_config;
	record_config.sample_rate = pattrib->samplerate;
	record_config.format = pattrib->format;
	record_config.channel_count = pattrib->chnum;
	record_config.device = device;
	record_config.buffer_bytes = 0; //0 means using default period_bytes
	RTAudioRecord_Init(audio_record, &record_config, RTAUDIO_INPUT_FLAG_NONE);
	return 0;
}

int pr_audiorecord_start(msg_attrib_t *pattrib)
{
	if (!audio_record) {
		printf("[PC RECORDER INFO] record start fail\n");
		return -1;
	}
	RTAudioRecord_Start(audio_record);

	for (int i = 0; i < pattrib->adcindex; i++) {
		RTAudioControl_SetChannelMicCategory(i, pattrib->ch_src[i]);
	}

	RTAudioRecord_SetParameters(audio_record, (const char *)pattrib->chmap);
	return 0;
}

int pr_audiorecord_stop(void)
{
	if (pr_adapter.record_stop) {
		printf("[PC RECORDER INFO] audio record already stopped\n");
		return 0;
	}

	pr_adapter.record_stop = 1;
	pr_adapter.record_status = RECORD_IDLE;

	printf("[PC RECORDER INFO] audio record task exit\n");

	RTAudioRecord_Stop(audio_record);
	RTAudioRecord_Destroy(audio_record);

	printf("[PC RECORDER INFO] audio record stopped\n");

#if defined(CONFIG_MEDIA_PLAYER) && CONFIG_MEDIA_PLAYER
	RTPlayer_Stop(g_pc_player);
#endif
	printf("[PC RECORDER INFO] audio player stopped\n");
	return 0;
}

int pc_msg_audio_data_binary(int seq, char *data, int data_len)
{
	unsigned char buf[MAX_URL_LEN];
	unsigned char checksum = 0;
	unsigned int offset = 0;

	for (int i = 0; i < data_len; i++) {
		checksum ^= data[i];
	}

	memcpy(buf, "data", strlen("data"));
	offset += strlen("data");
	*(int *)(buf + offset) = seq;
	offset += 4;
	*(short *)(buf + offset) = data_len;
	offset += 2;
	*(unsigned char *)(buf + offset) = checksum;
	offset += 1;
	rtos_mutex_take(pr_tx_mutex, MUTEX_WAIT_TIMEOUT);
	pr_uart_send_string((char *)buf, offset);
	pr_uart_send_string((char *)data, data_len);
	rtos_mutex_give(pr_tx_mutex);

	return 0;
}

int pc_msg_response_ack(int opt)
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

	printf("[PC RECORDER INFO] %s, %s\n", __func__, msg_js);
	rtos_mutex_take(pr_tx_mutex, MUTEX_WAIT_TIMEOUT);
	pr_uart_send_string(msg_js, strlen(msg_js));
	printf("[PC RECORDER INFO] %s, send ack done(%d)\n", __func__, strlen(msg_js));
	rtos_mutex_give(pr_tx_mutex);

	rtos_mem_free(msg_js);
	return 0;
}

int pc_msg_audio_chmap(msg_attrib_t *pattrib, int ch, int index)
{
	(void) index;
	int amic, dmic, ref;
	//unsigned char tempbuf[64];

	amic = ch & PR_AMIC_NONE;
	dmic = (ch >> 8) & PR_DMIC_NONE;
	ref = ch & PR_MIC_REF;

	if (amic != PR_AMIC_NONE) {
		pattrib->ch_src[pattrib->adcindex] = RTAUDIO_AMIC1 + amic - 1;
		if (ref) {
			pattrib->refch = pattrib->adcindex;
		}
		pattrib->adcindex++;
	} else if (dmic != PR_DMIC_NONE) {
		pattrib->ch_src[pattrib->adcindex] = RTAUDIO_DMIC1 + dmic - 1;
		if (ref) {
			pattrib->refch = pattrib->adcindex;
		}
		pattrib->adcindex++;
	}
	return 0;
}

bool check_cjson(char *inbuf, int len)
{
	char p[PC_BUFLEN], *p1;
	int left = 0, right = 0;

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
		return _TRUE;
	}
	return _FALSE;
}

void pc_msg_process(msg_attrib_t *pattrib)
{
	char tempbuf[PC_BUFLEN] = {0};
	cJSON *root, *typeobj;
	int opt = -1;
	//int offset = 0;
	int datasize = pc_datasize;

	memset(tempbuf, 0x00, PC_BUFLEN);
	memcpy(tempbuf, pc_buf, datasize);
	tempbuf[datasize++] = '\0';
	pc_datasize = 0;

	printf("[PC RECORDER INFO] %s, msg(%d): %s\n", __func__, datasize, tempbuf);

	if (datasize == 0) {
		printf("[PC RECORDER INFO] null data\n");
		return;
	}

	if (check_cjson(tempbuf, datasize) == _FALSE) {
		printf("[PC RECORDER INFO] invaild json\n");
		return;
	}

	if ((root = cJSON_Parse(tempbuf)) != NULL) {
		if ((typeobj = cJSON_GetObjectItem(root, "type")) != NULL) {
			printf("[PC RECORDER INFO] type: %s\n", typeobj->valuestring);
			for (unsigned int i = 0; i < sizeof(msg_type) / sizeof(struct pr_msg); i++) {
				if (!strcmp(typeobj->valuestring, msg_type[i].item_str)) {
					pattrib->type = msg_type[i].item;
					break;
				}
			}
		}
	}

	switch (pattrib->type) {
	case PR_MSG_CONFIG: {
		/* parse config information */
		cJSON *samplerate, *device, *format, *chnum, *mode, *record, *play, *curl;
		cJSON *ch1, *ch2, *ch3, *ch4, *ch5, *ch6, *ch7, *ch8;

		mode = cJSON_GetObjectItem(root, "mode");
		record = cJSON_GetObjectItem(root, "record");

		samplerate = cJSON_GetObjectItem(record, "sample_rate");
		device = cJSON_GetObjectItem(record, "device");
		format = cJSON_GetObjectItem(record, "format");
		chnum = cJSON_GetObjectItem(record, "chl_num");
		ch1 = cJSON_GetObjectItem(record, "chl1");
		ch2 = cJSON_GetObjectItem(record, "chl2");
		ch3 = cJSON_GetObjectItem(record, "chl3");
		ch4 = cJSON_GetObjectItem(record, "chl4");
		ch5 = cJSON_GetObjectItem(record, "chl5");
		ch6 = cJSON_GetObjectItem(record, "chl6");
		ch7 = cJSON_GetObjectItem(record, "chl7");
		ch8 = cJSON_GetObjectItem(record, "chl8");

		if (mode && mode->valueint == 1) {
			play = cJSON_GetObjectItem(root, "play");
			curl = cJSON_GetObjectItem(play, "url");

			pattrib->url = rtos_mem_malloc(strlen(curl->valuestring) + 1);
			memset(pattrib->url, 0x00, strlen(curl->valuestring) + 1);

			strncpy((char *)pattrib->url, curl->valuestring, strlen(curl->valuestring));
			if (player_is_running) {
				rtos_time_delay_ms(200);
				printf("[PC RECORDER INFO] %s, Player is running\n", __func__);
			} else {
				pr_adapter.record_stop = 0; // add for play before start record
				if (rtos_task_create(&playback_task, ((const char *)"playback_task"), pc_playback_task,
									 pattrib, 8192 * 4, 2) != SUCCESS) {
					printf("\n\r%s rtos_task_create(playback_task) failed", __FUNCTION__);
				}
			}
		}

		pattrib->samplerate = samplerate->valueint;
		pattrib->device = device->valueint;
		pattrib->format = format->valueint;
		pattrib->chnum = chnum->valueint;

		pattrib->refch = -1;
		pattrib->adcindex = 0;
		pattrib->offset = 0;
		memset(pattrib->chmap, 0x00, 256);

		pc_msg_audio_chmap(pattrib, ch1->valueint, 1);
		pc_msg_audio_chmap(pattrib, ch2->valueint, 2);
		pc_msg_audio_chmap(pattrib, ch3->valueint, 3);
		pc_msg_audio_chmap(pattrib, ch4->valueint, 4);
		pc_msg_audio_chmap(pattrib, ch5->valueint, 5);
		pc_msg_audio_chmap(pattrib, ch6->valueint, 6);
		pc_msg_audio_chmap(pattrib, ch7->valueint, 7);
		pc_msg_audio_chmap(pattrib, ch8->valueint, 8);

		memset(tempbuf, 0x00, PC_BUFLEN);
		if (pattrib->refch != -1) {
			sprintf(tempbuf, "ref_channel=%d;cap_mode=no_afe_all_data", pattrib->refch);
		} else {
			sprintf(tempbuf, "cap_mode=no_afe_all_data");
		}
		memcpy(pattrib->chmap + pattrib->offset, tempbuf, strlen(tempbuf));

		printf("[PC RECORDER INFO] samplerate: %d\n", pattrib->samplerate);
		printf("[PC RECORDER INFO] device: %d\n", pattrib->device);
		printf("[PC RECORDER INFO] format: %d\n", pattrib->format);
		printf("[PC RECORDER INFO] chnum: %d\n", pattrib->chnum);
		printf("[PC RECORDER INFO] refch: %d\n", pattrib->refch);
		printf("[PC RECORDER INFO] parameters, %s\n", pattrib->chmap);

		opt = pr_audiorecord_config(pattrib);
	}
	pc_msg_response_ack(opt);
	break;
	case PR_MSG_START:
		opt = pr_audiorecord_start(pattrib);
		pc_msg_response_ack(opt);
		pc_recorder_start(pattrib);
		break;
	case PR_MSG_STOP:
		opt = pr_audiorecord_stop();
		pc_msg_response_ack(opt);
		break;
	case PR_MSG_QUERY:
		opt = pr_audiorecord_query();
		if (opt != 0) {
			pc_msg_response_ack(opt);
		}
		break;
	case PR_MSG_VOLUME: {
		cJSON *volume;
		float vol = 0.0f;

		volume = cJSON_GetObjectItem(root, "value");
		vol = (float)(volume->valueint) / 100.0f;

		printf("[PC RECORDER INFO] volume: %f\n", vol);

#if defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
		RTAudioControl_SetHardwareVolume(vol, vol);
#endif
		pc_msg_response_ack(0);
	}
	break;
	default:
		printf("[PC RECORDER INFO] %s, unsupport type\n", __func__);
		pc_msg_response_ack(opt);
	}

	cJSON_Delete(root);
}

void pc_rx_task(void *param)
{
	(void)param;

	msg_attrib_t pattrib = {0};
	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);

	memset(pc_buf, 0, PC_BUFLEN);

	while (1) {
		rtos_sema_take(pr_rx_sema, RTOS_MAX_TIMEOUT);
		pc_msg_process(&pattrib);
	}

	rtos_task_delete(NULL);
}

void pc_tx_task(void *param)
{
	(void)param;
	int index = 0;

	while (1) {

		if (pr_adapter.tx_cnt < pr_adapter.rx_cnt) {
			index = pr_adapter.tx_cnt % RECORD_PAGE_NUM;
			memcpy(tx_buf, record_buf + index * RECORD_PAGE_SIZE, RECORD_PAGE_SIZE);
			pr_adapter.tx_cnt++;
			pc_msg_audio_data_binary(pr_adapter.tx_cnt, tx_buf, RECORD_PAGE_SIZE);
		}
		if (pr_adapter.record_stop) {
			break;
		}

		rtos_time_delay_ms(2);
	}
	printf("[PC RECORDER INFO] %s, exit\n", __func__);
	rtos_task_delete(tx_task);
}

static void pc_recorder_task(void *param)
{
	(void)param;
	int index = 0;
	recorder_is_running = true;
	while (1) {
		index = pr_adapter.rx_cnt % RECORD_PAGE_NUM;
		RTAudioRecord_Read(audio_record, record_buf + index * RECORD_PAGE_SIZE, RECORD_PAGE_SIZE, true);
		pr_adapter.rx_cnt++;
#if PR_UART_USE_DMA_TX
		pc_msg_audio_data_binary(pr_adapter.rx_cnt, record_buf + index * RECORD_PAGE_SIZE, RECORD_PAGE_SIZE);
#endif
		if (pr_adapter.record_stop) {
			break;
		}
	}
	printf("[PC RECORDER INFO] %s, exit\n", __func__);
	recorder_is_running = false;
	rtos_task_delete(record_task);
}

int pc_parse_url(char *url, char *host, char *resource, char *format)
{
	if (url) {
		char *http = NULL, *pos = NULL;

		http = strstr(url, "http://");
		if (http) { // remove http
			url += strlen("http://");
		}

		pos = strstr(url, "/");
		if (pos) {
			memcpy(host, url, (pos - url));
			url = pos;
		}
		printf("[PC RECORDER INFO] server: %s\n\r", host);

		pos = strstr(url, "/");
		if (pos) {
			resource[0] = '/';
			memcpy(resource + 1, pos + 1, strlen(pos + 1));
		}
		printf("[PC RECORDER INFO] resource: %s\n\r", resource);

		pos = strstr(resource, ".");
		if (pos) {
			memcpy(format, pos + 1, strlen(pos + 1));
		}
		printf("[PC RECORDER INFO] format: %s\n\r", format);
		return 0;
	}
	return -1;
}

void pc_playback_task(void *param)
{
	player_is_running = true;
#if defined(CONFIG_MEDIA_PLAYER) && CONFIG_MEDIA_PLAYER
	msg_attrib_t *pattrib = (msg_attrib_t *)param;
	int server_fd = -1;
	struct sockaddr_in server_addr;
	struct hostent *server_host;
	char host[64];
	char resource[128];
	char format[16];

	memset(host, 0, 64);
	memset(resource, 0, 128);
	memset(format, 0, 16);

	pc_parse_url((char *)pattrib->url, host, resource, format);

	if (strcmp(format, "m3u") == 0) {

		// if url is m3u format, should get the playlist first
		server_fd = socket(AF_INET, SOCK_STREAM, 0);

		printf("[PC RECORDER INFO] create socket: %d\n", server_fd);

		int recv_timeout_ms = 5000;
		setsockopt(server_fd, SOL_SOCKET, SO_RCVTIMEO, &recv_timeout_ms, sizeof(recv_timeout_ms));

		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(80);

		// Support SERVER_HOST in IP or domain name
		server_host = gethostbyname(host);
		if (server_host != NULL) {
			memcpy((void *) &server_addr.sin_addr, (void *) server_host->h_addr, 4);
		} else {
			printf("ERROR: server host\n");
			goto exit;
		}
reconnect:
		if (connect(server_fd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == 0) {
			unsigned char buf[PC_BUFLEN + 1];
			int pos = 0, read_size = 0, content_len = 0, header_removed = 0;
			int header_len = 0;
			int r_content_len = 0;
			//char *delimit = "http://";

			//char purl[MAX_URL_LEN] = {0};
			char *http0 = NULL, *http1 = NULL;
			char *p = NULL;

			printf("[PC RECORDER INFO] connect success\n");

			sprintf((char *)buf, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", resource, host);
			write(server_fd, (char const *)buf, strlen((char const *)buf));

			while ((read_size = read(server_fd, buf + pos, PC_BUFLEN - pos)) > 0) {

				if (header_removed == 0) {
					char *header = NULL;

					pos += read_size;
					buf[pos] = 0;
					header = strstr((char const *)buf, "\r\n\r\n");

					if (header) {
						char *body, *content_len_pos;

						body = header + strlen("\r\n\r\n");
						*(body - 2) = 0;
						header_removed = 1;
						printf("\nHTTP Header: %s\n", buf);

						// Remove header size to get first read size of data from body head
						read_size = pos - ((unsigned char *) body - buf);
						header_len = pos - read_size;
						pos = 0;

						content_len_pos = strstr((char const *)buf, "Content-Length: ");
						if (content_len_pos) {
							content_len_pos += strlen("Content-Length: ");
							*(char *)(strstr(content_len_pos, "\r\n")) = 0;
							content_len = atoi(content_len_pos);
						}
					} else {
						if (pos >= PC_BUFLEN) {
							printf("ERROR: HTTP header\n");
							goto exit;
						}

						continue;
					}

					memset(buf, 0, header_len);
					memcpy(buf, buf + header_len, read_size);
					memset(buf + read_size, 0, header_len);
					r_content_len += read_size;
				} else {
					r_content_len += read_size;
				}

				/* get playlist here */

				p = (char *)buf;

				http0 = strstr(p, "http://");
				pos = 0;
				while (http0) {
					p = http0;

					p += strlen("http://");//remove http

					http1 = strstr(p, "http://");
					if (http1) {
						/* get http url between 2 http:// */
						struct url_item *item = rtos_mem_malloc(sizeof(struct url_item));
						memset(item->item_str, 0, MAX_URL_LEN);
						memcpy(item->item_str, "http://", strlen("http://"));
						memcpy(item->item_str + strlen("http://"), p, http1 - http0 - strlen("http://") - 2); // remove \r\n
						list_add_tail(&item->node, &pr_url_list);
						printf("== %s\n", item->item_str);
						p += (http1 - http0 - strlen("http://"));
					} else {
						if (r_content_len == content_len) {
							struct url_item *item = rtos_mem_malloc(sizeof(struct url_item));
							memset(item->item_str, 0, MAX_URL_LEN);
							memcpy(item->item_str, http0, strlen(http0) - 2); // remove \r\n
							list_add_tail(&item->node, &pr_url_list);
							printf("## %s\n", item->item_str);
						} else {
							memset(buf, 0, PC_BUFLEN);
							memcpy(buf, http0, strlen(http0));
							pos = strlen(http0);
						}
					}

					http0 = strstr(p, "http://");
				}

				if (r_content_len == content_len) {
					break;
				}

			}

		} else {
			printf("[PC RECORDER INFO] connect failed\n");
			if (pr_adapter.record_stop) {
				goto exit;
			}
			rtos_time_delay_ms(2000);
			goto reconnect;
		}

		printf("close server fd\n");
		close(server_fd);

		char *purl = NULL;
		struct url_item *item = list_first_entry(&pr_url_list, struct url_item, node);
		purl = item->item_str;
		list_del(&item->node);

		while (purl) {
			printf("%s, len: %d\n", purl, strlen(purl));

			pc_player_test(purl);
			list_add_tail(&item->node, &pr_url_list);

			if (pr_adapter.record_stop) {
				rtos_mem_free(item);
				break;
			}

			item = list_first_entry(&pr_url_list, struct url_item, node);
			purl = item->item_str;
			list_del(&item->node);
		}

		struct url_item *req, *next;
		list_for_each_entry_safe(req, next, &pr_url_list, node, struct url_item) {
			list_del(&req->node);
			rtos_mem_free(req);
		}
	} else {
replay:
		pc_player_test((const char *)pattrib->url);

		if (!pr_adapter.record_stop) {
			goto replay;
		}
	}

exit:
	rtos_mem_free(pattrib->url);

#else
	(void)param;

	struct RTAudioTrack *audio_track;
	int track_buf_size = 4096;
	unsigned int channels = 2;
	unsigned int rate = 48000;
	printf("[PC RECORDER INFO] play sample channels:%d, rate:%d\n", channels, rate);

	audio_track = RTAudioTrack_Create();
	if (!audio_track) {
		printf("[PC RECORDER INFO] new RTAudioTrack failed\n");
		return;
	}
	uint32_t format = RTAUDIO_FORMAT_PCM_16_BIT;
	track_buf_size = RTAudioTrack_GetMinBufferBytes(audio_track, RTAUDIO_CATEGORY_MEDIA, rate, format, channels) * 4;
	RTAudioTrackConfig	track_config;
	track_config.category_type = RTAUDIO_CATEGORY_MEDIA;
	track_config.sample_rate = rate;
	track_config.format = format;
	track_config.channel_count = channels;
	track_config.buffer_bytes = track_buf_size;
	RTAudioTrack_Init(audio_track, &track_config, RTAUDIO_OUTPUT_FLAG_NONE);

	RTAudioTrack_Start(audio_track);

	ssize_t size = track_buf_size / 4;

	printf("[PC RECORDER INFO] audio track get size = %d\n", size);

	if (rate == 48000) {
		size = 96 * 2;
		while (1) {
			RTAudioTrack_Write(audio_track, (unsigned char *)sine_48000, size, true);
			if (pr_adapter.record_stop) {
				break;
			}
		}
	}

	RTAudioTrack_Stop(audio_track);
	RTAudioTrack_Destroy(audio_track);

#endif
	printf("[PC RECORDER INFO] %s, exit\n", __func__);
	player_is_running = false;
	rtos_task_delete(playback_task);
}


void pc_recorder_start(msg_attrib_t *pattrib)
{
	(void) pattrib;
	if (recorder_is_running) {
		rtos_time_delay_ms(200);
		printf("[PC RECORDER INFO] %s, Recorder is running\n", __func__);
		return;
	}
	pr_adapter.record_stop = 0;
	pr_adapter.record_status = RECORD_BUSY;


	if (rtos_task_create(&record_task, ((const char *)"pc_recorder_task"), pc_recorder_task,
						 NULL, 1024 * 4, 9) != SUCCESS) {
		printf("\n\r%s rtos_task_create(pc_recorder_task) failed", __FUNCTION__);
	}
#if PR_UART_USE_DMA_TX == 0
	if (rtos_task_create(&tx_task, ((const char *)"pc_tx_task"), pc_tx_task,
						 NULL, 1024 * 4, 3) != SUCCESS) {
		printf("\n\r%s rtos_task_create(pc_tx_task) failed", __FUNCTION__);
	}
#endif
}

void pc_recorder_main(void *param)
{
	(void)param;

	rtos_time_delay_ms(1000);

	while (!((wifi_get_join_status() == RTW_JOINSTATUS_SUCCESS))) {
		printf("Please connect to WIFI\n");
		rtos_time_delay_ms(1000);
	}

	printf("[PC RECORDER INFO] %s, start\n", __func__);

#if PR_UART_USE_DMA_TX
	rtos_sema_create(&pr_dma_tx_sema, 1, RTOS_SEMA_MAX_COUNT);
#endif
	rtos_sema_create(&pr_rx_sema, 0, RTOS_SEMA_MAX_COUNT);
	//rtw_init_queue(&url_list);
	INIT_LIST_HEAD(&pr_url_list);
	rtos_mutex_create(&pr_tx_mutex);

	pr_uart_init_mbed();

	pr_adapter.record_stop = 0;
	pr_adapter.record_status = RECORD_IDLE;
	pr_adapter.rx_cnt = 0;
	pr_adapter.rx_cnt = 0;

	memset(record_buf, 0x00, RECORD_PAGE_NUM * RECORD_PAGE_SIZE);

#if defined(CONFIG_AUDIO_MIXER) && CONFIG_AUDIO_MIXER
	RTAudioService_Init();
	RTAudioControl_SetAmplifierEnPin(PA_12);
	RTAudioControl_SetHardwareVolume(0.6, 0.6);
#endif
	if (rtos_task_create(NULL, (char const *)"pc_rx_task", pc_rx_task, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r[%s] Create pc_rx_task failed", __FUNCTION__);
	}

	rtos_task_delete(NULL);
}

void example_pc_recorder(void)
{
	if (rtos_task_create(NULL, (char const *)"pr_main", pc_recorder_main, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r[%s] Create pr_main failed", __FUNCTION__);
	}
}
