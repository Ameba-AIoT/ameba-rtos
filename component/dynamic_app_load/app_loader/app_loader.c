/**
 ******************************************************************************
 * @file    app_loader.c
 * @brief   Contains function for loading the app and setting up the freertos
 *          task
 ******************************************************************************
 * @attention
 *
 * LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Rohit Gujarathi rgujju.github.io
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "ameba.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h> //TODO: Replace with a minimal libc implementation
#include <stdlib.h> //TODO: Replace with a minimal libc implementation
#include "app_loader.h"
#include "syscall.h"

typedef void (* func_t)(void);

static const char *TAG = "APP_LOADER";

// TODO Add Tests:
// 2) Check name length =0,1,15,16,and greater, Check if it is 0s after name length
// 3) Check little endian/ big endian or whatever
// 4) malloc fails
// 5) app loading fails in xtaskCreateStatic
// 6) tinf->bin should fail

#define DEFAULT_STACK_SIZE 10

int LoadAppFromRAM(const uint8_t *tinf_img, int tinf_img_size, void *apps_info, int apps_info_size)
{
	if (apps_info == NULL || apps_info_size <= 0 || tinf_img == NULL || tinf_img_size <= 0) {
		RTK_LOGE(TAG, "params invalid!\n");
		return APP_INVALID;
	}

	DCache_CleanInvalidate((u32)tinf_img, tinf_img_size);

	tinf_t *tinf = (tinf_t *)tinf_img;
	uint32_t dummy_sys_addr = DUMMY_SYS_ADDR;

	uint32_t *data_base = tinf->bin + tinf->text_size;
	RTK_LOGI(TAG, "App data_base: 0x%08X\n", data_base);
	if (memcmp(tinf->magic, "TINF", 4) == 0 && memcmp(data_base, &dummy_sys_addr, 4) == 0) {
		uint32_t app_data_size = tinf->data_size + tinf->got_entries + tinf->bss_size;
		StackType_t *app_data_base = data_base;
		// uint32_t* app_stack_base = app_data_base+app_data_size;
		uint32_t *app_got_base = app_data_base + tinf->data_size;
		if (app_data_size != 0) {
			if (tinf->data_size > 0) {
				*app_data_base = (uintptr_t)&sys;

				memset(apps_info, 0x0, apps_info_size);
				memcpy(apps_info, app_data_base + 1, apps_info_size);
				DCache_CleanInvalidate((u32)apps_info, apps_info_size);

				apps_common_magic_t *magic = (apps_common_magic_t *)apps_info;

				// relocation
				magic->got_base_addr = (uint32_t)app_got_base;
				RTK_LOGI(TAG, "App sys_struct: 0x%08X\n", *app_data_base);
				RTK_LOGI(TAG, "App apps_info->magic: %s\n", magic->magic);

				if (strncmp((const char *)magic->got_base_addr, "appinfo", 8) != 0) {
					RTK_LOGE(TAG, "App magic not match!!\n");
					// free(apps_info);
					return APP_INVALID;
				}

				uint32_t offset = (sizeof(apps_common_magic_t) - sizeof(uint32_t)) / sizeof(uint32_t);
				uint32_t api_entries = (apps_info_size - offset * sizeof(uint32_t)) / sizeof(uint32_t);
				RTK_LOGI(TAG, "App api_entries: %d\n", api_entries);
				for (int i = 0; i < (int)api_entries; i++) {
					*((uint32_t *)apps_info + offset + i) = *((uint32_t *)apps_info + offset + i) + (uintptr_t)(tinf->bin);
				}
				RTK_LOGI(TAG, "App apps_info->got_base_addr: 0x%08X\n", magic->got_base_addr);
				// RTK_LOGI(TAG, "App apps_info->test_socket: 0x%08X\n", apps_info->test_socket);
				// RTK_LOGI(TAG, "App apps_info->test_pthread: 0x%08X\n", apps_info->test_pthread);
			}
			if (tinf->got_entries > 0) {
				uint32_t *got_entries_base = data_base + tinf->data_size;
				uint32_t data_offset = 0x10000000;
				for (uint8_t i = 0; i < tinf->got_entries; i++) {
					if (*(got_entries_base + i) >= data_offset) {
						// If the value is greater than data_offset then it is in the RAM section.
						// So add the app_data_base to it.
						*(app_got_base + i) = ((*(got_entries_base + i)) - data_offset) + (uint32_t)((uintptr_t)app_data_base);
						RTK_LOGI(TAG, "RAM   app_got_base[%d]: 0x%08X\n", i, *(app_got_base + i));
					} else {
						// else it is a relocation in the flash
						// so add the flash->bin ie the base of the app in the flash to it
						*(app_got_base + i) = (*(got_entries_base + i)) + (uint32_t)((uintptr_t)tinf->bin);
						RTK_LOGI(TAG, "FLASH app_got_base[%d]: 0x%08X\n", i, *(app_got_base + i));
					}
				}
			}
			if (tinf->bss_size > 0) {
				// Set BSS section to 0
				memset(app_data_base + (tinf->data_size + tinf->got_entries), 0, tinf->bss_size * 4);
				RTK_LOGI(TAG, "Data at bss section (RAM): 0x%08X\n", *(uint32_t *)(app_data_base + (tinf->data_size + tinf->got_entries)));
			}
		}

		__ISB();

		return APP_OK;
	} else {
		// If check fails then it probably is not an app or it is corrupted
		// Invalid app
		return APP_INVALID;
	}
}

int LoadApp(const uint8_t *tinf_img, int tinf_img_size, void *apps_info, int apps_info_size)
{
	// apps_1_struct_t *apps_info;
	if (apps_info == NULL || apps_info_size <= 0 || tinf_img == NULL || tinf_img_size <= 0) {
		RTK_LOGE(TAG, "params invalid!\n");
		return APP_INVALID;
	}

	DCache_CleanInvalidate((u32)tinf_img, tinf_img_size);

	uint32_t data_offset = 0x10000000;
	uint32_t text_offset = 0xF0000000;

	tinf_t *tinf = (tinf_t *)tinf_img;
	uint32_t dummy_sys_addr = DUMMY_SYS_ADDR;
	// Check if it is actually an APP
	// Check the magic, ie start of the header contains 'TINF'
	// and start of the binary part after header contains DUMMY_SYS_ADDR
	// DUMMY_SYS_ADDR is placed at the start using the application linker script
	uint32_t *rel_data_base = tinf->bin + tinf->text_size;
	uint32_t *data_base = rel_data_base + tinf->rel_data_size;
	RTK_LOGI(TAG, "App bin: 0x%08X\n", tinf->bin);
	RTK_LOGI(TAG, "App text size: %lu 32 bit word\n", tinf->text_size);
	RTK_LOGI(TAG, "App rel_data_base: 0x%08X\n", rel_data_base);
	RTK_LOGI(TAG, "App data_base: 0x%08X\n", data_base);
	if (memcmp(tinf->magic, "TINF", 4) == 0 && memcmp(data_base, &dummy_sys_addr, 4) == 0) {
		// Valid TINF Format app
		RTK_LOGI(TAG, "Loading app: %s %lu.%lu\n", tinf->app_name, tinf->major_version, tinf->minor_version);
		RTK_LOGI(TAG, "App text size: %lu 32 bit word\n", tinf->text_size);
		RTK_LOGI(TAG, "App rel data size: %lu 32 bit word\n", tinf->rel_data_size);
		RTK_LOGI(TAG, "App data size: %lu 32 bit word\n", tinf->data_size);
		RTK_LOGI(TAG, "App bss size: %lu 32 bit word\n", tinf->bss_size);
		RTK_LOGI(TAG, "App GOT entries: %ld\n", tinf->got_entries);
		// Allocate memory for data and bss section of the app on the heap
		uint32_t app_data_size = tinf->rel_data_size + tinf->data_size + tinf->got_entries + tinf->bss_size;
		// TODO: Add the size of the stack actually required by the app, currently hardcoded to DEFAULT_STACK_SIZE words, change in the xTaskCreate API also
		RTK_LOGI(TAG, "Allocating app memory of %ld bytes\n", app_data_size * 4);
		StackType_t *app_rel_data_base = malloc((app_data_size + DEFAULT_STACK_SIZE) * 4);
		if (app_rel_data_base == NULL) {
			return APP_OOM;
		}
		memset(app_rel_data_base, 0x0, (app_data_size + DEFAULT_STACK_SIZE) * 4);
		uint32_t *app_data_base = app_rel_data_base + tinf->rel_data_size;
		// app_stack_base is the address of the base of the stack used by
		// the rtos task
		uint32_t *app_stack_base = app_data_base + app_data_size;
		// app_got_base is the value which will get loaded to r9
		// This is address of the base of GOT which will actually be used
		// by the app for global data accesses
		// This value is passed as the parameter to the app_main. The app will
		// copy the this value from r0 to r9 register
		uint32_t *app_got_base = app_data_base + tinf->data_size;
		RTK_LOGI(TAG, "app_data_base: 0x%08X\n", app_data_base);
		RTK_LOGI(TAG, "app_stack_base: 0x%08X\n", app_stack_base);
		RTK_LOGI(TAG, "app_got_base: 0x%08X\n", app_got_base);

		// Layout in RAM:
		// low memory (0x200014a0)                                  high memory (0x200014cc)
		// |<--------------------- app_data_size ---------------------------------------------------->|
		// |<--- tinf->rel_data_size --->|<-- tinf->data_size -->|<-- got_entries -->|<-- bss_size -->|<-- stack_size -->|
		// +-----------------------------+-----------------------+-------------------+----------------+------------------+
		// | .data.rel                   | .data                 | .got              | .bss           | APP STACK        |
		// +-----------------------------+-----------------------+-------------------+----------------+------------------+
		//                               ^                       ^                                    ^
		//                               app_data_base           app_got_base                         app_stack_base

		if (app_data_size != 0) {
			if (tinf->rel_data_size > 0) {
				for (int i = 0; i < (int)tinf->rel_data_size; i++) {
					if (*(rel_data_base + i) >= text_offset) {
						*(app_rel_data_base + i) = (*(rel_data_base + i) - text_offset) + (uint32_t)((uintptr_t)tinf->bin);
					} else {
						*(app_rel_data_base + i) = *(rel_data_base + i);
					}

					RTK_LOGD(TAG, "rel_data_base[%d]: 0x%08X, app_rel_data_base[%d]: 0x%08X\n", i, *(rel_data_base + i), i, *(app_rel_data_base + i));
				}

			}
			if (tinf->data_size > 0) {
				// Copy data section from flash to the RAM we allocated above
				memcpy(app_data_base, data_base, (tinf->data_size * 4));

				RTK_LOGI(TAG, "Data at data section (flash): 0x%08X\n", *(uint32_t *)(tinf->bin + (tinf->text_size)));
				RTK_LOGI(TAG, "Data at data section (RAM): 0x%08X\n", *(uint32_t *)(app_data_base));
				// Replace the sys_struct address to the one on the mcu
				// The start of the app_data_base will point to start of the data section
				// This is where the sys_struct address was kept by the linker script
				*app_data_base = (uintptr_t)&sys;

				memset(apps_info, 0x0, apps_info_size);
				memcpy(apps_info, app_data_base + 1, apps_info_size);
				DCache_CleanInvalidate((u32)apps_info, apps_info_size);

				apps_common_magic_t *magic = (apps_common_magic_t *)apps_info;

				// relocation
				magic->got_base_addr = (uint32_t)app_got_base;
				RTK_LOGI(TAG, "App sys_struct: 0x%08X\n", *app_data_base);
				RTK_LOGI(TAG, "App apps_info->magic: %s\n", magic->magic);

				if (strncmp((const char *)magic->magic, "appinfo", 8) != 0) {
					RTK_LOGE(TAG, "App magic not match!!\n");
					free(app_rel_data_base);
					return APP_INVALID;
				}

				uint32_t offset = (sizeof(apps_common_magic_t) - sizeof(uint32_t)) / sizeof(uint32_t);
				uint32_t api_entries = (apps_info_size - offset * sizeof(uint32_t)) / sizeof(uint32_t);
				RTK_LOGI(TAG, "App api_entries: %d\n", api_entries);
				for (int i = 0; i < (int)api_entries; i++) {
					*((uint32_t *)apps_info + offset + i) = (*((uint32_t *)apps_info + offset + i) - text_offset) + (uintptr_t)(tinf->bin);
				}
				RTK_LOGI(TAG, "App apps_info->got_base_addr: 0x%08X\n", magic->got_base_addr);
				// RTK_LOGI(TAG, "App apps_info->test_socket: 0x%08X\n", apps_info->test_socket);
				// RTK_LOGI(TAG, "App apps_info->test_pthread: 0x%08X\n", apps_info->test_pthread);
			}
			if (tinf->got_entries > 0) {
				// Copy the GOT from flash to RAM

				// app_got_base is the base of the GOT in the RAM
				// this is where GOT will be copied to

				RTK_LOGI(TAG, "GOT in app stack: %p\n", app_got_base);
				// got_entries_base is the base of the GOT in the flash
				// tinf->got_entries number of entries from this location
				// needs to be copied into RAM
				uint32_t *got_entries_base = data_base + tinf->data_size;

				RTK_LOGI(TAG, "got_entries_base: %p\n", got_entries_base);

				// While copying add the base address (app_data_base) in RAM to each element of the GOT
				// TODO: Add more explaination about this
				// Need to subtract the data_offset to get the location with respect to 0
				RTK_LOGI(TAG, "Data offset: 0x%08X\n", data_offset);
				for (int i = 0; i < (int)tinf->got_entries; i++) {
					if (*(got_entries_base + i) >= text_offset) {
						// else it is a relocation in the flash
						// so add the flash->bin ie the base of the app in the flash to it
						*(app_got_base + i) = ((*(got_entries_base + i)) - text_offset) + (uint32_t)((uintptr_t)tinf->bin);
						RTK_LOGD(TAG, "FLASH app_got_base[%d]: 0x%08X\n", i, *(app_got_base + i));
					} else if (*(got_entries_base + i) >= data_offset) {
						// If the value is greater than data_offset then it is in the RAM section.
						// So add the app_data_base to it.
						*(app_got_base + i) = ((*(got_entries_base + i)) - data_offset) + (uint32_t)((uintptr_t)app_rel_data_base);
						RTK_LOGD(TAG, "RAM   app_got_base[%d]: 0x%08X\n", i, *(app_got_base + i));
					} else {
						RTK_LOGD(TAG, "app_got_base[%d]: 0x%08X isn't FLASH or RAM\n", i, *(got_entries_base + i));
					}
				}
			}
			if (tinf->bss_size > 0) {
				// Set BSS section to 0
				memset(app_data_base + (tinf->data_size + tinf->got_entries), 0, tinf->bss_size * 4);
				RTK_LOGI(TAG, "Data at bss section (RAM): 0x%08X\n", *(uint32_t *)(app_data_base + (tinf->data_size)));
			}
			uint32_t *app_stack_got = app_data_base + tinf->data_size;
			uint32_t *got_entries_base = data_base + tinf->data_size;
			for (int i = 0; i < (int)tinf->got_entries; i++) {
				// RTK_LOGI(TAG, "app_stack_got: 0x%08X: 0x%08X: 0x%08X and flash: 0x%08X\n",
				//                 app_stack_got, *app_stack_got, *((uint32_t*)((uintptr_t)(*app_stack_got))), *got_entries_base++);
				RTK_LOGD(TAG, "app_stack_got: 0x%08X: 0x%08X and flash: 0x%08X\n",
						 app_stack_got, *app_stack_got, *got_entries_base++);
				app_stack_got++;
			}
		}

		__ISB();
#if 0
#ifdef CONFIG_ARM_CORE_CA32
		TaskFunction_t AppStart = (TaskFunction_t)(tinf->bin);
		RTK_LOGI(TAG, "App entry point: 0x%08X\n", AppStart);
		TaskFunction_t app_main_orig = (TaskFunction_t)(tinf->bin);
		RTK_LOGI(TAG, "Check entry point: 0x%08X Data: 0x%08lX\n", (uint32_t *)app_main_orig, *(uint32_t *)app_main_orig);
		// TODO: The following line gives segfault for some reason
		RTK_LOGI(TAG, "Data at app entry point: 0x%08X\n", *((uint32_t *)(((uintptr_t)AppStart) & 0xFFFFFFFE)));

		AppStart(app_got_base);
#else
		TaskFunction_t AppStart = (TaskFunction_t)(((uintptr_t)(tinf->bin)) | 1); /* OR'ed with 1 to set the thumb bit */
		RTK_LOGI(TAG, "App entry point: 0x%08X\n", AppStart);
		TaskFunction_t app_main_orig = (TaskFunction_t)(tinf->bin);
		RTK_LOGI(TAG, "Check entry point: 0x%08X Data: 0x%08lX\n", (uint32_t *)app_main_orig, *(uint32_t *)app_main_orig);
		// TODO: The following line gives segfault for some reason
		RTK_LOGI(TAG, "Data at app entry point: 0x%08X\n", *((uint32_t *)(((uintptr_t)AppStart) & 0xFFFFFFFE)));

		AppStart(app_got_base);
