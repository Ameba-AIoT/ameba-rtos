/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */
#include "usbd_hal.h"
#include "usbd_fullmac.h"
#include "usbd_fullmac_class.h"
#include "usbd_core.h"

/* Private defines -----------------------------------------------------------*/

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
#define USBD_SPEED USB_SPEED_FULL
#else
#define USBD_SPEED USB_SPEED_HIGH
#endif

#define UBURN_REQ_FW_DOWNLOAD			0xF0U

#define UBURN_PKT_TYPE_QUERY			0x01U
#define UBURN_PKT_TYPE_WRITE			0x02U
#define UBURN_PKT_TYPE_CHKSUM				0x03U
#define UBURN_PKT_TYPE_JUMP				0x04U
#define UBURN_PKT_TYPE_ABORT			0x05U
#define UBURN_PKT_TYPE_ERASE			0x06U
#define UBURN_PKT_TYPE_CHK_HASH			0x07U
#define UBURN_PKT_TYPE_QUERY_ACK		0x81U
#define UBURN_PKT_TYPE_WRITE_ACK		0x82U
#define UBURN_PKT_TYPE_CHKSUM_ACK			0x83U
#define UBURN_PKT_TYPE_JUMP_ACK			0x84U
#define UBURN_PKT_TYPE_ABORT_ACK		0x85U
#define UBURN_PKT_TYPE_ERASE_ACK		0x86U
#define UBURN_PKT_TYPE_CHK_HASH_ACK		0x87U


#define UBURN_FW_TYPE_ROM				0xF0U
#define UBURN_FW_TYPE_BOOTLOADER		0xF1U
#define UBURN_FW_TYPE_APPLICATION  		0xF2U

#define UBURN_CFG_WRITE_CHK_EN			0x01U

/* Private types -------------------------------------------------------------*/
typedef struct {
	__IO u8 Aborted;
	__IO u32 AbortedDelayUs;
	__IO u8 JumpToRam;
	__IO u32 JumpToRamDelayUs;
} Fullmac_CtrlTypeDef;

typedef struct {
	// DWORD 0
	u32	DataLen: 16;	// Data length
	u32	DataOffset: 8;	// Data offset i.e. header length
	u32	Config: 8;		// Configuration

	// DWORD 1
	u32	PacketType: 8;	// Packet type
	u32	XferStatus: 8;	// Transfer status
	u32	RlVersion: 8;	// RL version
	u32	DevMode: 8;		// Device mode

	// DWORD 2
	u32	MemAddr;		// Memory address

	// DWORD 3
	u32	MemSize;		// Memory size

	// DWORD 4
	u32 Value;			// Value

	// DWORD 5
	u32	Reserved;		// Reserved
} Fullmac_PacketDescDef;

#define	UBURN_PKT_DESC_SIZE		(sizeof(Fullmac_PacketDescDef))

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u8 USBD_Itf_Init(void);
static u8 USBD_Itf_DeInit(void);
static u8 USBD_Itf_Setup(USBD_SetupReqTypedef *req, u8 *pbuf);
static u8 USBD_Itf_Receive(USBD_Class_HandleTypeDef *handle);
static u8 USBD_Itf_RxProcess(USBD_Class_HandleTypeDef *handle, u8 *buf, u32 len);
static u8 USBD_Itf_Transmit(u8 *buf, u16 len);
/* Private variables ---------------------------------------------------------*/

static USBD_ClassCallbackTypeDef USBD_fops = {
	USBD_Itf_Init,
	USBD_Itf_DeInit,
	USBD_Itf_Setup,
	USBD_Itf_Receive
};

static Fullmac_CtrlTypeDef USBD_Ctrl;
static const char *const TAG = "USB";
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Init callback for INIC class
  * @param  void
  * @retval Status
  */
static u8 USBD_Itf_Init(void)
{
	return HAL_OK;
}

/**
  * @brief  DeInit callback for INIC class
  * @param  void
  * @retval Status
  */
static u8 USBD_Itf_DeInit(void)
{
	Fullmac_CtrlTypeDef *ctrl = &USBD_Ctrl;

	ctrl->Aborted = 0;
	ctrl->JumpToRam = 0;

	return HAL_OK;
}

