/******************************************************************************
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

#include "whc_host_xfer.h"
#include <linux/fs.h>
#include <linux/slab.h>
#include <crypto/hash.h>
#include <linux/crypto.h>

#define CONFIG_WHC_TP_TEST_EN		0

#define WHC_XFER_DBG(fmt, arg...)
#define WHC_XFER_LOG(fmt, arg...) pr_info("[WHC] " fmt, ##arg)

static int whc_xfer_get_region_index(struct whc_xfer_adapter_t *adapter, u32 addr)
{
	int result = -1;
	int index;
	struct whc_mem_region_t *region;

	for (index = 0; ; ++index) {
		region = &adapter->regions[index];
		if ((region->start_addr == 0U) && (region->end_addr == 0U)) {
			break;
		}
		if ((addr >= region->start_addr) && (addr < region->end_addr)) {
			result = index;
			break;
		}
	}

	return result;
}

/*
 * @brief	Validate the hash by comparing file content hash with device hash
 * @param	adapter: Adapter handler
 * @param	file: File handle
 * @param	offset: File offset (will be modified)
 * @param	size: Data size to check
 * @param	hash: Hash from device
 * @retval	Result
 * 			0: Hash match (skip download)
 * 			<0: Hash mismatch or error (need to download)
 */
static int whc_xfer_check_hash(struct whc_xfer_adapter_t *adapter, struct file *file, loff_t *offset, u32 size, const u8 *hash)
{
	struct crypto_shash *tfm;
	struct shash_desc *desc;
	u8 *file_hash;
	int read_size;
	int remain_size = size;
	int ret = -1;
	loff_t file_offset = *offset;

	/* Allocate buffer for calculated hash */
	file_hash = (u8 *)whc_xfer_zmalloc(adapter->hash_size);
	if (file_hash == NULL) {
		WHC_XFER_LOG("Fail to malloc file hash buffer\n");
		return -1;
	}

	/* Allocate SHA-256 transform */
	tfm = crypto_alloc_shash("sha256", 0, 0);
	if (IS_ERR(tfm)) {
		WHC_XFER_LOG("Fail to alloc sha256 tfm (%ld)\n", PTR_ERR(tfm));
		goto exit_free_hash;
	}

	/* Allocate shash_desc */
	desc = (struct shash_desc *)whc_xfer_zmalloc(sizeof(struct shash_desc) + crypto_shash_descsize(tfm));
	if (desc == NULL) {
		WHC_XFER_LOG("Fail to malloc shash_desc\n");
		goto exit_free_tfm;
	}
	desc->tfm = tfm;

	/* Initialize hash */
	ret = crypto_shash_init(desc);
	if (ret < 0) {
		WHC_XFER_LOG("Fail to init sha256 (%d)\n", ret);
		goto exit_free_desc;
	}

	/* Read file and calculate hash */
	while (remain_size > 0) {
		read_size = (remain_size < adapter->read_buf_size) ? remain_size : adapter->read_buf_size;
		ret = whc_xfer_read_file(file, adapter->read_buf, read_size, &file_offset);
		if (ret < read_size) {
			WHC_XFER_LOG("Fail to read file for hash (%d)\n", ret);
			ret = -1;
			goto exit_free_desc;
		}

		ret = crypto_shash_update(desc, adapter->read_buf, read_size);
		if (ret < 0) {
			WHC_XFER_LOG("Fail to update sha256 (%d)\n", ret);
			goto exit_free_desc;
		}

		remain_size -= read_size;
	}

	/* Finalize hash */
	ret = crypto_shash_final(desc, file_hash);
	if (ret < 0) {
		WHC_XFER_LOG("Fail to finalize sha256 (%d)\n", ret);
		goto exit_free_desc;
	}

	/* Compare hashes */
	if (memcmp(file_hash, hash, adapter->hash_size) == 0) {
		WHC_XFER_LOG("Hash match, skip download\n");
		*offset = file_offset;  /* Update offset for caller */
		ret = 0;  /* Hash match - skip download */
	} else {
		WHC_XFER_LOG("Hash mismatch, need download\n");
		ret = -1;  /* Hash mismatch - need download */
	}

exit_free_desc:
	whc_xfer_mfree(desc);

exit_free_tfm:
	crypto_free_shash(tfm);

exit_free_hash:
	whc_xfer_mfree(file_hash);

	return ret;
}

/*
 * @brief	Validate the images
 * @param	adapter: Adapter handler
 * @param	image: Image
 * @retval	Result
 * 			0: OK
 * 			<0: Error
 */
