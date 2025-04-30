/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "xmodem_update_rom.h"
#include "ameba_secure_boot.h"

HAL_ROM_DATA_SECTION static const u8 ROM_IMG1_VALID_PATTEN[] = {
	0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff
};

HAL_ROM_BSS_SECTION_BANK u8 DerivedKey[16];

HAL_ROM_TEXT_SECTION
void BOOT_ROM_OTFCheck(Manifest_TypeDef *Manifest, u32 Addr)
{
	u32 len, endaddr;
	u32 mode;
	u8 iv[8] = {0};

	/* 1. check if RSIP enable */
	if (SYSCFG_OTP_RSIPEn() == FALSE) {
		return;
	}

	_memcpy(iv, Manifest->RsipIV, 8);

	/* boot all.bin size - manifest.bin size */
	len = Manifest->ImgSize;

	/* section shuold be 32B aligned */
	endaddr = (Addr + len + 1) & (~0x1F);

	if (len > (IMAGE_HEADER_LEN * 2) && IS_FLASH_ADDR(Addr)) {
		DBG_8195A("OTF EN\n");
		mode = SYSCFG_OTP_RSIPMode();

		switch (mode) {
		case RSIP_XTS_MODE:
			mode = OTF_XTS_MODE;
			break;
		case RSIP_CTR_MODE:
			mode = OTF_CTR_MODE;
			break;
		default:
			DBG_8195A("OTF Mode error\r\n");
			return;
		}

		RSIP_OTF_init(OTF_KEY_256);
		RSIP_IV_Set(OTF_IV_BOOTLOADER_IDX, iv);
		RSIP_OTF_Enable(OTF_BOOTLOADER_IDX, Addr, endaddr, ENABLE, OTF_IV_BOOTLOADER_IDX, mode);
		RSIP_OTF_Cmd(ENABLE);
	}

}

/*
  *This function won't return in case CRC check error
*/
HAL_ROM_TEXT_SECTION
void BOOT_ROM_OTPCheck(void)
{
	OTPC_TypeDef *OTPC  = OTPC_DEV;

	/*OTP CRC check Error, Trap here and never return*/
	if (OTPC->OTPC_OTP_CRC_RESULT & OTPC_BIT_CRC_FAIL) {

		DBG_8195A("Expect OTP CRC 0x%04x PG CRC 0x%04x\r\n",    \
				  OTPC_GET_CRC_DATA_ACT(OTPC->OTPC_OTP_CRC_DATA),  \
				  OTPC_GET_CRC_DATA_EXP(OTPC->OTPC_OTP_CRC_DATA));

		DBG_8195A("PG CRC Entry %x Magic 0x%04x\r\n",    \
				  OTPC_GET_CRC_ENTRY_EN(OTPC->OTPC_OTP_CRC_RESULT),  \
				  OTPC_GET_CRC_MAGIC(OTPC->OTPC_OTP_CRC_RESULT));

		while (1) {
			DelayMs(1000);
		}
	}
}

HAL_ROM_TEXT_SECTION
u32 BOOT_ROM_PatchCmd(ROM_PATCH_TABLE *pwr_cmd)
{
	u32 value = 0;
	u32 polling_count = 0;
	u32 address = 0;
	// _SUCCESS(1)/ _FAIL(0) removed from SDK, change return value to RTK_SUCCESS(0) / RTK_FAIL(-1) in the future
	u32 ret = 1;

	switch (pwr_cmd->cmd) {
	case ROM_PATCH_CMD_READ:
		address = pwr_cmd->address;

		value = HAL_READ32(0, address);

		DBG_8195A("[%x]: %x\n", address, value);
		break;

	case ROM_PATCH_CMD_WRITE:
		address = pwr_cmd->address;

		/*  Read the value from system register */
		value = HAL_READ32(0, address);

		value &= ~pwr_cmd->bitsc;
		value |= (pwr_cmd->bitss);

		/* Write the value back to sytem register */
		HAL_WRITE32(0, address, value);
		break;

	case ROM_PATCH_CMD_POLLING:
		address = pwr_cmd->address;

		do {
			value = HAL_READ32(0, address);

			value &= pwr_cmd->bitss;

			if (value == pwr_cmd->bitsc) {
				break;
			} else {
				DelayUs(10);
			}

			if (polling_count++ > 500) {
				DBG_8195A("Fail to Poll %x %x\n", address, value);
				return FALSE;
			}
		} while (1);

		break;

	case ROM_PATCH_CMD_DELAY:
		DelayUs(pwr_cmd->bitss);
		break;

	case ROM_PATCH_CMD_INVALID:
		break;

	case ROM_PATCH_CMD_END:
	default:
		ret = 0;
		break;
	}

	return ret;
}

