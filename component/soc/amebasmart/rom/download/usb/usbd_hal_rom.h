/**
  ******************************************************************************
  * @file    usbd_hal_rom.h
  * @author  Realsil WLAN5 Team
  * @version V1.0.0
  * @date    2020-11-23
  * @brief   This file is the header file for usbd_hal_rom.c
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2020, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USBD_HAL_ROM_H
#define USBD_HAL_ROM_H

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "usbd_def_rom.h"
#include "usbd_regs_rom.h"
#include "usbd_dbg_rom.h"

/* Exported defines ----------------------------------------------------------*/

/* USB section defines */
#define USBD_DATA_SECTION                      HAL_ROM_DATA_SECTION
#define USBD_BSS_SECTION                       HAL_ROM_BSS_SECTION_BANK
#define USBD_TEXT_SECTION                      HAL_ROM_TEXT_SECTION
#define USBD_WEEK                              __weak

/* USB buffer defines */
#define USBD_BUF_START_ADDR                    (MSP_RAM_HP_NS + 4 - 16 * 1024)

/* USB Core SW Configurations */
#define USBD_MAX_NUM_INTERFACES                1U
#define USBD_MAX_NUM_CONFIGURATION             1U
#define USBD_SELF_POWERED                      0U
#define USBD_DMA_ENABLE                        0U
#define USBD_MAX_ENDPOINTS                     4U
#define USBD_RX_FIFO_SIZE                      512U
#define USBD_NPTX_FIFO_SIZE                    256U
#define USBD_PTX_FIFO_SIZE                     64U

/* USB Core HW Configurations */
#define USBD_IN_TOKEN_QUEUE_DEPTH              8U

/* USB Core ID */
#define USB_OTG_CORE_ID_300A                   0x4F54300AU
#define USB_OTG_CORE_ID_310A                   0x4F54310AU

/* USB Core Mode */
#define USB_OTG_MODE_DEVICE                    0U
#define USB_OTG_MODE_HOST                      1U
#define USB_OTG_MODE_DRD                       2U

/* USB Core Speed */
#define USB_OTG_SPEED_HIGH                     0U
#define USB_OTG_SPEED_HIGH_IN_FULL             1U
#define USB_OTG_SPEED_FULL                     3U

/* USB Core Turnaround Timeout Value */
#ifndef USBD_HS_TRDT_VALUE
#define USBD_HS_TRDT_VALUE                     5U
#endif /* USBD_HS_TRDT_VALUE */
#ifndef USBD_FS_TRDT_VALUE
#define USBD_FS_TRDT_VALUE                     9U
#endif /* USBD_HS_TRDT_VALUE */

/* USB Core MPS */
#define USB_OTG_HS_MAX_PACKET_SIZE             512U
#define USB_OTG_FS_MAX_PACKET_SIZE             64U
#define USB_OTG_MAX_EP0_SIZE                   64U

/* USB Core PHY Frequency */
#define DSTS_ENUMSPD_HS_PHY_30MHZ_OR_60MHZ     (0U << 1)
#define DSTS_ENUMSPD_FS_PHY_30MHZ_OR_60MHZ     (1U << 1)
#define DSTS_ENUMSPD_LS_PHY_6MHZ               (2U << 1)
#define DSTS_ENUMSPD_FS_PHY_48MHZ              (3U << 1)

/* USB Core Frame Interval */
#define DCFG_FRAME_INTERVAL_80                 0U
#define DCFG_FRAME_INTERVAL_85                 1U
#define DCFG_FRAME_INTERVAL_90                 2U
#define DCFG_FRAME_INTERVAL_95                 3U

/* USB Core EP0 MPS */
#define DEP0CTL_MPS_64                         0U
#define DEP0CTL_MPS_32                         1U
#define DEP0CTL_MPS_16                         2U
#define DEP0CTL_MPS_8                          3U

/* USB EP Speed */
#define EP_SPEED_LOW                           0U
#define EP_SPEED_FULL                          1U
#define EP_SPEED_HIGH                          2U

/* USB EP Type */
#define EP_TYPE_CTRL                           0U
#define EP_TYPE_ISOC                           1U
#define EP_TYPE_BULK                           2U
#define EP_TYPE_INTR                           3U
#define EP_TYPE_MSK                            3U

/* USB EP Address Mask */
#define EP_ADDR_MSK                            0xFU

/* USB STS Defines */
#define STS_GOUT_NAK                           1U
#define STS_DATA_UPDT                          2U
#define STS_XFER_COMP                          3U
#define STS_SETUP_COMP                         4U
#define STS_SETUP_UPDT                         6U

