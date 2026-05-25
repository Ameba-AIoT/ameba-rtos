/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_MEM_H
#define ZEPHYR_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zephyr_queue.h>
#include <zephyr_lock.h>
#include <zephyr/sys/util.h>
#include <zephyr/toolchain/gcc.h>
#include <zephyr_time.h>

struct k_mem_slab {
	// _wait_q_t wait_q;
	struct k_spinlock lock;
	uint32_t num_blocks;
	size_t block_size;
	char *buffer;
	char *free_list;
	uint32_t num_used;
#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
	uint32_t max_used;
#endif
};

#define Z_MEM_SLAB_INITIALIZER(obj, slab_buffer, slab_block_size, \
			       slab_num_blocks) \
	{ \
	.lock = {}, \
	.num_blocks = slab_num_blocks, \
	.block_size = slab_block_size, \
	.buffer = slab_buffer, \
	.free_list = NULL, \
	.num_used = 0, \
	}

/*
#define K_MEM_SLAB_DEFINE(name, slab_block_size, slab_num_blocks, slab_align) \
	char __noinit_named(k_mem_slab_buf_##name) \
	  __aligned(WB_UP(slab_align)) \
	   _k_mem_slab_buf_##name[(slab_num_blocks) * WB_UP(slab_block_size)]; \
	STRUCT_SECTION_ITERABLE(k_mem_slab, name) = \
		Z_MEM_SLAB_INITIALIZER(name, _k_mem_slab_buf_##name, \
					WB_UP(slab_block_size), slab_num_blocks)
*/
#define K_MEM_SLAB_DEFINE(name, slab_block_size, slab_num_blocks, slab_align) \
	char _k_mem_slab_buf_##name[(slab_num_blocks) * WB_UP(slab_block_size)] __aligned(WB_UP(slab_align)); \
	STRUCT_SECTION_ITERABLE(k_mem_slab, name) = \
		Z_MEM_SLAB_INITIALIZER(name, _k_mem_slab_buf_##name, \
					WB_UP(slab_block_size), slab_num_blocks)

#define K_MEM_SLAB_DEFINE_STATIC K_MEM_SLAB_DEFINE

int k_mem_slab_init(struct k_mem_slab *slab);
int k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout);
void k_mem_slab_free(struct k_mem_slab *slab, void *mem);
uint32_t k_mem_slab_num_free_get(struct k_mem_slab *slab);

struct k_heap {
	char dummy;
};
void *k_malloc(size_t size);
void k_free(void *ptr);
void *k_heap_alloc(struct k_heap *h, size_t bytes, k_timeout_t timeout);
void k_heap_free(struct k_heap *h, void *mem);

#ifdef __cplusplus
}
#endif

#endif
