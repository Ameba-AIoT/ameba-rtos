/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_secure_boot.h"
#include "bootloader_km4tz.h"
#include "boot_ota_km4tz.h"
#ifdef CONFIG_COMPRESS_OTA_IMG
#include "ameba_boot_lzma.h"
#endif

static const char *const TAG = "BOOT";

/* Forward declaration for manifest size detection */
static Certificate_TypeDef Cert[2]; //Certificate of SlotA & SlotB
static Manifest_TypeDef Manifest[2]; //Manifest of SlotA & SlotB
u8 Signature[2][SIGN_MAX_LEN];
s64 Ver[2] = {0};  //32-bit full version
extern volatile u8 SecureBootEn;
extern volatile u8 SecureBootEn_PQC;

//static SubImgInfo_TypeDef SubImgInfo[12]; //store sub image addr and length

static const u32 ImagePattern[2] = {
	APP_IMAGE_PATTERN_1, APP_IMAGE_PATTERN_2,
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

static void BOOT_OTA_ValidIMGNum(void)
{
	s64 VN_CERT; //32-bit full version in OTP
	/* --------------------------------- Get bootldr version in OTP if anti-rollback --------------------------------- */
	VN_CERT = BOOT_OTA_GetCertRollbackVer();

	/* --------------------------------- Check full KeyVerion in certificate--------------------------------- */
	if ((VN_CERT > Ver[0]) && (VN_CERT > Ver[1])) {
		ValidIMGNum = NONEVALIDIMG;
	} else if ((VN_CERT <= Ver[0]) && (VN_CERT <= Ver[1])) {
		ValidIMGNum = TWOVALIDIMG;
	} else {
		ValidIMGNum = ONEVALIDIMG;
	}
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
	u8 i;

	/* ----1 load certificate(Slot A & Slot B) from flash to SRAM, get full version and check validation---- */
	for (i = 0; i < 2; i++) {
		BOOT_ImgCopy((void *)&Cert[i], (void *)OTA_Region[IMG_CERT][i], sizeof(Certificate_TypeDef));

		if (_memcmp(Cert[i].Pattern, ImagePattern, sizeof(ImagePattern)) == 0) {

			/* update 8K PQC certificate size */
			if (Cert[i].Ver >= CERT_VERSION_8KB) {
				OTA_Region[IMG_IMG2][i] = OTA_Region[IMG_CERT][i] + CERT_SIZE_8K_ALIGN;
			}

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

	BOOT_OTA_ValidIMGNum();

	return (Ver[0] >= Ver[1]) ? BOOT_FROM_OTA1 : BOOT_FROM_OTA2;
}

fih_ret BOOT_LoadSubImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num, char **ImgName, u8 ErrLog)
{
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	IMAGE_HEADER ImgHdr;
	u32 DstAddr, Len;
	u32 i;

	for (i = 0; i < Num; i++) {
		BOOT_ImgCopy((void *)&ImgHdr, (void *)StartAddr, IMAGE_HEADER_LEN);
		if (_memcmp(ImgHdr.signature, ImagePattern, sizeof(ImagePattern)) != 0) {
			if (ErrLog == TRUE) {
				RTK_LOGI(TAG, "%s Invalid\n", ImgName[i]);
			}
			FIH_RET(FIH_FAILURE);
		}

		DstAddr = ImgHdr.image_addr - IMAGE_HEADER_LEN;
		Len = ImgHdr.image_size + IMAGE_HEADER_LEN;

		/* np rom code jump address is from NP_BOOT_INDEX */
		if (ImgHdr.boot_index == NP_BOOT_INDEX) {
			HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_ADDR_KM4NS, ImgHdr.image_addr);
		}

		/* If not XIP sub-image, then copy it to specific address (include the IMAGE_HEADER)*/
		if ((!IS_FLASH_ADDR(DstAddr)) && (Len > IMAGE_HEADER_LEN)) {
			/* check PQC stack overlap */
			if (is_memory_range_valid(DstAddr, Len, ((u32)__km4tz_bd_ram_end__ - (u32)SBOOT_PQC_STACK_SIZE), (u32)__km4tz_bd_ram_end__)
				&& FIH_NOT_EQ(DISABLE, SecureBootEn_PQC)) {
				RTK_LOGI(TAG, "%s image Size exceed PQC stack\n", ImgName[i]);
				FIH_RET(FIH_FAILURE);
			}
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
	FIH_SET(fih_rc, FIH_SUCCESS);

	FIH_RET(fih_rc);
}

void BOOT_RSIPIvSet(Manifest_TypeDef *Manifest, u32 IV_index)
{
	u8 iv[8] = {0};

	/* 1. check if RSIP enable */
	if (MANIFEST_RSIP_GET_IMG_MODE(MANIFEST_AP_NP_IMG2_ID, Manifest->RSIPConfig) == MANIFEST_RSIP_INVALID_MODE) {
		/* if manifest is invalid, check OTP */
		if (FIH_EQ(FALSE, SYSCFG_OTP_RSIPEn())) {
			return;
		}
	}

	/* get iv from manifest */
	_memcpy(iv, Manifest->RsipIV, 8);
	RSIP_IV_Set(IV_index, iv);
}

/* Load np/ap img and check pattern if secure boot enabled*/
fih_ret BOOT_OTA_LoadIMGAll(u8 ImgIndex)
{
	SubImgInfo_TypeDef SubImgInfo[13];
	u32 LogAddr, PhyAddr, ImgAddr, TotalLen = 0;
	u8 Cnt, i;
	u32 Index = 0;
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	u32 AP_GCM_TagBase = 0;
	u32 NP_GCM_TagBase = 0;

	char *NpLabel[] = {"NP XIP IMG", "NP SRAM", "NP PSRAM"};
	char *ApLabel[] = {"AP XIP IMG", "AP SRAM", "AP PSRAM"};
	char *Name[] = {"IMG1", "IMG2", "IMG3"};

	PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];

	/* set IMG2 IV */
	BOOT_RSIPIvSet(&Manifest[ImgIndex], RSIP_IV1);

	u32 manifest_size = SBOOT_GetManifestSize(&Manifest[ImgIndex]);
	PhyAddr += manifest_size;

#ifndef CONFIG_SOLO
	/* remap NP XIP image. SOLO has no NP portion in this combined image: the
	 * km4ns(iot) app is an independent image loaded by BOOT_OTA_LoadNP. */
	LogAddr = (u32)__km4ns_flash_text_start__ - IMAGE_HEADER_LEN;

	/* If RSIP GCM mode enabled, skip MP image GCM Tag bin length */
	BOOT_ROM_CheckGCM(&Manifest[ImgIndex], &SubImgInfo[Index], PhyAddr, MANIFEST_AP_NP_IMG2_ID);
	if (SubImgInfo[Index].Len) {
		NP_GCM_TagBase = PhyAddr + IMAGE_HEADER_LEN;
		PhyAddr += SubImgInfo[Index].Len;
	}
	Index ++;

	RSIP_MMU_Config(MMU_ID1, LogAddr, (u32)__km4ns_flash_text_end__, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID1, ENABLE);
	RSIP_MMU_Cache_Clean();

	/*NP IMG2 OTF configurations*/
	FIH_CALL(BOOT_ROM_OTFCheck, fih_rc, LogAddr, (u32)__km4ns_flash_text_end__, RSIP_IV1, RSIP_REGION1, RSIP_KEY_NUM1,
			 NP_GCM_TagBase, Manifest[ImgIndex].RSIPConfig, MANIFEST_AP_NP_IMG2_ID);

	/* NP XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(NpLabel) / sizeof(char *);
	ImgAddr = LogAddr;
	FIH_CALL(BOOT_LoadSubImage, fih_rc, &SubImgInfo[Index], ImgAddr, Cnt, NpLabel, TRUE);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	for (i = 0; i < Cnt; i++) {
		TotalLen += SubImgInfo[Index + i].Len;
	}
	Index += Cnt;
#else
	(void)NpLabel;
	(void)NP_GCM_TagBase;
#endif

	/* remap AP XIP image */
	PhyAddr += TotalLen;
	LogAddr = (u32)__km4tz_flash_text_start__ - IMAGE_HEADER_LEN;

	/* If RSIP GCM mode enabled, skip AP image GCM Tag bin length */
	BOOT_ROM_CheckGCM(&Manifest[ImgIndex], &SubImgInfo[Index], PhyAddr, MANIFEST_AP_NP_IMG2_ID);
	if (SubImgInfo[Index].Len) {
		AP_GCM_TagBase = PhyAddr + IMAGE_HEADER_LEN;
		PhyAddr += SubImgInfo[Index].Len;
	}
	Index ++;

	RSIP_MMU_Config(MMU_ID2, LogAddr, (u32)__km4tz_flash_text_end__, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID2, ENABLE);
	RSIP_MMU_Cache_Clean();

	/*AP IMG2 OTF configurations*/
	FIH_CALL(BOOT_ROM_OTFCheck, fih_rc, LogAddr, (u32)__km4tz_flash_text_end__, RSIP_IV1, RSIP_REGION2, RSIP_KEY_NUM1,
			 AP_GCM_TagBase, Manifest[ImgIndex].RSIPConfig, MANIFEST_AP_NP_IMG2_ID);

	/* AP XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(ApLabel) / sizeof(char *);
	ImgAddr = LogAddr;
	FIH_CALL(BOOT_LoadSubImage, fih_rc, &SubImgInfo[Index], ImgAddr, Cnt, ApLabel, TRUE);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	/* Get RDP image Addr */
	for (i = 0; i < Cnt; i++) {
		PhyAddr += SubImgInfo[Index + i].Len;
	}
	Index += Cnt;

#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
	HAL_WRITE32(OTPC_REG_BASE, SEC_OTP_SYSCFG0, HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0) | SEC_BIT_LOGIC_RDP_EN);

	/* Load IMG3 when TrustZone enabled */
	FIH_CALL(BOOT_DecRDPImg, fih_rc, PhyAddr, &Manifest[ImgIndex], &SubImgInfo[Index], &Cnt);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		FIH_RET(fih_rc);
	}
	Index += Cnt;
#endif

	assert_param(Index <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));

	/* IMG2(NP and AP) ECC verify if need */
	FIH_CALL(BOOT_SignatureCheck, fih_rc, &Manifest[ImgIndex], &Cert[ImgIndex], KEYID_NSPE);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	FIH_CALL(BOOT_SignatureCheck_PQC, fih_rc, &Manifest[ImgIndex], &Cert[ImgIndex], ImgIndex, KEYID_NSPE_PQC);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto SBOOT_FAIL;
	}

	if (FIH_EQ(DISABLE, SecureBootEn) && FIH_EQ(DISABLE, SecureBootEn_PQC)) {
		FIH_RET(fih_rc);
	} else {
		/* Calculate and validate image hash */
		FIH_CALL(SBOOT_Validate_ImgHash, fih_rc, Manifest[ImgIndex].HashAlg, Manifest[ImgIndex].ImgHash, SubImgInfo, Index);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			goto SBOOT_FAIL;
		}

		RTK_LOGI(TAG, "%s VERIFY PASS\n", Name[Manifest[ImgIndex].ImgID]);
		FIH_RET(fih_rc);
	}

