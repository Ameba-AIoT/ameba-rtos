/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "amebahp_secure_boot.h"
#include "bootloader_hp.h"
#include "boot_ota_hp.h"
#include "ameba_boot_lzma.h"

static const char *TAG = "BOOT";
static Certificate_TypeDef Cert[2]; //Certificate of SlotA & SlotB
static Manifest_TypeDef Manifest[2]; //Manifest of SlotA & SlotB
u8 Signature[2][SIGN_MAX_LEN];
s64 Ver[2] = {0};  //32-bit full version

static SubImgInfo_TypeDef SubImgInfo[14]; //store sub image addr and length

static const u32 ImagePattern[2] = {
	0x35393138, 0x31313738,
};

static const u32 CompressFlag[2] = {
	0x504D4F43, 0x53534552,
};

static u8 ValidIMGNum = 0;

/*
* @brif	OTA start address. Because KM0 & KM4 IMG2 are combined, users only need to set the start address
*	of KM0 IMG2.
*  In each entry, the first address is OTA1 address(SlotA address), the second address is OTA2 address (SlotB address)
*/
u32 OTA_Region[3][2];

BOOT_RAM_TEXT_SECTION
void BOOT_ImgCopy(void *__restrict dst0, const void *__restrict src0, size_t len0)
{
	if (SYSCFG_BootFromNor()) {
		_memcpy(dst0, src0, len0);
	} else {
		NandImgCopy((u8 *)dst0, (u8 *)src0, len0);
	}
}

/**
  * @brief  Get Version Number from OTP
  * @param  none
  * @retval bootloader version in OTP
  */
BOOT_RAM_TEXT_SECTION
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

#define SYSCFG_OTP_GetRollbackVerIMG SYSCFG_OTP_GetRollbackVer

/**
  * @brief  check Certificate pattern and version
  * @param none
  * @retval 0:Slot A  1:Slot B  2:V1 = V2
  */
BOOT_RAM_TEXT_SECTION
u8 BOOT_OTA_SlotSelect(void)
{
	u16 MajorVer[2] = {0}; //16-bit major
	u16 MinorVer[2] = {0}; //16-bit minor
	u32 Vertemp;
	s64 VN_CERT; //32-bit full version in OTP
	u8 ImgIndex, i;

	/* ----1 load certificate(Slot A & Slot B) from flash to SRAM, get full version and check validation---- */
	for (i = 0; i < 2; i++) {
		BOOT_ImgCopy((void *)&Cert[i], (void *)OTA_Region[IMG_CERT][i], sizeof(Certificate_TypeDef));

		if (_memcmp(Cert[i].Pattern, ImagePattern, sizeof(ImagePattern)) == 0) {
			/*do signature copy only when cert valid*/
			BOOT_ImgCopy((void *)&Signature[i], (void *)(OTA_Region[IMG_CERT][i] + Cert[i].TableSize), SIGN_MAX_LEN);
			BOOT_ImgCopy((void *)&Manifest[i], (void *)OTA_Region[IMG_IMG2][i], sizeof(Manifest_TypeDef)); // load img2 manifest together

			MajorVer[i] = (u16)Cert[i].MajorKeyVer;
			MinorVer[i] = (u16)Cert[i].MinorKeyVer;
			Vertemp = (MajorVer[i] << 16) | MinorVer[i];
			Ver[i] = (s64)Vertemp; // get 32-bit full version number
		} else {
			Ver[i] = -1;
		}
	}

	/* ---------------------------------2 Get bootldr version in OTP if anti-rollback --------------------------------- */
	VN_CERT = (s64)SYSCFG_OTP_GetRollbackVer();

	/* ---------------------------------3 Check full KeyVerion in certificate--------------------------------- */
	if ((VN_CERT > Ver[0]) && (VN_CERT > Ver[1])) {
		ValidIMGNum = NONEVALIDIMG;
	} else if ((VN_CERT <= Ver[0]) && (VN_CERT <= Ver[1])) {
		ValidIMGNum = TWOVALIDIMG;
	} else {
		ValidIMGNum = ONEVALIDIMG;
	}

	ImgIndex = (Ver[0] >= Ver[1]) ? BOOT_FROM_OTA1 : BOOT_FROM_OTA2;
	return ImgIndex;

}

