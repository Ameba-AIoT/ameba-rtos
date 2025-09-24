/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_secure_boot.h"
#include "bootloader_hp.h"
#include "boot_ota_hp.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *const TAG = "BOOT";
typedef struct {
	u32 NVICbackup_HP[6];
	u32 SCBVTORbackup_HP;
	u8 NVICIPbackup_HP[MAX_PERIPHERAL_IRQ_NUM];
} CPU_S_BackUp_TypeDef;

CPU_S_BackUp_TypeDef PMC_S_BK;

#define CHECK_AND_PRINT_FLAG(flagValue, bit, name) \
    do { \
        if ((flagValue) & (bit)) { \
            RTK_LOGS(NOTAG, RTK_LOG_INFO, "%s ", (name)); \
        } \
    } while (0)

BOOT_RAM_TEXT_SECTION
PRAM_START_FUNCTION BOOT_SectionInit(void)
{
	return (PRAM_START_FUNCTION)__image2_entry_func__;
}

BOOT_RAM_TEXT_SECTION
__attribute__((noinline)) void BOOT_NsStart(u32 Addr)
{
	/* clear stack */
	u32 stack_top = MSPLIM_RAM_HP;
	u32 stack_end = MSP_RAM_HP + 4;
	u32 stack_mid = (stack_top + stack_end) / 2;

	__set_MSP(stack_mid);

	_memset((void *)stack_mid, 0, stack_end - stack_mid);
	DCache_CleanInvalidate(stack_mid, stack_end - stack_mid);

	__set_MSP(stack_end);

	_memset((void *)stack_top, 0, stack_mid - stack_top);
	DCache_CleanInvalidate(stack_top, stack_mid - stack_top);

	/* jump to ns world */
	nsfunc *fp = (nsfunc *)cmse_nsfptr_create(Addr);
	fp();

}

/* open some always on functions in this function */
BOOT_RAM_TEXT_SECTION
void BOOT_RccConfig(void)
{
	u32 idx = 0;
	u32 TempVal = 0;
	u32 FenReg[4] = {REG_LSYS_FEN_GRP0, REG_LSYS_FEN_GRP1, REG_LSYS_FEN_GRP2, REG_AON_FEN};
	u32 FenSet[4] = {0};
	u32 FuncRegIndx = 0;

	u32 CenReg[4] = {REG_LSYS_CKE_GRP0, REG_LSYS_CKE_GRP1, REG_LSYS_CKE_GRP2, REG_AON_CLK};
	u32 CenSet[4] = {0};
	u32 ClkRegIndx = 0;

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
		TempVal = HAL_READ32(SYSTEM_CTRL_BASE_LP, CenReg[idx]);
		TempVal |= CenSet[idx] & (~(BIT(31) | BIT(30)));
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, CenReg[idx], TempVal);

		/*  Function configure */
		TempVal = HAL_READ32(SYSTEM_CTRL_BASE_LP, FenReg[idx]);
		TempVal |= FenSet[idx] & (~(BIT(31) | BIT(30)));
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, FenReg[idx], TempVal);
	}

	DelayUs(10);

	/*SDM32K*/
	SDM32K_Enable();
}

/**
  * @brief  set cache way restrict.
  * @param  None.
  * @retval None
  */
BOOT_RAM_TEXT_SECTION
void BOOT_CACHEWRR_Set(void)
{
	if (ENABLE == CACHEWRR_Def[0].WRR_EN) {
		ICache_Way_Set(CACHEWRR_Def[0].WRR_BASE, CACHEWRR_Def[0].WRR_TOP, CACHEWRR_Def[0].WRR);
	}
	if (ENABLE == CACHEWRR_Def[1].WRR_EN) {
		DCache_Way_Set(CACHEWRR_Def[1].WRR_BASE, CACHEWRR_Def[1].WRR_TOP, CACHEWRR_Def[1].WRR);
	}
}

/**
  * @brief  set tcm sharecache memory.
  * @param  TCM_Size: TCM mem size.
  *          This parameter can be one of the following values:
  *           TCM_SIZE_96KB: 96KB TCM, no cache
  *           TCM_SIZE_80KB: 80KB TCM,	0KB D-Cache, 16KB I-Cache
  *           TCM_SIZE_64KB: 64KB TCM, 16KB D-Cache, 16KB I-Cache
  *           TCM_SIZE_48KB: 48KB TCM, 16KB D-Cache, 32KB I-Cache
  *           TCM_SIZE_32KB: 32KB TCM, 16KB D-Cache, 48KB I-Cache
  *           TCM_SIZE_16KB: 16KB TCM, 32KB D-Cache, 48KB I-Cache
  *           TCM_SIZE_0KB :  0KB TCM, 32KB D-Cache, 64KB I-Cache
  * @retval None
  */
