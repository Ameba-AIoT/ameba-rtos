/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba.h"
#include "FreeRTOS.h"

u8 heap_index = 0;
u32 total_heap_size = 0;
//HeapRegion Addresses need to be sorted from smallest to largest
HeapRegion_t xHeapRegions[] = {
	{ NULL, 0 },			// Defines a block in bd_ram,	 start and length will be corrected in os_heap_init()
	{ NULL, 0 },			// Defines a block in sram0,	 start and length will be corrected in os_heap_init()
	{ NULL, 0 },			// Defines a block in sram1, 	 start and length will be corrected in os_heap_init()
	{ NULL, 0 },			// Defines a block in bd_psram,	 start and length will be corrected in os_heap_init()
	{ NULL, 0 },			// Defines a block in psram,	 start and length will be corrected in os_heap_init()
	{ NULL, 0 },			// Terminates the array.
};
static inline void os_heap_sort(void)
{
	int i, j;
	HeapRegion_t temp;

	for (i = 0; i < heap_index - 1; i++) {
		for (j = 0; j < heap_index - i - 1; j++) {
			if (xHeapRegions[j].pucStartAddress > xHeapRegions[j + 1].pucStartAddress) {
				temp = xHeapRegions[j];
				xHeapRegions[j] = xHeapRegions[j + 1];
				xHeapRegions[j + 1] = temp;
			}
		}
	}
}
bool os_heap_add(u8 *start_addr, size_t heap_size)
{
	assert_param(heap_index < sizeof(xHeapRegions) / sizeof(xHeapRegions[0]));

	xHeapRegions[heap_index].pucStartAddress = start_addr;
	xHeapRegions[heap_index].xSizeInBytes = (size_t)heap_size;
	if (xHeapRegions[heap_index].xSizeInBytes != 0) {
		total_heap_size += heap_size;
		heap_index++;
		return TRUE;
	}
	return FALSE;
}

_OPTIMIZE_NONE_
void os_heap_init(void)
{
#ifdef SRAM_HEAP0_START 		// Defines a block in bd_ram
	os_heap_add((uint8_t *)SRAM_HEAP0_START, (size_t)SRAM_HEAP0_SIZE);
#endif

#ifdef SRAM_HEAP1_START			// Defines a block in sram
	os_heap_add((uint8_t *)SRAM_HEAP1_START, (size_t)SRAM_HEAP1_SIZE);
#endif

#ifdef PSRAM_HEAP0_START		// Defines a block in bd_psram
	os_heap_add((uint8_t *)PSRAM_HEAP0_START, (size_t)PSRAM_HEAP0_SIZE);
#endif

#ifdef PSRAM_HEAP1_START		// Defines a block in psram
	os_heap_add((uint8_t *)PSRAM_HEAP1_START, (size_t)PSRAM_HEAP1_SIZE);
#endif

#ifdef SRAM_HEAP2_START 		// Defines a block in bd_ram
	os_heap_add((uint8_t *)SRAM_HEAP2_START, (size_t)SRAM_HEAP2_SIZE);
#endif

	if (heap_index > 0) {
		os_heap_sort();
	}

	vPortDefineHeapRegions(xHeapRegions);
}