/*efuse content
 * addr: 00----------------------------------->0F
 * data :12 34 56 78 9a bc de f0 01 02 03 04 05 06 07 08
 * addr: 0x78563412
 * cmd :0xbc
 * index: 0xF0de
 * bitsc :0x04030201
 * bitss: 0x08070605
*/
HAL_ROM_TEXT_SECTION
u32 BOOT_ROM_PatchEntry(u32 index)
{
	ROM_PATCH_TABLE *pwr_cmd = NULL;
	u32 ret = 1;

	/* Check whether need trigger Rom Patch */
	if (SYSCFG_OTP_RomPatchEn() == FALSE) {
		return 1;
	}

	/* get rom patch table from lp ram */
	pwr_cmd = (ROM_PATCH_TABLE *) HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_SW_PATCH);

	/*In case sys reset won't reset Lp on ,then the patch won't do autoload*/
	if (pwr_cmd == NULL) {
		goto exit;
	}

	/*excute the patch code*/
	do {
		/* rom patch empty */
		if (pwr_cmd->address == 0xFFFFFFFF) {
			goto exit;
		}

		/*check index && check valid*/
		if (pwr_cmd->index == index) {
			ret = BOOT_ROM_PatchCmd(pwr_cmd);
		}

		if (ret == 0) {
			goto exit;
		}

		pwr_cmd++;
	} while (1);

exit:
	return 1;
}

/* boot_finish time = (2^16)/40 = 1.6ms when no speedup, when speed up 256 cycles will be used */
/* 16 is defined in SPIC config form, 40 is SPIC IP clock */
/* after SPIC clock & function enable, auto mode read */
/* will be stucked when boot_finish not ready, */
/* but user mode will hang if boot_finish not ready */
HAL_ROM_TEXT_SECTION
void BOOT_ROM_SPUFlash(void)
{
	u32 temp = 0;
	u32 delay_time;

	/* Should decide to speedup or not before spic clk&func enable */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_LPLAT_CTRL);
	if (SYSCFG_OTP_SPICBootSpeedUpDis()) {
		temp &= ~LSYS_BIT_SPIC_SPDUP_SIM;
		delay_time = 1650;
	} else {
		temp |= LSYS_BIT_SPIC_SPDUP_SIM;
		delay_time = 10;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_LPLAT_CTRL, temp);

	/* spic clk&func enable */
	RCC_PeriphClockCmd(APBPeriph_FLASH, APBPeriph_FLASH_CLOCK, ENABLE);
	RCC_PeriphClockSource_SPIC(BIT_LSYS_CKSL_SPIC_XTAL);

	/* delay to ensure spic boot delay time expires */
	DelayUs(delay_time);
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_InitFlash(void)
{
	u8 InitBaudRate = 1;
	u8 PinmuxSel = PINMUX_S1;
	u32 IsTestMode = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_SYSTEM_CFG0) & LSYS_BIT_PTRP_TMODE;
	u32 TestMode = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_SYSTEM_CFG1) & LSYS_MASK_PTRP_ICFG;


	/*EFUSE: 0: 40M/(2*(0 + 1)), 1: 40M/(2*(1 + 1)) */
	InitBaudRate = SYSCFG_OTP_BootFlashBaud() + 1;

	DBG_8195A("FLASHRATE:%x\n", InitBaudRate);

	/* flash pinmux S0 for FORCE_USE_S0 TEST_MODE only. This mode is used to test MCM FT with CPU, which needs to boot from ext. flash(S0)  */
	if (IsTestMode && ((TestMode  == BIT_FORCE_USE_S0_ICFG) || (TestMode == BIT_MCM_FLASH_PG1B_ICFG))) {
		PinmuxSel = PINMUX_S0;
	}

	BOOT_ROM_PatchEntry(__LINE__);

	/* Boot stop for MCM_FLASH_PG test mode, in which flash is controlled by external socket.*/
	if (IsTestMode && ((TestMode == BIT_MCM_FLASH_PG_ICFG) || (TestMode == BIT_MCM_FLASH_PG1B_ICFG))) {
		PAD_SpicCtrl(PinmuxSel);

		if (TestMode == BIT_MCM_FLASH_PG1B_ICFG) {
			Pinmux_SpicCtrl(PINMUX_S1, ON);
		}

		while (1);
	}

	/* flash pin mux */
	Pinmux_SpicCtrl(PinmuxSel, ON);

	if (SYSCFG_BootFromNor()) {
		DBG_8195A("BOOT FROM NOR\n");

		FLASH_StructInit(&flash_init_para);
		flash_init_para.FLASH_pinmux = PinmuxSel;
		flash_init_para.FLASH_baud_rate = InitBaudRate; /* from EFUSE */
		flash_init_para.FLASH_baud_boot = InitBaudRate;

		if (SYSCFG_OTP_SPICAddr4ByteEn()) {
			flash_init_para.FLASH_addr_phase_len = ADDR_4_BYTE;
			DBG_8195A("FLASH ADDR 4B EN\n");
		}

		BOOT_ROM_PatchEntry(__LINE__);

		FLASH_Init(SpicOneBitMode);

		/* (1) soc deepstadby or deepsleep need it, or wakeup will fail
		(2) wait flash busy done to prevent Erase/Write is ongoing before reset, or auto read will fail */
		if (SYSCFG_OTP_FlashDSleepEn()) {
			FLASH_DeepPowerDown(DISABLE);
		}

		if (flash_init_para.FLASH_addr_phase_len == ADDR_4_BYTE) {
			FLASH_WriteEn();
			FLASH_TxCmd(FLASH_CMD_ENT_ADDR4B, 0, NULL);
			FLASH_WaitBusy(WAIT_FLASH_BUSY);
		}
	} else {
		DBG_8195A("BOOT FROM NAND\n");

		NAND_StructInit(&flash_init_para);
		flash_init_para.FLASH_pinmux = PinmuxSel;
		flash_init_para.FLASH_baud_rate = InitBaudRate; /* from EFUSE */
		flash_init_para.FLASH_baud_boot = InitBaudRate;

		BOOT_ROM_PatchEntry(__LINE__);
		if (SYSCFG_OTP_BigPageNand()) {
			flash_init_para.FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_4K_BIT_EXP;
		} else {
			flash_init_para.FLASH_pagesize_main_bit_exp = NAND_PAGE_SIZE_MAIN_2K_BIT_EXP;
		}

		NAND_Init(SpicOneBitMode);

		/*WinBond has Deepsleep Function.*/
	}

	BOOT_ROM_PatchEntry(__LINE__);

}

