/*
 * Copyright (c) 2026 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_RINGBUF_H
#define USB_RINGBUF_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file   usb_ringbuf.h
 * @brief  USB Ring Buffer Management Module
 *
 * This module implements a block-based memory manager that handles discrete data nodes rather than a byte stream.
 * This structure ensures that data frames address meet DMA cache-line alignment requirements.
 */

/** @addtogroup USB_Common_API USB Common API
 *  @{
 */

/* Exported defines ----------------------------------------------------------*/

#ifndef CONFIG_NON_OS

/* Exported types ------------------------------------------------------------*/

/** @addtogroup USB_Common_Types USB Common Types
 *  @{
 */
/** @defgroup USB_Common_Ringbuf_Types USB Ring Buffer Types
 *  @brief Node and manager types of the block-based USB ring buffer.
 *  @{
 */

/**
 * @brief Individual data node within the ring buffer.
 * Represents a single block of memory within the managed buffer pool.
 */
typedef struct {
	u8 *buf;               /**< Pointer to the actual data storage area for this node. */
	__IO u16 buf_len;      /**< Length of valid data currently stored in this node (bytes). */
	__IO u8 data_valid;    /**< Data validity flag. 1 indicates valid payload; 0 indicates empty/invalid. */
} usb_ringbuf_t;

/**
 * @brief Ring Buffer Manager Control Structure.
 * Main structure used to manage the state, memory, and pointers of the circular buffer.
 */
typedef struct {
	usb_ringbuf_t *list_node;   /**< Array of metadata nodes. Size equals 'capacity'. */
	u8 *buffer;                 /**< Base address of the contiguous memory pool. Total size = capacity * node_size. */
	u16 node_size;              /**< Maximum data size (in bytes) for a single node. */

	__IO u8 head;               /**< Read index (Consumer). Points to the next node to be dequeued. */
	__IO u8 tail;               /**< Write index (Producer). Points to the next slot for enqueuing. */
	__IO u8 capacity;           /**< Maximum number of nodes (blocks) the buffer can hold. */
} usb_ringbuf_manager_t;

/** @} End of USB_Common_Ringbuf_Types group */
/** @} End of USB_Common_Types group */

/* Exported macros -----------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/** @addtogroup USB_Common_Functions USB Common Functions
 *  @{
 */
/** @defgroup USB_Common_Ringbuf_Functions USB Ring Buffer Functions
 *  @brief Lifecycle and enqueue/dequeue operations of the block-based USB ring buffer.
 *  @{
 */

/**
 * @brief  Initializes the USB ring buffer manager.
 *
 * Allocates necessary memory resources for the node metadata and the raw data buffer.
 * It also resets the read/write indices.
 *
 * @param[in,out] handle       Pointer to the manager structure to initialize.
 * @param[in]     count        Capacity of the ring buffer (number of discrete nodes).
 * @param[in]     size         Maximum size in bytes for each data node.
 * @param[in]     cache_align  Memory alignment requirement (in bytes). Useful for DMA or cache line alignment.
 *
 * @return
 *   - 0: Success.
 *   - Non-zero: Error (e.g., memory allocation failed).
 */
int usb_ringbuf_manager_init(usb_ringbuf_manager_t *handle, u8 count, u16 size, u8 cache_align);

/**
 * @brief  De-initializes the ring buffer manager.
 *
 * Frees all allocated memory resources associated with the handle and resets internal states.
 *
 * @param[in,out] handle  Pointer to the manager structure to de-initialize.
 * @return 0 on success.
 */
int usb_ringbuf_manager_deinit(usb_ringbuf_manager_t *handle);

/**
 * @brief  Resets the ring buffer state.
 *
 * Clears the buffer logically by resetting the head and tail pointers to zero.
 * This function does not free memory; it simply discards all existing data.
 *
 * @param[in,out] handle  Pointer to the manager structure.
 * @return 0 on success.
 */
