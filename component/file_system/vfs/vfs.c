#include <stdio.h>
#include "ameba.h"
#include "time.h"
#include "vfs.h"
#include "ameba_ota.h"
#include "vfs_second_nor_flash.h"
#include "littlefs_adapter.h"

#ifdef CONFIG_SUPPORT_NAND_FLASH
#include "vfs_nand_ftl.h"
#endif

vfs_drv  vfs = {0};
rtos_mutex_t vfs_mutex = NULL;

extern u8 fatfs_flash_is_nand;
extern u32 FLASH_APP_BASE;
extern u8 SECTOR_NUM;
extern u32 FLASH_SECTOR_COUNT;
extern u32 SECTOR_SIZE_FLASH;
extern u32 FLASH_BLOCK_SIZE;

extern u32 SECOND_FLASH_SECTOR_COUNT;
extern u32 DATA_FLASH_SIZE;

u32 VFS1_FLASH_BASE_ADDR = 0;
u32 VFS1_FLASH_SIZE = 0;
u32 VFS2_FLASH_BASE_ADDR = 0;
u32 VFS2_FLASH_SIZE = 0;

#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
/* read-only ROLFS littlefs (blob embedded within app image) */
extern lfs_t g_rolfs_lfs;
extern u32 LFS_ROLFS_FLASH_BASE_ADDR;
extern u32 LFS_ROLFS_FLASH_SIZE;
extern int littlefs_rolfs_mount(void);
extern int littlefs_rolfs_unmount(void);

/* Leading fields of the SDK Manifest_TypeDef (see ameba_secure_boot.h). Only Ver
 * and ImgSize are needed to find the end of the app image, so we mirror just the
 * header here instead of pulling in the secure-boot header (and its rom/bootutil
 * dependency) into this shared file. Field offsets must match: Ver@16, ImgSize@24. */
typedef struct {
	u32 Pattern[2];
	u8  Rsvd1[8];
	u8  Ver;
	u8  ImgID;
	u8  AuthAlg;
	u8  HashAlg;
	u16 MajorImgVer;
	u16 MinorImgVer;
	u32 ImgSize;
} rolfs_manifest_hdr_t;
#endif

void vfs_build_filename(int vfs_id, int user_id,
						const char *filename, int prefix_len,
						char *out_name, size_t out_size)
{
	if (vfs.drv[vfs_id]->vfs_type == VFS_FATFS) {
		int drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_interface_type);
		char temp[4] = {0};

		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';

		DiagSnPrintf(out_name, out_size, "%s%s", temp, filename + prefix_len);
	} else {
		DiagSnPrintf(out_name, out_size, "%s", filename + prefix_len);
	}
}

int vfs_check_mount_flag(int vfs_type, int vfs_interface_type, char region, char *operation)
{
	(void) operation;
	volatile uint8_t *check_flag = NULL;
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
	/* read-only ROLFS littlefs has its own instance + mount flag */
	if (vfs_type == VFS_LITTLEFS && region == VFS_REGION_ROLFS) {
		if (lfs_rolfs_mount_flag != 1) {
			VFS_DBG(VFS_ERROR, "vfs-littlefs rolfs mount fail, %s is not allowed", operation);
			return -1;
		}
		return 0;
	}
#else
	(void) region;
#endif
	switch (vfs_type) {
#ifdef CONFIG_VFS_FATFS_INCLUDED
	case VFS_FATFS:
		if (vfs_interface_type == VFS_INF_SECOND_FLASH || vfs_interface_type == VFS_INF_SD
			|| vfs_interface_type == VFS_INF_SD_SPI || vfs_interface_type == VFS_INF_USBH) {
			check_flag = &fatfs2_mount_flag;
		} else if (vfs_interface_type == VFS_INF_FLASH) {
			check_flag = &fatfs_mount_flag;
		}
		if (check_flag != NULL && *check_flag != 1) {
			VFS_DBG(VFS_ERROR, "vfs-fatfs mount fail, %s is not allowed", operation);
			return -1;
		}
		break;
#endif
	case VFS_LITTLEFS:
		if (vfs_interface_type == VFS_INF_SECOND_FLASH) {
			check_flag = &lfs2_mount_flag;
		} else if (vfs_interface_type == VFS_INF_FLASH) {
			check_flag = &lfs_mount_flag;
		}
		if (check_flag != NULL && *check_flag != 1) {
			VFS_DBG(VFS_ERROR, "vfs-littlefs mount fail, %s is not allowed", operation);
			return -1;
		}
		break;
	default:
		return -1;
		break;
	}
	return 0;
}

