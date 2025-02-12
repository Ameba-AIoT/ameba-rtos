/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "ameba_secure_boot.h"
#include "bootloader_km4.h"
#include "boot_ota_km4.h"

#define SECURE_BOOT_SUPPORT	DISABLE
static const char *TAG = "BOOT";

typedef struct {
	u32 Pattern[2];
	u8 Rsvd1[8];
	u8 Ver;
	u8 ImgID;
	u8 AuthAlg;
	u8 HashAlg;
	u16 MajorImgVer;
	u16 MinorImgVer;
	u32 ImgSize;
	u32 SecEpoch;
	u8 RsipIV[16];
} Manifest_Min_t;

static Manifest_Min_t Manifest[2]; //Manifest of SlotA & SlotB
#if SECURE_BOOT_SUPPORT
u8 Signature[2][SIGN_MAX_LEN];
#else
u8 Signature[2][2];
#endif
s64 Ver[2] = {0};  //32-bit full version

static u32 ImagePattern[2] = {
	0x35393138,
	0x31313738,
};

static u8 ValidIMGNum = 0;
u32 OTA_Region[3][2] = {0};

/**
  * @brief  Get Version Number from OTP
  * @param  none
  * @retval bootloader version in OTP
  */
static u32 SYSCFG_OTP_GetRollbackVer(void)
{
	u32 tmp, Anti_VerNum = 0;

	/* Cert/app version is same, so Anti_VerNum for cert/app can be same */
	if (SYSCFG_OTP_AntiRollback()) {
		tmp = HAL_READ16(OTPC_REG_BASE, SEC_BOOT_IMGVER0);

		for (int i = 0; i < 16; i++) {
			if ((tmp & BIT(i)) == 0) {
				Anti_VerNum++;
			}
		}

		/*Only Check MajorImgVer with Anti_VerNum in otp*/
		Anti_VerNum = Anti_VerNum << 16;
	}

	return Anti_VerNum;
}

/**
  * @brief  check manifest pattern and version for IMG2, IMG3 and AP
  * @param  IMGType: IMG_IMG2/IMG_IMG3/IMG_IMG2
  * @retval 0:Slot A  1:Slot B
  */
u8 BOOT_OTA_IMGSlotSelect(u32 IMGType)
{
	u16 MajorVer[2] = {0}; //16-bit major
	u16 MinorVer[2] = {0}; //16-bit minor
	u32 Vertemp;
	s64 VN_IMG; //32-bit full version in OTP for IMG2
	u8 ImgIndex, i;
	/* ------------1 load IMG(Slot A & Slot B) from flash to SRAM, check pattern------------ */
	for (i = 0; i < 2; i++) {
		_memcpy((void *)&Manifest[i], (void *)OTA_Region[IMGType][i], sizeof(Manifest_Min_t));
		if (_memcmp(Manifest[i].Pattern, ImagePattern, sizeof(ImagePattern)) == 0) {
			MajorVer[i] = (u16)Manifest[i].MajorImgVer;
			MinorVer[i] = (u16)Manifest[i].MinorImgVer;
			Vertemp = MajorVer[i] << 16 | MinorVer[i];
			Ver[i] = (s64)Vertemp; // get 32-bit full version number
		} else {
			Ver[i] = -1;
		}
	}
	/* ---------------------------------2 Get bootldr version in OTP if anti-rollback --------------------------------- */
	VN_IMG = (s64)SYSCFG_OTP_GetRollbackVer();

	/* ---------------------------------3 Check full KeyVerion in manifest--------------------------------- */
	if ((VN_IMG > Ver[0]) && (VN_IMG > Ver[1])) {
		ValidIMGNum = NONEVALIDIMG;
	} else if ((VN_IMG <= Ver[0]) && (VN_IMG <= Ver[1])) {
		ValidIMGNum = TWOVALIDIMG;
	} else {
		ValidIMGNum = ONEVALIDIMG;
	}

	ImgIndex = (Ver[0] >= Ver[1]) ? BOOT_FROM_OTA1 : BOOT_FROM_OTA2;
	return ImgIndex;
}

u8 BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog)
{
	IMAGE_HEADER ImgHdr;
	u32 DstAddr, Len;
	u32 i;

	for (i = 0; i < Num; i++) {
		_memcpy((void *)&ImgHdr, (void *)StartAddr, IMAGE_HEADER_LEN);

		if ((ImgHdr.signature[0] != 0x35393138) || (ImgHdr.signature[1] != 0x31313738)) {
			if (ErrLog == _TRUE) {
				RTK_LOGE(TAG, "%s Invalid\n", ImgName[i]);
			}
			return _FALSE;
		}

		DstAddr = ImgHdr.image_addr - IMAGE_HEADER_LEN;
		Len = ImgHdr.image_size + IMAGE_HEADER_LEN;

		/* If not XIP sub-image, then copy it to specific address (include the IMAGE_HEADER)*/
		if ((!IS_FLASH_ADDR(DstAddr)) && (Len > IMAGE_HEADER_LEN)) {
			_memcpy((void *)DstAddr, (void *)StartAddr, Len);
			DCache_CleanInvalidate(DstAddr, Len);
		}

		/* empty Image, Just put in flash, for image hash later */
		if (Len == IMAGE_HEADER_LEN) {
			DstAddr = StartAddr;
		}

		if (SubImgInfo != NULL) {
			SubImgInfo[i].Addr = DstAddr;
			SubImgInfo[i].Len = Len;

			RTK_LOGI(TAG, "%s[%08x:%x]\n", ImgName[i], DstAddr, Len);
		}

		StartAddr += Len;
	}

	return _TRUE;
}