static int whc_xfer_check_image(struct whc_xfer_adapter_t *adapter, struct whc_image_t *image)
{
	struct whc_image_header_t *header;
	struct whc_mem_region_t *region;
	struct file *fp;
	loff_t offset = 0;
	int ret;
	int region_index;
	int sub_image_size;
	int image_file_size;
	int remain_image_size;
	int sub_image_cnt = 0;
	u32 sub_image_start_addr;
	u32 sub_image_end_addr;
	char *image_path;

	image_path = whc_xfer_join_path(adapter->image_dir, image->image_name);

	WHC_XFER_LOG("Check image: %s\n", image_path);

	ret = whc_xfer_check_file_exists(image_path);
	if (ret == 0) {
		WHC_XFER_LOG("Image file does not exist: %s\n", image_path);
		ret = -1;
		goto exit_free_path;
	}

	fp = whc_xfer_open_file(image_path);
	if (fp == NULL) {
		ret = -1;
		WHC_XFER_LOG("Fail to open image file\n");
		goto exit_free_path;
	}

	ret = whc_xfer_get_file_size(fp);
	if (ret <= 0) {
		ret = -1;
		WHC_XFER_LOG("Fail to read image size (%d)\n", ret);
		goto exit_close_file;
	}

	image_file_size = ret;
	ret = 0;

	if (adapter->manifest_pos == WHC_XFER_MANIFEST_POS_HEAD) {
		offset = WHC_XFER_MANIFEST_SIZE;
	}

	remain_image_size = image_file_size - WHC_XFER_MANIFEST_SIZE;

	while (remain_image_size >= WHC_XFER_IMAGE_HEADER_SIZE) {
		ret = whc_xfer_read_file(fp, adapter->read_buf, WHC_XFER_IMAGE_HEADER_SIZE, &offset);
		if (ret < WHC_XFER_IMAGE_HEADER_SIZE) {
			WHC_XFER_LOG("Fail to read image (%d)\n", ret);
			ret = -1;
			break;
		}
		ret = 0;
		header = (struct whc_image_header_t *)adapter->read_buf;
		if ((header->signature.d32[0] == image->signature[0]) && (header->signature.d32[1] == image->signature[1])) {
			sub_image_cnt++;
			sub_image_size = header->image_size + WHC_XFER_IMAGE_HEADER_SIZE;
			if (header->image_size == 0) {
				WHC_XFER_DBG("Sub-image (0x%08X) skipped\n", header->image_addr);
				remain_image_size -= sub_image_size;
				continue;
			}

			sub_image_start_addr = header->image_addr - WHC_XFER_IMAGE_HEADER_SIZE;
			sub_image_end_addr = sub_image_start_addr + sub_image_size - 1;
			WHC_XFER_DBG("Check sub-image (0x%08X)\n", sub_image_start_addr);
			region_index = whc_xfer_get_region_index(adapter, sub_image_start_addr);
			if (region_index < 0) {
				WHC_XFER_LOG("Invalid image: bad address 0x%08X\n", sub_image_start_addr);
				ret = -1;
				break;
			}

			region = &adapter->regions[region_index];
			if ((sub_image_end_addr < region->start_addr) || (sub_image_end_addr >= region->end_addr)) {
				WHC_XFER_LOG("Invalid image: bad address range 0x%08X-0x%08X\n", sub_image_start_addr, sub_image_end_addr);
				ret = -1;
				break;
			}

			if ((region->mem_type == WHC_MEM_TYPE_FLASH) && (image->image_type == WHC_IMAGE_TYPE_BOOTLOADER)) {
				WHC_XFER_LOG("Invalid bootloader image: flash sub-image not supported\n");
				ret = -1;
			}

			WHC_XFER_DBG("Sub-image (0x%08X) OK\n", sub_image_start_addr);

			remain_image_size -= sub_image_size;
			offset += header->image_size;
		} else {
			WHC_XFER_LOG("Invalid image: bad signature\n");
			ret = -1;
			break;
		}
	}

	if ((ret == 0) && (sub_image_cnt == 0)) {
		WHC_XFER_LOG("Invalid image: no valid sub-images\n");
		ret = -1;
	}

exit_close_file:
	whc_xfer_close_file(fp);

exit_free_path:
	whc_xfer_mfree(image_path);

	if (ret == 0) {
		WHC_XFER_LOG("Image OK\n");
	}

	return ret;
}

/*
 * @brief	Validate the images
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: OK
 * 			<0: Error
 */
static int whc_xfer_check_images(struct whc_xfer_adapter_t *adapter)
{
	int ret = -1;
	int i = 0;
	struct whc_image_t *image;

	while (true) {
		image = adapter->images + i;
		if (image->image_name != NULL) {
			ret = whc_xfer_check_image(adapter, image);
			if (ret < 0) {
				break;
			}
			i++;
		} else {
			break;
		}
	}

	return ret;
}

static int whc_xfer_check_protocol_version(struct whc_xfer_adapter_t *adapter, u16 protocol_version)
{
	int ret = 0;
	u16 min = adapter->min_protocol_version;
	u16 max = adapter->max_protocol_version;

	if (min == 0U) {
		if (max != 0U) {
			if (protocol_version > max) {
				ret = -1;
			} else {
				/* OK */
			}
		} else {
			/* OK */
		}
	} else {
		if (protocol_version < min) {
			ret = -1;
		} else if (max != 0U) {
			if (protocol_version > max) {
				ret = -1;
			} else {
				/* OK */
			}
		} else {
			/* OK */
		}
	}

	return ret;
}

/*
 * @brief	Validate the device info
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: OK
 * 			<0: Error
 */
