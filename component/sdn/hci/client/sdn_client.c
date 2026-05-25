#include <sdn.h>
#include <sdn_intf.h>
#include <sdn_watchdog.h>
#include <sdn_user_conf_intf.h>
#if defined(CONFIG_BT_COEXIST)
#include "sdn_coex_intf.h"
#endif

extern void bt_hci_client_recv_data(uint8_t type, uint8_t *pdata);
extern int rtk_wpan_vhdlc_receive(uint8_t *buf, uint32_t length);

struct sdn_client_ipc_tx {
	struct list_head free_list;
	struct list_head busy_list;
	uint8_t *msg_pool;
#if defined(CONFIG_BT_COEXIST)
	uint8_t *coex_pool;
	struct list_head coex_free_list;
#endif
	struct sdn_intf_task task;
	void *done_sema;
};

struct sdn_client_ipc_rx {
	struct list_head free_list;
	struct list_head busy_list;
	uint8_t *msg_pool;
	struct sdn_intf_task task;
	struct sdn_data_buf ctrl;
	uint8_t ctrl_msg[4];
#ifdef CONFIG_BT_SDN
	uint8_t num_bt_hci_cmd;
	uint8_t num_bt_acl_data;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	uint8_t num_154;
#endif
};

struct sdn_client_ipc {
	struct sdn_client_ipc_tx tx;
	struct sdn_client_ipc_rx rx;
	bool is_mp;
};
static struct sdn_client_ipc g_sdn_client_intf = {0};
extern struct sdn_t g_sdn;

#define SDN_CLIENT_RX_TASK_PRI          5
#define SDN_CLIENT_RX_TASK_SIZE         768

#define SDN_CLIENT_TX_TASK_PRI          4
#define SDN_CLIENT_TX_TASK_SIZE         768

#define BT_HCI_H4_NONE                                          0x00    /* None of the known packet types */
#define BT_HCI_H4_CMD                                           0x01    /* HCI Command packet */
#define BT_HCI_H4_ACL                                           0x02    /* HCI ACL Data packet */
#define BT_HCI_H4_SCO                                           0x03    /* HCI Synchronous Data packet */
#define BT_HCI_H4_EVT                                           0x04    /* HCI Event packet */
#define BT_HCI_H4_ISO                                           0x05


#if defined(CONFIG_MP_INCLUDED)
bool sdn_uart_is_on(void);
void sdn_uart_tx(struct sdn_data_buf *pdata_buf);
#endif

static void _add_tail_lock(struct sdn_data_buf *pdata_buf, struct list_head *head)
{
	rtos_critical_enter(RTOS_CRITICAL_BT);
	list_add_tail(&pdata_buf->list, head);
	rtos_critical_exit(RTOS_CRITICAL_BT);
}

