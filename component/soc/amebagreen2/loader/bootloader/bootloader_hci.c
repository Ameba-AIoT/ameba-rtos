/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "bootloader_km4tz.h"
#include "ameba_dump_stack.h"
#include "ameba_secure_boot.h"
#include "boot_ota_km4tz.h"
#ifdef CONFIG_FULLMAC_DEV
#include "hci_core.h"
#endif

static const char *const TAG = "BOOT";
extern IMAGE_HEADER EmptyImgHdr;

static void _Init_SDIO_By_PinGrp(void)
{
	u8 idx;

	assert_param(SDIO_Pin_Grp <= 0x6);
	RTK_LOGW(TAG, "Modify SDIO_Pin_Grp by Board...\n");

	/* Pinmux function and Pad control */
	for (idx = 0; idx < 6; idx++) {
		PAD_PullCtrl(SDIO_PAD[SDIO_Pin_Grp][idx], GPIO_PuPd_UP);
		Pinmux_Config(SDIO_PAD[SDIO_Pin_Grp][idx], PINMUX_FUNCTION_SDIO_SLV);
	}

	/* SDIO function enable and clock enable*/
	RCC_PeriphClockCmd(APBPeriph_SDD, APBPeriph_SDD_CLOCK, ENABLE);

	/* CCCR Autoload done */
	SDIO_WIFI->SPDIO_SYS_CPU_INDICATION |= SDIO_WIFI_BIT_READEE_DONE;

	/* SDIO pinmux function enable */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SDIO_CTRL) | LSYS_BIT_SDD_PMUX_FEN);
}

void Boot_SDIO_Pinmux_init(void)
{
	u32 boot_src = SYSCFG_OTP_BOOTSEL();
	/* When not boot from sdio, sdio pinmux shall init early in bootloader.
	 * If done too late, it may cause host side SDIO card recognition timeout, resulting in sdio power supply failure. */
	if (boot_src != BOOT_FROM_SDIO) {
		_Init_SDIO_By_PinGrp();
	}
}

#ifdef CONFIG_FULLMAC_DEV
void Boot_Fullmac_ImgInfo_Set(SubImgInfo_TypeDef *SubImgInfo, IMAGE_HEADER *ImgHdr)
{
	if (ImgHdr != &EmptyImgHdr) {
		SubImgInfo->Addr = ImgHdr->image_addr - IMAGE_HEADER_LEN;
		SubImgInfo->Len = ImgHdr->image_size + IMAGE_HEADER_LEN;
	} else {
		SubImgInfo->Addr = (u32)&EmptyImgHdr;
		SubImgInfo->Len = IMAGE_HEADER_LEN;
	}
}

