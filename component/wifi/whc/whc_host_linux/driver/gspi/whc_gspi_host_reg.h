
/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Realtek wireless local area network IC driver.
 *
 * Copyright(c) 2026 Realtek Corporation. All rights reserved.
 *
 * GSPI register map, host view. Mirrors component/soc/amebagreen2/fwlib/include/
 * ameba_gspi.h, which is the authoritative source; that header cannot be included
 * here because its AUTO_GEN block needs CMSIS (__IO/__I) and its handle type needs
 * SoC types. Keep the two in sync when the SoC header changes.
 *
 * Two things differ from GSPI and are easy to get wrong:
 *  - the GSPI and GSPI host-domain maps agree only below 0x40. H2C/C2H_MSG, the
 *    AVAL thresholds and RX_AGG all sit elsewhere, so whc_GSPI_host_reg.h must
 *    not be used as a reference.
 *  - the domain IDs are 0x00/0x0C/0x1F, not GSPI's 0/4/7.
 */
#ifndef _WHC_GSPI_REG_H_
#define _WHC_GSPI_REG_H_
#include <linux/bits.h>		/* BIT() */
#include <linux/types.h>	/* u32, used by the field-building macros */

/*********************************************************
*
*Command word (wire format, not a register)
*
*********************************************************/
/* R/W bit31, Fun bit29, domain bits[28:24], address bits[23:8], byte enables
 * bits[3:0]. For the two FIFO domains the low 16 bits carry the transfer length
 * instead of an address. */
#define GSPI_CMD_RW_BIT						(BIT(31))	// 1 = write
#define GSPI_CMD_FUN_BIT					(BIT(29))	// mandatory, undocumented by the UM
#define GSPI_CMD_DOMAIN_SHIFT				24
#define GSPI_CMD_DOMAIN_MSK					0x1F
#define GSPI_CMD_ADDR_SHIFT					8
#define GSPI_CMD_ADDR_MSK					0xFFFF
#define GSPI_CMD_LEN_MSK					0xFFFF		// FIFO domains: length, not address
#define GSPI_CMD_BYTE_EN_MSK				0xF
#define GSPI_CTRL_DOMAIN_ID					0x00		// control registers
#define GSPI_TX_FIFO_DOMAIN_ID				0x0C		// host -> device
#define GSPI_RX_FIFO_DOMAIN_ID				0x1F		// device -> host

/*********************************************************
*
*Frame layout
*
*********************************************************/
/* One frame lives inside a single CS-low window and has three phases:
 *   register read  : CMD 4B -> Status 8B -> Data 4B
 *   everything else: CMD 4B -> Data NB  -> Status 8B
 * The data phase length must be a multiple of 4; an unaligned length still
 * delivers the packet but leaves the Status phase undriven. */
#define GSPI_CMD_PHASE_LEN					4
#define GSPI_STATUS_PHASE_LEN				8
#define GSPI_REG_FRAME_LEN					16		// CMD 4 + Status 8 + Data 4
#define GSPI_TX_DESC_SIZE					16		// = sizeof(INIC_TX_DESC)
#define GSPI_ALIGN4(x)						(((x) + 3) & ~3u)
#define GSPI_FIFO_FRAME_LEN(len) \
	(GSPI_CMD_PHASE_LEN + GSPI_ALIGN4(len) + GSPI_STATUS_PHASE_LEN)

/*********************************************************
*
*GSPI Local registers
*
*********************************************************/
#define GSPI_REG_SPI_INT					0x04	// 4bytes, INT timeout config. NOT readable
#define GSPI_REG_HIMR						0x14	// 4bytes, NOT readable, use the shadow
#define GSPI_REG_HISR						0x18	// 4bytes, write 1 to clear
#define GSPI_REG_RX0_REQ_LEN				0x1C	// 4bytes, RX length + ready flags
#define GSPI_REG_FREE_TXBD_NUM				0x20	// 4bytes, only bits[15:0] are the count
#define GSPI_REG_HCPWM						0x38	// 1byte, byte0 of the 0x38 word, from CCPWM
#define GSPI_REG_HCPWM2						0x3A	// 2bytes, bits[31:16] of the 0x38 word
#define GSPI_REG_AVAI_PGTH_L				0x40	// 4bytes, low threshold for AVAL_INT
#define GSPI_REG_AVAI_PGTH_H				0x44	// 4bytes, high threshold for AVAL_INT
#define GSPI_REG_RX_AGG						0x48	// 4bytes, RX aggregation config
#define GSPI_REG_H2C_MSG					0x4C	// 4bytes, sync to CPU_H2C_MSG
#define GSPI_REG_C2H_MSG					0x50	// 4bytes, sync from CPU_C2H_MSG
#define GSPI_REG_HRPWM						0x80	// 1byte, byte0 of the 0x80 word, sync to CRPWM

