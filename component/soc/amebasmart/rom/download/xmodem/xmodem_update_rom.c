/********************************************************************************
 * Copyright (c) 2014, Realtek Semiconductor Corp.
 * All rights reserved.
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *******************************************************************************
 */

#include "ameba_soc.h"
#include "xmodem_rom.h"
#include "xmodem_uart_rom.h"
#include "xmodem_update_rom.h"
#include "ameba_secure_boot.h"
#include "usbd_intf_rom.h"

int BOOT_ROM_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum);

/**
  * @brief  This function is used to write 1024 xmodem data to SRAM.
  * @param  ptr: data pointer.
  * @param  wr_offset: target address.
  * @param  frame_size: write size.
  * @retval none
  */
HAL_ROM_TEXT_SECTION
void xmodem_img_write(char *ptr,  unsigned int wr_offset, unsigned int frame_size)
{
	u32 index = 0;

	/* only allow to write boot sram, to avoid attack */
	if ((!IS_BOOT_ADDR(wr_offset)) || (!IS_BOOT_ADDR(wr_offset + frame_size - 1))) {
		return;
	}

	for (index = 0; index < (frame_size / 4); index++) {
		*((u32 *)(wr_offset + index * 4)) = (*((u32 *)(ptr + index * 4)));
	}
	DCache_CleanInvalidate(wr_offset, frame_size);
}

HAL_ROM_TEXT_SECTION
void xmodem_vefiry_floader(void)
{
	SubImgInfo_TypeDef SubImgInfo;
	Manifest_TypeDef *Manifest;
	IMAGE_HEADER *ImgHdr = (IMAGE_HEADER *)(__ram_start_table_start__ - IMAGE_HEADER_LEN);

	/* 1. get flashloader info */
	SubImgInfo.Addr = ImgHdr ->image_addr - IMAGE_HEADER_LEN;
	SubImgInfo.Len = ImgHdr->image_size + IMAGE_HEADER_LEN;

	/* 2. locate manifest */
	Manifest = (Manifest_TypeDef *)(SubImgInfo.Addr + SubImgInfo.Len);

	/* 3. verify signature */
	if (BOOT_ROM_SignatureCheck(Manifest, &SubImgInfo, 1) < 0) {

		DBG_8195A("Flashloader SBOOT Check Fail!\n");

		/* clear flashloader */
		if (!IS_FLASH_ADDR(SubImgInfo.Addr)) {
			_memset((void *)SubImgInfo.Addr, 0, SubImgInfo.Len);
			DCache_CleanInvalidate(SubImgInfo.Addr, SubImgInfo.Len);
		}

		/* clear stack */
		__set_MSP(MSP_RAM_HP);
		_memset((void *)MSPLIM_RAM_HP, 0, MSP_RAM_HP - 128 - MSPLIM_RAM_HP);
		DCache_CleanInvalidate(MSPLIM_RAM_HP, MSP_RAM_HP - 128 - MSPLIM_RAM_HP);

		while (1) {
			DelayMs(1000);
		}
	}
}

HAL_ROM_TEXT_SECTION
void xmodem_img_download(u8 uart_idx)
{
	u32 baud_rate = 115200; /* default handshake baudrate 115200 */
	u32 result;

	/* dont close this message */
	DiagPrintf("Flash Download Start \n");

	usbd_enabled = 0;
	if (SYSCFG_OTP_UsbDownloadEn()) {
		usbd_enabled = 1;
	}

	/* init USB if USB download is not disabled */
	if (usbd_enabled) {
		/* init USB */
		if (USBD_Init() != 0) {
			/* Once USB not successfully initialized, USB download shall be disabled */
			usbd_enabled = 0;
		}
	}

	/* init uart */
	xmodem_uart_port_init(uart_idx, PINMUX_S0, baud_rate);

	/* recv and write to sram */
	result = xmodem_img_rxbuffer(baud_rate);

	/* deinit uart */
	xmodem_uart_port_deinit(uart_idx);

	/* switch to USB download mode */
	if ((usbd_enabled == 1) && (result == XM_RESULT_USB)) {
		/* poll the USB download done/aborted status while the download is acctually processed in USB interrupt */
		result = USBD_Process();
		/* deinit USB */
		USBD_DeInit();
	}

	/* If cancel, normal boot */
	if (result == XM_RESULT_CAN) {
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE_LP,
					REG_LSYS_BOOT_CFG) & ~LSYS_BIT_BOOT_PATCH_FUNC0);
		return;
	}

	/* ECC verify */
	xmodem_vefiry_floader();

	/* boot to ram from uart */
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE_LP,
				REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_PATCH_FUNC0);

	DBG_8195A("Flash Download Done \n");
}