void vfs_init()
{
	if (vfs_mutex == NULL) {
		VFS_DBG(VFS_INFO, "vfs_mutex init\r\n");
		memset(&vfs, 0, sizeof(vfs_drv));
		rtos_mutex_create(&vfs_mutex);
	} else {
		VFS_DBG(VFS_INFO, "vfs_mutex is already init");
	}

#ifdef CONFIG_SUPPORT_NAND_FLASH
	if (SHOULD_USE_NAND()) {
		if (NAND_FTL_Init() == HAL_OK) {
#ifdef CONFIG_VFS_FATFS_INCLUDED
			//fatfs config
			SECTOR_SIZE_FLASH = vfs_nand_flash_pagesize;
			SECTOR_NUM = vfs_nand_flash_pagenum;
			FLASH_BLOCK_SIZE = SECTOR_SIZE_FLASH * SECTOR_NUM;
			fatfs_flash_is_nand = 1;
#endif
			//littlefs config
			g_nand_lfs_cfg.read_size = vfs_nand_flash_pagesize;
			g_nand_lfs_cfg.prog_size = vfs_nand_flash_pagesize;
			g_nand_lfs_cfg.cache_size = vfs_nand_flash_pagesize;
		} else {
			VFS_DBG(VFS_ERROR, "nand ftl init fail");
		}
	}
#endif
}

void vfs_deinit()
{
	if (vfs_mutex != NULL) {
		VFS_DBG(VFS_INFO, "vfs_mutex deinit\r\n");
		memset(&vfs, 0, sizeof(vfs_drv));
		rtos_mutex_delete(vfs_mutex);
		vfs_mutex = NULL;
	} else {
		VFS_DBG(VFS_INFO, "vfs_mutex is already deinit");
	}
}

int find_vfs_number(const char *name, int *prefix_len, int *user_id)
{
	size_t i, j = 0;
	size_t name_tag_len;
	int ret = -1;

	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Filename is invalid!");
		return ret;
	}

	name_tag_len = 0;
	while (name[name_tag_len] != '\0' && name[name_tag_len] != ':' && name[name_tag_len] != '/') {
		name_tag_len++;
	}

	for (i = 0; i < VFS_USER_REGION_MAX; i++) {
		size_t tag_len;
		size_t cmp_len;
		if (vfs.user[i].tag == NULL) {
			VFS_DBG(VFS_INFO, "VFS tag not match!");
			continue;  /* skip holes left by unregister, don't stop */
		}
		tag_len = strlen(vfs.user[i].tag);
		cmp_len = name_tag_len > tag_len ? name_tag_len : tag_len;
		ret =  strncmp(name, vfs.user[i].tag, cmp_len);
		if (ret == 0) {
			/* initialise prefix_len so the loop below is a no-op when name has no suffix after the tag */
			if (prefix_len != NULL) {
				*prefix_len = (int)tag_len;
			}
			for (j = tag_len; j < strlen(name); j++) {
				if (name[j] != '/' && name[j] != ':') {
					if (prefix_len != NULL) {
						*prefix_len = j;
					}
					break;
				} else {
					if ((j + 1) == strlen(name)) {
						if (prefix_len != NULL) {
							*prefix_len = j + 1;
						}
						break;
					}
				}
			}

			if (user_id != NULL) {
				*user_id = i;
			}
			return vfs.user[i].vfs_type_id;
		}
	}
	return ret;
}

