#include <stdio.h>
#include "ameba.h"
#include "time.h"
#include "vfs_internal.h"
#include "ameba_ota.h"
#include "vfs_second_nor_flash.h"
#ifdef CONFIG_VFS_LITTLEFS_INCLUDED
#include "littlefs_adapter.h"
#else
u32 LFS_FLASH_BASE_ADDR;
u32 LFS_FLASH_SIZE;
volatile uint8_t lfs_mount_flag;
volatile uint8_t lfs2_mount_flag;
#endif

#ifdef CONFIG_SUPPORT_NAND_FLASH
#include "vfs_nand_ftl.h"
#endif

#ifdef CONFIG_VFS_SECOND_FLASH_NAND
#include "vfs_second_nand_ftl.h"
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
		int drv_id = vfs.drv[vfs_id]->get_interface(vfs.user[user_id].vfs_target);
		char temp[4] = {0};

		if (drv_id < 0) {
			VFS_DBG(VFS_ERROR, "target %d has no fatfs drive", vfs.user[user_id].vfs_target);
			out_name[0] = '\0';
			return;
		}
		temp[0] = drv_id + '0';
		temp[1] = ':';
		temp[2] = '/';

		DiagSnPrintf(out_name, out_size, "%s%s", temp, filename + prefix_len);
	} else {
		DiagSnPrintf(out_name, out_size, "%s", filename + prefix_len);
	}
}

static volatile uint8_t *vfs_mount_flag(int fs, int target)
{
	(void) target;
#ifdef CONFIG_VFS_REALFS_INCLUDED
	if (fs == VFS_REALFS) {
		return &realfs_mount_flag;
	}
#endif
#if defined(CONFIG_VFS_RAM)
	if (target == VFS_TARGET_RAM) {
		return &ram_mount_flag;
	}
#endif
	switch (fs) {
#ifdef CONFIG_VFS_LITTLEFS_INCLUDED
	case VFS_LITTLEFS:
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
		if (target == VFS_TARGET_FLASH_APP_LFS) {
			return &lfs_rolfs_mount_flag;
		}
#endif
		return target == VFS_TARGET_SECOND_FLASH ? &lfs2_mount_flag : &lfs_mount_flag;
#endif
#ifdef CONFIG_VFS_FATFS_INCLUDED
	case VFS_FATFS:
		return target == VFS_TARGET_FLASH_VFS2 || target == VFS_TARGET_FLASH_APP_FATFS ? &fatfs_mount_flag : &fatfs2_mount_flag;
#endif
	default:
		return NULL;
	}
}

