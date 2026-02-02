#include "ameba_soc.h"
#include "main.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE || defined CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
#include "vfs.h"
#include <mbedtls/platform.h>
#endif
#include "ameba_rtos_version.h"
#ifdef CONFIG_MBEDTLS_ENABLED
#include "ssl_rom_to_ram_map.h"
#endif
#if defined(CONFIG_BT_COEXIST)
#include "rtw_coex_ipc.h"
#endif
#include "ameba_diagnose.h"

static const char *const TAG = "MAIN";

void app_init_debug(void)
{
	u32 debug[4];

	debug[LEVEL_ERROR] = 0xFFFFFFFF;
	debug[LEVEL_WARN]  = 0x0;
	debug[LEVEL_TRACE] = 0x0;

	if (SYSCFG_OTP_DisBootLog()) {
		debug[LEVEL_INFO] = 0;
	} else {
		debug[LEVEL_INFO] = BIT(MODULE_BOOT);
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
	ssl_function_map.ssl_calloc = (void *(*)(unsigned int, unsigned int))rtos_mem_calloc;
	ssl_function_map.ssl_free = (void (*)(void *))rtos_mem_free;
	ssl_function_map.ssl_printf = (long unsigned int (*)(const char *, ...))DiagPrintf;
	ssl_function_map.ssl_snprintf = (int (*)(char *s, size_t n, const char *format, ...))DiagSnPrintf;
}
#endif

void app_pmu_init(void)
{
	DBG_INFO_MSG_ON(MODULE_PMC);
	DBG_INFO_MSG_ON(MODULE_KM4);
#ifndef CONFIG_MP_SHRINK
	SOCPS_SleepInit();
#endif
	/* if wake from deepsleep, that means we have released wakelock last time */
	if (BOOT_Reason() & AON_BIT_RSTF_DSLP) {
		pmu_set_sysactive_time(2);
		u32 temp = SOCPS_AONWakeReason();
		if (temp) {
			if (temp & AON_BIT_TIM_ISR_EVT) {
				AONTimer_ClearINT();
				NVIC_ClearPendingIRQ(AON_TIM_IRQ);
				RTK_LOGI(TAG, "deepsleep clear aontimer event \n");
			}
		}
		pmu_acquire_wakelock(PMU_OS);
		pmu_acquire_deepwakelock(PMU_OS);
	}

	if (!LSYS_GET_KR4_IS_NP(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1))) {
		pmu_acquire_wakelock(PMU_KR4_RUN);
		if ((SWR_CORE_Vol_Get() == CORE_VOL_1P0) && (dsp_status_on())) {
			pmu_acquire_wakelock(PMU_DSP_RUN);
		}
	}
}

#ifdef CONFIG_VFS_ENABLED
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

//default main
int main(void)
{
	RTK_LOGI(TAG, "KM4 MAIN \n");

	ameba_rtos_get_version();

	InterruptRegister(IPC_INTHandler, IPC_KM4_IRQ, (u32)IPCKM4_DEV, INT_PRI5);
	InterruptEn(IPC_KM4_IRQ, INT_PRI5);

#ifdef CONFIG_MBEDTLS_ENABLED
	app_mbedtls_rom_init();
#endif

	/*IPC table initialization*/
	ipc_table_init(IPCKM4_DEV);
	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);

#ifdef CONFIG_VFS_ENABLED
	app_filesystem_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif

//only NP can init FW
#if defined(CONFIG_WLAN)
#if !defined(CONFIG_MP_SHRINK) && defined(CONFIG_WHC_DEV) && CONFIG_WHC_DEV
	wififw_task_create();
#endif

	wifi_init();
#endif

#ifdef CONFIG_SHELL
	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();
	InterruptRegister((IRQ_FUN) shell_uart_irq_rom, UART_LOG_IRQ, (u32)NULL, INT_PRI4);
	InterruptEn(UART_LOG_IRQ, INT_PRI4);
	LOGUART_INTConfig(LOGUART_DEV, LOGUART_BIT_ERBI, ENABLE);
#endif

	app_init_debug();

	app_pmu_init();

	rtk_diag_init(RTK_DIAG_HEAP_SIZE, RTK_DIAG_SEND_BUFFER_SIZE);

	/* Execute application example */
	app_example();

	RTK_LOGI(TAG, "KM4 START SCHEDULER \n");

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}