#define GRXSTS_PKTSTS_IN                       2U
#define GRXSTS_PKTSTS_IN_XFER_COMP             3U
#define GRXSTS_PKTSTS_DATA_TOGGLE_ERR          5U
#define GRXSTS_PKTSTS_CH_HALTED                7U

/* Exported types ------------------------------------------------------------*/

/* HAL lock definition */
typedef enum {
	USB_HAL_UNLOCKED = 0x00U,
	USB_HAL_LOCKED   = 0x01U
} USB_LockTypeDef;

/* USB configuration structure */
typedef struct {
	u32 dev_endpoints;           /*!< Device Endpoints number.
                                         This parameter depends on the used USB core.
                                         This parameter must be a number between Min_Data = 1 and Max_Data = 15 */
	u32 speed;                   /*!< USB Core speed.
                                         This parameter can be any value of @ref USB_Core_Speed_                */

	u32 dma_enable;              /*!< Enable or disable of the USB embedded DMA used only for OTG HS.        */

	u32 ep0_mps;                 /*!< Set the Endpoint 0 Max Packet size.                                    */
} USB_CfgTypeDef;

/* USB endpoint structure */
typedef struct {
	u8  num;                  /*!< Endpoint number
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 15   */

	u8  is_in;                /*!< Endpoint direction
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

	u8  is_stall;             /*!< Endpoint stall condition
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

	u8  type;                 /*!< Endpoint type
                                       This parameter can be any value of @ref USB_EP_Type_                     */

	u8  data_pid_start;       /*!< Initial data PID
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

	u8  even_odd_frame;       /*!< IFrame parity
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 1    */

	u16 tx_fifo_num;          /*!< Transmission FIFO number
                                       This parameter must be a number between Min_Data = 1 and Max_Data = 15   */

	u32 maxpacket;            /*!< Endpoint Max packet size
                                       This parameter must be a number between Min_Data = 0 and Max_Data = 64KB */

	u8  *xfer_buff;           /*!< Pointer to transfer buffer                                               */

	u32 dma_addr;             /*!< 32 bits aligned transfer buffer address                                  */

	u32 xfer_len;             /*!< Current transfer length                                                  */

	u32 xfer_count;           /*!< Partial transfer length in case of multi packet transfer                 */

	u8  is_zlp;               /*!< ZLP packet */
} USB_EPTypeDef;

/* USB device LPM suspend state */
typedef enum {
	LPM_L0 = 0x00, /* on */
	LPM_L1 = 0x01, /* LPM L1 sleep */
	LPM_L2 = 0x02, /* suspend */
	LPM_L3 = 0x03, /* off */
} USB_LPM_StateTypeDef;

/* PCD state */
typedef enum {
	HAL_PCD_STATE_RESET   = 0x00,
	HAL_PCD_STATE_READY   = 0x01,
	HAL_PCD_STATE_ERROR   = 0x02,
	HAL_PCD_STATE_BUSY    = 0x03,
	HAL_PCD_STATE_TIMEOUT = 0x04
} USB_PCD_StateTypeDef;

/* PCD handle structure */
typedef struct {
	/*!< PCD required parameters  */
	USB_CfgTypeDef Init;
	/*!< USB Address */
	__IO u8 USB_Address;
	/*!< IN endpoint parameters */
	USB_EPTypeDef IN_ep[USBD_MAX_ENDPOINTS];
	/*!< OUT endpoint parameters */
	USB_EPTypeDef OUT_ep[USBD_MAX_ENDPOINTS];
	/*!< PCD peripheral status */
	USB_LockTypeDef Lock;
	/*!< PCD communication state */
	__IO USB_PCD_StateTypeDef State;
	/*!< PCD Error code */
	__IO  u32 ErrorCode;
	/*!< Setup packet buffer */
	u32 Setup[12];
	/*!< LPM State */
	USB_LPM_StateTypeDef LPM_State;
	/*!< PHY initialized */
	u8 PHY_Initialized;
	/*!< ISR installed */
	u8 ISR_Installed;
	/*!< IN EP sequence from In token queue */
	u8 in_ep_sequece[USBD_MAX_ENDPOINTS];
	/*!< Pointer to upper stack Handler */
	void *pData;
} USB_PCD_HandleTypeDef;

/* USB device speed */
typedef enum {
	USBD_SPEED_HIGH  = 0U,
	USBD_SPEED_FULL  = 1U,
	USBD_SPEED_LOW   = 2U,
} USBD_SpeedTypeDef;