int vfs_check_mount_flag(int user_id, const char *operation)
{
	(void) operation;
	if (user_id < 0 || user_id >= VFS_USER_MAX || !vfs.user[user_id].tag) {
		return -1;
	}
	user_config *user = &vfs.user[user_id];
	volatile uint8_t *flag = vfs_mount_flag(user->vfs_type, user->vfs_target);
	if (!flag || *flag != 1) {
		VFS_DBG(VFS_ERROR, "target %d not mounted: %s", user->vfs_target, operation);
		return -1;
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
#ifdef CONFIG_VFS_LITTLEFS_INCLUDED
			g_nand_lfs_cfg.read_size = vfs_nand_flash_pagesize;
			g_nand_lfs_cfg.prog_size = vfs_nand_flash_pagesize;
			g_nand_lfs_cfg.cache_size = vfs_nand_flash_pagesize;
#endif
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
		/* Unmount whatever is still registered so the backends' mount flags are cleared; a later vfs_init() would
		 * otherwise see them set and refuse to register. unregister takes vfs_mutex itself, so call it unlocked. */
		for (int n = 0; n < VFS_USER_MAX; n++) {
			const char *tag = NULL;
			for (int i = 0; i < VFS_USER_MAX && tag == NULL; i++) {
				tag = vfs.user[i].tag;
			}
			if (tag == NULL) {
				break;
			}
			vfs_target_unregister(tag);
		}
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

	for (i = 0; i < VFS_USER_MAX; i++) {
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

	for (i = 0; i < VFS_USER_MAX; i++) {
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

char *vfs_get_prefix(int target)
{
	char *prefix = NULL;

	if (!vfs_mutex) {
		return NULL;
	}
	/* register/unregister fill a slot field by field under vfs_mutex; scan under it too so a half-written slot is never seen */
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);
	for (int i = 0; i < VFS_USER_MAX; i++) {
		if (vfs.user[i].tag && vfs.user[i].vfs_target == target && !vfs_check_mount_flag(i, "prefix")) {
			prefix = (char *)vfs.user[i].tag;
			break;
		}
	}
	rtos_mutex_give(vfs_mutex);
	return prefix;
}

int vfs_target_fs(int target)
{
	switch (target) {
	case VFS_TARGET_FLASH_VFS1:
#if defined(CONFIG_VFS1_LITTLEFS)
		return VFS_LITTLEFS;
#elif defined(CONFIG_VFS1_REALFS)
		return VFS_REALFS;
#endif
		break;
	case VFS_TARGET_FLASH_VFS2:
#if defined(CONFIG_VFS2_LITTLEFS)
		return VFS_LITTLEFS;
#elif defined(CONFIG_VFS2_FATFS)
		return VFS_FATFS;
#endif
		break;
#if defined(CONFIG_FATFS_WITHIN_APP_IMG)
	case VFS_TARGET_FLASH_APP_FATFS:
		return VFS_FATFS;
#endif
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
	case VFS_TARGET_FLASH_APP_LFS:
		return VFS_LITTLEFS;
#endif
	case VFS_TARGET_SECOND_FLASH:
#if defined(CONFIG_VFS_SECOND_FLASH_LITTLEFS)
		return VFS_LITTLEFS;
#elif defined(CONFIG_VFS_SECOND_FLASH_FATFS)
		return VFS_FATFS;
#elif defined(CONFIG_VFS_SECOND_FLASH_REALFS)
		return VFS_REALFS;
#endif
		break;
	case VFS_TARGET_SD:
#if defined(CONFIG_VFS_SD_FATFS)
		return VFS_FATFS;
#elif defined(CONFIG_VFS_SD_REALFS)
		return VFS_REALFS;
#endif
		break;
#if defined(CONFIG_VFS_SD_SPI)
	case VFS_TARGET_SD_SPI:
		return VFS_FATFS;
#endif
#if defined(CONFIG_VFS_USB_HOST)
	case VFS_TARGET_USBH:
		return VFS_FATFS;
#endif
	case VFS_TARGET_RAM:
#if defined(VFS_RAM_LITTLEFS)
		return VFS_LITTLEFS;
#elif defined(VFS_RAM_FATFS)
		return VFS_FATFS;
#elif defined(VFS_RAM_REALFS)
		return VFS_REALFS;
#endif
		break;
	default:
		break;
	}
	return -1;
}

static int vfs_target_setup(int target)
{
	switch (target) {
	case VFS_TARGET_FLASH_VFS1:
	case VFS_TARGET_FLASH_VFS2: {
		u32 start, end;
		flash_get_layout_info(target == VFS_TARGET_FLASH_VFS1 ? VFS1 : VFS2, &start, &end);
		if (start == 0xFFFFFFFF || end == 0xFFFFFFFF || start < SPI_FLASH_BASE || end <= start) {
			RTK_LOGE("VFS", "no target partition\n");
			return -1;
		}
		u32 base = start - SPI_FLASH_BASE;
		u32 size = end - start + 1;
		if (target == VFS_TARGET_FLASH_VFS1) {
			VFS1_FLASH_BASE_ADDR = base;
			VFS1_FLASH_SIZE = size;
		} else {
			VFS2_FLASH_BASE_ADDR = base;
			VFS2_FLASH_SIZE = size;
		}
		if (vfs_target_fs(target) == VFS_LITTLEFS || vfs_target_fs(target) == VFS_REALFS) {
			LFS_FLASH_BASE_ADDR = base;
			LFS_FLASH_SIZE = size;
		}
#ifdef CONFIG_VFS_FATFS_INCLUDED
		if (vfs_target_fs(target) == VFS_FATFS) {
			FLASH_APP_BASE = base;
			FLASH_SECTOR_COUNT = size / SECTOR_SIZE_FLASH;
		}
#endif
		break;
	}
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
	case VFS_TARGET_FLASH_APP_LFS: {
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
			&& img_hdr->image_size <= img2_end_addr - app_end - sizeof(IMAGE_HEADER)
			&& img_hdr->signature[0] == PATTERN_ROLFS_1 && img_hdr->signature[1] == PATTERN_ROLFS_2) {
			LFS_ROLFS_FLASH_BASE_ADDR = app_end + sizeof(IMAGE_HEADER) - SPI_FLASH_BASE;  //payload follows the 32B header
			LFS_ROLFS_FLASH_SIZE = img_hdr->image_size;
			VFS_DBG(VFS_INFO, "find rolfs littlefs blob at 0x%x size 0x%x\r\n", LFS_ROLFS_FLASH_BASE_ADDR, LFS_ROLFS_FLASH_SIZE);
		} else {
			RTK_LOGE("VFS", "no rolfs image\n");
			return -1;
		}
		break;
	}
#endif
#if defined(CONFIG_FATFS_WITHIN_APP_IMG)
	case VFS_TARGET_FLASH_APP_FATFS: {
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
		if ((u32)img_hdr + sizeof(IMAGE_HEADER) > img2_end_addr
			|| (u32)img_hdr + 0x1000 > img2_end_addr
			|| img_hdr->image_size > img2_end_addr - (u32)img_hdr - 0x1000) {
			RTK_LOGE("VFS", "no fatfs image\n");
			return -1;
		} else {
			FLASH_APP_BASE = (u32)img_hdr + 0x1000 - SPI_FLASH_BASE;
			FLASH_SECTOR_COUNT = img_hdr->image_size / SECTOR_SIZE_FLASH;
		}
		break;
	}
#endif
	case VFS_TARGET_SECOND_FLASH:
#ifdef CONFIG_LITTLEFS_SECOND_FLASH
		LFS_SECOND_FLASH_BASE_ADDR = 0;
#ifdef CONFIG_SECOND_FLASH_NOR
		LFS_SECOND_FLASH_SIZE = DATA_FLASH_SIZE / 8 * 1024 * 1024;
#elif defined(CONFIG_VFS_SECOND_FLASH_NAND)
		if (SECOND_NAND_FTL_Init() != HAL_OK) {
			VFS_DBG(VFS_ERROR, "second nand init fail");
			return -1;
		}
		LFS_SECOND_FLASH_SIZE = vfs_second_nand_capacity;
#else
		LFS_SECOND_FLASH_SIZE = current_flash_model.flash_size;
#endif
#endif
#ifdef CONFIG_FATFS_SECOND_FLASH
#ifdef CONFIG_SECOND_FLASH_NOR
		SECOND_FLASH_SECTOR_COUNT = DATA_FLASH_SIZE / 8 * 1024 * 1024 / 512;
#else
		SECOND_FLASH_SECTOR_COUNT = current_flash_model.flash_size / 512;
#endif
#endif
		break;
	default:
		break;
	}
	return 0;
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
	if (!vfs_mutex) {
		return;
	}
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);
	int user_id = find_inf_number(prefix);
	if (user_id < 0) {
		VFS_DBG(VFS_ERROR, "prefix not registered, encrypt callback ignored");
		goto EXIT;
	}
	if (vfs.user[user_id].vfs_enc_callback != NULL) {
		VFS_DBG(VFS_WARNING, "User encrypt call back already exist !!!");
		goto EXIT;
	}
	for (int i = 0; i < VFS_USER_MAX; i++) {
		if (vfs.user[i].tag && vfs.user[i].vfs_target == vfs.user[user_id].vfs_target) {
			vfs.user[i].vfs_enc_callback = encrypt_func;
			vfs.user[i].vfs_dec_callback = decrypt_func;
			vfs.user[i].enc_iv_len = iv_len;
		}
	}
EXIT:
	rtos_mutex_give(vfs_mutex);
}

int vfs_target_register(const char *prefix, int target, char flag)
{
	int ret = -1;
	int user_num = -1;
	int alias = -1;
	int fs = vfs_target_fs(target);

	if (!vfs_mutex || !prefix || !prefix[0] || target < 0 || target >= VFS_TARGET_MAX || fs < 0) {
		VFS_DBG(VFS_ERROR, "target off or bad param");
		return -1;
	}
	/* Required flag: APP image targets are always read only, the RAM disk always read write, FATFS is read only
	 * under FATFS_READ_ONLY; any other target accepts either flag, but never a value outside VFS_RW / VFS_RO. */
	int need_ro = (target == VFS_TARGET_FLASH_APP_FATFS || target == VFS_TARGET_FLASH_APP_LFS);
	int need_rw = (target == VFS_TARGET_RAM);
#ifdef CONFIG_FATFS_READ_ONLY
	need_ro |= (fs == VFS_FATFS);
#endif
	if ((flag != VFS_RW && flag != VFS_RO) || (need_ro && flag != VFS_RO) || (need_rw && flag != VFS_RW)) {
		RTK_LOGE("VFS", "target %d needs %s\n", target, need_ro ? "RO" : need_rw ? "RW" : "RW/RO");
		return -1;
	}
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);
	int registered_id = find_inf_number(prefix);
	if (registered_id >= 0) {
		user_config *user = &vfs.user[registered_id];
		if (user->vfs_target != target || user->vfs_ro_flag != flag) {
			RTK_LOGE("VFS", "prefix mismatch\n");
		} else {
			VFS_DBG(VFS_INFO, "It has been already registered");
			ret = 0;
		}
		goto EXIT;
	}
	for (int i = 0; i < VFS_USER_MAX; i++) {
		if (!vfs.user[i].tag) {
			if (user_num < 0) {
				user_num = i;
			}
		} else if (vfs.user[i].vfs_target == target) {
			alias = i;
		}
	}
	if (user_num < 0) {
		RTK_LOGE("VFS", "no user slot\n");
		goto EXIT;
	}
	if (alias >= 0) {
		vfs.user[user_num] = vfs.user[alias];
		vfs.user[user_num].tag = prefix;
		vfs.user[user_num].vfs_ro_flag = flag;
		ret = 0;
		goto EXIT;
	}
	/* Targets sharing one backend instance share its mount flag, so a set flag means the instance is taken. */
	volatile uint8_t *mount_flag = vfs_mount_flag(fs, target);
	if (!mount_flag || *mount_flag) {
		RTK_LOGE("VFS", "target %d busy\n", target);
		goto EXIT;
	}
	int vfs_num = vfs_scan_vfs(fs);
	if (vfs_num < 0) {
		switch (fs) {
#ifdef CONFIG_VFS_LITTLEFS_INCLUDED
		case VFS_LITTLEFS:
			vfs_num = vfs_register(&littlefs_drv);
			VFS_DBG(VFS_INFO, "littlefs register");
			break;
#endif
#ifdef CONFIG_VFS_FATFS_INCLUDED
		case VFS_FATFS:
			vfs_num = vfs_register(&fatfs_drv);
			VFS_DBG(VFS_INFO, "fatfs register");
			break;
#endif
#ifdef CONFIG_VFS_REALFS_INCLUDED
		case VFS_REALFS:
			vfs_num = vfs_register(&realfs_drv);
			VFS_DBG(VFS_INFO, "realfs register");
			break;
#endif
		default:
			break;
		}
	}
	if (vfs_num < 0) {
		VFS_DBG(VFS_ERROR, "fs %d driver register fail", fs);
		goto EXIT;
	}
	if (vfs_target_setup(target)) {
		goto EXIT;
	}
	ret = vfs.drv[vfs_num]->mount(target);
	if (ret) {
		RTK_LOGE("VFS", "target %d mount fail\n", target);
		goto EXIT;
	}
	user_config *user = &vfs.user[user_num];
	user->tag = prefix;
	user->vfs_type = fs;
	user->vfs_type_id = vfs_num;
	user->vfs_ro_flag = flag;
	user->vfs_target = target;
#ifdef CONFIG_VFS_LITTLEFS_INCLUDED
	if (fs == VFS_LITTLEFS) {
		switch (target) {
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
		case VFS_TARGET_FLASH_APP_LFS:
			user->fs = &g_rolfs_lfs;
			break;
#endif
#ifdef CONFIG_LITTLEFS_SECOND_FLASH
		case VFS_TARGET_SECOND_FLASH:
			user->fs = &g_second_lfs;
			break;
#endif
#if defined(VFS_RAM_LITTLEFS)
		case VFS_TARGET_RAM:
			user->fs = &g_ram_lfs;
			break;
#endif
		default:
			user->fs = &g_lfs;
			break;
		}
	}
#endif
EXIT:
	rtos_mutex_give(vfs_mutex);
	return ret;
}

