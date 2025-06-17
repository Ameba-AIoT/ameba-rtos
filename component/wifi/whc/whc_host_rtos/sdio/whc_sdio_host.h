
#ifndef __INIC_SDIO_HOST_H__
#define __INIC_SDIO_HOST_H__

#include "os_wrapper.h"

//#define tskIDLE_PRIORITY	0
#define DEV_DMA_ALIGN	1
#define SDIO_INT_MODE	1
#define SDIO_BLOCK_SIZE		256
#ifdef CONFIG_AMEBADPLUS
#define CALCULATE_FREE_TXBD 1 // for DP bug, comment after 7005 for tp
#endif

#ifdef CONFIG_AMEBAGREEN2
#define GREEN2_WA
#endif

#define CONFIG_SDIO_TX_ENABLE_AVAL_INT 1

#define CONFIG_WHC_BRIDGE_HOST 1

#define PWR_STATE_ACTIVE	0
#define PWR_STATE_SLEEP		1

#define SD_IO_TRY_CNT (8)
#define TX_PACKET_802_3		(0x83)

#define NET_IF_NUM 2
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

/* auth/assoc/key resnd limit can be configured, refer max >> RTW_JOIN_TIMEOUT
 * including auth + assoc + 4way handshake, no dhcp
 */
#define RTW_JOIN_TIMEOUT (3 * 12000 + 13100 + 20200 + 50) //(MAX_CNT_SCAN_TIMES * SCANNING_TIMEOUT + MAX_JOIN_TIMEOUT + KEY_EXCHANGE_TIMEOUT + 50)

struct whc_sdio_host_priv_t {
	rtos_sema_t host_send; /* sema to protect inic  host send */
	rtos_sema_t host_send_api; /* sema to protect inic ipc host send api */
	rtos_sema_t host_recv_wake; /* for recv task */
	rtos_sema_t host_recv_done; /* for recv task */

	uint8_t *rx_buf;
	struct whc_buf_info *txbuf_info; /* to free in hdl */
};


struct INIC_TX_DESC {
	/* u4Byte 0 */
	uint32_t	txpktsize: 16;		// bit[15:0]
	uint32_t	offset: 8;    			// bit[23:16], store the sizeof(struct INIC_TX_DESC)
	uint32_t	bus_agg_num: 8;		// bit[31:24], the bus aggregation number

	/* u4Byte 1 */
	uint32_t type: 8;				// bit[7:0], the packet type
	uint32_t data: 8;				// bit[8:15], the value to be written to the memory
	uint32_t reply: 1;				// bit[16], request to send a reply message
	uint32_t rsvd0: 15;

	/* u4Byte 2 */
	uint32_t	start_addr;			// 1) memory write/read start address 2) RAM start_function address

	/* u4Byte 3 */
	uint32_t data_len: 16;			// bit[15:0], the length to write/read
	uint32_t rsvd2: 16;			// bit[31:16]
};

/**
  * @brief  INIC RX DESC structure definition
  * @note: Ameba1 is 6 dword, but AmebaZ is 4 dwords
  */
struct INIC_RX_DESC {
	/* u4Byte 0 */
	u32	pkt_len: 16;			// bit[15:0], the packet size
	u32	offset: 8;			// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	u32	rsvd0: 6;				// bit[29:24]
	u32	icv: 1;				//icv error
	u32	crc: 1;				// crc error

	/* u4Byte 1 */
	/************************************************/
	/*****************receive packet type*********************/
	/*	0x82:	802.3 packet						      */
	/*	0x80:	802.11 packet						      */
	/*	0x10:	C2H command					      */
	/*	0x50:	Memory Read						      */
	/*	0x52:	Memory Write						      */
	/*	0x54:	Memory Set						      */
	/*	0x60:	Indicate the firmware is started		      */
	u32	type: 8;				// bit[7:0], the type of this packet
	u32	rsvd1: 24;			// bit[31:8]

	/* u4Byte 2 */
	u32	start_addr;

	/* u4Byte 3 */
	u32 data_len: 16;			// bit[15:0], the type of this packet
	u32 result: 8;				// bit[23:16], the result of memory write command
	u32 rsvd2: 8;				// bit[31:24]
};

struct whc_sdio {
	rtos_mutex_t	lock; /* mutex to protect send host event_priv message */
	rtos_sema_t txbd_wq;
	//struct sdio_func *func;
#ifdef TODO
#endif
	void *func;

	uint32_t 		sdio_himr;
	uint32_t 		sdio_hisr;
	uint16_t		txbd_size;
	uint16_t		rxbd_num;
	uint16_t 		SdioTxBDFreeNum;
	uint32_t 		SdioTxMaxSZ; //The Size of Single Tx buf addressed by TX_BD
	uint8_t			SdioRxFIFOCnt;
	uint8_t			tx_avail_int_triggered;
	uint8_t	tx_block_mode;
	uint8_t	rx_block_mode;
	uint32_t block_transfer_len;

	int32_t bSurpriseRemoved;

	uint8_t dev_state;

