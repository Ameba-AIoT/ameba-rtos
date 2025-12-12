/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_nand_ftl.h"
#include "ameba_nand_mfg_ftl.h"
#include "ameba_ftl.h"
/* Private defines -----------------------------------------------------------*/

#define NF_STS_ECC_MASK								0x30U
#define NF_STS_ECC_NO_BITFLIPS						(0 << 4)
#define NF_STS_ECC_HAS_BITFLIPS						(1 << 4)
#define NF_STS_ECC_ERROR							(2 << 4)
#define NF_STS_DOSILICON_ECCS2						(1 << 6)
#define NF_STS_MICRON_ECCS2							(1 << 6)

#define NF_ECCSR_MACRONIX_CMD						0x7CU
#define NF_ECCSR_MACRONIX_ADDR						0x00U  /* Dummy */
#define NF_ECCSR_MACRONIX_MASK						0x0FU

#define NF_CFG_WINBOND_BUF_READ						(1 << 3)

#define NF_WINBOND_DIE_SEL_CMD						0xC2U

#define NF_PARAMETER_PAGE_ADDR						0x01U
#define NF_PARAMETER_PAGE_GIGADEVICE_ADDR			0x04U
#define NF_PARAMETER_PAGE_SIZE						256U
#define NF_PARAMETER_PAGE_CNT						3U
#define NF_PARAMETER_PAGE_TOTAL_SIZE				(NF_PARAMETER_PAGE_SIZE * NF_PARAMETER_PAGE_CNT)
#define NF_PARAMETER_PAGE_OFFSET_MFG				32U
#define NF_PARAMETER_PAGE_OFFSET_MODEL				44U
#define NF_PARAMETER_PAGE_OFFSET_BYTES_PER_PAGE		80U
#define NF_PARAMETER_PAGE_OFFSET_OOB_BYTES_PER_PAGE	84U
#define NF_PARAMETER_PAGE_OFFSET_PAGES_PER_BLOCK	92U
#define NF_PARAMETER_PAGE_OFFSET_BLOCKS_PER_LUN		96U
#define NF_PARAMETER_PAGE_OFFSET_LUNS				100U
#define NF_PARAMETER_PAGE_OFFSET_MAX_BB_PER_LUN		103U
#define NF_PARAMETER_PAGE_OFFSET_REQ_HOST_ECC_LEVEL	112U
#define NF_PARAMETER_PAGE_OFFSET_CRC				254U

#define NF_PARAMETER_PAGE_ONFI_CRC_POLYNOM			0x8005U
#define NF_PARAMETER_PAGE_ONFI_CRC_BASE				0x4F4EU

/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static u8 nand_ftl_get_mfg_status(u8 cmd, u8 addr);
static u16 nand_ftl_calculate_onfi_crc16(const u8 *buf);
static s32 nand_ftl_get_valid_parameter_offset(u8 *buf);
static u8 nand_ftl_winbond_init(struct nand_ftl_device *nand);
static u8 nand_ftl_winbond_select_target(struct nand_ftl_device *nand, u8 target);
static u8 nand_ftl_common_get_ecc_status(struct nand_ftl_device *nand, u8 status);
static u8 nand_ftl_dosilicon_get_ecc_status(struct nand_ftl_device *nand, u8 status);
static u8 nand_ftl_gigadevice_get_ecc_status(struct nand_ftl_device *nand, u8 status);
static u8 nand_ftl_macronix_get_ecc_status(struct nand_ftl_device *nand, u8 status);
static u8 nand_ftl_micron_get_ecc_status(struct nand_ftl_device *nand, u8 status);
static u8 nand_ftl_winbond_get_ecc_status(struct nand_ftl_device *nand, u8 status);
static u8 nand_ftl_read_parameter_page(struct nand_ftl_device *nand, u8 *data);

/* Private variables ---------------------------------------------------------*/

static struct nand_ftl_mfg_ops NandDefaultOps = {
	.get_ecc_status = nand_ftl_common_get_ecc_status,
};

static struct nand_ftl_mfg_ops DosiliconOps = {
	.get_ecc_status = nand_ftl_dosilicon_get_ecc_status,
};

static struct nand_ftl_mfg_ops GigaDeviceOps = {
	.get_ecc_status = nand_ftl_gigadevice_get_ecc_status,
};

