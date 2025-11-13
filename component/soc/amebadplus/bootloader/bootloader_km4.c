/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "bootloader_km4.h"
#include "boot_ota_km4.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *const TAG = "BOOT";
extern u8 SDIO_Pin_Grp;
extern const u8 SDIO_PAD[5][6];
extern PSRAMINFO_TypeDef PsramInfo;
#define CHECK_AND_PRINT_FLAG(flagValue, bit, name) \
    do { \
        if ((flagValue) & (bit)) { \
            RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s ", (name)); \
        } \
    } while (0)

PRAM_START_FUNCTION BOOT_SectionInit(void)
{
	return (PRAM_START_FUNCTION)__image2_entry_func__;
}

void BOOT_NsStart(u32 Addr)
{
	/* clear stack */
	u32 stack_mid = (MSPLIM_RAM_HP + MSP_RAM_HP + 4) / 2;

	//1. clear stack
	__set_MSP(stack_mid);
	_memset((void *)stack_mid, 0, MSP_RAM_HP - stack_mid);

	__set_MSP(MSP_RAM_HP);
	_memset((void *)MSPLIM_RAM_HP, 0, stack_mid - MSPLIM_RAM_HP);

	DCache_CleanInvalidate(MSPLIM_RAM_HP, MSP_RAM_HP - MSPLIM_RAM_HP);

#ifndef CONFIG_TRUSTZONE
	FuncPtr pFunc = (FuncPtr)Addr;
	pFunc();
#endif

#ifdef CONFIG_RDP_TFM
	UNUSED(Addr); // TFM get addr from VTOR_NS
	PRAM_START_FUNCTION Image3EntryFun = (PRAM_START_FUNCTION)__ram_image3_start__;
	if (HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) & LSYS_BIT_BOOT_WAKE_FROM_PS_HS) {
		Image3EntryFun->RamWakeupFun();
	} else {
		RTK_LOGI(TAG, "Start TFM @ 0x%lx ...\r\n", Image3EntryFun->RamStartFun);
		Image3EntryFun->RamStartFun();
	}
#else
	/* jump to ns world */
	nsfunc *fp = cmse_nsfptr_create(Addr);
	fp();
#endif

	/* avoid compiler to pop stack when exit BOOT_NsStart */
	while (1);
}

/* open some always on functions in this function */
void BOOT_RccConfig(void)
{
	u32 idx = 0;
	u32 TempVal = 0;

	u32 FenReg[4] = {REG_LSYS_FEN_GRP0, REG_LSYS_FEN_GRP1, NULL, REG_AON_FEN};
	u32 FenSet[4] = {0};
	u32 FuncRegIndx = 0;

	u32 CenReg[4] = {REG_LSYS_CKE_GRP0, REG_LSYS_CKE_GRP1, NULL, REG_AON_CLK};
	u32 CenSet[4] = {0};
	u32 ClkRegIndx = 0;

	/*Prevent the gdma function enable from hitting the gdma clk edge, causing a reset release timing collision*/
	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_FEN_GRP0);
	TempVal |= APBPeriph_DMAC;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_FEN_GRP0, TempVal);

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0);
	TempVal |= APBPeriph_DMAC_CLOCK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0, TempVal);

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_CTRL);
	TempVal |= LSYS_OTHERCPU_RST_EN(1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SW_RST_CTRL, TempVal);

	for (idx = 0; ; idx++) {
		/*  Check if search to end */
		if (RCC_Config[idx].func == 0xFFFFFFFF) {
			break;
		}

		if (RCC_Config[idx].state != ENABLE) {
			continue;
		}

		ClkRegIndx = (RCC_Config[idx].clk >> 30) & 0x03;
		FuncRegIndx = (RCC_Config[idx].func >> 30) & 0x03;

		CenSet[ClkRegIndx] |= RCC_Config[idx].clk;
		FenSet[FuncRegIndx] |= RCC_Config[idx].func;
	}

	for (idx = 0; idx < 4; idx++) {
		/*  Clock configure */
		TempVal = HAL_READ32(SYSTEM_CTRL_BASE, CenReg[idx]);
		TempVal |= CenSet[idx] & (~(BIT(31) | BIT(30)));
		HAL_WRITE32(SYSTEM_CTRL_BASE, CenReg[idx], TempVal);

		/*  Function configure */
		TempVal = HAL_READ32(SYSTEM_CTRL_BASE, FenReg[idx]);
		TempVal |= FenSet[idx] & (~(BIT(31) | BIT(30)));
		HAL_WRITE32(SYSTEM_CTRL_BASE, FenReg[idx], TempVal);
	}

	DelayUs(10);

	/*SDM32K*/
	SDM32K_Enable();
}

