#include <ameba.h>
#include <sdn_intf.h>
#include <ameba_ipc_api.h>
#include <ameba_ipccfg.h>
#include <ameba_ipc.h>

void sdn_client_tx_buf_complete(struct sdn_data_buf *pdata_buf);

static void _client_ipc_rx_int_hdl(void *data, uint32_t irq_status, uint32_t channel_num)
{
	(void)data;
	(void)irq_status;
	PIPC_MSG_STRUCT p_ipc_rx_msg = ipc_get_message(IPC_AP_TO_NP, channel_num);
	struct sdn_data_buf *pbuf = (struct sdn_data_buf *)(p_ipc_rx_msg->msg);

	DCache_Invalidate(p_ipc_rx_msg->msg, p_ipc_rx_msg->msg_len);

	if (pbuf->owner == SDN_INTF_MEM_OWNER_DEV) { /* AP returns memory */
		sdn_client_tx_buf_complete(pbuf);
	} else {
		sdn_h2c(pbuf->protocol, pbuf->type, pbuf->data, pbuf->len);
	}
}

void sdn_c2h(struct sdn_data_buf *pdata_buf)
{
	IPC_MSG_STRUCT ipc_tx = {0};

	if (pdata_buf) {
		ipc_tx.msg = (uint32_t)pdata_buf;
		ipc_tx.msg_len = sizeof(struct sdn_data_buf) + pdata_buf->len;
		DCache_Clean(ipc_tx.msg, ipc_tx.msg_len);
	}

	ipc_send_message(IPC_NP_TO_AP, IPC_N2A_BT_VIRTUAL_HCI, &ipc_tx);
}

// controller IPC TX channel table define
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE sdn_client_ipc_tx_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = (void *)NULL,
	.RxIrqData = (void *) NULL,
	.Txfunc = IPC_TXHandler,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_NP_TO_AP,
	.IPC_Channel = IPC_N2A_BT_VIRTUAL_HCI  // channel number 8 on NP
};

// controller IPC RX channel table define
IPC_TABLE_DATA_SECTION
const IPC_INIT_TABLE sdn_client_ipc_rx_table = {
	.USER_MSG_TYPE = IPC_USER_POINT,
	.Rxfunc = _client_ipc_rx_int_hdl,
	.RxIrqData = (void *)NULL,
	.Txfunc = (void *)NULL,
	.TxIrqData = (void *) NULL,
	.IPC_Direction = IPC_AP_TO_NP,
	.IPC_Channel = IPC_A2N_BT_VIRTUAL_HCI  // channel number 9 on AP
};