int vfs_target_unregister(const char *prefix)
{
	int ret = -1;
	if (!vfs_mutex) {
		return ret;
	}
	rtos_mutex_take(vfs_mutex, MUTEX_WAIT_TIMEOUT);
	int user_id = find_inf_number(prefix);
	if (user_id >= 0) {
		int target = vfs.user[user_id].vfs_target;
		int vfs_id = vfs.user[user_id].vfs_type_id;
		for (int i = 0; i < VFS_USER_MAX; i++) {
			if (i != user_id && vfs.user[i].tag && vfs.user[i].vfs_target == target) {
				memset(&vfs.user[user_id], 0, sizeof(user_config));
				ret = 0;
				goto EXIT;
			}
		}
		ret = vfs.drv[vfs_id]->unmount(target);
		memset(&vfs.user[user_id], 0, sizeof(user_config));
	}
EXIT:
	rtos_mutex_give(vfs_mutex);
	return ret;
}

int vfs_auto_register(void)
{
	static const struct {
		u8 target;
		u8 flag;
		const char *prefix;
	} boot[] = {
		{VFS_TARGET_FLASH_VFS1, VFS_RW, VFS_PREFIX},
#if defined(CONFIG_VFS2_LITTLEFS) || defined(CONFIG_VFS2_FATFS)
#if defined(CONFIG_VFS2_FATFS) && defined(CONFIG_FATFS_READ_ONLY)
		{VFS_TARGET_FLASH_VFS2, VFS_RO, VFS2_PREFIX},
#else
		{VFS_TARGET_FLASH_VFS2, VFS_RW, VFS2_PREFIX},
#endif
#endif
#if defined(CONFIG_FATFS_WITHIN_APP_IMG)
		{VFS_TARGET_FLASH_APP_FATFS, VFS_RO, VFS_R3_PREFIX},
#endif
#if defined(CONFIG_LITTLEFS_WITHIN_APP_IMG)
		{VFS_TARGET_FLASH_APP_LFS, VFS_RO, VFS_ROLFS_PREFIX},
#endif
	};
	int ret = 0;
	for (u32 i = 0; i < sizeof(boot) / sizeof(boot[0]); i++) {
		if (vfs_target_register(boot[i].prefix, boot[i].target, boot[i].flag)) {
			RTK_LOGE("VFS", "%s init fail\n", boot[i].prefix);
			ret = -1;
		} else {
			RTK_LOGI("VFS", "%s init success\n", boot[i].prefix);
		}
	}
	return ret;
}
