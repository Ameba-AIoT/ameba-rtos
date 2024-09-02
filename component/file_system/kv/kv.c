/////////////////////////////////////////////////
//
// kv , key-value pair
//
/////////////////////////////////////////////////

#include "kv.h"
#include "vfs.h"
#include "ff.h"
#include "diag.h"
#include "littlefs_adapter.h"

extern FILE *fopen(const char *filename, const char *mode);
extern int fclose(FILE *stream);
extern size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
extern size_t fread(void *ptr, size_t size, size_t count, FILE *stream);


static char *prefix;
int rt_kv_init(void)
{
	int ret = -1;
	char *path = NULL;

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 1)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (lfs_mount_fail) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	prefix = find_vfs_tag(VFS_REGION_1);
	if (prefix == NULL) {
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 1, "%s:KV", prefix);
	ret = mkdir(path, 0);
	if (ret == LFS_ERR_EXIST || ret == -FR_EXIST) {
		VFS_DBG(VFS_INFO, "KV dir already exist");
		ret = 0;
	}

exit:
	if (path) {
		rtos_mem_free(path);
	}
	return ret;
}


int32_t rt_kv_set(const char *key, const void *val, int32_t len)
{
	vfs_file *finfo;
	int res = -1;
	char *path = NULL;

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (lfs_mount_fail) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limit exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	finfo = (vfs_file *)fopen(path, "w");
	if (finfo == NULL) {
		VFS_DBG(VFS_ERROR, "fopen failed, res is %d", res);
		goto exit;
	}

	res = fwrite(val, len, 1, (FILE *)finfo);
	if (res != len) {
		VFS_DBG(VFS_ERROR, "fwrite failed,err is %d!!", res);
	}
	fclose((FILE *)finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int32_t rt_kv_get(const char *key, void *buffer, int32_t len)
{
	vfs_file *finfo;
	int res = -1;
	char *path = NULL;

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (lfs_mount_fail) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:KV/%s", prefix, key);
	finfo = (vfs_file *)fopen(path, "r");
	if (finfo == NULL) {
		VFS_DBG(VFS_WARNING, "fopen failed ");
		goto exit;
	}

	res = fread(buffer, len, 1, (FILE *)finfo);
	if (res < 0) {
		VFS_DBG(VFS_ERROR, "fread failed,err is %d!!!", res);
	}
	fclose((FILE *)finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int32_t rt_kv_delete(const char *key)
{
	int res = -1;
	char *path = NULL;

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		VFS_DBG(VFS_ERROR, "KV init fail");
		goto exit;
	}

	if (lfs_mount_fail) {
		VFS_DBG(VFS_INFO, "KV init fail");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		VFS_DBG(VFS_ERROR, "key len limite exceed, max len is %d", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
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
