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

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup GSPI_Register_Definitions GSPI Register Definitions
  * @{
  */

/** @defgroup GSPI_SPI_INT
  * @brief
  * @{
  */
#define GSPI_MASK_R_INT_TIMEROUT   ((u32)0x0000FFFF << 16)           /*!< R/W 0xc00  Timeout to release hardware auto mask HISR[1:0](for int line only) HISR[0]: release after rxdata has been transferred to host HISR[1]: release after host read free_txbd_num The unit is 32usec. The default value is 0xc00 means that timeout is 98304 usec ( 32*0xc00)*/
#define GSPI_R_INT_TIMEROUT(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define GSPI_GET_R_INT_TIMEROUT(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define GSPI_BIT_R_EN_MASK_TIMER   ((u32)0x00000001 << 0)            /*!< R/W 0x0  hardware auto mask HISR[1:0] after host driver read HISR[1:0]*/
/** @} */

/** @defgroup GSPI_HIMR
  * @brief
  * @{
  */
#define GSPI_BIT_CPU_NOT_RDY_MSK          ((u32)0x00000001 << 22) /*!< R/W 0x0  This bit is clear, and CPU_NOT_RDY is masked.(for int line only)*/
#define GSPI_BIT_H2C_BUS_FAIL_MSK         ((u32)0x00000001 << 20) /*!< R/W 0x0  This bit is clear, and H2C_BUS_FAIL_INT is masked.(for int line only)*/
#define GSPI_BIT_CPWM2_INT_MSK            ((u32)0x00000001 << 19) /*!< R/W 0x0  This bit is clear, and CPWM2_INT is masked.(for int line only)*/
#define GSPI_BIT_CPWM1_INT_MSK            ((u32)0x00000001 << 18) /*!< R/W 0x0  This bit is clear, and CPWM1_INT is masked.(for int line only)*/
#define GSPI_BIT_C2H_MSG_INT_MSK          ((u32)0x00000001 << 17) /*!< R/W 0x0  This bit is clear, and C2H_MSG_INT is masked.(for int line only)*/
#define GSPI_BIT_TXBD_OVERFLOW_MSK        ((u32)0x00000001 << 4)  /*!< R/W 0x0  This bit is clear, and TXBD_OVERFLOW is masked.(for int line only)*/
#define GSPI_BIT_TXAGG_SIZE_MISMATCH_MSK  ((u32)0x00000001 << 3)  /*!< R/W 0x1  This bit is clear, and TXAGG_SIZE_MISMATCH is masked.(for int line only)*/
#define GSPI_BIT_TXPKT_SIZE_OVER_BUFF_MSK ((u32)0x00000001 << 2)  /*!< R/W 0x1  This bit is clear, and TXPKT_SIZE_OVER_BUFF is masked.(for int line only)*/
#define GSPI_BIT_TXFIFO_AVAL_INT_MSK      ((u32)0x00000001 << 1)  /*!< R/W 0x1  This bit is clear, and TXFIFO_AVAL_INT is masked.(for int line only)*/
#define GSPI_BIT_RX_REQUEST_MSK           ((u32)0x00000001 << 0)  /*!< R/W 0x1  This bit is clear, and RX_REQUEST_INT is masked.(for int line only)*/
/** @} */

/** @defgroup GSPI_HISR
  * @brief
  * @{
  */
#define GSPI_BIT_CPU_NOT_RDY          ((u32)0x00000001 << 22) /*!< RW1C 0x0  When CPU_RDY_IND is cleared(write 0), trigger this interrupt,will report in int line,but can't be read*/
#define GSPI_BIT_H2C_BUS_FAIL         ((u32)0x00000001 << 20) /*!< RW1C 0x0  H2C BUS Fail Interrupt:To notify Host DMA transfer error by AHB BUS resource conflict. Host get this error INT, and know the previous packet fail, and need to re-send or IO_RESET device,will report in int line,but can't be read*/
#define GSPI_BIT_CPWM2_INT            ((u32)0x00000001 << 19) /*!< RW1C 0x0  CPWM2 Write Interrupt: This interrupt will be raised when FW writes CPWM2_TOGGLING of CPWM2 register.  Write 1 to clear*/
#define GSPI_BIT_CPWM1_INT            ((u32)0x00000001 << 18) /*!< RW1C 0x0  CPWM1 Write Interrupt: This interrupt will be raised when FW writes CPWM1_TOGGLING of CPWM1 register. Write 1 to clear*/
#define GSPI_BIT_C2H_MSG_INT          ((u32)0x00000001 << 17) /*!< RW1C 0x0  CPU to Host Message INT Status: This interrupt will be raised when FW sends a C2H message. Write 1 to clear*/
#define GSPI_BIT_TXBD_OVERFLOW        ((u32)0x00000001 << 4)  /*!< RW1C 0x0  When host send TX packet and there is no enough TXBD, trigger this interrupt*/
#define GSPI_BIT_TXAGG_SIZE_MISMATCH  ((u32)0x00000001 << 3)  /*!< RW1C 0x0  If the total size of the Tx aggregation packets is LARGER the CMD53 Data Size, this interrupt trigger.*/
#define GSPI_BIT_TXPKT_SIZE_OVER_BUFF ((u32)0x00000001 << 2)  /*!< RW1C 0x0  If the Packet size (Aggregated or not) is larger than the Buffersize assigned in the TX_BD, this interrupt trigger*/
#define GSPI_BIT_TXFIFO_AVAL_INT      ((u32)0x00000001 << 1)  /*!< RW1C 0x0  When the free page is lower than SDIO_AVAI_BD_NUM_TH_L and higher than SDIO_AVAI_BD_NUM_TH_H consequently, this interrupt occurs. When Host read FREE_TXBD_NUM, the bit becomes zero. When using CMD52 to read FREE_TXBD_NUM, we suggest to read offset 0x23->0x22->0x21->0x20 consequently*/
#define GSPI_BIT_RX_REQUEST           ((u32)0x00000001 << 0)  /*!< R 0x0  This bit is set when one complete Rx aggregation packet is received into RXPKTBUF and RX_REQUEST_MSK is set to 1. Only when Rx FIFO is empty, the bit becomes zero.*/
/** @} */

/** @defgroup GSPI_RXPKT_INFO
  * @brief
  * @{
  */
#define GSPI_BIT_SYNC_RX_LEN_READY        ((u32)0x00000001 << 31)          /*!< R 0x0  Indicate RX0_REQ_LEN is ready*/
#define GSPI_BIT_SYNC_SPDIO_TXDMA_FIFO_OK ((u32)0x00000001 << 30)          /*!< R 0x0  indicate txdma fifo is ready to receive data, will be set to 0 when txdma transfer starts*/
#define GSPI_MASK_RX_LENGTH_LAT           ((u32)0x00FFFFFF << 0)           /*!< R 0x0  The requested RX Packet length; Unit: Byte For the RX Aggregation case, the length is total length of the packet. Each packet is  4byte alignment. This value is legal only when RX_REQ_LEN_RDY is 1*/
#define GSPI_RX_LENGTH_LAT(x)             (((u32)((x) & 0x00FFFFFF) << 0))
#define GSPI_GET_RX_LENGTH_LAT(x)         ((u32)(((x >> 0) & 0x00FFFFFF)))
/** @} */

/** @defgroup GSPI_TXPKT_INFO_1
  * @brief
  * @{
  */
#define GSPI_MASK_HGQ_FREEPG   ((u32)0x0000FFFF << 0)           /*!< R 0x0  number of free txbd entry*/
#define GSPI_HGQ_FREEPG(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GSPI_GET_HGQ_FREEPG(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GSPI_HCPWM
  * @brief
  * @{
  */
#define GSPI_MASK_R_HCPWM2   ((u32)0x0000FFFF << 16)           /*!< R 0x0  ccpwm message extension(now is async read) [31]:  CPWM2_TRIGGERING, FW write this bit(auto clear 0) to trigger cpwm2_int*/
#define GSPI_R_HCPWM2(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define GSPI_GET_R_HCPWM2(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define GSPI_MASK_R_HCPWM    ((u32)0x000000FF << 0)            /*!< R 0x0  ccpwm message(now is async read) [7]:  CPWM1_TRIGGERING, FW write this bit(auto clear 0) to trigger cpwm1_int*/
#define GSPI_R_HCPWM(x)      (((u32)((x) & 0x000000FF) << 0))
#define GSPI_GET_R_HCPWM(x)  ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GSPI_SPI_AVAI_PATH_L
  * @brief
  * @{
  */
#define GSPI_MASK_SPI_AVAI_PGTH_L   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  low threshold for TXBD Number*/
#define GSPI_SPI_AVAI_PGTH_L(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GSPI_GET_SPI_AVAI_PGTH_L(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GSPI_SPI_AVAI_PATH_H
  * @brief
  * @{
  */
#define GSPI_MASK_SPI_AVAI_PGTH_H   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  High threshold for TXBD Number*/
#define GSPI_SPI_AVAI_PGTH_H(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define GSPI_GET_SPI_AVAI_PGTH_H(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup GSPI_RXBD_AGG_CTRL
  * @brief
  * @{
  */
#define GSPI_BIT_SPI_RX_AGG_EN       ((u32)0x00000001 << 15)          /*!< R/W 0x0  Enable SDIO Bus RX Aggregation*/
#define GSPI_MASK_SPI_RX_AGG_TO      ((u32)0x0000007F << 8)           /*!< R/W 0x0  Timeout threshold for RX Agg; Unit: 32us*/
#define GSPI_SPI_RX_AGG_TO(x)        (((u32)((x) & 0x0000007F) << 8))
#define GSPI_GET_SPI_RX_AGG_TO(x)    ((u32)(((x >> 8) & 0x0000007F)))
#define GSPI_MASK_SPI_RX_AGG_BDCNT   ((u32)0x000000FF << 0)           /*!< R/W 0x0  RX_BD count threshold for RX Agg*/
#define GSPI_SPI_RX_AGG_BDCNT(x)     (((u32)((x) & 0x000000FF) << 0))
#define GSPI_GET_SPI_RX_AGG_BDCNT(x) ((u32)(((x >> 0) & 0x000000FF)))
/** @} */

/** @defgroup GSPI_H2C_MSG
  * @brief
  * @{
  */
#define GSPI_BIT_DRV2FW_INFO_SPI_31 ((u32)0x00000001 << 31)          /*!< R/WA0 0x0  spi h2c message [31]:  H2C_MSG_TRIGGERING, FW write this bit(auto clear 0) to trigger h2c_msg_int*/
#define GSPI_MASK_DRV2FW_INFO_SPI   ((u32)0x7FFFFFFF << 0)           /*!< R/W 0x0  spi h2c message [31]:  H2C_MSG_TRIGGERING, FW write this bit(auto clear 0) to trigger h2c_msg_int*/
#define GSPI_DRV2FW_INFO_SPI(x)     (((u32)((x) & 0x7FFFFFFF) << 0))
#define GSPI_GET_DRV2FW_INFO_SPI(x) ((u32)(((x >> 0) & 0x7FFFFFFF)))
/** @} */

/** @defgroup GSPI_C2H_MSG
  * @brief
  * @{
  */
#define GSPI_MASK_SPDIO_C2H_MSG   ((u32)0xFFFFFFFF << 0)           /*!< R 0x0  spi c2h message(now is async read) [31]:  C2H_MSG_TRIGGERING, FW write this bit(auto clear 0) to trigger c2h_msg_int*/
#define GSPI_SPDIO_C2H_MSG(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define GSPI_GET_SPDIO_C2H_MSG(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup GSPI_HRPWM
  * @brief
  * @{
  */
#define GSPI_BIT_R_HRPWM2_15 ((u32)0x00000001 << 31)           /*!< R/WA0 0x0  trigger hrpwm message interrupt to FW*/
#define GSPI_MASK_R_HRPWM2   ((u32)0x00007FFF << 16)           /*!< R/W 0x0  hrpwm message extension*/
#define GSPI_R_HRPWM2(x)     (((u32)((x) & 0x00007FFF) << 16))
#define GSPI_GET_R_HRPWM2(x) ((u32)(((x >> 16) & 0x00007FFF)))
#define GSPI_BIT_R_HRPWM_7   ((u32)0x00000001 << 7)            /*!< R/WA0 0x0  trigger hrpwm message interrupt to FW*/
#define GSPI_MASK_R_HRPWM    ((u32)0x0000007F << 0)            /*!< R/W 0x0  hrpwm message*/
#define GSPI_R_HRPWM(x)      (((u32)((x) & 0x0000007F) << 0))
#define GSPI_GET_R_HRPWM(x)  ((u32)(((x >> 0) & 0x0000007F)))
/** @} */

/** @defgroup GSPI_CPU_INDICATION
  * @brief
  * @{
  */
#define GSPI_BIT_SYNC_CPU_RDY_IND ((u32)0x00000001 << 24) /*!< R 0x0  indicate CPU is ready*/
/** @} */

/** @defgroup GSPI_SPI_CFG
  * @brief
  * @{
  */
#define GSPI_BIT_R_LIT_ENDIAN ((u32)0x00000001 << 1) /*!< R/W 0x1  0:big endian 1: little endian (default)*/
#define GSPI_BIT_R_32BIT_MODE ((u32)0x00000001 << 0) /*!< R/W 0x1  0:16 bits word length 1:32 bits word length (default)*/
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup GSPI_Exported_Types GSPI Exported Types
  * @{
  */

/** @brief GSPI Register Declaration
  */

typedef struct {
	__IO uint32_t RSVD0               ;  /*!< Reserved,  Address offset:0x000-0x003 */
	__IO uint32_t GSPI_SPI_INT        ;  /*!< Register,  Address offset:0x004 */
	__IO uint32_t RSVD1[3]            ;  /*!< Reserved,  Address offset:0x008-0x013 */
	__IO uint32_t GSPI_HIMR           ;  /*!< Register,  Address offset:0x014 */
	__IO uint32_t GSPI_HISR           ;  /*!< Register,  Address offset:0x018 */
	__I  uint32_t GSPI_RXPKT_INFO     ;  /*!< Register,  Address offset:0x01C */
	__I  uint32_t GSPI_TXPKT_INFO_1   ;  /*!< Register,  Address offset:0x020 */
	__IO uint32_t RSVD2[5]            ;  /*!< Reserved,  Address offset:0x024-0x037 */
	__I  uint32_t GSPI_HCPWM          ;  /*!< Register,  Address offset:0x038 */
	__IO uint32_t RSVD3               ;  /*!< Reserved,  Address offset:0x03C-0x03F */
	__IO uint32_t GSPI_SPI_AVAI_PATH_L;  /*!< Register,  Address offset:0x040 */
	__IO uint32_t GSPI_SPI_AVAI_PATH_H;  /*!< Register,  Address offset:0x044 */
	__IO uint32_t GSPI_RXBD_AGG_CTRL  ;  /*!< Register,  Address offset:0x048 */
	__IO uint32_t GSPI_H2C_MSG        ;  /*!< Register,  Address offset:0x04C */
	__I  uint32_t GSPI_C2H_MSG        ;  /*!< Register,  Address offset:0x050 */
	__IO uint32_t RSVD4[11]           ;  /*!< Reserved,  Address offset:0x054-0x07F */
	__IO uint32_t GSPI_HRPWM          ;  /*!< Register,  Address offset:0x080 */
	__I  uint32_t GSPI_CPU_INDICATION ;  /*!< Register,  Address offset:0x084 */
	__IO uint32_t RSVD5[26]           ;  /*!< Reserved,  Address offset:0x088-0x0EF */
	__IO uint32_t GSPI_SPI_CFG        ;  /*!< Register,  Address offset:0x0F0 */
} GSPI_TypeDef;

/** @} */

// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* Exported constants --------------------------------------------------------*/

/** @defgroup GSPI_Exported_Constants GSPI Exported Constants
  * @{
  */

/** @defgroup GSPI_Register_Definitions GSPI device register map (host view)
  * @brief Byte / halfword addresses used by the driver.
  * @{
  */

#define GSPI_REG_SPI_INT          0x0004   /*!< Not readable, write only. */
#define GSPI_REG_HIMR             0x0014   /*!< Not readable, use the shadow. */
#define GSPI_REG_HISR             0x0018   /*!< Write 1 to clear. */
#define GSPI_REG_RX0_REQ_LEN      0x001C   /*!< = GSPI_RXPKT_INFO. bit31 RDY, bits[23:0] byte count. */
#define GSPI_REG_FREE_TX_BD_NUM   0x0020   /*!< = GSPI_TXPKT_INFO_1. Only bits[15:0] are the count. */
#define GSPI_REG_HCPWM            0x0038   /*!< 8bit, byte0 of the 0x38 word. From device CCPWM. */
#define GSPI_REG_HCPWM2           0x003A   /*!< 16bit, bits[31:16] of the 0x38 word. From CCPWM2. */
#define GSPI_REG_AVAI_PGTH_L      0x0040
#define GSPI_REG_AVAI_PGTH_H      0x0044
#define GSPI_REG_RX_AGG           0x0048
#define GSPI_REG_H2C_MSG          0x004C
#define GSPI_REG_C2H_MSG          0x0050
#define GSPI_REG_HRPWM            0x0080   /*!< 8bit, byte0 of the 0x80 word: payload[6:0] +
                                                bit7 TOGGLING -> device RPWM1_INT. Also the
                                                word address for 32-bit HRPWM/HRPWM2 access. */
#define GSPI_REG_HRPWM2           0x0082   /*!< 16bit, bits[31:16] of the 0x80 word. Its toggle is
                                                bit15 here but bit31 in the 32-bit view. */
#define GSPI_REG_CPU_INDICATION   0x0084   /*!< Word address. CPU_RDY is bit24 here. */
#define GSPI_REG_SPI_CFG          0x00F0
/** @} */

/** @defgroup GSPI_Sw_Definitions Software-defined masks and bits
  * @brief The few things AUTO_GEN has no counterpart for. Beware that AUTO_GEN
  *        numbers bits inside the 32-bit word, so an AUTO_GEN bit matches a byte
  *        or halfword access only when its field sits in byte 0; CPU_RDY (bit24)
  *        and the HRPWM2 toggle (bit31) do not, hence both are accessed 32-bit.
  * @{
  */
#define GSPI_MASK_TX_ERR          (GSPI_BIT_TXPKT_SIZE_OVER_BUFF | GSPI_BIT_TXAGG_SIZE_MISMATCH | GSPI_BIT_TXBD_OVERFLOW)
/*!< HISR bits 5..16 are unused: any of them set means the device never drove the
 *   Status phase and the bytes read back are the idle bus level. H2C_BUS_FAIL (20)
 *   and CPU_NOT_RDY (22) are "can't be read", so neither this nor TX_ERR sees them. */
#define GSPI_MASK_HISR_INVALID    ((u32)0x0001FFE0)
/*!< Bit positions within the 16-bit halfword: HRPWM2 at 0x0082, HCPWM2 at 0x003A.
 *   The manual marks HRPWM2[14:0] "NO USED", so ACT/CG are a software convention.
 *   Wrap in GSPI_R_HRPWM2() to write them through the 32-bit word at 0x0080. */
#define GSPI_BIT_HRPWM2_ACT       ((u32)0x00000001 << 0)
#define GSPI_BIT_HRPWM2_CG        ((u32)0x00000001 << 1)
#define GSPI_BIT_HCPWM2_ACT       ((u32)0x00000001 << 0)
/** @} */

/** @defgroup GSPI_Command_Word Command word fields
  * @brief Wire-format fields of the 4-byte Command phase.
  *
  *        Layout: R/W bit31, Fun bit29, domain bits[28:24], address bits[23:8],
  *        byte enables bits[3:0]. For the two FIFO domains the low 16 bits carry
  *        the transfer length instead of an address.
  *
  *        One frame lives inside a single CS-low window and has three phases:
  *          register read  : CMD 4B -> Status 8B -> Data 4B
  *          everything else: CMD 4B -> Data NB  -> Status 8B
  *        The data phase length must be a multiple of 4; an unaligned length still
  *        delivers the packet but leaves the Status phase undriven.
  * @{
  */
#define GSPI_CMD_FUN_BIT        ((u32)0x00000001 << 29)  /*!< Mandatory, undocumented by the UM. */
#define GSPI_DOMAIN_CTRL        0x00   /*!< Control register domain. */
#define GSPI_DOMAIN_TXFIFO      0x0C   /*!< Host -> device FIFO. */
#define GSPI_DOMAIN_RXFIFO      0x1F   /*!< Device -> host FIFO. */
#define GSPI_ALIGN4(x)          (((x) + 3) & ~3u) /*!< Round a data-phase length up to the 4-byte multiple the frame requires. */
/** @} */

/** @defgroup GSPI_Frame_Sizes Frame element sizes
  * @{
  */
#define GSPI_CMD_LEN            4    /*!< Command phase. */
#define GSPI_STATUS_LEN         8    /*!< Status phase. */
#define GSPI_TX_DESC_SIZE       16   /*!< = sizeof(INIC_TX_DESC). */
/*!< Buffer bytes a FIFO transfer of len data bytes needs, laid out as
 *   [CMD 4B][data, 4-byte aligned][Status 8B]. */
#define GSPI_FIFO_BUF_SIZE(len) (GSPI_CMD_LEN + GSPI_ALIGN4(len) + GSPI_STATUS_LEN)
/** @} */

/** @defgroup GSPI_SPI_CFG SPI_CFG values and activation
  * @{
  */
#define GSPI_LITTLE_ENDIAN_32   0x03   /*!< Device reset value; host must swap. */
#define GSPI_BIG_ENDIAN_32      0x01   /*!< No host swap needed. */
/*!< Activation retry policy, see GSPI_Configuration(): a frame sent while the
 *   device is still running its own SPDIO init is simply not seen. */
#define GSPI_ACTIVATE_RETRY     3
#define GSPI_ACTIVATE_RETRY_MS  50
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
