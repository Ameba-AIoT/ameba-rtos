#ifndef __WHC_HOST_SDIO_DESC_H__
#define __WHC_HOST_SDIO_DESC_H__

#include <stdint.h>
#include "whc_host_sdio_reg.h"

/* Common definitions / descriptors shared by all whc host SDIO ports
 * (stm32 / threadx / zephyr). Port-specific config (block size, interrupt
 * mode, priv struct ...) stays in each port's own header. */

#define PWR_STATE_ACTIVE	0
#define PWR_STATE_SLEEP		1

#define SD_IO_TRY_CNT		(8)
#define TX_PACKET_802_3		(0x83)

/* host <-> dev control event type */
enum WHC_WIFI_CTRL_TYPE {
	WHC_WIFI_EVT_XIMT_PKTS = 0xa5a5a500,
	WHC_WIFI_EVT_RECV_PKTS,

	WHC_WIFI_EVT_MAX,
};

struct whc_msg_info {
	uint32_t	event;
	uint32_t	wlan_idx;
	uint32_t	data_len;
	uint32_t	pad_len;
};

struct INIC_TX_DESC {
	/* u4Byte 0 */
	uint32_t	txpktsize: 16;		// bit[15:0]
	uint32_t	offset: 8;    			// bit[23:16], store the sizeof(struct INIC_TX_DESC)
	uint32_t	bus_agg_num: 8;		// bit[31:24], the bus aggregation number

	/* u4Byte 1 */
	uint32_t type: 8;				// bit[7:0], the packet type
	uint32_t data: 8;				// bit[15:8], the value to be written to the memory
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
	uint32_t	pkt_len: 16;			// bit[15:0], the packet size
	uint32_t	offset: 8;			// bit[23:16], the offset from the packet start to the buf start, also means the size of RX Desc
	uint32_t	rsvd0: 6;				// bit[29:24]
	uint32_t	icv: 1;				//icv error
	uint32_t	crc: 1;				// crc error

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
	uint32_t	type: 8;				// bit[7:0], the type of this packet
	uint32_t	rsvd1: 24;			// bit[31:8]

	/* u4Byte 2 */
	uint32_t	start_addr;

	/* u4Byte 3 */
	uint32_t data_len: 16;			// bit[15:0], the data length of this packet
	uint32_t result: 8;				// bit[23:16], the result of memory write command
	uint32_t rsvd2: 8;				// bit[31:24]
};

#define SIZE_RX_DESC	(sizeof(struct INIC_RX_DESC))
#define SIZE_TX_DESC	(sizeof(struct INIC_TX_DESC))

#ifndef _RND
#define _RND(sz, r) ((((sz)+((r)-1))/(r))*(r))
#define RND4(x)	(((x >> 2) + (((x & 3) == 0) ?  0: 1)) << 2)

static inline uint32_t _RND4(uint32_t sz)
{
	uint32_t	val;

	val = ((sz >> 2) + ((sz & 3) ? 1 : 0)) << 2;

	return val;
}

static inline uint32_t _RND8(uint32_t sz)
{
	uint32_t	val;

	val = ((sz >> 3) + ((sz & 7) ? 1 : 0)) << 3;

	return val;
}

static inline uint32_t _RND128(uint32_t sz)
{
	uint32_t	val;

	val = ((sz >> 7) + ((sz & 127) ? 1 : 0)) << 7;

	return val;
}
#endif

#define CONCAT_TO_UINT32(b4, b3, b2, b1) (((uint32_t)((b4) & 0xFF) << 24) | ((uint32_t)((b3) & 0xFF) << 16) | ((uint32_t)((b2) & 0xFF) << 8) | ((uint32_t)((b1) & 0xFF)))

#endif /* __WHC_HOST_SDIO_DESC_H__ */
