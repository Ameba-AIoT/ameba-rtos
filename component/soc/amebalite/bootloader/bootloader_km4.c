/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

#include "ameba_soc.h"
#include "ameba_secure_boot.h"
#include "bootloader_km4.h"
#include "boot_ota_km4.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *TAG = "BOOT";
typedef struct {
	u32 NVICbackup_HP[6];
	u32 SCBVTORbackup_HP;
	u8 NVICIPbackup_HP[MAX_PERIPHERAL_IRQ_NUM];
} CPU_S_BackUp_TypeDef;

CPU_S_BackUp_TypeDef PMC_S_BK;

PRAM_START_FUNCTION BOOT_SectionInit(void)
{
	return (PRAM_START_FUNCTION)__image2_entry_func__;
}

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

#ifndef CONFIG_TRUSTZONE
	FuncPtr pFunc = (FuncPtr)Addr;
	pFunc();
#endif

	/* jump to ns world */
	nsfunc *fp = cmse_nsfptr_create(Addr);
	fp();
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
	TempVal |= APBPeriph_GDMA;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_FEN_GRP0, TempVal);

	TempVal = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0);
	TempVal |= APBPeriph_GDMA_CLOCK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP0, TempVal);

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

/* init psram if needed */
u32 BOOT_PSRAM_Init(void)
{
	//PCTL_InitTypeDef PCTL_InitStruct;
	PSPHY_InitTypeDef PSPHY_InitStruct;
	PSRAM_INFO_Update();

	if (PsramInfo.Psram_Vendor != PSRAM_VENDOR_NONE) {
		PSRAM_CTRL_SPU(ENABLE);
		RCC_PeriphClockCmd(APBPeriph_PSRAM, APBPeriph_PSRAM_CLOCK, ENABLE);

		/* init psram phy */
		PSRAM_PHY_StructInit(&PSPHY_InitStruct);
		PSRAM_PHY_Init(&PSPHY_InitStruct);
		PSRAM_CTRL_Init();

		if (PsramInfo.Psram_Vendor == PSRAM_VENDOR_APM) {
			RTK_LOGI(TAG, "Init APM PSRAM\r\n");
			/* init psram device */
			PSRAM_APM_DEVIC_Init();

		} else {
			RTK_LOGI(TAG, "Init WB PSRAM\r\n");
			/* init psram device */
			PSRAM_WB_DEVIC_Init();
		}

		/* psram calibration */
		if ((SYSCFG_CHIPType_Get() == CHIP_TYPE_ASIC) || (SYSCFG_CHIPType_Get() == CHIP_TYPE_FPGA)) {
			if (PSRAM_calibration(DISABLE) == FALSE) {
				PSRAM_InfoDump();
				PSRAM_calibration(ENABLE);
				return FALSE;
			}
		}

		/* enable auto gating */
		PSRAM_AutoGating(ENABLE, Psram_IDLETIME_PLL, PsramInfo.Psram_Resume_Cnt);
	}

	return TRUE;
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

void BOOT_SCBVTORBackup_HP(void)
{
	PMC_S_BK.SCBVTORbackup_HP = SCB->VTOR;
}

void BOOT_SCBVTORReFill_HP(void)
{
	SCB->VTOR = PMC_S_BK.SCBVTORbackup_HP;
}

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

void BOOT_NVICReFill_HP(void)
{
	u32 i = 0;

	for (i = 0; i < MAX_PERIPHERAL_IRQ_NUM; i++) {
		NVIC->IPR[i] = PMC_S_BK.NVICIPbackup_HP[i];
	}

	/* mask log uart irq in secure world when wake */
	NVIC->ISER[0] = PMC_S_BK.NVICbackup_HP[0];
	NVIC->ISER[1] = (PMC_S_BK.NVICbackup_HP[1] & (~(BIT(UART_LOG_IRQ % 32))));
	NVIC->ISER[2] = PMC_S_BK.NVICbackup_HP[2];
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

	/*Clear the wake up reason*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_AON_BOOT_REASON_HW, temp);

	/*Backup it to system register,So the software can read from the register*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_REASON_SW, temp);

	RTK_LOGI(TAG, "KM4 BOOT REASON: %lx \n", temp);

}

void BOOT_Enable_KR4(void)
{
	/* Kr4 start table bacukp status Init*/
	RRAM_DEV->kr4_BackupTbl_Done = 0;

	/* Let KR4 run */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_BOOT_CFG) | LSYS_BIT_BOOT_KR4_RUN);

	/*If KR4_L2MEM_EN is not set, kr4 cannot access rom*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL) | LSYS_BIT_KR4_L2MEM_EN);
	RCC_PeriphClockCmd(APBPeriph_KR4, APBPeriph_KR4_CLOCK, ENABLE);
}


void BOOT_WakeFromPG(void)
{
	PRAM_START_FUNCTION Image2EntryFun = NULL;
	u32 *vector_table = NULL;
	//cmse_address_info_t cmse_address_info = cmse_TT((void *)DiagPrintf);

	//RTK_LOGI(TAG, "%s\n", __func__);
	Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;

	/* Config Non-Security World Registers Firstly in BOOT_WakeFromPG */
	BOOT_RAM_TZCfg();

	if (!LSYS_GET_KR4_IS_NP(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1))) {

		if (SYSCFG_OTP_FlashDSleepEn()) {
			FLASH_DeepPowerDown(DISABLE);
			/* to fix hw bug: between flash user read and auto read wrap, need single auto read, otherwise SPIC may return error */
			HAL_READ32(SPI_FLASH_BASE, 0);
		}

		set_psram_resume();
	}

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamWakeupFun;
	SCB_NS->VTOR = (u32)vector_table;
