#include <basic_types.h>
#include <ameba.h>
#include <sdn_host.h>
#include <rtk_status.h>
#include <os_wrapper.h>
#include <ameba_ipc_api.h>
#include <ameba_ipccfg.h>
#include <ameba_ipc.h>
#include <sdn_intf.h>
#ifdef CONFIG_BT_SDN
#include <sdn_user_conf_bt.h>
#endif
#include <sdn_user_conf_intf.h>

struct sdn_host_ipc_tx {
	struct sdn_data_buf *pbuf;
	uint32_t len;
	void *ctrl_sema;
	struct sdn_intf_task task;
};

struct sdn_host_ipc_rx {
	struct list_head busy_list;
	struct sdn_intf_task task;
};

static struct sdn_host_ipc {
	struct sdn_host_ipc_tx tx;
	struct sdn_host_ipc_rx rx;
} g_sdn_host_intf = {0};

#define SDN_HOST_IPC_RX_TASK_PRI                5
#define SDN_HOST_IPC_RX_TASK_SIZE               768


#if defined(CONFIG_BT_SDN)
#if defined(CONFIG_BLE_LL_EXT_ADV_ENABLE) || defined(CONFIG_BLE_LL_DATA_LEN_EXT_ENABLE)
#define SDN_INTF_MAX_BT_LEN        258
#else
#define SDN_INTF_MAX_BT_LEN        42     /* cmd 0x2027 has the max length: 3 + 39 */
#endif
#else
#define SDN_INTF_MAX_BT_LEN 	   0
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
#define SDN_INTF_HOST_TX_MAX_LEN 	(SDN_INTF_MAX_BT_LEN > SDN_INTF_MAX_154_LEN ? SDN_INTF_MAX_BT_LEN : SDN_INTF_MAX_154_LEN)
#else
#define SDN_INTF_HOST_TX_MAX_LEN 	SDN_INTF_MAX_BT_LEN
#endif

extern void *sdn_host_mtx;
static void sdn_host_ipc_return_buf(struct sdn_data_buf *pdata_buf)
{
	while (rtos_mutex_take(sdn_host_mtx, 100) != RTK_SUCCESS) {
		if (g_sdn_host_intf.rx.task.stop == 1) { /* To avoid mutex deadlock when sdn disable */
			return;
		}
	}
	sdn_h2c(SDN_INTF_MEM, 0, pdata_buf, 0);
	rtos_mutex_give(sdn_host_mtx);
}

u32 IPC_wait_idle(IPC_TypeDef *IPCx, u32 IPC_ChNum);

static void sdn_host_ipc_tx_deinit(void)
{
	if (g_sdn_host_intf.tx.pbuf) {
		rtos_mem_free(g_sdn_host_intf.tx.pbuf);
		g_sdn_host_intf.tx.pbuf = NULL;
	}

	if (g_sdn_host_intf.tx.ctrl_sema) {
		rtos_sema_delete(g_sdn_host_intf.tx.ctrl_sema);
		g_sdn_host_intf.tx.ctrl_sema = NULL;
	}
}

static uint32_t sdn_host_ipc_tx_init(void)
{
	if (g_sdn_host_intf.tx.ctrl_sema == NULL) {
		if (RTK_SUCCESS != rtos_sema_create(&g_sdn_host_intf.tx.ctrl_sema, 0, 1)) {
			goto fail;
		}
	}

	if (g_sdn_host_intf.tx.pbuf == NULL) {
		g_sdn_host_intf.tx.pbuf = (struct sdn_data_buf *)rtos_mem_malloc(sizeof(struct sdn_data_buf) + SDN_INTF_HOST_TX_MAX_LEN);
		if (g_sdn_host_intf.tx.pbuf == NULL) {
			goto fail;
		}
		g_sdn_host_intf.tx.pbuf->data = (uint8_t *)g_sdn_host_intf.tx.pbuf + sizeof(struct sdn_data_buf);
		g_sdn_host_intf.tx.pbuf->owner = SDN_INTF_MEM_OWNER_HOST;
	}
	return SDN_INTF_ERR_OK;
fail:
	sdn_host_ipc_tx_deinit();
	return SDN_INTF_ERR_OPEN_FAIL;
}

