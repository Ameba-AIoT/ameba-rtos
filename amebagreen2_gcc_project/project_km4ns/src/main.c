#include "ameba_soc.h"
#include "main.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE || defined CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
#include "vfs.h"
#endif
#include "os_wrapper.h"
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif
#include "ameba_diagnose.h"

static const char *const TAG = "MAIN";
u32 use_hw_crypto_func;

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
#include "ftl_int.h"

void app_ftl_init(void)
{
	u32 ftl_start_addr, ftl_end_addr;

	flash_get_layout_info(FTL, &ftl_start_addr, &ftl_end_addr);
	ftl_phy_page_start_addr = ftl_start_addr - SPI_FLASH_BASE;
	ftl_phy_page_num = (ftl_end_addr - ftl_start_addr + 1) / PAGE_SIZE_4K;
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
	use_hw_crypto_func = 1;
}

void app_pmu_init(void)
{
	pmu_acquire_wakelock(PMU_OS);
	pmu_init_wakeup_timer();
	/* If the current cpu is np, need to hold the lock of another cpu */
	if ((HAL_READ32(PMC_BASE, SYSPMC_CTRL) & PMC_BIT_CPU_IS_AP) == 0) {
		pmu_acquire_wakelock(PMU_CPU0_RUN);
	}
}

void app_IWDG_refresh(void *arg)
{
	UNUSED(arg);
	WDG_Refresh(IWDG_DEV);
}

void app_IWDG_init(void)
{
	/* usually IWDG will enable by HW, and the bark interval is 4095ms by default */
	if (0 == (HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_FEN) & APBPeriph_IWDG)) {
		return;
	}

	IWDG_LP_Enable(IWDG_DEV, DISABLE);
	/*set IWDG timeout to 4s*/
	WDG_Timeout(IWDG_DEV, 0x0FFF);
	WDG_Refresh(IWDG_DEV);
	RTK_LOGI(TAG, "IWDG refresh thread Started!\n");

	/* Due to inaccurate of Aon clk(50% precision), IWDG should be refreshed every 2S with the lowest priority level thread. */
	/* Writing to FLASH during OTA makes SYSTICK stop and run frequently,
	and SYSTICK may be 3-4 times slower than expected, so reduce refresh period to 1/4 of 2s. */
	rtos_timer_t xTimer = NULL;
	rtos_timer_create(&xTimer, "WDG_Timer", NULL, 500, TRUE, app_IWDG_refresh);

	if (xTimer == NULL) {
		RTK_LOGE(TAG, "IWDG refresh timer create error\n");
	} else {
		rtos_timer_start(xTimer, 0);
	}
}

extern int rt_kv_init(void);

void app_filesystem_init(void)
{
#if !(defined(CONFIG_MP_SHRINK)) && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE || defined CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
	int ret = 0;
	vfs_init();
#ifdef CONFIG_FATFS_WITHIN_APP_IMG
	ret = vfs_user_register(VFS_R3_PREFIX, VFS_FATFS, VFS_INF_FLASH, VFS_REGION_3, VFS_RO);
	if (ret == 0) {
		RTK_LOGI(TAG, "VFS-FAT Init Success \n");
	} else {
		RTK_LOGI(TAG, "VFS-FAT Init Fail \n");
	}
#endif

	vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	ret = rt_kv_init();
	if (ret == 0) {
		RTK_LOGI(TAG, "File System Init Success \n");
		return;
	}


	RTK_LOGE(TAG, "File System Init Fail \n");
#endif
}

/*
 * This function will be replaced when Sdk example is compiled using CMD "make EXAMPLE=xxx" or "make xip xxx"
 * To aviod compile error when example is not compiled
 */
_WEAK void app_pre_example(void)
{


}

_WEAK void app_example(void)
{


}

//default main
int main(void)
{
	/*IPC table initialization*/
	ipc_table_init(IPCNP_DEV);
	InterruptRegister(IPC_INTHandler, IPC_CPU1_IRQ, (u32)IPCNP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_CPU1_IRQ, INT_PRI_MIDDLE);

	app_pmu_init();

#ifdef CONFIG_MBEDTLS_ENABLED
	app_mbedtls_rom_init();
#endif

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

//only KM4NS can init FW
#if defined(CONFIG_WIFI_FW_EN) && defined(CONFIG_FW_DRIVER_COEXIST)
	wififw_task_create();
#endif

#ifdef CONFIG_WLAN
	wifi_init();
#endif

	/* init console */
	shell_init_rom(0, NULL);
	shell_init_ram();
	/* Register Log Uart Callback function */
	InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(UART_LOG_IRQ, INT_PRI_LOWEST);
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_NP, ENABLE);

	app_IWDG_init();

	rtk_diag_init(RTK_DIAG_HEAP_SIZE, RTK_DIAG_SEND_BUFFER_SIZE);

	/* Execute application example */
	app_example();

	RTK_LOGI(TAG, "NP OS START \n");

	/* Set delay function & critical function for hw ipc sema */
	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
	IPC_SEMDelayStub(rtos_time_delay_ms);

	//Enable Schedule
	rtos_sched_start();
}