#ifdef CONFIG_DEBUG_CRASHDUMP_IMG1
	/* Add redefine secure fault handler to vector table* */
	Fault_Hanlder_Redirect(NULL);
#endif
	/* refill SCB_VTOR_S after wakeup */
	if (PMC_S_BK.SCBVTORbackup_HP) {
		BOOT_SCBVTORReFill_HP();
	}

	/* refill NVIC registers after wakeup */
	BOOT_NVICReFill_HP();

	/* Enable FPU and Secure/Usage/Mem/Bus Fault */
	BOOT_SCBConfig_HP();

	/* Set non-secure main stack (MSP_NS) */
	__TZ_set_MSP_NS(MSP_RAM_HP_NS);

	/* Set PSPS Temp */
	__set_PSP(MSP_RAM_HP_NS - 2048);

	/* Set non-secure main stack (MSP_NS) */
	//RTK_LOGI(TAG, "NonSecure Start @ 0x%lx MSP_NS:%lx VCTOR:%lx\r\n", vector_table[1], vector_table[0], vector_table);

	/* Start non-secure state software application */
	//RTK_LOGI(TAG, "Start NonSecure @ 0x%lx ...\r\n", NonSecure_ResetHandler);
	//RTK_LOGI(TAG, "Cache Enable:%lx\r\n", DCache_IsEnabled());
	BOOT_NsStart(vector_table[1]);

	return;
}


/**
  * @brief Get BD numer form efuse.
  * @retval BD number
  */
