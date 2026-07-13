/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SDIO_SD_H
#define _AMEBA_SDIO_SD_H

/* Includes ------------------------------------------------------------------*/
#include "ameba_sd_host.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @defgroup SDHOST SDHOST
  * @brief SDHOST driver modules
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Types SDHOST Exported Types
  * @{
  */

/**
  * @brief  SD operation result type.
  */
typedef enum {
	SD_OK = 0,       /*!< Operation completed successfully. */
	SD_NODISK,       /*!< No SD card detected. */
	SD_INSERT,       /*!< SD card has been inserted. */
	SD_INITERR,      /*!< SD card initialization failed. */
	SD_PROTECTED,    /*!< SD card is write-protected. */
	SD_ERROR,        /*!< General SD operation error. */
} SD_RESULT;

/**
  * @brief  HAL Lock structure definition
  */
typedef enum {
	HAL_UNLOCKED = 0x00U, /*!< Resource is not locked. */
	HAL_LOCKED   = 0x01U  /*!< Resource is locked. */
} HAL_LockTypeDef;

/** @defgroup SD_State_Types SD State enumeration structure
  * @{
  */
/** @brief SD driver state machine states. */
typedef enum {
	SD_STATE_RESET                  = 0x00000000U,  /*!< SD not yet initialized or deinitialized. */
	SD_STATE_READY                  = 0x00000001U,  /*!< SD initialized and ready for use.        */
	SD_STATE_TIMEOUT                = 0x00000002U,  /*!< SD timeout state.                        */
	SD_STATE_BUSY                   = 0x00000003U,  /*!< SD process ongoing.                      */
	SD_STATE_PROGRAMMING            = 0x00000004U,  /*!< SD programming state.                    */
	SD_STATE_RECEIVING              = 0x00000005U,  /*!< SD receiving state.                      */
	SD_STATE_TRANSFER               = 0x00000006U,  /*!< SD data transfer state.                  */
	SD_STATE_ERROR                  = 0x0000000FU   /*!< SD is in error state.                    */
} SD_StateTypeDef;
/**
  * @}
  */

/** @defgroup SD_Card_State_Types SD Card State Type Definition
  * @{
  */
/** @brief SD card state machine states reported by the SD card. */
typedef enum {
	SD_CARD_READY          = 0x00000001U,  /*!< SD card is in ready state.              */
	SD_CARD_IDENTIFICATION = 0x00000002U,  /*!< SD card is in identification state.     */
	SD_CARD_STANDBY        = 0x00000003U,  /*!< SD card is in standby state.            */
	SD_CARD_TRANSFER       = 0x00000004U,  /*!< SD card is in data transfer state.      */
	SD_CARD_SENDING        = 0x00000005U,  /*!< SD card is in sending data state.       */
	SD_CARD_RECEIVING      = 0x00000006U,  /*!< SD card is in receiving data state.     */
	SD_CARD_PROGRAMMING    = 0x00000007U,  /*!< SD card is in programming state.        */
	SD_CARD_DISCONNECTED   = 0x00000008U,  /*!< SD card is in disconnected state.       */
	SD_CARD_ERROR          = 0x000000FFU,  /*!< SD card error state indicator.          */
} SD_CardStateTypeDef;
/**
  * @}
  */

/** @brief SDHOST CFG Structure Definition
  */
typedef struct {
	u8 sdioh_bus_speed;		/*!< Specifies SD Host bus speed, which should be SD_SPEED_DS or SD_SPEED_HS. */
	u8 sdioh_bus_width;		/*!< Specifies SD Host bus width, which should be a value of @ref SDHOST_Bus_Width. */
	u32 sdioh_cd_pin;		/*!< Specifies the card detect pin, which should be _PNC if not used, or an available GPIO pad. */
	u32 sdioh_wp_pin;		/*!< Specifies the write protection pin, which should be _PNC if not used, or an available GPIO pad. */
} SDIOHCFG_TypeDef;

/**
  * @brief  SDIO Card Information Structure definition
  */
typedef struct {
	u32 CardType;                     /*!< Specifies the card type.                        */
	u32 CardVersion;                  /*!< Specifies the card version.                     */
	u32 CardSpecVer;                  /*!< Specifies the card physical layer spec version. */
	u16 Class;                        /*!< Specifies the class of the card command.        */
	u32 RelCardAdd;                   /*!< Specifies the Relative Card Address.            */
	u32 BlockNbr;                     /*!< Specifies the Card Capacity in blocks.          */
	u32 BlockSize;                    /*!< Specifies one block size in bytes.              */
	u32 LogBlockNbr;                  /*!< Specifies the Card logical Capacity in blocks.  */
	u32 LogBlockSize;                 /*!< Specifies logical block size in bytes.          */
} SDIO_CardInfoTypeDef;

/** @brief SD Handle Structure Definition
  */
typedef struct {
	SDIOHOST_TypeDef         *Instance;   /*!< SD registers base address.          */

	SDIO_CardInfoTypeDef     Card;        /*!< Card information.                   */
	HAL_LockTypeDef          Lock;        /*!< SD locking object.                  */
	u32                      CSD[4];      /*!< SD card specific data table.        */
	u32                      CID[4];      /*!< SD card identification number table.*/
	u32                      SCR[2];      /*!< SD configuration register.          */

	__IO SD_StateTypeDef     State;       /*!< SD card State.                      */
	__IO u32                 Context;     /*!< SD transfer context.                */
	__IO u32                 ErrorCode;   /*!< SD Card Error codes.                */

	u8                       *pTxBuffPtr; /*!< Pointer to SD Tx transfer Buffer.   */
	u8                       *pRxBuffPtr; /*!< Pointer to SD Rx transfer Buffer.   */
	u32                      TxXferSize;  /*!< SD Tx Transfer size.                */
	u32                      RxXferSize;  /*!< SD Rx Transfer size.                */
} SD_HdlTypeDef;

/** @defgroup SD_CSD_Types SD CSD Register Structure Definition
  * @{
  */
/** @brief SD Card-Specific Data (CSD) register decoded structure. */
typedef struct {
	__IO u8  CSDStruct;            /*!< CSD structure.                        */
	__IO u8  SysSpecVersion;       /*!< System specification version.         */
	__IO u8  Reserved1;            /*!< Reserved.                             */
	__IO u8  TAAC;                 /*!< Data read access time 1.              */
	__IO u8  NSAC;                 /*!< Data read access time 2 in CLK cycles.*/
	__IO u8  MaxBusClkFrec;        /*!< Max. bus clock frequency.             */
	__IO u16 CardComdClasses;      /*!< Card command classes.                 */
	__IO u8  RdBlockLen;           /*!< Max. read data block length.          */
	__IO u8  PartBlockRead;        /*!< Partial blocks for read allowed.      */
	__IO u8  WrBlockMisalign;      /*!< Write block misalignment.             */
	__IO u8  RdBlockMisalign;      /*!< Read block misalignment.              */
	__IO u8  DSRImpl;              /*!< DSR implemented.                      */
	__IO u8  Reserved2;            /*!< Reserved.                             */
	__IO u32 DeviceSize;           /*!< Device Size.                          */
	__IO u8  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min.          */
	__IO u8  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max.          */
	__IO u8  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min.         */
	__IO u8  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max.         */
	__IO u8  DeviceSizeMul;        /*!< Device size multiplier.               */
	__IO u8  EraseGrSize;          /*!< Erase group size.                     */
	__IO u8  EraseGrMul;           /*!< Erase group size multiplier.          */
	__IO u8  WrProtectGrSize;      /*!< Write protect group size.             */
	__IO u8  WrProtectGrEnable;    /*!< Write protect group enable.           */
	__IO u8  ManDeflECC;           /*!< Manufacturer default ECC.             */
	__IO u8  WrSpeedFact;          /*!< Write speed factor.                   */
	__IO u8  MaxWrBlockLen;        /*!< Max. write data block length.         */
	__IO u8  WriteBlockPaPartial;  /*!< Partial blocks for write allowed.     */
	__IO u8  Reserved3;            /*!< Reserved.                             */
	__IO u8  ContentProtectAppli;  /*!< Content protection application.       */
	__IO u8  FileFormatGroup;      /*!< File format group.                    */
	__IO u8  CopyFlag;             /*!< Copy flag (OTP).                      */
	__IO u8  PermWrProtect;        /*!< Permanent write protection.           */
	__IO u8  TempWrProtect;        /*!< Temporary write protection.           */
	__IO u8  FileFormat;           /*!< File format.                          */
	__IO u8  ECC;                  /*!< ECC code.                             */
} SD_CardCSDTypeDef;
/**
  * @}
  */

/** @defgroup SD_CID_Types SD CID Register Structure Definition
  * @{
  */
/** @brief SD Card Identification (CID) register decoded structure. */
typedef struct {
	__IO u8  ManufacturerID;  /*!< Manufacturer ID.      */
	__IO u16 OEM_AppliID;     /*!< OEM/Application ID.   */
	__IO u32 ProdName1;       /*!< Product Name part1.   */
	__IO u8  ProdName2;       /*!< Product Name part2.   */
	__IO u8  ProdRev;         /*!< Product Revision.     */
	__IO u32 ProdSN;          /*!< Product Serial Number.*/
	__IO u8  Reserved1;       /*!< Reserved1.            */
	__IO u16 ManufactDate;    /*!< Manufacturing Date.   */

} SD_CardCIDTypeDef;
/**
  * @}
  */

/** @defgroup SD_Status_Types SD Status Types
  * @{
  */
/** @brief SD card status decoded structure reported via ACMD13 (SD_STATUS). */
typedef struct {
	__IO u8  DataBusWidth;           /*!< Shows the currently defined data bus width.                */
	__IO u8  SecuredMode;            /*!< Card is in secured mode of operation.                      */
	__IO u16 CardType;               /*!< Carries information about card type.                       */
	__IO u32 ProtectedAreaSize;      /*!< Carries information about the capacity of protected area.  */
	__IO u8  SpeedClass;             /*!< Carries information about the speed class of the card.     */
	__IO u8  PerformanceMove;        /*!< Carries information about the card's performance move.     */
	__IO u8  AllocationUnitSize;     /*!< Carries information about the card's allocation unit size. */
	__IO u16 EraseSize;              /*!< Determines the number of AUs to be erased in one operation.*/
	__IO u8  EraseTimeout;           /*!< Determines the timeout for a single AU erase operation.    */
	__IO u8  EraseOffset;            /*!< Carries information about the erase offset.                */

} SD_StatusTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Constants SDHOST Exported Constants
  * @{
  */

/** @defgroup SD_Block_Size SD Block Size
  * @{
  */
#define SD_BLOCK_SIZE (512)   /*!< Block size is 512 bytes by default. */
#define SD_MALLOC_BLK_CNT (8)   /*!< Malloc size: 8 blocks. */
/**
  * @}
  */

/// @cond
/** @defgroup SDHOST_Configuration SDHOST Configuration
  * @{
  */
#define SD_FATFS_TIMEOUT (30 * 1000 * 1000)   /*!< Read & Write operation timeout in microseconds (30 s). */
#define SD_SEMA_MAX_DELAY  (10)
/**
  * @}
  */
/// @endcond

/** @defgroup SD_Context_Constants SD Context Enumeration
  * @{
  */
#define SD_CONTEXT_NONE                            0x00000000U   /*!< No SD transfer context active. */
#define SD_CONTEXT_READ_SINGLE_BLOCK               0x00000001U   /*!< Context flag: single-block read operation. */
#define SD_CONTEXT_READ_MULTIPLE_BLOCK             0x00000002U   /*!< Context flag: multiple-block read operation. */
#define SD_CONTEXT_WRITE_SINGLE_BLOCK              0x00000010U   /*!< Context flag: single-block write operation. */
#define SD_CONTEXT_WRITE_MULTIPLE_BLOCK            0x00000020U   /*!< Context flag: multiple-block write operation. */
#define SD_CONTEXT_IT                              0x00000008U   /*!< Context flag: transfer processed in interrupt mode. */
#define SD_CONTEXT_DMA                             0x00000080U   /*!< Context flag: transfer processed in DMA mode. */

/**
  * @}
  */

/** @defgroup SD_Card_Type_Constants SD Supported Card Type
  * @{
  */
#define CARD_SDSC                  0x00000001U   /*!< SD Standard Capacity card type (up to 2 GB). */
#define CARD_SDHC_SDXC             0x00000002U   /*!< SD High/Extended Capacity card type (SDHC/SDXC). */
#define CARD_SDIO_ONLY             0x00000004U   /*!< SDIO-only card type (no memory function). */
#define CARD_COMBO                 0x00000008U   /*!< Combo card type (memory + SDIO functions). */
#define CARD_UNSUPPORTED           0x00000010U   /*!< Unsupported card type identifier. */

/**
  * @}
  */

/** @defgroup SD_Card_Version_Constants SD Supported Version
  * @{
  */
#define CARD_V1_X                  0x00000001U   /*!< SD card version 1.x identifier. */
#define CARD_V2_X                  0x00000002U   /*!< SD card version 2.x identifier. */
/**
  * @}
  */

/** @defgroup SD_CMD_Argument_Constants SDIO CMD Argument
  * @{
  */

/* R4 response (IO OCR) */
#define SD_RESP_R4_NUM_FUNCTIONS(ocr)    (((ocr) >> 28) & 0x7)   /*!< Extract number of I/O functions from R4 OCR response. */
#define SD_RESP_R4_MEM_PRESENT           (1<<27)   /*!< R4 response bit: memory present or not. */
#define SD_RESP_R4_0CR_SHIFT             0   /*!< Shift position of OCR field in R4 response. */
#define SD_RESP_R4_0CR_MASK              0x00fffff0   /*!< Bitmask of OCR voltage window in R4 response. */

/* CMD52_CMD53 direction */
#define BUS_READ	0   /*!< CMD52/CMD53 direction flag: read operation. */
#define BUS_WRITE	1   /*!< CMD52/CMD53 direction flag: write operation. */

/* CMD52 arguments */
#define SD_ARG_CMD52_R_FLAG        (0 << 31)   /*!< CMD52 argument flag: read direction (bit 31 = 0). */
#define SD_ARG_CMD52_W_FLAG        (1 << 31)   /*!< CMD52 argument flag: write direction (bit 31 = 1). */
#define SD_ARG_CMD52_FUNC_SHIFT    28   /*!< Shift position of function number field in CMD52 argument. */
#define SD_ARG_CMD52_FUNC_MASK     0x7   /*!< Bitmask of function number field in CMD52 argument. */
#define SD_ARG_CMD52_RAW_FLAG      (1 << 27)   /*!< CMD52 argument flag: read-after-write (RAW) operation. */
#define SD_ARG_CMD52_ADDR_SHIFT    9   /*!< Shift position of register address field in CMD52 argument. */
#define SD_ARG_CMD52_ADDR_MASK     0x1ffff   /*!< Bitmask of register address field in CMD52 argument. */
#define SD_ARG_CMD52_DATA_SHIFT    0   /*!< Shift position of data byte field in CMD52 argument. */
#define SD_ARG_CMD52_DATA_MASK     0xff   /*!< Bitmask of data byte field in CMD52 argument. */

/* CMD53 arguments */
#define SD_ARG_CMD53_R_FLAG        (0 << 31)   /*!< CMD53 argument flag: read direction (bit 31 = 0). */
#define SD_ARG_CMD53_W_FLAG        (1 << 31)   /*!< CMD53 argument flag: write direction (bit 31 = 1). */
#define SD_ARG_CMD53_FUNC_SHIFT    28   /*!< Shift position of function number field in CMD53 argument. */
#define SD_ARG_CMD53_FUNC_MASK     0x7   /*!< Bitmask of function number field in CMD53 argument. */
#define SD_ARG_CMD53_BLOCK_MODE    (1 << 27)   /*!< CMD53 argument flag: block transfer mode. */
#define SD_ARG_CMD53_BYTE_MODE     (0 << 27)   /*!< CMD53 argument flag: byte transfer mode. */
#define SD_ARG_CMD53_OP_CODE       (1 << 26)   /*!< CMD53 argument flag: incrementing address operation. */
#define SD_ARG_CMD53_ADDR_SHIFT    9   /*!< Shift position of register address field in CMD53 argument. */
#define SD_ARG_CMD53_ADDR_MASK     0x1ffff   /*!< Bitmask of register address field in CMD53 argument. */
#define SD_ARG_CMD53_COUNT_SHIFT   0   /*!< Shift position of byte/block count field in CMD53 argument. */
#define SD_ARG_CMD53_COUNT_MASK    0x1ff   /*!< Bitmask of byte/block count field in CMD53 argument. */
#define SD_ARG_CMD53_COUNT_MAX     512   /*!< Maximum byte/block count value for CMD53. */

/**
  * @}
  */

/** @defgroup SDIO_CCCR_Offsets SDIO Device CCCR Register Offsets
  * @{
  */
#define SDIOD_CCCR_REV             ((u32)0x00)   /*!< CCCR offset: CCCR/SDIO revision register. */
#define SDIOD_CCCR_SDREV           ((u32)0x01)   /*!< CCCR offset: SD specification revision register. */
#define SDIOD_CCCR_IOEN            ((u32)0x02)   /*!< CCCR offset: I/O function enable register. */
#define SDIOD_CCCR_IORDY           ((u32)0x03)   /*!< CCCR offset: I/O function ready register. */
#define SDIOD_CCCR_INTEN           ((u32)0x04)   /*!< CCCR offset: interrupt enable register. */
#define SDIOD_CCCR_INTPEND         ((u32)0x05)   /*!< CCCR offset: interrupt pending register. */
#define SDIOD_CCCR_IOABORT         ((u32)0x06)   /*!< CCCR offset: I/O abort register. */
#define SDIOD_CCCR_BICTRL          ((u32)0x07)   /*!< CCCR offset: bus interface control register. */
#define SDIOD_CCCR_CAPABILITIES    ((u32)0x08)   /*!< CCCR offset: card capabilities register. */
#define SDIOD_CCCR_CISPTR_0        ((u32)0x09)   /*!< CCCR offset: common CIS base address pointer byte 0 (LSB). */
#define SDIOD_CCCR_CISPTR_1        ((u32)0x0A)   /*!< CCCR offset: common CIS base address pointer byte 1. */
#define SDIOD_CCCR_CISPTR_2        ((u32)0x0B)   /*!< CCCR offset: common CIS base address pointer byte 2 (MSB). */
#define SDIOD_CCCR_BUSSUSP         ((u32)0x0C)   /*!< CCCR offset: bus suspend register. */
#define SDIOD_CCCR_FUNCSEL         ((u32)0x0D)   /*!< CCCR offset: function select register. */
#define SDIOD_CCCR_EXECFLAGS       ((u32)0x0E)   /*!< CCCR offset: execution flags register. */
#define SDIOD_CCCR_RDYFLAGS        ((u32)0x0F)   /*!< CCCR offset: ready flags register. */
#define SDIOD_CCCR_BLKSIZE_0       ((u32)0x10)   /*!< CCCR offset: function 0 block size register byte 0 (LSB). */
#define SDIOD_CCCR_BLKSIZE_1       ((u32)0x11)   /*!< CCCR offset: function 0 block size register byte 1 (MSB). */
#define SDIOD_CCCR_POWER_CONTROL   ((u32)0x12)   /*!< CCCR offset: power control register. */
#define SDIOD_CCCR_SPEED_CONTROL   ((u32)0x13)   /*!< CCCR offset: bus speed select register. */
#define SDIOD_CCCR_UHS_I           ((u32)0x14)   /*!< CCCR offset: UHS-I support register. */
#define SDIOD_CCCR_DRIVE           ((u32)0x15)   /*!< CCCR offset: drive strength register. */
#define SDIOD_CCCR_INTEXT          ((u32)0x16)   /*!< CCCR offset: interrupt extension register. */
#define SDIOD_SEP_INT_CTL          ((u32)0xF2)   /*!< CCCR offset: separate interrupt control register. */
#define SDIOD_FBR_BASE(x)          ((x) * 0x100)   /*!< Compute FBR base address offset for function x. */
#define SDIOD_CCCR_F1BLKSIZE_0     (SDIOD_FBR_BASE(0x1) + 0x10)   /*!< FBR offset: function 1 block size register byte 0 (LSB). */
#define SDIOD_CCCR_F1BLKSIZE_1     (SDIOD_FBR_BASE(0x1) + 0x11)   /*!< FBR offset: function 1 block size register byte 1 (MSB). */

/**
  * @}
  */

/** @defgroup SDIO_CCCR_Bits SDIO Device CCCR Register Bit Definitions
  * @{
  */
/* SDIOD_CCCR_REV Bits */
#define SDIO_REV_SDIOID_MASK       ((u8)0xF0)   /*!< Bitmask of SDIO specification revision in CCCR_REV. */
#define SDIO_REV_CCCRID_MASK       ((u8)0x0F)   /*!< Bitmask of CCCR format version in CCCR_REV. */

/* SDIOD_CCCR_SDREV Bits */
#define SD_REV_PHY_MASK            ((u8)0x0F)   /*!< Bitmask of SD physical layer spec version in CCCR_SDREV. */

/* SDIOD_CCCR_IOEN Bits */
#define SDIO_FUNC_ENABLE_1         ((u8)0x02)   /*!< CCCR_IOEN bit: enable I/O for function 1. */
#define SDIO_FUNC_ENABLE_2         ((u8)0x04)   /*!< CCCR_IOEN bit: enable I/O for function 2. */
#define SDIO_FUNC_ENABLE_3         ((u8)0x08)   /*!< CCCR_IOEN bit: enable I/O for function 3. */

/* SDIOD_CCCR_IORDY Bits */
#define SDIO_FUNC_READY_1          ((u8)0x02)   /*!< CCCR_IORDY bit: I/O ready status for function 1. */
#define SDIO_FUNC_READY_2          ((u8)0x04)   /*!< CCCR_IORDY bit: I/O ready status for function 2. */
#define SDIO_FUNC_READY_3          ((u8)0x08)   /*!< CCCR_IORDY bit: I/O ready status for function 3. */

/* SDIOD_CCCR_INTEN Bits */
#define INTR_CTL_MASTER_EN         ((u8)0x01)   /*!< CCCR_INTEN bit: master interrupt enable. */
#define INTR_CTL_FUNC1_EN          ((u8)0x02)   /*!< CCCR_INTEN bit: interrupt enable for function 1. */
#define INTR_CTL_FUNC2_EN          ((u8)0x04)   /*!< CCCR_INTEN bit: interrupt enable for function 2. */

/* SDIOD_SEP_INT_CTL Bits */
#define SEP_INTR_CTL_MASK          ((u8)0x01)   /*!< SEP_INT_CTL bit: out-of-band interrupt mask. */
#define SEP_INTR_CTL_EN            ((u8)0x02)   /*!< SEP_INT_CTL bit: out-of-band interrupt output enable. */
#define SEP_INTR_CTL_POL           ((u8)0x04)   /*!< SEP_INT_CTL bit: out-of-band interrupt polarity. */

/* SDIOD_CCCR_INTPEND Bits */
#define INTR_STATUS_FUNC1          ((u8)0x02)   /*!< CCCR_INTPEND bit: interrupt pending for function 1. */
#define INTR_STATUS_FUNC2          ((u8)0x04)   /*!< CCCR_INTPEND bit: interrupt pending for function 2. */
#define INTR_STATUS_FUNC3          ((u8)0x08)   /*!< CCCR_INTPEND bit: interrupt pending for function 3. */

/* SDIOD_CCCR_IOABORT Bits */
#define IO_ABORT_RESET_ALL         ((u8)0x08)   /*!< CCCR_IOABORT bit: reset all I/O functions. */
#define IO_ABORT_FUNC_MASK         ((u8)0x07)   /*!< CCCR_IOABORT bitmask: select function to abort. */

/* SDIOD_CCCR_BICTRL Bits */
#define BUS_CARD_DETECT_DIS        ((u8)0x80)   /*!< CCCR_BICTRL bit: disable card detect pull-up. */
#define BUS_SPI_CONT_INTR_CAP      ((u8)0x40)   /*!< CCCR_BICTRL bit: card supports continuous SPI interrupt. */
#define BUS_SPI_CONT_INTR_EN       ((u8)0x20)   /*!< CCCR_BICTRL bit: enable continuous SPI interrupt. */
#define BUS_SD_DATA_WIDTH_MASK     ((u8)0x03)   /*!< CCCR_BICTRL bitmask: SD bus width selection field. */
#define BUS_SD_DATA_WIDTH_4BIT     ((u8)0x02)   /*!< CCCR_BICTRL value: set bus width to 4-bit mode. */
#define BUS_SD_DATA_WIDTH_1BIT     ((u8)0x00)   /*!< CCCR_BICTRL value: set bus width to 1-bit mode. */

/* SDIOD_CCCR_CAPABILITIES Bits */
#define SDIO_CAP_4BLS              ((u8)0x80)   /*!< CCCR_CAP bit: supports 4-bit mode for low-speed card. */
#define SDIO_CAP_LSC               ((u8)0x40)   /*!< CCCR_CAP bit: card is a low-speed card. */
#define SDIO_CAP_E4MI              ((u8)0x20)   /*!< CCCR_CAP bit: enable interrupt between data blocks in 4-bit mode. */
#define SDIO_CAP_S4MI              ((u8)0x10)   /*!< CCCR_CAP bit: supports interrupt between data blocks in 4-bit mode. */
#define SDIO_CAP_SBS               ((u8)0x08)   /*!< CCCR_CAP bit: supports suspend/resume. */
#define SDIO_CAP_SRW               ((u8)0x04)   /*!< CCCR_CAP bit: supports read wait. */
#define SDIO_CAP_SMB               ((u8)0x02)   /*!< CCCR_CAP bit: supports multi-block transfer. */
#define SDIO_CAP_SDC               ((u8)0x01)   /*!< CCCR_CAP bit: supports direct commands during multi-byte transfer. */

/* SDIOD_CCCR_POWER_CONTROL Bits */
#define SDIO_POWER_SMPC            ((u8)0x01)   /*!< CCCR_POWER bit: card supports master power control (RO). */
#define SDIO_POWER_EMPC            ((u8)0x02)   /*!< CCCR_POWER bit: enable master power control above 200 mA (RW). */

/* SDIOD_CCCR_SPEED_CONTROL Bits */
#define SDIO_SPEED_SHS             ((u8)0x01)   /*!< CCCR_SPEED bit: card supports high-speed clocking mode (RO). */
#define SDIO_SPEED_EHS             ((u8)0x02)   /*!< CCCR_SPEED bit: enable high-speed clocking mode (RW). */

/* SDIO_FUNCTION_INT_MASK Bits*/
#define SDIO_FUNC_MASK_F1          ((u8)0x01)   /*!< Function interrupt mask bit: enable mask for function 1. */
#define SDIO_FUNC_MASK_F2          ((u8)0x02)   /*!< Function interrupt mask bit: enable mask for function 2. */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
void SD_IRQ_Notify(void);
void SD_CreateADMAEntry(u32 *pBuf, u16 Length, sdioh_adma_line_end_t End, u32 *pDescTable);
SD_RESULT SD_ReadBlocks_ADMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 *pDescTbl, u32 DescItemNum);
SD_RESULT SD_WriteBlocks_ADMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 *pDescTbl, u32 DescItemNum);

