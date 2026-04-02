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
#include "ameba_inic.h"
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/jiffies.h>
#include <linux/namei.h>

/* Legacy exports for compatibility - these now get data from HAL */
const struct whc_mem_region_t *whc_mem_regions = NULL;
const struct whc_image_t *whc_images = NULL;

/**
 * @brief Initialize HAL data
 * Called once during module init to populate the legacy exports
 */
void whc_hal_init(void)
{
	const struct whc_hal_config_t *config = whc_hal_get_config();

	whc_mem_regions = config->mem_regions;
	whc_images = config->images;
}

/**
 * @brief Allocate and initialize XFER adapter
 * @param interface: Interface private data (SDIO or USB)
 * @param ops: XFER operations
 * @return Pointer to adapter on success, NULL on failure
 */
struct whc_xfer_adapter_t *whc_xfer_adapter_alloc(void *interface, const struct whc_xfer_ops_t *ops)
{
	struct whc_xfer_adapter_t *adapter;
	const struct whc_hal_config_t *hal_config;
	u8 *buf;

	/* Initialize HAL to populate legacy exports */
	whc_hal_init();
	hal_config = whc_hal_get_config();

	adapter = (struct whc_xfer_adapter_t *)kzalloc(sizeof(struct whc_xfer_adapter_t), GFP_KERNEL);
	if (adapter == NULL) {
		return NULL;
	}

	/* Initialize from HAL configuration */
#ifdef CONFIG_WHC_HCI_SDIO
	adapter->hash_size = hal_config->hash_size + sizeof(INIC_RX_DESC);
	adapter->xfer_page_size = hal_config->xfer_page_size + sizeof(INIC_TX_DESC);
#else
	adapter->hash_size = hal_config->hash_size;
	adapter->xfer_page_size = hal_config->xfer_page_size;
#endif

	adapter->hash_size = hal_config->hash_size;
	adapter->xfer_page_size = hal_config->xfer_page_size;
	adapter->read_buf_size = hal_config->read_buf_size;
	adapter->manifest_pos = hal_config->manifest_pos;
	adapter->chip_id = hal_config->chip_id;
	adapter->min_protocol_version = hal_config->min_protocol_version;
	adapter->max_protocol_version = hal_config->max_protocol_version;
	adapter->regions = (struct whc_mem_region_t *)hal_config->mem_regions;
	adapter->images = (struct whc_image_t *)hal_config->images;
	adapter->image_dir = (char *)hal_config->image_dir;

	/* Allocate TX buffer (DESC + page_size for firmware write data) */
	buf = (u8 *)kzalloc(WHC_XFER_DESC_SIZE + adapter->xfer_page_size + WHC_XFER_DMA_ALIGNMENT, GFP_KERNEL);
	if (buf == NULL) {
		goto exit_tx_buf_malloc;
	}

	adapter->tx_raw_buf = buf;
	adapter->tx_buf = (u8 *)(((unsigned long)buf + WHC_XFER_DMA_ALIGNMENT - 1) & ~(WHC_XFER_DMA_ALIGNMENT - 1));

	/* Allocate RX buffer (DESC + hash_size for hash/device_info response) */
	buf = (u8 *)kzalloc(WHC_XFER_DESC_SIZE + adapter->hash_size + WHC_XFER_DMA_ALIGNMENT, GFP_KERNEL);
	if (buf == NULL) {
		goto exit_rx_buf_malloc;
	}

	adapter->rx_raw_buf = buf;
	adapter->rx_buf = (u8 *)(((unsigned long)buf + WHC_XFER_DMA_ALIGNMENT - 1) & ~(WHC_XFER_DMA_ALIGNMENT - 1));

	/* Allocate read buffer */
	buf = (u8 *)kzalloc(adapter->read_buf_size, GFP_KERNEL);
	if (buf == NULL) {
		goto exit_read_buf_malloc;
	}

	adapter->read_buf = buf;
	adapter->interface = interface;
	adapter->ops = (struct whc_xfer_ops_t *)ops;

