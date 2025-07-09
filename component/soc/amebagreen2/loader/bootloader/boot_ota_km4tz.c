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
#include "bootloader_km4tz.h"
#include "boot_ota_km4tz.h"
#ifdef CONFIG_COMPRESS_OTA_IMG
#include "ameba_boot_lzma.h"
#endif

static const char *const TAG = "BOOT";
static Certificate_TypeDef Cert[2]; //Certificate of SlotA & SlotB
static Manifest_TypeDef Manifest[2]; //Manifest of SlotA & SlotB
u8 Signature[2][SIGN_MAX_LEN];
s64 Ver[2] = {0};  //32-bit full version

//static SubImgInfo_TypeDef SubImgInfo[12]; //store sub image addr and length

static const u32 ImagePattern[2] = {
	0x35393138, 0x31313738,
};

static const u32 CompressFlag[2] = {
	0x504D4F43, 0x53534552,
};

static u8 ValidIMGNum = 0;
/*
* @brif	OTA start address. Because NP & AP IMG2 are combined, users only need to set the start address of NP IMG2.
*  In each entry, the first address is OTA1 address(SlotA address), the second address is OTA2 address (SlotB address)
*/
u32 OTA_Region[IMG_TYPE_CNT][2] = {0};

/**
  * @brief  Get Version Number from OTP
  * @param  none
  * @retval Certificate version in OTP
  */
static s64 BOOT_OTA_GetCertRollbackVer(void)
{
	u32 Anti_VerNum = 0, VER0, VER1;

	/* Cert/app version is same, so Anti_VerNum for cert/app can be same */
	if (SYSCFG_OTP_AntiRollback()) {
		VER0 = HAL_READ32(OTPC_REG_BASE, SEC_BOOT_VER0);
		VER1 = HAL_READ32(OTPC_REG_BASE, SEC_BOOT_VER1);

		for (int i = 0; i < 32; i++) {
			if ((VER0 & BIT(i)) == 0) {
				Anti_VerNum++;
			}

			if ((VER1 & BIT(i)) == 0) {
				Anti_VerNum++;
			}
		}

		/*Only Check MajorImgVer with Anti_VerNum in otp*/
		Anti_VerNum = Anti_VerNum << 16;
	}

	return Anti_VerNum;
}

/**
  * @brief  check Certificate pattern and version
  * @param none
  * @retval 0:Slot A  1:Slot B  2:V1 = V2
  */
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
			Vertemp = (MajorVer[i] << 16) | MinorVer[i]; // get 32-bit full version number
			Ver[i] = (s64)Vertemp;
		} else {
			Ver[i] = -1;
		}
	}

	/* ---------------------------------2 Get bootldr version in OTP if anti-rollback --------------------------------- */
	VN_CERT = BOOT_OTA_GetCertRollbackVer();

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

u8 BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog)
{
	IMAGE_HEADER ImgHdr;
	u32 DstAddr, Len;
	u32 i;

	for (i = 0; i < Num; i++) {
		BOOT_ImgCopy((void *)&ImgHdr, (void *)StartAddr, IMAGE_HEADER_LEN);
		if (_memcmp(ImgHdr.signature, ImagePattern, sizeof(ImagePattern)) != 0) {
			if (ErrLog == TRUE) {
				RTK_LOGI(TAG, "%s Invalid\n", ImgName[i]);
			}
			return FALSE;
		}

		DstAddr = ImgHdr.image_addr - IMAGE_HEADER_LEN;
		Len = ImgHdr.image_size + IMAGE_HEADER_LEN;

		/* np rom code jump address is from NP_BOOT_INDEX */
		if (ImgHdr.boot_index == NP_BOOT_INDEX) {
			HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_ADDR_NS, ImgHdr.image_addr);
		}

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
			RTK_LOGI(TAG, "%s[%08lx:%lx]\n", ImgName[i], DstAddr, Len);
		}

		StartAddr += Len;
	}

	return TRUE;
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

/**
 * @brief  Set GCM Tag addr and tag len
 * @param  start_addr: bus addr (VA) for XIP image
 * @param  tag_addr: bus addr (VA) for tags
 * @param  tag_len: valid value: 4 / 8 / 16 Bytes
 */