void Boot_Fullmac_Secure_Check(u8 ImgCnt, u8 FlashValid, u8 PsramValid)
{
	SubImgInfo_TypeDef SubImgInfo[3];
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	Manifest_TypeDef *Manifest;
	IMAGE_HEADER *ImgHdr;

	char *ApLabel[] = {"AP XIP IMG", "AP SRAM", "AP PSRAM"};
	u8 Cnt = sizeof(ApLabel) / sizeof(char *);

	if (ImgCnt == 1) {
		// Trandionly fullmac download, put manifest after sram_2.bin
		ImgHdr = (IMAGE_HEADER *)((u32)__image2_entry_func__ - IMAGE_HEADER_LEN);
		Boot_Fullmac_ImgInfo_Set(&SubImgInfo[0], ImgHdr);
		assert_param(SubImgInfo[0].Addr == (u32)ImgHdr);
		Manifest = (Manifest_TypeDef *)(SubImgInfo[0].Addr + SubImgInfo[0].Len);
		Cnt = ImgCnt;
	} else {
		Boot_Fullmac_ImgInfo_Set(&SubImgInfo[0], &EmptyImgHdr);
		ImgHdr = (IMAGE_HEADER *)((u32)__image2_entry_func__ - IMAGE_HEADER_LEN);
		Boot_Fullmac_ImgInfo_Set(&SubImgInfo[1], ImgHdr);
		Boot_Fullmac_ImgInfo_Set(&SubImgInfo[2], &EmptyImgHdr);

		// for sram recycle, manifest is locate at 0x3007F000.
		Manifest = (Manifest_TypeDef *)0x3007F000;
		assert_param(SubImgInfo[1].Addr == (u32)ImgHdr);

		if (FlashValid) {
			ImgHdr = (IMAGE_HEADER *)((u32)__km4tz_flash_text_start__ - IMAGE_HEADER_LEN);
			Boot_Fullmac_ImgInfo_Set(&SubImgInfo[0], ImgHdr);
			Cnt = ImgCnt;
		}
		if (PsramValid) {
			ImgHdr = (IMAGE_HEADER *)((u32)__km4tz_bd_psram_start__);
			Boot_Fullmac_ImgInfo_Set(&SubImgInfo[2], ImgHdr);
			Cnt = ImgCnt;
		}
	}

	assert_param(Cnt <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);

	FIH_CALL(BOOT_Extract_SignatureCheck, fih_rc, Manifest, &SubImgInfo[0], Cnt);
	if (FIH_EQ(fih_rc, FIH_SUCCESS)) {
		return;
	}

	RTK_LOGE(TAG, "FULLMAC IMG2 Load IMG Fail!\n");
	/* stuck and clear msp if boot fail */
	SBOOT_Validate_Fail_Stuck(FALSE);
}

static void Boot_ResetSystem(void)
{
	WDG_InitTypeDef WDG_InitStruct;
	WDG_StructInit(&WDG_InitStruct);
	WDG_InitStruct.Window = 65535;
	WDG_InitStruct.Timeout = 1;
	WDG_InitStruct.EICNT = 1;
	WDG_InitStruct.EIMOD = 0;

	WDG_Init(IWDG_DEV, &WDG_InitStruct);
	WDG_Enable(IWDG_DEV);
	WDG_Refresh(IWDG_DEV);

	while (1);
}

/** Note:
 *  1. HCI_WriteImage receive empty EmptyImgHdr if addr is 0xFFFFFFFF and size is 0x20
 *  2. Manifest is download at 0x3007F000
 */
void Boot_Fullmac_ImgDownload(void)
{
	int status;

	/* Init host control interface */
	status = HCI_Init();
	if (status == HAL_OK) {
		/* Wait for image2 download */
		status = HCI_WaitForExit();
		HCI_DeInit();
		if (status == HAL_OK) {
			/* Boot to image2 */
		} else {
			/* Abort download */
			Boot_ResetSystem();
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "HCI init failed: %d\n", status);
	}
}

void Boot_Fullmac_LoadIMGAll(void)
{
	u32 boot_src = SYSCFG_OTP_BOOTSEL();
	u8 mem_type = ChipInfo_MemoryType();
	if ((boot_src == BOOT_FROM_FLASH) || (boot_src == BOOT_FROM_FLASH1)) { // for test purpose, delete it later
		flash_highspeed_setup();
		Boot_Fullmac_LoadImage();
	} else {
		switch (mem_type) {
		case MEMORY_ONE_FLASH:
			/* rom code init flash only when BOOT_FROM_FLASH */
			void BOOT_ROM_InitFlash(void);
			BOOT_ROM_InitFlash();
			flash_highspeed_setup();

			Boot_Fullmac_ImgDownload();
			Boot_Fullmac_XipEn();
			Boot_Fullmac_Secure_Check(3, TRUE, FALSE);
			break;
		case MEMORY_MCM_PSRAM:
			/* Ensure BOOT_PSRAM_Init() in called in Bootloader */
			Boot_Fullmac_ImgDownload();
			Boot_Fullmac_Secure_Check(3, FALSE, TRUE);
			break;
		default:
		case MEMORY_SINGLE_DIE:
			Boot_Fullmac_Secure_Check(1, FALSE, FALSE);
			break;
		}
	}
}
#endif

