
#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"
#include "ameba_rtos_version.h"
#include "ssl_rom_to_ram_map.h"
#if defined(CONFIG_VFS_ENABLED)
#include "vfs.h"
#endif
#ifdef CONFIG_MBEDTLS_ENABLED
#include "threading_alt.h"
#endif
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif

//#include "wifi_fast_connect.h"
static const char *const TAG = "MAIN";
u32 use_hw_crypto_func;

#if (defined(CONFIG_BT) && CONFIG_BT) && (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
#include "bt_inic.h"
#endif
#if defined(CONFIG_SDN_HOST) && CONFIG_SDN_HOST
extern void sdn_host_init(void);
#endif
void app_fullmac_init(void)
{

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

void app_mbedtls_rom_init(void)
{
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))rtos_mem_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))rtos_mem_free;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
#if defined(CONFIG_MBEDTLS_THREADING)
	mbedtls_threading_init();
#endif
	/* mbedtls calls into the HW crypto engine with interrupts disabled via the
	 * IPC semaphore, so cache inconsistency cannot occur; suppress the
	 * cache-misalignment warning log.
	 */
	extern u32 crypto_disable_cache_warning_tz;
	crypto_disable_cache_warning_tz = 1;
}

void app_pmu_init(void)
{

	SOCPS_SleepInit();
	pmu_init_wakeup_timer();

	/* only one core in fullmac mode */
#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	/* If the current cpu is np, need to hold the lock of another cpu */
	if (SOCPS_CPURoleGetEntry() == 0) {
		pmu_acquire_wakelock(PMU_CPU1_RUN);
	}
#endif
}

void app_solo_config(void)
{
	/* register NMI */
	NMI_Handler_set(SOLO_Plfm1ResetHandler);

	/* register rxi intr handler */
	InterruptRegister((IRQ_FUN) SOLO_Rxi300TimoutHandler, RXI300_IRQ, (u32)NULL, INT_PRI_HIGHEST);
	InterruptEn(RXI300_IRQ, INT_PRI_HIGHEST);
}

#if defined(CONFIG_VFS_ENABLED)
extern uint32_t vfs_ftl_init(void);
extern int vfs_kv_init(void);
void app_filesystem_init(void)
{
	int ret = 0;
	vfs_init();

	vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	ret = vfs_kv_init();
	if (ret == 0) {
		RTK_LOGI(TAG, "File System Init Success \n");
	} else {
		RTK_LOGE(TAG, "File System Init Fail \n");
	}

#ifdef CONFIG_FATFS_WITHIN_APP_IMG
	ret = vfs_user_register(VFS_R3_PREFIX, VFS_FATFS, VFS_INF_FLASH, VFS_REGION_3, VFS_RO);
	if (ret == 0) {
		RTK_LOGI(TAG, "VFS-FAT Init Success \n");
	} else {
		RTK_LOGI(TAG, "VFS-FAT Init Fail \n");
	}
#endif

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	vfs_ftl_init();
#endif
}
#endif

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

void app_rtc_init(void)
{
#ifdef RTL8720F_TODO
#endif
}

void CPU1_WDG_RST_Handler(void)
{
	/* Let NP run */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_CPU1_RUN);

	/* clear CPU1_WDG_RST intr*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW, AON_BIT_RSTF_WDG0_CPU);
}

//default main
int main(void)
{
	RTK_LOGI(TAG, "KM4TZ MAIN \n");
	ameba_rtos_get_version();

#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	app_fullmac_init();
	app_IWDG_init();
#else
	/*IPC table initialization*/
	ipc_table_init(IPCAP_DEV);
	InterruptRegister(IPC_INTHandler, IPC_KM4TZ_IRQ, (u32)IPCAP_DEV, INT_PRI5);
	InterruptEn(IPC_KM4TZ_IRQ, INT_PRI5);
#endif

#if defined(CONFIG_MBEDTLS_ENABLED)
	app_mbedtls_rom_init();
#endif

#if defined(CONFIG_VFS_ENABLED)
	app_filesystem_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif

#if defined(CONFIG_WIFI_FW_EN) && defined(CONFIG_FW_DRIVER_COEXIST)
	wififw_task_create();
#endif

#ifdef CONFIG_ETHERNET
	ethernet_mii_init();
#endif

#ifdef CONFIG_WLAN
	wifi_init();
#endif

	/* initialize BT iNIC */
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC)
	bt_inic_init();
#endif
#ifdef CONFIG_SDN_HOST
	sdn_host_init();
#endif

#ifdef CONFIG_SOLO
	app_solo_config();
#endif

#ifdef CONFIG_SHELL
	/* init console */
	shell_init_rom(0, NULL);
	shell_init_ram();
#endif

	app_pmu_init();
	app_rtc_init();

	/* Register CPU1_WDG_RST_IRQ Callback function */
	InterruptRegister((IRQ_FUN) CPU1_WDG_RST_Handler, KM4TZ_NS_WDG_IRQ, (u32)NULL, INT_PRI_LOWEST);//KM4TZ_S_WDG_IRQ
	InterruptEn(KM4TZ_NS_WDG_IRQ, INT_PRI_LOWEST);//KM4TZ_S_WDG_IRQ

#ifdef CONFIG_SHELL
#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	/* Register Log Uart Callback function */
	InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(UART_LOG_IRQ, INT_PRI_LOWEST);
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_AP, ENABLE);
#endif
#endif
	/* Execute application example */
	app_example();

	/* Set delay function for hw ipc sema */
	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit, RTOS_CRITICAL_SEMA);
	IPC_SEMDelayStub(rtos_time_delay_ms);

	RTK_LOGI(TAG, "KM4TZ START SCHEDULER \n");

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}
