/**
 * @file      rtk_bt_device_firmware_update_amebalite.c
 * @brief     Bluetooth device firmware update relate api function definition for amebalite
 * @copyright Copyright (c) 2024. Realtek Semiconductor Corporation. All rights reserved.
 */

#include <bt_api_config.h>

#if defined(RTK_BLE_MESH_SUPPORT) && RTK_BLE_MESH_SUPPORT
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

#include <ameba.h>
#include <ameba_ota.h>
#include <ameba_boot.h>
#include <ameba_flashcfg.h>
#include <flash_api.h>
#include <sys_api.h>

#include <rtk_bt_device_firmware_update_interface.h>
#include <rtk_bt_mesh_def.h>

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE || \
    defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE || \
    defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE || \
    defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
// Get the target start and end addr on flash for save new firmware
static bool mesh_dfu_get_target_addr_by_img_id(uint8_t img_id, uint32_t *target_start, uint32_t *target_end)
{
	uint8_t idx;

	idx = ota_get_cur_index(img_id);

	if (OTA_IMGID_BOOT == img_id) {
		// Get target image boot start and end addr
		if (OTA_INDEX_1 == idx) {
			flash_get_layout_info(IMG_BOOT_OTA2, target_start, target_end);
		} else {
			flash_get_layout_info(IMG_BOOT, target_start, target_end);
		}
		return true;
	} else if (OTA_IMGID_APP == img_id) {
		// Get target image app start and end addr
		if (OTA_INDEX_1 == idx) {
			flash_get_layout_info(IMG_APP_OTA2, target_start, target_end);
		} else {
			flash_get_layout_info(IMG_APP_OTA1, target_start, target_end);
		}
		return true;
	} else {
		BT_LOGE("[%s] Unknown image id:%d.\r\n", __func__, img_id);
		return false;
	}
}

// Read data from flash
static void fw_update_server_read_flash(uint32_t addr, uint8_t *p, uint32_t len)
{
	flash_t flash;
	flash_stream_read(&flash, addr - SPI_FLASH_BASE, len, p);

#if !(defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE) && 0
	{
		static uint8_t print_times = 0;
		if (2 > print_times) {
			print_times++;
			for (uint32_t i = 0; i < len; i++) {
				if (i % 16 == 0) {
					BT_LOGA("\r\n");
				}
				BT_LOGA(" 0x%02x ", p[i]);
			}
			BT_LOGA("\r\n\r\n");
		}
	}
#endif
}
#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE or BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE or BT_MESH_ENABLE_DFU_TARGET_ROLE

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE || \
    defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE || \
    defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
static uint32_t start_addr_save_new_firmware = 0, end_addr_save_new_firmware = 0;

static bool initiator_or_standalone_updater_or_distributor_resource_init(void)
{
	uint32_t addr = 0;

	start_addr_save_new_firmware = 0;
	if (!mesh_dfu_get_target_addr_by_img_id(OTA_IMGID_APP, &addr, &end_addr_save_new_firmware)) {
		BT_LOGE("[%s] Get image addr fail.\r\n", __func__);
		return false;
	}

	// On OTA area for save new image, skip the Certificate(4K) + Manifest(4K) for save the new firmware
	start_addr_save_new_firmware = addr + 0x2000;

	BT_LOGD("[%s] Set flash addr from 0x%08x to 0x%08x for save new firmware image.\r\n", __func__, (unsigned int)start_addr_save_new_firmware,
			(unsigned int)end_addr_save_new_firmware);
	return true;
}

// whether signuture field in image is "OTA" ?
static bool new_firmware_image_have_right_signature(uint8_t *p_ota)
{
	uint8_t *p = p_ota;
	update_file_hdr *hdr = (update_file_hdr *)p_ota;

	if (1 == hdr->HdrNum) {
		p += sizeof(update_file_hdr);
		if (memcmp(p, "OTA", 3)) {
			BT_LOGE("[%s] Signature in file header is not OTA, fail. Actual is %c%c%c\r\n", __func__, p[0], p[1], p[2]);
			return false;
		} else {
			return true;
		}
	} else {
		BT_LOGE("[%s] There is more than one image(num:%d) in ota.bin, do not cover this situation temp, fail.\r\n", __func__, hdr->HdrNum);
		return false;
	}
}