BOOT_RAM_TEXT_SECTION
void BOOT_TCMSet(u32 TCM_Size)
{
	u32 Rtemp;
	u32 index = 0;
	for (index = 0; index < 7; index++) {
		if (TCM_Size == CACHETCM_Def[index].TCM_Size) {
			break;
		}
	}

	if (0 == index || 7 == index) {
		/* enable nonsecure cache in app_start */
		return;
	}

	ICache_Invalidate();
	DCache_CleanInvalidate(0xFFFFFFFF, 0xFFFFFFFF);
	Cache_Enable(DISABLE);

	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_STATUS);
	Rtemp |= HSYS_BIT_SHARE_CACHE_MEM;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_STATUS, Rtemp);

	if (CACHE_WWR_4WAY != CACHETCM_Def[index].DWRR) {
		DCache_Way_Set(0x0, 0xffff0000, CACHETCM_Def[index].DWRR);
	}

	if (CACHE_WWR_4WAY != CACHETCM_Def[index].IWRR) {
		ICache_Way_Set(0x0, 0xffff0000, CACHETCM_Def[index].IWRR);
	}

	RIDR->DTCM0_BASE = CACHETCM_Def[index].TCM_BASE;
	RIDR->DTCM0_TOP = CACHETCM_Def[index].TCM_TOP;
	//RTK_LOGD(TAG, "%x = %x, %x = %x\n", &(RIDR->DTCM0_BASE), RIDR->DTCM0_BASE, &(RIDR->DTCM0_TOP), RIDR->DTCM0_TOP);
	if (ENABLE == CACHETCM_Def[index].ICACHE_EN) {
		SCB_EnableICache();
	}
	if (ENABLE == CACHETCM_Def[index].DCACHE_EN) {
		SCB_EnableDCache();
	}
}

BOOT_RAM_TEXT_SECTION
void BOOT_GRstConfig(void)
{
	u16 TempVal = 0;
	u16 Val = 0;

	/* step1: enable pwr cut */
	TempVal = HAL_READ16(SYSTEM_CTRL_BASE_LP, REG_LSYS_PWC);
	TempVal |= LSYS_BIT_POR_HP_DDRPHY | LSYS_BIT_POR_HP_PLAT | LSYS_BIT_POR_LP_PLAT;
	HAL_WRITE16(SYSTEM_CTRL_BASE_LP, REG_LSYS_PWC, TempVal);

	/* delay 40us before release iso, for power stable */
	DelayUs(40);

	/* step2: release iso */
	Val = HAL_READ16(SYSTEM_CTRL_BASE_LP, REG_LSYS_ISO);
	Val &= (~TempVal);
	HAL_WRITE16(SYSTEM_CTRL_BASE_LP, REG_LSYS_ISO, Val);

	/* step 3: release por */
	Val = HAL_READ16(SYSTEM_CTRL_BASE_LP, REG_LSYS_POR);
	Val |= TempVal;
	HAL_WRITE16(SYSTEM_CTRL_BASE_LP, REG_LSYS_POR, Val);
}


/* For read command, hpr > lpr/vpr. If there has hpr requests, lpr/vpr requests will be masked. */
BOOT_RAM_TEXT_SECTION
void BOOT_DDR_LCDC_HPR(void)
{
	u32 ret;
	/* Set LCDC to HPR, ar_pri = 0 means LPR, ar_pri = 1 means VPR */
	/* In AXI xxx_pri sideband signal, ar_pri[1] is defined HPR command in DDRC.BE. */
	ret = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BUS_PRI_CTRL);
	ret &= ~LSYS_MASK_LCDC_ARPRI;
	ret |= LSYS_LCDC_ARPRI(2);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BUS_PRI_CTRL, ret);
}

/* init ddr if needed */
BOOT_RAM_TEXT_SECTION
void BOOT_DDR_Init(void)
{
	u32 temp;

	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL);
	temp |= HSYS_BIT_EXT_MEM_IS_DDR;
	if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_RTLSIM) || \
		(SYSCFG_CHIPType_Get() == CHIP_TYPE_PALADIUM)) {
		temp |= HSYS_BIT_DDRC_SPDUPSIM;
	}
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL, temp);

	/*release DDR isolation*/
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_ISO);
	temp &= ~(LSYS_BIT_ISO_HP_DDRPHY);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_ISO, temp);

	/*DDRPHY init*/
	DDRPHY_InitTypeDef DDRPHY_InitStruct;
	DDR_PHY_StructInit(&DDRPHY_InitStruct);
	DDR_PHY_Init(&DDRPHY_InitStruct);

	/*DDRC init*/
	ddr_init();

	/*DDRPHY Calibration Offline*/
	DDR_PHY_CAL();

#if DDR_AUTOGATING
	DDR_PHY_AutoGating();

	/* The DIW switch dynamically method is the same as before. Setting 0 means off.*/
	//DDRC_DEV->DDRC_DPERF0 = (DDRC_DEV->DDRC_DPERF0 & ~DDRC_MASK_DIW) | DDRC_DIW(diw);
