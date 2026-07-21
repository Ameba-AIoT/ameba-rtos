
#ifndef __WHC_SDIO_HOST_H__
#define __WHC_SDIO_HOST_H__

#include "os_wrapper.h"
#include "../../whc_common/host_sdio/whc_host_sdio_desc.h"
#include "../../whc_common/host_sdio/whc_host_sdio_trx_common.h"

//#define tskIDLE_PRIORITY	0
#define DEV_DMA_ALIGN	1
#define WHC_SDIO_INT_MODE	1
#define WHC_SDIO_INT_GPIO   1
#define SDIO_BLOCK_SIZE		256
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
#define WIFI_STACK_SIZE_RX_REQ_TASK (4096)
#define SDIO_POLLING_STACK_SIZE 1024

struct whc_sdio {
	rtos_mutex_t lock; /* mutex to protect send host message */
	rtos_sema_t txbd_wq;
	rtos_sema_t host_send; /* sema to protect whc host send */
	rtos_sema_t host_send_block_sema; /* sema to block */
	rtos_sema_t host_recv_wake; /* for recv task */
	rtos_sema_t host_recv_done; /* for recv task */
	rtos_sema_t host_irq; /* for sdio irq */
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
	uint8_t tx_avail_int_triggered;
	uint8_t tx_block_mode;
	uint8_t rx_block_mode;
	uint8_t dev_state;
	uint8_t whc_host_init_done;

	/* one fixed tx buf for fullhan, protected by host_send mutex */
	__attribute__((aligned(4))) uint8_t tx_buf[SIZE_TX_DESC + sizeof(struct whc_msg_info) + MAX_SKB_BUF_SIZE_NORMAL];
};

/* ---- port glue for the shared whc_host_sdio_trx_common.c ---- */
#define whc_malloc(_sz)		rt_malloc(_sz)
#define whc_free(_p)		rt_free(_p)
#define whc_sem_take_timeout(_s, _t)	rtos_sema_take((_s), (_t))


#define whc_sem_give(_s)	rtos_sema_give(_s)

#define whc_mutex_take(_m, _t)		rtos_mutex_take((_m), (_t))
#define whc_mutex_give(_m)		rtos_mutex_give(_m)
#define whc_msleep(_ms)			rt_thread_mdelay(_ms)
#define whc_mutex_init(_m)		rtos_mutex_create(&(_m))
#define whc_host_sdio_alloc_irq(_p)	/* no-op */

#define whc_host_sdio_rx_default(_b) \
	do { whc_host_pkt_rx_to_user((_b) + SIZE_RX_DESC); whc_free(_b); } while (0)

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