static bool initiator_or_standalone_updater_or_distributor_load_data_from_flash(uint32_t addr_offset, uint32_t len, uint8_t *pout)
{
	uint32_t addr = 0;
	bool ret = true;
	static bool get_valid_new_image = true;

	if (!start_addr_save_new_firmware) {
		BT_LOGE("[%s] The start of addr save firmware is not valid.\r\n", __func__);
		return false;
	}

	if (!get_valid_new_image) {
		BT_LOGE("[%s][%d] Get valid new image from flash fail.\r\n", __func__, __LINE__);
		return false;
	}

	addr = start_addr_save_new_firmware + addr_offset;

	fw_update_server_read_flash(addr, pout, len);
	BT_LOGD("[%s] Read len %d data from flash addr 0x%08x.\r\n", __func__, len, addr);

	if (0 == addr_offset) {
		// Read the first packet for new firmware, make sure signature field is "OTA"
		if (!new_firmware_image_have_right_signature(pout)) {
			BT_LOGE("[%s] The signature field in ota.bin is not OTA, failed.\r\n", __func__);
			ret = false;
		}
	}

	if (!ret) {
		BT_LOGE("[%s][%d] Get valid new image from flash fail.\r\n", __func__, __LINE__);
		get_valid_new_image = false;
	}

	return ret;
}

#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE or BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE or BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE || \
    defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

// Erase flash from start_addr to end_addr
static void distributor_or_target_erase_flash(uint32_t start_addr, uint32_t end_addr)
{
	const uint32_t sector_size = 4 * 1024;
	const uint32_t block_size = 64 * 1024;
	uint32_t align_addr, offset;
	flash_t flash;
	uint8_t *p = NULL;

	if (start_addr >= end_addr) {
		BT_LOGE("[%s][%d] Erase flash fail, start(0x%x) >= end(0x%x).\r\n", __func__, __LINE__, start_addr, end_addr);
		return ;
	}

	BT_LOGD("[%s] Start erase flash from 0x%08x to 0x%08x.\r\n", __func__, start_addr, end_addr);

	p = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sector_size);
	if (!p) {
		BT_LOGE("[%s][%d] Malloc fail.\r\n", __func__, __LINE__);
		// return ;
	}

	if (start_addr % sector_size) {
		BT_LOGD("[%s][%d] Start addr 0x%08x is not 4K align.\r\n", __func__, __LINE__, start_addr);

		offset = start_addr % sector_size;
		align_addr = start_addr - offset;
		memset(p, 0xff, sector_size);

		flash_stream_read(&flash, align_addr - SPI_FLASH_BASE, offset, p);

		flash_erase_sector(&flash, align_addr - SPI_FLASH_BASE);

		flash_stream_write(&flash, align_addr - SPI_FLASH_BASE, offset, p);

		start_addr = align_addr + sector_size;
	}

	if (end_addr % sector_size) {
		BT_LOGD("[%s][%d] End addr 0x%08x is not 4K align.\r\n", __func__, __LINE__, end_addr);

		offset = end_addr % sector_size;
		align_addr = end_addr - offset;
		memset(p, 0xff, sector_size);

		flash_stream_read(&flash, end_addr - SPI_FLASH_BASE, (sector_size - offset), p);

		flash_erase_sector(&flash, align_addr - SPI_FLASH_BASE);

		flash_stream_write(&flash, end_addr - SPI_FLASH_BASE, (sector_size - offset), p);

		end_addr = align_addr;
	}

	if (p) {
		osif_mem_free(p);
	}

	do {
		// For flash_erase_block API, the input address should align with 64K bytes
		if (end_addr - start_addr >= block_size && !(start_addr % block_size)) {
			flash_erase_block(&flash, start_addr - SPI_FLASH_BASE);
			start_addr += block_size;
		} else {
			flash_erase_sector(&flash, start_addr - SPI_FLASH_BASE);
			start_addr += sector_size;
		}
	} while (start_addr < end_addr);
}

