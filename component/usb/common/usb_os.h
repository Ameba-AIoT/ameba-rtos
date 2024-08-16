/**
  ******************************************************************************
  * @file    usb_ch9.h
  * @author  Realsil WLAN5 Team
  * @brief   This file provides general defines for USB SPEC CH9
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef USB_OS_H
#define USB_OS_H

/* Includes ------------------------------------------------------------------*/

#include "platform_autoconf.h"

#ifdef CONFIG_RTL8721D
#define STD_PRINTF

#include "platform_stdlib.h"
#include "basic_types.h"
#include "log.h"
#endif

#include "basic_types.h"
#include "ameba.h"
#include <string.h>
#ifndef CONFIG_FLOADER_USBD_EN
#include "os_wrapper.h"
#endif

/* Exported defines ----------------------------------------------------------*/

#ifndef CONFIG_FLOADER_USBD_EN
#define USB_OS_SEMA_TIMEOUT		(RTOS_SEMA_MAX_COUNT)
#endif

/* Exported types ------------------------------------------------------------*/

#ifndef CONFIG_FLOADER_USBD_EN

typedef rtos_mutex_t usb_os_lock_t;

typedef rtos_sema_t usb_os_sema_t;

typedef rtos_queue_t usb_os_queue_t;

typedef rtos_task_t usb_os_task_t;

#endif

/* Exported macros -----------------------------------------------------------*/

#ifndef UNUSED
#define UNUSED(X)			(void)X
#endif

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
#define USB_HIGH_BYTE(x)	((u8)(((x) & 0xFF00U) >> 8U))
#endif

#ifndef MIN
#define MIN(a, b)			(((a) < (b)) ? (a) : (b))
#endif

#ifndef MAX
#define MAX(a, b)			(((a) > (b)) ? (a) : (b))
#endif

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

void usb_os_sleep_ms(u32 ms);

void usb_os_delay_us(u32 us);

void usb_os_memset(void *buf, u8 val, u32 size);

void usb_os_memcpy(void *dst, const void *src, u32 size);

#ifndef CONFIG_FLOADER_USBD_EN

void *usb_os_malloc(u32 size);

void usb_os_mfree(void *handle);

int usb_os_lock_create(usb_os_lock_t *lock);

int usb_os_lock_delete(usb_os_lock_t lock);

int usb_os_lock(usb_os_lock_t lock);

int usb_os_unlock(usb_os_lock_t lock);

int usb_os_lock_safe(usb_os_lock_t lock);

int usb_os_unlock_safe(usb_os_lock_t lock);

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

