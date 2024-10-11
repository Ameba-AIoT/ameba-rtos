#ifndef _RTW_SDIO_H_
#define _RTW_SDIO_H_

struct inic_sdio;

#include "ameba_inic.h"
#include "rtw_sdio_reg.h"
#include "rtw_sdio_drvio.h"
#include "rtw_sdio_ops.h"

#define CALCULATE_FREE_TXBD
#define CONFIG_SDIO_TX_ENABLE_AVAL_INT
#define CONFIG_SDIO_TX_OVF_CTRL
//#define CONFIG_SDIO_RX_AGGREGATION //for Rx Aggregation
//#define CONFIG_SDIO_TX_AGGREGATION

//#define CONFIG_POWER_SAVING
//#define CONFIG_PS_DYNAMIC_CHK

#define BUF_ALIGN_SZ	4

#define SDIO_BLOCK_SIZE		256
#define ETH_ALEN	6 //ethernet address length

#define SD_IO_TRY_CNT (8)
#define MAX_CONTINUAL_IO_ERR SD_IO_TRY_CNT

#define MAX_RX_AGG_NUM 6

//#define SDIO_HOST_FAKE_SLEEP

#define PWR_STATE_ACTIVE	0
#define PWR_STATE_SLEEP		1

enum RPWM2_EVENT {
	RPWM2_PWR_SUSPEND		= 0,		// Clock Gated
	RPWM2_PWR_RESUME		= 1,		// Wakeup event
};
struct inic_sdio {
	struct mutex	lock; /* mutex to protect send host event_priv message */

	wait_queue_head_t txbd_wq;
	u8 tx_avail_int_triggered;

	struct sdio_func *func;

	u32 		sdio_himr;
	u32 		sdio_hisr;
	u16			txbd_size;
	u16			rxbd_num;
	u16 		SdioTxBDFreeNum;
	u32 		SdioTxMaxSZ; //The Size of Single Tx buf addressed by TX_BD
	u8			SdioRxFIFOCnt;
	u8	func_number;
	u8	tx_block_mode;
	u8	rx_block_mode;
	u32 block_transfer_len;

	s32 bSurpriseRemoved;
	u8	irq_alloc;

	void *sys_sdio_irq_thd;

	atomic_t continual_io_error;

	u8 dev_state;

	int (*rx_process_func)(struct sk_buff *pskb);

};

extern struct inic_sdio inic_sdio_priv;
extern struct hci_ops_t sdio_intf_ops;

#endif  //_RTW_SDIO_H_