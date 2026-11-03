/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include <platform_autoconf.h>
#include <string.h>
#include "basic_types.h"
#include "ameba.h"
#include "os_wrapper.h"
#include "atcmd_service.h"
#include "usb_app_mgr.h"

/* Private defines -----------------------------------------------------------*/

/* The app start/stop run on this thread, not on the AT task: some need several KB of stack,
   e.g. usbd_vendor reads the chip UUID for its serial number */
#define USB_APP_MGR_THREAD_STACK_SIZE       6144U
#define USB_APP_MGR_THREAD_PRIORITY         5
#define USB_APP_SWITCH_DELAY_MAX_MS         10000U

#define USB_APP_REQ_START                   0U
#define USB_APP_REQ_STOP                    1U
#define USB_APP_REQ_SWITCH                  2U

/* Private types -------------------------------------------------------------*/

/* One start/stop/switch request handed to the manager thread */
typedef struct {
	const usb_app_t *app;
	u32 delay;
	int ret;
	u8 type;
} usb_app_req_t;

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

static void usb_app_cmd(u16 argc, char **argv);
static int usb_app_request(u8 type, const usb_app_t *app, u32 delay);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "APP";

/* Serializes the AT requests: one at a time is handed to the manager thread */
static rtos_mutex_t usb_app_lock;
/* Request hand-off to the manager thread and its completion */
static rtos_sema_t usb_app_req_sema;
static rtos_sema_t usb_app_done_sema;
static usb_app_req_t usb_app_req;
/* The running app, NULL when the controller is free */
static const usb_app_t *volatile usb_app_cur;
/* A stop failed: the app may still own the stack, no further start is allowed */
static u8 usb_app_broken;
static u32 usb_app_switch_cnt;

/* Exported variables --------------------------------------------------------*/

/* AT commands "AT+<cmd>=<p1>,<p2>": the command name is matched case-sensitively by the AT parser */
ATCMD_TABLE_DATA_SECTION
const log_item_t usb_app_cmd_table[] = {
	{"+USBAPP", usb_app_cmd},
};

/* Private functions ---------------------------------------------------------*/

static const usb_app_t *usb_app_find(const char *name)
{
	u32 i;

	if (name == NULL) {
		return NULL;
	}
	for (i = 0U; i < usb_app_table_num; i++) {
		if (_stricmp(usb_app_table[i].name, name) == 0) {
			return &usb_app_table[i];
		}
	}

	return NULL;
}

static const char *usb_app_role_str(u8 role)
{
	return (role == USB_APP_ROLE_HOST) ? "host" : "device";
}

static int usb_app_parse_ms(const char *arg, u32 *val)
{
	u32 v = 0U;
	const char *p = arg;

	if ((p == NULL) || (*p == '\0')) {
		return -RTK_ERR_BADARG;
	}
	while (*p != '\0') {
		if ((*p < '0') || (*p > '9')) {
			return -RTK_ERR_BADARG;
		}
		v = v * 10U + (u32)(*p - '0');
		if (v > USB_APP_SWITCH_DELAY_MAX_MS) {
			return -RTK_ERR_BADARG;
		}
		p++;
	}
	*val = v;

	return RTK_SUCCESS;
}

/* Manager thread only, the single mutator of usb_app_cur. The AT task holding usb_app_lock and
   waiting for usb_app_done_sema is what keeps a second request out, not the lock itself */
static int usb_app_do_stop(void)
{
	int ret;
	u32 heap;

	if (usb_app_cur == NULL) {
		return RTK_SUCCESS;
	}
	heap = rtos_mem_get_free_heap_size();
	ret = usb_app_cur->stop();
	if (ret != RTK_SUCCESS) {
		/* The app keeps its resources, so the controller is not free either */
		usb_app_broken = 1U;
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Stop %s fail: %d\n", usb_app_cur->name, ret);
		return ret;
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "Stop %s, heap 0x%x->0x%x\n", usb_app_cur->name, heap, rtos_mem_get_free_heap_size());
	usb_app_cur = NULL;

	return RTK_SUCCESS;
}

