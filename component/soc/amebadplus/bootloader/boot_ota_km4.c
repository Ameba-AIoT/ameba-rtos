/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "boot_security_km4.h"
#include "boot_ota_km4.h"

static const char *TAG = "BOOT";
static Certificate_TypeDef Cert[2]; //Certificate of SlotA & SlotB
static Manifest_TypeDef Manifest[2]; //Manifest of SlotA & SlotB
u8 Signature[2][SIGN_MAX_LEN];
s64 Ver[2] = {0};  //32-bit full version

//static SubImgInfo_TypeDef SubImgInfo[12]; //store sub image addr and length

static const u32 ImagePattern[2] = {
	0x35393138,
	0x31313738,
};

static u8 ValidIMGNum = 0;
/*
* @brif	OTA start address. Because KM0 & KM4 IMG2 are combined, users only need to set the start address of KM0 IMG2.
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
		_memcpy((void *)&Cert[i], (void *)OTA_Region[IMG_CERT][i], sizeof(Certificate_TypeDef));

		if (_memcmp(Cert[i].Pattern, ImagePattern, sizeof(ImagePattern)) == 0) {
			/*do signature copy only when cert valid*/
			_memcpy((void *)&Signature[i], (void *)(OTA_Region[IMG_CERT][i] + Cert[i].TableSize), SIGN_MAX_LEN);
			_memcpy((void *)&Manifest[i], (void *)OTA_Region[IMG_IMG2][i], sizeof(Manifest_TypeDef)); // load img2 manifest together

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
		_memcpy((void *)&ImgHdr, (void *)StartAddr, IMAGE_HEADER_LEN);

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

			RTK_LOGI(TAG, "%s[%08lx:%lx]\n", ImgName[i], DstAddr, Len);
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

/* Load km0/km4 img and check pattern if secure boot enabled*/
u8 BOOT_OTA_LoadIMGAll(u8 ImgIndex)
{
	SubImgInfo_TypeDef SubImgInfo[11];
	u32 LogAddr, PhyAddr, ImgAddr, TotalLen = 0;
	u8 Cnt;
	u8 i;
	u32 Index = 0;
	u8 ret;

	char *Km0Label[] = {"KM0 XIP IMG", "KM0 SRAM", "KM0 PSRAM", "KM0 ENTRY"};
	char *Km4Label[] = {"KM4 XIP IMG", "KM4 SRAM", "KM4 PSRAM", "KM4 ENTRY"};

	PhyAddr = OTA_Region[IMG_IMG2][ImgIndex];

	/* set IMG2 IV */
	BOOT_RSIPIvSet(&Manifest[ImgIndex], RSIP_IV1);

	/* remap KM0 XIP image */
	PhyAddr += MANIFEST_SIZE_4K_ALIGN;
	LogAddr = (u32)__km0_flash_text_start__ - IMAGE_HEADER_LEN;

	RSIP_MMU_Config(MMU_ID1, LogAddr, LogAddr + 0x01000000 - 0x20, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID1, ENABLE);
	RSIP_MMU_Cache_Clean();
	/*KM0 IMG2 RSIP configurations*/
	BOOT_OTFCheck(LogAddr, LogAddr + 0x01000000 - 0x20, RSIP_IV1, RSIP_REGION1);

	/* KM0 XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(Km0Label) / sizeof(char *);
	ImgAddr = LogAddr;
	if (BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, Km0Label, _TRUE) == FALSE) {
		return FALSE;
	}

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

	RSIP_MMU_Config(MMU_ID2, LogAddr, LogAddr + 0x01000000 - 0x20, PhyAddr);
	RSIP_MMU_Cmd(MMU_ID2, ENABLE);
	RSIP_MMU_Cache_Clean();

	BOOT_OTFCheck(LogAddr, LogAddr + 0x01000000 - 0x20, RSIP_IV1, RSIP_REGION2);

	/* KM4 XIP & SRAM, read with virtual addr in case of encryption */
	Cnt = sizeof(Km4Label) / sizeof(char *);
	ImgAddr = LogAddr;
	if (BOOT_LoadSubImage(&SubImgInfo[Index], ImgAddr, Cnt, Km4Label, _TRUE) == FALSE) {
		return FALSE;
	}

	/* Get RDP image Addr */
	for (i = 0; i < Cnt; i++) {
		PhyAddr += SubImgInfo[Index + i].Len;
	}
	Index += Cnt;

	/* check if RDP enable */
	if (SYSCFG_OTP_RDPEn() == TRUE) {
		if (BOOT_DecRDPImg(PhyAddr, Manifest[ImgIndex].RsipIV, &SubImgInfo[Index], &Cnt) == FALSE) {
			return FALSE;
		}
		Index += Cnt;
	} else {
#if defined (CONFIG_TRUSTZONE_EN) && (CONFIG_TRUSTZONE_EN == 1U)
		RTK_LOGI(TAG, "RDP Shall En when TZ Configed.\n");
		assert_param(FALSE);
#endif
	}

	assert_param(Index <= sizeof(SubImgInfo) / sizeof(SubImgInfo_TypeDef));

	/* IMG2(KM0 and KM4) ECC verify if need */
	ret = BOOT_SignatureCheck(&Manifest[ImgIndex], SubImgInfo, Index, &Cert[ImgIndex], KEYID_NSPE);

	if (ret != 0) {
		return FALSE;
	}
	return TRUE;
}

void BOOT_OTA_Region_Init(void)
{
	flash_get_layout_info(IMG_APP_OTA1, &OTA_Region[IMG_CERT][0], NULL);
	flash_get_layout_info(IMG_APP_OTA2, &OTA_Region[IMG_CERT][1], NULL);

	OTA_Region[IMG_IMG2][0] = OTA_Region[IMG_CERT][0] + CERT_SIZE_4K_ALIGN;
	OTA_Region[IMG_IMG2][1] = OTA_Region[IMG_CERT][1] + CERT_SIZE_4K_ALIGN;
}

u8 BOOT_OTA_IMG(void)
{
	u8 ImgIndex = 0;
	u8 ret, i;
	u32 version;

	/* step 1: init OTA region */
	BOOT_OTA_Region_Init();

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
		ret = BOOT_CertificateCheck(&Cert[ImgIndex], ImgIndex);

		/* step3.2: try another cert if bigger ver failed and smaller ver is valid */
		if (ret != 0) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		}

		/* step3.3: load and check img2 if cert passed */
		ret = BOOT_OTA_LoadIMGAll(ImgIndex);

		/* step3.4: try another ver from cert if valid ver exsit */
		if (ret != TRUE) {
			ImgIndex = (ImgIndex + 1) % 2;
			continue;
		} else {
			break;
		}
	}

	/* step4: check if boot pass */
	if (i == ValidIMGNum) {
		goto Fail;
	}

#ifdef CONFIG_RDP_TFM
	PRAM_START_FUNCTION Image3EntryFun = (PRAM_START_FUNCTION)__ram_image3_start__;
	Image3EntryFun->VectorNS = (u32)&Manifest[ImgIndex];
#endif

	version = (u32)(Ver[ImgIndex] & 0xFFFFFFFF);
	RTK_LOGI(TAG, "IMG2 BOOT from OTA %d, Version: %lx.%lx \n", ImgIndex + 1, ((version >> 16) & 0xFFFF), (version & 0xFFFF));
	return ImgIndex;

Fail:
	RTK_LOGE(TAG, "OTA Certificate & IMG2 invalid, BOOT FAIL!!\n");

	/* step5: stuck and clear msp if boot fail */
	BOOT_Validate_Fail_Stuck();

	return 0;
}

