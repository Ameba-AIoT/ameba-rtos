#define LOG_TAG "PCRecorder"
#include "log/log.h"

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#include "osal_c/osal_errnos.h"
#include "osal_c/osal_mutex.h"
#include "osal_c/osal_condition.h"
#include "osal_c/osal_thread.h"

#include "serial_api.h"
#include "serial_ex_api.h"
#include "cJSON/cJSON.h"
#include "pc_recorder.h"
#include "pc_recorder_config.h"

#define PR_UART_USE_DMA_TX  1

#define PR_BAUDRATE         1500000
#define PR_MSG_ACK          0x00
#define PR_MSG_ERROR        0x01
#define PR_MSG_CONFIG       0x02
#define PR_MSG_START        0x03
#define PR_MSG_STOP         0x04
#define PR_MSG_DATA         0x05
#define PR_MSG_QUERY        0x06
#define PR_MSG_VOLUME       0x07

#define PR_VERSION          "1.0"
#define RECORD_IDLE         0x0
#define RECORD_BUSY         0x1

#define PC_BUFLEN           1024

extern void sys_jtag_off(void);

typedef struct PCRecorderAdapter {
    int rx_cnt;
    int tx_cnt;
    uint8_t record_stop;
    uint8_t record_status;
    osal_thread_t* pc_rx_task;
    bool pc_rx_task_running;
    int start_cnt;
    int end_cnt;
    osal_mutex_t pr_rx_con_mutex;
    osal_cond_t pr_rx_con;
    rtos_sema_t pr_dma_tx_sema;
    osal_mutex_t pr_tx_mutex;
    serial_t pr_sobj;
    unsigned char pc_buf[PC_BUFLEN];
    int pc_datasize;
} PCRecorderAdapter;

static PCRecorderAdapter g_pr_adapter = {0};

typedef struct PCRecorderMsg {
    int item;
    char *item_str;
} PCRecorderMsg;

const struct PCRecorderMsg msg_type[] = {
    {PR_MSG_ACK,    "ack"},
    {PR_MSG_ERROR,  "error"},
    {PR_MSG_CONFIG, "config"},
    {PR_MSG_START,  "start"},
    {PR_MSG_STOP,   "stop"},
    {PR_MSG_DATA,   "data"},
    {PR_MSG_QUERY,  "query"},
    {PR_MSG_VOLUME, "volume"},
};

void PCRecorder_uartSendString(char *pstr, int len)
{
#if PR_UART_USE_DMA_TX
    int ret = rtos_sema_take(g_pr_adapter.pr_dma_tx_sema, RTOS_MAX_TIMEOUT);
    if (ret) {
        MEDIA_LOGE("take sema error");
    }

    ret = serial_send_stream_dma(&g_pr_adapter.pr_sobj, pstr, len);
    if (ret != 0) {
        MEDIA_LOGE("%s Error(%d)", __FUNCTION__, (int)ret);
    }
#else
    while (len) {
        serial_putc(&g_pr_adapter.pr_sobj, *pstr);
        pstr++;
        len--;
    }
#endif
}

void PCRecorder_uartIrq(uint32_t id, SerialIrq event)
{
    serial_t    *sobj = (serial_t *)id;
    unsigned char rc = 0;

    if (event == RxIrq) {
        rc = serial_getc(sobj);
        if (rc == '{') {
            g_pr_adapter.start_cnt++;
        } else if (rc == '}') {
            g_pr_adapter.end_cnt++;
        }

        g_pr_adapter.pc_buf[g_pr_adapter.pc_datasize++] = rc;

        if ((g_pr_adapter.start_cnt != 0) && g_pr_adapter.start_cnt == g_pr_adapter.end_cnt) {
            g_pr_adapter.start_cnt = 0;
            g_pr_adapter.end_cnt = 0;
            osal_cond_signal(&g_pr_adapter.pr_rx_con);
            return;
        }
    }
}

#if PR_UART_USE_DMA_TX
static void PCRecorder_uartDMATxDone(uint32_t id)
{
    (void) id;
    int res = rtos_sema_give(g_pr_adapter.pr_dma_tx_sema);
    if (res) {
        MEDIA_LOGE("give sema error");
    }
}
#endif