/* Same single-mutator contract as usb_app_do_stop() */
static int usb_app_do_start(const usb_app_t *app)
{
	int ret;

	if (usb_app_broken != 0U) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "A stop failed, reset the board\n");
		return -RTK_ERR_IO;
	}
	if (usb_app_cur != NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "%s running, stop or switch\n", usb_app_cur->name);
		return -RTK_ERR_BUSY;
	}
	ret = app->start();
	if (ret != RTK_SUCCESS) {
		if (ret == -RTK_ERR_TIMEOUT) {
			/* The start rollback could not join a worker: it may still own the stack */
			usb_app_broken = 1U;
		}
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Start %s fail: %d\n", app->name, ret);
		return ret;
	}
	usb_app_cur = app;
	RTK_LOGS(TAG, RTK_LOG_INFO, "Start %s (%s), heap 0x%x\n", app->name, usb_app_role_str(app->role), rtos_mem_get_free_heap_size());

	return RTK_SUCCESS;
}

static void usb_app_list(void)
{
	u32 i;

	RTK_LOGS(TAG, RTK_LOG_INFO, "Apps (%d):\n", usb_app_table_num);
	for (i = 0U; i < usb_app_table_num; i++) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "  %s %s%s\n", usb_app_table[i].name, usb_app_role_str(usb_app_table[i].role),
				 (usb_app_cur == &usb_app_table[i]) ? " *" : "");
	}
}

static void usb_app_status(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Cur: %s, switch %d, heap 0x%x%s\n",
			 (usb_app_cur != NULL) ? usb_app_cur->name : "none", usb_app_switch_cnt, rtos_mem_get_free_heap_size(),
			 (usb_app_broken != 0U) ? ", broken" : "");
}

static void usb_app_usage(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Usage: AT+USBAPP=<cmd>[,<args>]\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  list                    List the apps built in, * marks the running one\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  start,<app>             Start an app, the controller must be free\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  stop                    Stop the running app\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  switch,<app>[,<ms>]     Stop the running app, wait ms (0..%d, default %d), start app\n",
			 USB_APP_SWITCH_DELAY_MAX_MS, USB_APP_SWITCH_DELAY_MS);
	RTK_LOGS(TAG, RTK_LOG_INFO, "  status                  Print the running app, switch count and free heap\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  help                    Print this help\n");
	RTK_LOGS(TAG, RTK_LOG_INFO, "  Stop the app before using the USB verify commands\n");
}

