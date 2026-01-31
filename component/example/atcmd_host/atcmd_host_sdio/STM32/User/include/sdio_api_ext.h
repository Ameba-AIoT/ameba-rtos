#ifndef SDIO_API_EXT_H
#define SDIO_API_EXT_H

#include "sdio_host_common.h"


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

#ifndef FALSE
#define FALSE   0
#endif

#ifndef TRUE
#define TRUE    (!FALSE)
#endif

#ifndef BIT
#define BIT(__n)       (1U<<(__n))
#endif


#define SDIO_HIMR_RX_REQUEST_MSK				(BIT(0))

#define SDIO_HISR_RX_REQUEST					(BIT(0))

#define SDIO_REG_HIMR							0x14 // 4bytes
#define SDIO_REG_HISR							0x18 // 4bytes
#define SDIO_REG_RX0_REQ_LEN					0x1c // 4bytes
#define SDIO_REG_FREE_TXBD_NUM				0x20 // 2bytes
#define SDIO_REG_HCPWM						0x38 // 1byte, host domain, sync from CCPWM
#define SDIO_REG_HCPWM2						0x3a // 2bytes, sync from CCPWM2

#define SDIO_LOCAL_DOMAIN_ID				0
#define SDIO_TX_FIFO_DOMAIN_ID			4
#define SDIO_RX_FIFO_DOMAIN_ID			7

#define SDIO_LOCAL_MSK						0xFFF
#define SDIO_TX_FIFO_MSK					0x1FFF
#define SDIO_RX_FIFO_MSK 					0x3

#define SDIO_BLOCK_SIZE		256
#define USER_BLK_SZ SDIO_64B_BLOCK
#define BASIC_FUNC 0	// func0
#define USER_FUNC 1		// func1: SDIO_WIFI

#define MAX_BLOCKS 511
#define MAX_BYTE_SIZE 511

#define SD_IO_TRY_CNT (8)

#define TX_PACKET_802_3		(0x83)

#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))

#define SIZE_RX_DESC	(sizeof(struct INIC_RX_DESC))
#define SIZE_TX_DESC	(sizeof(struct INIC_TX_DESC))

static inline  u32 _RND4(u32 sz)
{
	u32	val;
	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;
	return val;
}

uint8_t *sdio_host_read_from_dev(uint32_t size);
uint32_t sdio_read_port(uint32_t addr, uint32_t cnt, uint8_t *mem);
int sd_read(uint32_t addr, uint32_t cnt, void *pdata);
rtw_result_t sdio_readb(uint32_t addr, uint8_t *out);
s32 sdio_read_common(uint32_t addr, uint32_t cnt, uint8_t *pbuf);
uint8_t *sdio_read_rxfifo(uint32_t size);

uint32_t sdio_write_port(uint32_t addr, uint32_t cnt, uint8_t *mem);
s32 sdio_local_write(uint32_t addr, uint32_t cnt, uint8_t *pbuf);
int sd_write(uint32_t addr, uint32_t cnt, void *pdata);
void sdio_host_send_to_dev(u8 *buf, u32 len);
void sdio_send_data(uint8_t *buf, uint32_t len, void *pskb);
rtw_result_t sdio_writeb(uint32_t addr, uint8_t value);
rtw_result_t sdio_cmd53_wrap(rtw_sdio_transfer_direction_t direction, uint32_t addr, uint16_t byte_cnt, uint8_t *data);

#endif