/** @defgroup SDHOST_Exported_Functions SDHOST Exported Functions
  * @{
  */

/** @defgroup SD_Initialization_Functions SD Initialization and De-initialization Functions
  * @{
  */
SD_RESULT SD_Init(void);
SD_RESULT SD_DeInit(void);
SD_RESULT SD_CardInit(void);
/**
  * @}
  */

/** @defgroup SD_IO_Functions SD Input and Output Operation Functions
  * @{
  */
/* Blocking mode: Polling */
SD_RESULT SD_ReadBlocks_PIO(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks, u32 Timeout);
SD_RESULT SD_WriteBlocks_PIO(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks, u32 Timeout);
SD_RESULT SD_Erase(SD_HdlTypeDef *hsd, u32 BlockStartAdd, u32 BlockEndAdd);
/* Non-Blocking mode: IT */
SD_RESULT SD_ReadBlocks_IT(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks);
SD_RESULT SD_WriteBlocks_IT(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks);
/* Non-Blocking mode: DMA */
SD_RESULT SD_ReadBlocks_DMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks);
SD_RESULT SD_WriteBlocks_DMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks);
SD_RESULT SD_IO_RW_Direct(SD_HdlTypeDef *hsd, u8 RWFlag, u8 Func, u32 Addr, u8 In, u8 *Out);
SD_RESULT SD_IO_RW_Extended(SD_HdlTypeDef *hsd, u8 RWFlag, u8 Func, u8 OpCode, u32 Addr, u8 *pData, u8 IsBlock, u16 Cnt);
/**
  * @}
  */