static int whc_xfer_check_device(struct whc_xfer_adapter_t *adapter, int image_type)
{
	char *image_path;
	struct whc_image_t *image;
	struct whc_mem_region_t *region;
	struct whc_image_header_t *header;
	struct whc_dev_info_t *device_info = &adapter->device_info;
	struct file *fp;
	loff_t offset = 0;
	u32 sub_image_start_addr;
	u32 sub_image_end_addr;
	u32 sub_image_size;
	int image_file_size;
	int remain_image_size;
	int region_index = 0;
	int ret = 0;

	WHC_XFER_LOG("Check device info\n");

	WHC_XFER_LOG("Chip ID: 0x%04X\n", device_info->chip_id);
	WHC_XFER_LOG("Chip Version: 0x%02X\n", device_info->cut_version);
	WHC_XFER_LOG("Protocol Version: 0x%04X\n", device_info->protocol_version);
	WHC_XFER_LOG("SRAM Size: %dKB\n", device_info->sram_size);
	WHC_XFER_LOG("PSRAM Size: %dKB\n", device_info->psram_size);
	WHC_XFER_LOG("Flash Size: %dKB\n", device_info->flash_size);
	WHC_XFER_LOG("TCM Size: %dKB\n", device_info->tcm_size);

	if (adapter->chip_id != device_info->chip_id) {
		WHC_XFER_LOG("Unsupported device: 0x%04X\n", device_info->chip_id);
		ret = -1;
		goto exit;
	}

	ret = whc_xfer_check_protocol_version(adapter, device_info->protocol_version);
	if (ret < 0) {
		WHC_XFER_LOG("Unsupported protocol version: 0x%04X\n", device_info->protocol_version);
		goto exit;
	}

	image = &adapter->images[image_type];
	image_path = whc_xfer_join_path(adapter->image_dir, image->image_name);

	ret = whc_xfer_check_file_exists(image_path);
	if (ret == 0) {
		ret = -1;
		WHC_XFER_LOG("Image file does not exist: %s\n", image_path);
		goto exit_free_path;
	} else if (ret < 0) {
		WHC_XFER_LOG("Image file %s access error: %d\n", image_path, ret);
		goto exit_free_path;
	}

	fp = whc_xfer_open_file(image_path);
	if (fp == NULL) {
		ret = -1;
		WHC_XFER_LOG("Fail to open image file\n");
		goto exit_free_path;
	}

	ret = whc_xfer_get_file_size(fp);
	if (ret <= 0) {
		ret = -1;
		WHC_XFER_LOG("Fail to read image size (%d)\n", ret);
		goto exit_close_file;
	}

	image_file_size = ret;
	ret = 0;

	if (adapter->manifest_pos == WHC_XFER_MANIFEST_POS_HEAD) {
		offset = WHC_XFER_MANIFEST_SIZE;
	}

	remain_image_size = image_file_size - WHC_XFER_MANIFEST_SIZE;

	while (remain_image_size >= WHC_XFER_IMAGE_HEADER_SIZE) {
		WHC_XFER_DBG("Remain size %d\n", remain_image_size);
		ret = whc_xfer_read_file(fp, adapter->read_buf, WHC_XFER_IMAGE_HEADER_SIZE, &offset);
		if (ret < WHC_XFER_IMAGE_HEADER_SIZE) {
			WHC_XFER_LOG("Fail to read image (%d)\n", ret);
			ret = -1;
			break;
		}
		ret = 0;
		header = (struct whc_image_header_t *)adapter->read_buf;
		if ((header->signature.d32[0] == image->signature[0]) && (header->signature.d32[1] == image->signature[1])) {
			sub_image_size = header->image_size + WHC_XFER_IMAGE_HEADER_SIZE;
			sub_image_start_addr = header->image_addr - WHC_XFER_IMAGE_HEADER_SIZE;
			sub_image_end_addr = sub_image_start_addr + sub_image_size - 1;
			if (header->image_size == 0) {
				WHC_XFER_DBG("Sub-image (0x%08X) skipped\n", sub_image_start_addr);
				remain_image_size -= sub_image_size;
				continue;
			}
			if (header->image_size == 0xFFFFFFFFU) {
				/* Manifest image detected - should not be here */
				break;
			}

			WHC_XFER_DBG("Check sub-image (0x%08X)\n", sub_image_start_addr);
			region_index = whc_xfer_get_region_index(adapter, sub_image_start_addr);
			if (region_index < 0) {
				WHC_XFER_LOG("Invalid image: bad address 0x%08X\n", sub_image_start_addr);
				ret = -1;
				break;
			}

			region = &adapter->regions[region_index];
			if ((sub_image_end_addr < region->start_addr) || (sub_image_end_addr >= region->end_addr)) {
				WHC_XFER_LOG("Invalid image: bad address range 0x%08X-0x%08X\n", sub_image_start_addr, sub_image_end_addr);
				ret = -1;
				break;
			}

			if ((region->mem_type == WHC_MEM_TYPE_FLASH) && (sub_image_size > (u32)(device_info->flash_size) * 1024)) {
				WHC_XFER_LOG("Invalid image: flash sub-image overrun\n");
				ret = -1;
				break;
			} else if ((region->mem_type == WHC_MEM_TYPE_SRAM) && (sub_image_size > (u32)(device_info->sram_size) * 1024)) {
				WHC_XFER_LOG("Invalid image: SRAM sub-image overrun\n");
				ret = -1;
				break;
			} else if ((region->mem_type == WHC_MEM_TYPE_PSRAM) && (sub_image_size > (u32)(device_info->psram_size) * 1024)) {
				WHC_XFER_LOG("Invalid image: PSRAM sub-image overrun\n");
				ret = -1;
				break;
			} else if ((region->mem_type == WHC_MEM_TYPE_TCM) && (sub_image_size > (u32)(device_info->tcm_size) * 1024)) {
				WHC_XFER_LOG("Invalid image: TCM sub-image overrun\n");
				ret = -1;
				break;
			}

			WHC_XFER_DBG("Sub-image (0x%08X) OK\n", sub_image_start_addr);
			remain_image_size -= sub_image_size;
			offset += header->image_size;
		} else {
			WHC_XFER_LOG("Invalid image: bad signature\n");
			ret = -1;
			break;
		}
	}

exit_close_file:
	whc_xfer_close_file(fp);

exit_free_path:
	whc_xfer_mfree(image_path);

	if (ret == 0) {
		WHC_XFER_LOG("Image OK\n");
	}

exit:
	return ret;
}

static int whc_xfer_page_write(struct whc_xfer_adapter_t *adapter, int offset, u8 *buf, int size)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	struct whc_xfer_desc_t *tx_desc;
	struct whc_xfer_desc_t *rx_desc;
	u8 *tx_buff = adapter->tx_buf;
	u8 *rx_buff = adapter->rx_buf;
	int tx_size = size + WHC_XFER_DESC_SIZE;
	int rx_size = WHC_XFER_DESC_SIZE;
	int timeout = ((size + WHC_XFER_FLASH_PAGE_SIZE - 1) / WHC_XFER_FLASH_PAGE_SIZE) * WHC_XFER_PAGE_WRITE_TIMEOUT;
	int ret;
