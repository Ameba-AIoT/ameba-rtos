/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "flash_api.h"
#include "ota_internal.h"

/* IMG Flash Physical Address use for OTA */
// static u32 IMG_ADDR[OTA_IMGID_MAX][2] = {0};
extern u32 IMG_ADDR[][2];

int ota_storage_init(void)
{
	RTK_LOGI(OTA_TAG, "[STORAGE] Initializing...\n");

	if (IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1] == 0) {
		flash_get_layout_info(IMG_BOOT, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_BOOT_OTA2, &IMG_ADDR[OTA_IMGID_BOOT][OTA_INDEX_2], NULL);
		flash_get_layout_info(IMG_APP_OTA1, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_APP_OTA2, &IMG_ADDR[OTA_IMGID_APP][OTA_INDEX_2], NULL);
#ifdef CONFIG_LINUX_FW_EN
		flash_get_layout_info(IMG_APIMG_OTA1, &IMG_ADDR[OTA_IMGID_APIMG][OTA_INDEX_1], NULL);
		flash_get_layout_info(IMG_APIMG_OTA2, &IMG_ADDR[OTA_IMGID_APIMG][OTA_INDEX_2], NULL);
#endif
	}
	RTK_LOGI(OTA_TAG, "[STORAGE] BOOT OTA1: 0x%08x, OTA2: 0x%08x\n",
			 (unsigned int)IMG_ADDR[OTA_IMGID_BOOT][0],
			 (unsigned int)IMG_ADDR[OTA_IMGID_BOOT][1]);
	RTK_LOGI(OTA_TAG, "[STORAGE] APP  OTA1: 0x%08x, OTA2: 0x%08x\n",
			 (unsigned int)IMG_ADDR[OTA_IMGID_APP][0],
			 (unsigned int)IMG_ADDR[OTA_IMGID_APP][1]);

	RTK_LOGI(OTA_TAG, "[STORAGE] Init complete\n");
	return OTA_OK;
}

u32 ota_storage_get_image_addr(u8 imgId, u8 slotIdx)
{
	if (imgId >= OTA_IMGID_MAX || slotIdx >= 2) {
		RTK_LOGE(OTA_TAG, "[STORAGE] Invalid params: ID=%lu, idx=%lu\n", imgId, slotIdx);
		return 0;
	}
	return IMG_ADDR[imgId][slotIdx];
}

/**
  * @brief  Check if target image length exceeds the layout
  * @param  pOtaTgtHdr: Target header manager
  * @retval OTA_OK: image length is valid
  *         OTA_ERR: image length is invalid
  */
int ota_storage_check_layout(ota_hdr_manager_t *pOtaTgtHdr)
{
	u32 end_addr = 0;
	u8 slotIdx = OTA_INDEX_1;

	for (int index = 0; index < pOtaTgtHdr->ValidImgCnt; index++) {
		if (ota_get_cur_index(pOtaTgtHdr->FileImgHdr[index].ImgID) == OTA_INDEX_1) {
			slotIdx = OTA_INDEX_2;
		}

		if (pOtaTgtHdr->FileImgHdr[index].ImgID == OTA_IMGID_BOOT) {
			if (slotIdx == OTA_INDEX_1) {
				flash_get_layout_info(IMG_BOOT, NULL, &end_addr);
			} else {
				flash_get_layout_info(IMG_BOOT_OTA2, NULL, &end_addr);
			}
		} else if (pOtaTgtHdr->FileImgHdr[index].ImgID == OTA_IMGID_APP) {
			if (slotIdx == OTA_INDEX_1) {
				flash_get_layout_info(IMG_APP_OTA1, NULL, &end_addr);
			} else {
				flash_get_layout_info(IMG_APP_OTA2, NULL, &end_addr);
			}
		}

		u32 start_addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[index].ImgID][slotIdx];

		if ((end_addr - start_addr) < pOtaTgtHdr->FileImgHdr[index].ImgLen) {
			RTK_LOGE(OTA_TAG, "ImgID: %lu, OTA%d start addr: 0x%08X, end addr: 0x%08X, OTA image Length(%d) > Layout(%d)!!!\n",
					 pOtaTgtHdr->FileImgHdr[index].ImgID, slotIdx + 1, (unsigned int)start_addr, (unsigned int)end_addr, pOtaTgtHdr->FileImgHdr[index].ImgLen,
					 (end_addr - start_addr));
			return OTA_ERR;
		}
	}

	return OTA_OK;
}

/**
  * @brief  Verify checksum.
  * @param  pOtaTgtHdr: Target header manager
  * @param  slotIdx: OTA slot index (0=OTA1, 1=OTA2)
  * @param  hdrIdx: Header index
  *
  * @retval OTA_ERR: verify fail, OTA_OK: verify ok
  */