HAL_ROM_TEXT_SECTION
u32 BOOT_ROM_CheckBadBlock(u32 block_id)
{
	u8 oob[4];

	/*bad block: the first byte of spare area of the fisrt page of block is not 0xFF*/
	NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
	if ((oob[0] != 0xFF) || (oob[1] != 0xFF)) {
		DBG_8195A("Block 0x%x is Bad Block!\n", block_id);
		return TRUE;
	}
	return FALSE;
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_NandCopy(u8 *pData, u8 *HostAddr, size_t DataLen)
{
	u32 PageAddr, ByteAddr, ByteLen, BlockId;
	u32 NandAddr = (u32)HostAddr - SPI_FLASH_BASE;

	u32 page_begin = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
	u32 page_end = NAND_ADDR_TO_PAGE_ADDR(NandAddr + DataLen - 1);
	u32 page_cnt = page_end - page_begin + 1;
	u32 g_erase_blockid = 0;
	u32 pData_Offset = 0;

	while (page_cnt) {
		/*Maybe lookup the L2P table to find the mapping between logic addr and physical addr. */
		PageAddr = NAND_ADDR_TO_PAGE_ADDR(NandAddr);
		BlockId = NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr);
		if (BlockId >= g_erase_blockid) {
			while (BOOT_ROM_CheckBadBlock(BlockId)) {
				/*jump to next block when current block is bad block*/
				BlockId = BlockId + 1;
				PageAddr += NAND_BLOCK_PAGE_CNT;
				NandAddr += NAND_BLOCK_PAGE_CNT * NAND_PAGE_SIZE_MAIN;
			}

			g_erase_blockid = BlockId + 1; /* next block we should check */
		}

		ByteAddr = NAND_ADDR_TO_BYTE_ADDR(NandAddr);
		ByteLen = (page_cnt == 1) ? DataLen : (NAND_PAGE_SIZE_MAIN - ByteAddr);

		/*Read Data from NandAddr to NandAddr+DataLen, and put the return data into pData[pData_Offset].*/
		u8 status = NAND_Page_Read(PageAddr, ByteAddr, ByteLen, &pData[pData_Offset]);
		if (0 != (status & NAND_STATUS_ECC_MASK)) {
			DBG_PRINTF(MODULE_BOOT, LEVEL_INFO, "Read Page 0x%x of Block 0x%x May Fail, status 0x%x!\n",
					   PageAddr & NAND_BLOCK_PAGE_MASK, NAND_PAGE_ADDR_TO_BLOCK_ID(PageAddr), status);
		}

		pData_Offset = pData_Offset + ByteLen;
		NandAddr = NandAddr + ByteLen;
		DataLen = DataLen - ByteLen;
		page_cnt--;
	}
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_Copy(void *__restrict dst0, const void *__restrict src0, size_t len0)
{
	if (SYSCFG_BootFromNor()) {
		_memcpy(dst0, src0, len0);
	} else {
		BOOT_ROM_NandCopy((u8 *)dst0, (u8 *)src0, len0);
	}
}

HAL_ROM_TEXT_SECTION
int BOOT_ROM_LoadImage(SubImgInfo_TypeDef *SubImgInfo, u32 StartAddr, u8 Num)
{
	IMAGE_HEADER ImgHdr;
	u32 DstAddr, Len;
	u32 i;

	for (i = 0; i < Num; i++) {
		BOOT_ROM_Copy((void *)&ImgHdr, (void *)StartAddr, IMAGE_HEADER_LEN);

		DstAddr = ImgHdr.image_addr - IMAGE_HEADER_LEN;
		Len = ImgHdr.image_size + IMAGE_HEADER_LEN;

		if (!((ImgHdr.signature[0] == SPIC_CALIB_PATTERN[0]) && (ImgHdr.signature[1] == SPIC_CALIB_PATTERN[1]) &&
			  ((IS_FLASH_ADDR(DstAddr) && IS_FLASH_ADDR(DstAddr + Len)) || (IS_BOOT_ADDR(DstAddr) && IS_BOOT_ADDR(DstAddr + Len))))) {
			return IMG_INVALID;
		}

		/* If not XIP sub-image, then copy it to specific address (include the IMAGE_HEADER)*/
		if (IS_BOOT_ADDR(DstAddr)) {
			BOOT_ROM_Copy((void *)DstAddr, (void *)StartAddr, Len);
			DCache_CleanInvalidate(DstAddr, Len);
		}

		if (SubImgInfo != NULL) {
			SubImgInfo[i].Addr = DstAddr;
			SubImgInfo[i].Len = Len;
		}

		StartAddr += Len;
	}

	return 0;
}

HAL_ROM_TEXT_SECTION
int BOOT_ROM_SignatureCheck(Manifest_TypeDef *Manifest, SubImgInfo_TypeDef *SubImgInfo, u8 SubImgNum)
{
	int ret = 0;
	u8 PubKeyHash[32];
	u8 i, j;
	u8 RMASBootEn = 0;
	OTPC_TypeDef *OTPC  = OTPC_DEV;
	u32 HashLen, PCRProtBits;

	/* 1. check if secure boot enable. */
	if (SYSCFG_OTP_RMAMode()) {
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			PubKeyHash[j] = HAL_READ8(OTPC_REG_BASE, SEC_PKKEY_RMA_PK_0 + j);
			if (PubKeyHash[j] != 0xFF) {
				RMASBootEn = 1;
			}
		}

		/* If in RMA mode but RMA Pubkey is not programed, skip SBOOT */
		if (!RMASBootEn) {
			return 0;
		}
	} else {
		/* If in Normal mode and secure boot is not enable, skip SBOOT */
		if (!SYSCFG_OTP_SBootEn()) {
			return 0;
		}

		/* 2. read public key hash from OTP. Start with a random index to avoid side channel attack. */
		j = _rand() % 32;
		for (i = 0; i < 32; i++, j = (j + 1) % 32) {
			PubKeyHash[j] = HAL_READ8(OTPC_REG_BASE, SEC_PKKEY_PK1_0 + j);
		}
	}

	DBG_8195A("IMG1 SBOOT EN\n");

	/* 3. verify signature */
	ret = SBOOT_SignatureVerify((u8 *)PubKeyHash, Manifest, SubImgInfo, SubImgNum);

	/* 4. copy measurements to PCRs and lock it */
	if (ret == 0) {
		if ((Manifest->HashAlg == HASHID_SHA256) || (Manifest->HashAlg == HASHID_HMAC256)) {
			HashLen = 32;
			PCRProtBits = OTPC_BIT_PCR_PROT_K3;
		} else {
			HashLen = HASH_MAX_LEN;
			PCRProtBits = OTPC_BIT_PCR_PROT_K3 | OTPC_BIT_PCR_PROT_K2;
		}

		_memcpy((void *)PCR_BASE, (void *)Manifest->ImgHash, HashLen);
		DCache_Clean(PCR_BASE, HashLen);
		__DSB();

		OTPC->OTPC_OTP_PARAM |= PCRProtBits;
	}

	return ret;
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_LoaderEntry(void)
{
	PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE)__ram_start_table_start__;

	/* reset MSP to origin. The data in old stack is discarded.
		Be carefully when add functions after this!!! They should not utilize data in old stack any more */
	__set_MSP(MSP_RAM_HP);

	if (pRamStartFun->FlashStartFun) {
		/* Goto Flash code */
		pRamStartFun->FlashStartFun();
	} else {
		/* Goto Ram code */
		pRamStartFun->RamStartFun();
	}
}

