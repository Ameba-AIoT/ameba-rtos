/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_SDIOH_H
#define _AMEBA_SDIOH_H

#include "hal_platform.h"

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup SDHOST SDHOST
  * @brief SDHOST driver modules
  * @{
  */

/// @cond
/* Registers Definitions --------------------------------------------------------*/
/**
 * @defgroup SDHOST_Register_Definitions SDHOST Register Definitions
 * @{
 **/

/**
 * @defgroup SDHOST_SRAM_CRL
 * @{
 **/
#define SDIOH_SHIFT_LX_BURST_SIZE									6
#define SDIOH_LX_BURST_SIZE_64B										(0 << SDIOH_SHIFT_LX_BURST_SIZE)
#define SDIOH_SHIFT_MAP_SEL											5
#define SDIOH_MAP_SEL_DEC											(1 << SDIOH_SHIFT_MAP_SEL)
#define SDIOH_SHIFT_DIRECT_ACCESS_SEL										4
#define SDIOH_DIRECT_ACCESS_SEL										(1 << SDIOH_SHIFT_DIRECT_ACCESS_SEL)
/** @} */

/**
 * @defgroup SDHOST_DMA_CRL1
 * @{
 **/
#define SDIOH_MASK_DRAM_SA											0x0FFFFFFF
/** @} */

/**
 * @defgroup SDHOST_DMA_CRL2
 * @{
 **/
#define SDIOH_MASK_DMA_LEN											0x0000FFFF
/** @} */

/**
 * @defgroup SDHOST_DMA_CRL3
 * @{
 **/
#define SDIOH_DAT64_SEL												BIT(5)
#define SDIOH_RSP17_SEL												BIT(4)
#define SDIOH_SHIFT_DDR_WR											1
#define SDIOH_DMA_XFER												BIT(0)
/** @} */

/**
 * @defgroup SDHOST_SD_ISR
 * @{
 **/
#define SDIOH_DMA_TRANSFER_DONE										BIT(4)
#define SDIOH_CARD_ERROR											BIT(2)
#define SDIOH_CARD_END												BIT(1)
#define SDIOH_SD_ISR_ALL											(0x16)
/** @} */

/**
 * @defgroup SDHOST_SD_ISREN
 * @{
 **/
#define SDIOH_DMA_CTL_INT_EN										BIT(4)
#define SDIOH_CARD_ERR_INT_EN										BIT(2)
#define SDIOH_CARD_END_INT_EN										BIT(1)
#define SDIOH_WRITE_DATA											BIT(0)
/** @} */

/**
 * @defgroup SDHOST_CKGEN_CTL
 * @{
 **/
#define SDIOH_SHIFT_SD30_SAMP_CLK_SRC								12
#define SDIOH_SHIFT_SD30_PUSH_CLK_SRC								8
#define SDIOH_SHIFT_CRC_CLK_SRC										4
#define SDIOH_SD30_SAMP_CLK_VP1										(2 << SDIOH_SHIFT_SD30_SAMP_CLK_SRC)
#define SDIOH_SD30_PUSH_CLK_VP0										(1 << SDIOH_SHIFT_SD30_PUSH_CLK_SRC)
#define SDIOH_CRC_CLK_SSC											(0 << SDIOH_SHIFT_CRC_CLK_SRC)
#define SDIOH_MASK_CLKDIV											(0x7)
#define SDIOH_CLK_DIV1												0
#define SDIOH_CLK_DIV2												1
#define SDIOH_CLK_DIV4												2
#define SDIOH_CLK_DIV8												3

/** @} */

/**
 * @defgroup SDHOST_CARD_STOP
 * @{
 **/
#define SDIOH_TARGET_MODULE_SD										BIT(2)
/** @} */

/**
 * @defgroup SDHOST_CARD_SELECT
 * @{
 **/
#define SDIOH_CARD_SEL_SD_MODULE									0x2
/** @} */

/**
 * @defgroup SDHOST_CARD_EXIST
 * @{
 **/
#define SDIOH_SD_WP													BIT(5)
#define SDIOH_SD_EXIST												BIT(2)
/** @} */

/**
 * @defgroup SDHOST_CARD_INT_EN
 * @{
 **/
#define SDIOH_SDMMC_INT_EN											BIT(2)
/** @} */

/**
 * @defgroup SDHOST_CARD_INT_PEND
 * @{
 **/
#define SDIOH_SDMMC_INT_PEND										BIT(2)
/** @} */

/**
 * @defgroup SDHOST_CARD_CLK_EN_CTL
 * @{
 **/
#define SDIOH_SD_CARD_MOUDLE_EN										BIT(2)
/** @} */

/**
 * @defgroup SDHOST_SD_CONFIG1
 * @{
 **/
#define SDIOH_INITIAL_MODE											BIT(7)
#define SDIOH_CLK_DIV_BY_128										0
#define SDIOH_CLK_DIV_BY_256										BIT(6)
#define SDIOH_SD30_ASYNC_FIFO_RST_N									BIT(4)
#define SDIOH_SHIFT_MODE_SEL										2
#define SDIOH_MASK_MODE_SEL											0xc
#define SDIOH_SHIFT_BUS_WIDTH										0
#define SDIOH_MASK_BUS_WIDTH										0x3
/** @} */

/**
 * @defgroup SDHOST_SD_CONFIG2
 * @{
 **/
#define SDIOH_CRC7_CAL_EN											0
#define SDIOH_CRC7_CAL_DIS											BIT(7)
#define SDIOH_CRC16_CHK_EN											0
#define SDIOH_CRC16_CHK_DIS											BIT(6)
#define SDIOH_WAIT_WR_CRCSTA_TO_EN									0
#define SDIOH_WAIT_WR_CRCSTA_TO_DIS									BIT(5)
#define SDIOH_IGNORE_WR_CRC_ERR_EN									0
#define SDIOH_IGNORE_WR_CRC_ERR_DIS									BIT(4)
#define SDIOH_WAIT_BUSY_END_DIS										0
#define SDIOH_WAIT_BUSY_END_EN										BIT(3)
#define SDIOH_CRC7_CHK_EN											0
#define SDIOH_CRC7_CHK_DIS											BIT(2)

/** @} */

/**
 * @defgroup SDHOST_SD_CONFIG3
 * @{
 **/
#define SDIOH_STOP_STA_WAIT_BUSY_EN									0
#define SDIOH_STOP_STA_WAIT_BUSY_DIS								BIT(7)
#define SDIOH_CMD_STA_WAIT_BUSY_EN									0
#define SDIOH_CMD_STA_WAIT_BUSY_DIS									BIT(6)
#define SDIOH_DATA_PHA_WAIT_BUSY_EN									BIT(5)
#define SDIOH_DATA_PHA_WAIT_BUSY_DIS								0
#define SDIOH_SD30_CLK_STOP_EN										BIT(4)
#define SDIOH_SD30_CLK_STOP_DIS										0
#define SDIOH_SD20_CLK_STOP_EN										BIT(3)
#define SDIOH_SD20_CLK_STOP_DIS										0
#define SDIOH_SD_CMD_RESP_CHK_EN									BIT(2)
#define SDIOH_SD_CMD_RESP_CHK_DIS									0
#define SDIOH_ADDR_MODE_SECTOR										0
#define SDIOH_ADDR_MODE_BYTE										BIT(1)
#define SDIOH_CMD_RESP_TO_EN										BIT(0)
#define SDIOH_CMD_RESP_TO_DIS										0
/** @} */

/**
 * @defgroup SDHOST_SD_STATUS1_2
 * @{
 **/
#define SDIOH_SD_TUNNING_PAT_COMP_ERR								BIT(0)
#define SDIOH_GET_WRCRC_STA_TO_ERR									BIT(1)
#define SDIOH_MASK_WR_CRC_STA										0x1C
#define SDIOH_WR_CRC_ERR											BIT(5)
#define SDIOH_CRC16_ERR												BIT(6)
#define SDIOH_CRC7_ERR												BIT(7)
#define SDIOH_SD_CMD_RSP_TO_ERR										BIT(8)
#define SDIOH_SD_CMD_RSP_INVALID									BIT(9)
/** @} */

/**
 * @defgroup SDHOST_SD_SAMPLE_POINT_CTL
 * @{
 **/
#define SDIOH_SD_SAMPLE_POINT_SEL										BIT(3)
/** @} */

/**
 * @defgroup SDHOST_SD_BLOCK_CNT_L
 * @{
 **/
#define SDIOH_MASK_BLOCL_CNT_L										0xFF
/** @} */

/**
 * @defgroup SDHOST_SD_BLOCK_CNT_H
 * @{
 **/
#define SDIOH_MASK_BLOCL_CNT_H										0x7F
/** @} */

/**
 * @defgroup SDHOST_SD_TRANSFER
 * @{
 **/
#define SDIOH_START_TRANSFER										BIT(7)
#define SDIOH_TRANSFER_END											BIT(6)
#define SDIOH_SD_MODULE_FSM_IDLE									BIT(5)
#define SDIOH_ERR_OCCUR												BIT(4)
#define SDIOH_MASK_COM_CODE											0xF
/** @} */

/**
 * @defgroup SDHOST_SD_CMD_STATE
 * @{
 **/
#define SDIOH_CMD_FSM_IDLE											BIT(7)
/** @} */

/**
 * @defgroup SDHOST_SD_DATA_STATE
 * @{
 **/
#define SDIOH_DATA_FSM_IDLE											BIT(7)
/** @} */
/** @} */
/// @endcond

/* Exported Types --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Types SDHOST Exported Types
  * @{
  */

/// @cond
/**
 * @brief SDHOST_HOST Register Declaration
 */
typedef struct {
	__IO uint32_t RSVD1[256];
	__IO uint32_t SRAM_CRL;			/*!< SRAM Control Register,		Address offset: 0x00000400*/
	__IO uint32_t DMA_CRL1;			/*!< DMA Control Register 1,		Address offset: 0x00000404*/
	__IO uint32_t DMA_CRL2;			/*!< DMA Control Register 2,		Address offset: 0x00000408*/
	__IO uint32_t DMA_CRL3;			/*!< DMA Control Register 3,		Address offset: 0x0000040C*/
	__IO uint32_t SYS_LOW_PWR;		/*!< Low Power Control Register,	Address offset: 0x00000410*/
	__IO uint32_t RSVD2[4];
	__IO uint32_t SD_ISR;				/*!< SD Interrupt Status Register,Address offset: 0x00000424*/
	__IO uint32_t SD_ISREN;			/*!< SD Interrupt Enable Register,Address offset: 0x00000428*/
	__IO uint32_t RSVD3[18];
	__IO uint32_t PAD_CTL;			/*!< Pad Control Register,		Address offset: 0x00000474*/
	__IO uint32_t CKGEN_CTL;			/*!< Clock Generation Control Register,	Address offset: 0x00000478*/
	__IO uint32_t RSVD4[33];
	__IO uint16_t RSVD5;
	__IO uint8_t   CARD_DRIVER_SEL;	/*!< Card Driving Selection Register,Address offset: 0x00000502*/
	__O  uint8_t   CARD_STOP;			/*!< Stop Transfer Register,Address offset: 0x00000503*/
	__IO uint32_t RSVD6[2];
	__IO uint16_t RSVD7;
	__IO uint8_t   CARD_SELECT;		/*!< Card Type Select Register,	Address offset: 0x0000050E*/
	__IO uint8_t   DUMMY1;				/*!< Dummy Register 1,			Address offset: 0x0000050F*/
	__IO uint32_t	RSVD8[3];
	__IO uint16_t	RSVD9;
	__IO uint8_t	RSVD10;
	__I   uint8_t   CARD_EXIST;			/*!< Card Detection Register,	Address offset: 0x0000051F*/
	__IO uint8_t   CARD_INT_EN;		/*!< Card Interrupt Enable Register,Address offset: 0x00000520*/
	__IO uint8_t   CARD_INT_PEND;		/*!< Card Interrupt Status Register,Address offset: 0x00000521*/
	__IO uint16_t	RSVD11;
	__IO uint32_t	RSVD12;
	__IO uint8_t	RSVD13;
	__IO uint8_t   CARD_CLK_EN_CTL;	/*!< Card Clock Enable Control Register,Address offset: 0x00000529*/
	__IO uint16_t	RSVD14;
	__IO uint32_t	RSVD15;
	__IO uint8_t   CLK_PAD_DRIVE;		/*!< Clock Pad Driving Register,	Address offset: 0x00000530*/
	__IO uint8_t   CMD_PAD_DRIVE;		/*!< Command Pad Driving Register ,Address offset: 0x00000531*/
	__IO uint8_t   DAT_INT_PEND;		/*!< Data Pad Driving Register,	Address offset: 0x00000532*/
	__IO uint8_t	RSVD16;
	__IO uint32_t	RSVD17[19];
	__IO uint8_t   SD_CONFIG1;			/*!< SD Configuration Register 1,	Address offset: 0x00000580*/
	__IO uint8_t   SD_CONFIG2;			/*!< SD Configuration Register 2  ,Address offset: 0x00000581*/
	__IO uint8_t   SD_CONFIG3;			/*!< SD Configuration Register 3,	Address offset: 0x00000582*/
	__I   uint8_t   SD_STATUS1;			/*!< SD Status Register 1  ,		Address offset: 0x00000583*/
	__IO uint8_t   SD_STATUS2;			/*!< SD Status Register 2,		Address offset: 0x00000584*/
	__IO uint8_t   SD_BUS_STATUS;		/*!< SD Bus Status Register,	Address offset: 0x00000585*/
	__IO uint8_t	RSVD18;
	__IO uint8_t	SD_SAMPLE_POINT_CTL;		/*!< SD Sample Point Control Register,	Address offset: 0x00000587*/
	__IO uint8_t	RSVD19;
	__IO uint8_t   SD_CMD[6];			/*!< SD Command Register 0-5,	Address offset: 0x00000589-0x0000058E*/
	__IO uint8_t   SD_BYTE_CNT_L;		/*!< Byte Count Register (Low Byte),	Address offset: 0x0000058F*/
	__IO uint8_t   SD_BYTE_CNT_H;		/*!< Byte Count Register (High Byte),	Address offset: 0x00000590*/
	__IO uint8_t   SD_BLOCK_CNT_L;	/*!< Block Count Register (Low Byte),	Address offset: 0x00000591*/
	__IO uint8_t   SD_BLOCK_CNT_H;	/*!< Block Count Register (High Byte),	Address offset: 0x00000592*/
	__IO uint8_t   SD_TRANSFER;		/*!< SD Transfer Control Register,	Address offset: 0x00000593*/
	__IO uint8_t   RSVD20;
	__I   uint8_t   SD_CMD_STATE;		/*!< SD Command State Register,	Address offset: 0x00000595*/
	__I   uint8_t   SD_DATA_STATE;		/*!< SD Data State Register, 	Address offset: 0x00000596*/
} SDIOH_TypeDef;
/// @endcond

/**
  * @brief SDHOST Init Structure Definition
  */
typedef struct {
	u8 SDIOH_idle_level;	/*!< Indicate the idle pin level mask. When operate in 1-Bit bus width, this value is 0x03.
								When operate in 4-Bit bus width, this value is 0x1F. */
} SDIOH_InitTypeDef;

/**
  * @brief SDHOST DMA Control Structure Definition
  */
typedef struct {
	u32 start_addr;		/*!< Specify the DMA start address. Unit: 8 Bytes. */
	u16 blk_cnt;		/*!< Specify the DMA transfer length.  Unit: 512 Bytes). */
	u8 op;				/*!< Specify the data move direction. Should be a value of @ref SDHOST_DMA_Operation. */
	u8 type;			/*!< Specify the transfer type. Shold be a value of @ref SDHOST_DMA_Transfer_Type. */
} SDIOH_DmaCtl;

/**
  * @brief SDHOST Command Parameters Structure Definition
  */
typedef struct {
	u32 arg;			/*!< Specify the argument to be transfered with command. */
	u8 idx;				/*!< Specify the command to be transfered. */
	u8 rsp_type;		/*!< Specify the response type. Should be a value of @ref SDHOST_Card_Response_Classification. */
	u8 rsp_crc_chk;		/*!< Specify CRC7 check enable or not. Should be ENABLE or DISABLE. */
	u8 data_present;	/*!< Specify whether there is data transfer on the data line after getting response from the card. Should be a value of
    						@ref SDHOST_Data_Present */
} SDIOH_CmdTypeDef;


/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup SDHOST_Exported_Constants SDHOST Exported Constants
  * @{
  */

/** @defgroup SDHOST_Work_Mode SDHOST Work Mode
  * @{
  */
#define SDIOH_NORMAL_WRITE				0   /*!< Normal write transfer mode */
#define SDIOH_AUTO_WRITE3				1   /*!< Auto write mode 3 */
#define SDIOH_AUTO_WRITE4				2   /*!< Auto write mode 4 */
#define SDIOH_AUTO_READ3				5   /*!< Auto read mode 3 */
#define SDIOH_AUTO_READ4				6   /*!< Auto read mode 4 */
#define SDIOH_SEND_CMD_GET_RSP			8   /*!< Send command and get response mode */
#define SDIOH_AUTO_WRITE1				9   /*!< Auto write mode 1 */
#define SDIOH_AUTO_WRITE2				10   /*!< Auto write mode 2 */
#define SDIOH_NORMAL_READ				12   /*!< Normal read transfer mode */
#define SDIOH_AUTO_READ1				13   /*!< Auto read mode 1 */
#define SDIOH_AUTO_READ2				14   /*!< Auto read mode 2 */
#define SDIOH_TUNING					15   /*!< Tuning mode */
/**
  * @}
  */

/** @defgroup SDHOST_Card_Response_Type SDHOST Card Response Type
  * @{
  */
#define SDIOH_RESP_NONE					0   /*!< No response expected */
#define SDIOH_RESP_R1					1   /*!< Response type R1 */
#define SDIOH_RESP_R2					2   /*!< Response type R2 */
#define SDIOH_RESP_R3					3   /*!< Response type R3 */
#define SDIOH_RESP_R6					4   /*!< Response type R6 */
#define SDIOH_RESP_R7					5   /*!< Response type R7 */
/**
  * @}
  */

/** @defgroup SDHOST_Card_Response_Classification SDHOST Card Response Classification
  * @{
  */
#define SDIOH_NO_RESP					0   /*!< No response classification */
#define SDIOH_RSP_6B					1   /*!< 6-byte response classification */
#define SDIOH_RSP_17B					2   /*!< 17-byte response classification */
/**
  * @}
  */

/** @defgroup SDHOST_Data_Present SDHOST Data Present
  * @{
  */
#define SDIOH_NO_DATA					0   /*!< No data present after command response */
#define SDIOH_DATA_EXIST				1   /*!< Data present after command response */
/**
  * @}
  */

/** @defgroup SDHOST_DMA_Transfer_Type SDHOST DMA Transfer Type
  * @{
  */
#define SDIOH_DMA_NORMAL				0   /*!< Normal DMA transfer type */
#define SDIOH_DMA_64B					1   /*!< 64-byte DMA transfer type */
#define SDIOH_DMA_R2					2   /*!< R2 response DMA transfer type */
/**
  * @}
  */

/** @defgroup SDHOST_Mode SDHOST Mode
  * @{
  */
#define SDIOH_SD20_MODE					0   /*!< SD 2.0 compatible mode (Default Speed / High Speed, 3.3V signaling). */
#define SDIOH_DDR_MODE					1   /*!< DDR (double data rate) mode. */
#define SDIOH_SD30_MODE					2   /*!< UHS-I mode (SD 3.0 specification). */
/**
  * @}
  */

/** @defgroup SDHOST_Bus_Width SDHOST Bus Width
  * @{
  */
#define SDIOH_BUS_WIDTH_1BIT			((u8)0x00U)   /*!< 1-bit bus width. */
#define SDIOH_BUS_WIDTH_4BIT			((u8)0x01U)   /*!< 4-bit bus width. */
/**
  * @}
  */

/** @defgroup SDHOST_DMA_Operation SDHOST DMA Operation
  * @{
  */
#define SDIOH_DMA_WRITE					0   /*!< DMA write operation (host to card) */
#define SDIOH_DMA_READ					1   /*!< DMA read operation (card to host) */
/**
  * @}
  */

/** @defgroup SDHOST_Timeout_Value SDHOST Timeout Value
  * @{
  */
#define SDIOH_CMD_CPLT_TIMEOUT			5000   /*!< Maximum timeout value when checking the command complete flag, unit: us. */
#define SDIOH_XFER_CPLT_TIMEOUT			1000000   /*!< Maximum timeout value when checking the transfer complete flag, unit: us. */

#define SDIOH_READ_TIMEOUT				100000   /*!< Read operation timeout value, unit: us. */
#define SDIOH_WRITE_TIMEOUT				250000   /*!< Write operation timeout value, unit: us. */
#define SDIOH_ERASE_TIMEOUT				2000000   /*!< Erase operation timeout value, unit: us. */
/**
  * @}
  */

/** @defgroup SDHOST_CMD_Value SDHOST CMD Value
  * @{
  */
#define HOST_COMMAND					BIT(6)   /*!< Transmission bit of SD_CMD0 register; set to 1 when the host is the transmitter. */
#define SDIOH_CMD_IDX_MASK				0x3F		 /*!< Command index mask of SD_CMD0 register. */
#define SDIOH_CMD8_VHS					0x1		 /*!< VHS field value in CMD8 for the 2.7-3.6V voltage range. */
#define SDIOH_CMD8_CHK_PATN				0xAA		 /*!< Check pattern field value in CMD8. */
#define SDIOH_OCR_VDD_WIN				0xFF8000	 /*!< OCR field value in ACMD41 representing VDD voltage window bits [23:15]. */
/**
  * @}
  */

/** @defgroup SDHOST_Specific_Data_Length SDHOST Specific Data Length
  * @{
  */
#define SDIOH_C6R2_BUF_LEN				64	  	 /*!< Buffer length for CMD6 status data and R2 responses, in bytes. */
#define SDIOH_CSD_LEN					16   /*!< CSD register response length, in bytes */
/**
  * @}
  */

/** @defgroup SDHOST_Card_Response SDHOST Card Response
  * @{
  */
#define SDIO_RESP0						((u8)0x00U)   /*!< Response register 0. */
#define SDIO_RESP1						((u8)0x01U)   /*!< Response register 1. */
#define SDIO_RESP2						((u8)0x02U)   /*!< Response register 2. */
#define SDIO_RESP3						((u8)0x03U)   /*!< Response register 3. */
#define SDIO_RESP4						((u8)0x04U)   /*!< Response register 4. */
#define SDIO_RESP5						((u8)0x05U)   /*!< Response register 5. */
/**
  * @}
  */

/** @defgroup SDHOST_Signal_Level SDHOST Signal Level
  * @{
  */
#define SDIOH_SIG_VOL_33				0   /*!< Signal voltage level 3.3V */
#define SDIOH_SIG_VOL_18	 			1   /*!< Signal voltage level 1.8V */
/**
  * @}
  */

/** @defgroup SDHOST_DMA_ALIGN_ADDR SDHOST DMA ALIGN ADDR
  * @{
  */
#define SDIOH_DMA_ALIGN_SZ      (8)   /*!< DMA buffer alignment size in bytes (8-byte alignment required by hardware). */
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

/** @defgroup SDHOST_Functions SDHOST Functions
  * @{
  */
_LONG_CALL_ u32 SDIOH_Busy(void);
_LONG_CALL_ u32 SDIOH_CheckTxError(u16 *status);
_LONG_CALL_ u32 SDIOH_WaitTxDone(u32 timeout_us);
_LONG_CALL_ void SDIOH_PreDMATrans(void);
_LONG_CALL_ u32 SDIOH_WaitDMADone(u32 timeout_us);
_LONG_CALL_ u32 SDIOH_GetISR(void);
_LONG_CALL_ void SDIOH_INTConfig(u8 SDIO_IT, u32 newState);
_LONG_CALL_ void SDIOH_INTClearPendingBit(u8 SDIO_IT);
_LONG_CALL_ u32 SDIOH_CheckBusState(u8 status, u32 timeout_us);
_LONG_CALL_ u8 SDIOH_GetBusWidth(void);
_LONG_CALL_ void SDIOH_SetBusWidth(u8 width);
_LONG_CALL_ void SDIOH_DMAConfig(SDIOH_DmaCtl *dma_ctl);
_LONG_CALL_ void SDIOH_DMAReset(void);
_LONG_CALL_ u32 SDIOH_SendCommand(SDIOH_CmdTypeDef *cmd_attrib, u32 timeout_us);
_LONG_CALL_ u8 SDIOH_GetResponse(u8 byte_index);
_LONG_CALL_ void SDIOH_SwitchSpeed(u8 clk_div, u8 mode);
_LONG_CALL_ u32 SDIOH_InitialModeCmd(u8 NewState, u8 Level);
_LONG_CALL_ u32 SDIOH_Init(u8 BusBitMode);
_LONG_CALL_ void SDIOH_DeInit(void);
_LONG_CALL_ void SDIOH_DebounceCmd(u8 NewState);
_LONG_CALL_ void SDIOH_DebounceSet(u32 debouncevalue);
_LONG_CALL_ u32 SDIOH_DebounceGet(void);

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

/* Other definitions --------------------------------------------------------*/

#undef SDIOH_SUPPORT_SD30

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