BOOT_RAM_TEXT_SECTION
u8 BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog)
{
	IMAGE_HEADER ImgHdr;
	u32 DstAddr, Len;
	u32 i;

	for (i = 0; i < Num; i++) {
		BOOT_ImgCopy((void *)&ImgHdr, (void *)StartAddr, IMAGE_HEADER_LEN);

		if ((ImgHdr.signature[0] != 0x35393138) || (ImgHdr.signature[1] != 0x31313738)) {
			if (ErrLog == _TRUE) {
				RTK_LOGI(TAG, "%s Invalid\n", ImgName[i]);
			}
			return _FALSE;
		}

		DstAddr = ImgHdr.image_addr - IMAGE_HEADER_LEN;
		Len = ImgHdr.image_size + IMAGE_HEADER_LEN;

		/* If not XIP sub-image, then copy it to specific address (include the IMAGE_HEADER)*/
		if ((!IS_FLASH_ADDR(DstAddr)) && (Len > IMAGE_HEADER_LEN)) {
			BOOT_ImgCopy((void *)DstAddr, (void *)StartAddr, Len);
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

BOOT_RAM_TEXT_SECTION
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
BOOT_RAM_TEXT_SECTION
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

BOOT_RAM_TEXT_SECTION
/* Load km0/km4 img and check pattern if secure boot enabled*/
u8 BOOT_OTA_LoadIMG2(u8 ImgIndex)
{
	u32 LogAddr, PhyAddr, ImgAddr, TotalLen = 0;
	u8 Cnt;
	u8 i;
	u32 ret;
	u32 Index = 0;

	char *Km0Label[] = {"KM0 XIP IMG", "KM0 SRAM", "KM0 DRAM"};
	char *Km4Label[] = {"KM4 XIP IMG", "KM4 SRAM", "KM4 DRAM"};

	PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];

	/* set IMG2 IV */
	BOOT_RSIPIvSet(&Manifest[ImgIndex], OTF_IV_IMG2_IDX);

	/* remap KM0 XIP image */
	PhyAddr += MANIFEST_SIZE_4K_ALIGN;
	LogAddr = (u32)__km0_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_LP_IDX, LogAddr, LogAddr + 0x01000000 - 0x20, PhyAddr);
	RSIP_MMU_Cmd(MMU_LP_IDX, ENABLE);

	/*KM0 IMG2 RSIP configurations*/
	BOOT_OTFCheck(LogAddr, LogAddr + 0x01000000 - 0x20, OTF_IV_IMG2_IDX, OTF_LP_IDX);

	/* KM0 XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(Km0Label) / sizeof(char *);
	ImgAddr = SYSCFG_BootFromNor() ? LogAddr : PhyAddr;

	if (BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, Km0Label, _TRUE) == FALSE) {
		return FALSE;
	}
	SubImgInfo[Index].Addr = SYSCFG_BootFromNor() ? LogAddr : PhyAddr;

	for (i = 0; i < Cnt; i++) {
		TotalLen += SubImgInfo[i].Len;
	}
	Index += Cnt;

	/* add 4K-align padding info */
	SubImgInfo[Index].Addr = ImgAddr + TotalLen;
	SubImgInfo[Index].Len = ((((TotalLen - 1) >> 12) + 1) << 12) - TotalLen;
	TotalLen += SubImgInfo[Index].Len;
	Index ++;

	/* remap KM4 XIP image */
	PhyAddr += TotalLen;
	LogAddr = (u32)__km4_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_HP_IDX, LogAddr, LogAddr + 0x01000000 - 0x20, PhyAddr);
	RSIP_MMU_Cmd(MMU_HP_IDX, ENABLE);

	BOOT_OTFCheck(LogAddr, LogAddr + 0x01000000 - 0x20, OTF_IV_IMG2_IDX, OTF_HP_IDX);

	/* KM4 XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(Km4Label) / sizeof(char *);
	ImgAddr = SYSCFG_BootFromNor() ? LogAddr : PhyAddr;
	if (BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, Km4Label, _TRUE) == FALSE) {
		return FALSE;
	}
	SubImgInfo[Index].Addr = SYSCFG_BootFromNor() ? LogAddr : PhyAddr;
	Index += Cnt;

	/* check if RDP enable */
	if (SYSCFG_OTP_RDPEn() == TRUE) {
		/* Load from OTA and ECC check for IMG3 */
		RTK_LOGI(TAG, "RDP EN\n");
		Cnt = BOOT_OTA_RDP(SubImgInfo, Index, ImgIndex);
		Index += Cnt;
	} else {
#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
		RTK_LOGW(TAG, "RDP Shall En when TZ Configed.\n");
		assert_param(FALSE);
#endif
	}

#ifndef CONFIG_LINUX_FW_EN
	if (Boot_AP_Enbale) {
		/* Load from OTA and ECC check for APIMG */
		Cnt = BOOT_OTA_AP(SubImgInfo, Index, ImgIndex);
		Index += Cnt;
	}
#endif
	assert_param(Index < sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));

	/* IMG2+IMG3+APIMG ECC verify if need */
	ret = BOOT_SignatureCheck(&Manifest[ImgIndex], SubImgInfo, Index, &Cert[ImgIndex], KEYID_NSPE);

	RTK_LOGI(TAG, "IMG2(OTA%d) %s, ret: %d\n", ImgIndex + 1, ret > 0 ? "VALID" : "INVALID", ret);

	if (ret == FALSE) {
		return FALSE;
	}

	return TRUE;
}

