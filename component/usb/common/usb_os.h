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

/**
 * @brief Defines the maximum timeout value for semaphore operations.
 * @note This is only defined when an RTOS is used (`!CONFIG_NON_OS`).
 */
#ifndef CONFIG_NON_OS
#define USB_OS_SEMA_TIMEOUT		(RTOS_SEMA_MAX_COUNT)
#endif

/**
 * @brief Ensures a variable is aligned to the cache line size for DMA operations.
 * @details This is critical for DMA buffers to prevent data corruption due to
 *          cache coherency issues.
 */
#ifndef USB_DMA_ALIGNED
#define USB_DMA_ALIGNED		__attribute__((aligned(CACHE_LINE_SIZE)))
#endif

/**
 * @brief Checks if a memory address is aligned to the cache line size.
 * @param x The memory address to check.
 * @return Non-zero if aligned, 0 otherwise.
 */
#ifndef USB_IS_MEM_DMA_ALIGNED
#define USB_IS_MEM_DMA_ALIGNED(x)		((u32)((u32)(x) & ((CACHE_LINE_SIZE)-1)) == 0)
#endif

/**
 * @brief Extracts the low byte from a 16-bit value.
 * @param x The 16-bit value.
 * @return The low byte (u8).
 */
#ifndef USB_LOW_BYTE
#define USB_LOW_BYTE(x)		((u8)((x) & 0x00FFU))
#endif

/**
 * @brief Extracts the high byte from a 16-bit value.
 * @param x The 16-bit value.
 * @return The high byte (u8).
 */
#ifndef USB_HIGH_BYTE
#define USB_HIGH_BYTE(x)	((u8)(((x) >> 8) & 0x00FFU))
#endif

/**
 * @brief Type definitions for abstracting OS objects.
 * @note These types are mapped to the underlying RTOS types when `CONFIG_NON_OS`
 *       is not defined. For a non-OS build, these are not used.
 * @{
 */
#ifndef CONFIG_NON_OS
typedef rtos_mutex_t usb_os_lock_t;   /**< Abstracted type for a mutex or lock. */
typedef rtos_sema_t usb_os_sema_t;    /**< Abstracted type for a semaphore. */
typedef rtos_queue_t usb_os_queue_t;  /**< Abstracted type for a message queue. */
typedef rtos_task_t usb_os_task_t;    /**< Abstracted type for a task handle. */
#endif
/** @} */
/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
 * @brief Puts the current task to sleep for a specified number of milliseconds.
 * @param[in] ms The duration to sleep in milliseconds.
 */
void usb_os_sleep_ms(u32 ms);

/**
 * @brief Delays execution for a specified number of microseconds.
 * @details This is typically a busy-wait loop.
 * @param[in] us The duration to delay in microseconds.
 */
void usb_os_delay_us(u32 us);

/**
 * @brief Fills a block of memory with a specified value.
 * @param[out] buf Pointer to the memory block to fill.
 * @param[in] val The value to be set.
 * @param[in] size The number of bytes to be set to the value.
 */
void usb_os_memset(void *buf, u8 val, u32 size);

/**
 * @brief Copies a block of memory from a source to a destination.
 * @param[out] dst Pointer to the destination array where the content is to be copied.
 * @param[in] src Pointer to the source of data to be copied.
 * @param[in] size The number of bytes to copy.
 */
void usb_os_memcpy(void *dst, const void *src, u32 size);

#ifndef CONFIG_NON_OS
/**
 * @brief Allocates a block of memory from the heap.
 * @param[in] size: The size of the memory block to allocate, in bytes.
 * @return A pointer to the allocated memory, or NULL if the request fails.
 */
void *usb_os_malloc(u32 size);

/**
 * @brief Frees a previously allocated block of memory.
 * @param[in] handle: A pointer to the memory block to be freed.
 */
void usb_os_mfree(void *handle);

