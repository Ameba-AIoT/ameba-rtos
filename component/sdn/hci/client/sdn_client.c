#include <ameba.h>
#include <os_wrapper.h>
#include <sdn_intf.h>
#include <sdn_conf.h>
#include <sdn_user_conf_intf.h>
#include <sdn_user_conf_bt.h>
#if defined(CONFIG_BT_COEXIST)
#include "sdn_coex_intf.h"
#endif
#if defined(CONFIG_WLAN) && CONFIG_WLAN
#include "wifi_api.h"
#include "wifi_intf_drv_to_app_internal.h"
#endif

#define LEN_ALIGN4(size)    (((size) + 3) & (~3))
#define LEN_ALIGN32(size)   (((size) + 31) & (~31))

#ifdef CONFIG_BT_SDN
#if defined(CONFIG_BLE_LL_EXT_ADV_ENABLE) && BT_LL_FEATURE_BT50_LE_EXT_ADV
#define SDN_INTF_MAX_BT_CMD_LEN        255    /* ext adv data */
#define SDN_INTF_MAX_BT_EVT_LEN        257
#else
#define SDN_INTF_MAX_BT_CMD_LEN        42     /* cmd 0x2027 has the max length: 3 + 39 */
#define SDN_INTF_MAX_BT_EVT_LEN        70     /* cmd complete event for 0x1002 has the max length: 2 + 3 + 65 */
#endif
#endif

struct sdn_client_tx {
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	uint8_t *wapn_pool;
	struct list_head wpan_list;
#endif
#ifdef CONFIG_BT_SDN
	uint8_t *bt_pool;
	struct list_head bt_event_list;
#endif
	struct list_head busy_list;
#if defined(CONFIG_BT_COEXIST)
	uint8_t *coex_pool;
	struct list_head coex_free_list;
#endif
	struct sdn_intf_task task;
	void *done_sema;
};

struct sdn_client_rx {
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	uint8_t *wapn_pool;
	struct list_head wpan_list;
#endif
#ifdef CONFIG_BT_SDN
	uint8_t *bt_pool;
	struct list_head bt_cmd_list;
#endif
	struct list_head busy_list;
	struct sdn_intf_task task;
	struct sdn_data_buf ctrl;
	uint8_t ctrl_msg[6];
};

struct sdn_client {
	struct sdn_client_tx tx;
	struct sdn_client_rx rx;
	bool is_mp;
};
static struct sdn_client g_sdn_client_intf = {0};
extern struct sdn_t g_sdn;

#define SDN_CLIENT_RX_TASK_PRI          5
#define SDN_CLIENT_RX_TASK_SIZE         1024

#define SDN_CLIENT_TX_TASK_PRI          4
#define SDN_CLIENT_TX_TASK_SIZE         1024

#define BT_HCI_H4_NONE                                          0x00    /* None of the known packet types */
#define BT_HCI_H4_CMD                                           0x01    /* HCI Command packet */
#define BT_HCI_H4_ACL                                           0x02    /* HCI ACL Data packet */
#define BT_HCI_H4_SCO                                           0x03    /* HCI Synchronous Data packet */
#define BT_HCI_H4_EVT                                           0x04    /* HCI Event packet */
#define BT_HCI_H4_ISO                                           0x05

#if defined(CONFIG_BT_COEXIST)
#define TAG_SDN_COEX    "SDN_COEX"
#endif

extern int rtk_wpan_vhdlc_receive(uint8_t *buf, uint32_t length);
extern void sdn_coex_b2w_scbd_bt_on(uint8_t bt_on, uint8_t direct_send);
extern void sdn_loguart_init(void);
#ifdef CONFIG_SDN_HOST
extern void sdn_host_init(void);
#endif
extern int rtk_ot_start(void);
extern int rtk_ot_loop_exit(void);
extern int rtk_zb_start(void);
extern int rtk_zb_loop_exit(void);
extern bool ble_ll_init(void);
extern void ble_ll_deinit(void);
extern void sdn_pwr_leave_suspend(void);
extern void sdn_sche_init(void);
extern void sdn_sche_deinit(void);
extern void sdn_hal_init(void);
extern void sdn_hal_deinit(void);
extern void sdn_log_init(void);
extern void sdn_log_deinit(void);
extern void sdn_watchdog_init(void);
extern void sdn_watchdog_deinit(void);
#ifdef CONFIG_MP_INCLUDED
bool sdn_uart_is_on(void);
void sdn_uart_tx(struct sdn_data_buf *pdata_buf);
#endif

