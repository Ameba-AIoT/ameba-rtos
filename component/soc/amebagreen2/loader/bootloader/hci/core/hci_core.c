/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#define COMPIL_LOG_LEVEL                RTK_LOG_DEBUG

#include "hci_core.h"
#include "ameba_nor_ftl.h"
#if defined(CONFIG_WHC_INTF_USB)
#include "usbd_intf.h"
#elif defined(CONFIG_WHC_INTF_SDIO)
#include "sdio_intf.h"
#endif
/* Private defines -----------------------------------------------------------*/

#define HCI_FLASH_PAGE_SIZE 1024U
#define HCI_SRAM_SIZE_IN_KB	512U

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "HCI";

HCI_AdapterTypeDef HCI_Adapter;
IMAGE_HEADER EmptyImgHdr;
/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Write image frame
  * @param  addr : Destination address
  * @param  src  : Frame buffer
  * @param  size : Frame size
  * @retval Status
  */
static int HCI_WriteImage(u32 addr, const u8 *src, u32 size)
{
	u32 i;

	/* Host Send empty img header with addr == 0xFFFFFFFF */
	if ((addr == 0xFFFFFFFF) && (size == IMAGE_HEADER_LEN)) {
		addr = (u32)&EmptyImgHdr;
	}

	if (IS_FLASH_ADDR(addr)) {
		if (!IS_FLASH_ADDR(addr + size - 1)) {
			return HAL_ERR_PARA;
		}

		if ((size % HCI_FLASH_PAGE_SIZE) != 0) {
			return HAL_ERR_PARA;
		}

		for (i = 0; i < size; i += HCI_FLASH_PAGE_SIZE) {
			nor_ftl_write_page(addr + i, src + i);
		}
	} else {
		/* Only allow to write hs sram, to avoid attack */
#ifdef HCI_TARGET_ADDR_CHECK_TODO
		if (!RANGE_IS_FULLMAC(addr, size - 1)) {
			return HAL_ERR_PARA;
		}
#endif

		for (i = 0; i < size; i += 4) {
			*((u32 *)(addr + i)) = (*((u32 *)(src + i)));
		}

		DCache_CleanInvalidate(addr, size);
	}
	return HAL_OK;
}

/**
  * @brief  Calculate checksum
  * @param  addr : Start address
  * @param  size : Calculate size in byte
  * @param  result : Calculate result
  * @retval Status
  */
static int HCI_CalculateChecksum(u32 addr, u32 size, u32 *result)
{
	u32 checksum = 0;
	u32 data;
	u32 i;

	/* Host Send empty img header with addr == 0xFFFFFFFF */
	if ((addr == 0xFFFFFFFF) && (size == IMAGE_HEADER_LEN)) {
		addr = (u32)&EmptyImgHdr;
	}

	if (IS_FLASH_ADDR(addr)) {
		if (!IS_FLASH_ADDR(addr + size - 1)) {
			return HAL_ERR_PARA;
		}
	} else {
#ifdef HCI_TARGET_ADDR_CHECK_TODO
		/* Only allow to read hs sram, to avoid attack */
		if (!RANGE_IS_FULLMAC(addr, size - 1)) {
			return HAL_ERR_PARA;
		}
#endif
	}

	for (i = 0; i < size; i += 4) {
		/* performance: km4 spic read 32bytes cost 1.095us with 1 cache line @4IO/100M
			8*HAL_READ32 cost 1.095us *4(max) * 5 = 20us @1IO/20M
		*/
		data = HAL_READ32(addr, i);
		checksum ^= data;
		if ((i % 25000) == 0) {
			/* 500*1000*8/20us =200000cycles，refresh per 500ms normally
			if cache miss, 200000/8=25000cycle */
			WDG_Refresh(IWDG_DEV);
		}
	}

	*result = checksum;

	return HAL_OK;
}

