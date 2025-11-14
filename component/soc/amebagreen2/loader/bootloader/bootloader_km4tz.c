/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "bootloader_km4tz.h"
#include "ameba_dump_stack.h"
#include "ameba_secure_boot.h"
#include "boot_ota_km4tz.h"

static const char *const TAG = "BOOT";
extern void BOOT_ROM_Copy(void *__restrict dst0, const void *__restrict src0, size_t len0);
MCM_MemTypeDef meminfo;
extern PSRAMINFO_TypeDef PsramInfo;

#define CHECK_AND_PRINT_FLAG(flagValue, bit, name)         \
	do {                                                   \
		if ((flagValue) & (bit)) {                         \
			RTK_LOGS(TAG, RTK_LOG_ALWAYS, "%s\n", (name)); \
		}                                                  \
	} while (0)

__NO_RETURN void BOOT_NsStart(u32 Addr)
{
#ifndef CONFIG_TRUSTZONE
	FuncPtr pFunc = (FuncPtr)Addr;
	pFunc();
#endif

	/* jump to ns world */
	nsfunc *fp = cmse_nsfptr_create(Addr);
	fp();

	/* avoid compiler to pop stack when exit BOOT_NsStart */
	while (1);
}

__NO_RETURN void BOOT_ClearMSP_NsStart(u32 Addr)
{
	// do not use bss section after, because fullmac pg loader will overlay bss section
#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV)) && \
	!defined(CONFIG_FULLMAC_PG_LOADER)
	_memcpy((void *)__km4tz_pg_loader_ram_start__, (void *)__embedded_bin_start, __embedded_bin_end - __embedded_bin_start);
	DCache_CleanInvalidate((u32)__km4tz_pg_loader_ram_start__, __embedded_bin_end - __embedded_bin_start);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_ADDR_TZ, __km4tz_pg_loader_ram_start__);
#endif

	/* clear stack */
	u32 stack_mid = (MSPLIM_RAM_HP + MSP_RAM_HP + 4) / 2;

	// 1. clear stack
	__set_MSP(stack_mid);
	_memset((void *)stack_mid, 0, MSP_RAM_HP - stack_mid);

	__set_MSP(MSP_RAM_HP);
	_memset((void *)MSPLIM_RAM_HP, 0, stack_mid - MSPLIM_RAM_HP);

	DCache_CleanInvalidate(MSPLIM_RAM_HP, MSP_RAM_HP - MSPLIM_RAM_HP);

	BOOT_NsStart(Addr);
}

/* open some always on functions in this function */
void BOOT_RccConfig(void)
{
	RCC_PeriphClockCmd(APBPeriph_SDM, APBPeriph_SDM_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_GPIO, APBPeriph_GPIO_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_LTIM0, APBPeriph_LTIM0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_PTIM0, APBPeriph_PTIM0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_PTIM1, APBPeriph_PTIM1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_DMAC, APBPeriph_DMAC_CLOCK, ENABLE);
}

/* init psramc for flash r/w if needed */
int BOOT_Data_Flash_Init(void)
{
#if defined(CONFIG_SECOND_FLASH_NOR) || defined(CONFIG_SECOND_FLASH_NAND)

	if (meminfo.mem_type == MCM_TYPE_PSRAM) {
		RTK_LOGE(TAG, "Can't support second flash for chip with mem psram\r\n");
		return RTK_FAIL;
	}

	Combo_SPIC_Init();

#endif
	return RTK_SUCCESS;
}