u32 BOOT_ChipInfo_PSRAMType(void)
{
	u32 memoryinfo = ChipInfo_PSRAMType();
	u32 Psram_clk_Max = PSRAM_CLK_LIMIT_GET(memoryinfo);
	SocClk_Info_TypeDef *pSocClk_Info;
	u32 PsramSetClk = 0;
	u32 ret;

	/*Boot_SocClk_Info_Idx is valid*/
	if (Boot_SocClk_Info_Idx != 0xFF) {
		pSocClk_Info = &SocClk_Info[Boot_SocClk_Info_Idx];

		if (pSocClk_Info->PSRAMC_CKD & ISPLLD) {
			PsramSetClk = pSocClk_Info->PLLD_CLK / (pSocClk_Info->PSRAMC_CKD & (~ISPLLD)) / 2;
		} else {
			PsramSetClk = pSocClk_Info->PLLM_CLK / pSocClk_Info->PSRAMC_CKD / 2;
		}
	}

	/* Boot_Clk_Config_Level shall get from ChipInfo_PSRAMType() */
	if (Psram_clk_Max == 0) { //None PSRAM
#ifndef CONFIG_CODE_XIP_DATA_SRAM
		assert_param(0);//Code shall XIP in no psram chip
#endif
		ret = 0;/*0.9V, PSRAM-166M*/
	} else if (Psram_clk_Max == PSRAM_DEVICE_CLK_166) {
		assert_param(PsramSetClk <= 166 * MHZ_TICK_CNT);
		ret = 1;/*1.0V, PSRAM-166M*/
	} else if (Psram_clk_Max == PSRAM_DEVICE_CLK_200) {
		assert_param(PsramSetClk <= 200 * MHZ_TICK_CNT);
		ret = 2;/*1.0V, PSRAM-200M*/
	} else if (Psram_clk_Max == PSRAM_DEVICE_CLK_250) {
		assert_param(PsramSetClk <= 250 * MHZ_TICK_CNT);
		ret = 3;/*1.0V, PSRAM-250M*/
	} else {
		RTK_LOGW(TAG, "Handle QFN144 or New Chip as 0.9V, PSRAM-166M\n");
		ret = 0;/*0.9V, PSRAM-166M*/
	}

	if (Boot_SocClk_Info_Idx != 0xFF) {
		/*Boot_SocClk_Info_Idx is valid, use user config socclk*/
		return Boot_SocClk_Info_Idx;
	} else {
		return ret;
	}
}

void BOOT_SOC_ClkChk(SocClk_Info_TypeDef *pSocClk_Info)
{
	u32 CpuClk, PsramClk;

	u32 PllMClk = pSocClk_Info->PLLM_CLK;
	u32 PllDClk = pSocClk_Info->PLLD_CLK;

	u32 chipinfo = ChipInfo_PSRAMType();
	PsramInfo.Psram_Vendor = PSRAM_VENDOR_GET(chipinfo);
	assert_param(PllMClk <= PLL_600M);
	assert_param(PllMClk >= PLL_330M);
	assert_param(PllDClk <= PLL_600M);
	assert_param(PllDClk >= PLL_330M);
	assert_param(pSocClk_Info->CPU_CKD);
	assert_param(pSocClk_Info->PSRAMC_CKD);

	if (pSocClk_Info->CPU_CKD & ISPLLD) {
		CpuClk = PllDClk / (pSocClk_Info->CPU_CKD & (~ISPLLD));
	} else {
		CpuClk = PllMClk / pSocClk_Info->CPU_CKD;
	}

	if (pSocClk_Info->PSRAMC_CKD & ISPLLD) {
		PsramClk = PllDClk / (pSocClk_Info->PSRAMC_CKD & (~ISPLLD));
	} else {
		PsramClk = PllMClk / pSocClk_Info->PSRAMC_CKD;
	}

	/* Clk info limit check */
	if (pSocClk_Info->Vol_Type == CORE_VOL_0P9) {
		assert_param(CpuClk <= CPU_0P9V_CLK_LMT_BCUT);
	} else {
		assert_param(CpuClk <= CPU_1P0V_CLK_LMT_BCUT);
	}

	assert_param(PsramClk <= PSRAMC_CLK_LIMIT);
	RTK_LOGI(TAG, "KM4 CPU CLK: %lu Hz \n", CpuClk);
	RTK_LOGI(TAG, "KR4 CPU CLK: %lu Hz \n", CpuClk);
	if (PsramInfo.Psram_Vendor != PSRAM_VENDOR_NONE) {
		RTK_LOGI(TAG, "PSRAM Ctrl CLK: %lu Hz \n", PsramClk);
	}

}

