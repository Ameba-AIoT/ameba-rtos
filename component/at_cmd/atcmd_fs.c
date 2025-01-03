#include "atcmd_service.h"
#include "log.h"
#include "stdlib.h"
#include "kv.h"
#include "vfs.h"

static const char *const TAG = "AT-FS";
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
			if (rt_kv_delete(filename)) {
				error_no = 2;
			}
			goto end;
		}

		if (operation == 2) {
			/*get file size*/
			u8 size = rt_kv_size(filename);
			at_printf("+FS: %d", size);
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

			if (rt_kv_get_offset(filename, buffer, length, offset) == length) {
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
				buffer_ptr += get_len;
				remain_len -= get_len;
			}

			atcmd_tt_mode_end();
			if (rt_kv_set_offset(filename, buffer, length, offset) != length) {
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

		if (rt_kv_list((char *)buffer, 4096) != 0) {
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
	}

}

int atcmd_get_ssl_certificate_size(CERT_TYPE cert_type, int index)
{
	char *prefix;
	char *path = (char *)rtos_mem_zmalloc(128);
	struct stat stat_buf;
	prefix = find_vfs_tag(VFS_REGION_1);
	switch (cert_type) {
	case CLIENT_CA:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/client_ca_%d.crt", prefix, index);
		break;
	case CLIENT_CERT:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/client_cert_%d.crt", prefix, index);
		break;
	case CLIENT_KEY:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/client_key_%d.key", prefix, index);
		break;
	case SERVER_CA:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/server_ca_%d.crt", prefix, index);
		break;
	case SERVER_CERT:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/server_cert_%d.crt", prefix, index);
		break;
	case SERVER_KEY:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/server_key_%d.key", prefix, index);
		break;
	default:
		return -1;
	}

	if (stat(path, &stat_buf) < 0) {
		return 0;
	}

	return stat_buf.st_size;
}

int atcmd_get_ssl_certificate(char *buffer, CERT_TYPE cert_type, int index)
{
	if (buffer == NULL) {
		return -1;
	}

	int ret;
	char *prefix;
	char *path = (char *)rtos_mem_zmalloc(128);
	vfs_file *finfo;
	struct stat stat_buf;
	prefix = find_vfs_tag(VFS_REGION_1);
	switch (cert_type) {
	case CLIENT_CA:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/client_ca_%d.crt", prefix, index);
		break;
	case CLIENT_CERT:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/client_cert_%d.crt", prefix, index);
		break;
	case CLIENT_KEY:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/client_key_%d.key", prefix, index);
		break;
	case SERVER_CA:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/server_ca_%d.crt", prefix, index);
		break;
	case SERVER_CERT:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/server_cert_%d.crt", prefix, index);
		break;
	case SERVER_KEY:
		DiagSnPrintf(path, sizeof(path), "%s:CERT/server_key_%d.key", prefix, index);
		break;
	default:
		return -1;
	}

	ret = stat(path, &stat_buf);
	if (ret < 0 || stat_buf.st_size == 0) {
		return 0;
	}

	finfo = (vfs_file *)fopen(path, "r");
	if (finfo == NULL) {
		return -1;
	}

	ret = fread(buffer, stat_buf.st_size, 1, (FILE *)finfo);

	fclose((FILE *)finfo);

	return ret == stat_buf.st_size ? ret : -1;
}

log_item_t at_fs_items[] = {
	{"+FS", at_fs, {NULL, NULL}},
};

void at_fs_init(void)
{
	atcmd_service_add_table(at_fs_items, sizeof(at_fs_items) / sizeof(at_fs_items[0]));
}