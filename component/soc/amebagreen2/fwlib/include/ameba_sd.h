/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SDIO_SD_H
#define _AMEBA_SDIO_SD_H

/* Includes ------------------------------------------------------------------*/
#include "ameba_sd_host.h"

/** @addtogroup Ameba_Mbed_API
  * @{
  */

/** @defgroup SD SD
  * @brief SD HAL module driver
  * @{
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup SD_Exported_Types SD Exported Types
  * @{
  */

/** @defgroup SD_Result
  * @{
  */
typedef enum {
	SD_OK = 0,
	SD_NODISK,
	SD_INSERT,
	SD_INITERR,
	SD_PROTECTED,
	SD_ERROR,
} SD_RESULT;
/**
  * @}
  */

/**
  * @brief  HAL Lock structures definition
  */
typedef enum {
	HAL_UNLOCKED = 0x00U,
	HAL_LOCKED   = 0x01U
} HAL_LockTypeDef;

/** @defgroup SD_Exported_Types_Group2 SD State enumeration structure
  * @{
  */
typedef enum {
	SD_STATE_RESET                  = 0x00000000U,  /*!< SD not yet initialized or disabled  */
	SD_STATE_READY                  = 0x00000001U,  /*!< SD initialized and ready for use    */
	SD_STATE_TIMEOUT                = 0x00000002U,  /*!< SD Timeout state                    */
	SD_STATE_BUSY                   = 0x00000003U,  /*!< SD process ongoing                  */
	SD_STATE_PROGRAMMING            = 0x00000004U,  /*!< SD Programming State                */
	SD_STATE_RECEIVING              = 0x00000005U,  /*!< SD Receiving State                  */
	SD_STATE_TRANSFER               = 0x00000006U,  /*!< SD Transfer State                  */
	SD_STATE_ERROR                  = 0x0000000FU   /*!< SD is in error state                */
} SD_StateTypeDef;
/**
  * @}
  */

/** @defgroup SD_Exported_Types_Group3 SD Card State enumeration structure
  * @{
  */
typedef u32 SD_CardStateTypeDef;

#define SD_CARD_READY          0x00000001U  /*!< Card state is ready                     */
#define SD_CARD_IDENTIFICATION 0x00000002U  /*!< Card is in identification state         */
#define SD_CARD_STANDBY        0x00000003U  /*!< Card is in standby state                */
#define SD_CARD_TRANSFER       0x00000004U  /*!< Card is in transfer state               */
#define SD_CARD_SENDING        0x00000005U  /*!< Card is sending an operation            */
#define SD_CARD_RECEIVING      0x00000006U  /*!< Card is receiving operation information */
#define SD_CARD_PROGRAMMING    0x00000007U  /*!< Card is in programming state            */
#define SD_CARD_DISCONNECTED   0x00000008U  /*!< Card is disconnected                    */
#define SD_CARD_ERROR          0x000000FFU  /*!< Card response Error                     */
/**
  * @}
  */

/** @defgroup SD_Exported_Types_Group4 SD Handle Structure definition
  * @{
  */

/** @brief SDHOST CFG Structure Definition
  */
typedef struct {
	u8 sdioh_bus_speed;		/*!< Specify SD Host bus speed, should be SD_SPEED_DS or SD_SPEED_HS*/
	u8 sdioh_bus_width;		/*!< Specify SD Host bus width, should be a value of @ref SD_Bus_Width */
	u32 sdioh_cd_pin;		/*!< Specify Card Detect pin, should be a value of _PNC or available gpio pad */
	u32 sdioh_wp_pin;		/*!< Specify Write Protection pin, should be a value of _PNC or available gpio pad */
} SDIOHCFG_TypeDef;

/**
  * @brief  SDIO Card Information Structure definition
  */
typedef struct {
	u32 CardType;                     /*!< Specifies the card Type                         */
	u32 CardVersion;                  /*!< Specifies the card version                      */
	u32 CardSpecVer;                  /*!< Specifies the card physical layer spec version  */
	u16 Class;                        /*!< Specifies the class of the card command         */
	u32 RelCardAdd;                   /*!< Specifies the Relative Card Address             */
	u32 BlockNbr;                     /*!< Specifies the Card Capacity in blocks           */
	u32 BlockSize;                    /*!< Specifies one block size in bytes               */
	u32 LogBlockNbr;                  /*!< Specifies the Card logical Capacity in blocks   */
	u32 LogBlockSize;                 /*!< Specifies logical block size in bytes           */
} SDIO_CardInfoTypeDef;