SBOOT_FAIL:
	RTK_LOGE(TAG, "%s VERIFY FAIL, ret = %d\n", Name[Manifest[ImgIndex].ImgID], fih_rc);
	/* clear copied image */
	for (i = 0; i < Index; i++) {
		if (!IS_FLASH_ADDR(SubImgInfo[i].Addr)) {
			_memset((void *)SubImgInfo[i].Addr, 0, SubImgInfo[i].Len);
			DCache_CleanInvalidate(SubImgInfo[i].Addr, SubImgInfo[i].Len);
		}
	}
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

	BOOT_OTA_ValidIMGNum();

	return ExtractIdx;
}

#ifdef CONFIG_COMPRESS_OTA_IMG
/**
  * @brief  Shared extract: if one of the two slots holds a COMPRESSED image
  *         (manifest Pattern == CompressFlag), decompress it into the other
  *         (override) slot and clear its CompressFlag so it is not re-extracted.
  *         Assumes OTA_Region[IMG_CERT][0/1] already point at the two slot starts
  *         and BOOT_Extract_SlotSelect() has populated Manifest[]/Ver[].
  * @param  ota1_layout_id  layout region id of slot A: IMG_APP_OTA1 (mcu/AP) or
  *                         IMG_NP_OTA1 (SOLO iot); slot B is +1 (enum is contiguous).
  * @param  lzma_scratch    >=32KB RAM (16KB output_buf + ~16KB LZMA probs, colocated
  *                         by bootLzma). MUST NOT be live memory during extraction.
  */