/** @defgroup SD_Control_Functions SD Peripheral Control Functions
  * @{
  */
SD_RESULT SD_SwitchFunction(SD_HdlTypeDef *hsd, u8 mode, u8 grp, u8 func, u8 *pData);
SD_RESULT SD_SwitchBusSpeed(SD_HdlTypeDef *hsd, u8 BusSpeed);
SD_RESULT SD_ConfigBusSpeed(SD_HdlTypeDef *hsd, u8 BusSpeed);
SD_RESULT SD_ConfigBusWidth(SD_HdlTypeDef *hsd, u8 WideMode);
/**
  * @}
  */

/** @defgroup SD_Card_Functions SD Card Related Functions
  * @{
  */
SD_CardStateTypeDef SD_GetCardState(SD_HdlTypeDef *hsd);
void SD_GetCardCID(SD_HdlTypeDef *hsd, SD_CardCIDTypeDef *pCID);
void SD_GetCardInfo(SD_HdlTypeDef *hsd, SDIO_CardInfoTypeDef *pCardInfo);
SD_RESULT SD_GetCardCSD(SD_HdlTypeDef *hsd, SD_CardCSDTypeDef *pCSD);
SD_RESULT SD_GetSDStatus(SD_HdlTypeDef *hsd, SD_StatusTypeDef *pStatus);

