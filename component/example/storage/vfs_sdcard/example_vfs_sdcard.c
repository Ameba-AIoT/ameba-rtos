#include "ameba_soc.h"
#include "os_wrapper.h"
#include "vfs.h"
#include "vfs_fatfs.h"
#include "example_vfs_sdcard.h"

void fatfs_usr_cb(int status)
{
	RTK_LOGS(NOTAG, RTK_LOG_INFO, "Fatfs hotplug user callback works, status : %d\n", status);
}

void example_vfs_sdcard_thread(void *param)
{
	rtos_time_delay_ms(3000);
	char filename[] = "vfs_file_1";
	char content[] = "VFS sdcard module works normally !!!";
	char path[128] = {0};
	char buffer[64] = {0};
	char *prefix;
	FILE *finfo;
	int res = 0;

	(void)param;

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\r\n====================Example: VFS SDcard====================\r\n");

	fatfs_set_hotplug_usr_cb(fatfs_usr_cb);

	res = vfs_user_register("sdcard", VFS_FATFS, VFS_INF_SD, VFS_REGION_4, VFS_RW);
	if (res == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "VFS-SDcard Init Success \n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "VFS-SDcard Init Fail \n");
		goto exit;
	}

	prefix = find_vfs_tag(VFS_REGION_4);
	DiagSnPrintf(path, sizeof(path), "%s:%s", prefix, filename);

	finfo = fopen(path, "w");
	if (finfo == NULL) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fopen() failed\r\n", __FUNCTION__);
		goto exit;
	}

	res = fwrite(content, strlen(content), 1, finfo);
	if (res != (int)strlen(content)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fwrite() failed, err is %d\r\n", __FUNCTION__, res);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] fwrite() succeeded, write %d characters:%s\r\n", __FUNCTION__, strlen(content), content);
	}

	fclose(finfo);

	finfo = fopen(path, "r");
	res = fread(buffer, strlen(content), 1, finfo);
	if (res < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fread() failed, err is %d\r\n", __FUNCTION__, res);
	} else {
		if (memcmp(buffer, content, strlen(content)) != 0) {
			RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] fread() content error, read buffer is %s\r\n", __FUNCTION__, buffer);
		} else {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] fread() succeeded, read %d characters:%s\r\n", __FUNCTION__, strlen(buffer), buffer);
		}
	}

	fclose(finfo);

	res = remove(path);
	if (res < 0) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "[%s] remove() file failed, err is %d\r\n", __FUNCTION__, res);
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "[%s] remove() file succeeded\r\n", __FUNCTION__);
	}

exit:
	rtos_task_delete(NULL);
}

void example_vfs_sdcard(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_vfs_sdcard_thread"), example_vfs_sdcard_thread, NULL, 4096 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(example_vfs_sdcard_thread) failed", __FUNCTION__);
	}
}
