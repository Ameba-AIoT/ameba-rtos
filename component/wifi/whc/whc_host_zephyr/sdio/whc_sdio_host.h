
#ifndef __WHC_SDIO_HOST_H__
#define __WHC_SDIO_HOST_H__

#include "../../whc_common/host_sdio/whc_host_sdio_desc.h"
#include "../../whc_common/host_sdio/whc_host_sdio_trx_common.h"

#define SDIO_BLOCK_SIZE		256
//check ic when porting
#define CALCULATE_FREE_TXBD // for DP bug, comment after 7005 for tp

#define MUTEX_WAIT_TIMEOUT K_FOREVER
#define SEMA_MAX_COUNT	0xFFFFFFFF

#define MAX_SKB_BUF_SIZE_NORMAL	1664

struct whc_sdio {
	/* one fixed tx buf, protected by host_send sema, sync tansfer for sdio host */
	__attribute__((aligned(4))) uint8_t tx_buf[SIZE_TX_DESC + sizeof(struct whc_msg_info) + MAX_SKB_BUF_SIZE_NORMAL];

	struct k_mutex lock; /* mutex to protect send host message */
	struct k_sem txbd_wq;
	struct k_sem host_send; /* sema to protect whc host send */
	struct k_sem host_recv_wake; /* for recv task */
	struct k_sem host_recv_done; /* for recv task */

	/* for host<->dev synchronous request/return */
	struct k_mutex send_mutex;
	struct k_sem api_ret_sema;
	struct sdio_func *func;

	uint8_t *rx_buf;
	uint8_t *rx_ret_msg;

	uint32_t 	sdio_himr;
	uint32_t	sdio_hisr;
	uint32_t	block_transfer_len;
	uint32_t	SdioTxMaxSZ; //The Size of Single Tx buf addressed by TX_BD

	uint16_t	txbd_size;
	uint16_t	txbd_wptr;
	uint16_t	rxbd_num;
	uint16_t	SdioTxBDFreeNum;

	uint8_t	SdioRxFIFOCnt;
	uint8_t	tx_avail_int_triggered;
	uint8_t	tx_block_mode;
	uint8_t	rx_block_mode;
	uint8_t dev_state;
	uint8_t b_waiting_for_ret: 1;
};

struct spdio_buf_t {
	void *priv; //priv data from user
	uint32_t buf_allocated; //The spdio buffer allocated address
	uint32_t buf_addr; //The spdio buffer physical address, it must be 4-bytes aligned
	uint16_t size_allocated; //The actual allocated size
	uint16_t buf_size;
	uint8_t type; //The type of the data which this buffer carries, spdio_rx_data_t and spdio_tx_data_t
	uint8_t reserved;
};

struct whc_txbuf_info_t {
	struct spdio_buf_t txbuf_info;
	void *ptr;	/* pointer to the original buffer*/
	u8 is_skb: 1;	/* the original buffer is skb or not */
};

/* ---- port glue for the shared whc_host_sdio_trx_common.c ---- */
#define WHC_MALLOC(_sz)		k_malloc(_sz)
#define WHC_FREE(_p)		k_free(_p)
#define WHC_SEM_TAKE(_s)	k_sem_take(&(_s), MUTEX_WAIT_TIMEOUT)
#define WHC_SEM_TAKE_TIMEOUT(_s, _t)	rtos_sema_take(&(_s), (_t))
#define WHC_HOST_SDIO_RX_INT_DISABLE(_p) \
	do { uint32_t _himr = (_p)->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK); \
		rtw_write32((_p), SDIO_REG_HIMR,  _himr); } while (0)
#define WHC_HOST_SDIO_RX_INT_RESTORE(_p) \
	do { uint32_t _himr = (_p)->sdio_himr; \
		rtw_write32((_p), SDIO_REG_HIMR,  _himr); } while (0)
#define WHC_SEM_GIVE(_s)	k_sem_give(&(_s))
#define WHC_HOST_SDIO_HISR_CLEAR(_p) \
	do { uint32_t _v = (_p)->sdio_hisr & MASK_SDIO_HISR_CLEAR; \
		if (_v) { rtw_write32((_p), SDIO_REG_HISR, _v); } } while (0)

#define WHC_MUTEX_TAKE(_m, _t)		k_mutex_lock(&(_m), (_t))
#define WHC_MUTEX_GIVE(_m)		k_mutex_unlock(&(_m))
#define WHC_MSLEEP(_ms)			k_sleep(K_MSEC(_ms))
#define WHC_HOST_SDIO_ALLOC_IRQ(_p)		rtw_sdio_alloc_irq(_p)

#define WHC_HOST_SDIO_RX_DEFAULT(_b)	whc_host_pkt_rx_to_user(_b)

void whc_host_sdio_recv_pkts(uint8_t *buf);

uint32_t rtw_sdio_init(struct whc_sdio *priv);

s32 wifi_on(uint8_t mode);
void whc_host_sdio_init(void);

#endif