HAL_ROM_TEXT_SECTION
int BOOT_ROM_KeyDeriveFunc(const unsigned char *secret, size_t slen,
						   const char *label,
						   const unsigned char *context, size_t clen,
						   unsigned char *dstbuf, size_t dlen)
{
	size_t nb;
	size_t i, j, k, md_len;
	unsigned char tmp[128];
	unsigned char h_i[32];
	hw_sha_context Context;

	/* Enable func & clock */
	RCC_PeriphClockCmd(APBPeriph_IPSEC, APBPeriph_IPSEC_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LX1, APBPeriph_LX1_CLOCK, ENABLE);

	/* Initialize hash engine */
	CRYPTO_SHA_Init(NULL);

	_memset(&Context, 0, sizeof(hw_sha_context));
	md_len = 32;

	if (sizeof(tmp) < md_len + strlen(label) + clen) {
		return -1;
	}

	nb = strlen(label);
	_memcpy(tmp + md_len, label, nb);
	_memcpy(tmp + md_len + nb, context, clen);
	nb += clen;

	/*
	 * Compute P_<hash>(secret, label + random)[0..dlen]
	 */
	rtl_crypto_hmac_sha2_init(SHA2_256, secret, slen, &Context);
	rtl_crypto_hmac_sha2_update(tmp + md_len, nb, &Context);
	rtl_crypto_hmac_sha2_final(tmp, &Context);

	for (i = 0; i < dlen; i += md_len) {
		_memset(&Context, 0, sizeof(hw_sha_context));
		rtl_crypto_hmac_sha2_init(SHA2_256, secret, slen, &Context);
		rtl_crypto_hmac_sha2_update(tmp, md_len + nb, &Context);
		rtl_crypto_hmac_sha2_final(h_i, &Context);

		_memset(&Context, 0, sizeof(hw_sha_context));
		rtl_crypto_hmac_sha2_init(SHA2_256, secret, slen, &Context);
		rtl_crypto_hmac_sha2_update(tmp, md_len, &Context);
		rtl_crypto_hmac_sha2_final(tmp, &Context);

		k = (i + md_len > dlen) ? dlen % md_len : md_len;

		for (j = 0; j < k; j++) {
			dstbuf[i + j]  = h_i[j];
		}
	}

	_memset(tmp, 0, sizeof(tmp));
	_memset(h_i, 0, sizeof(h_i));
	DCache_CleanInvalidate((u32)tmp, sizeof(tmp));
	DCache_CleanInvalidate((u32)h_i, sizeof(h_i));

	return (0);
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_HUKDerive(u32 *SecEpoch, u8 *DerivedKey, u32 Len)
{
	u8 HUK[16];
	u32 value;
	int i;

	/* check if HUK derive enable */
	if (!SYSCFG_OTP_HUKDerivEn()) {
		return;
	}

	/* if HUK read-protection sticky bit  is already set, return directly */
	value = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_LPLAT_CTRL);
	if (value & LSYS_BIT_HUK_PROT) {
		return;
	}

	for (i = 0; i < 16; i++) {
		HUK[15 - i] = HAL_READ8(OTPC_REG_BASE, SEC_NEWKEY_HUK_0 + i);
	}

	BOOT_ROM_KeyDeriveFunc(HUK, sizeof(HUK), "secret derivation", (u8 *)SecEpoch, 4, DerivedKey, Len);

	/* setup sticky bit to prevent HUK read out by later boot stages */
	value |= LSYS_BIT_HUK_PROT;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_LPLAT_CTRL, value);

	_memset(HUK, 0, sizeof(HUK));
	DCache_CleanInvalidate((u32)HUK, sizeof(HUK));
}