#if CONFIG_WHC_WRITE_CHK_EN
	int i = 0;
	u32 checksum = 0;
	u32 *data_buf = (u32 *)buf;
#endif

#if CONFIG_WHC_TP_TEST_EN
	u32 write_start_time;
#endif

	if ((tx_buff == NULL) || (rx_buff == NULL)) {
		return -1;
	}

#if CONFIG_WHC_WRITE_CHK_EN
	for (i = 0; i < size / 4; ++i) {
		checksum ^= data_buf[i];
	}
#endif

	tx_desc = (struct whc_xfer_desc_t *)tx_buff;
	tx_desc->data_len = size;
	tx_desc->data_offset = WHC_XFER_DESC_SIZE;
	tx_desc->type = WHC_XFER_REQ_WRITE;
	tx_desc->addr = offset;
#if CONFIG_WHC_WRITE_CHK_EN
	tx_desc->config = WHC_XFER_CFG_WRITE_CHK_MASK;
	tx_desc->value.d32 = checksum;
#endif

	whc_xfer_memcpy(tx_buff + WHC_XFER_DESC_SIZE, buf, size);

#if CONFIG_WHC_TP_TEST_EN
	write_start_time = whc_xfer_get_time();
#endif
	ret = ops->handshake(adapter, tx_buff, tx_size, rx_buff, rx_size, timeout);
#if CONFIG_WHC_TP_TEST_EN
	WHC_XFER_LOG("Write %d time %dms\n", size, whc_xfer_get_elapsed_ms(write_start_time));
#endif
	if (ret == 0) {
		rx_desc = (struct whc_xfer_desc_t *)rx_buff;
		WHC_XFER_DBG("Write response: len %d type 0x%02X status %d addr 0x%08X size %d\n",
					 rx_desc->data_len, rx_desc->type, rx_desc->status, rx_desc->addr, rx_desc->size);
		if ((rx_desc->type == WHC_XFER_ACK_WRITE) && (rx_desc->status == WHC_XFER_STAUS_OK) && (rx_desc->addr == offset) && (rx_desc->size == size)) {
			WHC_XFER_DBG("Write OK\n");
		} else {
			WHC_XFER_LOG("Write failed (%d)\n", rx_desc->status);
			ret = -1;
		}
	} else {
		WHC_XFER_LOG("Write request error (%d)\n", ret);
	}

	return ret;
}

static int whc_xfer_calculate_checksum(struct whc_xfer_adapter_t *adapter, int offset, int size, u32 checksum)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	u8 *tx_buff = adapter->tx_buf;
	u8 *rx_buff = adapter->rx_buf;
	struct whc_xfer_desc_t *tx_desc;
	struct whc_xfer_desc_t *rx_desc;
	int tx_size = WHC_XFER_DESC_SIZE;
	int rx_size = WHC_XFER_DESC_SIZE;
	int timeout = ((size + 1024 - 1) / 1024) * WHC_XFER_FLASH_CHECKSUM_1KB_TIMEOUT;
	int ret;

#if CONFIG_WHC_TP_TEST_EN
	u32 start_time = whc_xfer_get_time();
#endif

	if ((tx_buff == NULL) || (rx_buff == NULL)) {
		return -1;
	}

	tx_desc = (struct whc_xfer_desc_t *)tx_buff;
	tx_desc->data_len = 0;
	tx_desc->data_offset = WHC_XFER_DESC_SIZE;
	tx_desc->type = WHC_XFER_REQ_CHECK;
	tx_desc->addr = offset;
	tx_desc->size = size;

	ret = ops->handshake(adapter, tx_buff, tx_size, rx_buff, rx_size, timeout);
	if (ret == 0) {
		rx_desc = (struct whc_xfer_desc_t *)rx_buff;
		WHC_XFER_DBG("Checksum response: len %d type 0x%02X status %d addr 0x%08X size %d\n",
					 rx_desc->data_len, rx_desc->type, rx_desc->status, rx_desc->addr, rx_desc->size);
		if ((rx_desc->type == WHC_XFER_ACK_CHECK) && (rx_desc->status == WHC_XFER_STAUS_OK) && (rx_desc->addr == offset) && (rx_desc->size == size)) {
			if (checksum == rx_desc->value.d32) {
				WHC_XFER_DBG("Checksum OK\n");
			} else {
				WHC_XFER_LOG("Checksum error: expected 0x%08X got 0x%08X\n", checksum, rx_desc->value.d32);
				ret = -1;
			}
		} else {
			WHC_XFER_LOG("Checksum response error\n");
			ret = -1;
		}
	} else {
		WHC_XFER_LOG("Checksum request error (%d)\n", ret);
	}

#if CONFIG_WHC_TP_TEST_EN
	WHC_XFER_LOG("Checksum cost %dms\n", whc_xfer_get_elapsed_ms(start_time));
#endif

	return ret;
}