void BOOT_SocClk_Update(void)
{
	u32 Temp;
	u32 PllMClk, PllDClk;
	u32 HbusDiV, EcdsaDiv, CpuDiv;
	SocClk_Info_TypeDef *pSocClk_Info;
	RRAM_TypeDef *rram = RRAM_DEV;

	u32 Boot_Clk_Config_Level = BOOT_ChipInfo_PSRAMType();
	pSocClk_Info = &SocClk_Info[Boot_Clk_Config_Level];

	BOOT_SOC_ClkChk(pSocClk_Info);

	PllMClk = pSocClk_Info->PLLM_CLK;
	PllDClk = pSocClk_Info->PLLD_CLK;

	// km4:Sram shall be 1:1 or 2:1
	if (pSocClk_Info->Vol_Type == CORE_VOL_0P9) {
		HbusDiV = CLKDIV_ROUND_UP(PllMClk, HBUS_0P9V_CLK_LIMIT);/*target clk HBUS_0P9V_CLK_LIMIT*/
	} else {
		HbusDiV = CLKDIV_ROUND_UP(PllMClk, HBUS_1P0V_CLK_LIMIT);/*target clk HBUS_1P0V_CLK_LIMIT*/
	}
	EcdsaDiv = CLKDIV_ROUND_UP(PllMClk, ECDSA_CLK_LIMIT);/*target clk ECDSA_CLK_LIMIT*/
	CpuDiv = pSocClk_Info->CPU_CKD;

	//0. configure core power according user setting
	SWR_CORE_Vol_Set(pSocClk_Info->Vol_Type);

	//1. select clk_lbus
	CPU_ClkSet_NonOS(CLK_CPU_LBUS);

	//2. gated selected PLL clock output (1-dsp_pll; 0-cpu_pll)
	PLL_TypeDef *PLL = (PLL_TypeDef *)PLL_BASE;
	PLL->PLL_CPUPLL_CTRL0 &= ~PLL_BIT_CPUPLL_CK_EN;
	PLL->PLL_PERIPLL_CTRL0 &= ~PLL_BIT_PERIPLL_CK_EN;

	//3. modify PLL clock
	PLL_ClkSet(CLK_CPU_MPLL, PllMClk);
	PLL_ClkSet(CLK_CPU_DPLL, PllDClk);
	rram->PLLD_Clk_Info = PllDClk;

	//4. Disable km4/kr4/sram divn
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1);
	Temp &= ~APBPeriph_PLL_CLOCK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1, Temp);

	//5. enable DSP/CPU PLL clock output (1-dsp_pll; 0-cpu_pll)
	PLL->PLL_CPUPLL_CTRL0 |= PLL_BIT_CPUPLL_CK_EN;
	PLL->PLL_PERIPLL_CTRL0 |= PLL_BIT_PERIPLL_CK_EN;

	//6. change ckd of km4/kr4/sram according to voltage
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0);
	Temp &= ~(LSYS_MASK_CKD_KM4 | LSYS_MASK_CKD_KR4 | LSYS_MASK_CKD_SRAM);
	Temp &= ~(LSYS_MASK_CKD_HBUS | LSYS_MASK_CKD_GDMA_AXI);
	Temp &= ~LSYS_MASK_CKD_PSRAM;

	/* Note: for sram, max 240MHz under 1.0v, max 200MHz under 0.9v, and km4:Sram shall be 1:1 or 2:1 */
	Temp |= LSYS_CKD_KM4(CpuDiv - 1) | LSYS_CKD_KR4(CpuDiv - 1) | LSYS_CKD_SRAM(CpuDiv - 1);
	Temp |= LSYS_CKD_HBUS(HbusDiV - 1) | LSYS_CKD_GDMA_AXI(HbusDiV - 1);
	Temp |= LSYS_CKD_PSRAM(pSocClk_Info->PSRAMC_CKD - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP0, Temp);

	if (pSocClk_Info->PSRAMC_CKD & ISPLLD) {
		RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_DSPPLL);
	} else {
		RCC_PeriphClockSource_PSRAM(BIT_LSYS_CKSL_PSRAM_CPUPLL);
	}

	/* change ckd of ecdsa */
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1);
	Temp = (Temp & ~LSYS_MASK_CKD_ECDSA) | LSYS_CKD_ECDSA(EcdsaDiv - 1);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKD_GRP1, Temp);

	//7. enable km4/kr4/sram divn
	Temp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1);
	Temp |= APBPeriph_PLL_CLOCK;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_CKE_GRP1, Temp);

	//8. select divn_clk
	CPU_ClkSet_NonOS(CLK_CPU_MPLL);

}

