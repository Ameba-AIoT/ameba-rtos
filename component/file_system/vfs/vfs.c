#include <stdio.h>
#include "os_wrapper.h"
#include "ameba.h"
#include "time.h"
#include "vfs.h"
#include "ameba_ota.h"

vfs_drv  vfs = {0};
rtos_mutex_t vfs_mutex = NULL;
extern u32 FLASH_APP_BASE;
extern u32 FLASH_SECTOR_COUNT;
extern u32 LFS_FLASH_BASE_ADDR;
extern u32 LFS_FLASH_SIZE;

u32 VFS1_FLASH_BASE_ADDR = 0;
u32 VFS1_FLASH_SIZE = 0;
u32 VFS2_FLASH_BASE_ADDR = 0;
u32 VFS2_FLASH_SIZE = 0;

void vfs_init()
{
	if (vfs_mutex == NULL) {
		VFS_DBG(VFS_INFO, "vfs_mutex init\r\n");
		memset(&vfs, 0, sizeof(vfs_drv));
		rtos_mutex_create(&vfs_mutex);
	} else {
		VFS_DBG(VFS_INFO, "vfs_mutex is already init");
	}
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

	for (i = 0; i < MAX_FS_SIZE; i++) {
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

	for (i = 0; i < MAX_USER_SIZE; i++) {
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
	for (int i = 0; i < MAX_USER_SIZE; i++) {
		if (vfs.user[i].vfs_region == region && vfs.user[i].tag != NULL) {
			return (char *)vfs.user[i].tag;
		}
	}
	return NULL;
}

void vfs_assign_region(int vfs_type, char region)
{
	u32 vfs1_start_addr;
	u32 vfs1_end_addr;
	u32 vfs2_start_addr;
	u32 vfs2_end_addr;

	if (VFS1_FLASH_BASE_ADDR == 0) {
		flash_get_layout_info(VFS1, &vfs1_start_addr, &vfs1_end_addr);
		flash_get_layout_info(USER, &vfs2_start_addr, &vfs2_end_addr);
		VFS1_FLASH_BASE_ADDR = vfs1_start_addr - SPI_FLASH_BASE;
		VFS1_FLASH_SIZE = (vfs1_end_addr - vfs1_start_addr) + 1;
		VFS2_FLASH_BASE_ADDR = vfs2_start_addr - SPI_FLASH_BASE;
		VFS2_FLASH_SIZE = (vfs2_end_addr - vfs2_start_addr) + 1;
	}

	if (vfs_type == VFS_LITTLEFS) {
		LFS_FLASH_BASE_ADDR = region == 1 ? VFS1_FLASH_BASE_ADDR : VFS2_FLASH_BASE_ADDR;
		LFS_FLASH_SIZE = region == 1 ? VFS1_FLASH_SIZE : VFS2_FLASH_SIZE;
	}

#ifdef CONFIG_VFS_FATFS_INCLUDED
	if (vfs_type == VFS_FATFS) {
#ifdef CONFIG_FATFS_WITHIN_APP_IMG
		u8 ota_index = ota_get_cur_index(OTA_IMGID_APP);
		u32 img2_start_addr, img2_end_addr;
		flash_get_layout_info(ota_index == OTA_INDEX_1 ? IMG_APP_OTA1 : IMG_APP_OTA2, &img2_start_addr, &img2_end_addr);
		IMAGE_HEADER *img_hdr = (IMAGE_HEADER *)(img2_start_addr + 0x2000);  //add cert+manifest offset
		while ((u32)img_hdr < img2_end_addr) {
			if (img_hdr->signature[0] == 0x5f736676 && img_hdr->signature[1] == 0x5f746166 && img_hdr->image_addr == (u32)(&VFS1_FLASH_BASE_ADDR)) {
				VFS_DBG(VFS_INFO, "find vfs region2 : 0x%x !!!\r\n", img_hdr);
				break;
			}
			img_hdr = (IMAGE_HEADER *)((u32)img_hdr + 0x1000);
		}
		if ((u32)img_hdr >= img2_end_addr) {
			VFS_DBG(VFS_INFO, "no fatfs binary \r\n");
		} else {
			FLASH_APP_BASE = (u32)img_hdr + 32 - SPI_FLASH_BASE;
			FLASH_SECTOR_COUNT = img_hdr->image_size / 512;
		}
#else
		FLASH_APP_BASE = region == 1 ? VFS1_FLASH_BASE_ADDR : VFS2_FLASH_BASE_ADDR;
		FLASH_SECTOR_COUNT = region == 1 ? (VFS1_FLASH_SIZE / 512) : (VFS2_FLASH_SIZE / 512);
#endif
	}
#endif
	return;
}

int vfs_register(vfs_opt *drv, int vfs_type)
{
	unsigned char drv_num = -1;
	if (vfs.nbr < MAX_FS_SIZE) {
		drv->drv_num = vfs.nbr;	// record driver number for a specific disk
		vfs.drv[vfs.nbr] = drv;
		vfs.drv[vfs.nbr]->vfs_type = vfs_type;
		vfs.nbr++;
		drv_num = drv->drv_num;
	}
	return drv_num;
}

int vfs_scan_vfs(int vfs_type)
{
	int vfs_num = -1;
	unsigned int i = 0;
	for (i = 0; i < vfs.nbr; i++) {
		if (vfs.drv[i]->vfs_type == vfs_type) {
			vfs_num = i;
		}
	}
	return vfs_num;
}

void vfs_set_user_encrypt_callback(char *prefix, vfs_encrypt_callback_t encrypt_func, vfs_decrypt_callback_t decrypt_func, unsigned char iv_len)
{
	int user_id;
	find_vfs_number(prefix, NULL, &user_id);
	if (vfs.user[user_id].vfs_encrypt_callback != NULL) {
		VFS_DBG(VFS_WARNING, "User encrypt call back already exist !!!");
		return;
	}

	vfs.user[user_id].vfs_encrypt_callback = encrypt_func;
	vfs.user[user_id].vfs_decrypt_callback = decrypt_func;
	vfs.user[user_id].encrypt_iv_len = iv_len;
	return;
}

int vfs_user_register(const char *prefix, int vfs_type, int interface, char region, char flag)
{
	int vfs_num = 0;
	int ret = -1;
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);

	if (vfs_type != VFS_FATFS && vfs_type != VFS_LITTLEFS) {
		VFS_DBG(VFS_ERROR, "It don't support the file system");
		goto EXIT;
	} else if (vfs_type == VFS_LITTLEFS && interface != VFS_INF_FLASH) {
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

			vfs.user[vfs.ibr].tag = prefix;
			vfs.user[vfs.ibr].vfs_type = vfs_type;
			vfs.user[vfs.ibr].vfs_interface_type = interface;
			vfs.user[vfs.ibr].vfs_type_id = vfs_num;
			vfs.user[vfs.ibr].vfs_ro_flag = flag;
			vfs.user[vfs.ibr].vfs_region = region;
			vfs.ibr++;
			vfs_assign_region(vfs_type, region);
			ret = vfs.drv[vfs_num]->mount(interface);
			if (ret) {
				VFS_DBG(VFS_ERROR, "vfs mount fail");
				if (region == VFS_REGION_1) {
					lfs_mount_fail = 1;
				}
			}
			goto EXIT;
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
		vfs.ibr--;
		goto EXIT;
	} else {
		ret = -1;
	}
EXIT:
	rtos_mutex_give(vfs_mutex);
	return ret;
}