void BOOT_OTF_GCM_Set(u32 start_addr, u32 tag_addr, u8 tag_len)
{
	if (SYSCFG_OTP_RSIPEn() == FALSE) {
		return;
	}

#ifndef AMEBAGREEN2_TODO //Cannot get GCM mode from OTP
	if (SYSCFG_OTP_RSIPMode() == RSIP_GCM_MODE) {
		RSIP_OTF_Set_GCM_Tag(start_addr, tag_addr, tag_len);
	}
#endif
}

/* start addr --> logical addr */
fih_ret BOOT_OTFCheck(u32 start_addr, u32 end_addr, u32 IV_index, u32 OTF_index)
{
	u32 mode = SYSCFG_OTP_RSIPMode();

	/* 1. check if RSIP enable */
	if (FIH_EQ(FALSE, SYSCFG_OTP_RSIPEn())) {
		FIH_RET(FIH_SUCCESS);
	}

	switch (mode) {
	case RSIP_CTR_MODE:
		mode = OTF_CTR_MODE;
		break;
#ifndef AMEBAGREEN2_TODO //Cannot get GCM mode from OTP
	case RSIP_GCM_MODE:
		mode = OTF_GCM_MODE;
		break;
#endif
	case RSIP_XTS_MODE:
	default:
		mode = OTF_XTS_MODE;
		break;
	}

	RSIP_OTF_Enable(OTF_index, start_addr, end_addr, ENABLE, IV_index, RSIP_KEY_NUM1, mode);
	RSIP_OTF_Cmd(ENABLE);
	RTK_LOGI(TAG, "IMG2 OTF EN\n");

	FIH_RET(FIH_SUCCESS);
}

/* Load np/ap img and check pattern if secure boot enabled*/
fih_ret BOOT_OTA_LoadIMGAll(u8 ImgIndex)
{
	SubImgInfo_TypeDef SubImgInfo[10];
	u32 LogAddr, PhyAddr, ImgAddr, TotalLen = 0;
	u8 Cnt, i;
	u32 Index = 0;
	FIH_DECLARE(fih_rc, FIH_FAILURE);

	char *NpLabel[] = {"NP XIP IMG", "NP SRAM", "NP PSRAM"};
	char *ApLabel[] = {"AP XIP IMG", "AP SRAM", "AP PSRAM"};

	PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];

	/* set IMG2 IV */
	BOOT_RSIPIvSet(&Manifest[ImgIndex], RSIP_IV1);

	/* remap NP XIP image */
	PhyAddr += MANIFEST_SIZE_4K_ALIGN;
	LogAddr = (u32)__km4ns_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_ID1, LogAddr, (u32)__km4ns_flash_text_end__, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID1, ENABLE);
	RSIP_MMU_Cache_Clean();

	/* check set GCM settings */
	BOOT_OTF_GCM_Set((u32)__km4tz_flash_text_start__ - IMAGE_HEADER_LEN, SPI_FLASH_BASE + 0x100000, 4);

	/*NP IMG2 RSIP configurations*/
	FIH_CALL(BOOT_OTFCheck, fih_rc, LogAddr, (u32)__km4ns_flash_text_end__, RSIP_IV1, RSIP_REGION1);

	/* NP XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(NpLabel) / sizeof(char *);
	ImgAddr = SYSCFG_OTP_BootFromNor() ? LogAddr : PhyAddr;
	if (BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, NpLabel, TRUE) == FALSE) {
		FIH_RET(fih_rc);
	}

	for (i = 0; i < Cnt; i++) {
		TotalLen += SubImgInfo[i].Len;
	}
	Index += Cnt;

	/* Remove 4K-align padding info for nand secure boot */

	/* remap AP XIP image */
	PhyAddr += TotalLen;
	LogAddr = (u32)__km4tz_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_ID2, LogAddr, (u32)__km4tz_flash_text_end__, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID2, ENABLE);
	RSIP_MMU_Cache_Clean();

	FIH_CALL(BOOT_OTFCheck, fih_rc, LogAddr, (u32)__km4tz_flash_text_end__, RSIP_IV1, RSIP_REGION2);

	/* AP XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(ApLabel) / sizeof(char *);
	ImgAddr = SYSCFG_OTP_BootFromNor() ? LogAddr : PhyAddr;
	if (BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, ApLabel, TRUE) == FALSE) {
		FIH_RET(fih_rc);
	}

	/* Get RDP image Addr */
	for (i = 0; i < Cnt; i++) {
		PhyAddr += SubImgInfo[Index + i].Len;
	}
	Index += Cnt;

