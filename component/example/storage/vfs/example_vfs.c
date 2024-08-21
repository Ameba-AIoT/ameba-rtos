#include "vfs.h"
#include "example_vfs.h"
#include "basic_types.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "os_wrapper.h"
#include "ameba.h"

void example_vfs_thread(void *param)
{
	rtos_time_delay_ms(3000);
	char key[] = "vfs_key";
	char val[] = "VFS module works normally !!!";
	char path[128] = {0};
	char buffer[32] = {0};
	char *prefix;
	vfs_file *finfo;
	int res = 0;

	(void)param;

	prefix = find_vfs_tag(VFS_REGION_1);
	DiagSnPrintf(path, sizeof(path), "%s:%s", prefix, key);
	finfo = (vfs_file *)fopen(path, "w");
	if (finfo == NULL) {
		RTK_LOGS(NOTAG, "[%s] fopen failed \r\n", __FUNCTION__);
		goto exit;
	}

	res = fwrite(val, strlen(val), 1, (FILE *)finfo);
	if (res != (int)strlen(val)) {
		RTK_LOGS(NOTAG, "[%s] fwrite failed,err is %d!!\r\n", __FUNCTION__, res);

	} else {
		RTK_LOGS(NOTAG, "[%s] fwrite succeeded !!!\r\n", __FUNCTION__);
	}

	fclose((FILE *)finfo);

	finfo = (vfs_file *)fopen(path, "r");
	res = fread(buffer, strlen(val), 1, (FILE *)finfo);
	if (res < 0) {
		RTK_LOGS(NOTAG, "[%s] fread failed,err is %d!!!\r\n", __FUNCTION__, res);
	} else {
		if (memcmp(buffer, val, strlen(val)) != 0) {
			RTK_LOGS(NOTAG, "read buffer is %s \r\n", buffer);
			RTK_LOGS(NOTAG, "[%s] fread content error !!!\r\n", __FUNCTION__);
		} else {
			RTK_LOGS(NOTAG, "[%s] fread succeeded !!!\r\n", __FUNCTION__);
		}
	}

	fclose((FILE *)finfo);

	res = remove(path);
	if (res < 0) {
		RTK_LOGS(NOTAG, "[%s] remove file failed,err is %d!!!\r\n", __FUNCTION__, res);
	} else {
		RTK_LOGS(NOTAG, "[%s] remove file succeeded !!!\r\n", __FUNCTION__);
	}

exit:
	rtos_task_delete(NULL);
}

void example_vfs(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_vfs_thread"), example_vfs_thread, NULL, 4096 * 4, 1) != SUCCESS) {
		RTK_LOGS(NOTAG, "\n\r%s rtos_task_create(example_kv_thread) failed", __FUNCTION__);
	}
}