extern void bt_hci_rx_acl_data(uint8_t *pbuf);
extern void bt_hci_cmd_handler(uint8_t *pbuf);
extern void ble_conn_free_rx(void *rx);

bool _rtk_bt_pre_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	if (!(wifi_is_running(STA_WLAN_INDEX) || wifi_is_running(SOFTAP_WLAN_INDEX))) {
		return false;
	}

	wifi_ps_en_by_bt_state(DISABLE);
#endif

	return true;
}

void _rtk_bt_post_enable(void)
{
#if defined(CONFIG_WLAN) && CONFIG_WLAN
	wifi_ps_en_by_bt_state(ENABLE);
#endif
}

static void _add_tail_lock(struct sdn_data_buf *pdata_buf, struct list_head *head)
{
	rtos_critical_enter(RTOS_CRITICAL_BT);
	list_add_tail(&pdata_buf->list, head);
	rtos_critical_exit(RTOS_CRITICAL_BT);
}

uint32_t sdn_h2c(uint8_t protocol, uint8_t type, void *data, uint16_t len)
{
	struct sdn_data_buf *pdata_buf = NULL;

	// SDN_LOGA("%s protocol %d type %d\r\n", __func__, protocol, type);
	// SDN_DUMPA("", data, len);

#ifdef CONFIG_BT_SDN
	if (protocol == SDN_INTF_BT && type == BT_HCI_H4_ACL) {
		bt_hci_rx_acl_data(data);
		return SDN_INTF_ERR_OK;
	}
#endif

	/* This function is called in interrupt contex in IPC mode, critical is unnecessary. */
#ifdef CONFIG_SDN_HOST
	rtos_critical_enter(RTOS_CRITICAL_BT);
#endif

	switch (protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		if (!list_empty(&g_sdn_client_intf.rx.bt_cmd_list)) {
			pdata_buf = (struct sdn_data_buf *)g_sdn_client_intf.rx.bt_cmd_list.next;
			list_del(&pdata_buf->list);
		}
		break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	case SDN_INTF_154:
		if (!list_empty(&g_sdn_client_intf.rx.wpan_list)) {
			pdata_buf = (struct sdn_data_buf *)g_sdn_client_intf.rx.wpan_list.next;
			list_del(&pdata_buf->list);
		}
		break;
#endif

#ifndef CONFIG_SDN_HOST
	case SDN_INTF_CTRL:
		pdata_buf = &g_sdn_client_intf.rx.ctrl;
		break;
#endif

	default:
		break;
	}

#ifdef CONFIG_SDN_HOST
	rtos_critical_exit(RTOS_CRITICAL_BT);
#endif

	if (!pdata_buf) {
		return SDN_INTF_ERR_TX_DATA_FAIL;
	}

	pdata_buf->protocol = protocol;
	pdata_buf->type = type;
	memcpy(pdata_buf->data, data, len);
	pdata_buf->len = len;
#ifdef CONFIG_SDN_HOST
	_add_tail_lock(pdata_buf, &g_sdn_client_intf.rx.busy_list);
#else
	list_add_tail(&pdata_buf->list, &g_sdn_client_intf.rx.busy_list);
#endif
	rtos_sema_give(g_sdn_client_intf.rx.task.sema);
	return SDN_INTF_ERR_OK;
}

void sdn_client_tx_buf_complete(struct sdn_data_buf *pdata_buf)
{
#ifdef CONFIG_BT_SDN
	if (pdata_buf->protocol == SDN_INTF_BT) {
		if (pdata_buf->type == BT_HCI_H4_ACL) {
			ble_conn_free_rx(pdata_buf);
		} else {
			_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.bt_event_list);
		}
	}
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	if (pdata_buf->protocol == SDN_INTF_154) {
		_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.wpan_list);
	}
#endif
}