static void BOOT_OTA_ExtractSlot(u32 ota1_layout_id, u8 *lzma_scratch)
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
		u32 extract_manifest_size = SBOOT_GetManifestSize(pManifest);
		SubImgInfo[0].Addr = ExtractAddr + extract_manifest_size;
		SubImgInfo[0].Len = pManifest->ImgSize;

		FIH_CALL(BOOT_Extract_SignatureCheck, fih_rc, pManifest, SubImgInfo, 1);

		/* 2. Extract to override other slot After Check Pass */
		if (FIH_EQ(fih_rc, FIH_SUCCESS)) {
			RTK_LOGI(TAG, "Extract from 0x%x to Override 0x%x, Compress Len is 0x%x\n", SubImgInfo[0].Addr, OverrideAddr, SubImgInfo[0].Len);

			/* OverrideIdx is 0 or 1 */
			flash_get_layout_info(ota1_layout_id + OverrideIdx, &OverrideStart, &OverrideEnd);
			assert_param(OverrideStart == OverrideAddr);

			bootLzma_buffer_set(lzma_scratch);
			bootLzma_main_function(SubImgInfo[0].Addr, OverrideStart, OverrideEnd);
		}

		/* 3. Invalid CompressFlag even sboot check fail. */
		FLASH_TxData(ExtractAddr - SPI_FLASH_BASE, sizeof(EmpSig), EmpSig);
		DCache_Invalidate(ExtractAddr, sizeof(EmpSig));
	}
}