static struct nand_ftl_mfg_ops MacronixOps = {
	.get_ecc_status = nand_ftl_macronix_get_ecc_status,
};

static struct nand_ftl_mfg_ops MicronOps = {
	.get_ecc_status = nand_ftl_micron_get_ecc_status,
};

static struct nand_ftl_mfg_ops WinbondOps = {
	.init = nand_ftl_winbond_init,
	.select_target = nand_ftl_winbond_select_target,
	.get_ecc_status = nand_ftl_winbond_get_ecc_status,
};

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Read manufacture specific NAND Flash status register
  * @param  cmd : command
  * @param  addr : address
  * @retval Value of NAND Flash status register
  */
static u8 nand_ftl_get_mfg_status(u8 cmd, u8 addr)
{
	u8 data;

	NAND_RxCmd(cmd, 1, &addr, 1, &data);

	return data;
}

/**
  * @brief  Calculate ONFI CRC16 for parameter page
  * @param  buf : buffer for parameter page data
  * @retval ONFI CRC16 calculated result
  */
static u16 nand_ftl_calculate_onfi_crc16(const u8 *buf)
{
	u16 i;
	u16 crc = NF_PARAMETER_PAGE_ONFI_CRC_BASE;
	u16 len = NF_PARAMETER_PAGE_SIZE - 2U;
	while (len--) {
		crc ^= (*buf++) << 8;
		for (i = 0; i < 8; i++) {
			crc = (crc << 1) ^ ((crc & 0x8000) ? NF_PARAMETER_PAGE_ONFI_CRC_POLYNOM : 0U);
		}
	}

	return crc;
}

/**
  * @brief  Get the offset of valid parameter
  * @param  buf : buffer for parameter page data with multiple copies
  * @retval -1 : not found; >= 0 : offset of valid parameter
  */
static s32 nand_ftl_get_valid_parameter_offset(u8 *buf)
{
	s32 result = -1;
	u32 offset;
	u16 crc;
	u16 calCrc;

	for (offset = 0; offset < NF_PARAMETER_PAGE_TOTAL_SIZE; offset += NF_PARAMETER_PAGE_SIZE) {
		/* Check signature "ONFI" */
		if ((buf[offset] == 0x4FU) && (buf[offset + 1] == 0x4EU) && (buf[offset + 2] == 0x46U) && (buf[offset + 3] == 0x49U)) {
			/* Check CRC */
			crc = buf[offset + NF_PARAMETER_PAGE_OFFSET_CRC] + (buf[offset + NF_PARAMETER_PAGE_OFFSET_CRC + 1] << 8);
			calCrc = nand_ftl_calculate_onfi_crc16((const u8 *)(buf + offset));
			if (crc == calCrc) {
				result = (s32)offset;
				break;
			}
		}
	}

	return result;
}

/**
  * @brief  Init Winbond specific NAND FTL functionalities
  * @param  nand : NAND device info
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_ftl_winbond_init(struct nand_ftl_device *nand)
{
	u8 ret = ERR_NONE;
	u8 i;
	struct ameba_flash_info *info = nand->flash_info;

	/* Make sure all dies are in buffer read mode and not continuous read mode */
	if (info->targets > 1) {
		/* Default select target 0 */
		for (i = info->targets; i > 0; i--) {
			ret = nand_ftl_winbond_select_target(nand, i - 1);
			if (ret == ERR_NONE) {
				NAND_SetStatusBits(NAND_REG_CFG, NF_CFG_WINBOND_BUF_READ, ENABLE);
			}
		}
	} else {
		NAND_SetStatusBits(NAND_REG_CFG, NF_CFG_WINBOND_BUF_READ, ENABLE);
	}

	return ret;
}

/**
  * @brief  Select target for Winbond
  * @param  nand : NAND device info
  * @param  target : target index
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_ftl_winbond_select_target(struct nand_ftl_device *nand, u8 target)
{
	struct ameba_flash_info *info = nand->flash_info;
	if (info->targets > 1) {
		NAND_TxCmd(NF_WINBOND_DIE_SEL_CMD, 0, NULL, 1, &target);
	}
	return ERR_NONE;
}

/**
  * @brief  Get ECC status for common NAND devices
  * @param  nand : NAND device info
  * @param  status : raw ECC status
  * @retval Parsed ECC status:
  *			ERR_NONE : OK
  *			ERR_NAND_BITFLIP_ERROR : bitflip(s) corrected
  *			ERR_NAND_BITFLIP_FATAL : bitflips not corrected
  *			ERR_FAIL: other error
  */