/* init psramc for flash r/w if needed */
void BOOT_Data_Flash_Init(void)
{
#ifndef CONFIG_IMG2_FLASH
	assert_param(0); /*Code Can only XIP When No Psram*/
#endif

#if defined(CONFIG_SECOND_FLASH_NOR)
	Combo_SPIC_Init();
#endif
}

u32 BOOT_ChipInfo_ClkInfoIdx(void)
{
	/*psram die is wb955 which can run up to 200MHz*/

	/*Boot_SocClk_Info_Idx is valid, use user config socclk*/
	return Boot_SocClk_Info_Idx;
}

void BOOT_ComboSpic_PLL_Open(void)
{
	u32 Boot_Clk_Config_Level = BOOT_ChipInfo_ClkInfoIdx();
	const SocClk_Info_TypeDef *pSocClk_Info = &SocClk_Info[Boot_Clk_Config_Level];

	Combo_SPIC_Output_Div_Clk(pSocClk_Info->PSRAMC_CKD - 1);
}

/* init psram if needed */
void BOOT_PSRAM_Init(void)
{
	u8 pinname;
	PSPHY_InitTypeDef PSPHY_InitStruct;

	/* return directly to save code size*/
#if defined(CONFIG_DISABLE_PSRAM)
	RTK_LOGI(TAG, "CONFIG_DISABLE_PSRAM\r\n");
	return;
#endif

	//reset psram device
	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, DISABLE);
	DelayUs(1);
	RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);

	for (pinname = _PA_1; pinname <= _PA_4; pinname++) {
		PAD_PullCtrl(pinname, GPIO_PuPd_DOWN);
	}

	for (pinname = _PA_7; pinname <= _PA_10; pinname++) {
		PAD_PullCtrl(pinname, GPIO_PuPd_DOWN);
	}

	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_RTLSIM_PRESIM) {
		for (pinname = _PA_0; pinname <= _PA_11; pinname++) {
			PAD_PullCtrl(pinname, GPIO_PuPd_NOPULL);
		}
	}

	for (pinname = _PA_0; pinname <= _PA_11; pinname++) {
		Pinmux_Config(pinname, PINMUX_FUNCTION_SPIC1_PSRAM);
	}
	PSRAM_CLK_Update();
	/* init psram phy */
	PSRAM_PHY_StructInit(&PSPHY_InitStruct);
	PSRAM_PHY_Init(&PSPHY_InitStruct);

	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_APM) {

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
		if (PSRAM_calibration(DISABLE) == FALSE) {
			PSRAM_InfoDump();
			PSRAM_calibration(ENABLE);
		}
	}
}

void BOOT_SCBConfig_HP(void)
{
	/* Enable SecureFault, UsageFault, MemManageFault, BusFault */
	SCB->SHCSR |= SCB_SHCSR_SECUREFAULTENA_Msk | SCB_SHCSR_USGFAULTENA_Msk | \
				  SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

	SCB_NS->SHCSR |=  SCB_SHCSR_USGFAULTENA_Msk | \
					  SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk;

	/* Enable all access to FPU */
	SCB->CPACR |= 0x00f00000;
	SCB_NS->CPACR |=  0x00f00000;
	SCB->NSACR |= BIT(10) | BIT(11); // enable non-secure to access VFP
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
	u32 BOOT_REASON_MASK = (AON_BIT_RSTF_OCP < 1) - 1;

	/*Clear the wake up reason*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW, temp);

	/*Backup it to system register,So the software can read from the register*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW, temp);

	temp &= BOOT_REASON_MASK;
	RTK_LOGI(TAG, "KM4 BOOT REASON %x: ", temp);
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_OCP, "OCP");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WARM_KM02PERI, "WARM_KM02PERI");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WARM_KM42PERI, "WARM_KM42PERI");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG2, "WDG2");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG1, "WDG1");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG0, "WDG0");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_IWDG, "IWDG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_KM0_SYS, "KM0_SYS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_KM4_SYS, "KM4_SYS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_DSLP, "DSLP");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_BOR, "BOR");
	if (temp == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Initial Power on\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	}
}

void BOOT_Enable_KM0(void)
{
#if 0 //annotation for VSCode Debug KM4 main program
	/* halt KM0 for warm/wdg reset because of security */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS) | LSYS_BIT_KM0_RST_HALT);

	/* reset KM0 to avoid km0 in hardfault */
	RCC_PeriphClockCmd(APBPeriph_KM0, APBPeriph_KM0_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_KM0, APBPeriph_KM0_CLOCK, ENABLE);