/* init psram if needed */
int BOOT_PSRAM_Init(void)
{
	u8 pinname;
	PSPHY_InitTypeDef PSPHY_InitStruct;

	/* return directly to save code size*/
#if defined(CONFIG_DISABLE_PSRAM)
	RTK_LOGI(TAG, "CONFIG_DISABLE_PSRAM\r\n");
	return RTK_FAIL;
#endif

	if (meminfo.mem_type != MCM_TYPE_PSRAM) {
		return RTK_FAIL;
	}

	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);
	PSRAM_CLK_Update();

	for (pinname = _PC_9; pinname <= _PC_29; pinname++) {
		PAD_PullCtrl(pinname, GPIO_PuPd_NOPULL);
		Pinmux_Config(pinname, PINMUX_FUNCTION_SPIC1_PSRAM);
		PAD_DrvStrength(pinname, PAD_DRV_ABILITITY_HIGH);
	}

	/* init psram phy */
	PSRAM_PHY_StructInit(&PSPHY_InitStruct);
	PSRAM_PHY_Init(&PSPHY_InitStruct);

	if (meminfo.dram_info.model == MCM_PSRAM_VENDOR_APM) {
		RTK_LOGI(TAG, "Init APM PSRAM\r\n");

		/* init psram controller */
		PSRAM_CTRL_Init();
		/* init psram device */
		PSRAM_APM_DEVIC_Init();

	} else {
		RTK_LOGI(TAG, "Init WB PSRAM\r\n");

		/* init psram controller */
		PSRAM_CTRL_Init();
		/* init psram device */
		PSRAM_WB_DEVIC_Init();
	}

	/* psram calibration */
	if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC_POSTSIM) || (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA)) {
		PSRAM_calibration();
	}

	return RTK_SUCCESS;
}

void BOOT_SCBConfig_HP(void)
{
	/* Enable SecureFault, UsageFault, MemManageFault, BusFault */
	SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

	SCB_NS->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

	/* Enable all access to FPU */
	SCB->CPACR |= 0x00f00000;
	SCB_NS->CPACR |= 0x00f00000;
	SCB->NSACR |= BIT(10) | BIT(11); // enable non-secure to access VFP
}

void BOOT_ImgCopy(void *__restrict dst0, const void *__restrict src0, size_t len0)
{
	BOOT_ROM_Copy(dst0, src0, len0);
}

u32 BOOT_LoadImages(void)
{
	/* Load from OTA and ECC check for Certificate and IMG2/IMG3*/
	BOOT_OTA_IMG();

	return TRUE;
}

/**
 * @brief  Copy Boot reason to a common register and clear Boot Reason.
 * if Boot Reason is not cleared, it can't get correct boot reason next time.
 * @param  None
 * @retval None.
 */
void BOOT_ReasonSet(void)
{
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW);
	u32 BOOT_REASON_MASK = (AON_BIT_RSTF_THM < 1) - 1;

	/*Clear the wake up reason*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW, temp);

	/*Backup it to system register,So the software can read from the register*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW, temp);

	temp &= BOOT_REASON_MASK;
	RTK_LOGI(TAG, "AP BOOT REASON %x: \n", temp);
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_THM, "THM");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG2_CPU, "WDG2_CPU");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG1_CPU, "WDG1_CPU");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG0_CPU, "WDG0_CPU");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_SYS1_CPU, "SYS1_CPU");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_SYS0_CPU, "SYS0_CPU");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_OCP, "OCP");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WARM_KM4NS, "WARM_KM4NS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WARM_KM4TZ, "WARM_KM4TZ");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG2_GLB, "WDG2_GLB");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG1_GLB, "WDG1_GLB");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG0_GLB, "WDG0_GLB");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_IWDG, "IWDG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_SYS1_GLB, "SYS1_GLB");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_SYS0_GLB, "SYS0_GLB");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_DSLP, "DSLP");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_BOR, "BOR");
	if (temp == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Initial Power on\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	}
}

void BOOT_Enable_NP(void)
{
	/* Let NP run */
	/* if access addr in [start, end], access[19:10] + offset == destination addr[19:10] */
	HAL_WRITE32(CPU_MMU_BASE_S, REG_CMMU_REMAP0BAR_CPU1, 0x20001000);
	HAL_WRITE32(CPU_MMU_BASE_S, REG_CMMU_REMAP0EAR_CPU1, 0x200023FF);
	HAL_WRITE32(CPU_MMU_BASE_S, REG_CMMU_REMAP0OR_CPU1, 0x00002000 | CMMU_BIT_REMAP_EN0_CPU1);

	/* Let NP run */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_CPU1_RUN);
}