// Write data to flash
static void distributor_or_target_write_data_to_flash(uint32_t addr, uint8_t *p, uint32_t len)
{
	flash_t flash;
	flash_stream_write(&flash, addr - SPI_FLASH_BASE, len, p);

	BT_LOGD("[%s] Wrtie len %d data to flash start addr:0x%08x.\r\n", __func__, len, addr);
#if 0
	{
		static uint8_t print_times = 0;
		if (2 > print_times) {
			print_times++;
			for (uint32_t i = 0; i < len; i++) {
				if (i % 16 == 0) {
					BT_LOGA("\r\n");
				}
				BT_LOGA(" 0x%02x ", p[i]);
			}
			BT_LOGA("\r\n\r\n");
		}
	}
#endif
}
#endif  // BT_MESH_ENABLE_DFU_TARGET_ROLE or BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_INITIATOR_ROLE) && BT_MESH_ENABLE_DFU_INITIATOR_ROLE
bool mesh_dfu_initiator_resource_init(struct mesh_dfu_initiator_opts *p_if)
{
	if (!initiator_or_standalone_updater_or_distributor_resource_init()) {
		BT_LOGE("[%s] Resource init fail.\r\n", __func__);
		return false;
	}
	p_if->mesh_dfu_initiator_load_data_flash = initiator_or_standalone_updater_or_distributor_load_data_from_flash;
	return true;
}
#endif  // BT_MESH_ENABLE_DFU_INITIATOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE
bool mesh_dfu_standalone_updater_resource_init(struct mesh_dfu_standalone_updater_opts *p_if)
{
	if (!initiator_or_standalone_updater_or_distributor_resource_init()) {
		BT_LOGE("[%s] Resource init fail.\r\n", __func__);
		return false;
	}
	p_if->mesh_dfu_standalone_updater_load_data_flash = initiator_or_standalone_updater_or_distributor_load_data_from_flash;
	return true;
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

#if defined(BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE) && BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE
static bool distributor_receive_block_data(uint16_t block_num, uint8_t *pdata, uint32_t data_len)
{
	static uint32_t offset = 0;

	if (!start_addr_save_new_firmware) {
		BT_LOGE("[%s] Addr for save new firmware is 0, fail.\r\n", __func__);
		return false;
	}

	if (0 == block_num) {
		BT_LOGD("[%s] Receive data from initiator, start erase flash from 0x%08x to 0x%08x for save new image.\r\n", __func__, start_addr_save_new_firmware,
				end_addr_save_new_firmware);
		distributor_or_target_erase_flash(start_addr_save_new_firmware, end_addr_save_new_firmware);
		offset = 0;
	}

	BT_LOGD("[%s][%d] address:0x%08x, len:%d.\r\n", __func__, __LINE__, start_addr_save_new_firmware + offset, data_len);

	distributor_or_target_write_data_to_flash(start_addr_save_new_firmware + offset, pdata, data_len);

	offset += data_len;

	return true;
}

bool mesh_dfu_distributor_resource_init(struct mesh_dfu_distributor_opts *p_if)
{
	if (!initiator_or_standalone_updater_or_distributor_resource_init()) {
		BT_LOGE("[%s] Resource init fail.\r\n", __func__);
		return false;
	}
	p_if->mesh_dfu_distributor_load_data_flash = initiator_or_standalone_updater_or_distributor_load_data_from_flash;
	p_if->mesh_dfu_distributor_block_data = distributor_receive_block_data;
	return true;
}
#endif  // BT_MESH_ENABLE_DFU_DISTRIBUTOR_ROLE

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE
#define IMAGE_INCLUDE_BOOT_AND_APP  0xff

static uint32_t target_addr_image_boot, target_addr_image_app;  // Start addr on flash for save new firmware
static bool check_sum_result = false;

// Get image header info from ota target header according image id
static update_file_img_hdr *fw_update_server_get_img_hdr(update_ota_target_hdr *hdr, uint8_t image_id)
{
	uint8_t i, header_n;

	if (!hdr) {
		return NULL;
	}

	header_n = hdr->FileHdr.HdrNum;

	if (!(1 == header_n || 2 == header_n)) {
		return NULL;
	}

	for (i = 0; i < header_n; i++) {
		if (image_id == hdr->FileImgHdr[i].ImgID) {
			return &hdr->FileImgHdr[i];
		}
	}

	return NULL;
}

// Get manifest pointer from update_ota_target_hdr according image id
static Manifest_TypeDef *fw_update_server_get_manifest_p(update_ota_target_hdr *hdr, uint8_t image_id)
{
	uint8_t i, header_n;

	if (!hdr) {
		BT_LOGE("[%s] Input param hdr is NULL.\r\n", __func__);
		return NULL;
	}

	header_n = hdr->FileHdr.HdrNum;

	if (!(1 == header_n || 2 == header_n)) {
		BT_LOGE("[%s] Wrong input header num:%d.\r\n", __func__, header_n);
		return NULL;
	}

	for (i = 0; i < header_n; i++) {
		if (image_id == hdr->FileImgHdr[i].ImgID) {
			return &hdr->Manifest[i];
		}
	}

	BT_LOGE("[%s] Can not get target image id:%d, header_n:%d.\r\n", __func__, image_id, header_n);

	return NULL;
}

// Get addr on flash for save new firmware
static bool fw_update_server_get_target_image_info(update_ota_target_hdr *hdr)
{
	uint32_t addr_end_max_boot, addr_end_max_app;  // the end of addr save firmware
	update_file_img_hdr *img_hdr = NULL;
	bool get_boot_image = false;
	bool get_app_image = false;

	BT_LOGD("[%s] Enter.\r\n", __func__);

	// Get target image boot start and end addr
	mesh_dfu_get_target_addr_by_img_id(OTA_IMGID_BOOT, &target_addr_image_boot, &addr_end_max_boot);

	// Get target image app start and end addr
	mesh_dfu_get_target_addr_by_img_id(OTA_IMGID_APP, &target_addr_image_app, &addr_end_max_app);

	// Check whether the image size on ota.bin > local max flash size
	img_hdr = fw_update_server_get_img_hdr(hdr, OTA_IMGID_BOOT);
	if (img_hdr) {
		if (img_hdr->ImgLen > addr_end_max_boot - target_addr_image_boot) {
			BT_LOGE("[%s] The ota boot image len > actual flash size, fail.\r\n", __func__);
			return false;
		} else {
			get_boot_image = true;
		}
	}

	// Check whether the image size on ota.bin > local max flash size
	img_hdr = fw_update_server_get_img_hdr(hdr, OTA_IMGID_APP);
	if (img_hdr) {
		if (img_hdr->ImgLen > addr_end_max_app - target_addr_image_app) {
			BT_LOGE("[%s] The ota app image len > actual flash size, fail.\r\n", __func__);
			return false;
		} else {
			get_app_image = true;
		}
	}

	if (get_boot_image || get_app_image) {
		return true;
	} else {
		BT_LOGE("[%s] There is NO boot image or app image in header, fail.\r\n", __func__);
		return false;
	}
}

// Get the image id according start and end addr on ota bin
static uint8_t fw_update_server_get_image_id_by_ota_addr(update_ota_target_hdr *hdr, uint32_t start, uint32_t end)
{
	if (!hdr) {
		BT_LOGE("[%s] Input param is NULL.\r\n", __func__);
		return 100;
	}

	BT_LOGD("[%s] start:0x%x, end:0x%x.\r\n", __func__, start, end);
	for (uint8_t i = 0; i < hdr->FileHdr.HdrNum; i++) {
		update_file_img_hdr *img_hdr = &hdr->FileImgHdr[i];
		if (start >= img_hdr->Offset + img_hdr->ImgLen || end <= img_hdr->Offset) {
			continue;
		}
		if (start >= img_hdr->Offset && end <= img_hdr->Offset + img_hdr->ImgLen) {
			BT_LOGD("[%s] Single image, img_hdr->Offset:0x%x, img_hdr->ImgLen:0x%x.\r\n", __func__, img_hdr->Offset, img_hdr->ImgLen);
			return img_hdr->ImgID;
		} else {
			BT_LOGD("[%s] Blong 2 images, img_hdr->Offset:0x%x, img_hdr->ImgLen:0x%x.\r\n", __func__, img_hdr->Offset, img_hdr->ImgLen);
			// Received the block data blong to 2 image
			return IMAGE_INCLUDE_BOOT_AND_APP;
		}
	}

	return 101;
}

// Save manifest to ram temp
static uint8_t *fw_update_server_save_manifest_to_ram(Manifest_TypeDef *manifest, uint8_t *p)
{
	uint32_t l = sizeof(Manifest_TypeDef);

	if (!manifest) {
		BT_LOGE("[%s] Input param is NULL.\r\n", __func__);
	}

	BT_LOGD("[%s] Save manifest temp.\r\n", __func__);
	memcpy(manifest, p, l);

	p += l;

	return p;
}

// Recover manifest from ram
static bool fw_update_server_recover_manifest(Manifest_TypeDef *manifest, uint32_t addr)
{
	if (manifest) {
		BT_LOGD("[%s] Recover manifest from ram.\r\n", __func__);
		distributor_or_target_write_data_to_flash(addr, (uint8_t *)manifest, sizeof(Manifest_TypeDef));
		return true;
	} else {
		BT_LOGE("[%s] Get manifest fail.\r\n", __func__);
		return false;
	}
}

// Check the sum num of image data
static bool fw_update_server_check_sum(update_ota_target_hdr *hdr, uint8_t img_id)
{
	Manifest_TypeDef *mani_p = NULL;
	update_file_img_hdr *img_hdr = NULL;
	uint32_t check_sum, start_addr, flash_read_step, addr_offset;
	uint8_t *p = NULL;

	if (OTA_IMGID_BOOT == img_id) {
		start_addr = target_addr_image_boot;
	} else if (OTA_IMGID_APP == img_id) {
		start_addr = target_addr_image_app;
	} else {
		BT_LOGE("[%s] Unknown image id:%d.\r\n", __func__, img_id);
		return false;
	}

	mani_p = fw_update_server_get_manifest_p(hdr, img_id);
	if (!mani_p) {
		BT_LOGE("[%s] Get manifest fail.", __func__);
		return false;
	}

	img_hdr = fw_update_server_get_img_hdr(hdr, img_id);
	if (!img_hdr) {
		BT_LOGE("[%s] Get image header fail.", __func__);
		return false;
	}

	check_sum = 0;
	p = (uint8_t *)mani_p;
	for (uint32_t i = 0; i < sizeof(Manifest_TypeDef); i++) {
		check_sum += p[i];
	}

	BT_LOGD("[%s] Sum of manifest field:0x%x. start_addr:0x%x, img_id:%d\r\n", __func__, check_sum, start_addr, img_id);

	flash_read_step = 0x1000;
	p = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, flash_read_step);
	if (!p) {
		BT_LOGE("[%s] Malloc fail.\r\n", __func__);
		return false;
	}

	// Skip the manifest field on flash
	addr_offset = sizeof(Manifest_TypeDef);
	while (addr_offset < img_hdr->ImgLen) {
		uint32_t read_len;
		if (img_hdr->ImgLen - addr_offset > flash_read_step) {
			read_len = flash_read_step;
		} else {
			read_len = img_hdr->ImgLen - addr_offset;
		}

		fw_update_server_read_flash(start_addr + addr_offset, p, read_len);
		addr_offset += read_len;

		for (uint32_t i = 0; i < read_len; i++) {
			check_sum += p[i];
		}
	}
	if (p) {
		osif_mem_free(p);
	}

	if (check_sum == img_hdr->Checksum) {
		BT_LOGD("[%s] Check sum SUCCESS, sum:0x%08x.\r\n", __func__, check_sum);
		return true;
	} else {
		BT_LOGD("[%s] addr_offset:%d, img_hdr->ImgLen:%d.\r\n", __func__, addr_offset, img_hdr->ImgLen);
		BT_LOGE("[%s] Check sum FAIL, calcute result:0x%08x, header sum:0x%08x.\r\n", __func__, check_sum, img_hdr->Checksum);
		return false;
	}
}