#ifdef CONFIG_LINUX_FW_EN
	rxi316_DynSre_init(0x700, ENABLE);		//for linux lcdc, set the max count, set max to 1800 After ECO
#else
	rxi316_DynSre_init(0x1FF, ENABLE);
#endif
#endif
}

u32 PSRAM_CAL_BK;
/* init psram if needed */
BOOT_RAM_TEXT_SECTION
void BOOT_PSRAM_Init(void)
{
	PSPHY_TypeDef *psram_phy = PSRAMPHY_DEV;
	PSPHY_InitTypeDef PSPHY_InitStruct;
	//PCTL_InitTypeDef PCTL_InitStruct;

	PSRAM_PHY_StructInit(&PSPHY_InitStruct);
	PSRAM_PHY_Init(&PSPHY_InitStruct);

	PSRAM_CTRL_Init();

	if (PsramInfo.Psram_Vendor == MCM_PSRAM_VENDOR_APM) {
		//RTK_LOGD(TAG, "Init APM\r\n");
		PSRAM_APM_DEVIC_Init();
	} else {
		RTK_LOGI(TAG, "Init WB\r\n");
		PSRAM_WB_DEVIC_Init();
	}

	if (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG) & LSYS_BIT_BOOT_WAKE_FROM_PS_HS) {
		psram_phy->PSPHY_CAL_PAR = PSRAM_CAL_BK;
	} else {
		if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC) || (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA)) {
			PSRAM_calibration();
			PSRAM_CAL_BK = psram_phy->PSPHY_CAL_PAR;
		}
	}

	PSRAM_AutoGating(ENABLE, Psram_IDLETIME, Psram_RESUME_TIME / PsramInfo.PSRAMC_Clk_Unit);
}

BOOT_RAM_TEXT_SECTION
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


BOOT_RAM_TEXT_SECTION
void BOOT_SCBVTORBackup_HP(void)
{
	PMC_S_BK.SCBVTORbackup_HP = SCB->VTOR;
}

BOOT_RAM_TEXT_SECTION
void BOOT_SCBVTORReFill_HP(void)
{
	SCB->VTOR = PMC_S_BK.SCBVTORbackup_HP;
}

BOOT_RAM_TEXT_SECTION
void BOOT_NVICBackup_HP(void)
{
	u32 i = 0;

	PMC_S_BK.NVICbackup_HP[0] = NVIC->ISER[0];
	PMC_S_BK.NVICbackup_HP[1] = NVIC->ISER[1];
	PMC_S_BK.NVICbackup_HP[2] = NVIC->ISER[2];

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		PMC_S_BK.NVICIPbackup_HP[i] = NVIC->IPR[i];
	}

	DCache_CleanInvalidate((u32)&PMC_S_BK.NVICbackup_HP[0], sizeof(CPU_S_BackUp_TypeDef));
}

BOOT_RAM_TEXT_SECTION
void BOOT_NVICReFill_HP(void)
{
	u32 i = 0;

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		NVIC->IPR[i] = PMC_S_BK.NVICIPbackup_HP[i];
	}

	/* mask log uart irq */
	NVIC->ISER[0] = (PMC_S_BK.NVICbackup_HP[0] & (~(BIT(UART_LOG_IRQ))));
	NVIC->ISER[1] = PMC_S_BK.NVICbackup_HP[1];
	NVIC->ISER[2] = PMC_S_BK.NVICbackup_HP[2];
}

BOOT_RAM_TEXT_SECTION
u32 BOOT_LoadImages(void)
{
	u8 CertImgIndex;

#ifdef CONFIG_IMG2_FLASH
	if (FALSE == SYSCFG_BootFromNor()) {
		assert_param(0); /* Nand Cannot XIP */
	}
#endif

	/* Load from OTA and ECC check for Certificate and IMG2*/
	CertImgIndex = BOOT_OTA_IMG2();

	RTK_LOGI(TAG, "IMG2 BOOT from OTA %d\n", CertImgIndex + 1);

#ifdef CONFIG_LINUX_FW_EN
	if (Boot_AP_Enbale) {
		/* Load from OTA and ECC check for Linux Image */
		BOOT_OTA_AP_Linux(CertImgIndex);
	}
#endif
	return TRUE;
}