HAL_ROM_TEXT_SECTION
int BOOT_ROM_SecurityConfig(Manifest_TypeDef *Manifest, u32 ImgAddr)
{
	SubImgInfo_TypeDef SubImgInfo[2];
	int ret;
	u8 i;
	u32 LogAddr;

	if (SYSCFG_BootFromNor()) {
		LogAddr = (u32)__km4_boot_text_start__ - IMAGE_HEADER_LEN;

		/* clean here, for use len this time */
		if (Manifest->ImgSize > 0x10000000 - LogAddr) {
			ret = IMG_SIZE_INVALID;
			goto exit;
		}

		DCache_Invalidate(0x0A000000, Manifest->ImgSize);
		RSIP_MMU_Cache_Clean();

		RSIP_MMU_Config(MMU_BOOTLOADER_IDX, LogAddr, LogAddr + 0x01000000 - 0x20, ImgAddr + MANIFEST_SIZE_4K_ALIGN);
		RSIP_MMU_Cmd(MMU_BOOTLOADER_IDX, ENABLE);

		/* check if flash encrypt enable */
		BOOT_ROM_OTFCheck(Manifest, LogAddr);

		/* load images to SRAM */
		ret = BOOT_ROM_LoadImage(SubImgInfo, LogAddr, 1);

	} else {
		/* load images to SRAM */
		ret = BOOT_ROM_LoadImage(SubImgInfo, ImgAddr + MANIFEST_SIZE_4K_ALIGN, 1);

	}

	if (ret < 0) {
		goto exit;
	}

	/* check secure boot if need */
	ret = BOOT_ROM_SignatureCheck(Manifest, SubImgInfo, 1);

	if (ret < 0) {
		/* clear copied image */
		for (i = 0; i < 1; i++) {
			if (!IS_FLASH_ADDR(SubImgInfo[i].Addr)) {
				_memset((void *)SubImgInfo[i].Addr, 0, SubImgInfo[i].Len);
				DCache_CleanInvalidate(SubImgInfo[i].Addr, SubImgInfo[i].Len);
			}
		}
	}

exit:
	return ret;
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_BootldrSelect(u32 *SecEpoch)
{
	u32 ImgAddr[2] = {SPI_FLASH_BASE, 0x0};
	Manifest_TypeDef Manifest[2];
	u8 ImgIndex, ValidImgCnt = 0;
	int V1, V2, VerNum = 0;
	u16 tmp;
	int ret;
	u8 i;
	u32 BootCnt = 0;
	u32 BootInfo = 0;

	u16 MajorVer[2] = {0}; //16-bit major
	u16 MinorVer[2] = {0}; //16-bit minor

	/* get boot cnt, for version choose and IWDG refresh later */
	BootInfo = BKUP_Read(BKUP_REG0);
	BootCnt = BootInfo & BOOT_CNT_MASK;

	/* load manifest(OTA1) from flash to SRAM */
	BOOT_ROM_Copy((void *)&Manifest[0], (void *)SPI_FLASH_BASE, sizeof(Manifest_TypeDef));
	if (_memcmp(Manifest[0].Pattern, SPIC_CALIB_PATTERN, sizeof(SPIC_CALIB_PATTERN)) == 0) {

		MajorVer[0] = (int16_t)Manifest[0].MajorImgVer;
		MinorVer[0] = (int16_t)Manifest[0].MinorImgVer;
		V1 = (MajorVer[0] << 16) | MinorVer[0];
	} else {
		V1 = -1;
	}

	tmp = HAL_READ16(OTPC_REG_BASE, SEC_OTA_ADDR);

	/* Take default OTP value as invalid address */
	if (tmp != 0xFFFF) {
		ImgAddr[1] = tmp << 12;
	} else {
		ImgAddr[1] = 0;
	}

	/* check if OTA2 addr is valid. The OTA2 addr in OTP should be a multiples of 4KB. */
	if (IS_FLASH_ADDR(ImgAddr[1])) {
		/* load manifest(OTA2) from flash to SRAM */
		BOOT_ROM_Copy((void *)&Manifest[1], (void *)ImgAddr[1], sizeof(Manifest_TypeDef));
		if (_memcmp(Manifest[1].Pattern, SPIC_CALIB_PATTERN, sizeof(SPIC_CALIB_PATTERN)) == 0) {
			MajorVer[1] = (int16_t)Manifest[1].MajorImgVer;
			MinorVer[1] = (int16_t)Manifest[1].MinorImgVer;
			V2 = (MajorVer[1] << 16) | MinorVer[1];
		} else {
			V2 = -1;
		}
	} else {
		V2 = -1;
	}

	/* check if Anti-rollback enable */
	if (SYSCFG_OTP_AntiRollback()) {
		/* Get Version Number from OTP*/
		tmp = HAL_READ16(OTPC_REG_BASE, SEC_BOOT_IMGVER0);
		for (i = 0; i < 16; i++) {
			if ((tmp & BIT(i)) == 0) {
				VerNum++;
			}
		}
	} else {
		VerNum = 0;
	}
	VerNum = VerNum << 16;

	/* Check ImgVer */
	if ((VerNum > V1) && (VerNum > V2)) {			/* VN > V1&V2*/
		goto Fail;
	} else if ((VerNum <= V1) && (VerNum <= V2)) {	/* V1&V2 >= VN */
		ValidImgCnt = 2;
	} else {										/* Vn >= VN > Vm*/
		ValidImgCnt = 1;
	}
	ImgIndex = (V1 >= V2) ? BOOT_FROM_OTA1 : BOOT_FROM_OTA2;

	if (BootCnt >= BOOT_CNT_TOTAL_LIMIT) {
		//rom patch
		BOOT_ROM_PatchEntry(__LINE__);
		goto Fail;

	} else {
		BootCnt = BootCnt + 1;
	}

	BootInfo = (BootInfo & (~BOOT_CNT_MASK)) | BootCnt;

	if ((BootCnt >= BOOT_CNT_TRY_LIMIT) && (ValidImgCnt == 2)) {
		BootInfo |= BOOT_CNT_ERR;
		/* boot from another when bootcnt error*/
		/* it happens when newer bootloader cannot work normally */
		ImgIndex = (ImgIndex + 1) % 2;
	}

	/* load image, check OTF & ECC */
	for (i = 0; i < ValidImgCnt; i++) {
		ret = BOOT_ROM_SecurityConfig(&Manifest[ImgIndex], ImgAddr[ImgIndex]);
		DBG_8195A("IMG1(OTA%d) %s, ret: %d\n", ImgIndex + 1, ret < 0 ? "INVALID" : "VALID", ret);

		if (ret >= 0) {
			*SecEpoch = Manifest[ImgIndex].SecEpoch;
			break;
		}
		ImgIndex = (ImgIndex + 1) % 2;
	}

	if (ImgIndex) {
		BootInfo |= BOOT_VER_NUM;
	} else {
		BootInfo &= ~BOOT_VER_NUM;
	}

	BKUP_Write(BKUP_REG0, BootInfo);

	if (ret < 0) {
		goto Fail;
	}

	return;

Fail:
	DBG_8195A("IMG1 invalid, BOOT FAIL!!\n");

	/* clear stack */
	__set_MSP(MSP_RAM_HP);
	_memset((void *)MSPLIM_RAM_HP, 0, MSP_RAM_HP - 128 - MSPLIM_RAM_HP);
	DCache_CleanInvalidate(MSPLIM_RAM_HP, MSP_RAM_HP - 128 - MSPLIM_RAM_HP);

	while (1) {
		DelayMs(1000);
		if (BootCnt >= BOOT_CNT_TOTAL_LIMIT) {
			WDG_Refresh(IWDG_DEV);
		}
	}

	return;
}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_FromFlash(void)
{
	PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE)__ram_start_table_start__;
	u8 *Img1ValidCode = (u8 *)(__ram_start_table_start__ + sizeof(RAM_FUNCTION_START_TABLE));
	u32 *ptr = (u32 *)SPI_FLASH_BASE;
	u32 i = 0;
	u32 SecEpoch = 0;
	u32 CurrentMsp;
	u32 OTA_Addr[2];
	u32 index = 0;
	u32 tmp = HAL_READ16(OTPC_REG_BASE, SEC_OTA_ADDR);

	OTA_Addr[0] = SPI_FLASH_BASE;

	/* Take default OTP value as invalid address */
	if ((tmp != 0xFFFF) && (IS_FLASH_ADDR(tmp << 12))) {
		OTA_Addr[1] = tmp << 12;
	} else {
		OTA_Addr[1] = SPI_FLASH_BASE;
	}

	/* wait until flash power stable */
	while ((*ptr != SPIC_CALIB_PATTERN[0]) || (*(ptr + 1) != SPIC_CALIB_PATTERN[1])) {
		DCache_Invalidate(OTA_Addr[index], 8);
		DelayMs(10);

		index = (index + 1) % 2;
		ptr = (u32 *)OTA_Addr[index];

		if (i++ >= 100) {
			DBG_8195A("Flash image invalid \n");
			shell_rom(100000);
		}
	}

	RCC_PeriphClockCmd(APBPeriph_SCE, APBPeriph_CLOCK_NULL, ENABLE);
	BOOT_ROM_PatchEntry(__LINE__);

	/* bootloader OTA select */
	BOOT_ROM_BootldrSelect(&SecEpoch);
	BOOT_ROM_HUKDerive(&SecEpoch, DerivedKey, sizeof(DerivedKey));

	i = 0;
	while (ROM_IMG1_VALID_PATTEN[i] != 0xff) {
		if (Img1ValidCode[i] != ROM_IMG1_VALID_PATTEN[i]) {
			// image1 validation patten miss match, so don't jump to image1
			DBG_8195A("Image1 Validation Incorrect !!!\n");
			while (1) {
				// let the program stuck here
				DBG_8195A("Please Re-boot and try again, or re-burn the flash image\n");
				DelayMs(10000);
			}
		}
		i++;
	}

	DBG_8195A("IMG1 ENTRY[%x:%x]\n", pRamStartFun->FlashStartFun, pRamStartFun->RamStartFun);

	/* clear stack, reserve 128 byte of stack space for _memset() and DCache_CleanInvalidate().
		can make sure the secure boot secrets are cleared, becuase manifest occupies 400bytes of stack space at the beginning.*/
	CurrentMsp = __get_MSP();
	_memset((void *)MSPLIM_RAM_HP, 0, CurrentMsp - 128 - MSPLIM_RAM_HP);
	DCache_CleanInvalidate(MSPLIM_RAM_HP, CurrentMsp - 128 - MSPLIM_RAM_HP);

	/* branch to bootloader. This function is independent, to avoid it utilizing data in old stack */
	BOOT_ROM_LoaderEntry();

}