static void usb_app_cmd(u16 argc, char **argv)
{
	int ret = RTK_SUCCESS;
	const char *cmd = (argc >= 2U) ? argv[1] : NULL;
	const usb_app_t *app;
	u32 delay = USB_APP_SWITCH_DELAY_MS;

	if ((cmd == NULL) || (_stricmp(cmd, "help") == 0) || (_stricmp(cmd, "?") == 0)) {
		usb_app_usage();
		ret = (cmd == NULL) ? -RTK_ERR_BADARG : RTK_SUCCESS;
		goto end;
	}
	/* The AT entry is linked in even when usb_app_mgr_init() failed, which leaves the lock NULL */
	if (usb_app_lock == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Manager init fail\n");
		ret = -RTK_ERR_IO;
		goto end;
	}

	rtos_mutex_take(usb_app_lock, RTOS_MAX_TIMEOUT);
	if (_stricmp(cmd, "list") == 0) {
		usb_app_list();
	} else if (_stricmp(cmd, "status") == 0) {
		usb_app_status();
	} else if (_stricmp(cmd, "stop") == 0) {
		ret = usb_app_request(USB_APP_REQ_STOP, NULL, 0U);
	} else if ((_stricmp(cmd, "start") == 0) || (_stricmp(cmd, "switch") == 0)) {
		app = usb_app_find((argc >= 3U) ? argv[2] : NULL);
		if (app == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Unknown app, try AT+USBAPP=list\n");
			ret = -RTK_ERR_BADARG;
		} else if ((_stricmp(cmd, "switch") == 0) && (argc >= 4U) && (usb_app_parse_ms(argv[3], &delay) != RTK_SUCCESS)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid ms: 0..%d\n", USB_APP_SWITCH_DELAY_MAX_MS);
			ret = -RTK_ERR_BADARG;
		} else {
			ret = usb_app_request((_stricmp(cmd, "start") == 0) ? USB_APP_REQ_START : USB_APP_REQ_SWITCH, app, delay);
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cmd: %s, try AT+USBAPP=help\n", cmd);
		ret = -RTK_ERR_BADARG;
	}
	rtos_mutex_give(usb_app_lock);

end:
	if (ret == RTK_SUCCESS) {
		at_printf(ATCMD_OK_END_STR);
	} else {
		at_printf(ATCMD_ERROR_END_STR, ret);
	}
}

/* Caller holds usb_app_lock */
static int usb_app_request(u8 type, const usb_app_t *app, u32 delay)
{
	usb_app_req.type = type;
	usb_app_req.app = app;
	usb_app_req.delay = delay;
	usb_app_req.ret = RTK_SUCCESS;
	rtos_sema_give(usb_app_req_sema);
	rtos_sema_take(usb_app_done_sema, RTOS_SEMA_MAX_COUNT);

	return usb_app_req.ret;
}

static void usb_app_mgr_thread(void *param)
{
	const usb_app_t *app = (const usb_app_t *)param;
	int ret;

	/* No usb_app_lock here: an AT request holding it waits for this thread, and is served next */
	if ((app != NULL) && (usb_app_do_start(app) != RTK_SUCCESS)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Auto-start %s fail\n", app->name);
	}

	for (;;) {
		if (rtos_sema_take(usb_app_req_sema, RTOS_SEMA_MAX_COUNT) != RTK_SUCCESS) {
			continue;
		}
		if (usb_app_req.type == USB_APP_REQ_START) {
			ret = usb_app_do_start(usb_app_req.app);
		} else {
			ret = usb_app_do_stop();
			if ((ret == RTK_SUCCESS) && (usb_app_req.type == USB_APP_REQ_SWITCH)) {
				rtos_time_delay_ms(usb_app_req.delay);
				ret = usb_app_do_start(usb_app_req.app);
				if (ret == RTK_SUCCESS) {
					usb_app_switch_cnt++;
				}
			}
		}
		usb_app_req.ret = ret;
		rtos_sema_give(usb_app_done_sema);
	}
}

/* Exported functions --------------------------------------------------------*/
/**
  * @brief  Forward an AT command shared by several apps to the running app
  * @note   Called by the per-command entries of usb_app_table.c, in the AT task. The running app
  *         is read without the lock on purpose: a start/stop holds it for seconds and the command
  *         is the app's own, which guards against a concurrent stop like in a standalone build.
  * @param  name: Command name, e.g. "+USBDHID"
  * @param  argc: AT argument count
  * @param  argv: AT arguments
  * @retval None
  */
void usb_app_mgr_dispatch(const char *name, u16 argc, char **argv)
{
	const usb_app_t *app = usb_app_cur;
	const usb_app_cmd_t *cmd;

	if ((app != NULL) && (app->cmds != NULL)) {
		for (cmd = app->cmds; cmd->name != NULL; cmd++) {
			if (strcmp(cmd->name, name) == 0) {
				cmd->handler(argc, argv);
				return;
			}
		}
	}
	RTK_LOGS(TAG, RTK_LOG_ERROR, "AT%s: not for app %s\n", name, (app != NULL) ? app->name : "none");
	at_printf(ATCMD_ERROR_END_STR, -RTK_ERR_BADARG);
}


/**
  * @brief  Create the manager thread, which also starts CONFIG_USB_APP_DEFAULT if it names a built-in app
  * @retval RTK_SUCCESS, or the creation error
  */
int usb_app_mgr_init(void)
{
	int ret;
	const char *name = CONFIG_USB_APP_DEFAULT;
	const usb_app_t *app = NULL;

	ret = rtos_mutex_create(&usb_app_lock);
	if (ret != RTK_SUCCESS) {
		goto exit;
	}
	ret = rtos_sema_create(&usb_app_req_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit_free_lock;
	}
	ret = rtos_sema_create(&usb_app_done_sema, 0U, 1U);
	if (ret != RTK_SUCCESS) {
		goto exit_free_req;
	}

	usb_app_list();
	if (name[0] != '\0') {
		app = usb_app_find(name);
		if (app == NULL) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Auto-start app %s not built in\n", name);
		}
	}

	/* Also runs the auto-start: the app start brings the stack up, which needs the scheduler */
	ret = rtos_task_create(NULL, "usb_app_mgr", usb_app_mgr_thread, (void *)app,
						   USB_APP_MGR_THREAD_STACK_SIZE, USB_APP_MGR_THREAD_PRIORITY);
	if (ret != RTK_SUCCESS) {
		goto exit_free_done;
	}

	return RTK_SUCCESS;

exit_free_done:
	rtos_sema_delete(usb_app_done_sema);
	usb_app_done_sema = NULL;
exit_free_req:
	rtos_sema_delete(usb_app_req_sema);
	usb_app_req_sema = NULL;
exit_free_lock:
	rtos_mutex_delete(usb_app_lock);
	usb_app_lock = NULL;
exit:
	RTK_LOGS(TAG, RTK_LOG_ERROR, "Init fail: %d\n", ret);
	return ret;
}
