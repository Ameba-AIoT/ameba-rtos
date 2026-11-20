#include "example_wificast_throughput.h"

static const char *TAG = "example_tp";

/* ---------------------------------------------------------------- RX ---- */

struct tp_rx_stat {
	volatile u32 session;
	volatile u32 pkts;      /* distinct packets received in this session */
	volatile u32 bytes;     /* bytes of the distinct packets, tp_hdr included; wraps at 4 GB, the report task accumulates it in 64 bit */
	volatile u32 dups;      /* packets with an already seen sequence number, not counted in pkts/bytes */
	volatile u32 max_seq;   /* highest sequence number seen + 1 */
	volatile u32 tx_total;  /* sender's total packet count, valid when end_rcvd */
	volatile u32 first_ms;  /* time of the first received packet */
	volatile u32 last_ms;   /* time of the last received data packet */
	volatile u8  end_rcvd;
	volatile u8  active;
};

static struct tp_rx_stat g_rx;
static volatile u32 g_rx_done_session;  /* last reported session, ignores its duplicated END packets */

/* RTK_LOGI has no 64-bit format, so the size is printed in KB. */
static void tp_print_rate(const char *what, u64 bytes, u32 ms)
{
	u32 kbps = ms ? (u32)((bytes * 8) / ms) : 0;

	RTK_LOGI(TAG, "%s: %u KB in %u ms, %u.%03u Mbps\n", what, (u32)(bytes >> 10), ms, kbps / 1000, kbps % 1000);
}

/* Runs in the Wi-Fi task: only update counters here. */
static void example_recv_callback(wifi_cast_node_t *pnode, unsigned char *buf, unsigned int len, signed char rssi)
{
	(void)pnode;
	(void)rssi;
	struct tp_hdr *hdr = (struct tp_hdr *)buf;

	if (len < sizeof(struct tp_hdr) || hdr->magic != TP_MAGIC) {
		return;
	}

	if (hdr->session == g_rx_done_session) {
		return;
	}
	u32 now = rtos_time_get_current_system_time_ms();

	if (!g_rx.active || hdr->session != g_rx.session) {
		memset((void *)&g_rx, 0, sizeof(g_rx));
		g_rx.session = hdr->session;
		g_rx.first_ms = g_rx.last_ms = now;
		g_rx.active = 1;
	}

	if (hdr->flags & TP_FLAG_END) {
		g_rx.tx_total = hdr->seq;
		g_rx.end_rcvd = 1;
		return;
	}

	g_rx.last_ms = now;
	/* The sender reuses the sequence number after a failed ack = 1 send whose data may have arrived, count it once. */
	if (hdr->seq < g_rx.max_seq) {
		g_rx.dups++;
		return;
	}

	g_rx.pkts++;
	g_rx.bytes += len;
	if (hdr->seq + 1 > g_rx.max_seq) {
		g_rx.max_seq = hdr->seq + 1;
	}
}

/* Prints the per-second rate while data flows and a summary once the sender stops. */
static void example_rx_report_task(void *param)
{
	(void)param;
	u32 start_ms = 0, prev_ms = 0, prev_bytes = 0, prev_pkts = 0, cur_session = 0;
	u64 total_bytes = 0;
	u8 running = 0;

	while (1) {
		rtos_time_delay_ms(TP_REPORT_MS);

		if (!g_rx.active) {
			running = 0;
			continue;
		}

		u32 now = rtos_time_get_current_system_time_ms();
		u32 session = g_rx.session, bytes = g_rx.bytes, pkts = g_rx.pkts;

		/* A new session restarted the counters, do not mix the deltas of the two tests. */
		if (running && session != cur_session) {
			running = 0;
		}
		if (!running) {
			running = 1;
			cur_session = session;
			start_ms = prev_ms = g_rx.first_ms;
			prev_bytes = prev_pkts = 0;
			total_bytes = 0;
		}

		/* The u32 counter may wrap, the unsigned difference stays correct. */
		u32 delta = bytes - prev_bytes;

		total_bytes += delta;

		if (g_rx.end_rcvd || now - g_rx.last_ms >= TP_REPORT_MS) {
			u32 total = g_rx.end_rcvd ? g_rx.tx_total : g_rx.max_seq;

			tp_print_rate("RX total", total_bytes, g_rx.last_ms - start_ms);
			RTK_LOGI(TAG, "RX packets: %u received, %u expected, %u lost, %u dup%s\n", pkts, total,
					 total > pkts ? total - pkts : 0, g_rx.dups, g_rx.end_rcvd ? "" : " (END packet lost, tail loss not counted)");
			/* Commit the reported session only: if a new test started while printing, g_rx already holds it and marking it done
			 * would drop all its packets. */
			if (g_rx.session == session) {
				g_rx_done_session = session;
				g_rx.active = 0;
			}
			running = 0;
			continue;
		}

		tp_print_rate("RX", delta, now - prev_ms);
		RTK_LOGI(TAG, "RX pkts: %u (+%u)\n", pkts, pkts - prev_pkts);
		prev_ms = now;
		prev_bytes = bytes;
		prev_pkts = pkts;
	}
}

