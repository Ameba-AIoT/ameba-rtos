/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_OS_H
#define USB_OS_H

/* Includes ------------------------------------------------------------------*/

#include "platform_autoconf.h"
#include <string.h>
#include "basic_types.h"
#include "ameba.h"
#ifndef CONFIG_NON_OS
#include "os_wrapper.h"
#endif

/* Exported defines ----------------------------------------------------------*/

#ifndef CONFIG_NON_OS
#define USB_OS_SEMA_TIMEOUT		(RTOS_SEMA_MAX_COUNT)
#endif

/* Exported types ------------------------------------------------------------*/

#ifndef CONFIG_NON_OS

typedef rtos_mutex_t usb_os_lock_t;

typedef rtos_sema_t usb_os_sema_t;

typedef rtos_queue_t usb_os_queue_t;

typedef rtos_task_t usb_os_task_t;

#endif

/* Exported macros -----------------------------------------------------------*/

#ifndef USB_DMA_ALIGNED
#define USB_DMA_ALIGNED		__attribute__((aligned(CACHE_LINE_SIZE)))
#endif

#ifndef USB_IS_MEM_DMA_ALIGNED
#define USB_IS_MEM_DMA_ALIGNED(x)		((u32)((u32)(x) & ((CACHE_LINE_SIZE)-1)) == 0)
#endif

#ifndef USB_LOW_BYTE
#define USB_LOW_BYTE(x)		((u8)((x) & 0x00FFU))
#endif

#ifndef USB_HIGH_BYTE
#define USB_HIGH_BYTE(x)	((u8)(((x) >> 8) & 0x00FFU))
#endif

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void usb_os_sleep_ms(u32 ms);

void usb_os_delay_us(u32 us);

void usb_os_memset(void *buf, u8 val, u32 size);

void usb_os_memcpy(void *dst, const void *src, u32 size);

#ifndef CONFIG_NON_OS

void *usb_os_malloc(u32 size);

void usb_os_mfree(void *handle);

int usb_os_lock_create(usb_os_lock_t *lock);

int usb_os_lock_delete(usb_os_lock_t lock);

int usb_os_lock(usb_os_lock_t lock);

int usb_os_unlock(usb_os_lock_t lock);

int usb_os_enter_critical(u8 in_critical);

int usb_os_exit_critical(u8 in_critical);

int usb_os_sema_create(usb_os_sema_t *sema);

int usb_os_sema_delete(usb_os_sema_t sema);

int usb_os_sema_take(usb_os_sema_t sema, u32 timeout_ms);

int usb_os_sema_give(usb_os_sema_t sema);

int usb_os_queue_create(usb_os_queue_t *queue, u32 msg_num, u32 msg_size);

int usb_os_queue_delete(usb_os_queue_t queue);

int usb_os_queue_send(usb_os_queue_t queue, void *msg, u32 wait_ms);

int usb_os_queue_receive(usb_os_queue_t queue, void *msg, u32 wait_ms);

u32 usb_os_get_free_heap_size(void);

#endif

#endif /* USB_OS_H */

