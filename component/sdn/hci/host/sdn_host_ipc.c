#include <basic_types.h>
#include <ameba.h>
#include <sdn_host.h>
#include <rtk_status.h>
#include <os_wrapper.h>
#include <ameba_ipc_api.h>
#include <ameba_ipccfg.h>
#include <ameba_ipc.h>
#include <sdn_intf.h>
#include <sdn_user_conf_intf.h>

struct sdn_host_ipc_tx {
	uint8_t *pbuf;
	uint32_t len;
	struct list_head free_list;
	struct list_head busy_list;
	uint8_t *msg_pool;
	void *ctrl_sema;
	struct sdn_intf_task task;
};

struct sdn_host_ipc_rx {
#ifdef CONFIG_BT_SDN
	uint8_t num_bt;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	uint8_t num_154;
#endif
	struct list_head free_list;
	struct list_head busy_list;
	uint8_t *msg_pool;

	struct sdn_intf_task task;
};

static struct sdn_host_ipc {
	struct sdn_host_ipc_tx tx;
	struct sdn_host_ipc_rx rx;
} g_sdn_host_intf = {0};

#define SDN_HOST_IPC_RX_TASK_PRI                5
#define SDN_HOST_IPC_RX_TASK_SIZE               768


#define SDN_CONF_HOST_BT_RX_DATA_NUM                               8
#define SDN_CONF_HOST_BT_RX_EVT_NUM                                8
#define SDN_CONF_HOST_BT_RX_SUM_NUM                                (SDN_CONF_HOST_BT_RX_DATA_NUM + SDN_CONF_HOST_BT_RX_EVT_NUM)

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
#define SDN_CONF_HOST_154_RX_NUM                                   4
#else
#define SDN_CONF_HOST_154_RX_NUM                                   0
#endif


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
		g_sdn_host_intf.tx.pbuf = rtos_mem_malloc(sizeof(struct sdn_intf_data_msg) + SDN_INTF_MAX_DATA_LEN);
		if (g_sdn_host_intf.tx.pbuf == NULL) {
			goto fail;
		}
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

	//delete msg queue
	if (g_sdn_host_intf.rx.msg_pool) {
		rtos_mem_free(g_sdn_host_intf.rx.msg_pool);
		g_sdn_host_intf.rx.msg_pool = NULL;
	}

#ifdef CONFIG_BT_SDN
	g_sdn_host_intf.rx.num_bt = 0;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	g_sdn_host_intf.rx.num_154 = 0;
#endif
}

static struct sdn_data_buf *sdn_host_ipc_rx_get_buf(uint8_t protocol)
{
	struct sdn_data_buf *pdata_buf = NULL;
	bool balloc_avaliable = false;

	rtos_critical_enter(RTOS_CRITICAL_BT);
	switch (protocol) {
#ifdef CONFIG_BT_SDN
	case SDN_INTF_BT:
		if (g_sdn_host_intf.rx.num_bt) {
			g_sdn_host_intf.rx.num_bt--;
			balloc_avaliable = true;
		}
		break;
#endif

#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	case SDN_INTF_154:
		if (g_sdn_host_intf.rx.num_154) {
			g_sdn_host_intf.rx.num_154--;
			balloc_avaliable = true;
		}
		break;
#endif

	default:
		break;
	}

	if (balloc_avaliable) {
		if (false == list_empty(&g_sdn_host_intf.rx.free_list)) {
			pdata_buf = (struct sdn_data_buf *)g_sdn_host_intf.rx.free_list.next;
			list_del_init(&pdata_buf->list);
		}
	}
	rtos_critical_exit(RTOS_CRITICAL_BT);

	return pdata_buf;
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

			rtos_critical_enter(RTOS_CRITICAL_BT);
			switch (pdata_buf->pmsg->protocol) {
#ifdef CONFIG_BT_SDN
			case SDN_INTF_BT:
				g_sdn_host_intf.rx.num_bt++;
				break;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
			case SDN_INTF_154:
				g_sdn_host_intf.rx.num_154++;
				break;
#endif
			default:
				break;
			}
			list_add_tail(&pdata_buf->list, &g_sdn_host_intf.rx.free_list);
			rtos_critical_exit(RTOS_CRITICAL_BT);
		}
	}

	rtos_sema_delete(g_sdn_host_intf.rx.task.sema);
	g_sdn_host_intf.rx.task.sema = NULL;
	g_sdn_host_intf.rx.task.running = 0;
	rtos_task_delete(NULL);
}