#endif

	/* clear KM0 halt */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_STATUS) & ~LSYS_BIT_KM0_RST_HALT);

	/* Let KM0 run */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_KM0_RUN);

}

void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;;
	u32 *vector_table = NULL;

	BOOT_TRNG_ParaSet();

	/* Config Non-Security World Registers Firstly in BOOT_WakeFromPG */
	BOOT_RAM_TZCfg();

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamWakeupFun;
	SCB_NS->VTOR = (u32)vector_table;

	/* Add redefine secure fault handler to vector table* */
	Fault_Hanlder_Redirect(NULL);

	/* Enable FPU and Secure/Usage/Mem/Bus Fault */
	BOOT_SCBConfig_HP();

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	/* Start non-secure state software application */
	BOOT_NsStart(vector_table[1]);
}

void BOOT_SOC_ClkChk(const SocClk_Info_TypeDef *pSocClk_Info)
{
	u32 PllClk = pSocClk_Info->PLL_CLK;
	u32 Km4Clk, Km0Clk, PsramClk;

	assert_param(PllClk <= PLL_688P128M);
	assert_param(PllClk >= PLL_300M);
	assert_param(pSocClk_Info->KM4_CKD);
	assert_param(pSocClk_Info->KM0_CKD);
	assert_param(pSocClk_Info->PSRAMC_CKD);
	Km4Clk = PllClk / pSocClk_Info->KM4_CKD;
	Km0Clk = PllClk / pSocClk_Info->KM0_CKD;
	PsramClk = PllClk / pSocClk_Info->PSRAMC_CKD;

	/* Clk info limit check */
	if (pSocClk_Info->Vol_Type == CORE_VOL_0P9) {
		assert_param(Km4Clk <= KM4_0P9V_CLK_LIMIT);
		assert_param(Km0Clk <= KM0_0P9V_CLK_LIMIT);
	} else {
		assert_param(Km4Clk <= KM4_1P0V_CLK_LIMIT);
		assert_param(Km0Clk <= KM0_1P0V_CLK_LIMIT);
	}

	assert_param(PsramClk <= PSRAMC_CLK_LIMIT);
	RTK_LOGI(TAG, "KM4 CPU CLK: %lu Hz\n", Km4Clk);
	RTK_LOGI(TAG, "KM0 CPU CLK: %lu Hz\n", Km0Clk);
	RTK_LOGI(TAG, "PSRAM Ctrl CLK: %lu Hz \n", PsramClk);
}

void BOOT_SOC_ClkSet(void)
{
	u32 Temp;
	u32 PeriDiV, SramDiv, QspiDiv;

	u32 Boot_Clk_Config_Level = BOOT_ChipInfo_ClkInfoIdx();
	const SocClk_Info_TypeDef *pSocClk_Info = &SocClk_Info[Boot_Clk_Config_Level];
	u32 PllClk = pSocClk_Info->PLL_CLK;

	/*Do not Change Divider in FPGA*/
	if (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA) {
		Temp = CPU_ClkGet();
		DelayClkUpdate(Temp);
		return;
	}

	BOOT_SOC_ClkChk(pSocClk_Info);

	// km4:Sram shall be 1:1 or 2:1, sram clk = km4 clk/SramDiv
	SramDiv = (PllClk / pSocClk_Info->KM4_CKD > SRAM_CLK_LIMIT) ? CLKDIV(2) : CLKDIV(1);
	PeriDiV = CLKDIV_ROUND_UP(PllClk, PERI_CLK_LIMIT);/*target clk PERI_CLK_LIMIT*/
	QspiDiv = CLKDIV_ROUND_UP(PllClk, QSPI_CLK_LIMIT);/*target clk QSPI_CLK_LIMIT*/

	/*0. configure core power according user setting */
	CORE_LDO_Vol_Set(pSocClk_Info->Vol_Type);

	//1. switch clk to XTAL
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	Temp = Temp & ~LSYS_MASK_CKSL_LP;	//KM0 XTAL
	Temp = Temp & ~LSYS_BIT_CKSL_HP;	//KM4 XTAL
	Temp = Temp & ~LSYS_BIT_CKSL_HPERI;	//hperi XTAL
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);
	//1.1 update CPU Clk used in DelayUs
	Temp = CPU_ClkGet();
	DelayClkUpdate(Temp);

	//2. modify PLL clock
	PLL_ClkSet(PllClk);

	//3. change ckd
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0);
	Temp &= ~(LSYS_MASK_CKD_HP | LSYS_MASK_CKD_SRAM);
	Temp &= ~LSYS_MASK_CKD_LP;
	Temp &= ~LSYS_MASK_CKD_HPERI;
	Temp &= ~LSYS_MASK_CKD_QSPI;
	Temp |= LSYS_CKD_HP(pSocClk_Info->KM4_CKD - 1);
	Temp |= LSYS_CKD_LP(pSocClk_Info->KM0_CKD - 1);
	Temp |= LSYS_CKD_SRAM(SramDiv - 1);
	Temp |= LSYS_CKD_HPERI(PeriDiV - 1);
	Temp |= LSYS_CKD_QSPI(QspiDiv - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0, Temp);

	//4. back to pll
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0);
	Temp |= LSYS_BIT_CKSL_HP;
	Temp |= LSYS_BIT_CKSL_HPERI;
	Temp |= LSYS_CKSL_LP(CLK_KM0_PLL);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKSL_GRP0, Temp);
	//4.1 update CPU Clk used in DelayUs
	Temp = CPU_ClkGet();
	DelayClkUpdate(Temp);
}