int whc_xfer_write(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding)
{
	int i;
	int retry;
	int ret = 0;
	u32 page_num;
	u32 remain_size;
	u32 page;
	u32 offset;
	u32 write_size = 0;
	u32 checksum = 0;
	u8 *write_buf;
	u32 *align_buf;

	page_num = size / adapter->xfer_page_size;
	remain_size = size % adapter->xfer_page_size;

	for (page = 0; page < page_num; page++) {
		offset = page * adapter->xfer_page_size;
		WHC_XFER_DBG("Firmware write addr=0x%08x len=%d\n", addr + offset, adapter->xfer_page_size);
		ret = whc_xfer_page_write(adapter, addr + offset, buf + offset, adapter->xfer_page_size);
		if (ret == 0) {
			WHC_XFER_DBG("Firmware page %d write success\n", page);
		} else {
			WHC_XFER_LOG("Firmware page %d write fail (%d)\n", page, ret);
			break;
		}
	}

	if (ret != 0) {
		return ret;
	}

#if CONFIG_WHC_DOWNLOAD_CHK_EN
	align_buf = (u32 *)buf;
	for (i = 0; i < (size - remain_size) / 4; i++) {
		checksum ^= align_buf[i];
	}
#endif

	if (remain_size) {
		offset = page_num * adapter->xfer_page_size;

		if (need_padding) {
			retry = 0;
			while (retry++ < 3) {
				write_buf = (u8 *)whc_xfer_zmalloc(adapter->xfer_page_size);
				if (write_buf != NULL) {
					break;
				} else {
					/* Wait for system memory release */
					whc_xfer_msleep(1);
				}
			}

			if (write_buf == NULL) {
				WHC_XFER_LOG("Fail to malloc write buffer\n");
				return -1;
			}

			whc_xfer_memcpy(write_buf, buf + offset, remain_size);
			whc_xfer_memset(write_buf + remain_size, 0xFF, adapter->xfer_page_size - remain_size);
			write_size = adapter->xfer_page_size;
		} else {
			write_size = ((remain_size + 3) >> 2) << 2;
			if (write_size > remain_size) {
				retry = 0;
				while (retry++ < 3) {
					write_buf = (u8 *)whc_xfer_zmalloc(write_size);
					if (write_buf != NULL) {
						break;
					} else {
						/* Wait for system memory release */
						whc_xfer_msleep(1);
					}
				}

				if (write_buf == NULL) {
					WHC_XFER_LOG("Fail to malloc write buffer\n");
					return -1;
				}
				whc_xfer_memcpy(write_buf, buf + offset, remain_size);
			} else {
				write_buf = buf + offset;
			}
		}

		WHC_XFER_DBG("Firmware write remain addr=0x%08x len=%d(%d)\n", addr + offset, write_size, remain_size);

		ret = whc_xfer_page_write(adapter, addr + offset, write_buf, write_size);
		if (ret == 0) {
#if CONFIG_WHC_DOWNLOAD_CHK_EN
			align_buf = (u32 *)write_buf;
			for (i = 0; i < write_size / 4; i++) {
				checksum ^= align_buf[i];
			}
#endif
		} else {
			WHC_XFER_LOG("Firmware page %d write fail, retry %d#\n", page, retry);
		}

		/* Free allocated buffer if it was dynamically allocated */
		if ((need_padding || write_size > remain_size) && write_buf != NULL) {
			whc_xfer_mfree(write_buf);
		}
	}

#if CONFIG_WHC_DOWNLOAD_CHK_EN
	if (ret == 0) {
		ret = whc_xfer_calculate_checksum(adapter, addr, size - remain_size + write_size, checksum);
		if (ret == 0) {
			WHC_XFER_DBG("Checksum OK");
		} else {
			WHC_XFER_LOG("Checksum error, retry %d#\n", retry);
		}
	}
#endif

	return ret;
}