static void PCRecorder_uartInit(void)
{
    g_pr_adapter.pr_sobj.uart_idx = 0;
    serial_init(&g_pr_adapter.pr_sobj, PR_UART_TX, PR_UART_RX);
    serial_baud(&g_pr_adapter.pr_sobj, PR_BAUDRATE);
    serial_format(&g_pr_adapter.pr_sobj, 8, ParityNone, 1);

    serial_rx_fifo_level(&g_pr_adapter.pr_sobj, FifoLvHalf);
    serial_set_flow_control(&g_pr_adapter.pr_sobj, FlowControlNone, 0, 0);
    serial_irq_handler(&g_pr_adapter.pr_sobj, PCRecorder_uartIrq, (uint32_t)&g_pr_adapter.pr_sobj);
    serial_irq_set(&g_pr_adapter.pr_sobj, RxIrq, ENABLE);
#if PR_UART_USE_DMA_TX
    serial_send_comp_handler(&g_pr_adapter.pr_sobj, (void *)PCRecorder_uartDMATxDone, (uint32_t) &g_pr_adapter.pr_sobj);
#endif
}

static int PCRecorder_audioRecordQuery(void)
{
    cJSON *msg_obj;
    char *msg_js = NULL;

    if ((msg_obj = cJSON_CreateObject()) == NULL) {
        return -1;
    }


    cJSON_AddStringToObject(msg_obj, "type", "query");
    cJSON_AddStringToObject(msg_obj, "version", PR_VERSION);

    switch (g_pr_adapter.record_status) {
    case RECORD_IDLE:
        cJSON_AddStringToObject(msg_obj, "status", "idle");
        break;
    case RECORD_BUSY:
        cJSON_AddStringToObject(msg_obj, "status", "busy");
        break;
    }


    msg_js = cJSON_Print(msg_obj);
    cJSON_Delete(msg_obj);

    MEDIA_LOGD("[PC RECORDER INFO] %s, %s", __func__, msg_js);
    osal_mutex_lock(&g_pr_adapter.pr_tx_mutex);
    PCRecorder_uartSendString(msg_js, strlen(msg_js));
    osal_mutex_unlock(&g_pr_adapter.pr_tx_mutex);
    rtos_mem_free(msg_js);

    MEDIA_LOGD("PCRecorder_audioRecordQuery finish");
    return 0;
}

static int PCRecorder_msgAudioDataBinary(int seq, char *data, int data_len)
{
    unsigned char buf[64];
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
    osal_mutex_lock(&g_pr_adapter.pr_tx_mutex);
    PCRecorder_uartSendString((char *)buf, offset);
    PCRecorder_uartSendString(data, data_len);
    osal_mutex_unlock(&g_pr_adapter.pr_tx_mutex);

    return 0;
}

static int PCRecorder_msgResponseAck(int opt)
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

    MEDIA_LOGD("[PC RECORDER INFO] %s, %s", __func__, msg_js);
    osal_mutex_lock(&g_pr_adapter.pr_tx_mutex);
    PCRecorder_uartSendString(msg_js, strlen(msg_js));
    MEDIA_LOGD("[PC RECORDER INFO] %s, send ack done(%d)", __func__, strlen(msg_js));
    osal_mutex_unlock(&g_pr_adapter.pr_tx_mutex);

    rtos_mem_free(msg_js);

    return 0;
}