void BOOT_OTA_Extract(void)
{
	/* AP/mcu path: runs at cold boot from BOOT_OTA_IMG (km4tz app image2 not yet
	 * loaded), so the km4tz image2 BD RAM is free scratch. OTA_Region was set by
	 * the preceding BOOT_OTA_Region_Init(). */
	BOOT_OTA_ExtractSlot(IMG_APP_OTA1, __km4tz_bd_ram_start__);
}

#ifdef CONFIG_SOLO
/**
  * @brief  SOLO iot(NP) extract. Called from BOOT_OTA_LoadNP(), which runs BOTH at
  *         cold boot AND from the plfm1-reset NMI. In the plfm1 case the km4tz app
  *         is RUNNING out of the km4tz image2 BD RAM (0x20003000..), so that region
  *         (used by BOOT_OTA_Extract for AP) MUST NOT be scratch here. km4ns is
  *         halted at this point and its whole SRAM is reloaded by BOOT_OTA_LoadNP
  *         immediately after, so __km4ns_bd_ram_start__ is a safe transient LZMA
  *         scratch that never touches live km4tz memory.
  */
static void BOOT_OTA_ExtractNP(void)
{
	flash_get_layout_info(IMG_NP_OTA1, &OTA_Region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_NP_OTA2, &OTA_Region[IMG_CERT][1], NULL);
	BOOT_OTA_ExtractSlot(IMG_NP_OTA1, __km4ns_bd_ram_start__);
}
#endif
#endif

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
	BOOT_OTA_Extract();
