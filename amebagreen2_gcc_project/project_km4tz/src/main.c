
#include "ameba_soc.h"
#include "main.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
#include "vfs.h"
#endif
#include "os_wrapper.h"
#include "ameba_rtos_version.h"
#include "ssl_rom_to_ram_map.h"
//#include "wifi_fast_connect.h"
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif

static const char *const TAG = "MAIN";
u32 use_hw_crypto_func;

#if (defined(CONFIG_BT) && CONFIG_BT) && (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
#include "bt_inic.h"
#endif

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

void app_fullmac_init(void)
{

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
}

void app_pmu_init(void)
{
	SOCPS_SleepInit();
	pmu_init_wakeup_timer();
	pmu_set_sleep_type(SLEEP_PG);

	/* only one core in fullmac mode */
#ifndef CONFIG_WIFI_HOST_CONTROL
	/* If the current cpu is np, need to hold the lock of another cpu */
	if ((HAL_READ32(PMC_BASE, SYSPMC_CTRL) & PMC_BIT_CPU_IS_AP) == 0) {
		pmu_acquire_wakelock(PMU_CPU1_RUN);
	}
#endif
}

extern int rt_kv_init(void);

void app_filesystem_init(void)
{
#if !(defined(CONFIG_MP_INCLUDED)) && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE)
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

static void aontimer_dslp_handler(void)
{
	AONTimer_ClearINT();
	RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, DISABLE);
}

static void rtc_dslp_handler(void)
{
	RTC_AlarmClear();
	RTC_DayIntClear();
	RTC_WakeupClear();
}

static void wakepin_dslp_handler(void)
{
	u32 pinidx;
	pinidx = WakePin_Get_Idx();
	WakePin_ClearINT(pinidx);
}

static void dslp_wake_handler(void)
{
	u32 BootReason;

	BootReason = SOCPS_AONWakeReason();
	RTK_LOGI(TAG, "DSLP WAKE REASON: %lx \n", BootReason);

	if (BootReason & AON_BIT_TIM_ISR_EVT) {
		RCC_PeriphClockCmd(APBPeriph_ATIM, APBPeriph_ATIM_CLOCK, ENABLE);
		InterruptRegister((IRQ_FUN)aontimer_dslp_handler, AON_TIM_IRQ, NULL, 3);
		InterruptEn(AON_TIM_IRQ, 3);
	}

	if (BootReason & AON_BIT_RTC_ISR_EVT) {
		InterruptRegister((IRQ_FUN)rtc_dslp_handler, RTC_IRQ, NULL, 3);
		InterruptEn(RTC_IRQ, 3);
	}

	if (BootReason & (AON_BIT_GPIO_PIN0_WAKDET_EVT | AON_BIT_GPIO_PIN1_WAKDET_EVT | \
					  AON_BIT_GPIO_PIN2_WAKDET_EVT | AON_BIT_GPIO_PIN3_WAKDET_EVT)) {
		InterruptRegister((IRQ_FUN)wakepin_dslp_handler, AON_WAKEPIN_IRQ, NULL, 3);
		InterruptEn(AON_WAKEPIN_IRQ, 3);
	}
}


void app_rtc_init(void)
{
	RTC_InitTypeDef RTC_InitStruct;
	RTC_TimeTypeDef RTC_TimeStruct;

	RTC_TimeStructInit(&RTC_TimeStruct);
	RTC_TimeStruct.RTC_Year = 2021;
	RTC_TimeStruct.RTC_Hours = 10;
	RTC_TimeStruct.RTC_Minutes = 20;
	RTC_TimeStruct.RTC_Seconds = 30;

	RTC_StructInit(&RTC_InitStruct);
	/*enable RTC*/
	RTC_Enable(ENABLE);
	RTC_Init(&RTC_InitStruct);
	RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

u32 rtc_irq_init(void *Data)
{
	/* To avoid gcc warnings */
	(void) Data;
	u32 temp;

	RTC_ClearDetINT();
	SDM32K_Enable();
	SYSTIMER_Init(); /* 0.2ms */
	RCC_PeriphClockCmd(NULL, APBPeriph_RTC_CLOCK, ENABLE);

	if ((Get_OSC131_STATE() & RTC_BIT_FIRST_PON) == 0) {
		app_rtc_init();
		/*set first_pon to 1, this indicate RTC first pon state*/
		temp = Get_OSC131_STATE() | RTC_BIT_FIRST_PON;
		Set_OSC131_STATE(temp);

		/*before 131k calibratopn, cke_rtc should be enabled*/
		if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC_POSTSIM) {//Only Asic need OSC Calibration
			OSC131K_Calibration(30000); /* PPM=30000=3% *//* 7.5ms */
		}
	}

	RTC_ClkSource_Select(SDM32K);

	return 0;
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
	RTK_LOGI(TAG, "AP MAIN \n");
	ameba_rtos_get_version();

	if ((BOOT_Reason() & AON_BIT_RSTF_DSLP) != 0) {
		dslp_wake_handler();
	}

#ifdef CONFIG_WIFI_HOST_CONTROL
	app_fullmac_init();
	app_IWDG_int();
#else
	/*IPC table initialization*/
	ipc_table_init(IPCAP_DEV);
	InterruptRegister(IPC_INTHandler, IPC_CPU0_IRQ, (u32)IPCAP_DEV, INT_PRI5);
	InterruptEn(IPC_CPU0_IRQ, INT_PRI5);
#endif

#if defined(CONFIG_MBEDTLS_ENABLED)
	app_mbedtls_rom_init();
#endif

	app_filesystem_init();

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	app_ftl_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

#ifdef CONFIG_ETHERNET
	ethernet_mii_init();
#endif

#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif

#if defined(CONFIG_WIFI_FW_EN) && defined(CONFIG_FW_DRIVER_COEXIST)
	wififw_task_create();
#endif

#ifdef CONFIG_WLAN
	wifi_init();
#endif

	/* initialize BT iNIC */
#if defined(CONFIG_BT) && defined(CONFIG_BT_INIC)
	bt_inic_init();
#endif

	/* init console */
	shell_init_rom(0, NULL);
	shell_init_ram();

#ifdef CONFIG_WIFI_HOST_CONTROL
	/* Register Log Uart Callback function */
	InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(UART_LOG_IRQ, INT_PRI_LOWEST);
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_AP, ENABLE);
#endif

	app_pmu_init();

	/*Register RTC_DET_IRQ callback function */
	InterruptRegister((IRQ_FUN) rtc_irq_init, RTC_DET_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(RTC_DET_IRQ, INT_PRI_LOWEST);

	/* Register CPU1_WDG_RST_IRQ Callback function */
	InterruptRegister((IRQ_FUN) CPU1_WDG_RST_Handler, CPU1_WDG_RST_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(CPU1_WDG_RST_IRQ, INT_PRI_LOWEST);

	/* Execute application example */
	app_example();

	RTK_LOGI(TAG, "AP START SCHEDULER \n");

	/* Set delay function & critical function for hw ipc sema */
	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
	IPC_SEMDelayStub(rtos_time_delay_ms);

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}