#ifndef CONFIG_SDN_HOST
static void sdn_client_intf_close(void);
#endif
static void _tx_task_hdl(void *pcontext)
{
	(void)pcontext;
	struct sdn_data_buf *pdata_buf = NULL;

	g_sdn_client_intf.tx.task.running = 1;

	while (true) {
		rtos_sema_take(g_sdn_client_intf.tx.task.sema, MUTEX_WAIT_TIMEOUT);
		while (true) {
			if (g_sdn_client_intf.tx.task.stop) {
#ifdef CONFIG_BT_COEXIST
				// bt off notify wl
				sdn_coex_b2w_scbd_bt_on(0, 1);
#endif
				goto exit;
			}

			rtos_critical_enter(RTOS_CRITICAL_BT);
			if (list_empty(&g_sdn_client_intf.tx.busy_list)) {
				rtos_critical_exit(RTOS_CRITICAL_BT);
				break;
			}
			pdata_buf = (struct sdn_data_buf *)g_sdn_client_intf.tx.busy_list.next;
			list_del(&pdata_buf->list);
			rtos_critical_exit(RTOS_CRITICAL_BT);

#ifdef CONFIG_BT_COEXIST
			if (pdata_buf->protocol == SDN_INTF_COEX) {
				sdn_coex_msg_parse(pdata_buf);
				_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.coex_free_list);
			} else
#endif
#ifndef CONFIG_SDN_HOST
				if (pdata_buf->protocol == SDN_INTF_CTRL) {
					if (pdata_buf->type == SDN_INTF_CTRL_INTF_CLOSE) { /* pdata_buf may be changed after sdn_c2h(NULL). So pdata_buf is only valid before sdn_c2h(NULL) */
						g_sdn_client_intf.tx.task.stop = 1;
					}
					sdn_c2h(NULL); /* indicate host that msg has been processed. */
				} else
#endif
				{
					// SDN_DUMPA("sdn_tx:\r\n", pdata_buf->data, pdata_buf->len);
#ifdef CONFIG_MP_INCLUDED
					if (sdn_uart_is_on()) {
						sdn_uart_tx(pdata_buf);
					} else
#endif
					{
						sdn_c2h(pdata_buf);
					}
#ifdef CONFIG_SDN_HOST
					sdn_client_tx_buf_complete(pdata_buf); /* return memory directly in singlecore. */
#endif
				}
		}
	}

exit:
	g_sdn_client_intf.tx.task.running = 0;
	rtos_sema_delete(g_sdn_client_intf.tx.task.sema);

#ifndef CONFIG_SDN_HOST
	sdn_client_intf_close();
#endif
	rtos_task_delete(NULL);
}

static void sdn_client_tx_deinit(void)
{
	if (g_sdn_client_intf.tx.task.running) { /* in IPC mode, tx task kills itself. */
		g_sdn_client_intf.tx.task.stop = 1;
		rtos_sema_give(g_sdn_client_intf.tx.task.sema);

		while (g_sdn_client_intf.tx.task.running) {
			rtos_time_delay_ms(1);
		}
	}
#if defined(CONFIG_BT_COEXIST)
	if (g_sdn_client_intf.tx.coex_pool) {
		rtos_mem_free(g_sdn_client_intf.tx.coex_pool);
		g_sdn_client_intf.tx.coex_pool = NULL;
	}
#endif
#ifdef CONFIG_BT_SDN
	if (g_sdn_client_intf.tx.bt_pool) {
		rtos_mem_free(g_sdn_client_intf.tx.bt_pool);
		g_sdn_client_intf.tx.bt_pool = NULL;
	}
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	if (g_sdn_client_intf.tx.wapn_pool) {
		rtos_mem_free(g_sdn_client_intf.tx.wapn_pool);
		g_sdn_client_intf.tx.wapn_pool = NULL;
	}
#endif
}