/* ---------------------------------------------------------------- TX ---- */

struct tp_tx_param {
	wifi_cast_addr_t dst;
	u32 len;
	u32 seconds;
	u8  ack;
	u8  tx_rate;
	u8  retransmit;
	u8  retry_limit;  /* TP_RETRY_DEFAULT keeps the driver default */
	u32 wait_ms;      /* total ACK wait budget, spread across (1 + retransmit) attempts */
};

static volatile u8 g_tx_running;
static volatile u8 g_tx_stop;
static struct tp_tx_param g_tx_param;

/* Latency of a successful ack = 1 wifi_cast_send(): from the call until the ACK is received, in us. */
struct tp_lat {
	u32 n;
	u32 min;
	u32 max;
	u64 sum;
	u32 bad;  /* samples dropped because the us clock went backwards */
	u32 hist[TP_LAT_BUCKETS];
};

/* Upper bounds (us, exclusive) of the histogram buckets, the last bucket takes everything above. */
static const u32 tp_lat_bound[TP_LAT_BUCKETS - 1] = {1000, 2000, 5000, 20000, 100000};

static void tp_lat_add(struct tp_lat *l, u64 t0, u64 t1)
{
	if (t1 < t0) {
		l->bad++;
		return;
	}

	u32 us = (u32)(t1 - t0);

	if (l->n == 0 || us < l->min) {
		l->min = us;
	}
	if (us > l->max) {
		l->max = us;
	}
	l->sum += us;
	l->n++;

	int i = 0;

	while (i < TP_LAT_BUCKETS - 1 && us >= tp_lat_bound[i]) {
		i++;
	}
	l->hist[i]++;
}

static void tp_lat_print(const char *what, const struct tp_lat *l)
{
	if (l->n == 0) {
		return;
	}
	RTK_LOGI(TAG, "%s latency: %u samples, min %u us, avg %u us, max %u us%s\n", what, l->n, l->min, (u32)(l->sum / l->n), l->max,
			 l->bad ? ", clock went backwards, some samples dropped" : "");
	RTK_LOGI(TAG, "%s latency histogram: <1ms %u, 1~2ms %u, 2~5ms %u, 5~20ms %u, 20~100ms %u, >=100ms %u\n", what,
			 l->hist[0], l->hist[1], l->hist[2], l->hist[3], l->hist[4], l->hist[5]);
}

static void example_tx_task(void *param)
{
	(void)param;
	struct tp_tx_param *p = &g_tx_param;
	wifi_cast_node_t node = {0};
	wifi_cast_node_t *dst_node;
	wifi_cast_frame_info_t info = WIFI_CAST_FRAME_INFO_DEFAULT();
	struct tp_lat lat = {0};
	u32 ok = 0, fail = 0, seq = 0;
	u8 *buf;

	memcpy(node.mac, p->dst, ETH_ALEN);
	dst_node = wifi_cast_get_node_info(&node);
	if (!dst_node) {
		RTK_LOGE(TAG, "dst "MAC_FMT" is not in node list, run `wificast peer <mac>` first\n", MAC_ARG(p->dst));
		goto exit;
	}

	buf = (u8 *)rtos_mem_zmalloc(p->len);
	if (!buf) {
		RTK_LOGE(TAG, "malloc %u failed\n", p->len);
		goto exit;
	}
	memset(buf + sizeof(struct tp_hdr), 0x5A, p->len - sizeof(struct tp_hdr));

	struct tp_hdr *hdr = (struct tp_hdr *)buf;
	hdr->magic = TP_MAGIC;
	hdr->session = rtos_time_get_current_system_time_ms();

	info.ack = p->ack;
	info.tx_rate = p->tx_rate;
	info.retransmit_count = p->retransmit;
	if (p->retry_limit != TP_RETRY_DEFAULT) {
		info.retry_limit = p->retry_limit;
	}

	info.wait_ms = p->wait_ms;

	RTK_LOGI(TAG, "TX start: dst "MAC_FMT", len %u, %u s, ack %u, rate 0x%x, retransmit %u, retry_limit %u, wait %u ms\n",
			 MAC_ARG(p->dst), p->len, p->seconds, p->ack, p->tx_rate, p->retransmit, info.retry_limit, info.wait_ms);

	u32 start = rtos_time_get_current_system_time_ms();
	u32 prev = start, prev_ok = 0;

	while (!g_tx_stop) {
		u32 now = rtos_time_get_current_system_time_ms();

		if (now - start >= p->seconds * 1000) {
			break;
		}
		if (now - prev >= TP_REPORT_MS) {
			tp_print_rate("TX", (ok - prev_ok) * p->len, now - prev);
			prev = now;
			prev_ok = ok;
		}

		hdr->seq = seq;
		u64 t0 = rtos_time_get_current_system_time_us();

		if (wifi_cast_send(dst_node, buf, p->len, &info) == WIFI_CAST_OK) {
			if (p->ack) {
				tp_lat_add(&lat, t0, rtos_time_get_current_system_time_us());
			}
			ok++;
			seq++;  /* only count sent packets, so the receiver's lost count excludes failed attempts */
		} else {
			fail++;
			rtos_time_delay_ms(2);  /* tx queue is full or ack timeout, let the driver drain */
		}
	}

	u32 elapsed = rtos_time_get_current_system_time_ms() - start;

	/* Tell the receiver the total count so that tail loss is visible. */
	hdr->flags = TP_FLAG_END;
	hdr->seq = seq;
	for (int i = 0; i < 3; i++) {
		wifi_cast_send(dst_node, buf, sizeof(struct tp_hdr), &info);
		rtos_time_delay_ms(10);
	}
	rtos_mem_free(buf);

	tp_print_rate("TX total", (u64)ok * p->len, elapsed);
	RTK_LOGI(TAG, "TX packets: %u ok, %u fail\n", ok, fail);
	tp_lat_print("ACK", &lat);
exit:
	g_tx_running = 0;
	rtos_task_delete(NULL);
}

