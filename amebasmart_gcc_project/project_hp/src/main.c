
#include "ameba_soc.h"
#include "main.h"
#include "vfs.h"
#include "os_wrapper.h"
#include "ameba_rtos_version.h"
//#include "wifi_fast_connect.h"

static const char *TAG = "MAIN";
u32 use_hw_crypto_func;
u32 g_Boot_Status;


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

#if defined(CONFIG_CLINTWOOD ) && CONFIG_CLINTWOOD
	pmu_set_sleep_type(SLEEP_CG);
#else
	pmu_set_sleep_type(SLEEP_PG);
#endif
	pmu_acquire_deepwakelock(PMU_OS);

	/* if wake from deepsleep, that means we have released wakelock last time */
	//if (SOCPS_DsleepWakeStatusGet() == TRUE) {
	//	pmu_set_sysactive_time(2);
	//	pmu_release_wakelock(PMU_OS);
	//	pmu_tickless_debug(ENABLE);
	//}
}

/* enable or disable BT shared memory */
/* if enable, KM4 can access it as SRAM */
/* if disable, just BT can access it */
/* 0x100E_0000	0x100E_FFFF	64K */
void app_shared_btmem(u32 NewStatus)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL);

	if (NewStatus == ENABLE) {
		temp |= HSYS_BIT_SHARE_BT_MEM;
	} else {
		temp &= ~HSYS_BIT_SHARE_BT_MEM;
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL, temp);
}

_WEAK void app_functional_chk_status_set(u32 Boot_Status)
{
	g_Boot_Status = Boot_Status;
}

_WEAK void app_functional_chk_done_callback(void)
{
	/* g_Boot_Status is 0 by default, app_functional_chk_status_set shall be called to set g_Boot_Status  */
	if (g_Boot_Status == 0) {
		BKUP_Write(BKUP_REG0, BKUP_Read(BKUP_REG0) & (~BOOT_CNT_MASK));
	} else {
		//if function not work correctly, trigger reboot
		System_Reset();
	}
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

extern int rt_kv_init(void);

void app_filesystem_init(void)
{
#if defined(CONFIG_SINGLE_CORE_WIFI)
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
	return;
}

//default main
int main(void)
{
	RTK_LOGI(TAG, "KM4 MAIN \n");
	ameba_rtos_get_version();

	InterruptRegister(IPC_INTHandler, IPC_NP_IRQ, (u32)IPCNP_DEV, 5);
	InterruptEn(IPC_NP_IRQ, 5);

#ifdef CONFIG_MBED_TLS_ENABLED
	app_mbedtls_rom_init();
#endif
	//app_init_debug();

	/* init console */
	shell_init_rom(0, 0);
	shell_init_ram();

	ipc_table_init(IPCNP_DEV);
	IPC_SEMDelayStub((void *)rtos_time_delay_ms);

#ifndef CONFIG_MP_INCLUDED
	app_filesystem_init();
#endif

#if defined(CONFIG_FTL_ENABLED) && CONFIG_FTL_ENABLED
	app_ftl_init();
#endif


	/* pre-processor of application example */
	app_pre_example();

#ifdef CONFIG_WLAN
	wlan_initialize();
#endif

	//app_shared_btmem(ENABLE);

	app_pmu_init();

	app_functional_chk_done_callback();
	app_init_debug();

	/* Execute application example */
	app_example();
	IPC_SEMDelayStub(&rtos_time_delay_ms);

	RTK_LOGI(TAG, "KM4 START SCHEDULER \n");

	/* Enable Schedule, Start Kernel */
	rtos_sched_start();
}