static void *_alloc_data_list(struct list_head *head, uint16_t size, uint8_t num, uint8_t msg_type, bool align32)
{
	int i;
	uint8_t *pool, *data;
	uint16_t len = LEN_ALIGN4(sizeof(struct sdn_data_buf) + size);
	struct sdn_data_buf *pdata_buf = NULL;

#ifdef CONFIG_SDN_HOST
	(void)align32; /* no IPC, no need alignment */
#else
	if (align32) {
		len = LEN_ALIGN32(len);
	}
#endif

	INIT_LIST_HEAD(head);

	pool = rtos_mem_zmalloc(len * num);
	if (pool == NULL) {
		return NULL;
	}

	data = pool;
	for (i = 0; i < num; i++) {
		pdata_buf = (struct sdn_data_buf *)data;
		pdata_buf->data = data + sizeof(struct sdn_data_buf);
		pdata_buf->msg_type = msg_type;
		rtos_critical_enter(RTOS_CRITICAL_BT);
		list_add_tail(&pdata_buf->list, head);
		rtos_critical_exit(RTOS_CRITICAL_BT);
		data += len;
	}

	return pool;
}

static uint32_t sdn_client_tx_init(void)
{
	g_sdn_client_intf.tx.task.stop = 0;
	INIT_LIST_HEAD(&g_sdn_client_intf.tx.busy_list);

#ifdef CONFIG_BT_SDN
	g_sdn_client_intf.tx.bt_pool = _alloc_data_list(&g_sdn_client_intf.tx.bt_event_list, SDN_INTF_MAX_BT_EVT_LEN,
								   SDN_CONF_CLIENT_BT_EVT_TX_NUM, SDN_MSG(SDN_INTF_BT, BT_HCI_H4_EVT), true);
	if (!g_sdn_client_intf.tx.bt_pool) {
		goto fail;
	}
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	g_sdn_client_intf.tx.wapn_pool = _alloc_data_list(&g_sdn_client_intf.tx.wpan_list, SDN_INTF_MAX_154_LEN,
									 SDN_CONF_CLIENT_154_TX_NUM, SDN_MSG(SDN_INTF_154, 0), true);
	if (!g_sdn_client_intf.tx.wapn_pool) {
		goto fail;
	}
#endif
#if defined(CONFIG_BT_COEXIST)
	g_sdn_client_intf.tx.coex_pool = _alloc_data_list(&g_sdn_client_intf.tx.coex_free_list, COEX_SDN_BUF_SIZE,
									 COEX_SDN_MSG_NUM, SDN_MSG(SDN_INTF_COEX, 0), false);
	if (!g_sdn_client_intf.tx.coex_pool) {
		goto fail;
	}
#endif

	//tx task
	if (RTK_SUCCESS != rtos_sema_create(&g_sdn_client_intf.tx.task.sema, 0, 1)) {
		goto fail;
	}

	if (RTK_SUCCESS != rtos_task_create(&g_sdn_client_intf.tx.task.task, "sdn client tx", _tx_task_hdl,
										NULL, SDN_CLIENT_TX_TASK_SIZE, SDN_CLIENT_TX_TASK_PRI)) {
		goto fail;
	}

	return SDN_INTF_ERR_OK;

fail:
	sdn_client_tx_deinit();
	return SDN_INTF_ERR_OPEN_FAIL;
}

#ifdef CONFIG_MP_INCLUDED
void sdn_set_mp(bool is_mp)
{
	g_sdn_client_intf.is_mp = is_mp;
}
#endif

bool sdn_in_mp(void)
{
	return g_sdn_client_intf.is_mp;
}

void sdn_client_intf_send(void *pdata_buf)
{
	_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.busy_list);
	rtos_sema_give(g_sdn_client_intf.tx.task.sema);
}

#ifndef CONFIG_SDN_HOST
static void _sdn_ctrl_rx(struct sdn_data_buf *pdata_buf)
{
	switch (pdata_buf->type) {
	case SDN_INTF_CTRL_INTF_OPEN:
		sdn_enable();
		break;

	case SDN_INTF_CTRL_INTF_CLOSE:
		sdn_disable();
		break;

	case SDN_INTF_CTRL_PROTO_ADD:
		sdn_add_protocol(pdata_buf->data[0]);
		break;

	case SDN_INTF_CTRL_PROTO_REMOVE:
		sdn_remove_protocol(pdata_buf->data[0]);
		break;

#ifdef CONFIG_MP_INCLUDED
	case SDN_INTF_CTRL_MP:
		sdn_set_mp(pdata_buf->data[0]);
		break;

	case SDN_INTF_CTRL_BRIDGE_OPEN:
		sdn_bridge_open(pdata_buf->data[0]);
		break;

	case SDN_INTF_CTRL_BRIDGE_CLOSE:
		sdn_bridge_close();
		break;
#endif

	case SDN_INTF_CTRL_FIX_ADDR:
		sdn_fix_bt_addr(pdata_buf->data);
		break;

	default:
		return;
		break;
	}

	sdn_client_intf_send(pdata_buf); /* indicate host that msg has been processed. */
}
#endif