static void PCRecorder_msgProcess(void)
{
    unsigned char tempbuf[PC_BUFLEN];
    cJSON *root, *typeobj;
    int opt = -1;

    memset(tempbuf, 0x00, PC_BUFLEN);
    memcpy(tempbuf, g_pr_adapter.pc_buf, g_pr_adapter.pc_datasize);
    g_pr_adapter.pc_datasize = 0;

    MEDIA_LOGD("[PC RECORDER INFO] %s, msg: %s", __func__, (char *)tempbuf);
    int type = -1;

    if ((root = cJSON_Parse((char *)tempbuf)) != NULL) {
        if ((typeobj = cJSON_GetObjectItem(root, "type")) != NULL) {
            MEDIA_LOGD("[PC RECORDER INFO] type: %s", (char *)typeobj->valuestring);
            for (unsigned int i = 0; i < sizeof(msg_type) / sizeof(PCRecorderMsg); i++) {
                if (!strcmp(typeobj->valuestring, msg_type[i].item_str)) {
                    type = msg_type[i].item;
                    break;
                }
            }
        }
    }

    switch (type) {
    case PR_MSG_CONFIG:
        PCRecorder_msgResponseAck(0);
        break;
    case PR_MSG_START:
        g_pr_adapter.record_status = RECORD_BUSY;
        PCRecorder_msgResponseAck(0);
        break;
    case PR_MSG_STOP:
        g_pr_adapter.record_status = RECORD_IDLE;
        PCRecorder_msgResponseAck(0);
        break;
    case PR_MSG_QUERY:
        opt = PCRecorder_audioRecordQuery();
        if (opt != 0) {
            PCRecorder_msgResponseAck(opt);
        }
        break;
    case PR_MSG_VOLUME: {
        PCRecorder_msgResponseAck(0);
    }
    break;
    default:
        MEDIA_LOGE("[PC RECORDER INFO] %s, unsupport type =%d", __func__, type);
        PCRecorder_msgResponseAck(opt);
    }

    cJSON_Delete(root);
}

static bool PCRecorder_PCRxTask(void *param)
{
    MEDIA_LOGI("PCRxTask");
    (void)param;

    cJSON_Hooks memoryHook;
    memoryHook.malloc_fn = malloc;
    memoryHook.free_fn = free;
    cJSON_InitHooks(&memoryHook);

    memset(g_pr_adapter.pc_buf, 0, PC_BUFLEN);

    while (g_pr_adapter.pc_rx_task_running) {
        MEDIA_LOGI("wait uart");
        osal_mutex_lock(&g_pr_adapter.pr_rx_con_mutex);
        osal_cond_wait(&g_pr_adapter.pr_rx_con, &g_pr_adapter.pr_rx_con_mutex);
        osal_mutex_unlock(&g_pr_adapter.pr_rx_con_mutex);
        MEDIA_LOGI("wait uart finish");
        PCRecorder_msgProcess();
    }

    return false;
}

void PCRecorder_init()
{
    MEDIA_LOGI("PCRecorder_init");
    sys_jtag_off();
#if PR_UART_USE_DMA_TX
    rtos_sema_create(&g_pr_adapter.pr_dma_tx_sema, 1, RTOS_SEMA_MAX_COUNT);
#endif

    osal_mutex_init(&g_pr_adapter.pr_rx_con_mutex);
    osal_cond_init(&g_pr_adapter.pr_rx_con);

    osal_mutex_init(&g_pr_adapter.pr_tx_mutex);

    PCRecorder_uartInit();

    g_pr_adapter.record_stop = 0;
    g_pr_adapter.record_status = RECORD_IDLE;
    g_pr_adapter.rx_cnt = 0;
    g_pr_adapter.tx_cnt = 0;

    g_pr_adapter.pc_rx_task_running = true;
    osal_thread_param param;
    param.priority = OSAL_THREAD_PRI_NORMAL;
    param.stack_size = 1024 * 8;
    param.joinable = false;
    param.name = (char *)"PCRxTask";
    int res = osal_thread_create(&g_pr_adapter.pc_rx_task, PCRecorder_PCRxTask, NULL, &param);
    if (res) {
        MEDIA_LOGE("create dump task fail");
        return;
    }
}

void PCRecorder_deInit(void)
{
#if PR_UART_USE_DMA_TX
    rtos_sema_delete(g_pr_adapter.pr_dma_tx_sema);
#endif
    osal_mutex_destroy(&g_pr_adapter.pr_rx_con_mutex);
    osal_cond_destroy(&g_pr_adapter.pr_rx_con);
    osal_mutex_destroy(&g_pr_adapter.pr_tx_mutex);

    serial_free(&g_pr_adapter.pr_sobj);

    if (g_pr_adapter.pc_rx_task) {
        g_pr_adapter.pc_rx_task_running = false;
        osal_thread_request_exitAndWait(g_pr_adapter.pc_rx_task);
    }
}

int32_t PCRecorder_dump(void *buffer)
{
    if (g_pr_adapter.record_status == RECORD_BUSY) {
        g_pr_adapter.rx_cnt++;
        PCRecorder_msgAudioDataBinary(g_pr_adapter.rx_cnt, buffer, RECORD_PAGE_SIZE);
    }
    return 0;
}
