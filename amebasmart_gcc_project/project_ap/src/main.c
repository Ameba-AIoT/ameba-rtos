
/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.  All Rights Reserved.
 */

/* Standard includes. */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "main.h"

/* Scheduler includes. */

#include "ameba_soc.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
#include "vfs.h"
#endif
#include "os_wrapper.h"
#include "ssl_rom_to_ram_map.h"
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif
#include "ameba_diagnose.h"

static const char *const TAG = "MAIN";

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


void app_mbedtls_rom_init(void)
{
	CRYPTO_Init(NULL);
	CRYPTO_SHA_Init(NULL);
	RCC_PeriphClockCmd(APBPeriph_RSA, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_ECDSA, APBPeriph_ECDSA_CLOCK, ENABLE);
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))rtos_mem_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))rtos_mem_free;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
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
extern void wifi_init(void);
#endif

extern int rt_kv_init(void);

void app_filesystem_init(void)
{
#if !(defined(CONFIG_MP_SHRINK)) && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
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
}

u32 app_uart_rx_pin_wake_int_handler(void *data)
{
	GPIO_InitTypeDef *GPIO_InitStruct = (GPIO_InitTypeDef *)data;
	/*clear edge interrupt*/
	GPIO_INTConfig(GPIO_InitStruct->GPIO_Pin, DISABLE);
	/*Keep the AP active for 5 seconds */
	pmu_set_sysactive_time(5000);

	return 0;
}
void app_uart_rx_pin_wake_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {
		.GPIO_Pin = UART_LOG_RXD,	/*PB_23*/
		.GPIO_PuPd = GPIO_PuPd_UP,
		.GPIO_Mode = GPIO_Mode_INT,
		.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE,
		.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW,
	};
	GPIO_INTConfig(UART_LOG_RXD, DISABLE);
	GPIO_Direction(GPIO_InitStruct.GPIO_Pin, GPIO_Mode_IN);
	PAD_PullCtrl(GPIO_InitStruct.GPIO_Pin, GPIO_InitStruct.GPIO_PuPd);

	GPIO_INTMode(GPIO_InitStruct.GPIO_Pin, ENABLE, GPIO_InitStruct.GPIO_ITTrigger,
				 GPIO_InitStruct.GPIO_ITPolarity, GPIO_InitStruct.GPIO_ITDebounce);
	InterruptRegister(GPIO_INTHandler, GPIOB_IRQ, (u32)GPIOB_BASE, 3);
	InterruptEn(GPIOB_IRQ, 3);
	GPIO_UserRegIrq(UART_LOG_RXD, app_uart_rx_pin_wake_int_handler, &GPIO_InitStruct);
}

void app_pmu_init(void)
{
	pmu_set_sleep_type(SLEEP_PG);
	pmu_acquire_deepwakelock(PMU_OS);
	/*Init logUart rx pin for gpio wakeup*/
	app_uart_rx_pin_wake_init();
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

#ifdef CONFIG_MBEDTLS_ENABLED
	app_mbedtls_rom_init();
#endif

	ipc_table_init(IPCAP_DEV);

	app_pmu_init();

	app_filesystem_init();

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	app_ftl_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif

	/* wifi init*/
#ifdef CONFIG_WLAN
	wifi_init();
#endif

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	rtk_diag_init(RTK_DIAG_HEAP_SIZE, RTK_DIAG_SEND_BUFFER_SIZE);

	/* Execute application example */
	app_example();

	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
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