uint32_t sdn_h2c(uint8_t protocol, uint8_t type, void *data, uint16_t len)
{
	struct sdn_data_buf *pdata_buf = NULL;
	bool balloc_avaliable = false;

	// SDN_LOGA("%s type %d\r\n", __func__, type);
	// SDN_DUMPA("", data, len);

	rtos_critical_enter(RTOS_CRITICAL_BT);

	switch (protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		if (type == BT_HCI_H4_CMD) {
			if (g_sdn_client_intf.rx.num_bt_hci_cmd) {
				g_sdn_client_intf.rx.num_bt_hci_cmd--;
				balloc_avaliable = true;
			}
		} else {
			if (g_sdn_client_intf.rx.num_bt_acl_data) {
				g_sdn_client_intf.rx.num_bt_acl_data--;
				balloc_avaliable = true;
			}
		}
		break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	case SDN_INTF_154:
		if (g_sdn_client_intf.rx.num_154) {
			g_sdn_client_intf.rx.num_154--;
			balloc_avaliable = true;
		}
		break;
#endif

	case SDN_INTF_CTRL:
		pdata_buf = &g_sdn_client_intf.rx.ctrl;
		break;

	default:
		break;
	}

	if (balloc_avaliable) {
		if (!list_empty(&g_sdn_client_intf.rx.free_list)) {
			pdata_buf = (struct sdn_data_buf *)g_sdn_client_intf.rx.free_list.next;
			list_del_init(&pdata_buf->list);
		}
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	if (!pdata_buf) {
		return SDN_INTF_ERR_TX_DATA_FAIL;
	}

	pdata_buf->pmsg->protocol = protocol;
	pdata_buf->pmsg->type = type;
	memcpy(pdata_buf->pmsg->data, data, len);
	pdata_buf->len = len + sizeof(struct sdn_intf_data_msg);
	_add_tail_lock(pdata_buf, &g_sdn_client_intf.rx.busy_list);
	rtos_sema_give(g_sdn_client_intf.rx.task.sema);
	return SDN_INTF_ERR_OK;
}

static void _tx_task_hdl(void *pcontext)
{
	(void)pcontext;
	struct sdn_data_buf *pdata_buf = NULL;

	g_sdn_client_intf.tx.task.running = 1;

	while (true) {
		rtos_sema_take(g_sdn_client_intf.tx.task.sema, MUTEX_WAIT_TIMEOUT);
		if (g_sdn_client_intf.tx.task.stop) {
#if defined(CONFIG_BT_COEXIST)
			// bt off notify wl
			sdn_coex_b2w_scbd_bt_on(0, 1);
#endif
			break;
		}

		while (true) {
			pdata_buf = NULL;
			rtos_critical_enter(RTOS_CRITICAL_BT);
			if (!list_empty(&g_sdn_client_intf.tx.busy_list)) {
				pdata_buf = (struct sdn_data_buf *)g_sdn_client_intf.tx.busy_list.next;
				list_del_init(&pdata_buf->list);
			}
			rtos_critical_exit(RTOS_CRITICAL_BT);

			if (!pdata_buf) {
				break;
			}
#if defined(CONFIG_BT_COEXIST)
			if (pdata_buf->pmsg->protocol == SDN_INTF_COEX) {
				sdn_coex_msg_parse(pdata_buf->pmsg);
				_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.coex_free_list);
			} else
#endif
			{
				// SDN_DUMPA("sdn_tx:\r\n", pdata_buf->pmsg, pdata_buf->len);
#if defined(CONFIG_MP_INCLUDED)
				if (sdn_uart_is_on()) {
					sdn_uart_tx(pdata_buf);
				} else
#endif
				{
					sdn_c2h(pdata_buf);
				}
				_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.free_list);
			}
		}

	}

	g_sdn_client_intf.tx.task.running = 0;
	rtos_sema_delete(g_sdn_client_intf.tx.task.sema);
	rtos_task_delete(NULL);
}

