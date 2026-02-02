/////////////////////////////////////////////////
//
// kv , key-value pair
//
/////////////////////////////////////////////////

#include "platform_autoconf.h"
#include "kv.h"
#include "vfs.h"
#ifdef CONFIG_VFS_FATFS_INCLUDED
#include "ff.h"
#endif
#include "diag.h"
#include "littlefs_adapter.h"

int kv_init_done = 0;

static char *prefix;
int vfs_kv_init(void)
{
	int ret = -1;
	char *path = NULL;

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 1)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (lfs_mount_flag != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	prefix = find_vfs_tag(VFS_REGION_1);
	if (prefix == NULL) {
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 1, "%s:KV", prefix);
	ret = mkdir(path, 0);
	if (ret == LFS_ERR_EXIST
#ifdef CONFIG_VFS_FATFS_INCLUDED
		|| ret == -FR_EXIST
#endif
	   ) {
		VFS_DBG(VFS_INFO, "KV dir already exist");
		ret = 0;
	}

exit:
	if (ret == 0) {
		kv_init_done = 1;
	} else {
		kv_init_done = -1;
	}

	if (path) {
		rtos_mem_free(path);
	}
	return ret;
}

int32_t rt_kv_set(const char *key, const void *val, int32_t len)
{
	FILE *finfo;
	int res = -1;
	char *path = NULL;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limit exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	finfo = fopen(path, "w");
	if (finfo == NULL) {
		VFS_DBG(VFS_ERROR, "fopen failed, res is %d", res);
		goto exit;
	}

	res = fwrite(val, len, 1, finfo);
	if (res != len) {
		VFS_DBG(VFS_ERROR, "fwrite failed,err is %d!!", res);
	}
	fclose(finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}


int32_t rt_kv_set_offset(const char *key, const void *val, int32_t len, int32_t offset)
{
	FILE *finfo;
	int res = -1;
	char *path = NULL;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limit exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	finfo = fopen(path, "wx");
	if (finfo == NULL) {
		finfo = fopen(path, "+");
		if (finfo == NULL) {
			VFS_DBG(VFS_ERROR, "fopen failed, res is %d", res);
			goto exit;
		}
	}

	if (offset > 0) {
		res = fseek(finfo, offset, SEEK_SET);
		if (res < 0) {
			VFS_DBG(VFS_ERROR, "fseek failed,err is %d!!", res);
			goto exit;
		}
	}

	res = fwrite(val, len, 1, finfo);
	if (res != len) {
		VFS_DBG(VFS_ERROR, "fwrite failed,err is %d!!", res);
	}
	fclose(finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int32_t rt_kv_get(const char *key, void *buffer, int32_t len)
{
	FILE *finfo;
	int res = -1;
	char *path = NULL;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	finfo = fopen(path, "r");
	if (finfo == NULL) {
		VFS_DBG(VFS_WARNING, "fopen failed ");
		goto exit;
	}

	res = fread(buffer, len, 1, finfo);
	if (res < 0) {
		VFS_DBG(VFS_ERROR, "fread failed,err is %d!!!", res);
	}
	fclose(finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int32_t rt_kv_get_offset(const char *key, void *buffer, int32_t len, int32_t offset)
{
	FILE *finfo;
	int res = -1;
	char *path = NULL;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	finfo = fopen(path, "r");
	if (finfo == NULL) {
		VFS_DBG(VFS_WARNING, "fopen failed ");
		goto exit;
	}

	if (offset > 0) {
		res = fseek(finfo, offset, SEEK_SET);
		if (res < 0) {
			VFS_DBG(VFS_ERROR, "fseek failed,err is %d!!", res);
			goto exit;
		}
	}

	res = fread(buffer, len, 1, finfo);
	if (res < 0) {
		VFS_DBG(VFS_ERROR, "fread failed,err is %d!!!", res);
	}
	fclose(finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int32_t rt_kv_size(const char *key)
{
	struct stat *stat_buf;
	int res = -1;
	char *path = NULL;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if ((stat_buf = rtos_mem_zmalloc(sizeof(struct stat))) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);

	res = stat(path, stat_buf);
	if (res < 0) {
		VFS_DBG(VFS_WARNING, "stat failed,err is %d!!!", res);
	} else {
		res = stat_buf->st_size;
	}

exit:
	if (path) {
		rtos_mem_free(path);
	}

	if (stat_buf) {
		rtos_mem_free(stat_buf);
	}

	return res;
}

int32_t rt_kv_delete(const char *key)
{
	int res = -1;
	char *path = NULL;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	res = remove(path);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int rt_kv_list(char *buf, int32_t len)
{
	dirent *info;
	void *dir;
	char *path = NULL;
	char *name_str = NULL;
	int ret = -1;

	if (kv_init_done != 1) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (prefix == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV", prefix);

	dir = opendir(path);
	if (dir == NULL) {
		VFS_DBG(VFS_ERROR, "opendir failed");
		goto exit;
	}

	if ((name_str = rtos_mem_zmalloc(MAX_KEY_LENGTH + 10)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV malloc fail");
		goto exit;
	}

	char *buf_ptr = buf;
	u32 len_left = len - 1;
	u8 fmt_len = 0;
	while (1) {
		info = readdir(dir);
		if (info == NULL) {
			break;
		} else if (strcmp(info->d_name, ".") != 0 && strcmp(info->d_name, "..") != 0) {
			fmt_len = DiagSnPrintf(name_str, MAX_KEY_LENGTH + 10, "%s : %d\n", info->d_name, info->d_reclen);
			if (len_left < fmt_len) {
				VFS_DBG(VFS_WARNING, "buf len is not enough");
				break;
			}

			memcpy(buf_ptr, name_str, fmt_len);
			memset(name_str, 0, MAX_KEY_LENGTH + 10);
			buf_ptr += fmt_len;
			len_left -= fmt_len;
		}
	}

	ret = closedir(dir);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	if (name_str) {
		rtos_mem_free(name_str);
	}

	return ret;
}