// also the word address for 32-bit access
#define GSPI_REG_HRPWM2						0x82	// 2bytes, bits[31:16] of the 0x80 word
#define GSPI_REG_CPU_INDICATION				0x84	// 4bytes, CPU_RDY is bit24 (= bit0 of byte 0x87)
#define GSPI_REG_SPI_CFG					0xF0	// 1byte, mode / endian, first frame writes it

/************************************************
*
*BIT MAP FOR SOME REGISTERS
*
************************************************/
// GSPI_REG_HISR: read as a full 32-bit register, so these match the UM bit numbers
#define GSPI_HISR_RX_REQUEST				(BIT(0))
#define GSPI_HISR_TXBD_AVAL_INT				(BIT(1))
#define GSPI_HISR_TXPKT_SIZE_OVER_BUFF		(BIT(2))
#define GSPI_HISR_TXAGG_SIZE_MISMATCH		(BIT(3))
#define GSPI_HISR_TXBD_OVERFLOW				(BIT(4))
//BIT5~16 not used
#define GSPI_HISR_C2H_MSG_INT				(BIT(17))
#define GSPI_HISR_CPWM1_INT					(BIT(18))
#define GSPI_HISR_CPWM2_INT					(BIT(19))
#define GSPI_HISR_H2C_BUS_FAIL				(BIT(20))	// sets + drives INT, W1C works, reads back 0
//BIT21 not used
#define GSPI_HISR_CPU_NOT_RDY				(BIT(22))	// same: cannot be read
//BIT23~31 not used

#define MASK_GSPI_HISR_CLEAR				(GSPI_HISR_TXPKT_SIZE_OVER_BUFF|\
											GSPI_HISR_TXAGG_SIZE_MISMATCH|\
											GSPI_HISR_TXBD_OVERFLOW|\
											GSPI_HISR_C2H_MSG_INT|\
											GSPI_HISR_CPWM1_INT|\
											GSPI_HISR_CPWM2_INT|\
											GSPI_HISR_H2C_BUS_FAIL|\
											GSPI_HISR_CPU_NOT_RDY)

/* TX faults the Status trailer can actually show. H2C_BUS_FAIL is a real TX
 * failure but is unreadable, so it can only be caught on the INT line -- polling
 * TX errors has a blind spot by construction. */
#define GSPI_MASK_HISR_TX_ERR				(GSPI_HISR_TXPKT_SIZE_OVER_BUFF|\
											 GSPI_HISR_TXAGG_SIZE_MISMATCH|\
											 GSPI_HISR_TXBD_OVERFLOW)

/* Bits 5..16 are unused, so any of them set means the device never drove the
 * Status phase and the bytes read back are just the idle bus level. */
#define GSPI_MASK_HISR_INVALID				(0x0001FFE0)

// GSPI_REG_HIMR: same positions. Write-only in silicon, always reads back 0
#define GSPI_HIMR_RX_REQUEST_MSK			(BIT(0))
#define GSPI_HIMR_TXFIFO_AVAL_MSK			(BIT(1))
#define GSPI_HIMR_TXPKT_SIZE_OVER_BUFF_MSK	(BIT(2))
#define GSPI_HIMR_TXAGG_SIZE_MISMATCH_MSK	(BIT(3))
#define GSPI_HIMR_TXBD_OVERFLOW_MSK			(BIT(4))
#define GSPI_HIMR_C2H_MSG_MSK				(BIT(17))
#define GSPI_HIMR_CPWM1_MSK					(BIT(18))
#define GSPI_HIMR_CPWM2_MSK					(BIT(19))
#define GSPI_HIMR_H2C_BUS_FAIL_MSK			(BIT(20))
#define GSPI_HIMR_CPU_NOT_RDY_MSK			(BIT(22))
#define GSPI_HIMR_DISABLED					0

// Register GSPI_REG_SPI_INT
#define GSPI_EN_HISR_MASK_TIMER				(BIT(0))
#define GSPI_INT_TIMEOUT_SHIFT				16		// unit 32us
#define GSPI_INT_TIMEOUT_MSK				0xFFFF

// Register GSPI_REG_RX0_REQ_LEN
#define GSPI_RX_REQ_LEN_RDY					(BIT(31))
#define GSPI_RX_TXDMA_FIFO_OK				(BIT(30))	// prev TX moved FIFO -> SRAM
#define GSPI_RX_REQ_LEN_MSK					(0xFFFFFF)

// Register GSPI_REG_FREE_TXBD_NUM
#define GSPI_FREE_TXBD_NUM_MSK				(0xFFFF)