static void sdn_client_tx_deinit(void)
{
	//delete task
	if (g_sdn_client_intf.tx.task.running) {
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
	if (g_sdn_client_intf.tx.msg_pool) {
		rtos_mem_free(g_sdn_client_intf.tx.msg_pool);
		g_sdn_client_intf.tx.msg_pool = NULL;
	}
}

static uint32_t sdn_client_tx_init(void)
{
	uint8_t tx_num = SDN_CONF_CLIENT_TX_NUM;
	uint8_t i = 0;
	struct sdn_data_buf *pdata_buf = NULL;
	uint8_t *pool, *data;
	uint16_t size = sizeof(struct sdn_intf_data_msg) + SDN_INTF_MAX_DATA_LEN;

#if !defined(CONFIG_SDN_HOST) || !defined(CONFIG_SDN_DEV)
	size = (size + 31) & (~31); /* align with 32 bytes for IPC send */
#endif

	g_sdn_client_intf.tx.task.stop = 0;
	//msg resource
	INIT_LIST_HEAD(&g_sdn_client_intf.tx.busy_list);
	INIT_LIST_HEAD(&g_sdn_client_intf.tx.free_list);

	pool = rtos_mem_malloc(tx_num * (sizeof(struct sdn_data_buf) + size));
	if (pool == NULL) {
		goto fail;
	}

	data = pool + tx_num * size;
	for (i = 0; i < tx_num; i++) {
		pdata_buf = (struct sdn_data_buf *)(data + i * sizeof(struct sdn_data_buf));
		pdata_buf->pmsg = (struct sdn_intf_data_msg *)(pool + i * size);
		rtos_critical_enter(RTOS_CRITICAL_BT);
		list_add_tail(&pdata_buf->list, &g_sdn_client_intf.tx.free_list);
		rtos_critical_exit(RTOS_CRITICAL_BT);
	}
	g_sdn_client_intf.tx.msg_pool = pool;

#if defined(CONFIG_BT_COEXIST)
	INIT_LIST_HEAD(&g_sdn_client_intf.tx.coex_free_list);
	if (g_sdn_client_intf.tx.coex_pool == NULL) {
		g_sdn_client_intf.tx.coex_pool = rtos_mem_malloc(COEX_SDN_MSG_NUM * (sizeof(struct sdn_data_buf) + COEX_SDN_BUF_SIZE));
		if (g_sdn_client_intf.tx.coex_pool == NULL) {
			goto fail;
		}
	}
	for (i = 0; i < COEX_SDN_MSG_NUM; i++) {
		pdata_buf = (struct sdn_data_buf *)(g_sdn_client_intf.tx.coex_pool + i * sizeof(struct sdn_data_buf));
		pdata_buf->pmsg = (struct sdn_intf_data_msg *)(g_sdn_client_intf.tx.coex_pool + COEX_SDN_MSG_NUM * sizeof(struct sdn_data_buf) + i * COEX_SDN_BUF_SIZE);
		list_add_tail(&pdata_buf->list, &g_sdn_client_intf.tx.coex_free_list);
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

#ifndef CONFIG_SDN_HOST
void sdn_client_intf_close(void);
static void _sdn_ctrl_rx(struct sdn_intf_data_msg *pmsg)
{
	switch (pmsg->type) {
	case SDN_INTF_CTRL_INTF_OPEN:
		sdn_enable();
		break;

	case SDN_INTF_CTRL_INTF_CLOSE:
		sdn_disable();
		break;

	case SDN_INTF_CTRL_PROTO_ADD:
		sdn_add_protocol(pmsg->data[0]);
		break;

	case SDN_INTF_CTRL_PROTO_REMOVE:
		sdn_remove_protocol(pmsg->data[0]);
		break;

#if defined(CONFIG_MP_INCLUDED)
	case SDN_INTF_CTRL_MP:
		sdn_set_mp(pmsg->data[0]);
		break;

	case SDN_INTF_CTRL_BRIDGE_OPEN:
		sdn_bridge_open(pmsg->data[0]);
		break;

	case SDN_INTF_CTRL_BRIDGE_CLOSE:
		sdn_bridge_close();
		break;
#endif
	default:
		return;
		break;
	}

	sdn_c2h(NULL); /* indicate host that msg has been processed. */
	if (pmsg->type == SDN_INTF_CTRL_INTF_CLOSE) {
		sdn_client_intf_close();
	}
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
			list_del_init(&pdata_buf->list);
			rtos_critical_exit(RTOS_CRITICAL_BT);
			switch (pdata_buf->pmsg->protocol) {
			case SDN_INTF_BT:
#ifdef CONFIG_BT_SDN
				bt_hci_client_recv_data(pdata_buf->pmsg->type, pdata_buf->pmsg->data);
#endif
				break;

			case SDN_INTF_154:
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
				rtk_wpan_vhdlc_receive(pdata_buf->pmsg->data, pdata_buf->len - sizeof(struct sdn_intf_data_msg));
#endif
				break;

#ifndef CONFIG_SDN_HOST
			case SDN_INTF_CTRL:
				_sdn_ctrl_rx(pdata_buf->pmsg);
				continue;
#endif
				break;
			default:
				break;
			}

			rtos_critical_enter(RTOS_CRITICAL_BT);
			list_add_tail(&pdata_buf->list, &g_sdn_client_intf.rx.free_list);
			switch (pdata_buf->pmsg->protocol) {
#ifdef CONFIG_BT_SDN
			case SDN_INTF_BT:
				if (pdata_buf->pmsg->type == BT_HCI_H4_CMD) {
					g_sdn_client_intf.rx.num_bt_hci_cmd ++;
				} else {
					g_sdn_client_intf.rx.num_bt_acl_data ++;
				}
				break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
			case SDN_INTF_154:
				g_sdn_client_intf.rx.num_154 ++;
				break;
#endif

			default:
				break;
			}
			rtos_critical_exit(RTOS_CRITICAL_BT);
		}
	}

	g_sdn_client_intf.rx.task.running = 0;
	rtos_sema_delete(g_sdn_client_intf.rx.task.sema);
	rtos_task_delete(NULL);
}

static void sdn_client_rx_deinit(void)
{
	if (g_sdn_client_intf.rx.msg_pool) {
		rtos_mem_free(g_sdn_client_intf.rx.msg_pool);
		g_sdn_client_intf.rx.msg_pool = NULL;
	}

#ifdef CONFIG_BT_SDN
	g_sdn_client_intf.rx.num_bt_hci_cmd = 0;
	g_sdn_client_intf.rx.num_bt_acl_data = 0;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	g_sdn_client_intf.rx.num_154 = 0;
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

	g_sdn_client_intf.rx.ctrl.pmsg = (struct sdn_intf_data_msg *)g_sdn_client_intf.rx.ctrl_msg;

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
	uint32_t rx_num = 0;
	uint32_t i = 0;
	struct sdn_data_buf *pdata_buf = NULL;
	uint8_t *pool, *data;
	uint16_t size = sizeof(struct sdn_intf_data_msg) + SDN_INTF_MAX_DATA_LEN;

#if !defined(CONFIG_SDN_HOST) || !defined(CONFIG_SDN_DEV)
	size = (size + 31) & (~31); /* align with 32 bytes for IPC send */
#endif

	g_sdn_client_intf.rx.task.stop = 0;

#ifdef CONFIG_SDN_HOST
	if (SDN_INTF_ERR_OK != sdn_client_rx_task_init()) {
		goto fail;
	}
#endif

	INIT_LIST_HEAD(&g_sdn_client_intf.rx.free_list);

#ifdef CONFIG_BT_SDN
	g_sdn_client_intf.rx.num_bt_hci_cmd = SDN_CONF_CLIENT_BT_RX_CMD_NUM;
	g_sdn_client_intf.rx.num_bt_acl_data = SDN_CONF_CLIENT_BT_RX_ACL_NUM;
	rx_num += g_sdn_client_intf.rx.num_bt_hci_cmd;
	rx_num += g_sdn_client_intf.rx.num_bt_acl_data;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	g_sdn_client_intf.rx.num_154 = SDN_CONF_CLIENT_154_RX_NUM;
	rx_num += g_sdn_client_intf.rx.num_154;
#endif

	pool = rtos_mem_malloc(rx_num * (sizeof(struct sdn_data_buf) + size));
	if (pool == NULL) {
		goto fail;
	}

	data = pool + rx_num * size;
	for (i = 0; i < rx_num; i++) {
		pdata_buf = (struct sdn_data_buf *)(data + i * sizeof(struct sdn_data_buf));
		pdata_buf->pmsg = (struct sdn_intf_data_msg *)(pool + i * size);
		rtos_critical_enter(RTOS_CRITICAL_BT);
		list_add_tail(&pdata_buf->list, &g_sdn_client_intf.rx.free_list);
		rtos_critical_exit(RTOS_CRITICAL_BT);
	}
	g_sdn_client_intf.rx.msg_pool = pool;

	return SDN_INTF_ERR_OK;

fail:
	sdn_client_rx_deinit();
	return SDN_INTF_ERR_OPEN_FAIL;
}

uint32_t sdn_client_intf_open(void)
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

void sdn_client_intf_close(void)
{
	sdn_client_tx_deinit();
	sdn_client_rx_deinit();
}


uint8_t *sdn_client_intf_get_tx_buf(uint8_t protocol, uint8_t type, uint16_t len, void **pbuf)
{
	struct sdn_data_buf *pdata_buf = NULL;

	if (g_sdn_client_intf.tx.task.stop) {
		return NULL;
	}

	rtos_critical_enter(RTOS_CRITICAL_BT);
	if (!list_empty(&g_sdn_client_intf.tx.free_list)) {
		pdata_buf = list_first_entry(&g_sdn_client_intf.tx.free_list, struct sdn_data_buf, list);
		list_del_init(&pdata_buf->list);
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	*pbuf = pdata_buf;
	if (pdata_buf) {
		pdata_buf->pmsg->protocol = protocol;
		pdata_buf->pmsg->type = type;
		pdata_buf->len = len + sizeof(struct sdn_intf_data_msg);
		return pdata_buf->pmsg->data;
	}

	return NULL;
}
#if defined(CONFIG_BT_COEXIST)
uint8_t *sdn_client_intf_get_coex_buf(uint8_t type, uint16_t len, void **pbuf)
{
	struct sdn_data_buf *pdata_buf = NULL;

	if (g_sdn_client_intf.tx.task.stop) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Failed to get tx buf\r\n");
		return NULL;
	}

	if (len > COEX_SDN_BUF_SIZE - sizeof(struct sdn_intf_data_msg)) {
		RTK_LOGS(TAG_SDN_COEX, RTK_LOG_ERROR, "Too Long len=%d(>%d)\r\n", len, COEX_SDN_BUF_SIZE - sizeof(struct sdn_intf_data_msg));
		return NULL;
	}

	rtos_critical_enter(RTOS_CRITICAL_BT);
	if (!list_empty(&g_sdn_client_intf.tx.coex_free_list)) {
		pdata_buf = list_first_entry(&g_sdn_client_intf.tx.coex_free_list, struct sdn_data_buf, list);
		list_del_init(&pdata_buf->list);
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	*pbuf = pdata_buf;

	if (pdata_buf) {
		pdata_buf->pmsg->protocol = SDN_INTF_COEX;
		pdata_buf->pmsg->type = type;
		pdata_buf->len = len + sizeof(struct sdn_intf_data_msg);
		return pdata_buf->pmsg->data;
	}

	return NULL;
}
#endif
void sdn_client_intf_send(void *pdata_buf)
{
	_add_tail_lock(pdata_buf, &g_sdn_client_intf.tx.busy_list);
	rtos_sema_give(g_sdn_client_intf.tx.task.sema);
}

uint8_t sdn_client_intf_get_free_rx_num(uint8_t type, uint8_t sub_type)
{
	uint8_t free_rx_num = 0;
	switch (type) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		if (sub_type == BT_HCI_H4_CMD) {
			free_rx_num = g_sdn_client_intf.rx.num_bt_hci_cmd;
		} else {
			free_rx_num = g_sdn_client_intf.rx.num_bt_acl_data;
		}
		break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	case SDN_INTF_154:
		free_rx_num = g_sdn_client_intf.rx.num_154;
		break;
#endif
	default:
		break;
	}

	return free_rx_num;
}

uint8_t sdn_client_intf_get_rx_bt_acl_max_num(void)
{
	return SDN_CONF_CLIENT_BT_RX_ACL_NUM;
}

uint8_t sdn_client_intf_get_rx_bt_acl_max_len(void)
{
	return (SDN_INTF_MAX_DATA_LEN - 1);
}

#ifdef CONFIG_SDN_HOST
void sdn_host_init(void);
#endif
void sdn_client_init(void)
{
#ifdef CONFIG_SDN_HOST
	sdn_host_init();
#else
	sdn_client_rx_task_init(); /* in IPC case, rx task is always active. */
#endif
}

extern int rtk_ot_start(void);
extern int rtk_ot_loop_exit(void);
extern bool ble_ll_init(void);
extern void ble_ll_deinit(void);
extern uint32_t sdn_hal_irq_hdl_high(void *param);
extern uint32_t sdn_hal_irq_hdl_low(void *param);
extern void sdn_parse_efuse(void);
extern void sdn_pwr_leave_suspend(void);
extern void sdn_pwr_enter_lps(void);

bool sdn_enable(void)
{
	rtos_critical_enter(RTOS_CRITICAL_BT);
	sdn_pwr_leave_suspend();
	rtos_critical_exit(RTOS_CRITICAL_BT);

	if (SDN_INTF_ERR_OK != sdn_client_intf_open()) {
		return false;
	}

	sdn_log_init();

	sdn_sche_init();

	//HAL Enable
	sdn_hal_power_on();
	sdn_parse_efuse();
	sdn_hal_mac_enable((IRQ_FUN)sdn_hal_irq_hdl_high, (IRQ_FUN)sdn_hal_irq_hdl_low);
	sdn_hal_phy_init();

	sdn_watchdog_init();
#if defined(CONFIG_BT_COEXIST)
	sdn_coex_b2w_scbd_bt_on(1, 0);
#endif

	sdn_pwr_suspend_init();
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

#if defined(CONFIG_WPAN_THREAD_EN) && CONFIG_WPAN_THREAD_EN
	case SDN_INTF_154:
		rtk_ot_start();
		break;
#endif

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

#if defined(CONFIG_WPAN_THREAD_EN) && CONFIG_WPAN_THREAD_EN
	case SDN_INTF_154:
		rtk_ot_loop_exit();
		break;
#endif

	default:
		break;
	}
}

void sdn_disable(void)
{
	sdn_hal_mac_disable();

	sdn_hal_power_off();

	sdn_sche_deinit();

#ifdef CONFIG_SDN_HOST
	sdn_client_intf_close();
#endif
}