/**
  * @brief  Copy Boot reason to a common register and clear Boot Reason.
  * if Boot Reason is not cleared, it can't get correct boot reason next time.
  * @param  None
  * @retval None.
*/
BOOT_RAM_TEXT_SECTION
void BOOT_ReasonSet(void)
{
	u32 REG_AON_BOOT_REASON_HW_MASK = 0x00ffffff;
	u32 temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON_HW);

	/* keep lower 24bit, high 8 bit reserved for DMA */
	temp &= REG_AON_BOOT_REASON_HW_MASK;

	/*Clear the wake up reason*/
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_BOOT_REASON_HW, temp);

	/*Backup it to system register,So the software can read from the register*/
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_REASON_SW, temp);

	RTK_LOGI(TAG, "KM4 BOOT REASON %x: ", temp);
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET_VD33_POS, "GDET_VD33_POS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET_VD33_NEG, "GDET_VD33_NEG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET2_VD18_POS, "GDET2_VD18_POS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET2_VD18_NEG, "GDET2_VD18_NEG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET1_VD18_POS, "GDET1_VD18_POS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET1_VD18_NEG, "GDET1_VD18_NEG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET_VD09_POS, "GDET_VD09_POS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_GDET_VD09_NEG, "GDET_VD09_NEG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_AP_WARM2PERI, "AP_WARM2PERI");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_KM4_WARM2PERI, "KM4_WARM2PERI");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_KM0_WARM2PERI, "KM0_WARM2PERI");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG4, "WDG4");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG3, "WDG3");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG2, "WDG2");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_WDG1, "WDG1");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_IWDG, "IWDG");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_APSYS, "APSYS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_NPSYS, "NPSYS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_LPSYS, "LPSYS");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_DSLP, "DSLP");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_BOR_ACC, "BOR_ACC");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_BOR, "BOR");
	CHECK_AND_PRINT_FLAG(temp, AON_BIT_RSTF_THM, "THM");
	if (temp == 0) {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "Initial Power on\n");
	} else {
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	}
}

BOOT_RAM_TEXT_SECTION
void BOOT_Enable_KM0(void)
{
	u32 temp;

	/* lp platform master port has three master :KM0 Master, OTPC,SIC
	  * OTPC is selected by default and then KM0 is selected after autoload finished.
	  * Do not allow KM0 boot here, In Case conflict with SIC
	*/
	if (SYSCFG_TRP_TestMode() && !SYSCFG_TRP_OTPBYP()) {

		RTK_LOGI(TAG, "Disable KM0 Boot  in Test mode\r\n");
		return;
	}

	if (SYSCFG_TRP_TestMode() && SYSCFG_TRP_OTPBYP()) {
		RTK_LOGI(TAG, "Bypass OTP autoload\r\n");
	}

	/* Let KM0 run */
	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG);
	temp |= LSYS_BIT_BOOT_KM0_RUN;
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_BOOT_CFG, temp);

	asm volatile("sev");
}

BOOT_RAM_TEXT_SECTION
u32 BOOT_AP_Clk_Get(void)
{
	u32 AP_Clk;

	if (SocClk_Info->AP_CPU_CKD >= ISAPPLL) {
		AP_Clk = SocClk_Info->APPLL_CLK / (SocClk_Info->AP_CPU_CKD & ~ISAPPLL);

	} else {
		AP_Clk = SocClk_Info->NPPLL_CLK / (SocClk_Info->AP_CPU_CKD & ~ISAPPLL);

	}

	return AP_Clk;
}