/* ---------------------------------------------------------- commands ---- */

static int parse_mac(const char *s, u8 mac[ETH_ALEN])
{
	unsigned int v[ETH_ALEN];

	if (sscanf(s, MAC_FMT, &v[0], &v[1], &v[2], &v[3], &v[4], &v[5]) != ETH_ALEN) {
		return -1;
	}
	for (int i = 0; i < ETH_ALEN; i++) {
		mac[i] = (u8)v[i];
	}
	return 0;
}

static int parse_rate(const char *s, u8 *rate)
{
	static const struct {
		const char *name;
		u8 rate;
	} tbl[] = {
		{"6m", RTW_RATE_6M}, {"24m", RTW_RATE_24M}, {"54m", RTW_RATE_54M},
		{"mcs0", RTW_RATE_MCS0}, {"mcs7", RTW_RATE_MCS7},
		{"he0", RTW_RATE_HE1SS_MCS0}, {"he7", RTW_RATE_HE1SS_MCS7}, {"he9", RTW_RATE_HE1SS_MCS9},
	};

	for (u32 i = 0; i < sizeof(tbl) / sizeof(tbl[0]); i++) {
		if (_strcmp(s, tbl[i].name) == 0) {
			*rate = tbl[i].rate;
			return 0;
		}
	}
	return -1;
}

static void example_help(void)
{
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\twificast \n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t peer <mac>   : add the sender's mac on the receiver (unicast only)\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t tx [dst] [len] [sec] [ack] [rate] [retransmit] [retry_limit] [wait_ms]\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t               : start sending; dst is a mac or `bcast` (default bcast),\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t                 len default is the max frame length, sec default %d,\n", TP_DEFAULT_SECONDS);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t                 ack 0/1 (unicast only, default 0),\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t                 rate 6m/24m/54m/mcs0/mcs7/he0/he7/he9 (default 54m),\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t                 retransmit: extra sends per packet (default 0),\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t                 retry_limit: hardware retry times, unicast only (default: driver default),\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t                 wait_ms: total ACK wait budget when ack = 1 (default %d)\n", TP_WAIT_MS_DEFAULT);
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t channel [ch]  : switch to channel <ch>, or show the current channel without argument\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t stop         : stop sending\n");
	RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "\t\t help         : this help\n");
}

static void example_cmd_peer(u8 *argv[], u16 argc)
{
	wifi_cast_node_t node = {0};

	if (argc < 2 || parse_mac((const char *)argv[1], node.mac) < 0) {
		RTK_LOGE(TAG, "usage: wificast peer <xx:xx:xx:xx:xx:xx>\n");
		return;
	}
	WIFI_CAST_ERROR_CHECK(wifi_cast_add_node(&node));
}

static void example_cmd_channel(u8 *argv[], u16 argc)
{
	if (argc < 2) {
		RTK_LOGI(TAG, "current channel %u\n", wifi_cast_get_channel());
		return;
	}

	u32 ch = _strtoul((const char *)argv[1], NULL, 10);

	if (ch == 0 || ch > 255) {
		RTK_LOGE(TAG, "usage: wificast channel [1~255]\n");
		return;
	}
	if (wifi_cast_switch_channel((u8)ch) != WIFI_CAST_OK) {
		RTK_LOGE(TAG, "switch to channel %u failed\n", ch);
		return;
	}
	RTK_LOGI(TAG, "channel switched to %u\n", wifi_cast_get_channel());
}