int usb_ringbuf_reset(usb_ringbuf_manager_t *handle);

/**
 * @brief  Gets the head packet in place, without removing it.
 *
 * Consumer side only. The returned node stays owned by the consumer, and the producer
 * cannot overwrite it, until usb_ringbuf_pop_head() is called.
 *
 * @param[in]  handle  Pointer to the manager structure.
 * @param[out] len     Length of the packet, only valid when the return value is not NULL.
 * @return Pointer to the packet data, or NULL if the buffer is empty.
 */
u8 *usb_ringbuf_peek_head(usb_ringbuf_manager_t *handle, u32 *len);

/**
 * @brief  Removes the head packet previously read by usb_ringbuf_peek_head().
 *
 * Consumer side only.
 *
 * @param[in,out] handle  Pointer to the manager structure.
 */
void usb_ringbuf_pop_head(usb_ringbuf_manager_t *handle);

/**
 * @brief  Discards every queued packet from the consumer side.
 *
 * Unlike usb_ringbuf_reset(), only the head pointer is moved (to the tail), so it is safe
 * against a producer concurrently calling usb_ringbuf_add_tail() on another core.
 *
 * @param[in,out] handle  Pointer to the manager structure.
 */
void usb_ringbuf_drop_all(usb_ringbuf_manager_t *handle);

/**
 * @brief  Retrieves the number of valid data nodes currently in the buffer.
 *
 * @param[in] handle  Pointer to the manager structure.
 * @return The count of occupied nodes waiting to be read.
 */
u8 usb_ringbuf_get_count(usb_ringbuf_manager_t *handle);

/**
 * @brief  Checks if the ring buffer is empty.
 *
 * @param[in] handle  Pointer to the manager structure.
 * @return
 *   - 1: Buffer is empty (Head == Tail).
 *   - 0: Buffer contains data.
 */
int usb_ringbuf_is_empty(usb_ringbuf_manager_t *handle);

/**
 * @brief  Checks if the ring buffer is full.
 *
 * @param[in] handle  Pointer to the manager structure.
 * @return
 *   - 1: Buffer is full.
 *   - 0: Buffer has space for new nodes.
 */
int usb_ringbuf_is_full(usb_ringbuf_manager_t *handle);

/**
 * @brief  Writes a data packet to the tail of the buffer.
 *
 * Copies data from the source buffer into the current write slot (tail) and advances
 * the tail pointer.
 *
 * @param[in,out] handle  Pointer to the manager structure.
 * @param[in]     buf     Pointer to the source data to copy.
 * @param[in]     size    Length of the data to copy (must be <= node_size).
 * @param[in]     valid   Validity flag to set for this node (1 = Valid, 0 = Invalid).
 *
 * @return
 *   - 0: Success.
 *   - Non-zero: Error (e.g., buffer is full).
 */
int usb_ringbuf_add_tail(usb_ringbuf_manager_t *handle, const u8 *buf, u32 size, u8 valid);

/**
 * @brief  Reads and removes a data packet from the head of the buffer.
 *
 * Copies data from the current read slot (head) into the destination buffer and
 * advances the head pointer.
 *
 * @param[in,out] handle  Pointer to the manager structure.
 * @param[out]    buffer  Pointer to the destination buffer where data will be copied.
 * @param[in]     size    Maximum capacity of the destination buffer (safety check).
 * @param[out]    valid   Pointer to store the validity flag of the retrieved node (optional, can be NULL).
 *
 * @return The actual number of bytes read/copied. Returns 0 if buffer was empty.
 */
u32 usb_ringbuf_remove_head(usb_ringbuf_manager_t *handle, u8 *buffer, u32 size, u8 *valid);

/** @} End of USB_Common_Ringbuf_Functions group */
/** @} End of USB_Common_Functions group */

#endif /* CONFIG_NON_OS */

/** @} End of USB_Common_API group */

#ifdef __cplusplus
}
#endif

#endif /* USB_RINGBUF_H */