typedef struct {
	SDIOHOST_TypeDef         *Instance;   /*!< SD registers base address           */

	SDIO_CardInfoTypeDef     Card;        /*!< Card information                    */
	HAL_LockTypeDef          Lock;        /*!< SD locking object                   */
	u32                      CSD[4];      /*!< SD card specific data table         */
	u32                      CID[4];      /*!< SD card identification number table */
	u32                      SCR[2];      /*!< SD configuration register           */

	__IO SD_StateTypeDef     State;       /*!< SD card State                       */
	__IO u32                 Context;     /*!< SD transfer context                 */
	__IO u32                 ErrorCode;   /*!< SD Card Error codes                 */

	u8                       *pTxBuffPtr; /*!< Pointer to SD Tx transfer Buffer    */
	u8                       *pRxBuffPtr; /*!< Pointer to SD Rx transfer Buffer    */
	u32                      TxXferSize;  /*!< SD Tx Transfer size                 */
	u32                      RxXferSize;  /*!< SD Rx Transfer size                 */
} SD_HdlTypeDef;

/**
  * @}
  */

/** @defgroup SD_Exported_Types_Group4 Card Specific Data: CSD Register
  * @{
  */
typedef struct {
	__IO u8  CSDStruct;            /*!< CSD structure                         */
	__IO u8  SysSpecVersion;       /*!< System specification version          */
	__IO u8  Reserved1;            /*!< Reserved                              */
	__IO u8  TAAC;                 /*!< Data read access time 1               */
	__IO u8  NSAC;                 /*!< Data read access time 2 in CLK cycles */
	__IO u8  MaxBusClkFrec;        /*!< Max. bus clock frequency              */
	__IO u16 CardComdClasses;      /*!< Card command classes                  */
	__IO u8  RdBlockLen;           /*!< Max. read data block length           */
	__IO u8  PartBlockRead;        /*!< Partial blocks for read allowed       */
	__IO u8  WrBlockMisalign;      /*!< Write block misalignment              */
	__IO u8  RdBlockMisalign;      /*!< Read block misalignment               */
	__IO u8  DSRImpl;              /*!< DSR implemented                       */
	__IO u8  Reserved2;            /*!< Reserved                              */
	__IO u32 DeviceSize;           /*!< Device Size                           */
	__IO u8  MaxRdCurrentVDDMin;   /*!< Max. read current @ VDD min           */
	__IO u8  MaxRdCurrentVDDMax;   /*!< Max. read current @ VDD max           */
	__IO u8  MaxWrCurrentVDDMin;   /*!< Max. write current @ VDD min          */
	__IO u8  MaxWrCurrentVDDMax;   /*!< Max. write current @ VDD max          */
	__IO u8  DeviceSizeMul;        /*!< Device size multiplier                */
	__IO u8  EraseGrSize;          /*!< Erase group size                      */
	__IO u8  EraseGrMul;           /*!< Erase group size multiplier           */
	__IO u8  WrProtectGrSize;      /*!< Write protect group size              */
	__IO u8  WrProtectGrEnable;    /*!< Write protect group enable            */
	__IO u8  ManDeflECC;           /*!< Manufacturer default ECC              */
	__IO u8  WrSpeedFact;          /*!< Write speed factor                    */
	__IO u8  MaxWrBlockLen;        /*!< Max. write data block length          */
	__IO u8  WriteBlockPaPartial;  /*!< Partial blocks for write allowed      */
	__IO u8  Reserved3;            /*!< Reserved                              */
	__IO u8  ContentProtectAppli;  /*!< Content protection application        */
	__IO u8  FileFormatGroup;      /*!< File format group                     */
	__IO u8  CopyFlag;             /*!< Copy flag (OTP)                       */
	__IO u8  PermWrProtect;        /*!< Permanent write protection            */
	__IO u8  TempWrProtect;        /*!< Temporary write protection            */
	__IO u8  FileFormat;           /*!< File format                           */
	__IO u8  ECC;                  /*!< ECC code                              */
} SD_CardCSDTypeDef;
/**
  * @}
  */