static u8 nand_ftl_common_get_ecc_status(struct nand_ftl_device *nand, u8 status)
{
	u8 ret = ERR_FAIL;

	UNUSED(nand);

	switch (status & NF_STS_ECC_MASK) {
	case NF_STS_ECC_NO_BITFLIPS:
		ret = ERR_NONE;
		break;
	case NF_STS_ECC_HAS_BITFLIPS:
		/* No way to know exactly how many bitflips have been fixed, warn as the maximum possible value */
		ret = ERR_NAND_BITFLIP_ERROR;
		break;
	case NF_STS_ECC_ERROR:
		ret = ERR_NAND_BITFLIP_FATAL;
		break;
	default:
		break;
	}

	return ret;
}

/**
  * @brief  Get ECC status for Dosilicon NAND devices
  * @param  nand : NAND device info
  * @param  status : raw ECC status
  * @retval Parsed ECC status:
  *			ERR_NONE : OK
  *			ERR_NAND_BITFLIP_ERROR : bitflip(s) corrected
  *			ERR_NAND_BITFLIP_FATAL : bitflips not corrected
  *			ERR_FAIL: other error
  */
static u8 nand_ftl_dosilicon_get_ecc_status(struct nand_ftl_device *nand, u8 status)
{
	u8 ret = ERR_FAIL;

	UNUSED(nand);

	switch (status & NF_STS_ECC_MASK) {
	case NF_STS_ECC_NO_BITFLIPS:
		ret = ERR_NONE;
		break;
	case NF_STS_ECC_HAS_BITFLIPS:
		if (status & NF_STS_DOSILICON_ECCS2) {
			ret = ERR_NAND_BITFLIP_ERROR; /* 7~8 bitflips corrected, data refreshment must be taken to guarantee data retention */
		} else {
			ret = ERR_NAND_BITFLIP_WARN;  /* 1-3 bitflips corrected */
		}
		break;
	case NF_STS_ECC_ERROR:
		ret = ERR_NAND_BITFLIP_FATAL;
	default:
		break;
	}

	return ret;
}

/**
  * @brief  Get ECC status for GigaDevice NAND devices
  * @param  nand : NAND device info
  * @param  status : raw ECC status
  * @retval Parsed ECC status:
  *			ERR_NONE : OK
  *			ERR_NAND_BITFLIP_WARN : bitflip(s) corrected and bitflip(s) less than ECC bits
  *			ERR_NAND_BITFLIP_ERROR : bitflip(s) corrected and bitflip(s) equal to ECC bits
  *			ERR_NAND_BITFLIP_FATAL : bitflips not corrected
  *			ERR_FAIL: other error
  */
static u8 nand_ftl_gigadevice_get_ecc_status(struct nand_ftl_device *nand, u8 status)
{
	u8 ret = ERR_FAIL;

	UNUSED(nand);

	switch (status & NF_STS_ECC_MASK) {
	case NF_STS_ECC_NO_BITFLIPS:
		ret = ERR_NONE;
		break;
	case NF_STS_ECC_HAS_BITFLIPS:
		ret = ERR_NAND_BITFLIP_WARN; /* 4-7 bitflips corrected */
		break;
	case NF_STS_ECC_ERROR:
		ret = ERR_NAND_BITFLIP_FATAL;
		break;
	default:
		ret = ERR_NAND_BITFLIP_ERROR; /* 8 bitflips corrected */
		break;
	}

	return ret;
}

/**
  * @brief  Get ECC status for Macronix NAND devices
  * @param  nand : NAND device info
  * @param  status : raw ECC status
  * @retval Parsed ECC status:
  *			ERR_NONE : OK
  *			ERR_NAND_BITFLIP_WARN : bitflip(s) corrected and bitflip(s) less than ECC bits
  *			ERR_NAND_BITFLIP_ERROR : bitflip(s) corrected and bitflip(s) equal to ECC bits
  *			ERR_NAND_BITFLIP_FATAL : bitflips not corrected
  *			ERR_FAIL: other error
  */