int find_inf_number(const char *name)
{
	int i = 0;
	int ret = 0;
	size_t name_tag_len;

	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Filename is invalid!");
		return -1;
	}

	name_tag_len = 0;
	while (name[name_tag_len] != '\0' && name[name_tag_len] != ':' && name[name_tag_len] != '/') {
		name_tag_len++;
	}

	for (i = 0; i < VFS_USER_REGION_MAX; i++) {
		if (vfs.user[i].tag != NULL) {
			size_t tag_len = strlen(vfs.user[i].tag);
			size_t cmp_len = name_tag_len > tag_len ? name_tag_len : tag_len;
			ret =  strncmp(name, vfs.user[i].tag, cmp_len);
			if (ret == 0) {
				VFS_DBG(VFS_INFO, "Correct %s %d", __FUNCTION__, i);
				return i;
			} else {
				VFS_DBG(VFS_INFO, "Wrong %s %d", __FUNCTION__, i);
			}
		}
	}
	return -1;
}

char *find_vfs_tag(char region)
{
	for (int i = 0; i < VFS_USER_REGION_MAX; i++) {
		if (vfs.user[i].vfs_region == region && vfs.user[i].tag != NULL) {
			return (char *)vfs.user[i].tag;
		}
	}
	return NULL;
}

void vfs_assign_region(int vfs_type, char region, int interface)
{
	u32 vfs1_start_addr;
	u32 vfs1_end_addr;
	u32 vfs2_start_addr;
	u32 vfs2_end_addr;

	if (VFS1_FLASH_BASE_ADDR == 0) {
		flash_get_layout_info(VFS1, &vfs1_start_addr, &vfs1_end_addr);
		flash_get_layout_info(VFS2, &vfs2_start_addr, &vfs2_end_addr);
		VFS1_FLASH_BASE_ADDR = vfs1_start_addr - SPI_FLASH_BASE;
		VFS1_FLASH_SIZE = (vfs1_end_addr - vfs1_start_addr) + 1;
		VFS2_FLASH_BASE_ADDR = vfs2_start_addr - SPI_FLASH_BASE;
		VFS2_FLASH_SIZE = (vfs2_end_addr - vfs2_start_addr) + 1;
	}

	if (vfs_type == VFS_LITTLEFS) {
#ifdef CONFIG_LITTLEFS_SECOND_FLASH
		if (interface == VFS_INF_SECOND_FLASH) {
			if (region == VFS_REGION_4) {
				LFS_SECOND_FLASH_BASE_ADDR = 0x0;
#ifdef CONFIG_SECOND_FLASH_NOR
				LFS_SECOND_FLASH_SIZE = DATA_FLASH_SIZE / 8 * 1024 * 1024;
#else
				LFS_SECOND_FLASH_SIZE = current_flash_model.flash_size;
#endif
			} else {
				VFS_DBG(VFS_ERROR, "Interface(%d) with region(%d) is not supported by LITTLEFS\r\n", interface, region);
			}
		}
#endif
		if (interface == VFS_INF_FLASH) {
			if (region == VFS_REGION_1) {
				LFS_FLASH_BASE_ADDR = VFS1_FLASH_BASE_ADDR;
				LFS_FLASH_SIZE = VFS1_FLASH_SIZE;
			} else if (region == VFS_REGION_2) {
				LFS_FLASH_BASE_ADDR = VFS2_FLASH_BASE_ADDR;
				LFS_FLASH_SIZE = VFS2_FLASH_SIZE;
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
			} else if (region == VFS_REGION_ROLFS) {
				/* read-only littlefs blob appended right after the current OTA slot's
				 * app image. The app image layout is [cert(4K)][manifest(4K or 8K)][sub-images],
				 * and the blob is a 32B IMAGE_HEADER (PATTERN_ROLFS_*) followed by the littlefs
				 * payload. The app size is recorded in its manifest, so jump straight to
				 * the end instead of scanning:
				 *     app_end = 4096 + ManifestSize + manifest.ImgSize   (32B-aligned pad)
				 * cert is always 4K; ManifestSize is 4K normally or 8K when Manifest.Ver >= 2
				 * (PQC) — same rule the bootloader uses, so this stays correct across SoCs. */
				u8 ota_index = ota_get_cur_index(OTA_IMGID_APP);
				u32 img2_start_addr, img2_end_addr;
				flash_get_layout_info(ota_index == OTA_INDEX_1 ? IMG_APP_OTA1 : IMG_APP_OTA2, &img2_start_addr, &img2_end_addr);
				LFS_ROLFS_FLASH_BASE_ADDR = 0;
				LFS_ROLFS_FLASH_SIZE = 0;
				/* cert is always 4K; manifest follows immediately after */
				rolfs_manifest_hdr_t *manifest = (rolfs_manifest_hdr_t *)(img2_start_addr + 4096);
				u32 manifest_size = (manifest->Ver >= 2) ? (8 * 1024) : (4 * 1024);
				u32 app_end = img2_start_addr + 4096 + manifest_size + manifest->ImgSize;
				app_end = (app_end + 31) & ~31U;  //blob header is 32B-aligned (pack-time pad)
				IMAGE_HEADER *img_hdr = (IMAGE_HEADER *)app_end;
				if ((app_end + sizeof(IMAGE_HEADER)) <= img2_end_addr
					&& img_hdr->signature[0] == PATTERN_ROLFS_1 && img_hdr->signature[1] == PATTERN_ROLFS_2) {
					LFS_ROLFS_FLASH_BASE_ADDR = app_end + sizeof(IMAGE_HEADER) - SPI_FLASH_BASE;  //payload follows the 32B header
					LFS_ROLFS_FLASH_SIZE = img_hdr->image_size;
					VFS_DBG(VFS_INFO, "find rolfs littlefs blob at 0x%x size 0x%x\r\n", LFS_ROLFS_FLASH_BASE_ADDR, LFS_ROLFS_FLASH_SIZE);
				} else {
					VFS_DBG(VFS_ERROR, "no rolfs littlefs blob found in app image\r\n");
				}
#endif
			} else {
				VFS_DBG(VFS_ERROR, "Interface(%d) with region(%d) is not supported by LITTLEFS\r\n", interface, region);
			}
		}
	}

#ifdef CONFIG_VFS_FATFS_INCLUDED
	if (vfs_type == VFS_FATFS) {
		if (region == VFS_REGION_4) {
			if (interface == VFS_INF_SECOND_FLASH) {
#ifdef CONFIG_FATFS_SECOND_FLASH
#ifdef CONFIG_SECOND_FLASH_NOR
				SECOND_FLASH_SECTOR_COUNT = DATA_FLASH_SIZE / 8 * 1024 * 1024 / 512;
#else
				SECOND_FLASH_SECTOR_COUNT = current_flash_model.flash_size / 512;
#endif
#endif
			} else if (interface == VFS_INF_SD || interface == VFS_INF_SD_SPI || interface == VFS_INF_USBH) {
				// nothing to do now
			}	else {
				VFS_DBG(VFS_ERROR, "Interface(%d) with region(%d) is not supported by FATFS\r\n", interface, region);
			}
		}

		if (interface == VFS_INF_FLASH) {
			if (region == VFS_REGION_1) {
				FLASH_APP_BASE = VFS1_FLASH_BASE_ADDR;
				FLASH_SECTOR_COUNT = VFS1_FLASH_SIZE / SECTOR_SIZE_FLASH;
			} else if (region == VFS_REGION_2) {
				FLASH_APP_BASE = VFS2_FLASH_BASE_ADDR;
				FLASH_SECTOR_COUNT = VFS2_FLASH_SIZE / SECTOR_SIZE_FLASH;
			} else if (region == VFS_REGION_3) {
#ifdef CONFIG_FATFS_WITHIN_APP_IMG
#ifndef OTA_IMGID_APP
				u8 ota_index = ota_get_cur_index(OTA_IMGID_IMG2);
#else
				u8 ota_index = ota_get_cur_index(OTA_IMGID_APP);
#endif
				u32 img2_start_addr, img2_end_addr;
				flash_get_layout_info(ota_index == OTA_INDEX_1 ? IMG_APP_OTA1 : IMG_APP_OTA2, &img2_start_addr, &img2_end_addr);
				IMAGE_HEADER *img_hdr = (IMAGE_HEADER *)(img2_start_addr + 0x2000);  //add cert+manifest offset
				while ((u32)img_hdr + sizeof(IMAGE_HEADER) <= img2_end_addr) {
					if (img_hdr->signature[0] == PATTERN_VFS_1 && img_hdr->signature[1] == PATTERN_VFS_2 && img_hdr->image_addr == (u32)(&VFS1_FLASH_BASE_ADDR)) {
						VFS_DBG(VFS_INFO, "find vfs region2 : 0x%x !!!\r\n", img_hdr);
						break;
					}
					img_hdr = (IMAGE_HEADER *)((u32)img_hdr + 0x1000);
				}
				if ((u32)img_hdr >= img2_end_addr) {
					VFS_DBG(VFS_INFO, "no fatfs binary \r\n");
				} else {
					FLASH_APP_BASE = (u32)img_hdr + 0x1000 - SPI_FLASH_BASE;
					FLASH_SECTOR_COUNT = img_hdr->image_size / SECTOR_SIZE_FLASH;
				}
#else
				VFS_DBG(VFS_ERROR, "VFS_REGION_3 should used by CONFIG_FATFS_WITHIN_APP_IMG\r\n");
#endif
			} else {
				VFS_DBG(VFS_ERROR, "Interface(%d) with region(%d) is not supported by FATFS\r\n", interface, region);
			}
		}
	}
#endif
	return;
}