/** @defgroup SD_Exported_Types_Group5 Card Identification Data: CID Register
  * @{
  */
typedef struct {
	__IO u8  ManufacturerID;  /*!< Manufacturer ID       */
	__IO u16 OEM_AppliID;     /*!< OEM/Application ID    */
	__IO u32 ProdName1;       /*!< Product Name part1    */
	__IO u8  ProdName2;       /*!< Product Name part2    */
	__IO u8  ProdRev;         /*!< Product Revision      */
	__IO u32 ProdSN;          /*!< Product Serial Number */
	__IO u8  Reserved1;       /*!< Reserved1             */
	__IO u16 ManufactDate;    /*!< Manufacturing Date    */

} SD_CardCIDTypeDef;
/**
  * @}
  */

/** @defgroup SD_Exported_Types_Group6 SD Card Status returned by ACMD13
  * @{
  */
typedef struct {
	__IO u8  DataBusWidth;           /*!< Shows the currently defined data bus width                 */
	__IO u8  SecuredMode;            /*!< Card is in secured mode of operation                       */
	__IO u16 CardType;               /*!< Carries information about card type                        */
	__IO u32 ProtectedAreaSize;      /*!< Carries information about the capacity of protected area   */
	__IO u8  SpeedClass;             /*!< Carries information about the speed class of the card      */
	__IO u8  PerformanceMove;        /*!< Carries information about the card's performance move      */
	__IO u8  AllocationUnitSize;     /*!< Carries information about the card's allocation unit size  */
	__IO u16 EraseSize;              /*!< Determines the number of AUs to be erased in one operation */
	__IO u8  EraseTimeout;           /*!< Determines the timeout for any number of AU erase          */
	__IO u8  EraseOffset;            /*!< Carries information about the erase offset                 */

} SD_StatusTypeDef;
/**
  * @}
  */

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SD_Exported_Constants Exported Constants
  * @{
  */

#define SD_BLOCK_SIZE 512U // Block size is 512 bytes by default
#define SD_FATFS_TIMEOUT (30 * 1000 * 1000) // 30s: 30 * 1000 * 1000us
#define SD_SEMA_MAX_DELAY  10

/**
  * @}
  */

/** @defgroup SD_Exported_Constansts_Group1 SD context enumeration
  * @{
  */
#define SD_CONTEXT_NONE                            0x00000000U // None
#define SD_CONTEXT_READ_SINGLE_BLOCK               0x00000001U // Read single block operation
#define SD_CONTEXT_READ_MULTIPLE_BLOCK             0x00000002U // Read multiple blocks operation
#define SD_CONTEXT_WRITE_SINGLE_BLOCK              0x00000010U // Write single block operation
#define SD_CONTEXT_WRITE_MULTIPLE_BLOCK            0x00000020U // Write multiple blocks operation
#define SD_CONTEXT_IT                              0x00000008U // Process in Interrupt mode
#define SD_CONTEXT_DMA                             0x00000080U // Process in DMA mode

/**
  * @}
  */

/** @defgroup SD_Exported_Constansts_Group2 Supported Card Type
  * @{
  */
#define CARD_SDSC                  0x00000001U // SD Standard Capacity <2Go
#define CARD_SDHC_SDXC             0x00000002U // SD High Capacity <32Go, SD Extended Capacity <2To
#define CARD_SDIO_ONLY             0x00000004U // SDIO only card
#define CARD_COMBO                 0x00000008U // COMBO card
#define CARD_UNSUPPORTED           0x00000010U // unsupported card type

/**
  * @}
  */

/** @defgroup SD_Exported_Constansts_Group3 SD Supported Version
  * @{
  */
#define CARD_V1_X                  0x00000001U
#define CARD_V2_X                  0x00000002U
/**
  * @}
  */

/** @defgroup SD_Exported_Constansts_Group4 SDIO CMD Argument
  * @{
  */

/* R4 response (IO OCR) */
#define SD_RESP_R4_MEM_READY             (1<<31)

#define SD_RESP_R4_NUM_FUNCTIONS(ocr)    (((ocr) >> 28) & 0x7)

#define SD_RESP_R4_MEM_PRESENT           (1<<27)

#define SD_RESP_R4_0CR_SHIFT             0
#define SD_RESP_R4_0CR_MASK              0x00fffff0

/* CMD52_CMD53 direction */
#define BUS_READ	0
#define BUS_WRITE	1

/* CMD52 arguments */
#define SD_ARG_CMD52_R_FLAG        (0 << 31)
#define SD_ARG_CMD52_W_FLAG        (1 << 31)

#define SD_ARG_CMD52_FUNC_SHIFT    28
#define SD_ARG_CMD52_FUNC_MASK     0x7

#define SD_ARG_CMD52_RAW_FLAG      (1 << 27)

#define SD_ARG_CMD52_ADDR_SHIFT    9
#define SD_ARG_CMD52_ADDR_MASK     0x1ffff

#define SD_ARG_CMD52_DATA_SHIFT    0
#define SD_ARG_CMD52_DATA_MASK     0xff

/* CMD53 arguments */
#define SD_ARG_CMD53_R_FLAG        (0 << 31)
#define SD_ARG_CMD53_W_FLAG        (1 << 31)

#define SD_ARG_CMD53_FUNC_SHIFT    28
#define SD_ARG_CMD53_FUNC_MASK     0x7

#define SD_ARG_CMD53_BLOCK_MODE    (1 << 27)
#define SD_ARG_CMD53_BYTE_MODE     (0 << 27)

#define SD_ARG_CMD53_OP_CODE       (1 << 26)

#define SD_ARG_CMD53_ADDR_SHIFT    9
#define SD_ARG_CMD53_ADDR_MASK     0x1ffff

#define SD_ARG_CMD53_COUNT_SHIFT   0
#define SD_ARG_CMD53_COUNT_MASK    0x1ff
#define SD_ARG_CMD53_COUNT_MAX     512

/**
  * @}
  */

/** @defgroup SDIO Device CCCR offset
  * @{
  */
#define SDIOD_CCCR_REV             ((u32)0x00) /* CCCR/SDIO Revision */
#define SDIOD_CCCR_SDREV           ((u32)0x01) /* SD Revision */
#define SDIOD_CCCR_IOEN            ((u32)0x02) /* I/O Enable */
#define SDIOD_CCCR_IORDY           ((u32)0x03) /* I/O Ready */
#define SDIOD_CCCR_INTEN           ((u32)0x04) /* Interrupt Enable */
#define SDIOD_CCCR_INTPEND         ((u32)0x05) /* Interrupt Pending */
#define SDIOD_CCCR_IOABORT         ((u32)0x06) /* I/O Abort */
#define SDIOD_CCCR_BICTRL          ((u32)0x07) /* Bus Interface control */
#define SDIOD_CCCR_CAPABLITIES     ((u32)0x08) /* Card Capabilities */
#define SDIOD_CCCR_CISPTR_0        ((u32)0x09) /* Common CIS Base Address Pointer Register 0 (LSB) */
#define SDIOD_CCCR_CISPTR_1        ((u32)0x0A) /* Common CIS Base Address Pointer Register 1 */
#define SDIOD_CCCR_CISPTR_2        ((u32)0x0B) /* Common CIS Base Address Pointer Register 2 (MSB - only bit 1 valid)*/
#define SDIOD_CCCR_BUSSUSP         ((u32)0x0C) /* */
#define SDIOD_CCCR_FUNCSEL         ((u32)0x0D) /* */
#define SDIOD_CCCR_EXECFLAGS       ((u32)0x0E) /* */
#define SDIOD_CCCR_RDYFLAGS        ((u32)0x0F) /* */
#define SDIOD_CCCR_BLKSIZE_0       ((u32)0x10) /* Function 0 (Bus) SDIO Block Size Register 0 (LSB) */
#define SDIOD_CCCR_BLKSIZE_1       ((u32)0x11) /* Function 0 (Bus) SDIO Block Size Register 1 (MSB) */
#define SDIOD_CCCR_POWER_CONTROL   ((u32)0x12) /* Power Control */
#define SDIOD_CCCR_SPEED_CONTROL   ((u32)0x13) /* Bus Speed Select  (control device entry into high-speed clocking mode)  */
#define SDIOD_CCCR_UHS_I           ((u32)0x14) /* UHS-I Support */
#define SDIOD_CCCR_DRIVE           ((u32)0x15) /* Drive Strength */
#define SDIOD_CCCR_INTEXT          ((u32)0x16) /* Interrupt Extension */
#define SDIOD_SEP_INT_CTL          ((u32)0xF2) /* Separate Interrupt Control*/

#define SDIOD_FBR_BASE(x)          ((x) * 0x100) /* base of function x's FBRs */
#define SDIOD_CCCR_F1BLKSIZE_0     (SDIOD_FBR_BASE(0x1) + 0x10)  /* Function 1 SDIO Block Size Register 0 (LSB) */
#define SDIOD_CCCR_F1BLKSIZE_1     (SDIOD_FBR_BASE(0x1) + 0x11)  /* Function 1 SDIO Block Size Register 1 (MSB) */

/**
  * @}
  */

/** @defgroup SDIO Device CCCR Bits
  * @{
  */
/* SDIOD_CCCR_REV Bits */
#define SDIO_REV_SDIOID_MASK       ((u8)0xF0) /* SDIO spec revision number */
#define SDIO_REV_CCCRID_MASK       ((u8)0x0F) /* CCCR format version number */

/* SDIOD_CCCR_SDREV Bits */
#define SD_REV_PHY_MASK            ((u8)0x0F) /* SD format version number */

/* SDIOD_CCCR_IOEN Bits */
#define SDIO_FUNC_ENABLE_1         ((u8)0x02) /* function 1 I/O enable */
#define SDIO_FUNC_ENABLE_2         ((u8)0x04) /* function 2 I/O enable */
#define SDIO_FUNC_ENABLE_3         ((u8)0x08) /* function 3 I/O enable */

/* SDIOD_CCCR_IORDY Bits */
#define SDIO_FUNC_READY_1          ((u8)0x02) /* function 1 I/O ready */
#define SDIO_FUNC_READY_2          ((u8)0x04) /* function 2 I/O ready */
#define SDIO_FUNC_READY_3          ((u8)0x08) /* function 3 I/O ready */

/* SDIOD_CCCR_INTEN Bits */
#define INTR_CTL_MASTER_EN         ((u8)0x01) /* interrupt enable master */
#define INTR_CTL_FUNC1_EN          ((u8)0x02) /* interrupt enable for function 1 */
#define INTR_CTL_FUNC2_EN          ((u8)0x04) /* interrupt enable for function 2 */

/* SDIOD_SEP_INT_CTL Bits */
#define SEP_INTR_CTL_MASK          ((u8)0x01) /* out-of-band interrupt mask */
#define SEP_INTR_CTL_EN            ((u8)0x02) /* out-of-band interrupt output enable */
#define SEP_INTR_CTL_POL           ((u8)0x04) /* out-of-band interrupt polarity */

/* SDIOD_CCCR_INTPEND Bits */
#define INTR_STATUS_FUNC1          ((u8)0x02) /* interrupt pending for function 1 */
#define INTR_STATUS_FUNC2          ((u8)0x04) /* interrupt pending for function 2 */
#define INTR_STATUS_FUNC3          ((u8)0x08) /* interrupt pending for function 3 */

/* SDIOD_CCCR_IOABORT Bits */
#define IO_ABORT_RESET_ALL         ((u8)0x08) /* I/O card reset */
#define IO_ABORT_FUNC_MASK         ((u8)0x07) /* abort selection: function x */

/* SDIOD_CCCR_BICTRL Bits */
#define BUS_CARD_DETECT_DIS        ((u8)0x80) /* Card Detect disable */
#define BUS_SPI_CONT_INTR_CAP      ((u8)0x40) /* support continuous SPI interrupt */
#define BUS_SPI_CONT_INTR_EN       ((u8)0x20) /* continuous SPI interrupt enable */
#define BUS_SD_DATA_WIDTH_MASK     ((u8)0x03) /* bus width mask */
#define BUS_SD_DATA_WIDTH_4BIT     ((u8)0x02) /* bus width 4-bit mode */
#define BUS_SD_DATA_WIDTH_1BIT     ((u8)0x00) /* bus width 1-bit mode */

/* SDIOD_CCCR_CAPABLITIES Bits */
#define SDIO_CAP_4BLS              ((u8)0x80) /* 4-bit support for low speed card */
#define SDIO_CAP_LSC               ((u8)0x40) /* low speed card */
#define SDIO_CAP_E4MI              ((u8)0x20) /* enable interrupt between block of data in 4-bit mode */
#define SDIO_CAP_S4MI              ((u8)0x10) /* support interrupt between block of data in 4-bit mode */
#define SDIO_CAP_SBS               ((u8)0x08) /* support suspend/resume */
#define SDIO_CAP_SRW               ((u8)0x04) /* support read wait */
#define SDIO_CAP_SMB               ((u8)0x02) /* support multi-block transfer */
#define SDIO_CAP_SDC               ((u8)0x01) /* Support Direct commands during multi-byte transfer */

/* SDIOD_CCCR_POWER_CONTROL Bits */
#define SDIO_POWER_SMPC            ((u8)0x01) /* supports master power control (RO) */
#define SDIO_POWER_EMPC            ((u8)0x02) /* enable master power control (allow > 200mA) (RW) */

/* SDIOD_CCCR_SPEED_CONTROL Bits */
#define SDIO_SPEED_SHS             ((u8)0x01) /* supports high-speed [clocking] mode (RO) */
#define SDIO_SPEED_EHS             ((u8)0x02) /* enable high-speed [clocking] mode (RW) */

/* SDIO_FUNCTION_INT_MASK Bits*/
#define SDIO_FUNC_MASK_F1          ((u8)0x01) /* interrupt mask enable for function 1 */
#define SDIO_FUNC_MASK_F2          ((u8)0x02) /* interrupt mask enable for function 2 */
/**
  * @}
  */

/**
  * @}
  */

/* Exported macro ------------------------------------------------------------*/


/* Exported functions --------------------------------------------------------*/
/** @defgroup SD_Exported_Functions SD Exported Functions
  * @{
  */

/** @defgroup SD_Exported_Functions_Group1 Initialization and de-initialization functions
  * @{
  */
SD_RESULT SD_Init(void);
SD_RESULT SD_DeInit(void);
SD_RESULT SD_CardInit(void);
/**
  * @}
  */

/** @defgroup SD_Exported_Functions_Group2 Input and Output operation functions
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
void SD_CreateADMAEntry(u32 *pBuf, u16 Length, sdioh_adma_line_end_t End, u32 *pDescTable);
SD_RESULT SD_IO_RW_Direct(SD_HdlTypeDef *hsd, u8 RWFlag, u8 Func, u32 Addr, u8 In, u8 *Out);
SD_RESULT SD_IO_RW_Extended(SD_HdlTypeDef *hsd, u8 RWFlag, u8 Func, u8 OpCode, u32 Addr, u8 *pData, u8 IsBlock, u16 Cnt);
/**
  * @}
  */

/** @defgroup SD_Exported_Functions_Group3 Peripheral Control functions
  * @{
  */
SD_RESULT SD_SwitchFunction(SD_HdlTypeDef *hsd, u8 mode, u8 grp, u8 func, u8 *pData);
SD_RESULT SD_SwitchBusSpeed(SD_HdlTypeDef *hsd, u8 BusSpeed);
SD_RESULT SD_ConfigBusSpeed(SD_HdlTypeDef *hsd, u8 BusSpeed);
SD_RESULT SD_ConfigBusWidth(SD_HdlTypeDef *hsd, u8 WideMode);
/**
  * @}
  */

/** @defgroup SD_Exported_Functions_Group4 SD card related functions
  * @{
  */
SD_CardStateTypeDef SD_GetCardState(SD_HdlTypeDef *hsd);
void SD_GetCardCID(SD_HdlTypeDef *hsd, SD_CardCIDTypeDef *pCID);
void SD_GetCardInfo(SD_HdlTypeDef *hsd, SDIO_CardInfoTypeDef *pCardInfo);
SD_RESULT SD_GetCardCSD(SD_HdlTypeDef *hsd, SD_CardCSDTypeDef *pCSD);
SD_RESULT SD_GetSDStatus(SD_HdlTypeDef *hsd, SD_StatusTypeDef *pStatus);

_LONG_CALL_ void SD_SetSema(int (*sema_take_fn)(u32), int (*sema_give_isr_fn)(u32));
_LONG_CALL_ void SD_SetCdCallback(void (*cd_callback)(SD_RESULT));
u32 SD_WaitTransDone(SD_HdlTypeDef *hsd, u32 timeout_us);
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

/** @defgroup SD_Exported_Functions_Group5 SDIO related functions
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

#endif