static void _rx_task_hdl(void *pcontext)
{
	(void)pcontext;
	struct sdn_data_buf *pdata_buf = NULL;

	g_sdn_client_intf.rx.task.running = 1;

	while (1) {
		rtos_sema_take(g_sdn_client_intf.rx.task.sema, MUTEX_WAIT_TIMEOUT);

		if (g_sdn_client_intf.rx.task.stop) {
			break;
		}

		while (1) {
			rtos_critical_enter(RTOS_CRITICAL_BT);
			if (list_empty(&g_sdn_client_intf.rx.busy_list)) {
				rtos_critical_exit(RTOS_CRITICAL_BT);
				break;
			}

			pdata_buf = (struct sdn_data_buf *)g_sdn_client_intf.rx.busy_list.next;
			list_del(&pdata_buf->list);
			rtos_critical_exit(RTOS_CRITICAL_BT);
			switch (pdata_buf->protocol) {
#ifdef CONFIG_BT_SDN
			case SDN_INTF_BT:
				bt_hci_cmd_handler(pdata_buf->data);
				rtos_critical_enter(RTOS_CRITICAL_BT);
				list_add_tail(&pdata_buf->list, &g_sdn_client_intf.rx.bt_cmd_list);
				rtos_critical_exit(RTOS_CRITICAL_BT);
				break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
			case SDN_INTF_154:
				rtk_wpan_vhdlc_receive(pdata_buf->data, pdata_buf->len);
				rtos_critical_enter(RTOS_CRITICAL_BT);
				list_add_tail(&pdata_buf->list, &g_sdn_client_intf.rx.wpan_list);
				rtos_critical_exit(RTOS_CRITICAL_BT);
				break;
#endif

#ifndef CONFIG_SDN_HOST
			case SDN_INTF_CTRL:
				_sdn_ctrl_rx(pdata_buf);
				continue;
				break;
#endif
			default:
				break;
			}
		}
	}

	g_sdn_client_intf.rx.task.running = 0;
	rtos_sema_delete(g_sdn_client_intf.rx.task.sema);
	rtos_task_delete(NULL);
}

static void sdn_client_rx_deinit(void)
{
#ifdef CONFIG_BT_SDN
	if (g_sdn_client_intf.rx.bt_pool) {
		rtos_mem_free(g_sdn_client_intf.rx.bt_pool);
		g_sdn_client_intf.rx.bt_pool = NULL;
	}
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	if (g_sdn_client_intf.rx.wpan_pool) {
		rtos_mem_free(g_sdn_client_intf.rx.wpan_pool);
		g_sdn_client_intf.rx.wpan_pool = NULL;
	}
#endif

#ifdef CONFIG_SDN_HOST
	if (g_sdn_client_intf.rx.task.running) {
		g_sdn_client_intf.rx.task.stop = 1;
		rtos_sema_give(g_sdn_client_intf.rx.task.sema);
		while (g_sdn_client_intf.rx.task.running) {
			rtos_time_delay_ms(1);
		}
	}
#else
	INIT_LIST_HEAD(&g_sdn_client_intf.rx.busy_list);
#endif
}

static uint32_t sdn_client_rx_task_init(void)
{
	g_sdn_client_intf.rx.task.stop = 0;

	INIT_LIST_HEAD(&g_sdn_client_intf.rx.busy_list);

	g_sdn_client_intf.rx.ctrl.data = g_sdn_client_intf.rx.ctrl_msg;

	//RX task
	if (RTK_SUCCESS != rtos_sema_create(&g_sdn_client_intf.rx.task.sema, 0, 1)) {
		return SDN_INTF_ERR_OPEN_FAIL;
	}

	if (RTK_SUCCESS != rtos_task_create(&g_sdn_client_intf.rx.task.task, "sdn client rx", _rx_task_hdl,
										NULL, SDN_CLIENT_RX_TASK_SIZE, SDN_CLIENT_RX_TASK_PRI)) {
		rtos_sema_delete(g_sdn_client_intf.rx.task.sema);
		return SDN_INTF_ERR_OPEN_FAIL;
	}
	return SDN_INTF_ERR_OK;
}