/*
 * @brief	Boot image
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
int whc_xfer_boot(struct whc_xfer_adapter_t *adapter)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	u8 *tx_buff = adapter->tx_buf;
	u8 *rx_buff = adapter->rx_buf;
	struct whc_xfer_desc_t *tx_desc;
	struct whc_xfer_desc_t *rx_desc;
	int tx_size = WHC_XFER_DESC_SIZE;
	int rx_size = WHC_XFER_DESC_SIZE;
	int ret;

	if ((tx_buff == NULL) || (rx_buff == NULL)) {
		return -1;
	}

	tx_desc = (struct whc_xfer_desc_t *)tx_buff;
	tx_desc->data_len = 0;
	tx_desc->data_offset = WHC_XFER_DESC_SIZE;
	tx_desc->type = WHC_XFER_REQ_BOOT;

	ret = ops->handshake(adapter, tx_buff, tx_size, rx_buff, rx_size, WHC_XFER_DEFAULT_TIMEOUT);
	if (ret == 0) {
		rx_desc = (struct whc_xfer_desc_t *)rx_buff;
		if ((rx_desc->type == WHC_XFER_ACK_BOOT) && (rx_desc->status == WHC_XFER_STAUS_OK)) {
			WHC_XFER_DBG("Boot response OK\n");
		} else {
			WHC_XFER_LOG("Boot failed (%d)\n", rx_desc->status);
			ret = -1;
		}
	} else {
		WHC_XFER_LOG("Boot request error (%d)\n", ret);
	}

	return ret;
}

/*
 * @brief	Query device info
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_xfer_query(struct whc_xfer_adapter_t *adapter)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	struct whc_dev_info_t *info = &adapter->device_info;
	u8 *tx_buff = adapter->tx_buf;
	u8 *rx_buff = adapter->rx_buf;
	struct whc_xfer_desc_t *tx_desc;
	struct whc_xfer_desc_t *rx_desc;
	int tx_size = WHC_XFER_DESC_SIZE;
	int rx_size = WHC_XFER_DESC_SIZE + WHC_XFER_DEVICE_INFO_SIZE;
	int ret;

	if ((tx_buff == NULL) || (rx_buff == NULL)) {
		return -1;
	}

#if CONFIG_WHC_TP_TEST_EN
	u32 start_time = whc_xfer_get_time();
#endif

	tx_desc = (struct whc_xfer_desc_t *)tx_buff;
	tx_desc->data_len = 0;
	tx_desc->data_offset = WHC_XFER_DESC_SIZE;
	tx_desc->type = WHC_XFER_REQ_QUERY;

	ret = ops->handshake(adapter, tx_buff, tx_size, rx_buff, rx_size, WHC_XFER_QUERY_TIMEOUT);
	if (ret == 0) {
		rx_desc = (struct whc_xfer_desc_t *)rx_buff;
		WHC_XFER_DBG("Query response len %d type 0x%02X mode 0x%02X status %d addr 0x%08X size %d\n",
					 rx_desc->data_len, rx_desc->type, rx_desc->mode, rx_desc->status, rx_desc->addr, rx_desc->size);
		if ((rx_desc->type == WHC_XFER_ACK_QUERY) && (rx_desc->status == WHC_XFER_STAUS_OK)) {
			whc_xfer_memcpy(info, rx_buff + WHC_XFER_DESC_SIZE, WHC_XFER_DEVICE_INFO_SIZE);
		} else {
			WHC_XFER_LOG("Query failed (%d)\n", rx_desc->status);
			ret = -1;
		}
	} else {
		WHC_XFER_LOG("Query request error (%d)\n", ret);
	}

#if CONFIG_WHC_TP_TEST_EN
	WHC_XFER_LOG("Query cost %dms\n", whc_xfer_get_elapsed_ms(start_time));
#endif

	return ret;
}

/*
 * @brief	Calculate flash hash
 * @param	adapter: Adapter handler
 * @param	addr: Flash address
 * @param	size: Flash size
 * @param	hash: Hash data buffer
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_xfer_calculate_hash(struct whc_xfer_adapter_t *adapter, u32 addr, u32 size, u8 *hash)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	u8 *tx_buff = adapter->tx_buf;
	u8 *rx_buff = adapter->rx_buf;
	struct whc_xfer_desc_t *tx_desc;
	struct whc_xfer_desc_t *rx_desc;
	int tx_size = WHC_XFER_DESC_SIZE;
	int rx_size = WHC_XFER_DESC_SIZE + adapter->hash_size;
	int timeout = ((size + 1024 - 1) / 1024) * WHC_XFER_FLASH_HASH_1KB_TIMEOUT;
	int ret;

	if ((tx_buff == NULL) || (rx_buff == NULL)) {
		return -1;
	}

#if CONFIG_WHC_TP_TEST_EN
	u32 start_time = whc_xfer_get_time();
#endif

	tx_desc = (struct whc_xfer_desc_t *)tx_buff;
	tx_desc->data_len = 0;
	tx_desc->data_offset = WHC_XFER_DESC_SIZE;
	tx_desc->type = WHC_XFER_REQ_HASH;
	tx_desc->addr = addr;
	tx_desc->size = size;

	ret = ops->handshake(adapter, tx_buff, tx_size, rx_buff, rx_size, timeout);
	if (ret == 0) {
		rx_desc = (struct whc_xfer_desc_t *)rx_buff;
		WHC_XFER_DBG("Hash response len %d type 0x%02X mode 0x%02X status %d addr 0x%08X size %d\n",
					 rx_desc->data_len, rx_desc->type, rx_desc->mode, rx_desc->status, rx_desc->addr, rx_desc->size);
		if ((rx_desc->type == WHC_XFER_ACK_HASH) && (rx_desc->status == WHC_XFER_STAUS_OK) && (rx_desc->addr == addr) && (rx_desc->size == size)) {
			whc_xfer_memcpy(hash, rx_buff + WHC_XFER_DESC_SIZE, adapter->hash_size);
		} else {
			WHC_XFER_LOG("Hash failed (%d)\n", rx_desc->status);
			ret = -1;
		}
	} else {
		WHC_XFER_LOG("Hash request error (%d)\n", ret);
	}

#if CONFIG_WHC_TP_TEST_EN
	WHC_XFER_LOG("Hash cost %dms\n", whc_xfer_get_elapsed_ms(start_time));
#endif

	return ret;
}

/*
 * @brief	Erase flash
 * @param	adapter: Adapter handler
 * @retval	Result
 * 			0: Success
 * 			Others: Error
 */
static int whc_xfer_erase(struct whc_xfer_adapter_t *adapter, u32 addr, u32 size)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	u8 *tx_buff = adapter->tx_buf;
	u8 *rx_buff = adapter->rx_buf;
	struct whc_xfer_desc_t *tx_desc;
	struct whc_xfer_desc_t *rx_desc;
	int tx_size = WHC_XFER_DESC_SIZE;
	int rx_size = WHC_XFER_DESC_SIZE;
	int timeout = (size + 4096 - 1) / 4096 * WHC_XFER_FLASH_ERASE_4KB_TIMEOUT;
	int ret;

	if ((tx_buff == NULL) || (rx_buff == NULL)) {
		return -1;
	}

	tx_desc = (struct whc_xfer_desc_t *)tx_buff;
	tx_desc->data_len = 0;
	tx_desc->data_offset = WHC_XFER_DESC_SIZE;
	tx_desc->type = WHC_XFER_REQ_ERASE;
	tx_desc->addr = addr;
	tx_desc->size = size;

	ret = ops->handshake(adapter, tx_buff, tx_size, rx_buff, rx_size, timeout);
	if (ret == 0) {
		rx_desc = (struct whc_xfer_desc_t *)rx_buff;
		if ((rx_desc->type == WHC_XFER_ACK_ERASE) && (rx_desc->status == WHC_XFER_STAUS_OK)) {
			WHC_XFER_LOG("Erase done\n");
		} else {
			WHC_XFER_LOG("Erase failed (%d)\n", rx_desc->status);
			ret = -1;
		}
	} else {
		WHC_XFER_LOG("Erase request error (%d)\n", ret);
	}

	return ret;
}