struct _USBD_HandleTypeDef;

/* USB class structure */
typedef struct {
	u8(*Init)(struct _USBD_HandleTypeDef *pdev, u8 cfgidx);
	u8(*DeInit)(struct _USBD_HandleTypeDef *pdev, u8 cfgidx);
	/* Control Endpoints*/
	u8(*Setup)(struct _USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef  *req);
	u8(*EP0_TxSent)(struct _USBD_HandleTypeDef *pdev);
	u8(*EP0_RxReady)(struct _USBD_HandleTypeDef *pdev);
	/* Class Specific Endpoints*/
	u8(*DataIn)(struct _USBD_HandleTypeDef *pdev, u8 epnum);
	u8(*DataOut)(struct _USBD_HandleTypeDef *pdev, u8 epnum);
	u8(*SOF)(struct _USBD_HandleTypeDef *pdev);

	u8 *(*GetDeviceDescriptor)(USBD_SpeedTypeDef speed, u16 *length);
	u8 *(*GetLangIDStrDescriptor)(USBD_SpeedTypeDef speed, u16 *length);
	u8 *(*GetManufacturerStrDescriptor)(USBD_SpeedTypeDef speed, u16 *length);
	u8 *(*GetProductStrDescriptor)(USBD_SpeedTypeDef speed, u16 *length);
	u8 *(*GetSerialStrDescriptor)(USBD_SpeedTypeDef speed, u16 *length);
	u8 *(*GetHSConfigDescriptor)(u16 *length);
	u8 *(*GetFSConfigDescriptor)(u16 *length);
	u8 *(*GetOtherSpeedConfigDescriptor)(u16 *length);
	u8 *(*GetDeviceQualifierDescriptor)(u16 *length);
} USBD_ClassTypeDef;

/* USB device handle structure */
typedef struct {
	u32 status;
	u32 is_used;
	u32 total_length;
	u32 rem_length;
	u32 maxpacket;
} USBD_EndpointTypeDef;

/* USB device handle structure */
typedef struct _USBD_HandleTypeDef {
	u32 dev_config;
	u32 dev_default_config;
	u32 dev_config_status;
	USBD_SpeedTypeDef       dev_speed;
	USBD_EndpointTypeDef    ep_in[USBD_MAX_ENDPOINTS];
	USBD_EndpointTypeDef    ep_out[USBD_MAX_ENDPOINTS];
	u32 ep0_state;
	u32 ep0_data_len;
	u8 dev_state;
	u8 dev_old_state;
	u8 dev_address;
	u8 dev_connection_status;
	u8 dev_test_mode;
	u32 dev_remote_wakeup;

	USBD_SetupReqTypedef    request;
	USBD_ClassTypeDef       *pClass;
	void                    *pClassData;
	void                    *pUserData;
	void                    *pData;
} USBD_HandleTypeDef;

/* Unaligned 32 bit type for DFIFO usage */
typedef struct {
	u32 data __PACKED;
} USB_Unaligned32TypeDef;

/* Exported macros -----------------------------------------------------------*/

/* USB registers */
#define USB_GLOBAL     ((USB_OTG_GlobalTypeDef *)(USB_OTG_REG_BASE))
#define USB_PCGCCTL    *(__IO u32 *)(USB_OTG_REG_BASE + USB_OTG_PCGCCTL_BASE)
#define USB_DEVICE     ((USB_OTG_DeviceTypeDef *)(USB_OTG_REG_BASE + USB_OTG_DEVICE_BASE))
#define USB_INEP(i)    ((USB_OTG_INEndpointTypeDef *)(USB_OTG_REG_BASE + USB_OTG_IN_ENDPOINT_BASE + ((i) * USB_OTG_EP_REG_SIZE)))
#define USB_OUTEP(i)   ((USB_OTG_OUTEndpointTypeDef *)(USB_OTG_REG_BASE + USB_OTG_OUT_ENDPOINT_BASE + ((i) * USB_OTG_EP_REG_SIZE)))
#define USB_DFIFO(i)   *(__IO u32 *)(USB_OTG_REG_BASE + USB_OTG_FIFO_BASE + ((i) * USB_OTG_FIFO_SIZE))

/* Delay macros */
#define USB_DELAY_MS				DelayMs
#define USB_DELAY_US				DelayUs

/* Register access macros */
#define USB_READ32(addr)           (HAL_READ32(0, (addr)))
#define USB_WRITE32(addr, value)   (HAL_WRITE32(0, (addr), (value)))