	rtos_sema_t host_send; /* sema to protect inic  host send */
	rtos_sema_t host_send_api; /* sema to protect inic ipc host send api */
	rtos_sema_t host_recv_wake; /* for recv task */
	rtos_sema_t host_recv_done; /* for recv task */
	rtos_sema_t host_irq; /* for sdio irq */

	uint8_t *rx_buf;
};

#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)
static inline  u32 _RND4(u32 sz)
{

	u32	val;

	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;

	return val;

}

static inline  u32 _RND8(u32 sz)
{

	u32	val;

	val = ((sz >> 3) + ((sz & 7) ? 1 : 0)) << 3;

	return val;

}

static inline  u32 _RND128(u32 sz)
{

	u32	val;

	val = ((sz >> 7) + ((sz & 127) ? 1 : 0)) << 7;

	return val;

}

#define SDIO_LOCAL_DOMAIN_ID				0
#define SDIO_TX_FIFO_DOMAIN_ID				4
#define SDIO_RX_FIFO_DOMAIN_ID			7
#define SDIO_UNDEFINED_DOMAIN_ID			(-1)

#define SDIO_LOCAL_MSK						0xFFF
#define SDIO_TX_FIFO_MSK					0x1FFF
#define SDIO_RX_FIFO_MSK 					0x3

#define SDIO_LOCAL_OFFSET					0x10250000
#define TX_FIFO_OFFSET						0x10310000
#define RX_FIFO_OFFSET						0x10340000

/*********************************************************
*
*SDIO Local registers
*
*********************************************************/
#define SDIO_REG_TX_CTRL						0x00 // 1byte
#define SDIO_REG_STATIS_RECOVERY_TIMOUT	0x02 // 2bytes
#define SDIO_REG_32K_TRANS_IDLE_TIME		0x04 // 2bytes
#define SDIO_REG_HIMR							0x14 // 4bytes
#define SDIO_REG_HISR							0x18 // 4bytes
#define SDIO_REG_RX0_REQ_LEN					0x1c // 4bytes
#define SDIO_REG_FREE_TXBD_NUM				0x20 // 2bytes
#define SDIO_REG_TX_SEQNUM 					0x24 // 1byte, not used
#define SDIO_REG_HCPWM						0x38 // 1byte, host domain, sync from CCPWM
#define SDIO_REG_HCPWM2						0x3a // 2bytes, sync from CCPWM2
#define SDIO_REG_H2C_MSG						0x60 // 4bytes, host domain, sync from CPU_H2C_MSG
#define SDIO_REG_C2H_MSG						0x64 // 4bytes, sync from CPU_C2H_MSG
#define SDIO_REG_H2C_MSG_EXT					0x68 // 4bytes, sync from CPU_H2C_MSG_EXT
#define SDIO_REG_C2H_MSG_EXT					0x6c // 4bytes, sync from CPU_C2H_MSG_EXT
#define SDIO_REG_HRPWM						0x80 // 1byte, driver to FW, host domain, sync to CRPWM
#define SDIO_REG_HRPWM2						0x82 // 2bytes, driver to FW, host domain, sync to CRPWM2
#define SDIO_REG_CPU_IND						0x87 // 1 bytes, firmware indication to host
#define SDIO_REG_CMD_IN2RSP_TIME			0x88 // 2 bytes, command IN to response OUT time
#define SDIO_REG_ERR_FLAG						0xC0 // 1 byte, ERR FLAG for SDIO DEBUG
#define SDIO_REG_CMD_ERRCNT					0xC1 // 1 byte, the number of CRC error of SDIO command
#define SDIO_REG_DATA_ERRCNT					0xC2 // 1 byte, the number of CRC error of SDIO data block
#define SDIO_REG_CRC_ERR_INDEX				0xC3 // 1 byte, indicate there is crc error on D3-D0
#define SDIO_REG_AVAI_BD_NUM_TH_L			0xD0 // 4 bytes, low threshold for TXBD Number
#define SDIO_REG_AVAI_BD_NUM_TH_H			0xD4 // 4 bytes, high threshold for TXBD Number
#define SDIO_REG_RX_AGG_CFG					0xD8 // 2 bytes, SDIO bus Rx Aggregation config register

//READ-ONLY for driver, Modify by FW
#define SPDIO_REG_TXBD_NUM					0x1A4
#define SPDIO_REG_TXBD_WPTR					0x1A8
#define SPDIO_REG_TXBD_RPTR					0x1AC
#define SPDIO_REG_RXBD_NUM					0x1B4 // 2 bytes
#define SPDIO_REG_TXBUF_UNIT_SZ				0x1D9 // 1 bytes


/************************************************
*
*BIT MAP FOR SOME REGISTERS
*
************************************************/
// SDIO_REG_TX_CTRL
#define SDIO_EN_HISR_MASK_TIMER					(BIT(1))