void BOOT_Log_Init(void)
{
	u32 ChipType;

	/* close AGG function for auto test */
	if (Boot_Agg_En) {
		ChipType = SYSCFG_CHIPType_Get();
		if (!((ChipType == CHIP_TYPE_PALADIUM) || (ChipType == CHIP_TYPE_RTLSIM_PRESIM))) {
			/* open loguart agg function */
			LOGUART_WaitTxComplete();
			LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, DISABLE);
			LOGUART_AGGCmd(LOGUART_DEV, ENABLE);
			LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, ENABLE);
		}
	}

	/* open KM0 log */
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_2, ENABLE);
}


void Peripheral_Reset(void)
{
	//reason: The reason for maintaining these bits is for our debugging function.
	//issue: LSYS_PERIALL_RST_EN will reset cpu, causing loss of debug information, which is unexpected.
	//resolve: When initializing power, at bootloader, these bits are enabled.
	// HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_FEN, APBPeriph_RTC | APBPeriph_SDM | APBPeriph_OTPC | APBPeriph_LPON);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_FEN_GRP0, APBPeriph_TRNG | APBPeriph_FLASH | APBPeriph_KM4 | APBPeriph_KM0 | APBPeriph_PLFM | APBPeriph_SOC);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_FEN_GRP1, APBPeriph_DTIM | APBPeriph_LOGUART);
}

void SDIO_Pinmux_pre_init(void)
{
	u8 idx;

	assert_param(SDIO_Pin_Grp <= 0x4);

	/* Pinmux function and Pad control */
	for (idx = 0; idx < 6; idx++) {
		PAD_PullCtrl(SDIO_PAD[SDIO_Pin_Grp][idx], GPIO_PuPd_UP);
		Pinmux_Config(SDIO_PAD[SDIO_Pin_Grp][idx], PINMUX_FUNCTION_SDIO);
	}

	/* SDIO function enable and clock enable*/
	RCC_PeriphClockCmd(APBPeriph_SDIO, APBPeriph_SDIO_CLOCK, ENABLE);

	/* CCCR Autoload done */
	HAL_WRITE8(SDIO_REG_BASE, REG_SPDIO_CPU_IND,
			   HAL_READ8(SDIO_REG_BASE, REG_SPDIO_CPU_IND) | BIT_SYSTEM_READEE_DONE);

	/* SDIO pinmux funtion enable */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_USB_SDIO_CTRL,
				HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_USB_SDIO_CTRL) | LSYS_BIT_SDD_PMUX_FEN);
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