static void example_cmd_tx(u8 *argv[], u16 argc)
{
	struct tp_tx_param *p = &g_tx_param;
	u32 max_len = wifi_cast_get_max_frame_len();

	if (g_tx_running) {
		RTK_LOGE(TAG, "tx is running, `wificast stop` first\n");
		return;
	}

	memset(p, 0, sizeof(*p));
	memcpy(p->dst, WIFI_CAST_BROADCAST_MAC, ETH_ALEN);
	p->len = max_len;
	p->seconds = TP_DEFAULT_SECONDS;
	p->tx_rate = RTW_RATE_54M;
	p->retry_limit = TP_RETRY_DEFAULT;
	p->wait_ms = TP_WAIT_MS_DEFAULT;

	if (argc > 1 && _strcmp((const char *)argv[1], "bcast") != 0 && parse_mac((const char *)argv[1], p->dst) < 0) {
		RTK_LOGE(TAG, "invalid dst mac\n");
		return;
	}
	if (argc > 2) {
		p->len = _strtoul((const char *)argv[2], NULL, 10);
	}
	if (argc > 3) {
		p->seconds = _strtoul((const char *)argv[3], NULL, 10);
	}
	if (argc > 4) {
		p->ack = _strtoul((const char *)argv[4], NULL, 10) ? 1 : 0;
	}
	if (argc > 5 && parse_rate((const char *)argv[5], &p->tx_rate) < 0) {
		RTK_LOGE(TAG, "invalid rate\n");
		return;
	}
	if (argc > 6) {
		u32 retransmit = _strtoul((const char *)argv[6], NULL, 10);

		if (retransmit > 0xFF) {
			RTK_LOGE(TAG, "retransmit must be 0..255\n");
			return;
		}
		p->retransmit = (u8)retransmit;
	}
	if (argc > 7) {
		u32 retry = _strtoul((const char *)argv[7], NULL, 10);

		if (retry >= TP_RETRY_DEFAULT) {
			RTK_LOGE(TAG, "retry_limit must be 0..%u\n", TP_RETRY_DEFAULT - 1);
			return;
		}
		p->retry_limit = (u8)retry;
	}
	if (argc > 8) {
		p->wait_ms = _strtoul((const char *)argv[8], NULL, 10);
		if (p->wait_ms == 0) {
			RTK_LOGE(TAG, "wait_ms must be > 0\n");
			return;
		}
	}

	if (p->len < sizeof(struct tp_hdr) || p->len > max_len || p->seconds == 0 || p->seconds > TP_MAX_SECONDS) {
		RTK_LOGE(TAG, "len must be %u..%u and sec 1..%u\n", (u32)sizeof(struct tp_hdr), max_len, TP_MAX_SECONDS);
		return;
	}
	if (p->ack && IS_BROADCAST(p->dst)) {
		RTK_LOGE(TAG, "ack needs a unicast dst\n");
		return;
	}

	g_tx_stop = 0;
	g_tx_running = 1;
	if (rtos_task_create(NULL, ((const char *)"example_tp_tx"), example_tx_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_tp_tx\n");
		g_tx_running = 0;
	}
}

static void example_main_task(void *param)
{
	UNUSED(param);
	rtos_time_delay_ms(2000);
	RTK_LOGI(TAG, "------------->start\n");

	wifi_cast_config_t config = WIFI_CAST_INIT_CONFIG_DEFAULT();
	WIFI_CAST_ERROR_CHECK(wifi_cast_init(&config));
	WIFI_CAST_ERROR_CHECK(wifi_cast_register_recv_cb(example_recv_callback));

	if (rtos_task_create(NULL, ((const char *)"example_tp_rx"), example_rx_report_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_tp_rx\n");
	}

	rtos_task_delete(NULL);
}

void app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 512 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void WifiCastTestApp(u16 argc, u8 *argv[])
{
	if (_strcmp((const char *)argv[0], "tx") == 0) {
		example_cmd_tx(argv, argc);
	} else if (_strcmp((const char *)argv[0], "peer") == 0) {
		example_cmd_peer(argv, argc);
	} else if (_strcmp((const char *)argv[0], "channel") == 0) {
		example_cmd_channel(argv, argc);
	} else if (_strcmp((const char *)argv[0], "stop") == 0) {
		g_tx_stop = 1;
	} else {
		example_help();
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
	} else {
		example_help();
	}

	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE   wifi_cast_test_cmd_table[] = {
	{"wificast", CmdWifiCastTest},
};