/**
  * @brief  Transmit data to USB host
  * @param  buf: Data buffer
  * @param  lem: Data length to be transmitted
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW or HAL_BUSY
  */
static u8 USBD_Itf_Transmit(u8 *buf, u16 len)
{
	return USBD_Class_Transmit(buf, len);
}

/**
  * @brief  Receive callback for INIC class
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Status
  */
static u8 USBD_Itf_Receive(USBD_Class_HandleTypeDef *handle)
{
	return USBD_Itf_RxProcess(handle, handle->RxBuffer, handle->RxLength);
}

/**
  * @brief  Write image frame
  * @param  dest : Destination
  * @param  src  : Frame buffer
  * @param  len  : Frame length
  * @retval None
  */
static u8 USBD_Itf_WriteImage(u32 dest, const u8 *src,  u32 len)
{
	u32 index = 0;

	/* Only allow to write hs sram, to avoid attack */
	if (!RANGE_IS_FULLMAC(dest, len - 1)) {
		return HAL_ERR_PARA;
	}

	for (index = 0; index < len; index += 4) {
		*((u32 *)(dest + index)) = (*((u32 *)(src + index)));
	}

	DCache_CleanInvalidate(dest, len);
	return HAL_OK;
}

/**
  * @brief  Calculate checksum
  * @param  addr : Memory address in byte
  * @param  size : Memory size in byte
  * @retval Error code
  */
static u8 USBD_Itf_CalculateChecksum(u32 addr, u32 size, u32 *result)
{
	u32 checksum = 0;
	u32 i;
	u32 *buf = (u32 *)addr;

	/* Only allow to read hs sram, to avoid attack */
	if (!RANGE_IS_FULLMAC(addr, size - 1)) {
		return HAL_ERR_PARA;
	}

	for (i = 0; i < size / 4; ++i) {
		checksum ^= buf[i];
		if ((i % 100000) == 0) {
			WDG_Refresh(IWDG_DEV);
		}
	}

	*result = checksum;

	return HAL_OK;
}

static u8 USBD_Itf_Erase(u32 addr, u32 size)
{
	UNUSED(addr);
	UNUSED(size);
	return HAL_OK;
}

static u8 USBD_Itf_Check_hash(u32 addr, u32 size)
{
	UNUSED(addr);
	UNUSED(size);
	return HAL_OK;
}
/**
  * @brief  Setup callback for INIC class
  * @param  cmd: Command code
  * @param  buf: Buffer containing command data (request parameters)
  * @param  len: Number of data to be sent (in bytes)
  * @retval Status
  */
static u8 USBD_Itf_Setup(USBD_SetupReqTypedef *req, u8 *pbuf)
{
	u8 ret = HAL_ERR_PARA;
	u32 len = req->wLength;

	Fullmac_PacketDescDef *txDesc;

	if (req->bRequest != UBURN_REQ_FW_DOWNLOAD) {
		RTK_LOGW(TAG, "Invalid bmRequest=0x%02X bRequest=0x%02X wLength=0x%04X\n", req->bmRequest, req->bRequest, req->wLength);
		return HAL_ERR_PARA;
	}

	switch (req->wIndex) {
	case UBURN_PKT_TYPE_QUERY:
		if ((len == UBURN_PKT_DESC_SIZE) && (pbuf != NULL)) {
			_memset((void *)pbuf, 0, UBURN_PKT_DESC_SIZE);
			txDesc = (Fullmac_PacketDescDef *)pbuf;
			txDesc->DataLen = 0;
			txDesc->DataOffset = UBURN_PKT_DESC_SIZE;
			txDesc->PacketType = UBURN_PKT_TYPE_QUERY_ACK;
			txDesc->XferStatus = HAL_OK;
			txDesc->RlVersion = (u8)(EFUSE_GetChipVersion() & 0xFF);
			txDesc->DevMode = UBURN_FW_TYPE_BOOTLOADER;
			ret = HAL_OK;
		} else {
			RTK_LOGW(TAG, "QUERY: protocol err\n");
		}
		break;
	default:
		RTK_LOGW(TAG, "Invalid request index 0x%02X\n", req->wIndex);
		break;
	}
	return ret;
}