void BOOT_RSIPIvSet(Manifest_TypeDef *Manifest, u32 IV_index)
{
	u8 iv[8] = {0};

	/* 1. check if RSIP enable */
	if (SYSCFG_OTP_RSIPEn() == FALSE) {
		return;
	}

	/* get iv from manifest */
	_memcpy(iv, Manifest->RsipIV, 8);
	RSIP_IV_Set(IV_index, iv);
}

/* start addr --> logical addr */
void BOOT_OTFCheck(u32 start_addr, u32 end_addr, u32 IV_index, u32 OTF_index)
{
	u32 mode;

	/* 1. check if RSIP enable */
	if (SYSCFG_OTP_RSIPEn() == FALSE) {
		return;
	}

	if (IS_FLASH_ADDR(start_addr) && IS_FLASH_ADDR(end_addr)) {
		RTK_LOGI(TAG, "IMG2 OTF EN\n");
		mode = SYSCFG_OTP_RSIPMode();

		switch (mode) {
		case RSIP_XTS_MODE:
			mode = OTF_XTS_MODE;
			break;
		case RSIP_CTR_MODE:
			mode = OTF_CTR_MODE;
			break;
		default:
			RTK_LOGE(TAG, "OTF Mode error\r\n");
			return;
		}

		RSIP_OTF_Enable(OTF_index, start_addr, end_addr, ENABLE, IV_index, mode);
		RSIP_OTF_Cmd(ENABLE);
	}

	return;
}

u8 BOOT_Load_KR4_IMG1(void)
{
	SubImgInfo_TypeDef SubImgInfo;
	u32 LoaderAddr, PhyAddr;
	u8 Cnt;
	char *Kr4Label[] = {"KR4 BOOT"};

	/* step 1: init OTA region */
	flash_get_layout_info(IMG_BOOT, &LoaderAddr, NULL);

	_memcpy((void *)&Manifest[0], (void *)LoaderAddr, sizeof(Manifest_Min_t));
	PhyAddr = LoaderAddr + MANIFEST_SIZE_4K_ALIGN + Manifest[0].ImgSize;

	Cnt = sizeof(Kr4Label) / sizeof(char *);
	return BOOT_LoadSubImage(&SubImgInfo, PhyAddr, Cnt, Kr4Label, _TRUE);
}

u8 BOOT_Load_KR4_APP(void)
{
	SubImgInfo_TypeDef SubImgInfo[4];
	u32 LogAddr, PhyAddr;
	u8 Cnt, i;
	u8 ret;
	u8 ImgIndex;
	u32 version;

	char *Kr4Label[] = {"KR4 XIP IMG", "KR4 PSRAM", "KR4 SRAM", "KR4 PMC"};

	/* step 1: init OTA region */
	flash_get_layout_info(IMG_KR4_OTA1, &OTA_Region[IMG_IMG2][0], NULL);
	flash_get_layout_info(IMG_KR4_OTA2, &OTA_Region[IMG_IMG2][1], NULL);

	/* step2: Select Slot according to Manifest Version */
	ImgIndex = BOOT_OTA_IMGSlotSelect(IMG_IMG2);

	/* no valid AP IMG, boot fail */
	if (ValidIMGNum == NONEVALIDIMG) {
		goto Fail;
	}

	for (i = 0; i < ValidIMGNum; i++) {
		PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];

		/* set IMG2 IV */
		BOOT_RSIPIvSet((Manifest_TypeDef *)&Manifest[ImgIndex], 1);

		/* remap KR4 XIP image */
		PhyAddr += MANIFEST_SIZE_4K_ALIGN;
		LogAddr = (u32)__kr4_flash_text_start__ - IMAGE_HEADER_LEN;

		RSIP_MMU_Config(1, LogAddr, LogAddr + 0x01000000 - 0x20, PhyAddr);
		RSIP_MMU_Cmd(1, ENABLE);
		RSIP_MMU_Cache_Clean();
		/*KR4 IMG2 RSIP configurations*/
		BOOT_OTFCheck(LogAddr, LogAddr + 0x01000000 - 0x20, 1, 1);

		Cnt = sizeof(Kr4Label) / sizeof(char *);
		ret = BOOT_LoadSubImage(&SubImgInfo[ImgIndex], LogAddr, Cnt, Kr4Label, _TRUE);
		assert_param(Cnt <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));