HAL_ROM_TEXT_SECTION
void BOOT_ROM_InitDebugFlg(void)
{
	u32 Temp = 0;
	u32 ChipType = SYSCFG_CHIPType_Get();

	/* to initial ROM code using global variable */
	if (SYSCFG_OTP_DisBootLog() == FALSE) {
		if (ChipType != CHIP_TYPE_RTLSIM) {
			LOG_MASK(LEVEL_ERROR, 0xFFFFFFFF);
		} else {
			Temp = HAL_READ32(OTPC_REG_BASE, SEC_OTP_SYSCFG0);
			Temp |= SEC_BIT_DIS_BOOT_LOG_EN;
			HAL_WRITE32(OTPC_REG_BASE, SEC_OTP_SYSCFG0, Temp);
		}
	}
}

/**
  * @brief  Enable/Disable OSC131K
  * @param  status: ENABLE/DISABLE
  * @note After enable OSC131K, 7ms delay will be cost to output 131K, so we enable 131K in rom boot very early.
  * @note you should poll xx to check if OSC131 is enabled.
  */
__weak HAL_ROM_TEXT_SECTION
void BOOT_ROM_OSC131_Enable(void)
{
	u32 temp = 0;
	REGU_TypeDef *regu = REGU_BASE;

	temp = regu->REGU_POWER_CTRL;

	if ((temp & REGU_BIT_POW_32KOSC) == 0) {
		temp |= REGU_BIT_POW_32KOSC;
		regu->REGU_POWER_CTRL = temp;
	}
}