/**
  * @brief  RX data process
  * @param  buf: RX buffer
  * @param  len: RX data length (in bytes)
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
static u8 USBD_Itf_RxProcess(USBD_Class_HandleTypeDef *handle, u8 *buf, u32 len)
{
	u8 ret = HAL_ERR_HW;
	u32 checksum32 = 0;
	u32 i;
	u8 *data;
	u8 needCheck;
	u8 descLen;

	Fullmac_CtrlTypeDef *ctrl = &USBD_Ctrl;
	Fullmac_PacketDescDef *txDesc = (Fullmac_PacketDescDef *)(handle->TxBuffer);
	Fullmac_PacketDescDef *rxDesc;

	_memset((void *)txDesc, 0, UBURN_PKT_DESC_SIZE);
	txDesc->DataOffset = UBURN_PKT_DESC_SIZE;
	txDesc->PacketType = UBURN_PKT_TYPE_QUERY_ACK;
	txDesc->XferStatus = HAL_OK;
	txDesc->RlVersion = (u8)(EFUSE_GetChipVersion() & 0xFF);
	txDesc->DevMode = UBURN_FW_TYPE_BOOTLOADER;

	if ((buf == NULL) || (len < UBURN_PKT_DESC_SIZE)) {
		RTK_LOGW(TAG, "Invalid BULK OUT packet\n");
		txDesc->XferStatus = HAL_ERR_PARA;
		USBD_Itf_Transmit(handle->TxBuffer, UBURN_PKT_DESC_SIZE);
		return HAL_ERR_PARA;
	}

	rxDesc = (Fullmac_PacketDescDef *)buf;
	descLen = rxDesc->DataOffset;

	switch (rxDesc->PacketType) {
	case UBURN_PKT_TYPE_WRITE:
		if ((descLen == UBURN_PKT_DESC_SIZE) && (rxDesc->DataLen + descLen == len) && (rxDesc->DataLen > 0)) {
			txDesc->PacketType = UBURN_PKT_TYPE_WRITE_ACK;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->DataLen;
			data = buf + UBURN_PKT_DESC_SIZE;
			needCheck = rxDesc->Config & UBURN_CFG_WRITE_CHK_EN;
			if (needCheck) {
				for (i = 0; i < rxDesc->DataLen; i += 4) {
					checksum32 ^= *(u32 *)(data + i);
				}
			}
			if ((needCheck != 0U) && (checksum32 != rxDesc->Value)) {
				RTK_LOGE(TAG, "WRITE: addr 0x%08x len %d checksum err 0x%08x, exepcted 0x%08x\n",
						 rxDesc->MemAddr, rxDesc->DataLen, checksum32, rxDesc->Value);
				txDesc->XferStatus = HAL_ERR_PARA;
			} else { // ((needCheck == 0U) || ((needCheck != 0U) && (checksum32 == rxDesc->Value)))
				ret = USBD_Itf_WriteImage(rxDesc->MemAddr, data, rxDesc->DataLen);
				if (ret == HAL_OK) {
					RTK_LOGD(TAG, "WRITE: addr 0x%08x len %d OK\n", rxDesc->MemAddr, rxDesc->DataLen);
				} else {
					RTK_LOGE(TAG, "WRITE: addr 0x%08x len %d err %d\n", rxDesc->MemAddr, rxDesc->DataLen, ret);
				}
				txDesc->XferStatus = ret;
			}
		} else {
			RTK_LOGW(TAG, "WRITE: Invalid packet length\n");
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case UBURN_PKT_TYPE_CHKSUM:
		if (descLen == UBURN_PKT_DESC_SIZE) {
			txDesc->PacketType = UBURN_PKT_TYPE_CHKSUM_ACK;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->MemSize;
			RTK_LOGD(TAG, "CHK: addr 0x%08x size %d\n", rxDesc->MemAddr, rxDesc->MemSize);
			ret = USBD_Itf_CalculateChecksum(rxDesc->MemAddr, rxDesc->MemSize, &checksum32);
			if (ret == HAL_OK) {
				txDesc->Value = checksum32;
			}
			txDesc->XferStatus = ret;

			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case UBURN_PKT_TYPE_JUMP:
		txDesc->PacketType = UBURN_PKT_TYPE_JUMP_ACK;
		if (descLen == UBURN_PKT_DESC_SIZE) {
			RTK_LOGI(TAG, "JUMP to RAM\n");
			txDesc->XferStatus = HAL_OK;
			ctrl->JumpToRam = 1;
			ctrl->JumpToRamDelayUs = rxDesc->Value;
			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
			RTK_LOGW(TAG, "JUMP: Invalid packet length\n");
		}
		break;
	case UBURN_PKT_TYPE_ABORT:
		txDesc->PacketType = UBURN_PKT_TYPE_ABORT_ACK;
		if (descLen == UBURN_PKT_DESC_SIZE) {
			RTK_LOGW(TAG, "ABORT\n");
			ctrl->Aborted = 1;
			ctrl->AbortedDelayUs = rxDesc->Value;
			txDesc->XferStatus = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
			RTK_LOGW(TAG, "ABORT: Invalid packet length\n");
		}
		break;
	case UBURN_PKT_TYPE_ERASE:
		if (descLen == UBURN_PKT_DESC_SIZE) {
			txDesc->PacketType = UBURN_PKT_TYPE_ERASE_ACK;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->MemSize;
			RTK_LOGD(TAG, "ERASE: addr 0x%08x size %d\n", rxDesc->MemAddr, rxDesc->MemSize);
			ret = USBD_Itf_Erase(rxDesc->MemAddr, rxDesc->MemSize);
			txDesc->XferStatus = ret;
			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case UBURN_PKT_TYPE_CHK_HASH:
		if (descLen == UBURN_PKT_DESC_SIZE) {
			txDesc->PacketType = UBURN_PKT_TYPE_CHK_HASH_ACK;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->MemSize;
			RTK_LOGD(TAG, "CHK_HASH: addr 0x%08x size %d\n", rxDesc->MemAddr, rxDesc->MemSize);
			ret = USBD_Itf_Check_hash(rxDesc->MemAddr, rxDesc->MemSize);
			if (ret == HAL_OK) {
				txDesc->Value = 0;//TODO: Flash image hash data
			}
			txDesc->XferStatus = ret;
			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	default:
		txDesc->PacketType = UBURN_PKT_TYPE_QUERY_ACK;
		txDesc->XferStatus = HAL_ERR_PARA;
		break;
	}

	USBD_Itf_Transmit(handle->TxBuffer, UBURN_PKT_DESC_SIZE);

	return HAL_OK;
}
/* Exported functions --------------------------------------------------------*/