//3 Image 1
void BOOT_Image1(void)
{
#if defined(CONFIG_WHC_INTF_SDIO)
	/*Sdio pinmux pre init advanced to bootloader. If done too late, it may cause host side SDIO card recognition timeout, resulting in sdio power supply failure. */
	SDIO_Pinmux_pre_init();
#endif

	PRAM_START_FUNCTION Image2EntryFun = BOOT_SectionInit();
	//STDLIB_ENTRY_TABLE *prom_stdlib_export_func = (STDLIB_ENTRY_TABLE *)__rom_stdlib_text_start__;
	u32 *vector_table = NULL;
	RRAM_TypeDef *rram = RRAM_DEV;

	_memset((void *) __image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));
	BOOT_ReasonSet();

	Peripheral_Reset();

	if ((BOOT_Reason() == 0) || (!BOOT_RRAM_InfoValid())) {
		/*reset osc 131k counter, only for RTL6845*/
		OSC131K_Reset();

		_memset(RRAM_DEV, 0, sizeof(RRAM_TypeDef));
		RRAM_DEV->MAGIC_NUMBER = 0x6969A5A5;
	}

	BOOT_SOC_ClkSet();

	/* Mem_LDO in normal0 mode by default, change dummy load from 500uA to 200uA */
	LDO_Mem_Dummy_Ctrl(LDO_MEM_DUMMY_200UA);

	BOOT_TRNG_ParaSet();

	BOOT_Log_Init();

	RTK_LOGI(TAG, "IMG1 ENTER MSP:[%08lX]\n", __get_MSP());
	RTK_LOGI(TAG, "Build Time: %s %s\n", __DATE__, __TIME__);

	cmse_address_info_t cmse_address_info = cmse_TT((void *)BOOT_Image1);
	RTK_LOGI(TAG, "IMG1 SECURE STATE: %d\n", cmse_address_info.flags.secure);
	/*Do not use rom stdlib anymore,so mask the init here*/
	/* init stdlib bss and load stdlib data */
	//prom_stdlib_export_func->stdlib_init();

	BOOT_RccConfig();

	BOOT_Disable_PadSlewRate();

	flash_highspeed_setup();

	/* backup flash_init_para address for KM0 */
	DCache_Clean((u32)&flash_init_para, sizeof(FLASH_InitTypeDef));
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_FLASH_PARA_ADDR, (u32)&flash_init_para);

	/* If No PLL CLK, ComboSPIC cant not glitch-free mux to XTAL */
	BOOT_ComboSpic_PLL_Open();

	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();
	rram->Memory_Type = meminfo.mem_type;
	if (meminfo.mem_type == MCM_TYPE_PSRAM) {
		BOOT_PSRAM_Init();
		/* PA5 is psram cs pin, need PU for sleep power consideration */
		PAD_PullCtrl(_PA_5, GPIO_PuPd_UP);
		PAD_SleepPullCtrl(_PA_5, GPIO_PuPd_UP);
	} else if (meminfo.mem_type == MCM_TYPE_NOR_FLASH) {
#ifdef CONFIG_PSRAM_USED
		assert_param(0); /*Code Can only XIP When No Psram*/
#endif
		BOOT_Data_Flash_Init();

		/* PA5 is flash clk pin, need PD for sleep power consideration */
		PAD_PullCtrl(_PA_5, GPIO_PuPd_DOWN);
		PAD_SleepPullCtrl(_PA_5, GPIO_PuPd_DOWN);
	}

	BOOT_Share_Memory_Patch();

	BOOT_ResetMask_Config();

	BOOT_VerCheck();

	BOOT_OTPCheck();

	/* Load from OTA and ECC check for Certificate and IMG2/IMG3*/
	BOOT_OTA_IMG();

	/* set sw patch reg to 0 for security, this reg seure access only */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SW_PATCH, 0);

	/* it will switch shell control to KM0, disable loguart interrupt to avoid loguart irq not assigned in non-secure world.
	 it should switch before BOOT_RAM_TZCfg to avoid crash when loguart intr occur but it has been set to ns intr. */
	LOGUART_INTCoreConfig(LOGUART_DEV, LOGUART_BIT_INTR_MASK_KM4, DISABLE);
	InterruptDis(UART_LOG_IRQ);

	/**
	 * @brief If there are any functions placed on the flash (defined by BOOT_XIP_TEXT_SECTION) that need to be executed,
	 * they must be run before BOOT_RAM_TZcfg().
	 */
	/* Config Non-Security World Registers */
	BOOT_RAM_TZCfg();

	/*KM0 shall wait MPC setting for non-secure access*/
	BOOT_Enable_KM0();

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamStartFun;
	SCB_NS->VTOR = (u32)vector_table;

	/* Add redefine secure fault handler to vector table* */
	Fault_Hanlder_Redirect(NULL);

	/* Enable FPU and Secure/Usage/Mem/Bus Fault */
	BOOT_SCBConfig_HP();

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	/* Start non-secure state software application */
	RTK_LOGI(TAG, "Image2Entry @ 0x%lx ...\r\n", vector_table[1]);

	BOOT_NsStart(vector_table[1]);
}

IMAGE1_VALID_PATTEN_SECTION
const u8 RAM_IMG1_VALID_PATTEN[] = {
	0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff
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
	.FlashStartFun = BOOT_Image1,
	.ExportTable = &boot_export_symbol
};