static void sdn_host_ipc_rx_deinit(void)
{
	//stop task
	if (g_sdn_host_intf.rx.task.running) {
		g_sdn_host_intf.rx.task.stop = 1;
		rtos_sema_give(g_sdn_host_intf.rx.task.sema);

		while (g_sdn_host_intf.rx.task.running) {
			rtos_time_delay_ms(1);
		}
	}

	if (g_sdn_host_intf.rx.task.sema) {
		rtos_sema_delete(g_sdn_host_intf.rx.task.sema);
		g_sdn_host_intf.rx.task.sema = NULL;
	}
}


static void sdn_host_ipc_rx_task_hdl(void *pcontext)
{
	(void)pcontext;
	struct sdn_data_buf *pdata_buf = NULL;

	g_sdn_host_intf.rx.task.running = 1;
	g_sdn_host_intf.rx.task.stop = 0;

	while (1) {
		rtos_sema_take(g_sdn_host_intf.rx.task.sema, MUTEX_WAIT_TIMEOUT);
		if (g_sdn_host_intf.rx.task.stop == 1) {
			break;
		}

		while (1) {
			rtos_critical_enter(RTOS_CRITICAL_BT);
			if (list_empty(&g_sdn_host_intf.rx.busy_list)) {
				rtos_critical_exit(RTOS_CRITICAL_BT);
				break;
			}

			pdata_buf = (struct sdn_data_buf *)g_sdn_host_intf.rx.busy_list.next;
			list_del_init(&pdata_buf->list);
			rtos_critical_exit(RTOS_CRITICAL_BT);

			sdn_c2h(pdata_buf);
			sdn_host_ipc_return_buf(pdata_buf);
		}
	}

	rtos_sema_delete(g_sdn_host_intf.rx.task.sema);
	g_sdn_host_intf.rx.task.sema = NULL;
	g_sdn_host_intf.rx.task.running = 0;
	rtos_task_delete(NULL);
}

static uint32_t sdn_host_ipc_rx_init(void)
{
	INIT_LIST_HEAD(&g_sdn_host_intf.rx.busy_list);

	//RX task
	if (RTK_SUCCESS != rtos_sema_create(&g_sdn_host_intf.rx.task.sema, 0, 1)) {
		goto fail;
	}

	if (RTK_SUCCESS != rtos_task_create(&g_sdn_host_intf.rx.task.task, "sdn host ipc rx",
										sdn_host_ipc_rx_task_hdl, NULL,
										SDN_HOST_IPC_RX_TASK_SIZE, SDN_HOST_IPC_RX_TASK_PRI)) {
		goto fail;
	}

	return SDN_INTF_ERR_OK;

fail:
	sdn_host_ipc_rx_deinit();
	return SDN_INTF_ERR_OPEN_FAIL;
}

bool sdn_enable(void)
{
	if (SDN_INTF_ERR_OK != sdn_host_ipc_tx_init()) {
		return false;
	}

	if (SDN_INTF_ERR_OK != sdn_host_ipc_rx_init()) {
		sdn_host_ipc_tx_deinit();
		return false;
	}

	return (SDN_INTF_ERR_OK == sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_INTF_OPEN, NULL, 0));
}

void sdn_disable(void)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_INTF_CLOSE, NULL, 0);

	sdn_host_ipc_tx_deinit();
	sdn_host_ipc_rx_deinit();
}

void sdn_add_protocol(uint8_t protocol)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_PROTO_ADD, &protocol, sizeof(uint8_t));
}

void sdn_remove_protocol(uint8_t protocol)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_PROTO_REMOVE, &protocol, sizeof(uint8_t));
}

#ifdef CONFIG_MP_INCLUDED
void sdn_set_mp(bool is_mp)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_MP, &is_mp, sizeof(bool));
}