#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
#ifndef AMEBAGREEN2_TODO
	HAL_WRITE32(OTPC_REG_BASE, SEC_OTP_SYSCFG0, HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) | SEC_BIT_LOGIC_RDP_EN);
#endif

	/* check if RDP enable */
	if (SYSCFG_OTP_RDPEn() == TRUE) {
		if (BOOT_DecRDPImg(PhyAddr, Manifest[ImgIndex].RsipIV, &SubImgInfo[Index], &Cnt) == FALSE) {
			FIH_RET(fih_rc);
		}
		Index += Cnt;
	} else {
		RTK_LOGE(TAG, "RDP Shall En when TZ Configed.\n");
		assert_param(FALSE);
	}
#endif

	assert_param(Index <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));

	/* IMG2(NP and AP) ECC verify if need */
	FIH_CALL(BOOT_SignatureCheck, fih_rc, &Manifest[ImgIndex], SubImgInfo, Index, &Cert[ImgIndex], KEYID_NSPE);

	FIH_RET(fih_rc);
}

void BOOT_OTA_Region_Init(void)
{

	flash_get_layout_info(IMG_APP_OTA1, &OTA_Region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &OTA_Region[IMG_CERT][1], NULL);

	OTA_Region[IMG_IMG2][0] = OTA_Region[IMG_CERT][0] + CERT_SIZE_4K_ALIGN;
	OTA_Region[IMG_IMG2][1] = OTA_Region[IMG_CERT][1] + CERT_SIZE_4K_ALIGN;
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
	VN_CERT = (s64)BOOT_OTA_GetCertRollbackVer();

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

#ifdef CONFIG_COMPRESS_OTA_IMG
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
	FIH_DECLARE(fih_rc, FIH_FAILURE);

	/* No need to Extract IMG */
	if (ValidIMGNum == NONEVALIDIMG) {
		return;
	}

	if (Ver[ExtractIdx] >= Ver[OverrideIdx]) {
		/* 1. secure Boot Check */
		SubImgInfo[0].Addr = ExtractAddr + MANIFEST_SIZE_4K_ALIGN;
		SubImgInfo[0].Len = pManifest->ImgSize;

		FIH_CALL(BOOT_Extract_SignatureCheck, fih_rc, pManifest, SubImgInfo, 1);

		/* 2. Extract to override other slot After Check Pass */
		if (FIH_EQ(fih_rc, FIH_SUCCESS)) {
			RTK_LOGI(TAG, "Extract from 0x%x to Override 0x%x, Compress Len is 0x%x\n", SubImgInfo[0].Addr, OverrideAddr, SubImgInfo[0].Len);

			/* OverrideIdx is 0 or 1 */
			flash_get_layout_info(IMG_APP_OTA1 + OverrideIdx, &OverrideStart, &OverrideEnd);
			assert_param(OverrideStart == OverrideAddr);

			bootLzma_buffer_set(__km4tz_bd_ram_start__);
			bootLzma_main_function(SubImgInfo[0].Addr, OverrideStart, OverrideEnd);
		}

		/* 3. Invalid CompressFlag even sboot check fail. */
		FLASH_TxData(ExtractAddr - SPI_FLASH_BASE, sizeof(EmpSig), EmpSig);
		DCache_Invalidate(ExtractAddr, sizeof(EmpSig));
	}
}
#endif

