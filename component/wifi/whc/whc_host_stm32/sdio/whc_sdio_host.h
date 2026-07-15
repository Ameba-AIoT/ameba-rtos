
#ifndef __WHC_SDIO_HOST_H__
#define __WHC_SDIO_HOST_H__

#include "os_wrapper.h"
#include "../../whc_common/host_sdio/whc_host_sdio_desc.h"
#include "../../whc_common/host_sdio/whc_host_sdio_trx_common.h"

//#define tskIDLE_PRIORITY	0
#define DEV_DMA_ALIGN	1
#define SDIO_INT_MODE	1
#define SDIO_BLOCK_SIZE		256
#define CALCULATE_FREE_TXBD 1 // for DP bug, comment after 7005 for tp
#define CONFIG_SDIO_TX_ENABLE_AVAL_INT 1

#define CONFIG_WHC_CMD_PATH		1

#define NET_IF_NUM 2
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

#define MAX_SKB_BUF_SIZE_NORMAL	1664

//TODO check real stack size
#define SDIO_POLLING_STACK_SIZE 1024

/* auth/assoc/key resnd limit can be configured, refer max >> RTW_JOIN_TIMEOUT
 * including auth + assoc + 4way handshake, no dhcp
 */
#define RTW_JOIN_TIMEOUT (10 * 12000 + 13100 + 20200 + 50) //(MAX_CNT_SCAN_TIMES * SCANNING_TIMEOUT + MAX_JOIN_TIMEOUT + KEY_EXCHANGE_TIMEOUT + 50)

#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)

struct whc_sdio {
	rtos_mutex_t lock; /* mutex to protect send host message */
	rtos_sema_t txbd_wq;
	rtos_mutex_t host_send; /* sema to protect inic  host send */
	rtos_sema_t host_send_api; /* sema to protect inic ipc host send api */
	rtos_sema_t host_recv_wake; /* for recv task */
	rtos_sema_t host_recv_done; /* for recv task */
	rtos_sema_t host_irq; /* for sdio irq */
	void *func;
	uint8_t *rx_buf;
	u8 tx_buf[SIZE_TX_DESC + sizeof(struct whc_msg_info) + MAX_SKB_BUF_SIZE_NORMAL]; /* fix tx buf */

	uint32_t sdio_himr;
	uint32_t sdio_hisr;
	uint32_t block_transfer_len;
	uint32_t SdioTxMaxSZ; //The Size of Single Tx buf addressed by TX_BD

	uint16_t txbd_size;
	uint16_t txbd_wptr;
	uint16_t rxbd_num;
	uint16_t SdioTxBDFreeNum;
	uint8_t SdioRxFIFOCnt;
	uint8_t tx_avail_int_triggered;
	uint8_t tx_block_mode;
	uint8_t rx_block_mode;
	uint8_t dev_state;
	uint8_t whc_host_init_done;
};

/* ---- port glue for the shared whc_host_sdio_trx_common.c ---- */
#define WHC_MALLOC(_sz)		rtos_mem_malloc(_sz)
#define WHC_FREE(_p)		rtos_mem_free(_p)
#define WHC_SEM_TAKE(_s)	rtos_sema_take((_s), MUTEX_WAIT_TIMEOUT)
#define WHC_SEM_TAKE_TIMEOUT(_s, _t)	rtos_sema_take((_s), (_t))
#define WHC_HOST_SDIO_RX_INT_DISABLE(_p) \
	do { uint32_t _himr = (_p)->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK); \
		rtw_write32((_p), SDIO_REG_HIMR, _himr); } while (0)
#define WHC_HOST_SDIO_RX_INT_RESTORE(_p) \
	do { uint32_t _himr = (_p)->sdio_himr; \
		rtw_write32((_p), SDIO_REG_HIMR, _himr); } while (0)
#define WHC_SEM_GIVE(_s)	rtos_sema_give(_s)
#define WHC_HOST_SDIO_HISR_CLEAR(_p) \
	do { uint32_t _v = (_p)->sdio_hisr & MASK_SDIO_HISR_CLEAR; \
		if (_v) { rtw_write32((_p), SDIO_REG_HISR,  _v); } } while (0)

#define WHC_MUTEX_TAKE(_m, _t)		rtos_mutex_take((_m), (_t))
#define WHC_MUTEX_GIVE(_m)		rtos_mutex_give(_m)
#define WHC_MSLEEP(_ms)			vTaskDelay(pdMS_TO_TICKS(_ms))
#define WHC_HOST_SDIO_ALLOC_IRQ(_p)		/* no-op */

#define WHC_HOST_SDIO_RX_DEFAULT(_b) \
	do { whc_host_pkt_rx_to_user((_b) + SIZE_RX_DESC); WHC_FREE(_b); } while (0)


void whc_sdio_host_init_drv(void);
void whc_host_sdio_recv_pkts(uint8_t *buf);

uint32_t rtw_sdio_init(struct whc_sdio *priv);

s32 wifi_on(uint8_t mode);
void whc_host_init(void);

#endif


