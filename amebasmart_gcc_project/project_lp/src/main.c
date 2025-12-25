#include "ameba_soc.h"
#include "main.h"

static char *TAG = "MAIN";

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

	//RTK_LOGD(TAG, "app_init_debug: %lx:%lx:%lx:%lx\n",debug[0], debug[1], debug[2], debug[3]);
	//RTK_LOGD(TAG, "app_init_debug: %lx:%lx:%lx:%lx\n",debug[0], debug[1], debug[2], debug[3]);

}

void pmc_patch(void)
{

	if (EFUSE_GetChipVersion() == SYSCFG_CUT_VERSION_A) {

		/* PMC patch: gating KM0 CPU CLK */
		u32 temp = HAL_READ32(OTP_REGPATCH_BASE, 0xB0);

		temp = temp & (~0xFFF);
		temp |= 0x221;
		HAL_WRITE32(OTP_REGPATCH_BASE, 0xB0, temp);

		temp = HAL_READ32(PMC_BASE, SYSPMC_OPT);
		temp = temp | 0xC000;
		HAL_WRITE32(PMC_BASE, SYSPMC_OPT, temp);

		HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_0, 0x5A014084);
		HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_1, 0x201f3832);
		HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP1_0, 0x02014163);
		HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP1_1, 0x100);

	} else {

		/* PMC patch: set AON LDO into sleep mode and gating KM0 CPU CLK */
		HAL_WRITE32(OTP_REGPATCH_BASE, 0xB0, 0x10d0221);
		/* PMC patch: Clear CKE_LP to avoid IWDG keep running*/
		HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_0, 0x46009a9a);
		HAL_WRITE32(PMC_BASE, PMC_PATCH_GRP0_1, 0x20fff805);

	}

}

void app_pmu_init(void)
{
	pmu_set_sleep_type(SLEEP_PG);
	pmu_acquire_wakelock(PMU_OS);
	pmu_acquire_wakelock(PMU_KM4_RUN);

	pmu_acquire_deepwakelock(PMU_OS);
	pmu_release_deepwakelock(PMU_KM4_RUN);
	pmu_release_deepwakelock(PMU_AP_RUN);
#ifndef CONFIG_MP_SHRINK

	SOCPS_WakeEvent_Init();
	SOCPS_SleepInit();

	// set power cut stable time
	SOCPS_SetPowerCut_Time(PC_Stable_256);
#endif
	pmc_patch();

}

void app_IWDG_refresh(void *arg)
{
	UNUSED(arg);
	WDG_Refresh(IWDG_DEV);

}

void app_IWDG_init(void)
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

			/*Gate IWDG in low power mode, ortherwise IWDG will wakeup system*/
			IWDG_LP_Enable(IWDG_DEV, DISABLE);

			rtos_timer_start(xTimer, 0);
		}
	}
}

_WEAK void app_example(void)
{

}

//default main
int main(void)
{
	//u32 Temp = 0;

	InterruptRegister((IRQ_FUN)IPC_INTHandler, IPC_IRQ, (u32)IPCLP_DEV, INT_PRI_MIDDLE);
	InterruptEn(IPC_IRQ, INT_PRI_MIDDLE);

	/* Register Log Uart Callback function */
#ifndef CONFIG_LINUX_FW_EN
	InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI_LOWEST);
	InterruptEn(UART_LOG_IRQ, INT_PRI_LOWEST);
	LOGUART_INTConfig(LOGUART_DEV, LOGUART_BIT_ERBI, ENABLE);
#endif
	shell_init_rom(0, NULL);
	shell_init_ram();

	/*IPC table initialization*/
	ipc_table_init(IPCLP_DEV);
	app_pmu_init();

#if defined(CONFIG_WIFI_FW_EN) && CONFIG_WIFI_FW_EN
	wififw_task_create();
#endif

	app_init_debug();

	app_IWDG_init();

	/* Execute application example */
	app_example();
	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
	IPC_SEMDelayStub(&rtos_time_delay_ms);

	RTK_LOGI(TAG, "KM0 OS START \n");

	//Enable Schedule
	rtos_sched_start();
}