BOOT_RAM_TEXT_SECTION
void BOOT_SOC_ClkSet(void)
{
	u32 temp;
	u32 PsramDiv, HBusDiv, HPeriDiV;
	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();

	u32 APCLK = BOOT_AP_Clk_Get();
	u32 NPCLK = SocClk_Info->NPPLL_CLK / SocClk_Info->KM4_CPU_CKD;
	u32 LPCLK = 0;
	/*PsramDiv should always 1,psram controller target clk 500M */
	PsramDiv = SocClk_Info->NPPLL_CLK / 400 - 1;
	/*Hbus target clk 100M*/
	HBusDiv =  SocClk_Info->NPPLL_CLK / 100 - 1;
	/*HPeri target clk 200M*/
	HPeriDiV = SocClk_Info->NPPLL_CLK / 200 - 1;

	if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA)) {
		return ;
	}

	/*configure core power according user setting*/
	if (SocClk_Info->Vol_Type == VOL_10) {
		SWR_BST_MODE_Set(ENABLE);
		RRAM->VOL_TYPE = VOL_10;
		assert_param(NPCLK <= KM4_1P0V_CLK_LIMIT);
		if (Boot_AP_Enbale == ENABLE) {
			if ((meminfo.mem_type & MCM_TYPE_PSRAM) == MCM_TYPE_PSRAM) {
				assert_param(APCLK <= AP_1P0V_CLK_LIMIT_PSRAM);
			} else {
				assert_param(APCLK <= AP_1P0V_CLK_LIMIT_DDR);
			}
		}
	} else {
		RRAM->VOL_TYPE = VOL_09;
		assert_param(NPCLK <= KM4_09V_CLK_LIMIT);
		if (Boot_AP_Enbale == ENABLE) {
			assert_param(APCLK <= AP_09V_CLK_LIMIT);
		}
	}

	/* step 1: switch clk to XTAL */
	CLK_SWITCH_XTAL(ENABLE);

	/* step 2: set pll and np clk */
	PLL_NP_ClkSet(SocClk_Info->NPPLL_CLK * 1000000);

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_CKD_GRP0);
	temp &= ~LSYS_MASK_CKD_NP;
	temp &= ~LSYS_MASK_CKD_PSRAM;
	temp &= ~LSYS_MASK_CKD_HBUS;		//AHB
	temp &= ~LSYS_MASK_CKD_HPERI;

	temp |= LSYS_CKD_NP(SocClk_Info->KM4_CPU_CKD - 1);
	temp |= LSYS_CKD_PSRAM(PsramDiv);
	temp |= LSYS_CKD_HBUS(HBusDiv);
	temp |= LSYS_CKD_HPERI(HPeriDiV);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_CKD_GRP0, temp);

	/* step 3: back to pll */
	CLK_SWITCH_XTAL(DISABLE);

	// CONFIG AP clk When AP is Enabled
	if (Boot_AP_Enbale == DISABLE) {
		return;
	}

	/* open ap PLL, Max frequency is 1.12G@0.9V; 1.52G@1.0V In A-Cut, B-cut is 1.08G@0.9V; 1.48G@1.0V*/
	if (SocClk_Info->AP_CPU_CKD & ISAPPLL) {
		PLL_AP_ClkSet(SocClk_Info->APPLL_CLK * 1000000);
		PLL_AP(ENABLE);
		RRAM->APPLL_STATE = TRUE;
	} else {
		/* choose np pll as clk src */
		HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKSL, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKSL) & (~HSYS_MASK_CKSL_AP)) | HSYS_CKSL_AP(AP_CLK_NPPLL));
		RRAM->APPLL_STATE = FALSE;

	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKSL,
				(HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKSL) & ~HSYS_MASK_CKD_AP) | HSYS_CKD_AP(SocClk_Info->AP_CPU_CKD - 1));

	temp = HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_CKSL_GRP0);
	temp = LSYS_GET_CKSL_LSOC(temp);
	LPCLK = temp == OSC4M_CAL_CLK ? 4000000 : XTAL_ClkGet();

	RTK_LOGI(TAG, "NP Freq %lu MHz\n", NPCLK);
	RTK_LOGI(TAG, "AP Freq %lu MHz\n", APCLK);
	RTK_LOGI(TAG, "LP Freq %lu MHz\n", LPCLK / MHZ_TICK_CNT);

	/* Note that if no anti-rollback and warm reset continuously, clear BOOT_CNT to avoid boot from older bootloader */
	BKUP_Write(BKUP_REG0, BKUP_Read(BKUP_REG0) & ~BOOT_CNT_MASK);
}

// 0x1 for core 0, 0x3 for core 0/1
#ifdef CONFIG_LINUX_FW_EN
#define CORE_NUM 0x3	// Always core 0/1 for Linux
#else
#define CORE_NUM 0x1	// RTL sim shall modify this to open core0/1
#endif

BOOT_RAM_TEXT_SECTION
void BOOT_Disable_AP(void)
{
	CA32_TypeDef *ca32 = CA32_BASE;

	/* step 1: set ISO for default ISO error issue */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO) | HSYS_BIT_ISO_HP_AP_TOP));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO) | (HSYS_ISO_HP_AP_CORE(0x3))));

	/* shut down AP L1 Cache, default active mode */
	ca32->CA32_SRAM_CTRL2 |= CA32_MASK_CORE_SRAM_SD;
}

BOOT_RAM_TEXT_SECTION
void BOOT_Enable_AP(void)
{
	CA32_TypeDef *ca32 = CA32_BASE;

	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_4, ENABLE);

	/* step 1: set ISO */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO) | HSYS_BIT_ISO_HP_AP_TOP));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO) | (HSYS_ISO_HP_AP_CORE(0x3))));

	/* step 2: open top power */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC) | HSYS_BIT_PSW_HP_AP_TOP);
	DelayUs(50);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC) | HSYS_BIT_PSW_HP_AP_TOP_2ND);

	/* step 3: open ap l2 cache ram power */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC) | HSYS_BIT_PSW_HP_AP_L2);

	/* step 4: open core 0/1 power */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC) | HSYS_PSW_HP_AP_CORE(CORE_NUM));
	DelayUs(50);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC) | HSYS_PSW_HP_AP_CORE_2ND(CORE_NUM));

	/*delay until power stable*/
	DelayUs(500);

	/* step 5: release isolation */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO) & (~(HSYS_ISO_HP_AP_CORE(CORE_NUM)))));

	/* step 6: release top isolation */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, (HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO) & (~HSYS_BIT_ISO_HP_AP_TOP)));

	/* enable ap clock */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKE, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_CKE) | HSYS_BIT_CKE_AP);

	/* enable ap fen */
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_FEN, HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_FEN) | HSYS_BIT_FEN_AP);

	ca32->CA32_C0_RST_CTRL |= (CA32_NCOREPORESET(CORE_NUM) | CA32_NCORERESET(CORE_NUM) | CA32_BIT_NRESETSOCDBG | CA32_BIT_NL2RESET | CA32_BIT_NGICRESET);
}

