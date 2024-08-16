
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "main.h"
#include "vfs.h"
#include "ameba_rtos_version.h"
//#include "wifi_fast_connect.h"
static const char *TAG = "MAIN";
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
	pmu_set_sleep_type(SLEEP_PG);
	pmu_init_wakeup_timer();
#ifndef CONFIG_MP_SHRINK
	SOCPS_sleepInit();
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

void app_filesystem_init(void)
{
	int ret = 0;
	vfs_init();
	ret = vfs_user_register(VFS_PREFIX, VFS_LITTLEFS, VFS_INF_FLASH, VFS_REGION_1, VFS_RW);
	if (ret == 0) {
		ret = rt_kv_init();
		if (ret == 0) {
			RTK_LOGI(TAG, "File System Init Success\n");
			return;
		}
	}
	RTK_LOGE(TAG, "File System Init Fail \n");

	return;
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

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	InterruptRegister(IPC_INTHandler, IPC_KM4_IRQ, (u32)IPCKM4_DEV, INT_PRI5);
	InterruptEn(IPC_KM4_IRQ, INT_PRI5);

	/*IPC table initialization*/
	ipc_table_init(IPCKM4_DEV);

#if !(defined(CONFIG_MP_INCLUDED) || defined (CONFIG_SDIO_FULLMAC) || defined (CONFIG_SPI_FULLMAC))
	app_filesystem_init();
#endif

#ifdef CONFIG_MBED_TLS_ENABLED
	app_mbedtls_rom_init();
#endif

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	app_ftl_init();
#endif

	/* pre-processor of application example */
	app_pre_example();

#if (defined(CONFIG_SDIO_FULLMAC) || defined (CONFIG_SPI_FULLMAC)) && defined(CONFIG_KM4_AS_NP)
	wififw_task_create();
#endif

#if defined(CONFIG_WLAN)
	wlan_initialize();
#endif

	app_pmu_init();
#if defined(CONFIG_RTCIO_ENABLED) && CONFIG_RTCIO_ENABLED
	app_rtc_init();
#endif

	/* Execute application example */
	app_example();

	IPC_SEMDelay(rtos_time_delay_ms);

	RTK_LOGI(TAG, "KM4 START SCHEDULER \n");

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}