#endif

	/* step2: Select Slot according to Cert Version */
	ImgIndex = BOOT_OTA_SlotSelect();

	/* step2.1: boot fail if no valid, trap */
	if (ValidIMGNum == NONEVALIDIMG) {
		goto Fail;
	}

	/* step3: check img from selected idx */
	for (i = 0; i < ValidIMGNum; i++) {

		/* step3.1.1: check cert if sboot enabled */
		FIH_CALL(BOOT_CertificateCheck, fih_rc, &Cert[ImgIndex], ImgIndex);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			RTK_LOGE(TAG, "OTA %u Certificate ECC check fail, try OTA  %u \n", ImgIndex + 1, ((ImgIndex + 1) % 2) + 1);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		/* step3.1.2: check PQC cert if enabled */
		FIH_CALL(BOOT_CertificateCheck_PQC, fih_rc, &Cert[ImgIndex], ImgIndex);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			RTK_LOGE(TAG, "OTA %u Certificate PQC check fail, try OTA  %u \n", ImgIndex + 1, ((ImgIndex + 1) % 2) + 1);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		/* step3.2: load and check img2 if cert passed */
		FIH_CALL(BOOT_OTA_LoadIMGAll, fih_rc, ImgIndex);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			RTK_LOGE(TAG, "OTA %u Load image fail, try OTA %u \n", ImgIndex + 1, ((ImgIndex + 1) % 2) + 1);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		/* step3.3: try another ver from cert if valid ver exist */
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
	RTK_LOGI(TAG, "IMG2 BOOT from OTA %d, Version: %x.%x \n", ImgIndex + 1, ((version >> 16) & 0xFFFF), (version & 0xFFFF));

	/* Save RSIP remap regs to retention RAM so the app can query the running OTA index
	 * (ota_get_cur_index) without accessing RSIP registers. Index by OTA_IMGID_x.
	 * Same reason as OTA_NP_IMG_IDX in BOOT_OTA_LoadNP: the bootloader runs with the MPU
	 * disabled (GBSS cacheable) but D-cache on, so clean the write to SRAM — under SOLO
	 * km4ns reads OTA_IMG_REMAP[] (non-cacheable) and would otherwise get a stale value. */
	GBSS_DEV->OTA_IMG_REMAP[0] = RSIP_BASE->FLASH_MMU[0].RSIP_REMAP_x_OFFSET;
	GBSS_DEV->OTA_IMG_REMAP[1] = RSIP_BASE->FLASH_MMU[1].RSIP_REMAP_x_OFFSET;
	DCache_Clean((u32)GBSS_DEV, sizeof(GBSS_TypeDef));

	return ImgIndex;

Fail:
	RTK_LOGE(TAG, "OTA Certificate & IMG2 invalid, ret = %x, BOOT FAIL!!\n", fih_rc);

	/* step5: stuck and clear msp if boot fail */
	SBOOT_Validate_Fail_Stuck(FALSE);

	return RTK_FAIL;
}

#ifdef CONFIG_SOLO
/**
  * @brief  SOLO: load & verify the independent iot(km4ns) application image2 from
  *         its own IMG_NP_OTA1/2 slots, configure RSIP (MMU_ID1/RSIP_REGION1, freed
  *         from the mcu image's empty NP stub), and point the km4ns boot address at
  *         the iot image2 entry. Runs on km4tz AFTER the mcu(AP) image is loaded (so
  *         the AP globals Cert/Manifest/Ver/OTA_Region are free to reuse) and BEFORE
  *         BOOT_Enable_NP() releases CPU1.
  *
  *         Anti-rollback: BOOT_OTA_ValidIMGNum() (called below) applies the OTP
  *         SEC_BOOT_VER counter (BOOT_OTA_GetCertRollbackVer) against the NP cert
  *         key version, same as the AP path. With the SOLO single-key design (mcu &
  *         iot share the key version) the shared counter gates NP downgrades too.
  *
  *         NOTE: secure-boot / RSIP-OTF paths here are compile-verified only
  *         (OTP-dependent, not board-verified). The MMU_ID1 remap of
  *         __km4ns_app_flash_text_start__ -> selected IMG_NP physical slot is the
  *         part required for the non-secure boot of km4ns.
  * @retval selected NP OTA index, or RTK_FAIL if no valid iot image.
  */