void sdn_bridge_open(bool to_loguart)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_BRIDGE_OPEN, &to_loguart, sizeof(bool));
}

void sdn_bridge_close(void)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_BRIDGE_CLOSE, NULL, 0);
}
#endif

void sdn_fix_bt_addr(uint8_t *bdaddr)
{
	sdn_h2c(SDN_INTF_CTRL, SDN_INTF_CTRL_FIX_ADDR, bdaddr, 6);
}

static void sdn_host_ipc_rx_int_hdl(void *data, uint32_t irq_status, uint32_t channel_num)
{
	PIPC_MSG_STRUCT p_ipc_rx_msg = NULL;

	(void)data;
	(void)irq_status;

	p_ipc_rx_msg = ipc_get_message(IPC_NP_TO_AP, channel_num);

	if (!p_ipc_rx_msg) {
		return;
	}

	if (p_ipc_rx_msg->msg_len) {
		DCache_Invalidate(p_ipc_rx_msg->msg, p_ipc_rx_msg->msg_len);
		list_add_tail(&((struct sdn_data_buf *)(p_ipc_rx_msg->msg))->list, &g_sdn_host_intf.rx.busy_list);
		rtos_sema_give(g_sdn_host_intf.rx.task.sema);
	} else {
		rtos_sema_give(g_sdn_host_intf.tx.ctrl_sema);
	}
}

/* if protocol is SDN_INTF_CTRL, len couldn't exceed 6. */
uint32_t sdn_h2c(uint8_t protocol, uint8_t type, void *pdata, uint16_t len)
{
	IPC_MSG_STRUCT ipc_tx = {0};
	struct sdn_data_buf *pbuf = NULL;

	// RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "sdn_h2c protocol %d, type %d\r\n", protocol, type);

	if (len > SDN_INTF_HOST_TX_MAX_LEN) {
		RTK_LOGS(NOTAG, RTK_LOG_ALWAYS, "sdn_h2c ERROR %d > max(%d)\r\n", len, SDN_INTF_HOST_TX_MAX_LEN);
		return SDN_INTF_ERR_TX_DATA_FAIL;
	}

	if (protocol == SDN_INTF_MEM) { /* return mem to NP */
		pbuf = (struct sdn_data_buf *)pdata;
	} else {
		pbuf = g_sdn_host_intf.tx.pbuf;
		pbuf->protocol = protocol;
		pbuf->type = type;
		pbuf->len = len;
		memcpy(pbuf->data, pdata, len);
	}

	ipc_tx.msg_len = pbuf->len + sizeof(struct sdn_data_buf);
	ipc_tx.msg = (uint32_t)pbuf;
	DCache_Clean(ipc_tx.msg, ipc_tx.msg_len);

	if (IPC_SEND_SUCCESS != ipc_send_message(IPC_AP_TO_NP, IPC_A2N_BT_VIRTUAL_HCI, &ipc_tx)) {
		return SDN_INTF_ERR_TX_DATA_FAIL;
	}

	IPC_wait_idle(IPC_GetDev(IPC_AP_TO_NP, 0), IPC_A2N_BT_VIRTUAL_HCI);

	if (protocol == SDN_INTF_CTRL) {
		/* client only has one memory buffer for ctrl message, so send ctrl msg one by one. */
		rtos_sema_take(g_sdn_host_intf.tx.ctrl_sema, MUTEX_WAIT_TIMEOUT);
	}
	return SDN_INTF_ERR_OK;
}


//Host IPC TX
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE sdn_ipc_host_tx_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = (void *)NULL,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_AP_TO_NP,
	.IPC_Channel = IPC_A2N_BT_VIRTUAL_HCI  // channel number 9 on AP
};


// HOST IPC RX channel table define
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE sdn_ipc_host_rx_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = sdn_host_ipc_rx_int_hdl,
	.RxIrqData = (void *)NULL,
	.Txfunc = (void *)NULL,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_NP_TO_AP,
	.IPC_Channel = IPC_N2A_BT_VIRTUAL_HCI  // channel number 8 on NP
};