int ota_storage_verify_checksum(ota_hdr_manager_t *pOtaTgtHdr, u8 slotIdx, int hdrIdx)
{
	u32 i;
	u32 flash_checksum = 0;
	u32 addr;
	u32 len;
	ota_manifest_t *manifest = NULL;

	addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[hdrIdx].ImgID][slotIdx];
	len = pOtaTgtHdr->FileImgHdr[hdrIdx].ImgLen - sizeof(ota_manifest_t);
	manifest = &pOtaTgtHdr->Manifest[hdrIdx];

	/* add manifest's checksum */
	for (i = 0; i < sizeof(ota_manifest_t); i++) {
		flash_checksum += *((u8 *)manifest + i);
	}

	/* add flash data's checksum */
	for (i = 0; i < len; i++) {
		flash_checksum += *((u8 *)(addr + i + sizeof(ota_manifest_t)));

	}

	if (flash_checksum != pOtaTgtHdr->FileImgHdr[hdrIdx].Checksum) {
		RTK_LOGE(OTA_TAG, "OTA image(%08x) checksum error!!!\nCalculated checksum 0x%8x, host checksum 0x%8x\n", (unsigned int)addr, \
				 (unsigned int)flash_checksum, (unsigned int)pOtaTgtHdr->FileImgHdr[hdrIdx].Checksum);
		return OTA_ERR;
	} else {
		RTK_LOGI(OTA_TAG, "OTA image(%08x) checksum ok!!!\n", (unsigned int)addr);
	}
	return OTA_OK;
}

/**
 * @brief  Flash stream write
 */
int ota_storage_write(u32 addr, u8 *data, u32 len)
{
	flash_t g_flash;

	if (!data || len == 0) {
		return OTA_ERR;
	}

	flash_stream_write(&g_flash, addr, len, data);
	return OTA_OK;
}

/**
 * @brief  Flash stream read
 */
int ota_storage_read(u32 addr, u8 *data, u32 len)
{
	flash_t g_flash;

	if (!data || len == 0) {
		return OTA_ERR;
	}

	flash_stream_read(&g_flash, addr, len, data);
	return OTA_OK;
}

/**
 * @brief  Flash erase sector
 */
void ota_storage_erase_sector(u32 addr)
{
	flash_t g_flash;
	flash_erase_sector(&g_flash, addr);
}

/**
  * @brief  Update OTA manifest to Flash
  * @param  pOtaTgtHdr: Target header manager
  * @param  slotIdx: OTA slot index (0=OTA1, 1=OTA2)
  * @param  hdrIdx: Header index
  *
  * @retval OTA_ERR: update fail, OTA_OK: update ok
  */
int ota_storage_update_manifest(ota_hdr_manager_t *pOtaTgtHdr, u8 slotIdx, u8 hdrIdx)
{
	u32 addr;
	ota_manifest_t *manifest = NULL;


#if defined(CONFIG_OTA_SWITCH_BY_VALID_HEADER) && (CONFIG_OTA_SWITCH_BY_VALID_HEADER == 1)
	u8 ota_old_index = slotIdx ^ 1;
	u8 empty_sig[8] = {0x0};
#endif

	addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[hdrIdx].ImgID][slotIdx];
	manifest = &pOtaTgtHdr->Manifest[hdrIdx];

	RTK_LOGI(OTA_TAG, "update addr: 0x%08x\n", (unsigned int)addr);
	RTK_LOGI(OTA_TAG, "update version major: %d, minor: %d\n", manifest->MajorKeyVer, manifest->MinorKeyVer);

	/*write the manifest, write pattern finally*/
	ota_storage_write(addr - SPI_FLASH_BASE + 8, (u8 *)manifest + 8, sizeof(ota_manifest_t) - 8);
	ota_storage_write(addr - SPI_FLASH_BASE, (u8 *)manifest->Pattern, 8);

#if defined(CONFIG_OTA_SWITCH_BY_VALID_HEADER) && (CONFIG_OTA_SWITCH_BY_VALID_HEADER == 1)
	if (strncmp("OTA", (const char *)pOtaTgtHdr->FileImgHdr[hdrIdx].Signature, 3) == 0) {
		addr = IMG_ADDR[pOtaTgtHdr->FileImgHdr[hdrIdx].ImgID][ota_old_index];
	} else {
		return OTA_ERR;
	}

	/* clear the old image pattern to 0 finally */
	RTK_LOGI(OTA_TAG, "ImgID: %lu, clear the old image pattern, addr: 0x%08x\n", pOtaTgtHdr->FileImgHdr[hdrIdx].ImgID, (unsigned int)addr);
	ota_storage_write(addr - SPI_FLASH_BASE, empty_sig, 8);
#endif
	RTK_LOGI(OTA_TAG, "[%s] Update OTA success!\n", __FUNCTION__);

	return OTA_OK;
}