u8 BOOT_OTA_LoadNP(void)
{
	SubImgInfo_TypeDef SubImgInfo[13];
	u32 LogAddr, PhyAddr, ImgAddr, manifest_size, NP_GCM_TagBase = 0;
	u32 Vertemp, Index;
	u8 Cnt, i, ImgIndex = 0;
	FIH_DECLARE(fih_rc, FIH_FAILURE);
	char *NpLabel[] = {"NP XIP IMG", "NP SRAM", "NP PSRAM"};

#ifdef CONFIG_COMPRESS_OTA_IMG
	/* step0: if an iot OTA committed a COMPRESSED image to one NP slot, decompress
	 * it into the other NP slot (and clear its CompressFlag) before the version
	 * select/load below. No-op when neither slot carries the CompressFlag. */
	BOOT_OTA_ExtractNP();
#endif

	/* step1: NP OTA region (cert at slot start, img2 after 4K/8K cert) */
	flash_get_layout_info(IMG_NP_OTA1, &OTA_Region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_NP_OTA2, &OTA_Region[IMG_CERT][1], NULL);
	OTA_Region[IMG_IMG2][0] = OTA_Region[IMG_CERT][0] + CERT_SIZE_4K_ALIGN;
	OTA_Region[IMG_IMG2][1] = OTA_Region[IMG_CERT][1] + CERT_SIZE_4K_ALIGN;

	/* step2: load NP certificate(Slot A & B), get version, select slot */
	for (i = 0; i < 2; i++) {
		BOOT_ImgCopy((void *)&Cert[i], (void *)OTA_Region[IMG_CERT][i], sizeof(Certificate_TypeDef));
		if (_memcmp(Cert[i].Pattern, ImagePattern, sizeof(ImagePattern)) == 0) {
			if (Cert[i].Ver >= CERT_VERSION_8KB) {
				OTA_Region[IMG_IMG2][i] = OTA_Region[IMG_CERT][i] + CERT_SIZE_8K_ALIGN;
			}
			BOOT_ImgCopy((void *)&Signature[i], (void *)(OTA_Region[IMG_CERT][i] + Cert[i].TableSize), SIGN_MAX_LEN);
			BOOT_ImgCopy((void *)&Manifest[i], (void *)OTA_Region[IMG_IMG2][i], sizeof(Manifest_TypeDef));
			Vertemp = ((u16)Cert[i].MajorKeyVer << 16) | (u16)Cert[i].MinorKeyVer;
			Ver[i] = (s64)Vertemp;
		} else {
			Ver[i] = -1;
		}
	}
	BOOT_OTA_ValidIMGNum();
	if (ValidIMGNum == NONEVALIDIMG) {
		goto Fail;
	}
	ImgIndex = (Ver[0] >= Ver[1]) ? BOOT_FROM_OTA1 : BOOT_FROM_OTA2;

	/* step3: for the selected slot, cert check -> RSIP -> load -> signature */
	for (i = 0; i < ValidIMGNum; i++) {
		FIH_CALL(BOOT_CertificateCheck, fih_rc, &Cert[ImgIndex], ImgIndex);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			RTK_LOGE(TAG, "IOT OTA %u Cert ECC fail, try %u\n", ImgIndex + 1, ((ImgIndex + 1) % 2) + 1);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}
		FIH_CALL(BOOT_CertificateCheck_PQC, fih_rc, &Cert[ImgIndex], ImgIndex);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		Index = 0;
		PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];
		BOOT_RSIPIvSet(&Manifest[ImgIndex], RSIP_IV2);

		manifest_size = SBOOT_GetManifestSize(&Manifest[ImgIndex]);
		PhyAddr += manifest_size;
		LogAddr = (u32)__km4ns_app_flash_text_start__ - IMAGE_HEADER_LEN;

		BOOT_ROM_CheckGCM(&Manifest[ImgIndex], &SubImgInfo[Index], PhyAddr, MANIFEST_AP_NP_IMG2_ID);
		if (SubImgInfo[Index].Len) {
			NP_GCM_TagBase = PhyAddr + IMAGE_HEADER_LEN;
			PhyAddr += SubImgInfo[Index].Len;
		}
		Index ++;

		/* SOLO region allocation: BOOT_OTA_LoadIMGAll maps the mcu combined image's
		 * NP-XIP portion on MMU_ID1/RSIP_REGION1, but in SOLO that portion is an
		 * empty stub (the real km4ns moved to this separate iot image). So reuse
		 * ID1/REGION1 for the iot XIP (harmlessly overriding the stub mapping),
		 * which frees MMU_ID3/RSIP_REGION3 for the km4tz secure image
		 * (IMG3/TrustZone also uses ID3/REGION3 - see boot_security_km4tz.c). iot
		 * keeps its own IV (RSIP_IV2) and shares the app key (RSIP_KEY_NUM1) with
		 * the mcu(AP) image, per the SOLO single-key design. */
		RSIP_MMU_Config(MMU_ID1, LogAddr, (u32)__km4ns_app_flash_text_end__, PhyAddr);
		RSIP_MMU_Cmd(MMU_ID1, ENABLE);
		RSIP_MMU_Cache_Clean();

		FIH_CALL(BOOT_ROM_OTFCheck, fih_rc, LogAddr, (u32)__km4ns_app_flash_text_end__, RSIP_IV2, RSIP_REGION1, RSIP_KEY_NUM1,
				 NP_GCM_TagBase, Manifest[ImgIndex].RSIPConfig, MANIFEST_AP_NP_IMG2_ID);

		Cnt = sizeof(NpLabel) / sizeof(char *);
		ImgAddr = LogAddr;
		/* BOOT_LoadSubImage writes REG_LSYS_BOOT_ADDR_KM4NS when it sees the
		 * NP_BOOT_INDEX header, i.e. points km4ns at this iot image2 entry. */
		FIH_CALL(BOOT_LoadSubImage, fih_rc, &SubImgInfo[Index], ImgAddr, Cnt, NpLabel, TRUE);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			/* OTA1/OTA2 share MMU virtual addr; invalidate all D-cache to avoid corner case */
			DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}
		Index += Cnt;

		FIH_CALL(BOOT_SignatureCheck, fih_rc, &Manifest[ImgIndex], &Cert[ImgIndex], KEYID_NSPE);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}
		FIH_CALL(BOOT_SignatureCheck_PQC, fih_rc, &Manifest[ImgIndex], &Cert[ImgIndex], ImgIndex, KEYID_NSPE_PQC);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		if (FIH_EQ(DISABLE, SecureBootEn) && FIH_EQ(DISABLE, SecureBootEn_PQC)) {
			break;
		}
		FIH_CALL(SBOOT_Validate_ImgHash, fih_rc, Manifest[ImgIndex].HashAlg, Manifest[ImgIndex].ImgHash, SubImgInfo, Index);
		if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}
		break;
	}

	if (FIH_EQ(ValidIMGNum, i)) {
		goto Fail;
	}

	/* Record the active iot slot for the running iot_app (read by ota_get_cur_index
	 * on km4ns). This runs in the km4tz bootloader, where the MPU is DISABLED (verified
	 * on board: MPU->CTRL==0) while the D-cache is ENABLED — so the default memory map
	 * applies and GBSS (0x20001380) is CACHEABLE here, unlike in the app where
	 * app_mpu_nocache_init() maps it non-cacheable. Without the clean the write stays in
	 * the km4tz D-cache and never reaches SRAM, so km4ns (which reads GBSS non-cacheable,
	 * straight from SRAM) sees a STALE index -> a later OTA targets the RUNNING slot and
	 * erases it -> hard fault. Clean the line to SRAM here. */
	GBSS_DEV->OTA_NP_IMG_IDX = ImgIndex;
	DCache_Clean((u32)GBSS_DEV, sizeof(GBSS_TypeDef));
	RTK_LOGI(TAG, "IOT(NP) IMG2 BOOT from OTA %d\n", ImgIndex + 1);
	return ImgIndex;

Fail:
	RTK_LOGE(TAG, "IOT(NP) image invalid, km4ns will not boot\n");
	return RTK_FAIL;
}
#endif