// Write data to a single image flash area
static uint32_t fw_update_server_write_data_to_flash_for_single_image(update_ota_target_hdr *hdr, uint8_t img_id, uint32_t write_addr, uint8_t *p, uint32_t len)
{
	uint32_t mani_l = sizeof(Manifest_TypeDef);
	uint32_t start_addr;
	static uint32_t end_addr = 0;

	BT_LOGD("[%s] img_id:%d, write_addr:0x%x, end_addr:0x%x.\r\n", __func__, img_id, (unsigned int)write_addr, (unsigned int)end_addr);
	if (OTA_IMGID_BOOT == img_id) {
		start_addr = target_addr_image_boot;
	} else if (OTA_IMGID_APP == img_id) {
		start_addr = target_addr_image_app;
	} else {
		BT_LOGE("[%s] Fail, unknown image id:%d.\r\n", __func__, img_id);
		return 0;
	}

	// Calcute sum of image data
	if (write_addr == start_addr) {
		BT_LOGD("[%s] Begin of write data to flash, set check result to false as default.\r\n", __func__);
		check_sum_result = false;
	}

	// Start of the image, save manifest data to ram temp, in case of OTA fail, and reboot from fail new firmware
	if (write_addr == start_addr) {
		BT_LOGD("[%s] Start of the image data, write_addr:0x%08x.\r\n", __func__, write_addr);
		Manifest_TypeDef *mani_p = NULL;
		mani_p = fw_update_server_get_manifest_p(hdr, img_id);
		if (mani_p) {
			update_file_img_hdr *img_hdr = NULL;
			BT_LOGD("[%s] Start save manifest to ram, mani_l:%d.\r\n", __func__, mani_l);
			if (len < mani_l) {
				BT_LOGE("[%s] The received data len(%d) < manifest len(%d), fail.\r\n", __func__, len, mani_l);
				return 0;
			}
			p = fw_update_server_save_manifest_to_ram(mani_p, p);
			len -= mani_l;
			write_addr += mani_l;

			// Calcute the end addr on flash for save new firmware
			img_hdr = fw_update_server_get_img_hdr(hdr, img_id);
			if (!img_hdr) {
				BT_LOGE("[%s] Get imge hdr fail.\r\n", __func__);
				return 0;
			}
			end_addr = start_addr + img_hdr->ImgLen;
			distributor_or_target_erase_flash(start_addr, end_addr);
			BT_LOGD("[%s] Erase ota flash done, start_addr:0x%x, image len:0x%x, end_addr:0x%x.\r\n", __func__, (unsigned int)start_addr, (unsigned int)img_hdr->ImgLen,
					(unsigned int)end_addr);
		} else {
			BT_LOGE("[%s] Get manifest fail for start write ota image to flash, img_id:%d, write_addr:0x%x, start_addr:0x%x.\r\n", __func__, img_id, write_addr,
					start_addr);
			return 0;
		}
	}

	BT_LOGD("[%s] Write data to flash, write_addr:0x%08x, len:%d,end addr:0x%x.\r\n", __func__, write_addr, len, end_addr);
	distributor_or_target_write_data_to_flash(write_addr, p, len);
	write_addr += len;

	// When the whole image have writen to flash finish, recover manifest from ram
	if (write_addr == end_addr) {
		BT_LOGD("[%s] Write firmware data to flash end.\r\n", __func__);
		// Check the sum num of image data
		if (!fw_update_server_check_sum(hdr, img_id)) {
			BT_LOGE("[%s] Check sum num fail, start_addr:0x%08x, write_addr:0x%08x, end_addr:0x%08x.\r\n", __func__, start_addr, write_addr, end_addr);
			check_sum_result = false;
			return 0;
		}
		BT_LOGD("[%s] Set check sum result to 1.\r\n", __func__);
		check_sum_result = true;

		// Recover manifest from ram to flash
		write_addr = 0;
		Manifest_TypeDef *mani_p = NULL;
		mani_p = fw_update_server_get_manifest_p(hdr, img_id);
		if (!mani_p) {
			BT_LOGE("[%s] Get manifest fail for end image.\r\n", __func__);
		} else {
			BT_LOGD("[%s] Recover manifest from ram.\r\n", __func__);
			if (!fw_update_server_recover_manifest(mani_p, start_addr)) {
				BT_LOGE("[%s] Recover manifest fail.\r\n", __func__);
			}
		}
	}

	return write_addr;
}