/**
  * @brief  Erase flash
  * @param  addr : Start address
  * @param  size : Erase size in byte, shall be aligned to 4KB
  * @retval Status
  */
static int HCI_EraseFlash(u32 addr, u32 size)
{
	if (IS_FLASH_ADDR(addr)) {
		if (!IS_FLASH_ADDR(addr + size - 1)) {
			return HAL_ERR_PARA;
		}
	}

	nor_ftl_erase(addr, size);

	return HAL_OK;
}

/**
  * @brief  Calculate flash hash
  * @param  addr : Start address
  * @param  size : Calculate size in byte
  * @param  result : Calculate result
  * @retval Status
  */
static int HCI_CalculateHash(u32 addr, u32 size, u8 *result)
{
	UNUSED(result);

	if (IS_FLASH_ADDR(addr)) {
		if (!IS_FLASH_ADDR(addr + size - 1)) {
			return HAL_ERR_PARA;
		}
	}

	/* TODO */

	return HAL_OK;
}

/**
  * @brief  Get device info
  * @param  adapter: HCI adapter
  * @retval Status
  */
static void HCI_GetDeviceInfo(HCI_AdapterTypeDef *adapter)
{
	struct ameba_flash_info flashInfo;
	HCI_DeviceInfoTypeDef *deviceInfo = &adapter->DeviceInfo;
	u32 chipinfo;
	u16 sizeInKB;

	deviceInfo->ChipId = HCI_CHIP_ID;
	deviceInfo->CutVersion = EFUSE_GetChipVersion();
	deviceInfo->ProtocolVersion = HCI_PROTOCOL_VERSION;
	deviceInfo->SramSizeInKB = HCI_SRAM_SIZE_IN_KB;
	u32 MemType = ChipInfo_MemoryType();

	if (MemType == MEMORY_MCM_PSRAM)  {
		chipinfo = ChipInfo_MemoryInfo();
		switch (PSRAM_SIZE_GET(chipinfo)) {
		case PSRAM_SIZE_32Mb:
			sizeInKB = 4 * 1024;
			break;
		case PSRAM_SIZE_64Mb:
			sizeInKB = 8 * 1024;
			break;
		case PSRAM_SIZE_128Mb:
			sizeInKB = 16 * 1024;
			break;
		default: // >= PSRAM_SIZE_256Mb
			sizeInKB = 32 * 1024;
			break;
		}
		deviceInfo->PramSizeInKB = sizeInKB;
	} else if ((MemType == MEMORY_ONE_FLASH) || (MemType == MEMORY_TWO_FLASH)) {
		nor_ftl_init(&flashInfo);
		deviceInfo->FlashSizeInKB = flashInfo.capacity / 1024;
	}
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  RX data process
  * @param  adapter: HCI adapter
  * @param  buf: RX buffer
  * @param  size: RX data size in bytes
  * @retval Result of the operation: HAL_OK if all operations are OK else HAL_ERR_HW
  */
int HCI_RxProcess(HCI_AdapterTypeDef *adapter, u8 *buf, u32 size)
{
	u8 ret = HAL_ERR_HW;
	u32 checksum32 = 0;
	u32 i;
	u8 *data;
	u16 len;
	u16 txLen = HCI_PKT_DESC_SIZE;
	u8 needCheck;
	u8 descLen;

	HCI_PacketDescTypeDef *txDesc = (HCI_PacketDescTypeDef *)(adapter->TxBuffer);
	HCI_PacketDescTypeDef *rxDesc;

	_memset((void *)txDesc, 0, HCI_PKT_DESC_SIZE);
	txDesc->DataOffset = HCI_PKT_DESC_SIZE;
	txDesc->PacketType = HCI_XFER_ACK_MASK;
	txDesc->XferStatus = HAL_OK;
	txDesc->Version = (u8)(EFUSE_GetChipVersion() & 0xFF);
	txDesc->DevMode = HCI_FW_TYPE_BOOTLOADER;

	if ((buf == NULL) || (size < HCI_PKT_DESC_SIZE)) {
		//RTK_LOGS(TAG, RTK_LOG_WARN, "Invalid BULK OUT packet\n");
		txDesc->XferStatus = HAL_ERR_PARA;
		adapter->Interface->Transmit(adapter, adapter->TxBuffer, HCI_PKT_DESC_SIZE);
		return HAL_ERR_PARA;
	}

	rxDesc = (HCI_PacketDescTypeDef *)buf;
	descLen = rxDesc->DataOffset;

	switch (rxDesc->PacketType) {
	case HCI_XFER_REQ_QUERY:
		if (descLen == HCI_PKT_DESC_SIZE) {
			txDesc->PacketType |= HCI_XFER_REQ_QUERY;
			txDesc->XferStatus = HAL_OK;
			len = sizeof(HCI_DeviceInfoTypeDef);
			_memcpy((void *)(adapter->TxBuffer + HCI_PKT_DESC_SIZE), (void *)&adapter->DeviceInfo, len);
			txLen += len;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case HCI_XFER_REQ_WRITE:
		if ((descLen == HCI_PKT_DESC_SIZE) && (rxDesc->DataLen + descLen == size) && (rxDesc->DataLen > 0)) {
			txDesc->PacketType |= HCI_XFER_REQ_WRITE;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->DataLen;
			data = buf + HCI_PKT_DESC_SIZE;
			needCheck = rxDesc->Config & HCI_WRITE_CFG_CHK_EN;
			if (needCheck) {
				for (i = 0; i < rxDesc->DataLen; i += 4) {
					checksum32 ^= *(u32 *)(data + i);
				}
			}
			if ((needCheck != 0U) && (checksum32 != rxDesc->Value)) {
				//RTK_LOGS(TAG, RTK_LOG_DEBUG, "WRITE: addr 0x%08x size %d checksum err 0x%08x, exepcted 0x%08x\n",
				//		 rxDesc->MemAddr, rxDesc->DataLen, checksum32, rxDesc->Value);
				txDesc->XferStatus = HAL_ERR_PARA;
			} else { // ((needCheck == 0U) || ((needCheck != 0U) && (checksum32 == rxDesc->Value)))
				ret = HCI_WriteImage(rxDesc->MemAddr, data, rxDesc->DataLen);
				if (ret == HAL_OK) {
					//RTK_LOGS(TAG, RTK_LOG_DEBUG, "WRITE: addr 0x%08x size %d OK\n", rxDesc->MemAddr, rxDesc->DataLen);
				} else {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "WRITE: addr 0x%08x size %d err %d\n", rxDesc->MemAddr, rxDesc->DataLen, ret);
				}
				txDesc->XferStatus = ret;
			}
		} else {
			//RTK_LOGS(TAG, RTK_LOG_WARN, "WRITE: Invalid packet length\n");
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case HCI_XFER_REQ_CHECK:
		if (descLen == HCI_PKT_DESC_SIZE) {
			txDesc->PacketType |= HCI_XFER_REQ_CHECK;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->MemSize;
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "CHECK: addr 0x%08x size %d\n", rxDesc->MemAddr, rxDesc->MemSize);
			ret = HCI_CalculateChecksum(rxDesc->MemAddr, rxDesc->MemSize, &checksum32);
			if (ret == HAL_OK) {
				txDesc->Value = checksum32;
				//RTK_LOGS(TAG, RTK_LOG_DEBUG, "CHECK: addr 0x%08x size %d chk=0x%08x\n", rxDesc->MemAddr, rxDesc->MemSize, checksum32);
			}
			txDesc->XferStatus = ret;

			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case HCI_XFER_REQ_BOOT:
		if (descLen == HCI_PKT_DESC_SIZE) {
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "BOOT\n");
			txDesc->PacketType |= HCI_XFER_REQ_BOOT;
			txDesc->XferStatus = HAL_OK;
			adapter->Boot = 1;
			adapter->BootDelayUs = rxDesc->Value;
			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case HCI_XFER_REQ_ABORT:
		if (descLen == HCI_PKT_DESC_SIZE) {
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "ABORT\n");
			adapter->Abort = 1;
			adapter->AbortDelayUs = rxDesc->Value;
			txDesc->PacketType |= HCI_XFER_REQ_ABORT;
			txDesc->XferStatus = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case HCI_XFER_REQ_ERASE:
		if (descLen == HCI_PKT_DESC_SIZE) {
			txDesc->PacketType |= HCI_XFER_REQ_ERASE;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->MemSize;
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "ERASE: addr 0x%08x size %d\n", rxDesc->MemAddr, rxDesc->MemSize);
			ret = HCI_EraseFlash(rxDesc->MemAddr, rxDesc->MemSize);
			txDesc->XferStatus = ret;
			ret = HAL_OK;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	case HCI_XFER_REQ_HASH:
		if (descLen == HCI_PKT_DESC_SIZE) {
			txDesc->PacketType |= HCI_XFER_REQ_HASH;
			txDesc->MemAddr = rxDesc->MemAddr;
			txDesc->MemSize = rxDesc->MemSize;
			//RTK_LOGS(TAG, RTK_LOG_DEBUG, "HASH: addr 0x%08x size %d\n", rxDesc->MemAddr, rxDesc->MemSize);
			ret = HCI_CalculateHash(rxDesc->MemAddr, rxDesc->MemSize, adapter->TxBuffer + HCI_PKT_DESC_SIZE);
			txDesc->XferStatus = ret;
			txLen += HCI_HASH_SIZE;
		} else {
			txDesc->XferStatus = HAL_ERR_PARA;
		}
		break;
	default:
		txDesc->XferStatus = HAL_ERR_PARA;
		break;
	}

	adapter->Interface->Transmit(adapter, adapter->TxBuffer, txLen);

	return HAL_OK;
}

/**
  * @brief  HCI init
  * @param  None
  * @retval Status
  */
int HCI_Init(void)
{
	int ret;
	HCI_AdapterTypeDef *adapter = &HCI_Adapter;

#if defined(CONFIG_WHC_INTF_USB)
	adapter->Interface = &USBD_Interface;
#elif defined(CONFIG_WHC_INTF_SDIO)
	adapter->Interface = &SDIO_Interface;
#else
	RTK_LOGE(TAG, "Invalid WHC interface\n");
#endif

	rtk_log_level_set(TAG, COMPIL_LOG_LEVEL);

	HCI_GetDeviceInfo(adapter);

	ret = adapter->Interface->Init(adapter);
	return ret;
}

/**
  * @brief  HCI deinit
  * @param  None
  * @retval None
  */
void HCI_DeInit(void)
{
	HCI_AdapterTypeDef *adapter = &HCI_Adapter;
	if (adapter->Interface) {
		adapter->Interface->DeInit(adapter);
	}
}

/**
  * @brief  HCI wait routine
  * @param  None
  * @retval Result of the operation: HAL_OK if need to boot, HAL_ERR_PARA if aborted
  */
int HCI_WaitForExit(void)
{
	int ret = HAL_ERR_UNKNOWN;
	u32 cnt = 0;
	HCI_AdapterTypeDef *adapter = &HCI_Adapter;

	while (1) {
		if (++cnt >= 100) {
			cnt = 0;
			WDG_Refresh(IWDG_DEV);
		}
		if (adapter->Boot) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Done\n");
			DelayUs(adapter->BootDelayUs); // Wait transfer done, set this value as per host features
			ret = HAL_OK;
			break;
		}
		if (adapter->Abort) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Abort\n");
			DelayUs(adapter->AbortDelayUs);	// Wait transfer done, set this value as per host features
			ret = HAL_ERR_PARA;
			break;
		}
		DelayUs(10000);
	}

	return ret;
}