/*
 * @brief	Download sub-image data from file to device (helper function)
 * @param	adapter: Adapter handler
 * @param	fp: File pointer
 * @param	image: Image configuration
 * @param	sub_image_start_addr: Target address
 * @param	sub_image_size: Total size to download
 * @param	need_padding: Whether padding is needed
 * @param	offset: File offset (will be modified)
 * @retval	Result: 0 on success, <0 on error
 */
static int whc_xfer_download_sub_image_data(struct whc_xfer_adapter_t *adapter,
		struct file *fp, struct whc_image_t *image, u32 sub_image_start_addr,
		u32 sub_image_size, bool need_padding, loff_t *offset)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	int remain_sub_image_size;
	int read_size;
	int write_size;
	int write_addr;
	int ret = 0;

	WHC_XFER_DBG("Sub-image addr=0x%08X size=0x%08X\n", sub_image_start_addr, sub_image_size);
	WHC_XFER_DBG("Sub-image header write\n");

	write_addr = sub_image_start_addr;
	*offset -= WHC_XFER_IMAGE_HEADER_SIZE;

	remain_sub_image_size = sub_image_size;
	while (remain_sub_image_size > 0) {
		read_size = (remain_sub_image_size < adapter->read_buf_size) ? remain_sub_image_size : adapter->read_buf_size;
		ret = whc_xfer_read_file(fp, adapter->read_buf, read_size, offset);
		if (ret >= read_size) {
			write_size = ret;
			ret = ops->write(adapter, adapter->read_buf, write_addr, write_size, need_padding);
			if (ret == 0) {
				WHC_XFER_DBG("Sub-image write done\n");
				write_addr += write_size;
				remain_sub_image_size -= write_size;
			} else {
				WHC_XFER_LOG("Fail to download sub image (%d)\n", ret);
				break;
			}
		} else {
			WHC_XFER_LOG("Fail to read image (%d)\n", ret);
			ret = -1;
			break;
		}
	}

	return ret;
}

/*
 * @brief	Download manifest section (helper function)
 * @param	adapter: Adapter handler
 * @param	fp: File pointer
 * @param	manifest_target_addr: Target address for manifest
 * @param	image_file_size: Total image file size
 * @retval	Result: 0 on success, <0 on error
 */
static int whc_xfer_download_manifest(struct whc_xfer_adapter_t *adapter,
									  struct file *fp, u32 manifest_target_addr, int image_file_size)
{
	struct whc_xfer_ops_t *ops = adapter->ops;
	loff_t offset;
	int ret;

	if (adapter->manifest_pos == WHC_XFER_MANIFEST_POS_HEAD) {
		offset = 0;
	} else {
		offset = image_file_size - WHC_XFER_MANIFEST_SIZE;
	}

	WHC_XFER_DBG("Manifest addr=0x%08X\n", manifest_target_addr);

	ret = whc_xfer_read_file(fp, adapter->read_buf, WHC_XFER_MANIFEST_SIZE, &offset);
	if (ret < WHC_XFER_MANIFEST_SIZE) {
		WHC_XFER_LOG("Fail to read manifest (%d)\n", ret);
		return -1;
	}

	ret = ops->write(adapter, adapter->read_buf, manifest_target_addr, WHC_XFER_MANIFEST_SIZE, false);
	if (ret == 0) {
		WHC_XFER_DBG("Manifest download done\n");
		return 0;
	} else {
		WHC_XFER_LOG("Fail to download manifest (%d)\n", ret);
		return -1;
	}
}