BOOT_RAM_TEXT_SECTION
void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = NULL;
	u32 *vector_table = NULL;
	//cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);

	Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

	/* Config Non-Security World Registers Firstly in BOOT_WakeFromPG */
	BOOT_RAM_TZCfg();

	/* refill SCB_VTOR_S after wakeup */
	if (PMC_S_BK.SCBVTORbackup_HP) {
		BOOT_SCBVTORReFill_HP();
	}

	/* refill NVIC registers after wakeup */
	BOOT_NVICReFill_HP();

	/* Enable FPU and Secure/Usage/Mem/Bus Fault */
	BOOT_SCBConfig_HP();

	/*resume cache way restrict setting*/
	BOOT_CACHEWRR_Set();

	/*resume TCM setting*/
	BOOT_TCMSet(Boot_TCM_Size);

	RCC_PeriphClockCmd(APBPeriph_TRNG, APBPeriph_TRNG_CLOCK, ENABLE);
	/* Initial TRNG*/
	TRNG_Init();

	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();
	if (meminfo.mem_type & MCM_TYPE_DDR) {
		RTK_LOGI(TAG, "ReInit DDR\r\n");

		RCC_PeriphClockCmd(APBPeriph_DDRP, APBPeriph_DDRP_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_DDRC, APBPeriph_DDRC_CLOCK, ENABLE);

		BOOT_DDR_Init();
		BOOT_DDR_LCDC_HPR();

	} else {
		RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);
		/* MP ECO, for ddy pad pull down */
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_DUMMY_098, (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_DUMMY_098) | LSYS_BIT_PWDPAD15N_DQ));

		BOOT_PSRAM_Init();

		/* wait self refresh exit */
		DelayUs(150);

	}

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamWakeupFun;
	SCB_NS->VTOR = (u32)vector_table;

	/* Add redefine secure fault handler to vector table* */
	Fault_Hanlder_Redirect(NULL);

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	/* Set non-secure main stack (MSP_NS) */
	//RTK_LOGD(TAG, "NonSecure Start @ 0x%x MSP_NS:%x VCTOR:%x\r\n", vector_table[1], vector_table[0], vector_table);

	/* Start non-secure state software application */
	//RTK_LOGD(TAG, "Start NonSecure @ 0x%x ...\r\n", NonSecure_ResetHandler);
	//RTK_LOGD(TAG, "Cache Enable:%x\r\n", DCache_IsEnabled());
	BOOT_NsStart(vector_table[1]);

	return;
}

BOOT_RAM_TEXT_SECTION
u32 BOOT_Share_Memory_Patch(void)
{
	/* FTC memory workaroud: need enter standy fisrt before enter retention mode */
	/* system 256k sram, WIFI share mem, BT share mem are FTC type */
	u32 Rtemp;
	u32 sum = 0;
	u8 i;

	/* switch share mem control to system */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL);
	Rtemp |= HSYS_BIT_SHARE_WL_MEM | HSYS_BIT_SHARE_BT_MEM;
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL, Rtemp);

	/* access share memory to enter standby */
	/* BT: 0x2200_0000~0x2200_0000+272K = 48K+48K+80K+96K */
	Rtemp = HAL_READ32(HP_SRAM_EXT_BASE, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(HP_SRAM_EXT_BASE + 48 * 1024, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(HP_SRAM_EXT_BASE + 96 * 1024, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(HP_SRAM_EXT_BASE + 176 * 1024, 0x0);
	sum += Rtemp;

	/* WIFI:0x2210_0000~0x2210_0000+40K */
	Rtemp = HAL_READ32(HP_SRAM_EXT_BASE + 0x100000, 0x0);
	sum += Rtemp;

	/* read wifi_share_mem_rsvd to fix hw bug */
	sum += HAL_READ32(HP_SRAM_EXT_BASE + 0x100000 + 40 * 1024, 0x4);

	/* switch share mem control back */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL);
	Rtemp &= ~(HSYS_BIT_SHARE_WL_MEM | HSYS_BIT_SHARE_BT_MEM);
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HPLAT_CTRL, Rtemp);

	/* access system sram */
	for (i = 0; i < 4; i++) {
		Rtemp = HAL_READ32(HP_SRAM0_BASE, 0x10000 * i);
		sum += Rtemp;
	}

	for (i = 0; i < 2; i++) {
		Rtemp = HAL_READ32(LP_SRAM_BASE, 0x10000 * i);
		sum += Rtemp;
	}

	/* calculation sum is just to prevent compiler optimizing the simple read operation */
	return sum;
}

BOOT_RAM_TEXT_SECTION
void BOOT_Log_Init(void)
{
	u32 ChipType;

	/* close AGG function for auto test */
	if (Boot_Agg_En) {
		ChipType = SYSCFG_CHIPType_Get();
		if (!((ChipType == CHIP_TYPE_PALADIUM) || (ChipType == CHIP_TYPE_RTLSIM))) {
			/* open loguart agg function */
			LOGUART_WaitTxComplete();
			LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, DISABLE);
			LOGUART_AGGCmd(LOGUART_DEV, ENABLE);
			LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_1, ENABLE);
		}
	}

	/* open LP log */
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_2, ENABLE);
}