BOOT_RAM_TEXT_SECTION
void BOOT_OTA_Region_Init(void)
{
	flash_get_layout_info(IMG_APP_OTA1, &OTA_Region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &OTA_Region[IMG_CERT][1], NULL);
#ifdef CONFIG_LINUX_FW_EN
	flash_get_layout_info(IMG_APIMG_OTA1, &OTA_Region[IMG_APIMG][0], NULL);
	flash_get_layout_info(IMG_APIMG_OTA2, &OTA_Region[IMG_APIMG][1], NULL);
#endif
	OTA_Region[IMG_IMG2][0] = OTA_Region[IMG_CERT][0] + 0x1000;
	OTA_Region[IMG_IMG2][1] = OTA_Region[IMG_CERT][1] + 0x1000;
}

/**
  * @brief check manifest pattern and version
  * @param none
  * @retval first Extract OTA Idx
  */
u8 BOOT_Extract_SlotSelect(void)
{
	u32 Vertemp;
	s64 VN_CERT; //32-bit full version in OTP
	u8 i, ExtractIdx;

	/* 1. load manifest(Slot A & Slot B) from flash to SRAM, treat OTA1's certificate as Manifest because header 24B is same */
	for (i = 0; i < 2; i++) {
		_memcpy((void *)&Manifest[i], (void *)OTA_Region[IMG_CERT][i], sizeof(Manifest_TypeDef));
		if (_memcmp(Manifest[i].Pattern, CompressFlag, sizeof(CompressFlag)) == 0) {
			Vertemp = (Manifest[i].MajorImgVer << 16) | Manifest[i].MinorImgVer; // get 32-bit full version number
			Ver[i] = (s64)Vertemp;
			/* Ignore two OTA IMG are compressed image and Bootloader extract. */
			break;
		} else {
			Ver[i] = -1;
		}
	}

	if (i < 2) {
		ExtractIdx = i;
		/* slot M need to extract, Get slot N Img version(Certificate use ImagePattern) */
		i = (i + 1) % 2;
		_memcpy((void *)&Manifest[i], (void *)OTA_Region[IMG_CERT][i], sizeof(Manifest_TypeDef));
		if (_memcmp(Manifest[i].Pattern, ImagePattern, sizeof(ImagePattern)) == 0) {
			Vertemp = (Manifest[i].MajorImgVer << 16) | Manifest[i].MinorImgVer; // get 32-bit full version number
			Ver[i] = (s64)Vertemp;
		}
	} else {
		/* No CompressFlag found, No need extract */
		ValidIMGNum = NONEVALIDIMG;
		return 0;
	}

	/* ---------------------------------2 Get anti-rollback version in OTP --------------------------------- */
	VN_CERT = (s64)SYSCFG_OTP_GetRollbackVer();

	/* ---------------------------------3 Check full KeyVerion in certificate--------------------------------- */
	if ((VN_CERT > Ver[0]) && (VN_CERT > Ver[1])) {
		/* No IMG Valid, let BOOT_OTA_IMG2 handle this */
		ValidIMGNum = NONEVALIDIMG;
	} else if ((VN_CERT <= Ver[0]) && (VN_CERT <= Ver[1])) {
		ValidIMGNum = TWOVALIDIMG;
	} else {
		ValidIMGNum = ONEVALIDIMG;
	}

	return ExtractIdx;
}