int whc_xfer_download_image(struct whc_xfer_adapter_t *adapter, int image_type)
{
	struct whc_image_t *image;
	struct whc_mem_region_t *region;
	struct whc_image_header_t *header;
	struct file *fp;
	loff_t offset = 0;
	char *image_path;
	u8 *hash;
	int image_file_size;
	int remain_image_size;
	int region_index = 0;
	int ret = -1;
	u32 start_time;
	u32 sub_image_start_addr;
	u32 manifest_target_addr = 0;
	u32 sub_image_size;
	bool need_padding = false;
	bool skip_download = false;
	struct whc_xfer_ops_t *ops = adapter->ops;

	if (image_type == WHC_IMAGE_TYPE_BOOTLOADER) {
		/* Do pre-check only at rom phase */
		ret = whc_xfer_check_images(adapter);
		if (ret < 0) {
			WHC_XFER_LOG("Invalid images (%d)\n", ret);
			return ret;
		}
	} else if (image_type == WHC_IMAGE_TYPE_APPLICATION) {
		/* Do query only at bootloader */
		ret = whc_xfer_query(adapter);
		if (ret == 0) {
			/* Do post-check only at bootloader */
			ret = whc_xfer_check_device(adapter, image_type);
			if (ret < 0) {
				WHC_XFER_LOG("Device validate error (%d)\n", ret);
				return ret;
			}
		} else {
			WHC_XFER_LOG("Fail to query device info (%d)\n", ret);
			return ret;
		}
	} else {
		WHC_XFER_LOG("Invalid image type (%d)\n", image_type);
		return -1;
	}

	start_time = whc_xfer_get_time();

	hash = (u8 *)whc_xfer_zmalloc(adapter->hash_size);
	if (hash == NULL) {
		WHC_XFER_LOG("Fail to malloc hash buffer\n");
		goto exit;
	}

	image = &adapter->images[image_type];
	image_path = whc_xfer_join_path(adapter->image_dir, image->image_name);

	WHC_XFER_DBG("Start to download image: %s\n", image_path);

	fp = whc_xfer_open_file(image_path);
	if (fp == NULL) {
		ret = -1;
		WHC_XFER_LOG("Fail to open image file\n");
		goto exit_free_path;
	}

	ret = whc_xfer_get_file_size(fp);
	if (ret <= 0) {
		ret = -1;
		WHC_XFER_LOG("Fail to read image size (%d)\n", ret);
		goto exit_close_file;
	}

	image_file_size = ret;
	ret = 0;

	WHC_XFER_DBG("Image size: %d\n", image_file_size);

	if (adapter->manifest_pos == WHC_XFER_MANIFEST_POS_HEAD) {
		offset = WHC_XFER_MANIFEST_SIZE;
	}

	remain_image_size = image_file_size - WHC_XFER_MANIFEST_SIZE;

	while (remain_image_size >= WHC_XFER_IMAGE_HEADER_SIZE) {
		ret = whc_xfer_read_file(fp, adapter->read_buf, WHC_XFER_IMAGE_HEADER_SIZE, &offset);
		if (ret < WHC_XFER_IMAGE_HEADER_SIZE) {
			WHC_XFER_LOG("Fail to read image (%d)\n", ret);
			ret = -1;
			break;
		}
		ret = 0;
		header = (struct whc_image_header_t *)adapter->read_buf;
		if ((header->signature.d32[0] == image->signature[0]) && (header->signature.d32[1] == image->signature[1])) {
			sub_image_size = header->image_size + WHC_XFER_IMAGE_HEADER_SIZE;
			sub_image_start_addr = header->image_addr - WHC_XFER_IMAGE_HEADER_SIZE;
			if (header->image_size == 0) {
				WHC_XFER_DBG("empty-image (0x%08X) Send\n", sub_image_start_addr);

				ret = ops->write(adapter, adapter->read_buf, 0xFFFFFFFF, WHC_XFER_IMAGE_HEADER_SIZE, false);
				if (ret == 0) {
					WHC_XFER_DBG("empty-image header write done\n");
				} else {
					WHC_XFER_DBG("Fail to write empty-image header\n");
					break;
				}

				remain_image_size -= sub_image_size;
				continue;
			}
			if (header->image_size == 0xFFFFFFFF) {
				break;
			}

			region_index = whc_xfer_get_region_index(adapter, sub_image_start_addr);
			if (region_index < 0) {
				WHC_XFER_LOG("Invalid image address (0x%08X)\n", sub_image_start_addr);
				ret = -1;
				break;
			}

			region = &adapter->regions[region_index];
			need_padding = false;
			skip_download = false;

			if (region->mem_type == WHC_MEM_TYPE_FLASH) {
				WHC_XFER_DBG("Sub-image type: Flash\n");
				need_padding = true;
				sub_image_start_addr = image->flash_target_addr;
				ret = whc_xfer_calculate_hash(adapter, sub_image_start_addr, sub_image_size, hash);
				if (ret < 0) {
					WHC_XFER_LOG("Fail to get flash hash (%d)\n", ret);
					break;
				}

				/* Check if flash content matches file content */
				ret = whc_xfer_check_hash(adapter, fp, &offset, sub_image_size, hash);
				if (ret == 0) {
					/* Hash match, skip this sub-image download */
					WHC_XFER_LOG("Flash hash match, skip download (0x%08X)\n", sub_image_start_addr);
					skip_download = true;
					remain_image_size -= sub_image_size;
					continue;
				}
				WHC_XFER_LOG("Flash hash mismatch, download sub image (0x%08X)\n", sub_image_start_addr);

				/* sub_image_size will be aligned to 4KB at device side */
				ret = whc_xfer_erase(adapter, sub_image_start_addr, sub_image_size);
				if (ret < 0) {
					WHC_XFER_LOG("Fail to erase flash (%d)\n", ret);
					break;
				}
			} else if (region->mem_type == WHC_MEM_TYPE_SRAM) {
				WHC_XFER_DBG("Sub-image type: SRAM\n");
				if ((image_type == WHC_IMAGE_TYPE_BOOTLOADER) && (image->manifest_target_addr == 0U)) {
					/* Only Floader and fullmac loader put manifest after sram.bin */
					manifest_target_addr = sub_image_start_addr + sub_image_size;
				}
			} else if (region->mem_type == WHC_MEM_TYPE_PSRAM) {
				WHC_XFER_DBG("Sub-image type: PSRAM\n");
			} else {
				WHC_XFER_DBG("Sub-image type: TCM\n");
			}

			/* Download sub-image data if not skipped */
			if (!skip_download) {
				ret = whc_xfer_download_sub_image_data(adapter, fp, image, sub_image_start_addr,
													   sub_image_size, need_padding, &offset);
				if (ret != 0) {
					break;
				}
			}

			remain_image_size -= sub_image_size;
		} else {
			WHC_XFER_LOG("Invalid image: bad signature\n");
			ret = -1;
			break;
		}
	}

	if (ret != 0) {
		goto exit_close_file;
	}

	if (image->manifest_target_addr != 0U) {
		manifest_target_addr = image->manifest_target_addr;
	}

	if (manifest_target_addr != 0) {
		ret = whc_xfer_download_manifest(adapter, fp, manifest_target_addr, image_file_size);
		if (ret != 0) {
			goto exit_close_file;
		}
	}

	WHC_XFER_LOG("Image download success, %u bytes costs %dms\n", image_file_size, whc_xfer_get_elapsed_ms(start_time));

	if (image->post_process == WHC_POST_PROCESS_BOOT) {
		ret = ops->boot(adapter);
		if (ret == 0) {
			WHC_XFER_LOG("Boot from firmware...\n");
		} else {
			WHC_XFER_LOG("Fail to boot from firmware (%d)\n", ret);
		}
	}

exit_close_file:
	whc_xfer_close_file(fp);

exit_free_path:
	whc_xfer_mfree(image_path);

exit:
	whc_xfer_mfree(hash);

	return ret;
}
