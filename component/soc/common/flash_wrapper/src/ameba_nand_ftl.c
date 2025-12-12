/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------ */

#include "ameba_nand_ftl.h"
#include "ameba_nand_mfg_ftl.h"
#include "ameba_ftl.h"

/* Private defines -----------------------------------------------------------*/

#define NAND_MARK_BAD_AT_ERASE_FAIL	1
#define NAND_MARK_BAD_AT_WRITE_FAIL	0

/* Private types -------------------------------------------------------------*/

enum nand_ftl_page_worn_status {
	NAND_PAGE_WORN_OK = 0,
	NAND_PAGE_WORN_WARN = 1,
	NAND_PAGE_WORN_ERROR = 2,
	NAND_PAGE_WORN_FATAL = 3,
};

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

static struct nand_ftl_device NF_Device;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Get page offset as per physical address
  * @param  addr : Physical address
  * @retval Page offset
  */
static inline u32 nand_get_offset(struct nand_ftl_device *nand, u32 addr)
{
	struct ameba_flash_info *info = nand->flash_info;
	return addr / info->page_size;
}

/**
  * @brief  Get block aligned page offset
  * @param  nand : NAND device info
  * @param  offset : page offset
  * @retval Block aligned page offset
  */
static inline u32 nand_get_block_address(struct nand_ftl_device *nand, u32 offset)
{
	struct ameba_flash_info *info = nand->flash_info;
	return (offset - (offset % info->pages_per_block));
}

/**
  * @brief  Select target
  * @param  nand : NAND device info
  * @param  offset : page offset
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_select_target(struct nand_ftl_device *nand, u32 offset)
{
	u8 ret = ERR_NONE;
	u32 target;
	struct ameba_flash_info *info = nand->flash_info;
	struct nand_ftl_mfg_ops *ops = (struct nand_ftl_mfg_ops *)nand->mfg_ops;

	if (ops->select_target != NULL) {

		target = offset / (info->pages_per_block * info->blocks_per_lun * info->luns_per_target);

		if (target >= info->targets) {
			return ERR_INVALID;
		}

		if (target == nand->cur_target) {
			return ERR_NONE;
		}

		if (info->targets == 1) {
			nand->cur_target = target;
			return ERR_NONE;
		}

		ret = ops->select_target(nand, target);
	}

	return ret;
}

/**
  * @brief  Check bad block
  * @param  nand : NAND device info
  * @param  offset : page offset
  * @param  value : 0 - Good; 1 - Bad
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_is_bad(struct nand_ftl_device *nand, u32 offset, u8 *value)
{
	struct ameba_flash_info *info = nand->flash_info;
	u32 block_addr;
	u8 ret;
	u8 data[2];

	block_addr = nand_get_block_address(nand, offset);
	ret = NAND_Page_Read(block_addr, info->page_size, 2, data);
	if (ret != 0xFFU) {
		ret = ERR_NONE;
		if ((data[0] != NF_GOOD_BLOCK) || (data[1] != NF_GOOD_BLOCK)) {
			*value = 1;
		} else {
			*value = 0;
		}
	} else {
		ret = ERR_TIMEOUT;
	}

	return ret;
}

/**
  * @brief  Mark bad block
  * @param  nand : NAND device info
  * @param  offset : page offset
  * @param  value : 0 - Good; 1 - Bad
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_mark_bad(struct nand_ftl_device *nand, u32 offset)
{
	struct ameba_flash_info *info = nand->flash_info;
	u32 block_addr;
	u8 ret;
	u8 data[2] = {0, 0};

	data[0] = NF_BAD_BLOCK;
	data[1] = NF_BAD_BLOCK;

	block_addr = nand_get_block_address(nand, offset);
	ret = NAND_Page_Write(block_addr, info->page_size, 2, data);
	if (ret == 0U) {
		ret = ERR_NONE;
	} else if (ret == 0xFFU) {
		ret = ERR_TIMEOUT;
	} else {
		ret = ERR_HARDWARE;
	}

	return ret;
}

/**
  * @brief  Erase block
  * @param  nand : NAND device info
  * @param  offset : page offset
  * @retval ERR_NONE : OK; others : FAIL
  */