_LONG_CALL_ void SD_SetSema(int (*sema_take_fn)(u32), int (*sema_give_isr_fn)(u32));
_LONG_CALL_ void SD_SetCdCallback(void (*cd_callback)(SD_RESULT));
_LONG_CALL_ void SD_PreDMATrans(SD_HdlTypeDef *hsd);
_LONG_CALL_ u32 SD_WaitTransDone(SD_HdlTypeDef *hsd, u32 timeout_us);
_LONG_CALL_ SD_RESULT SD_Status(void);
_LONG_CALL_ int SD_CheckStatusTO(SD_HdlTypeDef *hsd, u32 timeout_us);
_LONG_CALL_ SD_RESULT SD_ReadBlocks(u32 sector, u8 *data, u32 count);
_LONG_CALL_ SD_RESULT SD_WriteBlocks(u32 sector, const u8 *data, u32 count);
_LONG_CALL_ SD_RESULT SD_GetCapacity(u32 *sector_count);
_LONG_CALL_ SD_RESULT SD_GetSectorSize(u32 *sector_size);
_LONG_CALL_ SD_RESULT SD_GetBlockSize(u32 *block_size);
/**
  * @}
  */

/** @defgroup SD_SDIO_Functions SDIO Related Functions
  * @{
  */
_LONG_CALL_ SD_RESULT SD_IO_ReadBytes(u8 FuncNum, u32 Addr, u8 *pData, u16 ByteCnt);
_LONG_CALL_ SD_RESULT SD_IO_ReadBlocks(u8 FuncNum, u32 Addr, u8 *pData, u16 BlockCnt);
_LONG_CALL_ SD_RESULT SD_IO_WriteBytes(u8 FuncNum, u32 Addr, u8 *pData, u16 ByteCnt);
_LONG_CALL_ SD_RESULT SD_IO_WriteBlocks(u8 FuncNum, u32 Addr, u8 *pData, u16 BlockCnt);
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif
#endif
