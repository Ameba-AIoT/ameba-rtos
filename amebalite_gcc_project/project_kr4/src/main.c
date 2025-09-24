#include "ameba_soc.h"
#include "main.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
#include "vfs.h"
#include <mbedtls/platform.h>
#endif
#include "os_wrapper.h"

#ifdef CONFIG_MBEDTLS_ENABLED
#include "ssl_rom_to_ram_map.h"
#endif
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
	ftl_phy_page_num = (ftl_end_addr - ftl_start_addr + 1) / PAGE_SIZE_4K;
	ftl_init(ftl_phy_page_start_addr, ftl_phy_page_num);
}
#endif


void app_init_debug(void)
{
	u32 debug[4];

	debug[LEVEL_ERROR] = 0xFFFFFFFF;
	debug[LEVEL_WARN]  = 0x0;
	debug[LEVEL_TRACE] = 0x0;

	if (SYSCFG_OTP_DisBootLog()) {
		debug[LEVEL_INFO] = 0;
	} else {
		debug[LEVEL_INFO] = BIT(MODULE_BOOT);    // | BIT(MODULE_OTP);
	}

	LOG_MASK(LEVEL_ERROR, debug[LEVEL_ERROR]);
	LOG_MASK(LEVEL_WARN, debug[LEVEL_WARN]);
	LOG_MASK(LEVEL_INFO, debug[LEVEL_INFO]);
	LOG_MASK(LEVEL_TRACE, debug[LEVEL_TRACE]);
}

#ifdef CONFIG_MBEDTLS_ENABLED
void app_mbedtls_rom_init(void)
{
	CRYPTO_Init(NULL);
	CRYPTO_SHA_Init(NULL);
	RCC_PeriphClockCmd(APBPeriph_ECDSA, APBPeriph_ECDSA_CLOCK, ENABLE);
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))rtos_mem_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))rtos_mem_free;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
}
#endif

void app_pmu_init(void)
{
	DBG_INFO_MSG_ON(MODULE_KR4);
	pmu_acquire_wakelock(PMU_OS);

	if (LSYS_GET_KR4_IS_NP(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1))) {
		pmu_acquire_wakelock(PMU_KM4_RUN);
		if ((SWR_CORE_Vol_Get() == CORE_VOL_1P0) && (dsp_status_on())) {
			pmu_acquire_wakelock(PMU_DSP_RUN);
		}
	}
}

void app_IWDG_refresh(void *arg)
{
	UNUSED(arg);
	WDG_Refresh(IWDG_DEV);
}

void app_IWDG_int(void)
{
	/*usually IWDG will enable by HW, and the bark interval is 4S
	 * Due to inaccurate of Aon clk, IWDG should be refreshed
	 * every 2S with  the lowest priority level thread.
	*/
	u32 Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_FEN);
	if ((Temp & APBPeriph_IWDG) == 0) {
		return;
	} else {
		RTK_LOGI(TAG, "IWDG refresh on!\n");

		rtos_timer_t xTimer = NULL;

		/* Writing to FLASH during OTA makes SYSTICK stop and run frequently,
		and SYSTICK may be 3-4 times slower than expected, so reduce refresh period to 1/4 of 2s. */
		rtos_timer_create(&xTimer, "WDG_Timer", NULL, 500, TRUE, app_IWDG_refresh);

		if (xTimer == NULL) {
			RTK_LOGE(TAG, "IWDG refresh error\n");
		} else {
			rtos_timer_start(xTimer, 0);
		}
	}
}

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
			RTK_LOGI(TAG, "File System Init Success\n");
			return;
		}
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
	InterruptRegister(IPC_INTHandler, IPC_KR4_IRQ, (u32)IPCKR4_DEV, INT_PRI3);
	InterruptEn(IPC_KR4_IRQ, INT_PRI3);

#ifdef CONFIG_MBEDTLS_ENABLED
	app_mbedtls_rom_init();
#endif

	/*IPC table initialization*/
	ipc_table_init(IPCKR4_DEV);

	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);

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

//only NP can init FW
#ifdef CONFIG_WLAN
#if !defined(CONFIG_MP_SHRINK) && ((defined(CONFIG_WHC_DEV) && CONFIG_WHC_DEV) || (defined(CONFIG_WHC_NONE) && CONFIG_WHC_NONE))
	wififw_task_create();
#endif

	wifi_init();
#endif

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	app_init_debug();

	app_pmu_init();

	app_IWDG_int();

	rtk_diag_init(RTK_DIAG_HEAP_SIZE, RTK_DIAG_SEND_BUFFER_SIZE);

	/* Execute application example */
	app_example();

	RTK_LOGI(TAG, "KR4 OS START \n");

	//Enable Schedule
	rtos_sched_start();
}