static u8 nand_erase_block(struct nand_ftl_device *nand, u32 offset)
{
	u8 ret;

	ret = NAND_Erase(offset);
	if (ret == 0U) {
		ret = ERR_NONE;
	} else if (ret == 0xFFU) {
		ret = ERR_TIMEOUT;
	} else {
#if NAND_MARK_BAD_AT_ERASE_FAIL
		ret = nand_mark_bad(nand, offset);
		if (ret == ERR_NONE) {
			ret = ERR_NAND_WORN_BLOCK;
		}
#endif
	}

	return ret;
}

/**
  * @brief  Get NAND page worn status
  * @param  ecc_status : ECC status of the NAND page
  * @retval nand_ftl_page_worn_status
  */
static enum nand_ftl_page_worn_status nand_get_page_worn_status(u8 ecc_status)
{
	enum nand_ftl_page_worn_status page_status;

	switch (ecc_status) {
	case ERR_NONE:
		page_status = NAND_PAGE_WORN_OK;
		break;
	case ERR_NAND_BITFLIP_WARN:
		page_status = NAND_PAGE_WORN_WARN;
		break;
	case ERR_NAND_BITFLIP_ERROR:
		page_status = NAND_PAGE_WORN_ERROR;
		break;
	default: // ERR_NAND_BITFLIP_FATAL
		page_status = NAND_PAGE_WORN_FATAL;
		break;
	}

	return page_status;
}

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Init NAND FTL functionalities
  * @param  None
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_init(struct ameba_flash_info *info)
{
	u8 flash_ID[3];
	u8 reg;
	u8 ret;
	struct nand_ftl_device *nand = &NF_Device;

	nand->last_erased_block_address = 0xFFFFFFFF;

	flash_ID[0] = 0;
	flash_ID[1] = 0;
	flash_ID[2] = 0;

	/* Reset to clean status */
	NAND_TxCmd(0xFF, 0, NULL, 0, NULL);

	NAND_RxCmd(flash_init_para.FLASH_cmd_rd_id, 0, NULL, 2, flash_ID);
	if (flash_ID[0] == NAND_MFG_MICRON) {
		/* Micron's did are two bytes */
		NAND_RxCmd(flash_init_para.FLASH_cmd_rd_id, 0, NULL, 3, flash_ID);
	}

	info->mid = flash_ID[0];
	info->did = flash_ID[1];
	info->ext_did = flash_ID[2];
	nand->flash_info = info;

	reg = NAND_GetStatus(NAND_REG_CFG);
	/* Enable internal ECC */
	reg |= NAND_CFG_ECC_ENABLE;
	/* Enable/disable QUAD as per init parameters */
	if (flash_init_para.FLASH_QuadEn_bit != 0) {
		reg |= NAND_CFG_QUAD_ENABLE;
	} else {
		reg &= ~NAND_CFG_QUAD_ENABLE;
	}
	NAND_SetStatus(NAND_REG_CFG, reg);

	ret = nand_ftl_mfg_init(nand);
	if (ret == ERR_NONE) {
		nand->initialized = 1;
	}

	return ret;
}