/* Lock macros */
#define USB_HAL_LOCK(__HANDLE__)                                                   \
									do{ 									       \
										if((__HANDLE__)->Lock == USB_HAL_LOCKED)   \
										{									       \
										   return HAL_BUSY; 				       \
										}									       \
										else								       \
										{									       \
										   (__HANDLE__)->Lock = USB_HAL_LOCKED;    \
										}									       \
									  }while (0U)

#define USB_HAL_UNLOCK(__HANDLE__)                                                 \
									  do{									       \
										  (__HANDLE__)->Lock = USB_HAL_UNLOCKED;   \
										}while (0U)

/* Macro to get variable aligned on 32-bytes */
/* In HS mode and when the DMA is used, all variables and data structures dealing
   with the DMA during the transaction process should be 32-bytes aligned */
#if defined ( __GNUC__ ) /* GNU Compiler */
#ifndef __ALIGN_END
#define __ALIGN_END    __attribute__ ((aligned (32)))
#endif /* __ALIGN_END */
#ifndef __ALIGN_BEGIN
#define __ALIGN_BEGIN
#endif /* __ALIGN_BEGIN */
#else
#ifndef __ALIGN_END
#define __ALIGN_END
#endif /* __ALIGN_END */
#ifndef __ALIGN_BEGIN
#endif /* __ALIGN_BEGIN */
#endif /* __GNUC__ */

#ifndef SWAPBYTE
#define  SWAPBYTE(addr)        (((u16)(*((u8 *)(addr)))) + \
								   (((u16)(*(((u8 *)(addr)) + 1U))) << 8U))
#endif

#ifndef LOBYTE
#define LOBYTE(x)  ((u8)(x & 0x00FFU))
#endif

#ifndef HIBYTE
#define HIBYTE(x)  ((u8)((x & 0xFF00U) >> 8U))
#endif

#ifndef MIN
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#endif

/* Exported variables --------------------------------------------------------*/

extern USBD_HandleTypeDef USBD_Device;
extern USBD_ClassTypeDef USBD_CDC;

/* Exported functions --------------------------------------------------------*/

HAL_Status USB_HalInit(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_DevInit(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_EnableGlobalInt(void);
HAL_Status USB_DisableGlobalInt(void);
HAL_Status USB_SetTurnaroundTime(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_SetDeviceMode(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_SetDevSpeed(USB_PCD_HandleTypeDef *hpcd, u8 speed);
HAL_Status USB_FlushRxFifo(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_FlushTxFifo(USB_PCD_HandleTypeDef *hpcd, u32 num);
HAL_Status USB_ActivateEndpoint(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
HAL_Status USB_DeactivateEndpoint(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
HAL_Status USB_EPStartXfer(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
HAL_Status USB_EP0StartXfer(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
HAL_Status USB_WritePacket(USB_PCD_HandleTypeDef *hpcd, u8 *src, u8 ch_ep_num, u16 len);
void *USB_ReadPacket(USB_PCD_HandleTypeDef *hpcd, u8 *dest, u16 len);
HAL_Status USB_EPSetStall(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
HAL_Status USB_EPClearStall(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
HAL_Status USB_SetDevAddress(USB_PCD_HandleTypeDef *hpcd, u8 address);
HAL_Status USB_DevConnect(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_DevDisconnect(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_StopDevice(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_ActivateSetup(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_EP0_OutStart(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_ResetLearnQueue(USB_PCD_HandleTypeDef *hpcd);
HAL_Status USB_EPPredictNextEp(USB_PCD_HandleTypeDef *hpcd, USB_EPTypeDef *ep);
u8 USB_GetDevSpeed(USB_PCD_HandleTypeDef *hpcd);
u32 USB_GetMode(USB_PCD_HandleTypeDef *hpcd);
u32 USB_ReadInterrupts(void);
u32 USB_ReadDevAllOutEpInterrupt(USB_PCD_HandleTypeDef *hpcd);
u32 USB_ReadDevOutEPInterrupt(USB_PCD_HandleTypeDef *hpcd, u8 epnum);
u32 USB_ReadDevAllInEpInterrupt(USB_PCD_HandleTypeDef *hpcd);
u32 USB_ReadDevInEPInterrupt(USB_PCD_HandleTypeDef *hpcd, u8 epnum);
void USB_ClearInterrupts(USB_PCD_HandleTypeDef *hpcd, u32 interrupt);

#endif /* USBD_HAL_ROM_H */

