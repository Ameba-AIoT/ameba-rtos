#ifndef _RTW_SDIO_H_
#define _RTW_SDIO_H_

struct whc_sdio;

#include "autoconf.h"
#include "ameba_inic.h"
#include "whc_sdio_host_reg.h"
#include "whc_sdio_host_drvio.h"

#define CONFIG_SDIO_TX_ENABLE_AVAL_INT
#define CONFIG_SDIO_TX_OVF_CTRL
//#define CONFIG_SDIO_RX_AGGREGATION //for Rx Aggregation
//#define CONFIG_SDIO_TX_AGGREGATION

//#define CONFIG_POWER_SAVING
//#define CONFIG_PS_DYNAMIC_CHK
//#define SDIO_HOST_FAKE_SLEEP

#define SIZE_RX_DESC	(sizeof(INIC_RX_DESC))
#define SIZE_TX_DESC	(sizeof(INIC_TX_DESC))

#define BUF_ALIGN_SZ	4

#define SDIO_BLOCK_SIZE		256
#define ETH_ALEN	6 //ethernet address length

#define MAX_RX_AGG_NUM 6

#define PWR_STATE_ACTIVE	0
#define PWR_STATE_SLEEP		1

enum RPWM2_EVENT {
	RPWM2_PWR_SUSPEND		= 0,		// Clock Gated
	RPWM2_PWR_RESUME		= 1,		// Wakeup event
};
struct whc_sdio {
	struct mutex	lock; /* mutex to protect send host event_priv message */

	wait_queue_head_t txbd_wq;
	u8 tx_avail_int_triggered;

	struct sdio_func *func;

	u32 		sdio_himr;
	u32 		sdio_hisr;
	u16			txbd_size;
	u16			txbd_wptr;
	u16			rxbd_num;
	u16 		SdioTxBDFreeNum;
	u32 		SdioTxMaxSZ; //The Size of Single Tx buf addressed by TX_BD
	u8	func_number;
	u8	tx_block_mode;
	u8	rx_block_mode;
	u32 block_transfer_len;

	s32 bSurpriseRemoved;
	u8	irq_alloc;

	void *sys_sdio_irq_thd;

	u8 dev_state;

	void (*rx_recv_notify)(void);
};

extern struct whc_sdio whc_sdio_priv;
extern struct hci_ops_t whc_sdio_host_intf_ops;

/* SDIO common functions */
u32 rtw_sdio_init_common(struct whc_sdio *priv);
u32 rtw_sdio_get_rx_len(struct whc_sdio *priv);

void whc_sdio_host_send_data(u8 *buf, u32 len, struct sk_buff *pskb);

int whc_sdio_xfer_download(struct whc_sdio *priv);


#endif  //_RTW_SDIO_H_