// Write data to two image flash area
static void fw_update_server_write_data_to_flash_for_two_image(update_ota_target_hdr *hdr, uint32_t *writing_boot, uint32_t *writing_app,
															   uint8_t writing_img_id, uint8_t *p, uint32_t len)
{
	uint32_t img_len;
	update_file_img_hdr *img_hdr;
	BT_LOGE("[%s][%d] Enter.\r\n", __func__, __LINE__);

	img_hdr = fw_update_server_get_img_hdr(hdr, writing_img_id);
	if (!img_hdr) {
		BT_LOGE("[%s] Get imge hdr fail.\r\n", __func__);
		return ;
	}

	BT_LOGD("[%s] Start.\r\n", __func__);

	switch (writing_img_id) {

	case OTA_IMGID_BOOT: {
		img_len = img_hdr->ImgLen - (*writing_boot - target_addr_image_boot);
		BT_LOGD("[%s] Write data to boot image.\r\n", __func__);
		*writing_boot = fw_update_server_write_data_to_flash_for_single_image(hdr, OTA_IMGID_BOOT, *writing_boot, p, img_len);
		p += img_len;
		len -= img_len;

		BT_LOGD("[%s] Write data to app image.\r\n", __func__);
		*writing_app = fw_update_server_write_data_to_flash_for_single_image(hdr, OTA_IMGID_APP, *writing_app, p, len);
		break;
	}

	case OTA_IMGID_APP: {
		img_len = img_hdr->ImgLen - (*writing_app - target_addr_image_app);
		BT_LOGD("[%s] Write data to boot image.\r\n", __func__);
		*writing_app = fw_update_server_write_data_to_flash_for_single_image(hdr, OTA_IMGID_APP, *writing_app, p, img_len);
		p += img_len;
		len -= img_len;

		BT_LOGD("[%s] Write data to app image.\r\n", __func__);
		*writing_boot = fw_update_server_write_data_to_flash_for_single_image(hdr, OTA_IMGID_BOOT, *writing_boot, p, len);
		break;
	}

	default: {
		BT_LOGE("[%s] Unknown img id:%d.\r\n", __func__, writing_img_id);
		break;
	}
	}
}