int vfs_register(const vfs_opt *drv)
{
	int drv_num = -1;
	if (vfs.drv_num < VFS_FS_MAX) {
		vfs.drv[vfs.drv_num] = drv;
		drv_num = vfs.drv_num;
		vfs.drv_num++;
	}
	return drv_num;
}

int vfs_scan_vfs(int vfs_type)
{
	int vfs_num = -1;
	unsigned int i = 0;
	for (i = 0; i < vfs.drv_num; i++) {
		if (vfs.drv[i]->vfs_type == vfs_type) {
			vfs_num = i;
		}
	}
	return vfs_num;
}

void vfs_set_user_encrypt_callback(char *prefix, vfs_enc_callback_t encrypt_func, vfs_dec_callback_t decrypt_func, unsigned char iv_len)
{
	int user_id = -1;
	int ret = find_vfs_number(prefix, NULL, &user_id);
	if (ret < 0 || user_id < 0 || user_id >= VFS_USER_REGION_MAX) {
		VFS_DBG(VFS_ERROR, "prefix not registered, encrypt callback ignored");
		return;
	}
	if (vfs.user[user_id].vfs_enc_callback != NULL) {
		VFS_DBG(VFS_WARNING, "User encrypt call back already exist !!!");
		return;
	}

	vfs.user[user_id].vfs_enc_callback = encrypt_func;
	vfs.user[user_id].vfs_dec_callback = decrypt_func;
	vfs.user[user_id].enc_iv_len = iv_len;
	return;
}