static u8 nand_ftl_macronix_get_ecc_status(struct nand_ftl_device *nand, u8 status)
{
	u8 ret = ERR_FAIL;
	u8 eccsr;

	UNUSED(nand);

	switch (status & NF_STS_ECC_MASK) {
	case NF_STS_ECC_NO_BITFLIPS:
		ret = ERR_NONE;
		break;
	case NF_STS_ECC_HAS_BITFLIPS:
		eccsr = nand_ftl_get_mfg_status(NF_ECCSR_MACRONIX_CMD, NF_ECCSR_MACRONIX_ADDR);
		if ((eccsr & NF_ECCSR_MACRONIX_MASK) == 4U) {
			ret = ERR_NAND_BITFLIP_ERROR; /* 4 bitflips corrected, data refreshment must be taken to guarantee data retention*/
		} else {
			ret = ERR_NAND_BITFLIP_WARN; /* 1-3 bitflips corrected */
		}
		break;
	case NF_STS_ECC_ERROR:
		ret = ERR_NAND_BITFLIP_FATAL;
		break;
	default:
		break;
	}

	return ret;
}

/**
  * @brief  Get ECC status for Micron NAND devices
  * @param  nand : NAND device info
  * @param  status : raw ECC status
  * @retval Parsed ECC status:
  *			ERR_NONE : OK
  *			ERR_NAND_BITFLIP_WARN : bitflip(s) corrected and bitflip(s) less than ECC bits
  *			ERR_NAND_BITFLIP_ERROR : bitflip(s) corrected and bitflip(s) equal to ECC bits
  *			ERR_NAND_BITFLIP_FATAL : bitflips not corrected
  *			ERR_FAIL: other error
  */
static u8 nand_ftl_micron_get_ecc_status(struct nand_ftl_device *nand, u8 status)
{
	u8 ret = ERR_FAIL;

	UNUSED(nand);

	switch (status & NF_STS_ECC_MASK) {
	case NF_STS_ECC_NO_BITFLIPS:
		ret = ERR_NONE;
		break;
	case NF_STS_ECC_HAS_BITFLIPS:
		if (status & NF_STS_MICRON_ECCS2) {
			ret = ERR_NAND_BITFLIP_ERROR; // 7-8 bitflips corrected, data refreshment must be taken to guarantee data retention
		} else {
			ret = ERR_NAND_BITFLIP_WARN;  // 1-3 bitflips corrected
		}
		break;
	case NF_STS_ECC_ERROR:
		ret = ERR_NAND_BITFLIP_FATAL;
		break;
	default:
		ret = ERR_NAND_BITFLIP_WARN;  // 4-6 bitflips corrected
		break;
	}

	return ret;
}

/**
  * @brief  Get ECC status for Winbond NAND devices
  * @param  nand : NAND device info
  * @param  status : raw ECC status
  * @retval Parsed ECC status:
  *			ERR_NONE : OK
  *			ERR_NAND_BITFLIP_ERROR : bitflip(s) corrected
  *			ERR_NAND_BITFLIP_FATAL : bitflips not corrected
  *			ERR_FAIL: other error
  */
static u8 nand_ftl_winbond_get_ecc_status(struct nand_ftl_device *nand, u8 status)
{
	return nand_ftl_common_get_ecc_status(nand, status);
}

