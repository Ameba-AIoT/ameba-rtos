#include "example_wificast_tx_sleep.h"

static const char *TAG = "example_main";

static rtos_sema_t g_tx_sema;
static u8 g_tx_buf[SLEEP_DATA_LEN];
/* Broadcast unless a peer is set; the broadcast node is added by wifi_cast_init(). */
static u8 g_peer_mac[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
static volatile u8 g_sleep_run;
static volatile u8 g_prep_alive;
static volatile u8 g_tx_alive;

extern int wifi_set_ips_internal(u8 enable);

#define IPS_REQ_RETRY_US	600
#define IPS_REQ_RETRY_MAX	5000

/* AP and NP stay awake and wifi leaves IPS while the frame is sent. */
static void example_sleep_acquire(void)
{
	pmu_acquire_wakelock(PMU_OS);
	wifi_set_ips_internal(0);
}

static int example_wifi_connected(void)
{
	u8 join_status = RTW_JOINSTATUS_UNKNOWN;

	return wifi_get_join_status(&join_status) == RTK_SUCCESS && join_status == RTW_JOINSTATUS_SUCCESS;
}

/* Allow AP and NP to enter clock-gated sleep. Not connected: request ips first and retry until entered; connected: ips can't be entered,
 * the system suspend takes the wowlan path. Return 0 on success, -1 on ips timeout (wakelock kept). */
static int example_sleep_release(void)
{
	u32 retry = 0;

	if (!example_wifi_connected()) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ips req\n");
		while (wifi_set_ips_internal(1) == -1) {
			if (++retry > IPS_REQ_RETRY_MAX) {
				RTK_LOGS(NOTAG, RTK_LOG_ERROR, "enter ips timeout\n");
				return -1;
			}
			rtos_time_delay_us(IPS_REQ_RETRY_US);
		}
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "ips req ok, retry %u\n", retry);
	}

	pmu_set_sleep_type(SLEEP_CG);
	pmu_release_wakelock(PMU_OS);
	return 0;
}

/* Task 1: kick the tx task every DATA_PERIOD_MS. */
static void example_prep_task(void *param)
{
	UNUSED(param);

	while (g_sleep_run) {
		rtos_sema_give(g_tx_sema);
		/* one long block so idle sees at least configEXPECTED_IDLE_TIME_BEFORE_SLEEP ticks, 1ms polling never reaches tickless */
		rtos_time_delay_ms(DATA_PERIOD_MS);
	}

	/* Wake the wificast task so it can see g_sleep_run is cleared. */
	rtos_sema_give(g_tx_sema);
	g_prep_alive = 0;
	rtos_task_delete(NULL);
}

/* Task 2: wait for the kick, send one frame and wait for the wifi ACK only, then block again. */
static void example_tx_task(void *param)
{
	UNUSED(param);
	wifi_cast_node_t node = {0};
	wifi_cast_frame_info_t info = WIFI_CAST_FRAME_INFO_DEFAULT();
	wcast_err_t ret;
	u32 seq = 0;

	info.ack = 0;
	info.retransmit_count = 0;
	memcpy(node.mac, g_peer_mac, ETH_ALEN);

	while (1) {
		rtos_sema_take(g_tx_sema, RTOS_MAX_DELAY);
		if (!g_sleep_run) {
			break;
		}

		/* Payload: frame head, then a sequence number, then the 0xA5 fill set at init. Written here, so it never changes while a send
		 * is still using the buffer. */
		memcpy(g_tx_buf + sizeof(struct example_frame_head), &seq, sizeof(seq));
		seq++;
		example_sleep_acquire();
		ret = wifi_cast_send(&node, g_tx_buf, SLEEP_DATA_LEN, &info);
		if (ret != WIFI_CAST_OK) {
			RTK_LOGE(TAG, "%s, send failed, ret: %d\n", __func__, ret);
		}
		if (example_sleep_release() != 0) {
			/* Stop the prep task too, otherwise it keeps running and `sleep on` is refused as already on. Release the wakelock
			 * kept by example_sleep_release(), the loop is the only place that acquired it. */
			g_sleep_run = 0;
			pmu_release_wakelock(PMU_OS);
			break;
		}
	}

	/* Leave IPS so the device can receive normally again. */
	wifi_set_ips_internal(0);
	g_tx_alive = 0;
	rtos_task_delete(NULL);
}