__weak HAL_ROM_TEXT_SECTION
void BOOT_ROM_ResetVsr(void)
{
	u32 BssLen = 0;

	PRAM_FUNCTION_START_TABLE pRamStartFun = (PRAM_FUNCTION_START_TABLE) __ram_start_table_start__;
	PDSLP_RETENTION_FUNC_TABLE pDSLPPatchFun = (PDSLP_RETENTION_FUNC_TABLE) __retention_entry_func__;
	ROM_SECURE_CALL_NS_ENTRY *prom_sec_call_ns_entry = (ROM_SECURE_CALL_NS_ENTRY *)__rom_entry_ns_start__;

	/* Set ACTLR.LoopBuf_Dis(0xE000E008, bit 6) to avoid Loop Buffer Bug in KM0/KM4 version 1.8, This register is not banked between Security domains.*/
	SCnSCB->ACTLR |= SCnSCB_ACTLR_LOOPBUF_DIS_Msk;
	__DSB();

	// if (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_CFG) & AON_BIT_DSLP_RETENTION_RAM_PATCH) {
	// 	pDSLPPatchFun->DSLPPatchFun0();
	// }

	/* Rom Bss Common Initial */
	BssLen = (__rom_bss_end__ - __rom_bss_start__);

	/* Check boot or wake up from power saving */
	if (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG) & LSYS_BIT_BOOT_WAKE_FROM_PS_HS) {
		pRamStartFun->RamPatchFun0();
	}

	_memset((void *) __rom_bss_start__, 0, BssLen);

	/* Rom Bss S Initial*/
	BssLen = (__rom_bss_end_s__ - __rom_bss_start_s__);
	_memset((void *) __rom_bss_start_s__, 0, BssLen);

	/* Rom Bss NS Initial. We can call clear_ns_rom_bss directly here because all memory address is Secure when boot */
	prom_sec_call_ns_entry->clear_ns_rom_bss();

	/* Need Modify */
	irq_table_init(MSP_RAM_HP);

	BOOT_ROM_PatchEntry(__LINE__);

	/* Release TRNG reset and clock */
	RCC_PeriphClockCmd(APBPeriph_TRNG, APBPeriph_TRNG_CLOCK, ENABLE);

	BOOT_ROM_PatchEntry(__LINE__);

	/* Initial TRNG*/
	TRNG_Init();

	TRNG_Random_Delay();
	BOOT_ROM_PatchEntry(__LINE__);
	/* enable OSC131K very early, because it takes 17ms to output 131K */
	BOOT_ROM_OSC131_Enable();

	BOOT_ROM_PatchEntry(__LINE__);

	/* open SPIC clock first, for SPIC BOOT_FINISH Issue */
	BOOT_ROM_SPUFlash();

	/* ROM Information */
	SYSCFG_ROMINFO_Set();

	/* Initial Global Variable */
	BOOT_ROM_InitDebugFlg();

	TRNG_Random_Delay();
	BOOT_ROM_PatchEntry(__LINE__);

	/* Initial Log Uart */
	LOGUART_DiagInit(TRUE);

	/* show build info */
	DBG_8195A("ROM:[V%d.%d]\n", ROMINFORMATION, ROMVERSION_SUB);

	TRNG_Random_Delay();
	BOOT_ROM_PatchEntry(__LINE__);
	/* Initial Flash */
	BOOT_ROM_InitFlash();

	/* enable cache before XIP */
	SCB_EnableDCache();
	SCB_EnableICache();

	TRNG_Random_Delay();
	BOOT_ROM_PatchEntry(__LINE__);

	/* check trap pin UART_DOWNLOAD_IMAGE _PA_7 == low or BKUP0 set BKUP_BIT_UARTBURN_BOOT*/
	if (SYSCFG_HWTrigDownload() || SYSCFG_SWTrigDownload()) {
		/*freeze IWDG here to avoid reboot during waiting the PC command*/
		IWDG_LP_Enable(IWDG_DEV, DISABLE);

		BKUP_Clear(0, BKUP_BIT_UARTBURN_BOOT);
		xmodem_img_download(2);

		/*unfreeze IWDG again*/
		IWDG_LP_Enable(IWDG_DEV, ENABLE);
	}

	TRNG_Random_Delay();
	BOOT_ROM_PatchEntry(__LINE__);

	/*Check OTP CRC*/
	BOOT_ROM_OTPCheck();

	/* Patch function 0 */
	/* use1: FPGA verification boot from RAM & Image Tool flash loader will enter here */
	if (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG) & LSYS_BIT_BOOT_PATCH_FUNC0) {
		pRamStartFun->RamPatchFun0();
	}

	TRNG_Random_Delay();
	BOOT_ROM_PatchEntry(__LINE__);
	/* boot from flash */
	BOOT_ROM_FromFlash();

	for (;;);
}