/**
  * @brief  Read parameter page for common NAND devices
  * @param  nand : NAND device info
  * @param  data : data buffer
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_ftl_read_parameter_page(struct nand_ftl_device *nand, u8 *data)
{
	u8 ret = ERR_FAIL;
	u8 reg;
	u8 bk_reg;
	u8 ck_reg;
	u8 retry;
	u8 mid;
	u8 did;
	u32 addr;
	struct ameba_flash_info *info = nand->flash_info;

	reg = NAND_GetStatus(NAND_REG_CFG);
	bk_reg = reg;

	reg |= NAND_CFG_OTP_ENABLE;
	reg &= ~NAND_CFG_ECC_ENABLE;

	for (retry = 0; retry < 3; retry++) {
		NAND_SetStatus(NAND_REG_CFG, reg);
		ck_reg = NAND_GetStatus(NAND_REG_CFG);
		if (((ck_reg & NAND_CFG_OTP_ENABLE) == NAND_CFG_OTP_ENABLE) && ((ck_reg & NAND_CFG_ECC_ENABLE) == 0)) {
			ret = ERR_NONE;
			break;
		}
	}

	if (ret != ERR_NONE) {
		NAND_SetStatus(NAND_REG_CFG, bk_reg);
		return ERR_TIMEOUT;
	}

	/* GD 38nm models use 0x04 while 24nm and newer models use 0x01 */
	mid = info->mid;
	did = info->did;
	if ((mid == NAND_MFG_GIGADEVICE) &&
		((did == 0x21) || (did == 0x31) || (did == 0x41) || (did == 0x51) ||
		 (did == 0x22) || (did == 0x32) || (did == 0x42) || (did == 0x52) ||
		 (did == 0x25) || (did == 0x35) || (did == 0x45) || (did == 0x55))) {
		addr = NF_PARAMETER_PAGE_GIGADEVICE_ADDR;
	} else {
		addr = NF_PARAMETER_PAGE_ADDR;
	}

	ret = NAND_Page_Read(addr, 0, NF_PARAMETER_PAGE_TOTAL_SIZE, data);
	if (ret == 0xFFU) {
		ret = ERR_TIMEOUT;
	} else {
		/* ECC status will be ignored */
		ret = ERR_NONE;
	}

	NAND_SetStatus(NAND_REG_CFG, bk_reg);

	return ret;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init manufacture specific NAND FTL functionalities
  * @param  nand : NAND device info
  * @param  ops : callback of manufacture specific FTL operations
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_mfg_init(struct nand_ftl_device *nand)
{
	u8 ret;
	u8 retry;
	s32 paramBufOffset;
	u8 *paramBuf;
	u8 buf[NF_PARAMETER_PAGE_TOTAL_SIZE];
	struct ameba_flash_info *info = nand->flash_info;
	struct nand_ftl_mfg_ops *ops;

	info->targets = 1;

	switch (info->mid) {
	case NAND_MFG_DOSILICON:
		nand->mfg_ops = &DosiliconOps;
		break;
	case NAND_MFG_GIGADEVICE:
		nand->mfg_ops = &GigaDeviceOps;
		break;
	case NAND_MFG_MACRONIX:
		nand->mfg_ops = &MacronixOps;
		break;
	case NAND_MFG_MICRON:
		nand->mfg_ops = &MicronOps;
		break;
	case NAND_MFG_WINBOND:
		nand->mfg_ops = &WinbondOps;
		if (info->did == 0xAB) { /* As per Linux SPI NAND Flash driver */
			info->targets = 2;
		}
		break;
	default:
		nand->mfg_ops = &NandDefaultOps;
		break;
	}

	ops = (struct nand_ftl_mfg_ops *)nand->mfg_ops;

	if (ops->init) {
		ret = ops->init(nand);
		if (ret != ERR_NONE) {
			return ret;
		}
	}

	for (retry = 0; retry < 3; retry++) {
		ret = nand_ftl_read_parameter_page(nand, buf);
		if (ret == ERR_NONE) {
			break;
		}
	}

	if (ret == ERR_NONE) {
		paramBufOffset = nand_ftl_get_valid_parameter_offset(buf);
		if (paramBufOffset >= 0) {
			paramBuf = buf + paramBufOffset;

			_memcpy((void *)info->mfg, (const void *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_MFG), NAND_ONFI_MFG_LEN);
			_memcpy((void *)info->model, (const void *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_MODEL), NAND_ONFI_MODEL_LEN);

			info->page_size = *(u32 *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_BYTES_PER_PAGE);
			info->oob_size = *(u16 *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_OOB_BYTES_PER_PAGE);
			info->pages_per_block = *(u32 *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_PAGES_PER_BLOCK);
			info->blocks_per_lun = *(u32 *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_BLOCKS_PER_LUN);
			info->luns_per_target = paramBuf[NF_PARAMETER_PAGE_OFFSET_LUNS];
			info->max_bad_blocks_per_lun = *(u16 *)(paramBuf + NF_PARAMETER_PAGE_OFFSET_MAX_BB_PER_LUN);
			info->request_host_ecc_level = paramBuf[NF_PARAMETER_PAGE_OFFSET_REQ_HOST_ECC_LEVEL];
			info->capacity = info->page_size * info->pages_per_block * info->blocks_per_lun * info->luns_per_target * info->targets;
		} else {
			ret = ERR_CHECKSUM;
		}
	}

	return ret;
}

