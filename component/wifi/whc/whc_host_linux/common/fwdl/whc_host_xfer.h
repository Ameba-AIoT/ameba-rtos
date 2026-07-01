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
#ifndef __WHC_HOST_XFER_H__
#define __WHC_HOST_XFER_H__

#include <linux/types.h>
#include <linux/stddef.h>
#include <linux/wait.h>
#include <linux/mutex.h>

/* XFER protocol commands */
#define WHC_XFER_REQ_QUERY				0x01
#define WHC_XFER_REQ_WRITE				0x02
#define WHC_XFER_REQ_CHECK				0x03
#define WHC_XFER_REQ_BOOT				0x04
#define WHC_XFER_REQ_ABORT				0x05
#define WHC_XFER_REQ_ERASE				0x06
#define WHC_XFER_REQ_HASH				0x07

#define WHC_XFER_ACK_QUERY				0x81
#define WHC_XFER_ACK_WRITE				0x82
#define WHC_XFER_ACK_CHECK				0x83
#define WHC_XFER_ACK_BOOT				0x84
#define WHC_XFER_ACK_ABORT				0x85
#define WHC_XFER_ACK_ERASE				0x86
#define WHC_XFER_ACK_HASH				0x87

#define WHC_XFER_STAUS_OK				0x00

/* Firmware types */
#define WHC_XFER_FW_TYPE_ROM					0xF0
#define WHC_XFER_FW_TYPE_BOOTLOADER			0xF1
#define WHC_XFER_FW_TYPE_APPLICATION			0xF2

#define WHC_XFER_CFG_WRITE_CHK_MASK			0x01

#define WHC_XFER_MANIFEST_SIZE				4096

#define WHC_XFER_MANIFEST_POS_HEAD			0
#define WHC_XFER_MANIFEST_POS_TAIL			1

#define WHC_XFER_FLASH_PAGE_SIZE				1024

#define WHC_XFER_DEFAULT_TIMEOUT				5
#define WHC_XFER_QUERY_TIMEOUT				5

/* Memory types (must match fullmac) */
#define WHC_XFER_MEM_TYPE_TCM_VAL			0
#define WHC_XFER_MEM_TYPE_FLASH_VAL			1
#define WHC_XFER_MEM_TYPE_SRAM_VAL			2
#define WHC_XFER_MEM_TYPE_PSRAM_VAL			3

/* Max 4KB sector erase time: typical 70ms for GD25Q256D, max 400ms for GD25Q256D */
#define WHC_XFER_FLASH_ERASE_4KB_TIMEOUT		600

/* Max page program time: typical 0.5ms for GD25Q128E, max 3ms for W25Q256JV */
/* Take 0.5ms * 10 = 5ms */
#define WHC_XFER_PAGE_WRITE_TIMEOUT			5

/* Read flash with 1IO@10MHz(10Mbps), ~0.8ms/KB */
/* Take 2ms/KB */
#define WHC_XFER_FLASH_HASH_1KB_TIMEOUT		2
#define WHC_XFER_FLASH_CHECKSUM_1KB_TIMEOUT	2

#define CONFIG_WHC_WRITE_CHK_EN				1
#define CONFIG_WHC_DOWNLOAD_CHK_EN			1

struct whc_xfer_desc_t {
	/* DWORD 0 */
	u32	data_len: 16;		/* Data length */
	u32	data_offset: 8;		/* Data offset i.e. header length */
	u32	config: 8;			/* Configuration */

	/* DWORD 1 */
	u32	type: 8;			/* Packet type */
	u32	status: 8;		/* Status */
	u32	version: 8;		/* Version */
	u32	mode: 8;			/* Device mode */

	/* DWORD 2 */
	u32	addr;			/* Memory address */

	/* DWORD 3 */
	u32	size;			/* Memory size */

	/* DWORD 4 */
	union {
		u32	d32;
		u16	d16[2];
		u8	d8[4];
	} value;				/* Target value */

	/* DWORD 5 */
	u32	rsvd5;
};

struct whc_dev_info_t {
	/* DWORD 0 */
	u32 chip_id: 16;
	u32 cut_version: 8;

	/* DWORD 1 */
	u32 protocol_version: 16;
	u32 sram_size: 16; /* Size in KB */

	/* DWORD 2 */
	u32 psram_size: 16; /* Size in KB */
	u32 flash_size: 16; /* Size in KB */

	/* DWORD 3 */
	u32 tcm_size: 16; /* Size in KB */
	u32 reserved3: 16; /* Size in KB */

	/* DWORD 4-7 */
	u32 reserved4_7[4];
};

struct whc_xfer_ops_t;
struct whc_sdio;

struct whc_xfer_adapter_t {
	struct whc_mem_region_t *regions;
	struct whc_image_t *images;
	struct whc_xfer_ops_t *ops;
	struct whc_dev_info_t device_info;
	void *interface;
	char *image_dir;
	u8 *tx_raw_buf;
	u8 *tx_buf;
	u8 *rx_raw_buf;
	u8 *rx_buf;
	u8 *read_buf;
	int read_buf_size;
	int hash_size;
	int xfer_page_size;
	int manifest_pos;
	u16 chip_id;
	u16 min_protocol_version;
	u16 max_protocol_version;

#ifdef CONFIG_WHC_HCI_SDIO
	/* SDIO XFER specific fields for interrupt-driven RX */
	wait_queue_head_t sdio_rx_wq;
	u8 sdio_rx_ready;
#endif
#ifdef CONFIG_WHC_HCI_USB
	/* USB XFER specific fields for control transfers */
	struct mutex ctrl_mutex;
	u8 *ctrl_in_buf;
#endif
};