void BOOT_OTA_Extract(void)
{
	u8 ExtractIdx = BOOT_Extract_SlotSelect();
	u8 OverrideIdx = (ExtractIdx + 1) % 2;

	u32 ExtractAddr = OTA_Region[IMG_CERT][ExtractIdx];
	u32 OverrideAddr = OTA_Region[IMG_CERT][OverrideIdx];
	u32 OverrideStart, OverrideEnd;

	u8 EmpSig[8] = {0};

	Manifest_TypeDef *pManifest = &Manifest[ExtractIdx];
	SubImgInfo_TypeDef SubImgInfo[1];
	u8 ret;

	/* No need to Extract IMG */
	if (ValidIMGNum == NONEVALIDIMG) {
		return;
	}

	if (Ver[ExtractIdx] >= Ver[OverrideIdx]) {
		/* 1. secure Boot Check */
		SubImgInfo[0].Addr = ExtractAddr + MANIFEST_SIZE_4K_ALIGN;
		SubImgInfo[0].Len = pManifest->ImgSize;

		ret = BOOT_Extract_SignatureCheck(pManifest, SubImgInfo, 1);

		/* 2. Extract to override other slot After Check Pass */
		if (ret == TRUE) {
			RTK_LOGI(TAG, "Extract from 0x%x to Override 0x%x, Compress Len is 0x%x\n", SubImgInfo[0].Addr, OverrideAddr, SubImgInfo[0].Len);

			/* OverrideIdx is 0 or 1 */
			flash_get_layout_info(IMG_APP_OTA1 + OverrideIdx, &OverrideStart, &OverrideEnd);
			assert_param(OverrideStart == OverrideAddr);

			bootLzma_buffer_set((u8 *)DDR_BASE);
			bootLzma_main_function(SubImgInfo[0].Addr, OverrideStart, OverrideEnd);
		}

		/* 3. Invalid CompressFlag even sboot check fail. */
		FLASH_TxData(ExtractAddr - SPI_FLASH_BASE, sizeof(EmpSig), EmpSig);
		DCache_Invalidate(ExtractAddr, sizeof(EmpSig));
	}
}

BOOT_RAM_TEXT_SECTION
u8 BOOT_OTA_IMG2(void)
{
	u8 ImgIndex = 0;
	u8 ret, i;
	u32 version;
	/* step 1: init OTA region */
	BOOT_OTA_Region_Init();

#ifdef CONFIG_COMPRESS_OTA_IMG
	/* 1. OTA2 is made up of manifest & an newer compressed OTA1 IMG.
	 * 2. Extract OTA2 to override OTA1 After OTA2 secure boot check pass.
	 * 3. Special pattern in manifest indicate extract is needed, clear pattern to 0 after extract.
	 */
	if (SYSCFG_BootFromNor()) {
		BOOT_OTA_Extract();
	}
#endif

	/* step2: Select Slot according to Cert Version */
	ImgIndex = BOOT_OTA_SlotSelect();

	/* step2.1: boot fail if no valid, trap */
	if (ValidIMGNum == NONEVALIDIMG) {
		goto Fail;
	}

	/* step3: check img from selected idx */
	for (i = 0; i < ValidIMGNum; i++) {

		/* step3.1: check cert if sboot enabled */
		ret = BOOT_CertificateCheck(&Cert[ImgIndex], ImgIndex);

		/* step3.2: try another cert if bigger ver failed and smaller ver is valid */
		if (ret != TRUE) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		/* step3.3: load and check img2 if cert passed */
		ret = BOOT_OTA_LoadIMG2(ImgIndex);

		/* step3.4: try another ver from cert if valid ver exsit */
		if (ret != TRUE) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		} else {
			break;
		}

	}

	if (i == ValidIMGNum) {
		goto Fail;
	}
	version = (u32)(Ver[ImgIndex] & 0xFFFFFFFF);

	RTK_LOGI(TAG, "IMG2 BOOT from OTA %d, Version: %x.%x \n", ImgIndex + 1, ((version >> 16) & 0xFFFF), (version & 0xFFFF));

	return ImgIndex; //verified slot index


Fail:
	RTK_LOGE(TAG, "OTA Certificate & IMG2 invalid, BOOT FAIL!!\n");

	/* clear stack */
	__set_MSP(MSP_RAM_HP);
	_memset((void *)MSPLIM_RAM_HP, 0, MSP_RAM_HP - 128 - MSPLIM_RAM_HP);
	DCache_CleanInvalidate(MSPLIM_RAM_HP, MSP_RAM_HP - 128 - MSPLIM_RAM_HP);

	while (1) {
		DelayMs(1000);
	}

	return 0;
}