#endif
#endif

		return APP_OK;
	} else {
		// If check fails then it probably is not an app or it is corrupted
		// Invalid app
		return APP_INVALID;
	}
}
extern unsigned char __dram_dynamic_app_text_start__[];
extern unsigned char __dram_dynamic_app_text_size__[];
int LoadAppWithName(const char *tinf_img_name, void *apps_info, int apps_info_size)
{
	// apps_1_struct_t *apps_info;
	if (apps_info == NULL || apps_info_size <= 0) {
		RTK_LOGE(TAG, "params invalid!\n");
		return APP_INVALID;
	}

	RTK_LOGI(TAG, "__dram_dynamic_app_text_size__: 0x%X\n", (uint32_t)__dram_dynamic_app_text_size__);

	uint32_t img_start_addr = (uint32_t)__dram_dynamic_app_text_start__;
	uint32_t img_end_addr = (uint32_t)__dram_dynamic_app_text_start__ + (uint32_t)__dram_dynamic_app_text_size__;
	RTK_LOGI(TAG, "img_start_addr: 0x%08X\n", img_start_addr);
	RTK_LOGI(TAG, "img_end_addr: 0x%08X\n", img_end_addr);

	int tinf_img_size = 0;
	uint32_t tinf_img_addr = 0;

	IMAGE_HEADER *img_hdr = (IMAGE_HEADER *)(img_start_addr);
	while ((uint32_t)img_hdr < img_end_addr) {
		if (img_hdr->signature[0] == 0x666E6974 && img_hdr->signature[1] == 0x30707061) {
			tinf_t *tinf = (tinf_t *)((uint32_t)img_hdr + 0x20);
			if (strcmp((const char *)tinf->app_name, (const char *)tinf_img_name) == 0) {
				tinf_img_size = img_hdr->image_size;
				tinf_img_addr = (uint32_t)img_hdr + 0x20;
				RTK_LOGI(TAG, "find target app: %s, addr: 0x%08X, size: %d\r\n", tinf->app_name, tinf_img_addr, tinf_img_size);
				break;
			}
		}
		img_hdr = (IMAGE_HEADER *)((uint32_t)img_hdr + img_hdr->image_size + 0x20);
	}

	if ((uint32_t)img_hdr >= img_end_addr) {
		RTK_LOGI(TAG, "no app binary, target app: %s\r\n", tinf_img_name);
		return APP_INVALID;
	}

	return LoadApp((const uint8_t *)tinf_img_addr, tinf_img_size, apps_info, apps_info_size);
}