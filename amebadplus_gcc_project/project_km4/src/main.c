
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "main.h"
#if (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE || defined CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
#include "vfs.h"
#endif
#include "ameba_rtos_version.h"
#ifdef CONFIG_MBEDTLS_ENABLED
#include "ssl_rom_to_ram_map.h"
#endif
//#include "wifi_fast_connect.h"
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

#if (defined(CONFIG_BT) && CONFIG_BT) && (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
#include "bt_inic.h"
#endif

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
	pmu_set_sleep_type(SLEEP_PG);
	pmu_init_wakeup_timer();
#ifndef CONFIG_MP_SHRINK
	SOCPS_SleepInit();
#endif
}

void app_calc_new_time(RTCIO_TimeInfo *pTimeInfoPre, RTC_TimeTypeDef *pTimeInfoPost)
{
	u8 IsLeapYear = 0;
	u32 CntTotal = 0;
	u32 modnum[4] = {60, 60, 24, 365};	//convert to: m h d y

	CntTotal = (pTimeInfoPre->Bkup_Year) + 1900;//total year
	IS_LEAP_YEAR_CHECK(CntTotal, IsLeapYear);
	modnum[3] += IsLeapYear;

	CntTotal = pTimeInfoPre->Pwd_Counter;

	TIME_STEP(pTimeInfoPre->Bkup_Seconds, CntTotal, modnum[0], pTimeInfoPost->RTC_Seconds);
	TIME_STEP(pTimeInfoPre->Bkup_Minutes, CntTotal, modnum[1], pTimeInfoPost->RTC_Minutes);
	TIME_STEP(pTimeInfoPre->Bkup_Hours, CntTotal, modnum[2], pTimeInfoPost->RTC_Hours);
	TIME_STEP(pTimeInfoPre->Bkup_Days, CntTotal, modnum[3], pTimeInfoPost->RTC_Days);
	TIME_STEP(((pTimeInfoPre->Bkup_Year) + 1900), CntTotal, 0xFFFF, pTimeInfoPost->RTC_Year);

	pTimeInfoPost->RTC_H12_PMAM = RTC_HourFormat_24;

}

void app_rtc_init(void)
{
	RTCIO_TimeInfo RTCIO_TimeStruct;
	RTC_InitTypeDef RTC_InitStruct;
	RTC_TimeTypeDef RTC_TimeStruct;

	if (BOOT_Reason() == 0) {
		if (RTCIO_IsEnabled() == TRUE) {
			/* shift out bkup data */
			RTCIO_GetTimeInfo(&RTCIO_TimeStruct);

			/* update RTC Time */
			app_calc_new_time(&RTCIO_TimeStruct, &RTC_TimeStruct);
		} else {
			RTC_TimeStructInit(&RTC_TimeStruct);
			RTC_TimeStruct.RTC_Year = 2021;
			RTC_TimeStruct.RTC_Hours = 10;
			RTC_TimeStruct.RTC_Minutes = 20;
			RTC_TimeStruct.RTC_Seconds = 30;
		}

		/* Only Asic need OSC Calibration */
		if (SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC_POSTSIM) {
			/* reset rcal to avoid rvals conflict in aon and rtcio domain */
			RTCIO_SetRValue(RTCIO_RECV_RVAL_RST);
			OSC131K_Calibration(30000);
			RTCIO_SetRValue(RTCIO_RECV_RVAL_CAL);
		}

		//share process for both rtcio enabled/disabled
		RCC_PeriphClockCmd(APBPeriph_RTC, APBPeriph_RTC_CLOCK, ENABLE);
		RTC_StructInit(&RTC_InitStruct);
		RTC_Init(&RTC_InitStruct);

		RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
	}

}

void fs_init_thread(void *param)
{
	(void)param;
#if !(defined(CONFIG_MP_SHRINK)) && (defined CONFIG_WHC_HOST || defined CONFIG_WHC_NONE || defined CONFIG_WHC_WPA_SUPPLICANT_OFFLOAD)
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

	vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	ret = rt_kv_init();
	if (ret == 0) {
		RTK_LOGI(TAG, "File System Init Success \n");
		goto exit;
	}


	RTK_LOGE(TAG, "File System Init Fail \n");
exit:
#endif
	rtos_task_delete(NULL);
}

void app_filesystem_init(void)
{
	rtos_task_create(NULL, ((const char *)"fs_init_thread"), fs_init_thread, NULL, 4096, 5);
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
	RTK_LOGI(TAG, "KM4 MAIN \n");
	ameba_rtos_get_version();
	/* Debug log control */
	app_init_debug();

	InterruptRegister(IPC_INTHandler, IPC_KM4_IRQ, (u32)IPCKM4_DEV, INT_PRI5);
	InterruptEn(IPC_KM4_IRQ, INT_PRI5);

	/*IPC table initialization*/
	ipc_table_init(IPCKM4_DEV);

	app_filesystem_init();

#ifdef CONFIG_MBEDTLS_ENABLED
	app_mbedtls_rom_init();
#endif

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	app_ftl_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

#if defined(CONFIG_WIFI_FW_EN) && CONFIG_WIFI_FW_EN
	wififw_task_create();
#endif

#if defined(CONFIG_BT_COEXIST)
	/* init coex ipc */
	coex_ipc_entry();
#endif

#if defined(CONFIG_WLAN)
	wifi_init();
#endif

	/* initialize BT iNIC */
#if (defined(CONFIG_BT) && CONFIG_BT) && (defined(CONFIG_BT_INIC) && CONFIG_BT_INIC)
	bt_inic_init();
#endif

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	app_pmu_init();
#if defined(CONFIG_RTCIO_ENABLED) && CONFIG_RTCIO_ENABLED
	app_rtc_init();
#endif

	rtk_diag_init(RTK_DIAG_HEAP_SIZE, RTK_DIAG_SEND_BUFFER_SIZE);

	/* Execute application example */
	app_example();
	IPC_patch_function(&rtos_critical_enter, &rtos_critical_exit);
	IPC_SEMDelay(rtos_time_delay_ms);


	RTK_LOGI(TAG, "KM4 START SCHEDULER \n");

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}