/**
  * @brief  USB download process
  * @param  None
  * @retval Result of the operation: 1 if Flashloader STX done, 0 if aborted
  */
int Fullmac_USBD_Process(void)
{
	int ret = HAL_ERR_UNKNOWN;
	u32 cnt = 0;
	Fullmac_CtrlTypeDef *ctrl = &USBD_Ctrl;

	while (1) {
		if (++cnt >= 100) {
			cnt = 0;
			WDG_Refresh(IWDG_DEV);
		}
		if (ctrl->JumpToRam) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Done\n");
			USB_DELAY_US(ctrl->JumpToRamDelayUs); // Wait transfer done, set this value as per USB host features
			ret = HAL_OK;
			break;
		}
		if (USBD_Ctrl.Aborted) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Aborted\n");
			USB_DELAY_US(ctrl->AbortedDelayUs);	// Wait transfer done, set this value as per USB host features
			ret = HAL_ERR_PARA;
			break;
		}
		USB_DELAY_US(10000);
	}

	return ret;
}


/**
  * @brief  USB download interface initialize
  * @param  void
  * @retval Status
  */
int Fullmac_USBD_Init(void)
{
	int ret;

	ret = USBD_Core_Init(&USBD_FULLMAC, &USBD_fops, USBD_SPEED, (void *)&USBD_Class_Handle);
	if (ret != HAL_OK) {
		RTK_LOGW(NOTAG, "USB init fail: %d\n", ret);
	}

	return ret;
}

/**
  * @brief  USB download interface de-initialize
  * @param  None
  * @retval Status
  */
int Fullmac_USBD_DeInit(void)
{
	int ret;

	ret = USBD_Core_DeInit();
	if (ret != HAL_OK) {
		RTK_LOGE(NOTAG, "Deinit fail: %d\n", ret);
	}

	return ret;
}