// Write data to flash according image id
static void fw_update_server_write_data_to_flash(update_ota_target_hdr *hdr, uint8_t image_id, uint8_t *p, uint32_t len)
{
	static uint32_t writing_flash_addr_boot;
	static uint32_t writing_flash_addr_app;
	static uint8_t writing_img_id = 0xff;

	BT_LOGD("[%s] Enter.\r\n", __func__);

	if (0xff == writing_img_id) {
		writing_flash_addr_boot = target_addr_image_boot;
		writing_flash_addr_app = target_addr_image_app;
		BT_LOGD("[%s] Set the init value for writing_flash_addr_boot:0x%x, writing_flash_addr_app:0x%x.\r\n", __func__, writing_flash_addr_boot,
				writing_flash_addr_app);
	}

	switch (image_id) {

	case OTA_IMGID_BOOT: {
		BT_LOGD("[%s] For boot image.\r\n", __func__);
		writing_flash_addr_boot = fw_update_server_write_data_to_flash_for_single_image(hdr, OTA_IMGID_BOOT, writing_flash_addr_boot, p, len);
		writing_img_id = OTA_IMGID_BOOT;
		break;
	}

	case OTA_IMGID_APP: {
		BT_LOGD("[%s] For app image.\r\n", __func__);
		writing_flash_addr_app = fw_update_server_write_data_to_flash_for_single_image(hdr, OTA_IMGID_APP, writing_flash_addr_app, p, len);
		writing_img_id = OTA_IMGID_APP;
		break;
	}

	case IMAGE_INCLUDE_BOOT_AND_APP: {
		fw_update_server_write_data_to_flash_for_two_image(hdr, &writing_flash_addr_boot, &writing_flash_addr_app, writing_img_id, p, len);
		break;
	}

	default: {
		BT_LOGE("[%s] Unknown image id:%d.\r\n", __func__, image_id);
		break;
	}
	}
}