void BOOT_Share_Cache_To_TCM(void)
{
	/* Halt and reset NP CPU */
	HAL_WRITE32(SYSTEM_CTRL_BASE_S, REG_LSYS_PLAT_STATUS, HAL_READ32(SYSTEM_CTRL_BASE_S, REG_LSYS_PLAT_STATUS) | LSYS_BIT_KM4NS_RST_HALT);
	RCC_PeriphClockCmd(APBPeriph_CPU1, APBPeriph_CPU1_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_CPU1, APBPeriph_CPU1_CLOCK, ENABLE);

	/* Set NP TCM share bit */
	HAL_WRITE32(SYSTEM_CTRL_BASE_S, REG_LSYS_PLAT_STATUS, HAL_READ32(SYSTEM_CTRL_BASE_S, REG_LSYS_PLAT_STATUS) | LSYS_BIT_KM4NS_SHARE_CACHE_MEM);

#ifdef CONFIG_FULLMAC_IN_SINGLE_DIE // When fullmac support XIP, need enable Cache and cannot share cache to TCM
	/* Disable AP CPU cache */
	Cache_Enable(DISABLE);

	/* Set AP TCM share bit */
	HAL_WRITE32(SYSTEM_CTRL_BASE_S, REG_LSYS_PLAT_STATUS, HAL_READ32(SYSTEM_CTRL_BASE_S, REG_LSYS_PLAT_STATUS) | LSYS_BIT_KM4TZ_SHARE_CACHE_MEM);
#endif
}

void BOOT_Config_PMC_Role(void)
{
#if (!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	HAL_WRITE32(PMC_BASE, SYSPMC_CTRL, HAL_READ32(PMC_BASE, SYSPMC_CTRL) & (~PMC_BIT_NP_CPU_ID));
	RTK_LOGI(TAG, "PMC_CORE_ROLE: KM4TZ\n");
#else
	RTK_LOGI(TAG, "PMC_CORE_ROLE: KM4NS\n");
#endif
}

void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	FIH_DECLARE(fih_rc, FIH_FAILURE);

	/* Config Non-Security World Registers Firstly in BOOT_WakeFromPG */
	FIH_CALL(BOOT_Wake_TZCfg, fih_rc);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto exit;
	}

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	BOOT_NsStart((u32)Image2EntryFun->RamWakeupFun);

exit:
	while (1);
}

/**
 * @brief  When enable SWD software control function, the chip cannot be debugged by default.
 *         To debug, the function need to be called in secure world and enable the specific debug mode.
 * @param  None.
 * @retval None.
 * @note   Called in secure world.
 */
void BOOT_SWD_SwCtrl(void)
{
	u32 regtmp = 0;

	/* enable swd pinmux */
	regtmp = HAL_READ32(PINMUX_REG_BASE, REG_SWD_SDD_CTRL);
	HAL_WRITE32(PINMUX_REG_BASE, REG_SWD_SDD_CTRL, regtmp | PAD_BIT_SWD_PMUX_EN);

	regtmp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SW_SWD_CTRL);
	/* allow to access AHB-AP when SWD software control func enabled in OTP */
	regtmp |= (LSYS_BIT_SW_SWD_DBGEN | LSYS_BIT_SW_SWD_NIDEN | LSYS_BIT_SW_SWD_SPIDEN | LSYS_BIT_SW_SWD_SPNIDEN);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SW_SWD_CTRL, regtmp);
}

void BOOT_Log_Init(void)
{
	u32 ChipType;

	/* close AGG function for auto test */
	if (Boot_Agg_En) {
		ChipType = SYSCFG_CHIPType_Get();
		if (!(ChipType == CHIP_TYPE_PALADIUM)) {
			/* open loguart agg function */
			LOGUART_WaitTxComplete();
			LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, DISABLE);
			LOGUART_AGGCmd(LOGUART_DEV, ENABLE);
			LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, ENABLE);
		}
	} else {
		LOGUART_WaitTxComplete();
		/* counter src is xtal40MHz when agg_en is 0 */
		LOGUART_AGGSetTimeOut(LOGUART_DEV, 0x3FFF);
	}

	/* open NP log */
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_2, ENABLE);
}

/* To avoid RRAM holding incorrect data, incorporate a MAGIC_NUMBER for verification. */
static bool BOOT_RRAM_InfoValid(void)
{
	if (RRAM_DEV->MAGIC_NUMBER != 0x6969A5A5) {
		return FALSE;
	} else {
		return TRUE;
	}
}