	return adapter;

exit_read_buf_malloc:
	kfree(adapter->rx_raw_buf);
	adapter->rx_raw_buf = NULL;

exit_rx_buf_malloc:
	kfree(adapter->tx_raw_buf);
	adapter->tx_raw_buf = NULL;

exit_tx_buf_malloc:
	kfree(adapter);

	return NULL;
}

/**
 * @brief Free XFER adapter
 * @param adapter: XFER adapter to free
 */
void whc_xfer_adapter_free(struct whc_xfer_adapter_t *adapter)
{
	if (adapter == NULL) {
		return;
	}

	if (adapter->ops && adapter->ops->deinit) {
		adapter->ops->deinit(adapter);
	}

	kfree(adapter->read_buf);
	kfree(adapter->rx_raw_buf);
	kfree(adapter->tx_raw_buf);
	kfree(adapter);
}

/* OS abstraction layer implementations */
void *whc_xfer_zmalloc(size_t len)
{
	void *buf = NULL;
	buf = kzalloc(len, in_interrupt() ? GFP_ATOMIC : GFP_KERNEL);
	return buf;
}

void whc_xfer_mfree(void *buf)
{
	if (buf != NULL) {
		kfree(buf);
	}
}

void whc_xfer_memset(void *buf, int c, size_t len)
{
	memset(buf, c, len);
}

void whc_xfer_memcpy(void *dst, void *src, size_t len)
{
	memcpy(dst, src, len);
}

u32 whc_xfer_get_time(void)
{
	return jiffies;
}

u32 whc_xfer_get_elapsed_ms(u32 start)
{
	u32 current_time = whc_xfer_get_time();
	u32 elapsed;

	if (current_time >= start) {
		elapsed = current_time - start;
	} else {
		elapsed = 0xFFFFFFFF - start + current_time;
	}
	return elapsed * 1000 / HZ;
}

void whc_xfer_msleep(unsigned long ms)
{
	msleep_interruptible(ms);
}

char *whc_xfer_join_path(const char *a, const char *b)
{
	size_t len_a, len_b;
	char *result;

	if (!a || !b) {
		return NULL;
	}

	len_a = strlen(a);
	len_b = strlen(b);

	/* +2 for possible '/' and null terminator */
	result = kmalloc(len_a + len_b + 2, GFP_KERNEL);
	if (!result) {
		return NULL;
	}

	strcpy(result, a);
	if (len_a && result[len_a - 1] != '/' && b[0] != '/') {
		result[len_a] = '/';
		strcpy(result + len_a + 1, b);
	} else {
		strcpy(result + len_a, b);
	}

	return result;
}

int whc_xfer_check_file_exists(const char *path)
{
	struct path fpath;
	int error;

	error = kern_path(path, LOOKUP_FOLLOW, &fpath);
	if (error) {
		if (error == -ENOENT) {
			return 0;  /* File does not exist */
		}
		return error;  /* Some other error occurred */
	}

	/* File exists */
	path_put(&fpath);
	return 1;
}

struct file *whc_xfer_open_file(const char *path)
{
	struct file *fp;

	fp = filp_open(path, O_RDONLY, 0);
	if (IS_ERR(fp)) {
		return NULL;
	}

	return fp;
}

void whc_xfer_close_file(struct file *file)
{
	if (file == NULL) {
		pr_err("[WHC] Invalid file handle\n");
		return;
	}

	filp_close(file, NULL);
}

int whc_xfer_get_file_size(struct file *file)
{
	int ret = -1;
	struct inode *inode;

	if (file == NULL) {
		pr_err("[WHC] Invalid file handle\n");
		return -1;
	}

	inode = file_inode(file);
	if (inode != NULL) {
		ret = i_size_read(inode);
	}

	return ret;
}

int whc_xfer_read_file(struct file *file, u8 *buf, int size, loff_t *offset)
{
	ssize_t bytes_read;

	if (file == NULL) {
		pr_err("[WHC] Invalid file handle\n");
		return -1;
	}

	bytes_read = kernel_read(file, buf, size, offset);

	return (int)bytes_read;
}