int vfs_user_register(const char *prefix, int vfs_type, int interface, char region, char flag)
{
	int vfs_num = 0;
	int user_num = -1;
	int ret = -1;
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);

	if (vfs_type != VFS_FATFS && vfs_type != VFS_LITTLEFS) {
		VFS_DBG(VFS_ERROR, "It don't support the file system");
		goto EXIT;
	} else if (vfs_type == VFS_LITTLEFS && interface > VFS_INF_SECOND_FLASH) {
		VFS_DBG(VFS_ERROR, "interface type not supported by littlefs");
		goto EXIT;
	} else {
		if (find_inf_number(prefix) >= 0) {
			VFS_DBG(VFS_INFO, "It has been already registered");
			ret = 0;
		} else {
			vfs_num = vfs_scan_vfs(vfs_type);
			if (vfs_num < 0) {
				if (vfs_type == VFS_LITTLEFS) {
					vfs_num = vfs_register(&littlefs_drv);
					VFS_DBG(VFS_INFO, "littlefs register");
				}
#ifdef CONFIG_VFS_FATFS_INCLUDED
				else {
					vfs_num = vfs_register(&fatfs_drv);
					VFS_DBG(VFS_INFO, "fatfs register");
				}
#endif
			}

			if (vfs_num < 0) {
				VFS_DBG(VFS_ERROR, "vfs_register failed \r\n");
				goto EXIT;
			}

			//find an idle user num
			for (int i = 0; i < VFS_USER_REGION_MAX; i++) {
				if (!vfs.user[i].tag) {
					user_num = i;
					break;
				}
			}

			if (user_num == -1) {
				VFS_DBG(VFS_INFO, "No more user region can be registered \r\n");
				goto EXIT;
			}

			vfs_assign_region(vfs_type, region, interface);
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
			if (vfs_type == VFS_LITTLEFS && region == VFS_REGION_ROLFS) {
				/* read-only ROLFS blob uses its own lfs instance and a mount that never formats */
				ret = littlefs_rolfs_mount();
			} else
#endif
			{
				ret = vfs.drv[vfs_num]->mount(interface);
			}
			if (ret) {
				VFS_DBG(VFS_ERROR, "vfs mount fail");
				goto EXIT;
			}

			vfs.user[user_num].tag = prefix;
			vfs.user[user_num].vfs_type = vfs_type;
			vfs.user[user_num].vfs_interface_type = interface;
			vfs.user[user_num].vfs_type_id = vfs_num;
			vfs.user[user_num].vfs_ro_flag = flag;
			vfs.user[user_num].vfs_region = region;
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
			if (vfs_type == VFS_LITTLEFS && region == VFS_REGION_ROLFS) {
				vfs.user[user_num].fs = &g_rolfs_lfs;
			} else
#endif
				if (interface == VFS_INF_FLASH) {
					vfs.user[user_num].fs = &g_lfs;
				}
#ifdef CONFIG_LITTLEFS_SECOND_FLASH
			if (interface == VFS_INF_SECOND_FLASH) {
				vfs.user[user_num].fs = &g_second_lfs;
			}
#endif
		}
	}
EXIT:
	rtos_mutex_give(vfs_mutex);
	return ret;
}

int vfs_user_unregister(const char *prefix, int vfs_type, int interface)
{
	int user_id = 0;
	int vfs_id = 0;
	int ret = -1;
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);
	user_id = find_inf_number(prefix);
	if (user_id >= 0) {
		vfs_id = vfs.user[user_id].vfs_type_id;
		char region = vfs.user[user_id].vfs_region;
		memset(&vfs.user[user_id], 0x00, sizeof(user_config));
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
		if (vfs_type == VFS_LITTLEFS && region == VFS_REGION_ROLFS) {
			ret = littlefs_rolfs_unmount();
		} else
#else
		(void) vfs_type;
		(void) region;
#endif
		{
			ret = vfs.drv[vfs_id]->unmount(interface);
		}
		goto EXIT;
	} else {
		ret = -1;
	}
EXIT:
	rtos_mutex_give(vfs_mutex);
	return ret;
}
