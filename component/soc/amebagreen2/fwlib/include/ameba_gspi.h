/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_GSPI_H_
#define _AMEBA_GSPI_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup GSPI GSPI
  * @brief GSPI host driver: SPI master side of Realtek's SDIO/SPI combo device
  *        interface. The peer device runs its SDIO device (SPDIO) IP in SPI
  *        Slave mode; this driver speaks the GSPI frame protocol over SPI0/SPI1.
  * @{
  */

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup GSPI_Register_Definitions GSPI device register map (host view)
  * @{
  */
#define GSPI_REG_TX_CTRL          0x0000
#define GSPI_REG_SPI_INT          0x0004   /*!< Not readable on RTL8721F, write only. */
#define GSPI_REG_HIMR             0x0014   /*!< Not readable on RTL8721F, use the shadow. */
#define GSPI_REG_HISR             0x0018   /*!< Write 1 to clear. */
#define GSPI_REG_RX0_REQ_LEN      0x001C   /*!< bit31 RDY, bits[23:0] byte count. */
#define GSPI_REG_FREE_TX_BD_NUM   0x0020
#define GSPI_REG_TX_SEQNUM        0x0024
#define GSPI_REG_HCPWM            0x0038   /*!< 8bit, synced from device CCPWM. */
#define GSPI_REG_HCPWM2           0x003A   /*!< 16bit, synced from device CCPWM2. */
#define GSPI_REG_AVAI_PGTH_L      0x0040
#define GSPI_REG_AVAI_PGTH_H      0x0044
#define GSPI_REG_RX_AGG           0x0048
#define GSPI_REG_H2C_MSG          0x004C
#define GSPI_REG_C2H_MSG          0x0050
#define GSPI_REG_HRPWM            0x0080   /*!< 8bit, bit7 TOGGLING -> device RPWM1_INT. */
#define GSPI_REG_HRPWM2           0x0082   /*!< 16bit, shares the 0x80 word with HRPWM. */
#define GSPI_REG_HPS_CLKR         0x0084   /*!< Documented "not used", usable as scratch. */
#define GSPI_REG_CPU_IND          0x0087   /*!< bit0 = device CPU ready for TRX. */
#define GSPI_REG_32K_TRANS        0x0088
#define GSPI_REG_DELY_LINE_SEL    0x008C
#define GSPI_REG_SPI_CFG          0x00F0
/** @} */

/** @defgroup GSPI_HISR_HIMR_Bits HISR / HIMR bits
  * @{
  */
#define GSPI_BIT_RX_REQUEST       ((u32)0x00000001 << 0)   /*!< Device has data to read. */
#define GSPI_BIT_AVAL_INT         ((u32)0x00000001 << 1)   /*!< TX BDs available again. */
#define GSPI_BIT_TXPKT_OVER       ((u32)0x00000001 << 2)
#define GSPI_BIT_TXAGG_MISMATCH   ((u32)0x00000001 << 3)
#define GSPI_BIT_TXBD_OVF         ((u32)0x00000001 << 4)
#define GSPI_BIT_C2H_MSG_INT      ((u32)0x00000001 << 17)
#define GSPI_BIT_CPWM1_INT        ((u32)0x00000001 << 18)
#define GSPI_BIT_CPWM2_INT        ((u32)0x00000001 << 19)
#define GSPI_MASK_TX_ERR          (GSPI_BIT_TXPKT_OVER | GSPI_BIT_TXAGG_MISMATCH | GSPI_BIT_TXBD_OVF)
/*!< Bits 5..16 are unused, so any of them set means the device never drove the
 *   Status phase and the bytes read back are just the idle bus level. */
#define GSPI_MASK_HISR_INVALID    ((u32)0x0001FFE0)
/** @} */

/** @defgroup GSPI_Reg_Bits Other register bits
  * @{
  */
#define GSPI_BIT_RX0_RDY          ((u32)0x00000001 << 31)  /*!< RX0_REQ_LEN ready. */
#define GSPI_MASK_RX0_LEN         ((u32)0x00FFFFFF)
#define GSPI_BIT_CPU_RDY          ((u32)0x00000001 << 0)   /*!< CPU_IND bit0. */
#define GSPI_BIT_HRPWM_TOGGLING   ((u32)0x00000001 << 7)
#define GSPI_BIT_HRPWM2_ACT       ((u32)0x00000001 << 0)
#define GSPI_BIT_HRPWM2_CG        ((u32)0x00000001 << 1)
#define GSPI_BIT_HRPWM2_TOGGLE    ((u32)0x00000001 << 15)
#define GSPI_BIT_HCPWM2_ACT       ((u32)0x00000001 << 0)
/** @} */

/* Exported constants --------------------------------------------------------*/

/** @defgroup GSPI_Exported_Constants GSPI Exported Constants
  * @{
  */

/** @defgroup GSPI_Frame_Sizes Frame element sizes
  * @{
  */
#define GSPI_CMD_LEN            4    /*!< Command phase, always 4 bytes. */
#define GSPI_STATUS_LEN         8    /*!< Status phase, always 8 bytes. */
#define GSPI_TX_DESC_SIZE       16   /*!< = sizeof(INIC_TX_DESC). */
/*!< Bytes a caller buffer must provide for a FIFO transfer of len data bytes.
 *   Layout: [CMD 4B][data, 4-byte aligned][Status 8B]. */
#define GSPI_FIFO_BUF_SIZE(len) (GSPI_CMD_LEN + (((len) + 3) & ~3u) + GSPI_STATUS_LEN)
/** @} */

/** @defgroup GSPI_SPI_CFG SPI_CFG values
  * @brief bit1 = endian (0 big, 1 little), bit0 = word length (0 16-bit, 1 32-bit).
  *        The naming is from the device's point of view: with BIG_ENDIAN_32 the
  *        bus carries a 32-bit word LSB first, which is the natural memory order
  *        of a little-endian host and therefore needs no software swap.
  * @{
  */
#define GSPI_LITTLE_ENDIAN_32   0x03   /*!< Device reset value; host must swap. */
#define GSPI_BIG_ENDIAN_32      0x01   /*!< No host swap needed. */
/** @} */

/** @defgroup GSPI_Return_Codes Return codes
  * @{
  */
#define GSPI_OK                 0
#define GSPI_NO_TXBD            1    /*!< Device back-pressure, retry later. */
#define GSPI_ERR_PARAM         (-1)
#define GSPI_ERR_TIMEOUT       (-2)
#define GSPI_ERR_STATUS        (-3)   /*!< Device reported a TX error in Status. */
#define GSPI_ERR_STATE         (-4)
/** @} */

/** @} */

/* Exported types --------------------------------------------------------*/

/** @defgroup GSPI_Exported_Types GSPI Exported Types
  * @{
  */

/** @brief Status phase of one GSPI frame. Every operation returns it, because
  *        every frame carries it on MISO. */
typedef struct {
	u32 word0;   /*!< First 32 bits. Treated as the raw HISR by the reference driver. */
	u32 word1;   /*!< Second 32 bits. */
	u8  valid;   /*!< 0 = trailer not driven by the device, word0/word1 are bus idle level. */
} GSPI_StatusTypeDef;

/** @brief GSPI host configuration, filled by the user before GSPI_Init(). */
typedef struct {
	u8  GSPI_Index;         /*!< 0: SPI0, 1: SPI1. */
	u32 GSPI_ClkPin;        /*!< SCLK pad, e.g. _PA_18. */
	u32 GSPI_MosiPin;
	u32 GSPI_MisoPin;
	u32 GSPI_CsPin;         /*!< Driven as GPIO: one frame must hold CS low. */
	u32 GSPI_ClkFreq;       /*!< Bus clock in Hz used after activation, max 25MHz. */
	u32 GSPI_ClkFreqInit;   /*!< Bus clock in Hz used for the activation frame. */
	u8  GSPI_DmaEn;         /*!< 1: use GDMA for data phases >= GSPI_DmaThreshold. */
	u16 GSPI_DmaThreshold;  /*!< Data-phase bytes from which GDMA is used. */
	u32 GSPI_XferTimeoutUs; /*!< Upper bound for one DMA data phase / bus idle wait. */
} GSPI_InitTypeDef;

/** @brief GSPI host handle. Allocated by the user, opaque contents. */
typedef struct {
	GSPI_InitTypeDef Init;
	SPI_TypeDef *spi_dev;
	GDMA_InitTypeDef txgdma;
	GDMA_InitTypeDef rxgdma;
	volatile u8 tx_dma_done;
	volatile u8 rx_dma_done;
	u32 himr_shadow;        /*!< HIMR cannot be read back, so it is shadowed here. */
	u8  swap;               /*!< 1 = 4-byte swap needed (device in little-endian mode). */
	u8  activated;
	u8  initialized;
	u8  regbuf[16] __attribute__((aligned(32)));   /*!< Register-frame scratch. */
} GSPI_HandleTypeDef;

/** @} */

/* Exported functions --------------------------------------------------------*/

/** @defgroup GSPI_Exported_Functions GSPI Exported Functions
  * @{
  */

void GSPI_StructInit(GSPI_InitTypeDef *init);
s32  GSPI_Init(GSPI_HandleTypeDef *hgspi, GSPI_InitTypeDef *init);
void GSPI_DeInit(GSPI_HandleTypeDef *hgspi);
s32  GSPI_Configuration(GSPI_HandleTypeDef *hgspi, u8 spi_cfg);
void GSPI_SetFreq(GSPI_HandleTypeDef *hgspi, u32 freq_hz);

u8   GSPI_ReadReg8(GSPI_HandleTypeDef *hgspi, u32 addr, GSPI_StatusTypeDef *sts);
u16  GSPI_ReadReg16(GSPI_HandleTypeDef *hgspi, u32 addr, GSPI_StatusTypeDef *sts);
u32  GSPI_ReadReg32(GSPI_HandleTypeDef *hgspi, u32 addr, GSPI_StatusTypeDef *sts);
s32  GSPI_WriteReg8(GSPI_HandleTypeDef *hgspi, u32 addr, u8 val, GSPI_StatusTypeDef *sts);
s32  GSPI_WriteReg16(GSPI_HandleTypeDef *hgspi, u32 addr, u16 val, GSPI_StatusTypeDef *sts);
s32  GSPI_WriteReg32(GSPI_HandleTypeDef *hgspi, u32 addr, u32 val, GSPI_StatusTypeDef *sts);

s32  GSPI_WriteTxFifo(GSPI_HandleTypeDef *hgspi, u8 *buf, u32 len, GSPI_StatusTypeDef *sts);
s32  GSPI_ReadRxFifo(GSPI_HandleTypeDef *hgspi, u8 *buf, u32 len, GSPI_StatusTypeDef *sts);

u32  GSPI_GetRxLen(GSPI_HandleTypeDef *hgspi, GSPI_StatusTypeDef *sts);
u32  GSPI_GetFreeTxBD(GSPI_HandleTypeDef *hgspi, GSPI_StatusTypeDef *sts);

void GSPI_INTConfig(GSPI_HandleTypeDef *hgspi, u32 mask, u32 newstate);
u32  GSPI_GetINTMask(GSPI_HandleTypeDef *hgspi);
u32  GSPI_INTStatus(GSPI_HandleTypeDef *hgspi, GSPI_StatusTypeDef *sts);
void GSPI_INTClear(GSPI_HandleTypeDef *hgspi, u32 bits);

void GSPI_DmaCmd(GSPI_HandleTypeDef *hgspi, u32 newstate);
u8   GSPI_DmaEnabled(GSPI_HandleTypeDef *hgspi);

/** @} */
/** @} */
/** @} */

#endif /* _AMEBA_GSPI_H_ */