void BOOT_Config_PMC_Role(void)
{
	RRAM_TypeDef *rram = RRAM_DEV;

#if defined(CONFIG_AS_INIC_NP) /*KM4 as PMC NP*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1) & (~LSYS_BIT_KR4_IS_NP));
	rram->PMC_CORE_ROLE_Flag = PMC_CORE_ROLE_AP2NP;
#elif defined(CONFIG_AS_INIC_AP) /*KM4 as PMC AP*/
	rram->PMC_CORE_ROLE_Flag = PMC_CORE_ROLE_AP2NP;
#elif defined(CONFIG_SINGLE_CORE_WIFI) /*KM4 as PMC NP*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1) & (~LSYS_BIT_KR4_IS_NP));
	rram->PMC_CORE_ROLE_Flag = PMC_CORE_ROLE_SINGLE;
#else /*KR4 as PMC NP*/
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1) | (LSYS_BIT_KR4_IS_NP));
	rram->PMC_CORE_ROLE_Flag = PMC_CORE_ROLE_SINGLE;
#endif

	rram->AP_WAKEUP_STATUS = 0;

	RTK_LOGI(TAG, "PMC_CORE_ROLE: %d (0 represents AP2NP), NP: %lu (1 represents KR4 is NP) \n", rram->PMC_CORE_ROLE_Flag,
			 LSYS_GET_KR4_IS_NP(HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_SYSTEM_CFG1)));
}

u32 BOOT_Share_Memory_Patch(void)
{
	/* FTC memory workaroud: need enter standy fisrt before enter retention mode */
	/* system 512k sram, WIFI share mem, BT share mem are FTC type */
	u32 Rtemp;
	u32 sum = 0;
	u8 i;

	/* switch share mem control to system */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL);
	Rtemp |= LSYS_BIT_SHARE_BT_MEM | LSYS_BIT_SHARE_WL_MEM;
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL, Rtemp);
	/* access share memory to enter standby */
	Rtemp = HAL_READ32(SHARE_MEM_BTOFF_E0_ADDRESS, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(SHARE_MEM_BTOFF_E1_ADDRESS, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(SHARE_MEM_BTON_E0_ADDRESS, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(SHARE_MEM_BTON_E1_ADDRESS, 0x0);
	sum += Rtemp;

	Rtemp = HAL_READ32(SHARE_MEM_WIFI_TXPKTBUF_ADDRESS, 0x0);
	sum += Rtemp;
	Rtemp = HAL_READ32(SHARE_MEM_WIFI_RXPKTBUF_ADDRESS, 0x0);
	sum += Rtemp;
	/* switch share mem control back */
	Rtemp = HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL);
	Rtemp &= ~(LSYS_BIT_SHARE_BT_MEM | LSYS_BIT_SHARE_WL_MEM);
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_PLAT_CTRL, Rtemp);

	/* access system sram */
	for (i = 0; i < 8; i++) {
		Rtemp = HAL_READ32(HP_SRAM0_BASE, 0x10000 * i);
		sum += Rtemp;
	}

	/* calculation sum is just to prevent compiler optimizing the simple read operation */
	return sum;
}

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

	/* open KR4 log */
	LOGUART_AGGPathCmd(LOGUART_DEV, LOGUART_PATH_INDEX_2, ENABLE);
}