static void example_tx_sleep(u8 enable)
{
	if (!enable) {
		g_sleep_run = 0;
		return;
	}
	if (g_tx_sema == NULL || !wifi_cast_get_initialized()) {
		RTK_LOGE(TAG, "wifi cast is not ready, try again later\n");
		return;
	}
	if (SLEEP_DATA_LEN > wifi_cast_get_max_frame_len()) {
		RTK_LOGE(TAG, "SLEEP_DATA_LEN %u is over the max frame length %u\n", (u32)SLEEP_DATA_LEN, wifi_cast_get_max_frame_len());
		return;
	}
	if (g_prep_alive || g_tx_alive) {
		RTK_LOGI(TAG, "sleep already on\n");
		return;
	}

	g_sleep_run = 1;
	g_tx_alive = 1;
	if (rtos_task_create(NULL, "example_tx_task", example_tx_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_tx_task\n");
		g_sleep_run = 0;
		g_tx_alive = 0;
		return;
	}
	g_prep_alive = 1;
	if (rtos_task_create(NULL, "example_prep_task", example_prep_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_prep_task\n");
		/* Let the tx task exit through its normal path. */
		g_sleep_run = 0;
		g_prep_alive = 0;
		rtos_sema_give(g_tx_sema);
	}
}

static void example_control_peer(u8 *argv[], u16 argc)
{
	wifi_cast_node_t node = {0};
	unsigned int v[ETH_ALEN];

	if (argc < 2 || sscanf((const char *)argv[1], MAC_FMT, &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != ETH_ALEN) {
		RTK_LOGE(TAG, "usage: wificast peer <xx:xx:xx:xx:xx:xx>\n");
		return;
	}
	for (int i = 0; i < ETH_ALEN; i++) {
		node.mac[i] = (u8)v[i];
	}
	if (wifi_cast_add_node(&node) == WIFI_CAST_OK) {
		memcpy(g_peer_mac, node.mac, ETH_ALEN);
	} else {
		/* Fails e.g. when wifi cast is not initialized yet, the destination would silently stay at the old peer. */
		RTK_LOGE(TAG, "add peer failed, wifi cast may not be ready, try again later\n");
	}
}

static void example_main_task(void *param)
{
	UNUSED(param);
	rtos_time_delay_ms(1000);
	for (u32 waited = 0; wifi_is_running(STA_WLAN_INDEX) == FALSE; waited++) {
		if (waited >= WIFI_READY_TIMEOUT_MS) {
			RTK_LOGE(TAG, "wifi is not running, give up\n");
			rtos_task_delete(NULL);
			return;
		}
		rtos_time_delay_ms(1);
	}
	RTK_LOGI(TAG, "------------->start\n");

	if (rtos_sema_create(&g_tx_sema, 0, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create tx sema\n\r");
		rtos_task_delete(NULL);
		return;
	}
	memset(g_tx_buf, 0xA5, sizeof(g_tx_buf));
	((struct example_frame_head *)g_tx_buf)->type = 0;
	((struct example_frame_head *)g_tx_buf)->len = SLEEP_DATA_LEN - sizeof(struct example_frame_head);

	wifi_cast_config_t config = WIFI_CAST_INIT_CONFIG_DEFAULT();
	/* Send only, no rx. */
	config.rx_dis = 1;
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(&config));
	if (SLEEP_DATA_LEN > wifi_cast_get_max_frame_len()) {
		RTK_LOGE(TAG, "SLEEP_DATA_LEN %u is over the max frame length %u, sleep on is refused\n", (u32)SLEEP_DATA_LEN, wifi_cast_get_max_frame_len());
	}

	rtos_task_delete(NULL);
}

void app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void WifiCastTestApp(u16 argc, u8  *argv[])
{
	if (_strcmp((const char *)argv[0], "peer") == 0) {
		example_control_peer(argv, argc);
	} else if (_strcmp((const char *)argv[0], "sleep") == 0) {
		if (argc > 1 && _strcmp((const char *)argv[1], "on") == 0) {
			example_tx_sleep(1);
		} else if (argc > 1 && _strcmp((const char *)argv[1], "off") == 0) {
			example_tx_sleep(0);
		}
	} else if (_strcmp((const char *)argv[0], "help") == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\twificast \n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t peer <mac> : set the receiver mac, default broadcast\n");
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t sleep on/off : start/stop periodic %d bytes send with sleep\n", SLEEP_DATA_LEN);
	}
}

u32
CmdWifiCastTest(
	u16 argc,
	u8  *argv[]
)
{
	if (argc > 0) {
		WifiCastTestApp(argc, argv);
	}

	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   wifi_cast_test_cmd_table[] = {
	{"wificast", CmdWifiCastTest},
};