static uint32_t sdn_host_ipc_rx_init(void)
{
	uint32_t rx_num = 0;
	uint32_t i = 0;
	struct sdn_data_buf *pdata_buf = NULL;
	uint8_t *pool, *data;
	uint16_t size = sizeof(struct sdn_intf_data_msg) + SDN_INTF_MAX_DATA_LEN;

#if !defined(CONFIG_SDN_HOST) || !defined(CONFIG_SDN_DEV)
	size = (size + 31) & (~31); /* align with 32 bytes for IPC send */
#endif

#ifdef CONFIG_BT_SDN
	g_sdn_host_intf.rx.num_bt = SDN_CONF_HOST_BT_RX_SUM_NUM;
	rx_num += g_sdn_host_intf.rx.num_bt;
#endif
#if defined(CONFIG_WPAN_DRIVER_VHDLC_PLATFORM) && CONFIG_WPAN_DRIVER_VHDLC_PLATFORM
	g_sdn_host_intf.rx.num_154 = SDN_CONF_HOST_154_RX_NUM;
	rx_num += g_sdn_host_intf.rx.num_154;
#endif

	INIT_LIST_HEAD(&g_sdn_host_intf.rx.busy_list);
	INIT_LIST_HEAD(&g_sdn_host_intf.rx.free_list);

	//Msg resource init
	pool = rtos_mem_malloc(rx_num * (sizeof(struct sdn_data_buf) + size));
	if (pool == NULL) {
		goto fail;
	}

	data = pool + rx_num * size;
	for (i = 0; i < rx_num; i++) {
		pdata_buf = (struct sdn_data_buf *)(data + i * sizeof(struct sdn_data_buf));
		pdata_buf->pmsg = (struct sdn_intf_data_msg *)(pool + i * size);
		list_add_tail(&pdata_buf->list, &g_sdn_host_intf.rx.free_list);
	}
	g_sdn_host_intf.rx.msg_pool = pool;

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
	struct sdn_data_buf *pdata_buf = NULL;

	(void)data;
	(void)irq_status;

	p_ipc_rx_msg = ipc_get_message(IPC_NP_TO_AP, channel_num);

	if (!p_ipc_rx_msg) {
		return;
	}

	if (p_ipc_rx_msg->msg_len) {
		DCache_Invalidate(p_ipc_rx_msg->msg, p_ipc_rx_msg->msg_len);
		pdata_buf = sdn_host_ipc_rx_get_buf(((struct sdn_intf_data_msg *)(p_ipc_rx_msg->msg))->protocol);
		if (pdata_buf != NULL) {
			memcpy(pdata_buf->pmsg, (uint8_t *)p_ipc_rx_msg->msg, p_ipc_rx_msg->msg_len);
			pdata_buf->len = p_ipc_rx_msg->msg_len;

			rtos_critical_enter(RTOS_CRITICAL_BT);
			list_add_tail(&pdata_buf->list, &g_sdn_host_intf.rx.busy_list);
			rtos_critical_exit(RTOS_CRITICAL_BT);

			rtos_sema_give(g_sdn_host_intf.rx.task.sema);
		}
	} else {
		rtos_sema_give(g_sdn_host_intf.tx.ctrl_sema);
	}
}

/* if protocol is SDN_INTF_CTRL, len couldn't exceed 6. */
uint32_t sdn_h2c(uint8_t protocol, uint8_t type, void *pdata, uint16_t len)
{
	IPC_MSG_STRUCT ipc_tx = {0};
	struct sdn_intf_data_msg *pdata_msg = NULL;

	pdata_msg = (struct sdn_intf_data_msg *)g_sdn_host_intf.tx.pbuf;
	pdata_msg->protocol = protocol;
	pdata_msg->type = type;
	memcpy(pdata_msg->data, pdata, len);

	ipc_tx.msg_len = len + sizeof(struct sdn_intf_data_msg);
	ipc_tx.msg = (uint32_t)pdata_msg;
	DCache_Clean(ipc_tx.msg, ipc_tx.msg_len);

	if (IPC_SEND_SUCCESS != ipc_send_message(IPC_AP_TO_NP, IPC_A2N_BT_VIRTUAL_HCI, &ipc_tx)) {
		return SDN_INTF_ERR_TX_CTRL_FAIL;
	}

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