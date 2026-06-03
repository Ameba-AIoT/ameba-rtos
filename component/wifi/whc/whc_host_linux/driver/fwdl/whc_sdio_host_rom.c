/******************************************************************************\
 *
 * Copyright(c) Realtek Corporation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
 *
 *
 ******************************************************************************/

#include <whc_host_linux.h>
#include "whc_host_xfer.h"

#define WHC_SDIO_ROM_PAGE_SIZE	4096	/* Use 4KB page size for ROM mode */

/**
 * @brief	Write a single page of firmware data to ROM
 * @param	priv: SDIO device handle
 * @param	offset: Memory offset
 * @param	pdata: Data buffer
 * @param	size: Data size
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_sdio_rom_write_page(struct whc_sdio *priv, u32 offset, void *pdata, u32 size)
{
	u8 ret;
	int try_cnt = 100;
	u8 reg8;
	u8 crc_err_cnt_old, crc_err_cnt_new;
	crc_err_cnt_old = rtw_read8(priv, SDIO_REG_CRC_ERR_INDEX);

	ret = rtw_write32(priv, SDIO_REG_FW_DL_ADDR, offset);
	if (ret) {
		goto exit;
	}

	reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
	reg8 &= (~SD_FW_PAGE_DONE);
	ret = rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);
	if (ret) {
		goto exit;
	}

	/* Write firmware data to TX_FIFO_ADDR */
	sdio_claim_host(priv->func);
	ret = sdio_memcpy_toio(priv->func, SDIO_TX_FW_ADDR | (size >> 2), pdata, size);
	sdio_release_host(priv->func);

	if (ret) {
		dev_err(&priv->func->dev, "Write firmware data fail!\n");
		goto exit;
	}

	msleep(2); /* Wait memory write done */

	for (; try_cnt > 0; try_cnt--) {
		reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
		if (reg8 & SD_FW_PAGE_DONE) {
			break;
		}
		msleep(1);
	}

	if (try_cnt <= 0) {
		dev_err(&priv->func->dev, "Polling SD_FW_PAGE_DONE bit timeout!\n");
		ret = -1;
		goto exit;
	}

	/* Check CRC error */
	crc_err_cnt_new = rtw_read8(priv, SDIO_REG_CRC_ERR_INDEX);
	if (crc_err_cnt_old != crc_err_cnt_new) {
		dev_err(&priv->func->dev, "CRC error: old (%x), new (%x)!\n", crc_err_cnt_old, crc_err_cnt_new);
		ret = -1;
		goto exit;
	}

	ret = 0;

exit:
	if (ret) {
		dev_err(&priv->func->dev, "%s: Write fail (%d)!\n", __FUNCTION__, ret);
	}

	return ret;
}

/**
 * @brief	Verify firmware checksum after ROM write
 * @param	priv: SDIO device handle
 * @param	buf: Firmware data buffer
 * @param	addr: Start address
 * @param	size: Firmware size
 * @retval	Result
 * 			0: Success (checksum match)
 * 			Others: Error (checksum mismatch)
 */
static int whc_sdio_rom_verify_checksum(struct whc_sdio *priv, u8 *buf, u32 addr, u32 size)
{
	u32 checksum = 0;
	u32 checksum_device;
	u32 *data32 = (u32 *)buf;
	u16 reg16;
	u8 reg8;
	int try_cnt = 100;
	int ret = 0;
	u32 i;

	/* Calculate local checksum - XOR all 32-bit words */
	for (i = 0; i < size / 4; i++) {
		checksum ^= data32[i];
	}

	/* Tell device firmware download is done */
	reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
	reg8 |= SD_FW_DL_DONE;
	rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);

	/* Set firmware start address and size for checksum */
	rtw_write32(priv, SDIO_REG_H2C_MSG_EXT, addr);
	rtw_write32(priv, SDIO_REG_H2C_MSG, size);

	/* Set NEED_CHECKSUM flag */
	reg16 = rtw_read16(priv, SDIO_REG_HRPWM2);
	reg16 |= HRPWM2_NEED_CHECKSUM;
	rtw_write16(priv, SDIO_REG_HRPWM2, reg16);

	/* Wait for device checksum ready */
	for (; try_cnt > 0; try_cnt--) {
		if (rtw_read16(priv, SDIO_REG_HCPWM2) & HCPWM2_CHECKSUM_READY) {
			break;
		}
		msleep(10);
	}

	if (try_cnt <= 0) {
		dev_err(&priv->func->dev, "%s: Wait checksum ready timeout!\n", __FUNCTION__);
		ret = -1;
		goto exit;
	}

	/* Read device checksum */
	checksum_device = rtw_read32(priv, SDIO_REG_C2H_MSG_EXT);

	/* Compare checksums */
	if (checksum == checksum_device) {
		dev_dbg(&priv->func->dev, "%s: Checksum success (0x%08X)\n", __FUNCTION__, checksum);
		ret = 0;
	} else {
		dev_err(&priv->func->dev, "%s: Checksum fail! Host=0x%08X, Device=0x%08X\n",
				__FUNCTION__, checksum, checksum_device);
		ret = -1;
	}

	/* Clear flags for retry */
	reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
	reg8 &= ~SD_FW_DL_DONE;
	rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);

	reg16 = rtw_read16(priv, SDIO_REG_HRPWM2);
	reg16 &= ~HRPWM2_NEED_CHECKSUM;
	rtw_write16(priv, SDIO_REG_HRPWM2, reg16);