// SDIO_REG_HISR: SDIO Host Interrupt Service Routine
#define SDIO_HISR_RX_REQUEST					(BIT(0))
#define SDIO_HISR_AVAL_INT					(BIT(1))
#define SDIO_HISR_TXPKT_OVER_BUFF			(BIT(2))
#define SDIO_HISR_TX_AGG_SIZE_MISMATCH		(BIT(3))
#define SDIO_HISR_TXBD_OVF					(BIT(4))
//BIT5~16 not used
#define SDIO_HISR_C2H_MSG_INT				(BIT(17))
#define SDIO_HISR_CPWM1						(BIT(18))
#define SDIO_HISR_CPWM2						(BIT(19))
#define SDIO_HISR_H2C_BUS_FAIL				(BIT(20))
//BIT21 not used
#define SDIO_HISR_CPU_NOT_RDY				(BIT(22))
//BIT23~31 not used

#define MASK_SDIO_HISR_CLEAR				(SDIO_HISR_TXPKT_OVER_BUFF|\
											SDIO_HISR_TX_AGG_SIZE_MISMATCH|\
											SDIO_HISR_TXBD_OVF|\
											SDIO_HISR_C2H_MSG_INT|\
											SDIO_HISR_CPWM1|\
											SDIO_HISR_CPWM2|\
											SDIO_HISR_H2C_BUS_FAIL|\
											SDIO_HISR_CPU_NOT_RDY)

// RTL8195A SDIO Host Interrupt Mask Register
#define SDIO_HIMR_RX_REQUEST_MSK				(BIT(0))
#define SDIO_HIMR_AVAL_MSK						(BIT(1))
#define SDIO_HIMR_TXPKT_SIZE_OVER_BUFF_MSK		(BIT(2))
#define SDIO_HIMR_AGG_SIZE_MISMATCH_MSK		(BIT(3))
#define SDIO_HIMR_TXBD_OVF_MSK					(BIT(4))
//BIT5~16 not used
#define SDIO_HIMR_C2H_MSG_MSK					(BIT(17))
#define SDIO_HIMR_CPWM1_MSK						(BIT(18))
#define SDIO_HIMR_CPWM2_MSK						(BIT(19))
#define SDIO_HIMR_H2C_BUS_FAIL_MSK				(BIT(20))
//BIT21 not used
#define SDIO_HIMR_CPU_NOT_RDY_MSK				(BIT(22))
//BIT23~31 not used

#define SDIO_HIMR_DISABLED			0

// CCPWM2 bit map definition for Firmware download
#define SDIO_INIT_DONE					(BIT(0))
#define SDIO_MEM_WR_DONE				(BIT(1))
#define SDIO_MEM_RD_DONE				(BIT(2))
#define SDIO_MEM_ST_DONE				(BIT(3))
#define SDIO_CPWM2_TOGGLE			(BIT(15))

// Register REG_SPDIO_CPU_IND
#define SDIO_SYSTEM_TRX_RDY_IND		(BIT(0))

//Register SDIO_REG_RX_AGG_CFG
#define SDIO_RX_AGG_EN				(BIT(15))

// Register SDIO_REG_HCPWM
#define SDIO_HCPWM_WLAN_TRX			(BIT(1))
#define SDIO_HCPWM_RPS_ST			(BIT(2))
#define SDIO_HCPWM_WWLAN			(BIT(3))
#define SDIO_HCPWM_TOGGLE			(BIT(7))

// Register RPWM2
//#define RPWM2_ACT_BIT			(0x00000001 << 0)	// Active
//#define RPWM2_CG_BIT			(0x00000001 << 1)	// Clock Gated
//#define RPWM2_TOGGLE_BIT		(0x00000001 << 15)	// Toggle bit

#define CONCAT_TO_UINT32(b4, b3, b2, b1) (((u32)((b4) & 0xFF) << 24) | ((u32)((b3) & 0xFF) << 16) | ((u32)((b2) & 0xFF) << 8) | ((u32)((b1) & 0xFF)))


void whc_sdio_host_init_drv(void);
void rtw_sdio_send_data(uint8_t *buf, uint32_t len, void *pskb);
void whc_sdio_host_rx_handler(uint8_t *buf);

int sdio_recv_process(uint8_t *pbuf);
void rtw_sdio_recv_data_process(void);

void whc_host_sdio_send_api_ret_value(uint32_t api_id, uint8_t *pbuf, uint32_t len);
void whc_host_api_message_send(uint32_t id, uint8_t *param, uint32_t param_len, uint8_t *ret, uint32_t ret_len);

uint8_t rtw_sdio_query_txbd_status(struct whc_sdio *priv);
uint32_t rtw_sdio_init(struct whc_sdio *priv);

void whc_host_api_task(void);
s32 wifi_on(uint8_t mode);
void whc_sdio_host_init(void);
int wifi_sdio_bridge_get_ip(uint8_t *ip);

//#define whc_host_send_data       whc_sdio_host_send_data
#define whc_host_init            whc_sdio_host_init
#define SIZE_RX_DESC	(sizeof(struct INIC_RX_DESC))
#define SIZE_TX_DESC	(sizeof(struct INIC_TX_DESC))

#endif