static uint32_t sdn_client_rx_init(void)
{
	g_sdn_client_intf.rx.task.stop = 0;

#ifdef CONFIG_SDN_HOST
	if (SDN_INTF_ERR_OK != sdn_client_rx_task_init()) {
		goto fail;
	}
#endif

#ifdef CONFIG_BT_SDN
	g_sdn_client_intf.rx.bt_pool = _alloc_data_list(&g_sdn_client_intf.rx.bt_cmd_list, SDN_INTF_MAX_BT_CMD_LEN,
								   SDN_CONF_CLIENT_BT_CMD_RX_NUM, SDN_MSG(SDN_INTF_BT, BT_HCI_H4_CMD), true);
	if (!g_sdn_client_intf.rx.bt_pool) {
		goto fail;
	}
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	g_sdn_client_intf.rx.wpan_pool = _alloc_data_list(&g_sdn_client_intf.rx.wpan_list, SDN_INTF_MAX_154_LEN,
									 SDN_CONF_CLIENT_154_RX_NUM, SDN_MSG(SDN_INTF_154, 0), true);
	if (!g_sdn_client_intf.rx.wpan_pool) {
		goto fail;
	}
#endif

	return SDN_INTF_ERR_OK;

fail:
	sdn_client_rx_deinit();
	return SDN_INTF_ERR_OPEN_FAIL;
}

static uint32_t sdn_client_intf_open(void)
{
	if (SDN_INTF_ERR_OK != sdn_client_tx_init()) {
		return SDN_INTF_ERR_OPEN_FAIL;
	}

	if (SDN_INTF_ERR_OK != sdn_client_rx_init()) {
		sdn_client_tx_deinit();
		return SDN_INTF_ERR_OPEN_FAIL;
	}

	return SDN_INTF_ERR_OK;
}

static void sdn_client_intf_close(void)
{
	sdn_client_tx_deinit();
	sdn_client_rx_deinit();
}

#ifdef CONFIG_BT_SDN
uint8_t *sdn_client_intf_get_bt_buf(uint16_t len, void **pbuf, bool discardable)
{
	struct sdn_data_buf *pdata_buf = NULL;
	struct list_head *pos = NULL;
	uint8_t free_num = 0;

	if (g_sdn_client_intf.tx.task.stop) {
		return NULL;
	}

	if (len > SDN_INTF_MAX_BT_EVT_LEN) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s ERROR %d > max(%d)\r\n", __func__, len, SDN_INTF_MAX_BT_EVT_LEN);
		return NULL;
	}
	rtos_critical_enter(RTOS_CRITICAL_BT);
	list_for_each(pos, &g_sdn_client_intf.tx.bt_event_list) {
		if (!discardable || ++free_num > 1) { /* reserve 1 entries for indiscardable event */
			pdata_buf = list_first_entry(&g_sdn_client_intf.tx.bt_event_list, struct sdn_data_buf, list);
			list_del(&pdata_buf->list);
			break;
		}
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	*pbuf = pdata_buf;
	if (pdata_buf) {
		pdata_buf->len = len;
		return pdata_buf->data;
	}

	if (!discardable) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s, len=%d\r\n", __func__, len);
	}
	return NULL;
}
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
uint8_t *sdn_client_intf_get_154_buf(uint8_t type, uint16_t len, void **pbuf)
{
	struct sdn_data_buf *pdata_buf = NULL;

	if (g_sdn_client_intf.tx.task.stop) {
		return NULL;
	}

	if (len > SDN_INTF_MAX_154_LEN) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "%s ERROR %d > max(%d)\r\n", __func__, len, SDN_INTF_MAX_154_LEN);
		return NULL;
	}
	rtos_critical_enter(RTOS_CRITICAL_BT);
	if (!list_empty(&g_sdn_client_intf.tx.wpan_list)) {
		pdata_buf = list_first_entry(&g_sdn_client_intf.tx.wpan_list, struct sdn_data_buf, list);
		list_del(&pdata_buf->list);
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	*pbuf = pdata_buf;
	if (pdata_buf) {
		pdata_buf->type = type;
		pdata_buf->len = len;
		return pdata_buf->data;
	}

	return NULL;
}
#endif

