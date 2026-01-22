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
extern u32 SECTOR_SIZE_FLASH;;
extern u32 FLASH_BLOCK_SIZE;

extern u32 SECOND_FLASH_SECTOR_COUNT;
extern u32 DATA_FLASH_SIZE;

u32 VFS1_FLASH_BASE_ADDR = 0;
u32 VFS1_FLASH_SIZE = 0;
u32 VFS2_FLASH_BASE_ADDR = 0;
u32 VFS2_FLASH_SIZE = 0;

int vfs_check_mount_flag(int vfs_type, int vfs_interface_type, char *operation)
{
	(void) operation;
	int *check_flag = NULL;
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
	int ret = -1;

	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Filename is invalid!");
		return ret;
	}

	for (i = 0; i < VFS_FS_MAX; i++) {
		if (vfs.user[i].tag == NULL) {
			VFS_DBG(VFS_INFO, "VFS tag not match!");
			break;
		}
		ret =  strncmp(name, vfs.user[i].tag, strlen(vfs.user[i].tag));
		if (ret == 0) {
			for (j = strlen(vfs.user[i].tag); j < strlen(name); j++) {
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

			*user_id = i;
			return vfs.user[i].vfs_type_id;
		}
	}
	return ret;
}

int find_inf_number(const char *name)
{
	int i = 0;
	int ret = 0;

	if (name == NULL) {
		VFS_DBG(VFS_ERROR, "Filename is invalid!");
		return -1;
	}

	for (i = 0; i < VFS_USER_REGION_MAX; i++) {
		if (vfs.user[i].tag != NULL) {
			ret =  strncmp(name, vfs.user[i].tag, strlen(vfs.user[i].tag));
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
				while ((u32)img_hdr < img2_end_addr) {
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

int vfs_register(vfs_opt *drv, int vfs_type)
{
	unsigned char drv_num = -1;
	if (vfs.drv_num < VFS_FS_MAX) {
		drv->drv_num = vfs.drv_num;	// record driver number for a specific disk
		vfs.drv[vfs.drv_num] = drv;
		vfs.drv[vfs.drv_num]->vfs_type = vfs_type;
		vfs.drv_num++;
		drv_num = drv->drv_num;
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
	int user_id;
	find_vfs_number(prefix, NULL, &user_id);
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
		if (!find_inf_number(prefix)) {
			VFS_DBG(VFS_INFO, "It has been already registered");
			ret = 0;
		} else {
			vfs_num = vfs_scan_vfs(vfs_type);
			if (vfs_num < 0) {
				if (vfs_type == VFS_LITTLEFS) {
					vfs_num = vfs_register(&littlefs_drv, vfs_type);
					VFS_DBG(VFS_INFO, "littlefs register");
				}
#ifdef CONFIG_VFS_FATFS_INCLUDED
				else {
					vfs_num = vfs_register(&fatfs_drv, vfs_type);
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
			ret = vfs.drv[vfs_num]->mount(interface);
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
	(void) vfs_type;
	int user_id = 0;
	int vfs_id = 0;
	int ret = -1;
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);
	user_id = find_inf_number(prefix);
	if (user_id >= 0) {
		vfs_id = vfs.user[user_id].vfs_type_id;
		memset(&vfs.user[user_id], 0x00, sizeof(user_config));
		ret = vfs.drv[vfs_id]->unmount(interface);
		goto EXIT;
	} else {
		ret = -1;
	}
EXIT:
	rtos_mutex_give(vfs_mutex);
	return ret;
}