exit:
	return ret;
}

/**
 * @brief	Write firmware data in ROM mode - writes all pages
 * @param	adapter: Adapter handler
 * @param	buf: Data buffer
 * @param	addr: Target address
 * @param	size: Data size
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 * @note	ROM mode always uses 4-byte aligned writes
 */
static int whc_sdio_rom_write_firmware(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size)
{
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;
	u32 page_num = size / WHC_SDIO_ROM_PAGE_SIZE;
	u32 remain_size = size % WHC_SDIO_ROM_PAGE_SIZE;
	u32 page;
	u32 offset;
	u32 write_size;
	u8 *write_buf;
	int ret = 0;
	int retry_cnt = 3;

	/* Write complete pages */
	for (page = 0; page < page_num;) {
		offset = page * WHC_SDIO_ROM_PAGE_SIZE;
		ret = whc_sdio_rom_write_page(priv, addr + offset, buf + offset, WHC_SDIO_ROM_PAGE_SIZE);
		if (ret == 0) {
			page++;
			retry_cnt = 3;
		} else if (--retry_cnt <= 0) {
			dev_err(&priv->func->dev, "Page %d write fail\n", page);
			return ret;
		}
	}

	/* Handle remaining data with 4-byte alignment */
	if (remain_size) {
		offset = page_num * WHC_SDIO_ROM_PAGE_SIZE;
		write_size = (remain_size + 3) & ~3;  /* 4-byte aligned */

		if (write_size > remain_size) {
			write_buf = whc_xfer_zmalloc(write_size);
			if (!write_buf) {
				dev_err(&priv->func->dev, "Fail to malloc buffer\n");
				return -1;
			}
			memcpy(write_buf, buf + offset, remain_size);
		} else {
			write_buf = buf + offset;
		}

		ret = whc_sdio_rom_write_page(priv, addr + offset, write_buf, write_size);

		if (write_size > remain_size) {
			whc_xfer_mfree(write_buf);
		}
	}

	return ret;
}

/**
 * @brief	Write data in ROM mode with verification
 * @param	adapter: Adapter handler
 * @param	buf: Data buffer
 * @param	addr: Target address
 * @param	size: Data size
 * @param	need_padding: Unused (ROM mode always uses 4-byte aligned padding)
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
int whc_sdio_rom_write(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding)
{
	int ret;
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;

	(void)need_padding;  /* Unused - ROM mode always uses 4-byte aligned padding */

	/* Write firmware data */
	ret = whc_sdio_rom_write_firmware(adapter, buf, addr, size);
	if (ret != 0) {
		dev_err(&priv->func->dev, "Firmware write failed (%d)\n", ret);
		return ret;
	}

#if CONFIG_WHC_DOWNLOAD_CHK_EN
	/* Verify checksum after write completes */
	ret = whc_sdio_rom_verify_checksum(priv, buf, addr, size);
	if (ret != 0) {
		dev_err(&priv->func->dev, "Checksum verification failed (%d)\n", ret);
		return ret;
	}
#endif

	return 0;
}

/**
 * @brief	Boot image in ROM mode - SDIO specific handling
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 * @note	On success, sets dev_state to WHC_XFER_FW_TYPE_BOOTLOADER
 */
int whc_sdio_rom_boot(struct whc_xfer_adapter_t *adapter)
{
	struct whc_sdio *priv = (struct whc_sdio *)adapter->interface;
	u8 reg8;
	u16 reg16;
	int try_cnt = 3;

	/* Tell device all images have been downloaded */
	for (; try_cnt > 0; try_cnt--) {
		reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
		reg8 |= SD_FW_DL_DONE;
		rtw_write8(priv, SDIO_REG_FW_DL_CTRL, reg8);
		rtw_write16(priv, SDIO_REG_HRPWM2, HRPWM2_BOOT_RAM);

		reg8 = rtw_read8(priv, SDIO_REG_FW_DL_CTRL);
		reg16 = rtw_read16(priv, SDIO_REG_HRPWM2);
		if ((reg8 & SD_FW_DL_DONE) && (reg16 & HRPWM2_BOOT_RAM)) {
			break;
		}
	}

	if (try_cnt <= 0) {
		dev_err(&priv->func->dev, "Fail to Set FW download complete bit\n");
		return -1;
	}

	return 0;
}