struct whc_xfer_ops_t {
	int(*init)(struct whc_xfer_adapter_t *adapter, void *params);
	void(*deinit)(struct whc_xfer_adapter_t *adapter);
	int(*check_firmware)(struct whc_xfer_adapter_t *adapter);
	int(*reset_device)(struct whc_xfer_adapter_t *adapter);
	int(*handshake)(struct whc_xfer_adapter_t *adapter, u8 *request, int request_size, u8 *response, int response_size, int timeout);
	int (*boot)(struct whc_xfer_adapter_t *adapter);
	int (*write)(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding);
};

enum whc_image_type_t {
	WHC_IMAGE_TYPE_BOOTLOADER = 0,
	WHC_IMAGE_TYPE_APPLICATION
};

enum whc_mem_type_t {
	WHC_MEM_TYPE_TCM = 0,
	WHC_MEM_TYPE_FLASH,
	WHC_MEM_TYPE_SRAM,
	WHC_MEM_TYPE_PSRAM
};

enum whc_post_process_t {
	WHC_POST_PROCESS_NONE = 0,
	WHC_POST_PROCESS_BOOT
};

struct whc_image_header_t {
	union {
		u32 d32[2];
		u8 d8[8];
	} signature;
	u32 image_size;
	u32 image_addr;
	u32 boot_index;
	u32 reserved[3];
};

struct whc_mem_region_t {
	u32 start_addr; /* Included */
	u32 end_addr; /* NOT included */
	enum whc_mem_type_t mem_type;
};

#define WHC_DEFINE_MEM_REGION(_start_addr, _end_addr, _mem_type) \
	.start_addr = _start_addr, \
	.end_addr = _end_addr, \
	.mem_type = _mem_type

struct whc_image_t {
	char *image_name;
	u32 signature[2];
	u32 flash_target_addr;
	u32 manifest_target_addr;
	enum whc_image_type_t image_type;
	enum whc_post_process_t post_process;
};

#define WHC_DEFINE_IMAGE(_image_name, _signature0, _signature1, _flash_target_addr, _manifest_target_addr, _image_type, _post_process) \
	.image_name = _image_name, \
	.signature = {_signature0, _signature1}, \
	.flash_target_addr = _flash_target_addr, \
	.manifest_target_addr = _manifest_target_addr, \
	.image_type = _image_type, \
	.post_process = _post_process

#define WHC_XFER_DESC_SIZE		(sizeof(struct whc_xfer_desc_t))
#define WHC_XFER_IMAGE_HEADER_SIZE	(sizeof(struct whc_image_header_t))
#define WHC_XFER_DEVICE_INFO_SIZE	(sizeof(struct whc_dev_info_t))

/* Additional constants for SDIO XFER */
#define WHC_XFER_DMA_ALIGNMENT				64

/* Include HAL for IC-specific configuration */
#include "whc_hal.h"

/* Function declarations */
void whc_hal_init(void);
struct whc_xfer_adapter_t *whc_xfer_adapter_alloc(void *interface, const struct whc_xfer_ops_t *ops);
void whc_xfer_adapter_free(struct whc_xfer_adapter_t *adapter);
int whc_xfer_download_image(struct whc_xfer_adapter_t *adapter, int image_type);
int whc_xfer_boot(struct whc_xfer_adapter_t *adapter);
int whc_xfer_write(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding);

/* OS abstraction layer */
char *whc_xfer_join_path(const char *dir, const char *filename);
int whc_xfer_check_file_exists(const char *path);
struct file *whc_xfer_open_file(const char *path);
void whc_xfer_close_file(struct file *file);
int whc_xfer_get_file_size(struct file *file);
int whc_xfer_read_file(struct file *file, u8 *buf, int size, loff_t *offset);
void *whc_xfer_zmalloc(size_t len);
void whc_xfer_mfree(void *buf);
void whc_xfer_memset(void *buf, int c, size_t len);
void whc_xfer_memcpy(void *dst, void *src, size_t len);
u32 whc_xfer_get_time(void);
u32 whc_xfer_get_elapsed_ms(u32 start);
void whc_xfer_msleep(unsigned long ms);

#ifdef CONFIG_WHC_HCI_SDIO
int whc_sdio_rom_write(struct whc_xfer_adapter_t *adapter, u8 *buf, u32 addr, u32 size, bool need_padding);
int whc_sdio_rom_boot(struct whc_xfer_adapter_t *adapter);
bool whc_sdio_check_dl_mode(struct whc_sdio *priv);
#endif

/* Memory regions and images configuration (initialized by HAL) */
extern const struct whc_mem_region_t *whc_mem_regions;
extern const struct whc_image_t *whc_images;

#endif /* __WHC_HOST_XFER_H__ */
