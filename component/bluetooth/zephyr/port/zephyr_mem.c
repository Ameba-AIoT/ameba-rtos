/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <zephyr_mem.h> 
#include <zephyr_port.h>

void *k_malloc(size_t size)
{
	return osif_mem_alloc(RAM_TYPE_DATA_ON, size);
}

void k_free(void *ptr)
{
	osif_mem_free(ptr);
}

void *k_heap_alloc(struct k_heap *h, size_t bytes, k_timeout_t timeout)
{
	(void)h;
	(void)timeout;

	return osif_mem_alloc(RAM_TYPE_DATA_ON, bytes);
}

void k_heap_free(struct k_heap *h, void *mem)
{
	(void)h;
	osif_mem_free(mem);
}

/**
 * @brief Initialize kernel memory slab subsystem.
 *
 * Perform any initialization of memory slabs that wasn't done at build time.
 * Currently this just involves creating the list of free blocks for each slab.
 *
 * @retval 0 on success.
 * @retval -EINVAL if @p slab contains invalid configuration and/or values.
 */
int k_mem_slab_init(struct k_mem_slab *slab)
{
	uint32_t j;
	char *p;

	/* blocks must be word aligned */
	if (((slab->block_size | (uintptr_t)slab->buffer) &
				(sizeof(void *) - 1)) != 0U) {
		return -EINVAL;
	}

	slab->free_list = NULL;
	p = slab->buffer;

	for (j = 0U; j < slab->num_blocks; j++) {
		*(char **)p = slab->free_list;
		slab->free_list = p;
		p += slab->block_size;
	}
	return 0;
}

int k_mem_slab_alloc(struct k_mem_slab *slab, void **mem, k_timeout_t timeout)
{
	(void)timeout;
	k_spinlock_key_t key = k_spin_lock(&slab->lock);
	int result;

	// SYS_PORT_TRACING_OBJ_FUNC_ENTER(k_mem_slab, alloc, slab, timeout);

	if (slab->free_list != NULL) {
		/* take a free block */
		*mem = slab->free_list;
		slab->free_list = *(char **)(slab->free_list);
		slab->num_used++;

#ifdef CONFIG_MEM_SLAB_TRACE_MAX_UTILIZATION
		slab->max_used = MAX(slab->num_used, slab->max_used);
#endif

		result = 0;
	} else {
        *mem = NULL;
		result = -ENOMEM;
    }

	k_spin_unlock(&slab->lock, key);

	return result;
}

void k_mem_slab_free(struct k_mem_slab *slab, void *mem)
{
	k_spinlock_key_t key = k_spin_lock(&slab->lock);

	*(char **) mem = slab->free_list;
	slab->free_list = (char *) mem;
	slab->num_used--;

	k_spin_unlock(&slab->lock, key);
}

uint32_t k_mem_slab_num_free_get(struct k_mem_slab *slab)
{
	return slab->num_blocks - slab->num_used;
}