//3 Image 1
void BOOT_Image1(void)
{
	u32 *vector_table = NULL;
	PRAM_START_FUNCTION Image2EntryFun = (PRAM_START_FUNCTION)__image2_entry_func__;
	//STDLIB_ENTRY_TABLE *prom_stdlib_export_func = (STDLIB_ENTRY_TABLE *)__rom_stdlib_text_start__;

	RBSS_UDELAY_CLK = CPU_ClkGet() / MHZ_TICK_CNT;

	_memset((void *) __image1_bss_start__, 0, (__image1_bss_end__ - __image1_bss_start__));

	BOOT_ReasonSet();

	if (BOOT_Reason() == 0) {
		memset(RRAM_DEV, 0, sizeof(RRAM_TypeDef));
	}

	/* enable IPC first for otp read */
	RCC_PeriphClockCmd(APBPeriph_IPC, APBPeriph_IPC_CLOCK, ENABLE);

	BOOT_VerCheck();

	/* Set SOC Clock according to core voltage */
	BOOT_SocClk_Update();

	/* open analog LDO */
	LDO_PSRAM(ENABLE);

	BOOT_Log_Init();

	RTK_LOGI(TAG, "IMG1 ENTER MSP:[%08lx]\n", __get_MSP());
	RTK_LOGI(TAG, "Build Time: %s %s\n", __DATE__, __TIME__);

	cmse_address_info_t cmse_address_info = cmse_TT((void *)BOOT_Image1);
	RTK_LOGI(TAG, "IMG1 SECURE STATE: %d\n", cmse_address_info.flags.secure);

	/* init stdlib bss and load stdlib data */
	//prom_stdlib_export_func->stdlib_init();

	BOOT_SectionInit();

	BOOT_RccConfig();

	flash_highspeed_setup();

	/* backup flash_init_para address for KR4 */
	DCache_Clean((u32)&flash_init_para, sizeof(FLASH_InitTypeDef));
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_FLASH_PARA_ADDR, (u32)&flash_init_para);

	/* share ipc memory between internal core by global register */
	RRAM_DEV->IPC_Share_Mem = (u32) __kr4_ipc_memory_start__;
	RRAM_DEV->IMQ_INIT_DONE = 0;
	RRAM_DEV->IMQ2_INIT_DONE = 0;


	BOOT_PSRAM_Init();

	BOOT_SRAM_Extend_Config();
	BOOT_Share_Memory_Patch();

	/*disable loguart interrupt to avoid loguart irq not assigned in non-secure world */
	LOGUART_INTConfig(LOGUART_DEV, LOGUART_BIT_ERBI, DISABLE);
	InterruptDis(UART_LOG_IRQ);

	BOOT_Config_PMC_Role();
#ifdef CONFIG_SOC_SOLO
	u8 ret = BOOT_Load_KR4_IMG1();
	BOOT_RAM_TZCfg(); /* Config Non-Security World & Clean DCache */
	if (TRUE == ret) {
		BOOT_Enable_KR4();
	}
	BOOT_Load_KM4_APP();
#else
	BOOT_OTA_IMG2();
	BOOT_RAM_TZCfg(); /* Config Non-Security World & Clean DCache */

	/* Swap KR4 Boot Entry(0x20040020 ~0x20040040) and KR4 Backup area contents for KR4 booting.*/
	SOCPS_Kr4_StartTbl_Backup();

	/*KR4 shall wait MPC setting for non-secure access*/
	BOOT_Enable_KR4();
	/* Wait until KR4 start table bacukp done */
	while (RRAM_DEV->kr4_BackupTbl_Done != 1) {
		DelayUs(1);
	}
#endif

	/* This Function Must be Called to Open DSP or Not After KR4 Backup Table done */
	BOOT_Enable_Config();

	vector_table = (u32 *)Image2EntryFun->VectorNS;
	vector_table[1] = (u32)Image2EntryFun->RamStartFun;
	SCB_NS->VTOR = (u32)vector_table;
#ifdef CONFIG_DEBUG_CRASHDUMP_IMG1
	/* Add redefine secure fault handler to vector table* */
	Fault_Hanlder_Redirect(NULL);
#endif
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
	RTK_LOGI(TAG, "Start IMG2 @ 0x%lx ...\r\n", vector_table[1]);

	BOOT_NsStart(vector_table[1]);

	return;
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
	.RamWakeupFun = NULL,
	.RamPatchFun0 = BOOT_WakeFromPG,
	.RamPatchFun1 = NULL,
	.RamPatchFun2 = NULL,
	.FlashStartFun = BOOT_Image1,
	.ExportTable = &boot_export_symbol
};

