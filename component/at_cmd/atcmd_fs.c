#include "platform_stdlib.h"
#include "atcmd_service.h"
#include "kv.h"
#include "vfs.h"

static const char *const TAG = "AT-FS";
static char g_cert_fs = VFS_REGION_1;

#define MAX_CERT_LEN 4096

int32_t at_fs_delete(const char *key)
{
	int res;
	char *path = NULL;
	char *prefix = find_vfs_tag(g_cert_fs);
	if (prefix == NULL) {
		return -1;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
		return -1;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:AT/%s", prefix, key);
	res = remove(path);
	rtos_mem_free(path);
	return res;
}

int32_t at_fs_size(const char *key)
{
	struct stat *stat_buf;
	int res = -1;
	char *path = NULL;
	char *prefix = find_vfs_tag(g_cert_fs);

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
		goto exit;
	}

	if ((stat_buf = rtos_mem_zmalloc(sizeof(struct stat))) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		RTK_LOGE(TAG, "key len limite exceed, max len is %d\r\n", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		RTK_LOGE(TAG, "vfs init fail\r\n");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:AT/%s", prefix, key);

	res = stat(path, stat_buf);
	if (res < 0) {
		RTK_LOGE(TAG, "stat failed,err is %d!!!\r\n", res);
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

int32_t at_fs_get_offset(const char *key, void *buffer, int32_t len, int32_t offset)
{
	FILE *finfo;
	int res = -1;
	char *path = NULL;
	char *prefix = find_vfs_tag(g_cert_fs);

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		RTK_LOGE(TAG, "key len limite exceed, max len is %d\r\n", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		RTK_LOGE(TAG, "vfs init fail\r\n");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:AT/%s", prefix, key);
	finfo = fopen(path, "r");
	if (finfo == NULL) {
		RTK_LOGE(TAG, "fopen failed\r\n");
		goto exit;
	}

	if (offset > 0) {
		res = fseek(finfo, offset, SEEK_SET);
		if (res < 0) {
			RTK_LOGE(TAG, "fseek failed,err is %d!!\r\n", res);
			goto exit;
		}
	}

	res = fread(buffer, len, 1, finfo);
	if (res < 0) {
		RTK_LOGE(TAG, "fread failed,err is %d!!\r\n", res);
	}
	fclose(finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int32_t at_fs_set_offset(const char *key, const void *val, int32_t len, int32_t offset)
{
	FILE *finfo;
	int res = -1;
	char *path = NULL;
	char *prefix = find_vfs_tag(g_cert_fs);

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
		goto exit;
	}

	if (strlen(key) > MAX_KEY_LENGTH - 3) {
		RTK_LOGE(TAG, "key len limite exceed, max len is %d\r\n", MAX_KEY_LENGTH - 3);
		goto exit;
	}

	if (prefix == NULL) {
		RTK_LOGE(TAG, "vfs init fail\r\n");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:AT/%s", prefix, key);
	finfo = fopen(path, "wx");
	if (finfo == NULL) {
		finfo = fopen(path, "+");
		if (finfo == NULL) {
			RTK_LOGE(TAG, "fopen failed\r\n");
			goto exit;
		}
	}

	if (offset > 0) {
		res = fseek(finfo, offset, SEEK_SET);
		if (res < 0) {
			RTK_LOGE(TAG, "fseek failed\r\n");
			goto exit;
		}
	}

	res = fwrite(val, len, 1, finfo);
	if (res != len) {
		RTK_LOGE(TAG, "fwrite failed\r\n");
	}
	fclose(finfo);

exit:
	if (path) {
		rtos_mem_free(path);
	}

	return res;
}

int at_fs_list(char *buf, int32_t len)
{
	dirent *info;
	void *dir;
	char *path = NULL;
	char *name_str = NULL;
	char *prefix = find_vfs_tag(g_cert_fs);
	int ret = -1;

	if (prefix == NULL) {
		RTK_LOGE(TAG, "vfs init fail\r\n");
		goto exit;
	}

	if ((path = rtos_mem_zmalloc(MAX_KEY_LENGTH + 2)) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
		goto exit;
	}

	DiagSnPrintf(path, MAX_KEY_LENGTH + 2, "%s:AT", prefix);

	dir = opendir(path);
	if (dir == NULL) {
		RTK_LOGE(TAG, "opendir failed\r\n");
		goto exit;
	}

	if ((name_str = rtos_mem_zmalloc(MAX_KEY_LENGTH + 10)) == NULL) {
		RTK_LOGE(TAG, "malloc fail\r\n");
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
				RTK_LOGE(TAG, "buf len is not enough\r\n");
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

static void at_fs_help(void)
{
	RTK_LOGI(TAG, "\r\n");
	RTK_LOGI(TAG, "AT+FS=<operation>[,<filename>][,<offset>,<length>]\r\n");
	RTK_LOGI(TAG, "\t<operation>:\t0=list, 1=delete, 2=get size, 3=read, 4=write\r\n");
	RTK_LOGI(TAG, "\t<filename>:\t\tneeded by operation 1,2,3,4\r\n");
	RTK_LOGI(TAG, "\t<offset>:\t\tneeded by operation 3,4\r\n");
	RTK_LOGI(TAG, "\t<length>:\t\tneeded by operation 3,4\r\n");
}

//AT+FS=<operation>,<filename>,<offset>,<length>
// operation: 0=list, 1=delete, 2=get size, 3=read, 4=write
void at_fs(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	u8 *buffer = NULL;
	u8 operation = 0;
	char *filename = NULL;
	int offset = 0, length = 0;
	u8 error_no = 0;

	if (arg == NULL) {
		RTK_LOGW(TAG, "The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if ((argc < 2) || (argc > 5)) {
		RTK_LOGW(TAG, "The parameters format ERROR\r\n");
		error_no = 1;
		goto end;
	}

	operation = atoi(argv[1]);
	if (operation > 4) {
		RTK_LOGW(TAG, "The operation is invalid\r\n");
		error_no = 1;
		goto end;
	}

	if (operation > 0) {
		if (argc < 3) {
			RTK_LOGW(TAG, "The parameters format ERROR\r\n");
			error_no = 1;
			goto end;
		}

		filename = (char *)argv[2];
		if (operation == 1) {
			/*delete file*/
			if (at_fs_delete(filename)) {
				error_no = 2;
			}
			goto end;
		}

		if (operation == 2) {
			/*get file size*/
			int size = at_fs_size(filename);
			if (size < 0) {
				error_no = 2;
			} else {
				at_printf("+FS: %d", size);
			}
			goto end;
		}

		if (argc < 5) {
			RTK_LOGW(TAG, "The parameters format ERROR\r\n");
			error_no = 1;
			goto end;
		}

		offset = atoi(argv[3]);
		length = atoi(argv[4]);
		if (operation == 3) {
			/*read file*/
			buffer = (u8 *)rtos_mem_zmalloc(length + 1);
			if (buffer == NULL) {
				RTK_LOGW(TAG, "Memory allocation failed\r\n");
				error_no = 2;
				goto end;
			}

			if (at_fs_get_offset(filename, buffer, length, offset) == length) {
				at_printf("+FS: %s", buffer);
			} else {
				error_no = 2;
			}
		}

		if (operation == 4) {
			/*write file*/
			buffer = (u8 *)rtos_mem_zmalloc(length + 1);
			if (buffer == NULL) {
				RTK_LOGW(TAG, "Memory allocation failed\r\n");
				error_no = 2;
				goto end;
			}

			if (atcmd_tt_mode_start(length) != 0) {
				error_no = 2;
				goto end;
			}

			u8 *buffer_ptr = buffer;
			int get_len = 0, remain_len = length;
			while (remain_len > 0) {
				get_len = atcmd_tt_mode_get(buffer_ptr, remain_len);
				if (get_len == 0) {
					RTK_LOGW(TAG, "host stops tt mode\r\n");
					error_no = 2;
					atcmd_tt_mode_end();
					goto end;
				}
				buffer_ptr += get_len;
				remain_len -= get_len;
			}

			atcmd_tt_mode_end();
			if (at_fs_set_offset(filename, buffer, length, offset) != length) {
				error_no = 2;
				goto end;
			}

		}
	} else if (operation == 0) {
		/*list file*/
		buffer = (u8 *)rtos_mem_zmalloc(4096);
		if (buffer == NULL) {
			RTK_LOGW(TAG, "Memory allocation failed\r\n");
			error_no = 2;
			goto end;
		}

		if (at_fs_list((char *)buffer, 4096) != 0) {
			error_no = 2;
		} else {
			at_printf("+FS: \r\n%s", buffer);
		}
	}

end:
	if (buffer != NULL) {
		rtos_mem_free(buffer);
	}

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		at_fs_help();
	}

}

int atcmd_get_ssl_certificate_size(CERT_TYPE cert_type, int index)
{
	if (index < 1 || index > 10) {
		return -1;
	}

	char *prefix;
	char *path = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);

	if (path == NULL) {
		return -1;
	}

	struct stat stat_buf;
	prefix = find_vfs_tag(g_cert_fs);
	if (prefix == NULL) {
		RTK_LOGW(TAG, "VFS region is not exist\r\n");
		return -1;
	}

	switch (cert_type) {
	case CLIENT_CA:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/client_ca_%d.crt", prefix, index);
		break;
	case CLIENT_CERT:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/client_cert_%d.crt", prefix, index);
		break;
	case CLIENT_KEY:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/client_key_%d.key", prefix, index);
		break;
	case SERVER_CA:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/server_ca_%d.crt", prefix, index);
		break;
	case SERVER_CERT:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/server_cert_%d.crt", prefix, index);
		break;
	case SERVER_KEY:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/server_key_%d.key", prefix, index);
		break;
	default:
		rtos_mem_free(path);
		return -1;
	}

	if (stat(path, &stat_buf) < 0) {
		rtos_mem_free(path);
		return 0;
	}

	rtos_mem_free(path);
	return stat_buf.st_size;
}

int atcmd_get_ssl_certificate(char *buffer, CERT_TYPE cert_type, int index)
{
	if (buffer == NULL) {
		return -1;
	}

	if (index < 1 || index > 10) {
		return -1;
	}

	int ret;
	char *prefix;
	char *path = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);

	if (path == NULL) {
		return -1;
	}

	FILE *finfo;
	struct stat stat_buf;
	prefix = find_vfs_tag(g_cert_fs);
	if (prefix == NULL) {
		RTK_LOGW(TAG, "VFS region is not exist\r\n");
		return -1;
	}

	switch (cert_type) {
	case CLIENT_CA:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/client_ca_%d.crt", prefix, index);
		break;
	case CLIENT_CERT:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/client_cert_%d.crt", prefix, index);
		break;
	case CLIENT_KEY:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/client_key_%d.key", prefix, index);
		break;
	case SERVER_CA:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/server_ca_%d.crt", prefix, index);
		break;
	case SERVER_CERT:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/server_cert_%d.crt", prefix, index);
		break;
	case SERVER_KEY:
		DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/server_key_%d.key", prefix, index);
		break;
	default:
		rtos_mem_free(path);
		return -1;
	}

	ret = stat(path, &stat_buf);
	if (ret < 0 || stat_buf.st_size == 0) {
		rtos_mem_free(path);
		return 0;
	}

	finfo = fopen(path, "r");
	if (finfo == NULL) {
		rtos_mem_free(path);
		return -1;
	}

	ret = fread(buffer, stat_buf.st_size, 1, finfo);

	fclose(finfo);

	rtos_mem_free(path);

	return ret == stat_buf.st_size ? ret : -1;
}

static void at_cert_help(void)
{
	RTK_LOGI(TAG, "\r\n");
	RTK_LOGI(TAG, "AT+CERT=<role>,<index>\r\n");
	RTK_LOGI(TAG, "\t<role>:\t0=client, 1=server\r\n");
	RTK_LOGI(TAG, "\t<index>:\tcert index, start from 1\r\n");
}

void at_cert(void *arg)
{
	int argc = 0, res;
	char *argv[MAX_ARGC] = {0};
	char *path = NULL;
	char *prefix;
	FILE *finfo;
	char *buffer = NULL;
	u8 error_no = 0, role, index;

	if (arg == NULL) {
		RTK_LOGW(TAG, "The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 3) {
		RTK_LOGW(TAG, "Number of parameters is wrong\r\n");
		error_no = 1;
		goto end;
	}

	role = atoi(argv[1]);
	index = atoi(argv[2]);

	path = (char *)rtos_mem_zmalloc(VFS_PATH_MAX);
	buffer = (char *)rtos_mem_zmalloc(MAX_CERT_LEN + 1);
	memset(buffer, 0, sizeof(buffer));
	prefix = find_vfs_tag(g_cert_fs);
	if (prefix == NULL) {
		RTK_LOGW(TAG, "VFS region is not exist\r\n");
		error_no = 2;
		goto end;
	}

	DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/%s_ca_%d.crt", prefix, role == 0 ? "client" : "server", index);
	at_printf("%s\r\n", path);
	finfo = fopen(path, "r");
	if (finfo != NULL) {
		res = fread(buffer, MAX_CERT_LEN, 1, finfo);
		if (res < 0) {
			RTK_LOGW(TAG, "%s read fail \r\n", path);
		} else {
			at_printf("%s", buffer);
		}
		fclose(finfo);
	}
	at_printf("\r\n");

	DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/%s_key_%d.key", prefix, role == 0 ? "client" : "server", index);
	at_printf("%s\r\n", path);
	finfo = fopen(path, "r");
	if (finfo != NULL) {
		res = fread(buffer, MAX_CERT_LEN, 1, finfo);
		if (res < 0) {
			RTK_LOGW(TAG, "%s read fail \r\n", path);
		} else {
			at_printf("%s\r\n", buffer);
		}
		fclose(finfo);
	}
	at_printf("\r\n");

	DiagSnPrintf(path, VFS_PATH_MAX, "%s:CERT/%s_cert_%d.crt", prefix, role == 0 ? "client" : "server", index);
	at_printf("%s\r\n", path);
	finfo = fopen(path, "r");
	if (finfo != NULL) {
		res = fread(buffer, MAX_CERT_LEN, 1, finfo);
		if (res < 0) {
			RTK_LOGW(TAG, "%s read fail \r\n", path);
		} else {
			at_printf("%s\r\n", buffer);
		}
		fclose(finfo);
	}
	at_printf("\r\n");


end:
	if (buffer != NULL) {
		rtos_mem_free(buffer);
	}

	if (path != NULL) {
		rtos_mem_free(path);
	}

	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		at_cert_help();
	}

}

static void at_fs_region_help(void)
{
	RTK_LOGI(TAG, "\r\n");
	RTK_LOGI(TAG, "AT+FSRGN=<fs region>\r\n");
	RTK_LOGI(TAG, "\t<fs region>:\t1=region1, 2=region2\r\n");
}

void at_fs_region(void *arg)
{
	int argc = 0;
	char *argv[MAX_ARGC] = {0};
	u8 error_no = 0, region;

	if (arg == NULL) {
		RTK_LOGW(TAG, "The parameters can not be ignored\r\n");
		error_no = 1;
		goto end;
	}

	argc = parse_param(arg, argv);
	if (argc != 2) {
		RTK_LOGW(TAG, "Number of parameters is wrong\r\n");
		error_no = 1;
		goto end;
	}

	region = atoi(argv[1]);
	if (region == 1) {
		g_cert_fs = VFS_REGION_1;
	} else if (region == 2) {
		g_cert_fs = VFS_REGION_2;
	} else if (region == 3) {
		g_cert_fs = VFS_REGION_3;
	} else if (region == 4) {
		g_cert_fs = VFS_REGION_4;
	} else {
		RTK_LOGW(TAG, "Parameter value is wrong\r\n");
		error_no = 1;
		goto end;
	}

end:
	if (error_no == 0) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, error_no);
		at_fs_region_help();
	}

}

ATCMD_APONLY_TABLE_DATA_SECTION
const log_item_t at_fs_items[] = {
	{"+FS", at_fs},
	{"+CERT", at_cert},
	{"+FSRGN", at_fs_region},
};