#if SECURE_BOOT_SUPPORT
		if (ret != TRUE) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		ret = BOOT_SignatureCheck(&Manifest, SubImgInfo, Cnt, &Cert[CertImgIndex], KEYID_AP); //BL1 ECC verify if need
#endif
		if (ret != TRUE) {
			/* OTA1 and OTA2 share one MMU virtual address. when image header of one OTA is error, will cause another OTA fail, here invalid all D-cache to avoid corner case. */
			DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		} else {
			version = (u32)(Ver[ImgIndex] & 0xFFFFFFFF);
			RTK_LOGI(TAG, "KR4 BOOT from OTA %d, Version: %x.%x \n", ImgIndex + 1, ((version >> 16) & 0xFFFF), (version & 0xFFFF));
			break;
		}
	}

	if (i == ValidIMGNum) {
		goto Fail;
	}

	return TRUE;

Fail:
	RTK_LOGE(TAG, "Fail to load KR4 image!\n");
#if SECURE_BOOT_SUPPORT
	while (1) {
		DelayMs(1000);
	}
#else
	return FALSE;
#endif
}

u8 BOOT_Load_KM4_APP(void)
{
	SubImgInfo_TypeDef SubImgInfo[5];
	u32 LogAddr, PhyAddr;
	u8 Cnt, i;
	u8 ret;
	u8 ImgIndex;
	u32 version;

	char *Km4Label[] = {"KM4 XIP IMG", "KM4 PSRAM", "KM4 SRAM", "KM4 BOOT", "KM4 PMC"};

	/* step 1: init OTA region */
	flash_get_layout_info(IMG_APP_OTA1, &OTA_Region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &OTA_Region[IMG_CERT][1], NULL);
	/* skip certificate */
	OTA_Region[IMG_IMG2][0] = OTA_Region[IMG_CERT][0] + 0x1000;
	OTA_Region[IMG_IMG2][1] = OTA_Region[IMG_CERT][1] + 0x1000;

	/* step2: Select Slot according to Manifest Version */
	ImgIndex = BOOT_OTA_IMGSlotSelect(IMG_IMG2);

	/* no valid AP IMG, boot fail */
	if (ValidIMGNum == NONEVALIDIMG) {
		goto Fail;
	}

	for (i = 0; i < ValidIMGNum; i++) {
		PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];

		/* set IMG2 IV */
		BOOT_RSIPIvSet((Manifest_TypeDef *)&Manifest[ImgIndex], 2);

		/* remap KM4 XIP image */
		PhyAddr += MANIFEST_SIZE_4K_ALIGN;
		LogAddr = (u32)__km4_flash_text_start__ - IMAGE_HEADER_LEN;

		RSIP_MMU_Config(2, LogAddr, LogAddr + 0x01000000 - 0x20, PhyAddr);
		RSIP_MMU_Cmd(2, ENABLE);
		RSIP_MMU_Cache_Clean();

		BOOT_OTFCheck(LogAddr, LogAddr + 0x01000000 - 0x20, 2, 2);
		/* KM4 XIP & SRAM, read with virtual addr in case of encryption */
		Cnt = sizeof(Km4Label) / sizeof(char *);
		ret = BOOT_LoadSubImage(&SubImgInfo[ImgIndex], LogAddr, Cnt, Km4Label, _TRUE);
		assert_param(Cnt <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));

#if SECURE_BOOT_SUPPORT
		if (ret != TRUE) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		ret = BOOT_SignatureCheck(&Manifest[ImgIndex], SubImgInfo, Cnt, &Cert[CertImgIndex], KEYID_NSPE); //BL1 ECC verify if need
#endif
		if (ret != TRUE) {
			/* OTA1 and OTA2 share one MMU virtual address. when image header of one OTA is error, will cause another OTA fail, here invalid all D-cache to avoid corner case. */
			DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		} else {
			version = (u32)(Ver[ImgIndex] & 0xFFFFFFFF);
			RTK_LOGI(TAG, "KM4 BOOT from OTA %d, Version: %x.%x \n", ImgIndex + 1, ((version >> 16) & 0xFFFF), (version & 0xFFFF));
			break;
		}
	}

	if (i == ValidIMGNum) {
		goto Fail;
	}

	return TRUE;

Fail:
	RTK_LOGE(TAG, "Fail to load KM4 image!\n");
	while (1) {
		DelayMs(1000);
	}
}

// DiagVprintf is too large, redirect to ROM symbal
int DiagVSprintf(char *buf, const char *fmt, va_list ap);
__WEAK
int DiagVprintf(const char *fmt, va_list args)
{
	return DiagVSprintf(NULL, fmt, args);
}