/**
  * @brief  Read page
  * @param  addr : address, shall be page aligned
  * @param  buf : data buffer
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_read_page(u32 addr, u8 *buf)
{
	struct nand_ftl_device *nand = &NF_Device;
	struct ameba_flash_info *info = nand->flash_info;
	struct nand_ftl_mfg_ops *ops = (struct nand_ftl_mfg_ops *)nand->mfg_ops;
	u32 offset = nand_get_offset(nand, addr);
	u8 status;
	u8 ret;
	u8 is_bad_block;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, offset);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_is_bad(nand, offset, &is_bad_block);
	if (ret != ERR_NONE) {
		return ret;
	}

	if (is_bad_block) {
		ret = ERR_NAND_BAD_BLOCK;
	} else {
		status = NAND_Page_Read(offset, 0, info->page_size, buf);
		if (status == 0U) {
			ret = ERR_NONE;
		} else if (status == 0xFFU) {
			ret = ERR_TIMEOUT;
		} else {
			ret = ops->get_ecc_status(nand, status);
		}
	}

	return ret;
}

/**
  * @brief  Read page status
  * @param  addr : address, shall be page aligned
  * @param  buf : data buffer
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_read_page_status(u32 addr, u8 *buf)
{
	struct nand_ftl_device *nand = &NF_Device;
	struct ameba_flash_info *info = nand->flash_info;
	struct nand_ftl_mfg_ops *ops = (struct nand_ftl_mfg_ops *)nand->mfg_ops;
	u32 offset = nand_get_offset(nand, addr);
	u8 status;
	u8 ret;
	u8 is_bad_block;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, offset);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_is_bad(nand, offset, &is_bad_block);
	if (ret != ERR_NONE) {
		return ret;
	}

	if (is_bad_block) {
		return ERR_NAND_BAD_BLOCK;
	}

	status = NAND_Page_Read(offset, 0, info->page_size, buf);
	if (status == 0U) {
		ret = ERR_NONE;
	} else if (status == 0xFFU) {
		ret = ERR_TIMEOUT;
	} else {
		ret = ops->get_ecc_status(nand, status);
	}

	return ret;
}

/**
  * @brief  Check block status
  * @param  addr : address, shall be block aligned
  * @param  buf : data buffer
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_read_block_status(u32 addr, u8 *buf, u8 *block_status, u32 *page_status)
{
	struct nand_ftl_device *nand = &NF_Device;
	struct ameba_flash_info *info = nand->flash_info;
	struct nand_ftl_mfg_ops *ops = (struct nand_ftl_mfg_ops *)nand->mfg_ops;
	u32 offset = nand_get_offset(nand, addr);
	u32 block_addr = nand_get_block_address(nand, offset);
	u32 status_15_0 = 0;
	u32 status_31_16 = 0;
	u32 status_47_32 = 0;
	u32 status_63_48 = 0;
	u32 i;
	u8 ret;
	u8 is_bad_block;
	u8 has_page_warn = 0;
	u8 has_page_error = 0;
	u8 has_page_fatal = 0;
	u8 ecc_status;
	enum nand_ftl_page_worn_status worn_status;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, block_addr);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_is_bad(nand, block_addr, &is_bad_block);
	if (ret != ERR_NONE) {
		return ret;
	}

	if (is_bad_block) {
		*block_status = ERR_NAND_BAD_BLOCK;
		return ERR_NONE;
	}

	for (i = 0; i < info->pages_per_block; ++i) {
		ret = NAND_Page_Read(block_addr + i, 0, info->page_size, buf);
		if (ret == 0U) {
			ret = ERR_NONE;
		} else if (ret == 0xFFU) {
			ret = ERR_TIMEOUT;
		} else {
			ecc_status = ops->get_ecc_status(nand, ret);
			if (ecc_status != ERR_NONE) {
				worn_status = nand_get_page_worn_status(ecc_status);
				if (i < 16) {
					status_15_0 |= worn_status << (i * 2);
				} else if (i < 32) {
					status_31_16 |= worn_status << ((i - 16) * 2);
				} else if (i < 48) {
					status_47_32 |= worn_status << ((i - 32) * 2);
				} else {
					status_63_48 |= worn_status << ((i - 48) * 2);
				}
				if (worn_status == NAND_PAGE_WORN_WARN) {
					has_page_warn = 1;
				} else if (worn_status == NAND_PAGE_WORN_ERROR) {
					has_page_error = 1;
				} else if (worn_status == NAND_PAGE_WORN_FATAL) {
					has_page_fatal = 1;
				}
			}
			ret = ERR_NONE;
		}
	}

	if (ret == ERR_NONE) {
		*page_status = status_15_0;
		*(page_status + 1) = status_31_16;
		*(page_status + 2) = status_47_32;
		*(page_status + 3) = status_63_48;
		if (has_page_fatal != 0) {
			*block_status = ERR_NAND_BITFLIP_FATAL;
		} else if (has_page_error != 0) {
			*block_status = ERR_NAND_BITFLIP_ERROR;
		} else if (has_page_warn != 0) {
			*block_status = ERR_NAND_BITFLIP_WARN;
		} else {
			*block_status = ERR_NONE;
		}
	}

	return ret;
}

/**
  * @brief  Erase block
  * @param  addr : address, shall be block aligned
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_erase_block(u32 addr, u8 force)
{
	struct nand_ftl_device *nand = &NF_Device;
	u32 offset = nand_get_offset(nand, addr);
	u8 ret;
	u8 is_bad_block;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, offset);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_is_bad(nand, offset, &is_bad_block);
	if (ret != ERR_NONE) {
		return ret;
	}

	if (is_bad_block && (force == 0)) {
		ret = ERR_NAND_BAD_BLOCK;
	} else {
		ret = nand_erase_block(nand, offset);
	}

	return ret;
}

/**
  * @brief  Write page
  * @param  addr : address, shall be page aligned
  * @param  buf : page address
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_write_page(u32 addr, const u8 *buf)
{
	struct nand_ftl_device *nand = &NF_Device;
	struct ameba_flash_info *info = nand->flash_info;
	u32 offset = nand_get_offset(nand, addr);
	u8 ret = ERR_NONE;
	u8 is_bad_block;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, offset);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_is_bad(nand, offset, &is_bad_block);
	if (ret != ERR_NONE) {
		return ret;
	}

	if (is_bad_block) {
		ret = ERR_NAND_BAD_BLOCK;
	} else {
		ret = NAND_Page_Write(offset, 0, info->page_size, (u8 *)buf);
		if (ret == 0U) {
			ret = ERR_NONE;
		} else if (ret == 0xFFU) {
			ret = ERR_TIMEOUT;
		} else {
#if NAND_MARK_BAD_AT_WRITE_FAIL
			ret = nand_mark_bad(nand, offset);
			if (ret == ERR_NONE) {
				ret = ERR_NAND_WORN_BLOCK;
			}
#else
			ret = ERR_HARDWARE;
#endif
		}
	}

	return ret;
}

/**
  * @brief  Check bad block
  * @param  addr : address, shall be block aligned
  * @param  value : 0 - Good; 1 - Bad
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_is_bad(u32 addr, u8 *value)
{
	struct nand_ftl_device *nand = &NF_Device;
	u32 offset = nand_get_offset(nand, addr);
	u8 ret;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, offset);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_is_bad(nand, offset, value);

	return ret;
}

/**
  * @brief  Mark bad block
  * @param  addr : address, shall be block aligned
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_mark_bad(u32 addr)
{
	struct nand_ftl_device *nand = &NF_Device;
	u32 offset = nand_get_offset(nand, addr);
	u8 ret;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	ret = nand_select_target(nand, offset);
	if (ret != ERR_NONE) {
		return ret;
	}

	ret = nand_mark_bad(nand, offset);

	return ret;
}

/**
  * @brief  Read NAND status register
  * @param  cmd : command
  * @param  addr : register address
  * @param  value : value of NAND status register
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_get_status(u8 cmd, u8 addr, u8 *value)
{
	struct nand_ftl_device *nand = &NF_Device;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	NAND_RxCmd(cmd, 1, &addr, 1, value);

	return ERR_NONE;
}

/**
  * @brief  Write NAND status register
  * @param  cmd : command
  * @param  addr : register address
  * @param  value : value of NAND status register
  * @retval ERR_NONE : OK; others : FAIL
  */
u8 nand_ftl_set_status(u8 cmd, u8 addr, u8 value)
{
	struct nand_ftl_device *nand = &NF_Device;

	if (!nand->initialized) {
		return ERR_FAIL;
	}

	NAND_TxCmd(cmd, 1, &addr, 1, &value);

	return NAND_WaitBusy(WAIT_FLASH_BUSY);
}

