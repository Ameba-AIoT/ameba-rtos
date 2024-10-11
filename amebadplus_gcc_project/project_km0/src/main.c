#include "ameba_soc.h"
#include "main.h"
#include "os_wrapper.h"

static const char *TAG = "MAIN";
u32 use_hw_crypto_func;


void app_init_debug(void)
{
	u32 debug[LEVEL_NUMs];

	debug[LEVEL_ERROR] = 0xFFFFFFFF;
	debug[LEVEL_INFO]  = 0x0;
	debug[LEVEL_WARN]  = 0x0;
	debug[LEVEL_TRACE] = 0x0;

	LOG_MASK(LEVEL_ERROR, debug[LEVEL_ERROR]);
	LOG_MASK(LEVEL_WARN,  debug[LEVEL_WARN]);
	LOG_MASK(LEVEL_INFO,  debug[LEVEL_INFO]);
	LOG_MASK(LEVEL_TRACE, debug[LEVEL_TRACE]);
}

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

void app_pmu_init(void)
{
	DBG_INFO_MSG_ON(MODULE_PMC);

	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_KM4_RUN);
	pmu_init_wakeup_timer();
}

void app_IWDG_refresh(void *arg)
{
	UNUSED(arg);
	WDG_Refresh(IWDG_DEV);
}

void app_IWDG_int(void)
{
	/* usually IWDG will enable by HW, and the bark interval is 4095ms by default */
	if (0 == (HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_FEN) & APBPeriph_IWDG)) {
		return;
	}

	IWDG_LP_Enable(IWDG_DEV, DISABLE);
	RTK_LOGI(TAG, "IWDG refresh on!\n");

	/* Due to inaccurate of Aon clk(50% precision), IWDG should be refreshed every 2S with the lowest priority level thread. */
	/* Writing to FLASH during OTA makes SYSTICK stop and run frequently,
	and SYSTICK may be 3-4 times slower than expected, so reduce refresh period to 1/4 of 2s. */
	rtos_timer_t xTimer = NULL;
	rtos_timer_create(&xTimer, "WDG_Timer", NULL, 500, TRUE, app_IWDG_refresh);

	if (xTimer == NULL) {
		RTK_LOGE(TAG, "IWDG refresh error\n");
	} else {
		rtos_timer_start(xTimer, 0);
	}
}

_WEAK void app_example(void)
{

}

//default main
int main(void)
{
	/* Debug log control */
	app_init_debug();

	InterruptRegister((IRQ_FUN)IPC_INTHandler, IPC_KM0_IRQ, (u32)IPCKM0_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_KM0_IRQ, INT_PRI_MIDDLE);

	/*IPC table initialization*/
	ipc_table_init(IPCKM0_DEV);

	app_pmu_init();
#ifdef CONFIG_MBEDTLS_ENABLED
	app_mbedtls_rom_init();
#endif

#if defined(CONFIG_WIFI_FW_EN) && CONFIG_WIFI_FW_EN
	wififw_task_create();
#endif

#ifdef CONFIG_WLAN
	wlan_initialize();
#endif

	shell_init_rom(0, NULL);
	shell_init_ram();
	/* Register Log Uart Callback function */
	InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(UART_LOG_IRQ, INT_PRI_LOWEST);
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM0, ENABLE);

	app_IWDG_int();

	/* Execute application example */
	app_example();

	IPC_SEMDelay(rtos_time_delay_ms);

	RTK_LOGI(TAG, "KM0 OS START \n");

	//Enable Schedule
	rtos_sched_start();
}

