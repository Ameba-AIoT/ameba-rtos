/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_APP_H
#define USB_APP_H

/* Includes ------------------------------------------------------------------*/

#include "basic_types.h"
#include "usb_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

/** @addtogroup USB_Common_Constants USB Common Constants
 *  @{
 */

/** @defgroup USB_Common_App_Constants USB App Constants
 *  @brief Start/stop lifecycle of a switchable USB application, used by the usb_drd example
 *  @{
 */

#define USB_APP_ROLE_DEVICE                 0U
#define USB_APP_ROLE_HOST                   1U

#define USB_APP_STATE_IDLE                  0U  /* Not started, or fully stopped */
#define USB_APP_STATE_RUNNING               1U  /* Started, workers alive */
#define USB_APP_STATE_STOPPING              2U  /* Owned by whoever claimed the stop */

/* Bound on joining one worker thread in a stop. A worker that does not leave in time is
   left alone, the app stays STOPPING and nothing it may still touch is freed. */
#define USB_APP_JOIN_TIMEOUT_MS             5000U

/** @} End of USB_Common_App_Constants group */
/** @} End of USB_Common_Constants group */

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */

/** @defgroup USB_Common_App_Types USB App Types
 *  @{
 */

/* One AT command of an app, same handler signature as the AT service: forwarded by the app
   manager while the app runs, so several apps may use the same command name */
typedef struct {
	const char *name;                       /* Command without "AT", e.g. "+USBDHID" */
	void (*handler)(u16 argc, char **argv);
} usb_app_cmd_t;

/* One switchable application, as listed by the app manager of the usb_drd example */
typedef struct {
	const char *name;                       /* Example directory name, e.g. "usbd_cdc_acm" */
	int (*start)(void);                     /* Bring the app up, task context only */
	int (*stop)(void);                      /* Tear it down completely, task context only, idempotent */
	const usb_app_cmd_t *cmds;              /* AT commands of the app ended by a NULL name, NULL if none */
	u8 role;                                /* USB_APP_ROLE_DEVICE or USB_APP_ROLE_HOST */
} usb_app_t;

/* Lifecycle state shared by the start/stop entry and the worker threads of one app */
typedef struct {
	usb_os_sema_t join_sema;                /* Given once by every worker as it leaves */
	volatile u8 state;                      /* USB_APP_STATE_* */
	volatile u8 quit;                       /* Set by the stop owner: workers leave their loops */
} usb_app_ctx_t;

/** @} End of USB_Common_App_Types group */
/** @} End of USB_Common_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Common_Functions USB Common Functions
 *  @{
 */

/** @defgroup USB_Common_App_Functions USB App Functions
 *  @{
 */

/**
  * @brief  Move an idle app to RUNNING and create its join semaphore
  * @note   Not atomic: the caller serializes every start of an app, as the USB app manager
  *         thread and a standalone example entry do
  * @param  ctx: App lifecycle state
  * @retval RTK_SUCCESS, -RTK_ERR_BUSY if the app is not idle, or the semaphore error
  */
static inline int usb_app_ctx_begin(usb_app_ctx_t *ctx)
{
	int ret;

	if (ctx->state != USB_APP_STATE_IDLE) {
		return -RTK_ERR_BUSY;
	}
	ret = usb_os_sema_create_with_count(&ctx->join_sema, 0U, RTOS_SEMA_MAX_COUNT);
	if (ret != RTK_SUCCESS) {
		return ret;
	}
	ctx->quit = 0U;
	ctx->state = USB_APP_STATE_RUNNING;

	return RTK_SUCCESS;
}

/**
  * @brief  Claim the right to tear the app down
  * @note   Called by the stop entry and by a worker that found the stack dead. Exactly one of
  *         them wins; the loser must not free anything.
  * @param  ctx: App lifecycle state
  * @retval 1 if the caller owns the teardown, 0 otherwise
  */
static inline u8 usb_app_ctx_claim_stop(usb_app_ctx_t *ctx)
{
	u8 owner = 0U;

	usb_os_enter_critical();
	if (ctx->state == USB_APP_STATE_RUNNING) {
		ctx->state = USB_APP_STATE_STOPPING;
		ctx->quit = 1U;
		owner = 1U;
	}
	usb_os_exit_critical();

	return owner;
}

/**
  * @brief  Wait for workers to leave
  * @param  ctx: App lifecycle state
  * @param  count: Number of workers to join
  * @retval RTK_SUCCESS, or -RTK_ERR_TIMEOUT if one did not leave in time
  */
static inline int usb_app_ctx_join(usb_app_ctx_t *ctx, u32 count)
{
	u32 i;

	for (i = 0U; i < count; i++) {
		if (usb_os_sema_take(ctx->join_sema, USB_APP_JOIN_TIMEOUT_MS) != RTK_SUCCESS) {
			return -RTK_ERR_TIMEOUT;
		}
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Called by a worker as the last thing before it deletes itself
  * @param  ctx: App lifecycle state
  * @retval None
  */
static inline void usb_app_ctx_leave(usb_app_ctx_t *ctx)
{
	usb_os_sema_give(ctx->join_sema);
}

/**
  * @brief  Finish a teardown: free the join semaphore and go back to IDLE
  * @param  ctx: App lifecycle state
  * @retval None
  */
static inline void usb_app_ctx_end(usb_app_ctx_t *ctx)
{
	usb_os_sema_delete(ctx->join_sema);
	ctx->join_sema = NULL;
	ctx->quit = 0U;
	ctx->state = USB_APP_STATE_IDLE;
}

/**
  * @brief  Stop entry for an app that is not the teardown owner
  * @note   A worker owning a teardown (dead stack) finishes it on its own; wait for that.
  * @param  ctx: App lifecycle state
  * @retval RTK_SUCCESS once idle, -RTK_ERR_TIMEOUT if the owner did not finish in time
  */
static inline int usb_app_ctx_wait_idle(usb_app_ctx_t *ctx)
{
	u32 waited = 0U;

	while (ctx->state != USB_APP_STATE_IDLE) {
		if (waited >= USB_APP_JOIN_TIMEOUT_MS) {
			return -RTK_ERR_TIMEOUT;
		}
		usb_os_sleep_ms(10U);
		waited += 10U;
	}

	return RTK_SUCCESS;
}

/** @} End of USB_Common_App_Functions group */
/** @} End of USB_Common_Functions group */

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_APP_H */