void Boot_Fullmac_LoadImage(void)
{
	SubImgInfo_TypeDef SubImgInfo[3];
	u32 LogAddr, PhyAddr, ImgAddr, TotalLen = 0;
	u8 Cnt, i;
	FIH_DECLARE(fih_rc, FIH_FAILURE);

	char *ApLabel[] = {"AP XIP IMG", "AP SRAM", "AP PSRAM"};

	// 1. for sram recycle, manifest is locate after image bin.
	// 2. Do not support RSIP-OTF, manifest cannot be found for image Header is encrypted.
	assert_param(SYSCFG_OTP_BootFromNor());

	/* step 1: init OTA region */
	BOOT_OTA_Region_Init();

	/* remap AP XIP image */
	PhyAddr = OTA_Region[IMG_CERT][0];
	LogAddr = (u32)__km4tz_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_ID2, LogAddr, (u32)__km4tz_flash_text_end__, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID2, ENABLE);
	RSIP_MMU_Cache_Clean();

	Cnt = sizeof(ApLabel) / sizeof(char *);
	ImgAddr = SYSCFG_OTP_BootFromNor() ? LogAddr : PhyAddr;
	if (BOOT_LoadSubImage(&SubImgInfo[0], ImgAddr, Cnt, ApLabel, TRUE) == FALSE) {
		goto Fail;
	}

	assert_param(SubImgInfo[1].Addr == ((u32)__image2_entry_func__ - IMAGE_HEADER_LEN));
	assert_param(SubImgInfo[2].Len == IMAGE_HEADER_LEN);

	for (i = 0; i < Cnt; i++) {
		TotalLen += SubImgInfo[i].Len;
	}
	PhyAddr += TotalLen;

	/* 2. locate manifest */
	BOOT_ImgCopy((void *)&Manifest[0], (void *)PhyAddr, sizeof(Manifest_TypeDef));

	/* 3. verify signature */
	assert_param(Cnt <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));
	FIH_CALL(BOOT_Extract_SignatureCheck, fih_rc, &Manifest[0], &SubImgInfo[0], Cnt);
	if (FIH_EQ(fih_rc, FIH_SUCCESS)) {
		return;
	}

Fail:
	RTK_LOGE(TAG, "FULLMAC IMG2 Load IMG Fail!\n");
	/* stuck and clear msp if boot fail */
	SBOOT_Validate_Fail_Stuck(FALSE);
}

u8 BOOT_OTA_IMG(void)
{
	u8 i, ImgIndex = 0;
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u32 version;

	/* step 1: init OTA region */
	BOOT_OTA_Region_Init();

#ifdef CONFIG_COMPRESS_OTA_IMG
	/* 1. OTA2 is made up of manifest & an newer compressed OTA1 IMG.
	 * 2. Extract OTA2 to override OTA1 After OTA2 secure boot check pass.
	 * 3. Special pattern in manifest indicate extract is needed, clear pattern to 0 after extract.
	 */
	if (SYSCFG_OTP_BootFromNor()) {
		BOOT_OTA_Extract();
	}
#endif

	/* step2: Select Slot according to Cert Version */
	ImgIndex = BOOT_OTA_SlotSelect();

	/* step2.1: boot fail if no valid, trap */
	if (ValidIMGNum == NONEVALIDIMG) {
		goto Fail;
	}

	version = (u32)(Ver[ImgIndex] & 0xFFFFFFFF);
	/* step3: check img from selected idx */
	for (i = 0; i < ValidIMGNum; i++) {

		/* step3.1: check cert if sboot enabled */
		FIH_CALL(BOOT_CertificateCheck, fih_rc, &Cert[ImgIndex], ImgIndex);

		/* step3.2: try another cert if bigger ver failed and smaller ver is valid */
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		/* step3.3: load and check img2 if cert passed */
		FIH_CALL(BOOT_OTA_LoadIMGAll, fih_rc, ImgIndex);

		/* step3.4: try another ver from cert if valid ver exist */
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			/* OTA1 and OTA2 share one MMU virtual address. when image header of one OTA is error, will cause another OTA fail, here invalid all D-cache to avoid corner case. */
			DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		} else {
			break;
		}
	}

	/* step4: check if boot pass */
	if (FIH_EQ(ValidIMGNum, i)) {
		goto Fail;
	}

	version = (u32)(Ver[ImgIndex] & 0xFFFFFFFF);
	RTK_LOGI(TAG, "IMG2 BOOT from OTA %d, Version: %ld.%ld \n", ImgIndex + 1, ((version >> 16) & 0xFFFF), (version & 0xFFFF));
	return ImgIndex;

Fail:
	RTK_LOGE(TAG, "OTA Certificate & IMG2 invalid, ret = %x, BOOT FAIL!!\n", fih_rc);

	/* step5: stuck and clear msp if boot fail */
	SBOOT_Validate_Fail_Stuck(FALSE);

	return RTK_FAIL;
}