// 3 Image 1
void BOOT_Image1(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	FIH_DECLARE(fih_rc, FIH_FAILURE);
#ifdef CONFIG_WHC_INTF_SDIO
	Boot_SDIO_Pinmux_init();
#endif

	_memset((void *)__image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));
	BOOT_ReasonSet();
	/* BOOT Reason: POR or BOR. */
	/* BOD is enabled by default. BOR may arise if voltage increases slowly during POR. */
	/* To avoid Retention RAM cannot be initialized to 0 correctly. */
	if (((BOOT_Reason() & ~AON_BIT_RSTF_BOR) == 0) || (!BOOT_RRAM_InfoValid())) {
		_memset(RRAM_DEV, 0, sizeof(RRAM_TypeDef));
		RRAM_DEV->MAGIC_NUMBER = 0x6969A5A5;
	}

	BOOT_VerCheck();

	BOOT_SOC_ClkSet();

	BOOT_Log_Init();

	RTK_LOGI(TAG, "IMG1 ENTER MSP:[%08x]\n", __get_MSP());
	RTK_LOGI(TAG, "Build Time: %s %s\n", __DATE__, __TIME__);

	BOOT_RccConfig();
	BOOT_Config_PMC_Role();
	BOOT_ResetMask_Config();

	meminfo = ChipInfo_MCMInfo();
	int ret = BOOT_PSRAM_Init();
	if (ret == RTK_FAIL) {
		LDO_MemSetInNormal(MLDO_OFF); /* psram initial fail or non-psram chip，close psram LDO(mem_ldo 1.8V)*/
#ifdef CONFIG_PSRAM_USED
		assert_param(0); /*Code Can only XIP When No Psram*/
#endif
	} else {
		LDO_MemSetInNormal(MLDO_NORMAL);
		LDO_MemSetInSleep(MLDO_SLEEP);
	}

#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	BOOT_Data_Flash_Init();

	flash_highspeed_setup();
	BOOT_LoadImages();
#endif

	/* it will switch shell control to NP, disable loguart interrupt to avoid loguart irq not assigned in non-secure world.
	 it should switch before BOOT_RAM_TZCfg to avoid crash when loguart intr occur but it has been set to ns intr. */
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_AP, DISABLE);
	InterruptDis(UART_LOG_IRQ);

	/* Config Non-Security World Registers
	This function should be called before NP startup to avoid secure issue
	Also should be called after psram init, to avoid secure function block psram init
	*/
	FIH_CALL(BOOT_RAM_TZCfg, fih_rc);
	if (FIH_NOT_EQ(fih_rc, FIH_SUCCESS)) {
		goto exit;
	}

#if !(!defined (CONFIG_WHC_INTF_IPC) && defined (CONFIG_WHC_DEV))
	/*NP shall wait MPC setting for non-secure access*/
	BOOT_Enable_NP();
#else
	BOOT_Share_Cache_To_TCM();
	Boot_Fullmac_LoadIMGAll();
#endif

	// vector_table = (u32 *)Image2EntryFun->VectorNS;
	// vector_table[1] = (u32)Image2EntryFun->RamStartFun;
	SCB_NS->VTOR = (u32)Image2EntryFun->VectorNS;

	/* Enable FPU and Secure/Usage/Mem/Bus Fault */
	BOOT_SCBConfig_HP();

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	/* Start non-secure state software application */
	RTK_LOGI(TAG, "Start NonSecure @ 0x%x ...\r\n", (u32)Image2EntryFun->RamStartFun);

	/* Set the depth of the stack to dump. */
	crash_SetExStackDepth(MIN_DUMP_DEPTH);

	BOOT_ClearMSP_NsStart((u32)Image2EntryFun->RamStartFun);

exit:
	SBOOT_Validate_Fail_Stuck(FALSE);
}

IMAGE1_VALID_PATTEN_SECTION
const u8 RAM_IMG1_VALID_PATTEN[] = {
	0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff,
};

IMAGE1_EXPORT_SYMB_SECTION
BOOT_EXPORT_SYMB_TABLE boot_export_symbol = {

};

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = NULL,
	.RamWakeupFun = BOOT_WakeFromPG,
	.RamPatchFun0 = NULL,
	.RamPatchFun1 = NULL,
	.RamPatchFun2 = NULL,
#ifndef CONFIG_FULLMAC_PG_LOADER
	.FlashStartFun = BOOT_Image1,
#endif
	.ExportTable = &boot_export_symbol,
};