/**
 * @brief Creates a new mutex.
 * @param[out] lock: Pointer to the mutex handle to be created.
 * @return 0 on success, non-zero on failure
 */
int usb_os_lock_create(usb_os_lock_t *lock);

/**
 * @brief Deletes a mutex.
 * @param[in] lock: The mutex handle to be deleted.
 * @return 0 on success, non-zero on failure
 */
int usb_os_lock_delete(usb_os_lock_t lock);

/**
 * @brief Acquires a mutex (locks it).
 * @param[in] lock: The mutex handle to be acquired.
 * @return 0 on success, non-zero on failure
 */
int usb_os_lock(usb_os_lock_t lock);

/**
 * @brief Releases a mutex (unlocks it).
 * @param[in] lock: The mutex handle to be released.
 * @return 0 on success, non-zero on failure
 */
int usb_os_unlock(usb_os_lock_t lock);

/**
 * @brief Enters a critical section (disables interrupts).
 * @param[in] in_critical: Flag indicating if this should be a critical section.
 * @return 0 on success, non-zero on failure
 */
int usb_os_enter_critical(u8 in_critical);

/**
 * @brief Exits a critical section (enables interrupts).
 * @param[in] in_critical: Flag indicating if this should be a critical section.
 * @return 0 on success, non-zero on failure
 */
int usb_os_exit_critical(u8 in_critical);

/**
 * @brief Creates a new binary or counting semaphore.
 * @param[out] sema: Pointer to the semaphore handle to be created.
 * @return 0 on success, non-zero on failure
 */
int usb_os_sema_create(usb_os_sema_t *sema);

/**
 * @brief Deletes a semaphore.
 * @param[in] sema: The semaphore handle to be deleted.
 * @return 0 on success, non-zero on failure
 */
int usb_os_sema_delete(usb_os_sema_t sema);

/**
 * @brief Acquires a semaphore.
 * @param[in] sema: The semaphore handle to acquire.
 * @param[in] timeout_ms: The maximum time in milliseconds to wait for the semaphore.
 * @return 0 on success, a negative error code on failure or timeout.
 */
int usb_os_sema_take(usb_os_sema_t sema, u32 timeout_ms);

/**
 * @brief Releases a semaphore.
 * @param[in] sema: The semaphore handle to release.
 * @return 0 on success, non-zero on failure
 */
int usb_os_sema_give(usb_os_sema_t sema);

/**
 * @brief Creates a new message queue.
 * @param[out] queue: Pointer to the queue handle to be created.
 * @param[in] msg_num: The maximum number of messages the queue can hold.
 * @param[in] msg_size: The size of each message in bytes.
 * @return 0 on success, non-zero on failure
 */
int usb_os_queue_create(usb_os_queue_t *queue, u32 msg_num, u32 msg_size);

/**
 * @brief Deletes a message queue.
 * @param[in] queue: The queue handle to be deleted.
 * @return 0 on success, non-zero on failure
 */
int usb_os_queue_delete(usb_os_queue_t queue);

/**
 * @brief Sends a message to a queue.
 * @param[in] queue: The handle of the target queue.
 * @param[in] msg: A pointer to the message to be sent.
 * @param[in] wait_ms: The maximum time in milliseconds to wait if the queue is full.
 * @return 0 on success, non-zero on failure
 */
int usb_os_queue_send(usb_os_queue_t queue, void *msg, u32 wait_ms);

/**
 * @brief Receives a message from a queue.
 * @param[in] queue: The handle of the target queue.
 * @param[out] msg: A pointer to a buffer to store the received message.
 * @param[in] wait_ms: The maximum time in milliseconds to wait if the queue is empty.
 * @return 0 on success, a negative error code on failure or timeout.
 */
int usb_os_queue_receive(usb_os_queue_t queue, void *msg, u32 wait_ms);

/**
 * @brief Gets the size of the free heap memory.
 * @return The number of free bytes in the heap.
 */
u32 usb_os_get_free_heap_size(void);
#endif

#endif /* USB_OS_H */