/* To avoid RRAM holding incorrect data, incorporate a MAGIC_NUMBER for verification. */
static bool BOOT_RRAM_InfoValid(void)
{
	if (RRAM->MAGIC_NUMBER != 0x6969A5A5) {
		return FALSE;
	} else {
		return TRUE;
	}
}

//3 Image 1
BOOT_RAM_TEXT_SECTION
void BOOT_Image1(void)
{
	u32 ret;
	u32 *vector_table = NULL;
	PRAM_START_FUNCTION Image2EntryFun = BOOT_SectionInit();
	STDLIB_ENTRY_TABLE *prom_stdlib_export_func = (STDLIB_ENTRY_TABLE *)__rom_stdlib_text_start__;
	RRAM_TypeDef *rram = RRAM;

	_memset((void *) __image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__)); /*clear bss first*/

	BOOT_ReasonSet();

	if ((BOOT_Reason() == 0) || (!BOOT_RRAM_InfoValid())) {
		_memset(RRAM, 0, sizeof(RRAM_TypeDef));
		RRAM->MAGIC_NUMBER = 0x6969A5A5;
	}


	BOOT_VerCheck();

	if (!SYSCFG_OTP_DisBootLog()) {
		LOG_MASK_MODULE(MODULE_BOOT, LEVEL_INFO, ENABLE);
	}

	/*open swr power*/
	SWR_Calib_DCore();

	if (Boot_MemSwr_Only) {
		/* mem swr mode with core */
		SWR_MEM_Manual(DISABLE);
		SWR_MEM(ENABLE);
	} else {
		SWR_MEM(ENABLE);

		/* audio swr mode with core */
		SWR_AUDIO_Manual(DISABLE);
		SWR_AUDIO(ENABLE);
	}

	BOOT_SOC_ClkSet();

	BOOT_GRstConfig();

	/* need about 100-300us, need sync */
	MCM_MemTypeDef meminfo = ChipInfo_MCMInfo();
	RTK_LOGI(TAG, "meminfo.mem_type=0x%x\n", meminfo.mem_type);
	if ((meminfo.mem_type & MCM_TYPE_PSRAM) == MCM_TYPE_PSRAM) {
		/* off ddrphy BG for psram chip, open by USB AND MIPI when need */
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1, HAL_READ32(SYSTEM_CTRL_BASE_LP,
					REG_LSYS_AIP_CTRL1) & (~(LSYS_BIT_BG_PWR | LSYS_BIT_BG_ON_MIPI | LSYS_BIT_BG_ON_USB2)));
		rram->MEM_TYPE = MCM_TYPE_PSRAM;
		RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_DUMMY_098, (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_DUMMY_098) | LSYS_BIT_PWDPAD15N_DQ));

	} else {
		/* off USB AND MIPI by default, open in IP */
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1, HAL_READ32(SYSTEM_CTRL_BASE_LP,
					REG_LSYS_AIP_CTRL1) & (~(LSYS_BIT_BG_ON_MIPI | LSYS_BIT_BG_ON_USB2)));
		rram->MEM_TYPE = MCM_TYPE_DDR;
		RCC_PeriphClockCmd(APBPeriph_DDRP, APBPeriph_DDRP_CLOCK, ENABLE);
		RCC_PeriphClockCmd(APBPeriph_DDRC, APBPeriph_DDRC_CLOCK, ENABLE);
		HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_DUMMY_098, (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_DUMMY_098)  | LSYS_BIT_PWDPAD15N_DQ | LSYS_BIT_PWDPAD15N_CA));

	}

	//4200825c[1:0]=2'b10, change BandGap from 0x3 to 0x2
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1, (HAL_READ32(SYSTEM_CTRL_BASE_LP, REG_LSYS_AIP_CTRL1) & ~LSYS_MASK_BG_ALL) | LSYS_BG_ALL(0x2));

	BOOT_Log_Init();

	RTK_LOGI(TAG, "IMG1 ENTER MSP:[%08x]\n", __get_MSP());
	RTK_LOGI(TAG, "Build Time: %s %s\n", __DATE__, __TIME__);

	cmse_address_info_t cmse_address_info = cmse_TT((void *)BOOT_Image1);
	RTK_LOGI(TAG, "IMG1 SECURE STATE: %d\n", cmse_address_info.flags.secure);

	/* init stdlib bss and load stdlib data */
	prom_stdlib_export_func->stdlib_init();

	BOOT_RccConfig();

	BOOT_CACHEWRR_Set();
	BOOT_TCMSet(Boot_TCM_Size);

	/* backup flash_init_para address for KM0 */
	DCache_Clean((u32)&flash_init_para, sizeof(FLASH_InitTypeDef));
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_FLASH_PARA_ADDR, (u32)&flash_init_para);

	//RRAM->IPC_Share_Mem = (u32) __km0_ipc_memory_start__;
	RRAM->IMQ_INIT_DONE = 0;

	flash_highspeed_setup();
	RTK_LOGI(TAG, "(meminfo.mem_type==0x%x\n", meminfo.mem_type);

	if ((meminfo.mem_type & MCM_TYPE_PSRAM) == MCM_TYPE_PSRAM) {
		RTK_LOGI(TAG, "Init PSRAM\r\n");
		PSRAM_INFO_Update(); //only when boot
		BOOT_PSRAM_Init();
		if (!Boot_MemSwr_Only) {
			// if Boot_MemSwr_Only, accompany core, or keep in PFM
			if (SWR_MEM_Mode_Set(SWR_PFM)) {
				RTK_LOGW(TAG, "set pfm fail\r\n");
			}
		}
	} else {
		if (Boot_MemSwr_Only) {
			while (1) {
				RTK_LOGE(TAG, "ERROR!! Should Not enable MemSwr_Only in DDR Chip!!!\r\n");
				DelayMs(5000);
			}
		}
		if (ChipInfo_DDRType() == MCM_DDR2) {
			RTK_LOGI(TAG, "Init DDR2\r\n");
		} else {
			RTK_LOGI(TAG, "Init DDR3\r\n");
		}
		BOOT_DDR_Init();
		BOOT_DDR_LCDC_HPR();
	}

	/* Enable All Mask in SysReset Domain */
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_SYSRST_MSK0, 0x0);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_SYSRST_MSK1, 0x0);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_LSYS_SYSRST_MSK2, 0x0);
	HAL_WRITE32(SYSTEM_CTRL_BASE_LP, REG_AON_SYSRST_MSK, 0x0);

	BOOT_Share_Memory_Patch();

	ret = BOOT_LoadImages();
	if (ret == FALSE) {
		goto INVALID_IMG2;
	}

	/* Config Non-Security World Registers and clean Dcache */
	BOOT_RAM_TZCfg();

	/*switch shell control to KM0, disable loguart interrupt to avoid loguart irq not assigned in non-secure world */
	LOGUART_INTConfig(LOGUART_DEV, LOGUART_BIT_ERBI, DISABLE);
	InterruptDis(UART_LOG_IRQ);
	BOOT_Enable_KM0();

	/* AP Power-on, AP start run */
	if (Boot_AP_Enbale) {
		BOOT_Enable_AP();
		/* indicate AP is running */
		HAL_WRITE8(SYSTEM_CTRL_BASE_LP, REG_LSYS_AP_STATUS_SW,
				   HAL_READ8(SYSTEM_CTRL_BASE_LP, REG_LSYS_AP_STATUS_SW) | LSYS_BIT_AP_RUNNING | LSYS_BIT_AP_ENABLE);
	} else {
		BOOT_Disable_AP();
	}
	/* indicate KM4 is running */
	HAL_WRITE8(SYSTEM_CTRL_BASE_LP, REG_LSYS_NP_STATUS_SW,
			   HAL_READ8(SYSTEM_CTRL_BASE_LP, REG_LSYS_NP_STATUS_SW) | LSYS_BIT_NP_RUNNING);

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamStartFun;
	SCB_NS->VTOR = (u32)vector_table;

	/* Add redefine secure fault handler to vector table* */
	Fault_Hanlder_Redirect(NULL);
	/* Enable FPU and Secure/Usage/Mem/Bus Fault */
	BOOT_SCBConfig_HP();

	/* it will be reset after PG, backup SCB_VTOR_S */
	BOOT_SCBVTORBackup_HP();

	/* backup NVIC_S */
	BOOT_NVICBackup_HP();

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	/* Start non-secure state software application */
	RTK_LOGI(TAG, "Start NonSecure @ 0x%x ...\r\n", vector_table[1]);

	BOOT_NsStart(vector_table[1]);

	return;

INVALID_IMG2:
	while (1) {
		DelayMs(1000);//each delay is 100us
	}
}

IMAGE1_VALID_PATTEN_SECTION
const u8 RAM_IMG1_VALID_PATTEN[] = {
	0x23, 0x79, 0x16, 0x88, 0xff, 0xff, 0xff, 0xff
};

IMAGE1_EXPORT_SYMB_SECTION
BOOT_EXPORT_SYMB_TABLE boot_export_symbol = {
	.rdp_decrypt_func = NULL,
};

IMAGE1_ENTRY_SECTION
RAM_FUNCTION_START_TABLE RamStartTable = {
	.RamStartFun = NULL,
	.RamWakeupFun = NULL,
	.RamPatchFun0 = BOOT_WakeFromPG,
	.RamPatchFun1 = NULL,
	.RamPatchFun2 = NULL,
	.FlashStartFun = BOOT_Image1,
	.ExportTable = &boot_export_symbol
};
