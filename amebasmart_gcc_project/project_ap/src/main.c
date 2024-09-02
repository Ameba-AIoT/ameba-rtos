
/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.  All Rights Reserved.
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* Scheduler includes. */

#include "ameba_soc.h"
#include "vfs.h"
#include "os_wrapper.h"

static const char *TAG = "MAIN";
u32 use_hw_crypto_func;

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
#include "ftl_int.h"

void app_ftl_init(void)
{
	u32 ftl_start_addr, ftl_end_addr;

	flash_get_layout_info(FTL, &ftl_start_addr, &ftl_end_addr);
	ftl_phy_page_start_addr = ftl_start_addr - SPI_FLASH_BASE;
	if (SYSCFG_BootFromNor()) {
		ftl_phy_page_num = (ftl_end_addr - ftl_start_addr) / PAGE_SIZE_4K;
	} else {
		ftl_phy_page_num = (ftl_end_addr - ftl_start_addr) / PAGE_SIZE_2K;
	}

	ftl_init(ftl_phy_page_start_addr, ftl_phy_page_num);
}
#endif


static void *app_mbedtls_calloc_func(size_t nelements, size_t elementSize)
{
	size_t size;
	void *ptr = NULL;
	size = nelements * elementSize;
	ptr = rtos_mem_malloc(size);
	if (ptr) {
		memset(ptr, 0, size);
	}
	return ptr;
}

static void app_mbedtls_free_func(void *buf)
{
	rtos_mem_free(buf);
}

void app_mbedtls_rom_init(void)
{
	mbedtls_platform_set_calloc_free(app_mbedtls_calloc_func, app_mbedtls_free_func);
	use_hw_crypto_func = 0;
	//rtl_cryptoEngine_init();
}
/*
 * This function will be replaced when Sdk example is compiled using CMD "make all EXAMPLE=xxx" or "make xip EXAMPLE=xxx"
 * To aviod compile error when example is not compiled
 */
_WEAK void app_pre_example(void)
{


}

_WEAK void app_example(void)
{


}

#ifdef CONFIG_WLAN
extern void wlan_initialize(void);
#endif

extern int rt_kv_init(void);

void app_filesystem_init(void)
{
#if defined(CONFIG_AS_INIC_AP)
	int ret = 0;
	vfs_init();
#ifdef CONFIG_FATFS_WITHIN_APP_IMG
	ret = vfs_user_register("fat", VFS_FATFS, VFS_INF_FLASH, VFS_REGION_2, VFS_RO);
	if (ret == 0) {
		RTK_LOGI(TAG, "VFS-FAT Init Success \n");
	} else {
		RTK_LOGI(TAG, "VFS-FAT Init Fail \n");
	}
#endif

	ret = vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	if (ret == 0) {
		ret = rt_kv_init();
		if (ret == 0) {
			RTK_LOGI(TAG, "File System Init Success \n");
			return;
		}
	}

	RTK_LOGE(TAG, "File System Init Fail \n");
#endif
	return;
}

void app_pmu_init(void)
{
	pmu_set_sleep_type(SLEEP_PG);
	pmu_acquire_deepwakelock(PMU_OS);
}

/*
 * Starts all the other tasks, then starts the scheduler.
 */
int main(void)
{
	RTK_LOGW(TAG, "************************** WARNING ***********************\n");
	RTK_LOGE(TAG, "Please read the <Cache Consistency When Using DMA> chapter in the Application_Note.pdf\n");
	RTK_LOGE(TAG, "Here is some brief of the chapter and can be removed in main.c\n");
	RTK_LOGE(TAG, "Cortex-A32 will automaticly prefetch data to D-cache even the address is not read/write!\n");
	RTK_LOGE(TAG, "DCache_Invalidate(0xFFFFFFFF, 0xFFFFFFFF) is same as DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF) in CA32\n");
	RTK_LOGE(TAG, "Invalidate by an address is only performed when the data is not dirty, ortherwise a clean is performed!\n");
	RTK_LOGE(TAG, "refer to TRM of CA32 about 'data prefetching' and 'Invalidating or cleaning a cache'.\n");
	RTK_LOGW(TAG, "**********************************************************\n");

	/* Create task here */
	InterruptRegister(IPC_INTHandler, IPC_AP_IRQ, (u32)IPCAP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_AP_IRQ, INT_PRI_MIDDLE);

#ifdef CONFIG_MBED_TLS_ENABLED
	app_mbedtls_rom_init();
#endif

	ipc_table_init(IPCAP_DEV);

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	app_pmu_init();

#ifndef CONFIG_MP_INCLUDED
	app_filesystem_init();
#endif


#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	app_ftl_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

	/* wifi init*/
#ifdef CONFIG_WLAN
	wlan_initialize();
#endif

	/* Execute application example */
	app_example();

	IPC_SEMDelayStub(&rtos_time_delay_ms);

	/* Start the tasks and timer running. */
	RTK_LOGI(TAG, "Cortex-A Start Scheduler\n");
	rtos_sched_start();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details.  */
	for (;;);
}

