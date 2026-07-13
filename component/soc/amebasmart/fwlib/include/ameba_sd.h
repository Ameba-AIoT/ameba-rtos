/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SDIO_SD_H
#define _AMEBA_SDIO_SD_H

#include "ameba_sdioh.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup SD
  * @verbatim
  *****************************************************************************************
  * Introduction
  *****************************************************************************************
  * SDHOST:
  *        - Base Address: SDIOH_BASE
  *        - IP Clock: 100MHz
  *        - Support SD Spec. Version 2.0
  *        - High Voltage SD Memory Card
  *        – Operating voltage range: 2.7-3.6 V
  *        - Support 1/4-bit mode SD
  *        – Bus Speed Mode (using 4 parallel data lines)
  *            -Default Speed mode: 3.3V signaling, Frequency up to 25 MHz, up to 12.5 MB/sec
  *            -High Speed mode: 3.3V signaling, Frequency up to 50 MHz, up to 25 MB/sec
  *        - Support hardware CRC function for SD
  *
  *****************************************************************************************
  * How to use SDIO Host Controller
  *****************************************************************************************
  *      To use the SDIO Host Controller, the following steps are mandatory.
  *
  *      1. Insert SD card to card slot.
  *
  *      2. Call SD_Init() function to initialize SDHOST and SD card, in which the following operations are executed:
  *        (1) Enable SDHOST peripheral clock.
  *        (2) Configure the SDHOST pinmux.
  *        (3) Initialize SDIO Host to initial-mode and enable card interrupt.
  *        (4) If card insert is detected, card identification is started.
  *        (5) When card identification is successful, card enters into data transfer mode (Default Speed Mode).
  *        (6) Set SDHOST and card to 4-bit bus width or High Speed mode if needed according to configure parameters.
  *
  *      3.  After initialization, users can call the following functions to read or write blocks.
  *             SD_RESULT SD_ReadBlocks(u32 sector,u8 *data,u32 count);
  *             SD_RESULT SD_WriteBlocks(u32 sector,const u8 *data,u32 count);
  *
  *      Note that if users access SD card through FATFS, then the above steps don't need to be implemented manually.
  *      They are already porting to FATFS low-level driver, and users can call FATFS API directly.
  *      Details can be found in FATFS example.
  *
  *****************************************************************************************
  * @endverbatim
  */

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SDHOST SDHOST
  * @brief SDHOST driver modules
  * @{
  */

/* Exported types --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Types SDHOST Exported Types
  * @{
  */

/** @brief SD command index enumeration.
  */
typedef enum {
	SD_CMD_GoIdleSte           = 0,    /*!< CMD0: Go Idle State. */
	EMMC_CMD_SendOpCond        = 1,   /*!< CMD1: Send operating condition (eMMC only). */
	SD_CMD_AllSendCid          = 2,   /*!< CMD2: Send all CID numbers. */
	SD_CMD_SendRelAddr         = 3,   /*!< CMD3: Ask card to publish new RCA. */
	SD_CMD_SetDsr              = 4,   /*!< CMD4: Set DSR register. */
	SD_CMD_SwitchFunc          = 6,   /*!< CMD6: Check/switch card function. */
	SD_CMD_SetBusWidth         = 6,   /*!< ACMD6: Set bus width. */
	SD_CMD_SelDeselCard        = 7,   /*!< CMD7: Select/deselect card. */
	SD_CMD_SendIfCond          = 8,   /*!< CMD8: Send interface condition (SD only). */
	EMMC_CMD_SendExtCsd        = 8,   /*!< CMD8: Send extended CSD register (eMMC only). */
	SD_CMD_SendCsd             = 9,   /*!< CMD9: Send card-specific data. */
	SD_CMD_SendCid             = 10,  /*!< CMD10: Send card identification. */
	SD_CMD_VolSwitch           = 11,  /*!< CMD11: Switch signaling voltage to 1.8V (SD only). */
	SD_CMD_StopXsmission       = 12,  /*!< CMD12: Stop transmission. */
	SD_CMD_SendSts             = 13,  /*!< CMD13: Send card status. */
	SD_CMD_SetBlklen           = 16,  /*!< CMD16: Set block length in bytes. */
	SD_CMD_RdSingleBlk         = 17,  /*!< CMD17: Read single block. */
	SD_CMD_RdMulBlk            = 18,  /*!< CMD18: Read multiple blocks. */
	SD_CMD_SendTuningBlk       = 19,  /*!< CMD19: Send tuning block for sampling clock tuning (SD only). */
	SD_CMD_SendNumWrBlks       = 22,  /*!< ACMD22: Get the number of successfully written blocks. */
	SD_CMD_SetBlkCnt           = 23,  /*!< CMD23: Set block count for transfer. */
	SD_CMD_SetWrBlkEraseCnt    = 23,  /*!< ACMD23: Set number of write blocks to pre-erase before writing. */
	SD_CMD_WrBlk               = 24,  /*!< CMD24: Write single block. */
	SD_CMD_WrMulBlk            = 25,  /*!< CMD25: Write multiple blocks. */
	SD_CMD_ProgCsd             = 27,  /*!< CMD27: Program CSD register. */
	SD_CMD_EraseBlkSt          = 32,  /*!< CMD32: Set first write block address to be erased (SD only). */
	SD_CMD_EraseBlkEd          = 33,  /*!< CMD33: Set last write block address to be erased (SD only). */
	EMMC_CMD_EraseAddrSt       = 35,  /*!< CMD35: Set first erase group start address (eMMC only). */
	EMMC_CMD_EraseAddrEd       = 36,  /*!< CMD36: Set last erase group end address (eMMC only). */
	SD_CMD_Erase               = 38,  /*!< CMD38: Erase selected blocks. */
	SD_CMD_SdSendOpCond        = 41,  /*!< ACMD41: Send host capacity support and request card operating condition (SD only). */
	SD_CMD_SendScr             = 51,  /*!< ACMD51: Read SD configuration register (SCR) (SD only). */
	SD_CMD_AppCmd              = 55   /*!< CMD55: Indicate that the next command is an application-specific command (SD only). */
} SD_COMMAND;

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

/** @brief SD CardInfo Structure Definition
  */
typedef struct {
	u8 csd[SDIOH_CSD_LEN]; 	/*!< Store the card-specific data (CSD) of the current SD card. */
	u16 rca;				/*!< Store the relative address (RCA) of the current SD card. */
	u8 is_sdhc_sdxc;		/*!< Indicate whether the current card is SDSC (0) or SDHC/SDXC (1). */
	u8 sd_spec_ver;			/*!< Specify the physical layer specification version of the current
								card, which would be a value of @ref SD_Specification_Version */
	u32 capaticy;			/*!< Specify the capacity of the current card. Unit: KB.*/
	u32 read_bl_len;		/*!< Specify max. read data block length of current card. Unit: byte.*/
	u32 write_bl_len;		/*!< Specify max. write data block length. Unit: byte.*/

	u8 sig_level;			/*!< Indicate the current signaling voltage level: 0 for 3.3V, 1 for 1.8V. */
	u8 bus_spd;				/*!< Specify current bus speed, which would be a value of @ref SD_Access_Mode. */
	SD_RESULT sd_status;	/*!< Specify current SD status, which would be a value of @ref SD_RESULT.*/

	u8 dma_buf[SDIOH_C6R2_BUF_LEN] __attribute__((aligned(CACHE_LINE_SIZE)));	/*!< DMA buffer, 32 byte-alignment.*/
} SD_CardInfo;

/** @brief SDHOST CFG Structure Definition
  */
typedef struct {
	u8 sdioh_bus_speed;		/*!< Specifies SD Host bus speed, which should be SD_SPEED_DS or SD_SPEED_HS. */
	u8 sdioh_bus_width;		/*!< Specifies SD Host bus width, which should be a value of @ref SDHOST_Bus_Width. */
	u32 sdioh_cd_pin;		/*!< Specifies the card detect pin, which should be _PNC if not used, or an available GPIO pad. */
	u32 sdioh_wp_pin;		/*!< Specifies the write protection pin, which should be _PNC if not used, or an available GPIO pad. */
} SDIOHCFG_TypeDef;

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

/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Constants SDHOST Exported Constants
  * @{
  */

/// @cond
/** @defgroup SDHOST_Configuration SDHOST Configuration
  * @{
  */
#define SD								0
#define EMMC							1
#define SDIO							SD
#define SD_SEMA_MAX_DELAY 				(10)
/**
  * @}
  */
/// @endcond

/** @defgroup SD_Specification_Version SD Specification Version
  * @{
  */
#define SD_SPEC_V101					((u8)0x0)   /*!< SD physical layer specification version 1.01. */
#define SD_SPEC_V110					((u8)0x1)   /*!< SD physical layer specification version 1.10. */
#define SD_SPEC_V200 					((u8)0x2)   /*!< SD physical layer specification version 2.00. */
#define SD_SPEC_V300					((u8)0x3)   /*!< SD physical layer specification version 3.00. */
/**
  * @}
  */

/** @defgroup SD_Access_Mode SD Access Mode
  * @{
  */
#define SD_SPEED_DS						((u8)0x00U)   /*!< Default speed mode, 3.3V signaling, up to 25 MHz (function 0). */
#define SD_SPEED_HS						((u8)0x01U)   /*!< High speed mode, 3.3V signaling, up to 50 MHz (function 1). */
#define SD_SPEED_SDR12					((u8)0x02U)   /*!< SDR12 mode, 1.8V signaling, up to 25 MHz (function 0). */
#define SD_SPEED_SDR25					((u8)0x03U)   /*!< SDR25 mode, 1.8V signaling, up to 50 MHz (function 1). */
#define SD_SPEED_SDR50					((u8)0x04U)   /*!< SDR50 mode, 1.8V signaling, up to 100 MHz (function 2). */
#define SD_SPEED_SDR104					((u8)0x05U)   /*!< SDR104 mode, 1.8V signaling, up to 208 MHz (function 3). */
#define SD_SPEED_DDR50					((u8)0x06U)   /*!< DDR50 mode, 1.8V signaling, up to 50 MHz DDR (function 4). */
#define SD_KEEP_CUR_SPEED				((u8)0x0FU)   /*!< Keep current bus speed without switching. */
/**
  * @}
  */

/** @defgroup SD_Block_Size SD Block Size
  * @{
  */
#define SD_BLOCK_SIZE					(512)    /*!< Block size is 512 bytes by default. */
#define SD_MALLOC_BLK_CNT			(8)   /*!< Malloc size: 8 blocks. */
/**
  * @}
  */

/** @defgroup SD_RESP_ACMD SD RESP ACMD
  * @{
  */
#define SD_APP_CMD						BIT(5)   /*!< Status bit indicating an application command follows. */
/**
  * @}
  */

/** @defgroup SDIO_RESP0_CMD SDIO RESP0 CMD
  * @{
  */
#define SD_ADDRESS_ERROR				BIT(6)   /*!< Status bit indicating an address error. */
#define SD_BLOCK_LEN_ERROR				BIT(5)   /*!< Status bit indicating a block length error. */
#define SD_WP_VIOLATION					BIT(2)   /*!< Status bit indicating a write protection violation. */
/**
  * @}
  */

/** @defgroup SDXC_Power_Control_ACMD41 SDXC Power Control ACMD41
  * @{
  */
#define SD_POWER_SAVING					0   /*!< Select power saving mode for SDXC ACMD41. */
#define SD_MAX_PERFORM					1   /*!< Select maximum performance mode for SDXC ACMD41. */
/**
  * @}
  */

/** @defgroup SD_Switch_18_Request_ACMD41 SD Switch 18 Request ACMD41
  * @{
  */
#define SD_USE_CUR_VOL					0   /*!< Keep current voltage without switching. */
#define SD_SWITCH_18V					1   /*!< Request switch to 1.8V signaling via ACMD41. */
/**
  * @}
  */

/** @defgroup SD_CMD6_OpMode SD CMD6 Operation Mode
  * @{
  */
#define SD_CMD6_CHECK_MODE				((u8)0x00U)   /*!< CMD6 check mode: query function support without switching. */
#define SD_CMD6_SWITCH_MODE				((u8)0x01U)   /*!< CMD6 switch mode: switch to the selected function. */
/**
  * @}
  */

/** @defgroup SD_Capacity_Support_ACMD41 SD Capacity Support ACMD41
  * @{
  */
#define SD_SUPPORT_SDSC_ONLY			0   /*!< Host supports SDSC cards only. */
#define SD_SUPPORT_SDHC_SDXC			1   /*!< Host supports SDHC and SDXC cards. */
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Functions SDHOST Exported Functions
  * @{
  */

/** @defgroup SD_Initialization_Functions SD Initialization and De-initialization Functions
  * @{
  */
_LONG_CALL_ SD_RESULT SD_Init(void);
_LONG_CALL_ SD_RESULT SD_DeInit(void);
_LONG_CALL_ void SD_CardInit(void);
/**
  * @}
  */

/** @defgroup SD_IO_Functions SD Input and Output Operation Functions
  * @{
  */
_LONG_CALL_ u32 SD_ReadBlock(uint8_t *readbuff, uint32_t BlockIdx);
_LONG_CALL_ u32 SD_ReadMultiBlocks(uint8_t *readbuff, uint32_t BlockIdx, uint32_t NumberOfBlocks);
_LONG_CALL_ u32 SD_WriteBlock(uint8_t *writebuff, uint32_t BlockIdx);
_LONG_CALL_ u32 SD_WriteMultiBlocks(uint8_t *writebuff, uint32_t BlockIdx, uint32_t NumberOfBlocks);
_LONG_CALL_ u32 SD_Erase(uint32_t StartBlock, uint32_t EndBlock);
/**
  * @}
  */

/** @defgroup SD_Control_Functions SD Peripheral Control Functions
  * @{
  */
_LONG_CALL_ u32 SD_SwitchBusSpeed(u8 speed);
/**
  * @}
  */

/** @defgroup SD_Card_Functions SD Card Related Functions
  * @{
  */
_LONG_CALL_ SD_RESULT SD_GetCapacity(u32 *sector_count);
_LONG_CALL_ SD_RESULT SD_ReadBlocks(u32 sector, u8 *data, u32 count);
_LONG_CALL_ SD_RESULT SD_WriteBlocks(u32 sector, const u8 *data, u32 count);
_LONG_CALL_ SD_RESULT SD_Status(void);
_LONG_CALL_ SD_RESULT SD_GetEXTCSD(u8 *pbuf);
_LONG_CALL_ u32 SD_GetSDStatus(u8 *buf_32align);
_LONG_CALL_ void SD_SetCdCallback(void (*cd_callback)(SD_RESULT));
_LONG_CALL_ void SD_SetSema(int (*sema_take_fn)(u32), int (*sema_give_isr_fn)(u32));
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