// Register GSPI_REG_HCPWM (byte0 of the 0x38 word)
#define GSPI_HCPWM_WLAN_TRX					(BIT(1))
#define GSPI_HCPWM_RPS_ST					(BIT(2))
#define GSPI_HCPWM_WWLAN					(BIT(3))
#define GSPI_HCPWM_TOGGLING					(BIT(7))

// Register GSPI_REG_HRPWM (byte0 of the 0x80 word), payload is bits[6:0]
#define GSPI_HRPWM_WLAN_TRX					(BIT(1))
#define GSPI_HRPWM_RPS_ST					(BIT(2))
#define GSPI_HRPWM_WWLAN					(BIT(3))
#define GSPI_HRPWM_TOGGLING					(BIT(7))
#define GSPI_HRPWM_PAYLOAD_MSK				(0x7F)

/* HRPWM2 lives in bits[31:16] of the same 0x80 word, so the 32-bit view and the
 * halfword view at 0x82 disagree by 16: the toggle is bit31 in one and bit15 in
 * the other. The driver writes 32-bit, hence GSPI_HRPWM2_SET() below.
 * ACT/CG are a software convention -- the UM marks HRPWM2[14:0] "NO USED". */
#define GSPI_HRPWM2_ACT_BIT					(BIT(0))	// payload bit, halfword view
#define GSPI_HRPWM2_CG_BIT					(BIT(1))	// payload bit, halfword view
#define GSPI_HRPWM2_TOGGLE_W32				(BIT(31))	// 32-bit view of the 0x80 word
#define GSPI_HRPWM2_PAYLOAD_SHIFT			16
#define GSPI_HRPWM2_PAYLOAD_MSK				0x7FFF

/* Build the 32-bit value to write at GSPI_REG_HRPWM: toggle + payload. */
#define GSPI_HRPWM2_SET(payload) \
	(GSPI_HRPWM2_TOGGLE_W32 | \
	 (((u32)(payload) & GSPI_HRPWM2_PAYLOAD_MSK) << GSPI_HRPWM2_PAYLOAD_SHIFT))

// Register GSPI_REG_HCPWM2 (halfword view at 0x3A)
#define GSPI_HCPWM2_ACT_BIT					(BIT(0))
#define GSPI_HCPWM2_IMG1_BIT				(BIT(2))	/* bootloader ready */
#define GSPI_HCPWM2_TOGGLING				(BIT(15))

// Register GSPI_REG_CPU_INDICATION, read as 32-bit at 0x84
#define GSPI_CPU_RDY_IND					(BIT(24))	// = bit0 of byte 0x87
#define GSPI_TEST_BUSY_CNT_SHIFT			4
#define GSPI_TEST_BUSY_CNT_MSK				0xF

// Register GSPI_REG_RX_AGG
#define GSPI_RX_AGG_EN						(BIT(15))
#define GSPI_RX_AGG_TIMEOUT_SHIFT			8		// unit 32us
#define GSPI_RX_AGG_TIMEOUT_MSK				0x7F
#define GSPI_RX_AGG_BDCNT_MSK				0xFF

// Register GSPI_REG_H2C_MSG / GSPI_REG_C2H_MSG
#define GSPI_MSG_TOGGLING					(BIT(31))
#define GSPI_MSG_MSK						(0x7FFFFFFF)

// Register GSPI_REG_SPI_CFG
#define GSPI_SPI_LIT_ENDIAN					(BIT(1))	// 0 = big endian
#define GSPI_SPI_32BIT_MODE					(BIT(0))	// 0 = 16-bit word length

/* Values written to SPI_CFG. Named from the device's side: BIG_ENDIAN_32 puts a
 * 32-bit word's LSB on the bus first, which is a little-endian host's memory
 * order and therefore needs no software swap. 0x03 is the device reset value. */
#define GSPI_LITTLE_ENDIAN_32				0x03
#define GSPI_BIG_ENDIAN_32					0x01

/* Activation retry policy: a frame sent while the device is still running its
 * own SPDIO init is simply not seen, so the next attempt is the first
 * transaction it observes. */

#define GSPI_ACTIVATE_RETRY					3
#define GSPI_ACTIVATE_RETRY_MS				50
#endif /* _WHC_GSPI_REG_H_ */

//READ-ONLY for driver, Modify by FW
// Note: should use CMD52 to access SPDIO control register!!! */
#define SPDIO_REG_TXBD_NUM					0x1A4
#define SPDIO_REG_TXBD_WPTR					0x1A8
#define SPDIO_REG_TXBD_RPTR					0x1AC
#define SPDIO_REG_RXBD_NUM					0x1B4 // 2 bytes
#define SPDIO_REG_TXBUF_UNIT_SZ				0x1D9 // 1 bytes