static bool dfu_fw_update_server_start(uint8_t fw_image_idx)
{
	// May be check fw image idx whether is correct or device is not ready for mesh OTA, will return false
	(void)fw_image_idx;
	BT_LOGD("[%s] Enter.\r\n", __func__);
	return true;
}

static void *mesh_ota_reboot_timer_handle = NULL;
static void mesh_ota_reboot_timer_func(void *param)
{
	(void)param;
	BT_LOGD("[%s] Actual start reboot.\r\n", __func__);
	sys_reset();
}

static bool dfu_fw_update_server_apply(void)
{
	uint32_t reboot_delay = 3000;

	// Because mesh stack should response status message for remote device, so should returen this function firstly for release app main task
	if (!osif_timer_create(&mesh_ota_reboot_timer_handle, "BT mesh ota reboot timer", 0, reboot_delay, false, mesh_ota_reboot_timer_func) || \
		!osif_timer_start(&mesh_ota_reboot_timer_handle)) {
		BT_LOGE("[%s] Create timer for reboot fail, start reboot after delay %dms.\r\n", __func__, reboot_delay);
		osif_delay(reboot_delay);
		sys_reset();
	} else {
		BT_LOGD("[%s] Create and start timer for reboot success.\r\n", __func__);
	}

	return true;
}

// Save ota header info to ram
static uint8_t *fw_update_server_save_header_info_to_ram(update_ota_target_hdr *hdr, uint8_t *p, uint32_t len)
{
	uint32_t hdr_num;
	uint8_t *p1 = p;

	if (!hdr || !p1) {
		BT_LOGE("[%s] Input param is NULL, hdr:0x%x, p1:0x%x, fail.\r\n", __func__, hdr, p1);
		return NULL;
	}

	BT_LOGD("[%s] Get ota file header info.\r\n", __func__);
	memcpy(&hdr->FileHdr, p1, sizeof(update_file_hdr));
	p1 += sizeof(update_file_hdr);
	if ((uint32_t)(p1 - p) > len) {
		BT_LOGE("[%s] Extend the max len:%d, fail.\r\n", __func__, len);
		return NULL;
	}

	// The header number should only be 1,2
	hdr_num = hdr->FileHdr.HdrNum;
	if (1 != hdr_num) {
		BT_LOGE("[%s] The image header number should only be 1, but actual is:%d, do not cover this situation, fail.\r\n", __func__, hdr_num);
		return NULL;
	}

	for (uint32_t i = 0; i < hdr_num; i++) {
		memcpy(&hdr->FileImgHdr[i], p1, sizeof(update_file_img_hdr));
		update_file_img_hdr *p_hdr = &hdr->FileImgHdr[i];
		BT_LOGD("[%s] Index:%d, get a ota image header (checksum:0x%x, image len:0x%x, image id:%d).\r\n", __func__, i, p_hdr->Checksum, p_hdr->ImgLen, p_hdr->ImgID);
		p1 += sizeof(update_file_img_hdr);
		if ((uint32_t)(p1 - p) > len) {
			BT_LOGE("[%s] Extend the max len:%d.\r\n", __func__, len);
			return NULL;
		}
	}

	return p1;
}

