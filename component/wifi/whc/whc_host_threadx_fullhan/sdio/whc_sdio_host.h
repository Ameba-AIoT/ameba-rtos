
#ifndef __WHC_SDIO_HOST_H__
#define __WHC_SDIO_HOST_H__

#include "os_wrapper.h"
#include "../../whc_common/host_sdio/whc_host_sdio_desc.h"
#include "../../whc_common/host_sdio/whc_host_sdio_trx_common.h"

//#define tskIDLE_PRIORITY	0
#define DEV_DMA_ALIGN	1
#define WHC_SDIO_INT_MODE	1
#define WHC_SDIO_INT_GPIO   1
#define SDIO_BLOCK_SIZE		512
#define CALCULATE_FREE_TXBD 1 // for DP
//#define CONFIG_SDIO_TX_ENABLE_AVAL_INT 1

#define CONFIG_WHC_CMD_PATH		1

#define NET_IF_NUM 2
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

#define MAX_SKB_BUF_SIZE_NORMAL	1664

/* auth/assoc/key resnd limit can be configured, refer max >> RTW_JOIN_TIMEOUT
 * including auth + assoc + 4way handshake, no dhcp
 */
#define RTW_JOIN_TIMEOUT (10 * 12000 + 13100 + 20200 + 50) //(MAX_CNT_SCAN_TIMES * SCANNING_TIMEOUT + MAX_JOIN_TIMEOUT + KEY_EXCHANGE_TIMEOUT + 50)

struct whc_sdio {
	/* one fixed tx buf for fullhan, protected by host_send sema */
	__attribute__((aligned(4))) uint8_t tx_buf[SIZE_TX_DESC + sizeof(struct whc_msg_info) + MAX_SKB_BUF_SIZE_NORMAL];

	rtos_mutex_t lock; /* mutex to protect send host message */
	rtos_sema_t txbd_wq;
	rtos_sema_t host_send; /* sema to protect whc host send */
	rtos_sema_t host_send_block_sema; /* sema to block */
	rtos_sema_t host_recv_wake; /* for recv task */
	rtos_sema_t host_recv_done; /* for recv task */
	rtos_sema_t host_irq; /* for sdio irq */
	rtos_mutex_t hw_lock;
	uint8_t *ret;
	uint8_t *rx_buf;
	void *func;

	uint32_t sdio_himr;
	uint32_t sdio_hisr;
	uint32_t SdioTxMaxSZ; //The Size of Single Tx buf addressed by TX_BD
	uint32_t block_transfer_len;
	int ret_len;

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
#define WHC_MALLOC(_sz)		rt_malloc(_sz)
#define WHC_FREE(_p)		rt_free(_p)
#define WHC_SEM_TAKE(_s)	rtos_sema_take((_s), MUTEX_WAIT_TIMEOUT)
#define WHC_SEM_TAKE_TIMEOUT(_s, _t)	rtos_sema_take((_s), (_t))
#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
#define WHC_HOST_SDIO_RX_INT_DISABLE(_p) \
	do { uint32_t _himr = (_p)->sdio_himr & (~SDIO_HIMR_RX_REQUEST_MSK); \
		rtw_write32((_p), SDIO_REG_HIMR, _himr); } while (0)
#define WHC_HOST_SDIO_RX_INT_RESTORE(_p) \
	do { uint32_t _himr = (_p)->sdio_himr; \
		rtw_write32((_p), SDIO_REG_HIMR, _himr); } while (0)
#else
#define WHC_HOST_SDIO_RX_INT_DISABLE(_p)	do {} while (0)
#define WHC_HOST_SDIO_RX_INT_RESTORE(_p)	do {} while (0)
#endif
#define WHC_SEM_GIVE(_s)	rtos_sema_give(_s)
#if defined(WHC_SDIO_INT_MODE) && !defined(WHC_SDIO_INT_GPIO)
#define WHC_HOST_SDIO_HISR_CLEAR(_p) \
	do { uint32_t _v = (_p)->sdio_hisr & MASK_SDIO_HISR_CLEAR; \
		if (_v) { rtw_write32((_p), SDIO_REG_HISR, _v); } } while (0)
#else
#define WHC_HOST_SDIO_HISR_CLEAR(_p)	do {} while (0)
#endif

#define WHC_MUTEX_TAKE(_m, _t)		rtos_mutex_take((_m), (_t))
#define WHC_MUTEX_GIVE(_m)		rtos_mutex_give(_m)
#define WHC_MSLEEP(_ms)			rt_thread_mdelay(_ms)
#define WHC_HOST_SDIO_ALLOC_IRQ(_p)		/* no-op */

#define WHC_HOST_SDIO_RX_DEFAULT(_b) \
	do { whc_host_pkt_rx_to_user((_b) + SIZE_RX_DESC); WHC_FREE(_b); } while (0)

void whc_sdio_host_init_drv(void);

/* same for whc_sdio_host_rx_handler */
void whc_host_sdio_recv_pkts(uint8_t *buf);

uint32_t rtw_sdio_init(struct whc_sdio *priv);

int wifi_on(uint8_t mode);
void whc_host_init(void);

//below for lwip
void lwip_module_init(void);
void lwip_set_ip(uint8_t idx, uint32_t addr, uint32_t netmask_addr, uint32_t gw_addr);
void lwip_netif_set_link_up(uint8_t idx);
void lwip_wlan_set_netif_info(int idx_wlan, void *dev, unsigned char *dev_addr);

#endif