BOOT_RAM_TEXT_SECTION
u8 BOOT_OTA_RDP(SubImgInfo_TypeDef *SubImgInfo, u8 Index, u8 ImgIndex)
{
	u8 i;
	u8 Cnt = 0;
	u32 PhyAddr;
	u32 Img2Addr = OTA_Region[IMG_IMG2][ImgIndex];

	/* calculate rdp phy addr */
	PhyAddr = Img2Addr;
	PhyAddr += MANIFEST_SIZE_4K_ALIGN;
	for (i = 0; i < Index; i++) {
		PhyAddr += SubImgInfo[i].Len;
	}

	// Check sub-image pattern and load RDP sub-image
	if ((Cnt = BOOT_LoadRDPImg(&Manifest[ImgIndex], &SubImgInfo[Index], PhyAddr)) == FALSE) {
		goto Fail;
	}

	return Cnt;

Fail:
	RTK_LOGE(TAG, "Fail to load RDP image!\n");
	while (1) {
		DelayMs(1000);
	}
}

BOOT_RAM_TEXT_SECTION
u8 BOOT_OTA_AP(SubImgInfo_TypeDef *SubImgInfo, u8 Index, u8 ImgIndex)
{
	u32 PhyAddr;
	u8 Cnt, i;
	u8 ret;
	u32 Img2Addr = OTA_Region[IMG_IMG2][ImgIndex];
	u32 LogAddr;
	u32 ImgAddr;
	//AP XIP IMG is BL33(RTOS), place at First for MMU remap
	char *APLabel[] = {"AP XIP IMG", "AP BL1 SRAM", "AP BL1 DRAM", "AP FIP"};

	/* calculate ap phy addr */
	PhyAddr = Img2Addr;
	PhyAddr += MANIFEST_SIZE_4K_ALIGN;
	for (i = 0; i < Index; i++) {
		PhyAddr += SubImgInfo[i].Len;
	}

	Cnt = sizeof(APLabel) / sizeof(char *);

	/* remap AP image */
	LogAddr = (u32)__ca32_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_AP_IDX, LogAddr, LogAddr + 0x02000000 - 0x20, PhyAddr);
	RSIP_MMU_Cmd(MMU_AP_IDX, ENABLE);

	/*AP RSIP configurations*/
	BOOT_OTFCheck(LogAddr, LogAddr + 0x02000000 - 0x20, OTF_IV_IMG2_IDX, OTF_AP_IDX);

	ImgAddr = SYSCFG_BootFromNor() ? LogAddr : PhyAddr;

	ret = BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, APLabel, _TRUE); // Check sub-image pattern and load AP sub-image

	if (ret != TRUE) {
		goto Fail;
	}

	return Cnt;

Fail:
	RTK_LOGE(TAG, "Fail to load AP image!\n");
	while (1) {
		DelayMs(1000);
	}
}

BOOT_RAM_TEXT_SECTION
u8 BOOT_OTA_AP_Linux(u8 CertImgIndex)
{
	u32 PhyAddr;
	u8 Cnt, i;
	u8 ret;
	u8 APImgIndex;
	Manifest_TypeDef Manifest;

	char *APLabel[] = {"AP BL1 SRAM", "AP BL1 DRAM", "AP FIP"};

	APImgIndex = CertImgIndex;

	/* no valid AP IMG, boot fail */
	for (i = 0; i < ValidIMGNum; i++) {

		PhyAddr = OTA_Region[IMG_APIMG][APImgIndex];
		BOOT_ImgCopy((void *)&Manifest, (void *)PhyAddr, sizeof(Manifest_TypeDef));

		Cnt = sizeof(APLabel) / sizeof(char *);
		PhyAddr += MANIFEST_SIZE_4K_ALIGN;

		ret = BOOT_LoadSubImage(SubImgInfo, PhyAddr, Cnt, APLabel, _TRUE); // Check sub-image pattern and load AP sub-image

		if (ret != TRUE) {
			APImgIndex = (APImgIndex + 1) % 2;
			continue;
		}

		ret = BOOT_SignatureCheck(&Manifest, SubImgInfo, Cnt, &Cert[CertImgIndex], KEYID_AP); //BL1 ECC verify if need

		if (ret != TRUE) {
			APImgIndex = (APImgIndex + 1) % 2;
			continue;
		} else {
			RTK_LOGI(TAG, "AP BOOT from OTA %d\n", APImgIndex + 1);
			break;
		}

	}

	if (i == ValidIMGNum) {
		goto Fail;
	}

	return 0;

Fail:
	RTK_LOGE(TAG, "Fail to load AP image!\n");
	while (1) {
		DelayMs(1000);
	}
}