static bool dfu_fw_update_server_block_data(uint16_t block_num, uint8_t *pdata, uint16_t data_len)
{
	static update_ota_target_hdr ota_hdr = {0};  // Firmware file header
	static uint32_t start_addr_of_ota, end_addr_of_ota;  // the global start and end addr of OTA.bin
	uint8_t *p, image_id;

	BT_LOGD("[%s] Block num:%d, data len:%d.\r\n", __func__, block_num, data_len);
	p = pdata;

	if (0 == block_num) {
		BT_LOGD("[%s] Receive first block data.\r\n", __func__);
		memset(&ota_hdr, 0, sizeof(update_ota_target_hdr));
		if (sizeof(update_file_hdr) + sizeof(update_file_img_hdr) <= data_len) {
			BT_LOGD("[%s] Save the ota header info.\r\n", __func__);
			p = fw_update_server_save_header_info_to_ram(&ota_hdr, p, data_len);
			if (!p) {
				BT_LOGE("[%s] Read ota file header fail.\r\n", __func__);
				return false;
			}

			start_addr_of_ota = p - pdata;
			end_addr_of_ota = data_len;

			BT_LOGD("[%s] Get target image info.\r\n", __func__);
			if (!fw_update_server_get_target_image_info(&ota_hdr)) {
				BT_LOGE("[%s] The length of OTA image > current flash image size, fail.\r\n", __func__);
				return false;
			}

			image_id = fw_update_server_get_image_id_by_ota_addr(&ota_hdr, start_addr_of_ota, end_addr_of_ota);
			BT_LOGD("[%s] image_id:%d for block num 0.\r\n", __func__, image_id);
			fw_update_server_write_data_to_flash(&ota_hdr, image_id, p, data_len - start_addr_of_ota);
			start_addr_of_ota = end_addr_of_ota;
			return true;
		} else {
			// todo, maybe never enter this case
			BT_LOGE("[%s] The block size is %d < header len %d, can not get the complete firmware image header. Do not cover this situation temp.\r\n", __func__, data_len,
					sizeof(update_file_hdr) + sizeof(update_file_img_hdr));
			return false;
		}
	} else {
		end_addr_of_ota = start_addr_of_ota + data_len;
		image_id = fw_update_server_get_image_id_by_ota_addr(&ota_hdr, start_addr_of_ota, end_addr_of_ota);
		BT_LOGD("[%s] image_id:%d for block num %d.\r\n", __func__, image_id, block_num);
		fw_update_server_write_data_to_flash(&ota_hdr, image_id, p, data_len);
		start_addr_of_ota = end_addr_of_ota;
		return true;
	}
}

static bool dfu_fw_update_server_verify(void)
{
	BT_LOGD("[%s] check_sum_result:%d.\r\n", __func__, check_sum_result);
	return check_sum_result;
}

bool mesh_dfu_fw_updater_server_resource_init(struct mesh_dfu_fw_update_server_opts *p_if)
{
	BT_LOGD("[%s][%d] Enter.\r\n", __func__, __LINE__);
	p_if->mesh_dfu_fw_update_server_start = dfu_fw_update_server_start;
	p_if->mesh_dfu_fw_update_server_apply = dfu_fw_update_server_apply;
	p_if->mesh_dfu_fw_update_server_block_data = dfu_fw_update_server_block_data;
	p_if->mesh_dfu_fw_update_server_verify = dfu_fw_update_server_verify;
	return true;
}

#endif  // BT_MESH_ENABLE_DFU_TARGET_ROLE
#endif  // RTK_BLE_MESH_SUPPORT