#if defined(CONFIG_BT_COEXIST)
#define RET_REASON_BUF_SUCC	(0)
#define RET_REASON_BUF_FAIL	(1)
#define RET_REASON_BT_STOP	(2)
#define RET_REASON_LEN_LONG	(3)
#define RET_REASON_NULL_PTR	(4)
uint8_t sdn_client_intf_get_coex_buf(uint8_t type, uint16_t len, void **pbuf, uint8_t **pdata)
{
	struct sdn_data_buf *pdata_buf = NULL;

	if (g_sdn_client_intf.tx.task.stop) {
		return RET_REASON_BT_STOP;
	}

	if (len > COEX_SDN_BUF_SIZE) {
		return RET_REASON_LEN_LONG;
	}

	if (pdata == NULL || pbuf == NULL) {
		return RET_REASON_NULL_PTR;
	}

	rtos_critical_enter(RTOS_CRITICAL_BT);
	if (!list_empty(&g_sdn_client_intf.tx.coex_free_list)) {
		pdata_buf = list_first_entry(&g_sdn_client_intf.tx.coex_free_list, struct sdn_data_buf, list);
		list_del(&pdata_buf->list);
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	*pbuf = pdata_buf;

	if (pdata_buf) {
		pdata_buf->type = type;
		pdata_buf->len = len;

		*pdata = pdata_buf->data;
		return RET_REASON_BUF_SUCC;

	}

	return RET_REASON_BUF_FAIL;
}
#endif
#ifdef CONFIG_BT_SDN
uint8_t sdn_client_intf_get_free_bt_cmd_num(void)
{
	uint8_t num = 0;
	struct list_head *pos = NULL;

	rtos_critical_enter(RTOS_CRITICAL_BT);
	list_for_each(pos, &g_sdn_client_intf.rx.bt_cmd_list) {
		num++;
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	return num;
}
#endif
void sdn_client_init(void)
{
	sdn_loguart_init();
#ifdef CONFIG_SDN_HOST
	sdn_host_init();
#else
	sdn_client_rx_task_init(); /* in IPC case, rx task is always active. */
#endif
}

bool sdn_enable(void)
{
	if (SDN_INTF_ERR_OK != sdn_client_intf_open()) {
		return false;
	}

#if SDN_HAL_SUSPEND_ENABLE
	rtos_critical_enter(RTOS_CRITICAL_BT);
	sdn_pwr_leave_suspend();
	rtos_critical_exit(RTOS_CRITICAL_BT);
#endif

	sdn_log_init();

	sdn_sche_init();

	sdn_hal_init();

	sdn_watchdog_init();

#if defined(CONFIG_BT_COEXIST)
	sdn_coex_b2w_scbd_bt_on(1, 0);
#endif

	return true;
}

void sdn_add_protocol(uint8_t protocol)
{
	switch (protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		ble_ll_init();
		break;
#endif

	case SDN_INTF_154:
#if defined(CONFIG_WPAN_THREAD_EN) && CONFIG_WPAN_THREAD_EN
		rtk_ot_start();
#endif
#if defined(CONFIG_WPAN_ZIGBEE_EN) && CONFIG_WPAN_ZIGBEE_EN
		rtk_zb_start();
#endif
		break;

	default:
		break;
	}
}

void sdn_remove_protocol(uint8_t protocol)
{
	switch (protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		ble_ll_deinit();
		break;
#endif

	case SDN_INTF_154:
#if defined(CONFIG_WPAN_THREAD_EN) && CONFIG_WPAN_THREAD_EN
		rtk_ot_loop_exit();
#endif
#if defined(CONFIG_WPAN_ZIGBEE_EN) && CONFIG_WPAN_ZIGBEE_EN
		rtk_zb_loop_exit();
#endif
		break;

	default:
		break;
	}
}

void sdn_disable(void)
{
	sdn_watchdog_deinit();

	sdn_hal_deinit();

	sdn_sche_deinit();

	sdn_log_deinit();

#ifdef CONFIG_SDN_HOST
	sdn_client_intf_close();
#endif
}