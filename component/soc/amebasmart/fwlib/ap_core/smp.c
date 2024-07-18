/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */

#include "sheipa.h"
#include "ameba_soc.h"
#include "FreeRTOS.h"

static const char *TAG = "#";
extern void _boot(void);
extern void vPortRestoreTaskContext(void);

/*-----------------------------------------------------------*/

void rtk_core1_power_on(void)
{
	u32 val;
	CA32_TypeDef *ca32 = CA32_BASE;

	ca32->CA32_C0_RST_CTRL &= (~(CA32_NCOREPORESET(0x2) | CA32_NCORERESET(0x2)));

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO);
	val |= (HSYS_ISO_HP_AP_CORE(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, val);
	DelayUs(50);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val |= (HSYS_PSW_HP_AP_CORE(0x3));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);
	DelayUs(50);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val |= (HSYS_PSW_HP_AP_CORE_2ND(0x3));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);
	DelayUs(500);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO);
	val &= ~(HSYS_ISO_HP_AP_CORE(0x3));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, val);
	DelayUs(50);

	ca32->CA32_C0_RST_CTRL |= (CA32_NCOREPORESET(0x2) | CA32_NCORERESET(0x2) | CA32_BIT_NRESETSOCDBG | CA32_BIT_NL2RESET | CA32_BIT_NGICRESET);

}


void rtk_core1_power_off(void)
{
	u32 val;

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO);
	val |= (HSYS_ISO_HP_AP_CORE(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_ISO, val);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val &= ~(HSYS_PSW_HP_AP_CORE_2ND(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);

	val =  HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC);
	val &= ~(HSYS_PSW_HP_AP_CORE(0x2));
	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_HP_PWC, val);
}

void vPortGateOtherCore(void)
{
#if ( configNUM_CORES > 1 )
	if (rtos_sched_get_state() == RTOS_SCHED_NOT_STARTED) {
		/* If os not start schedule, cpu1 cannot handle interrupt */
		return;
	}

	ulFlashPG_Flag = 1;
	__DSB();

	UBaseType_t ulCoreID = portGET_CORE_ID();
	ulCoreID = (ulCoreID + 1) % configNUM_CORES;
	arm_gic_raise_softirq(ulCoreID, IPI_FLASHPG_IRQ);

	CA32_TypeDef *ca32 = CA32_BASE;
	/* Ensure WFE is entered by IPI_FLASHPG_IRQ */
	while ((ulFlashPG_Flag == 1) || (CA32_GET_STANDBYWFE(ca32->CA32_C0_CPU_STATUS) != BIT(ulCoreID)));
#endif
}

void vPortWakeOtherCore(void)
{
	ulFlashPG_Flag = 0;
	__DSB();
	__SEV();
}

void vPortSecondaryOff(void)
{
#if ( configNUM_CORES > 1 )
	int state;
	int count = 10;

	/* Notify secondary core to migrate task to primary core and enter wfi*/
	pmu_set_secondary_cpu_state(1, CPU1_HOTPLUG);
	arm_gic_raise_softirq(1, 1);
	//add a delay to wait cpu1 enter wfi.
	DelayUs(100);

	/* Power off secondary core */
	do {
		state  = psci_affinity_info(1, 0);
		if (state == 1) {
			RTK_LOGS(TAG, "cpu1 power off\n");
			rtk_core1_power_off();
			return;
		}

		DelayUs(50);
	} while (count--);

	RTK_LOGS(TAG, "Secondary core power off fail: %d\n", state);
#endif
}

void vPortSecondaryStart(void)
{
	/* Wait until scheduler starts */
	if (pmu_get_secondary_cpu_state(portGET_CORE_ID()) == CPU1_RUNNING)
		while (rtos_sched_get_state() == RTOS_SCHED_NOT_STARTED);

	RTK_LOGS(TAG, "CPU%d: on\n", (int)portGET_CORE_ID());
#if ( configNUM_CORES > 1 )
	/* Configure the hardware ready to run the demo. */
	prvSetupHardwareSecondary();

	/* Enable ipi for yield core */
	configSETUP_IPI_INTERRUPT();
#endif
	/* Start the timer that generates the tick ISR. */
	configSETUP_TICK_INTERRUPT();

	/* Secondary core is up, set cpu state to CPU1_RUNNING */
	pmu_set_secondary_cpu_state(1, CPU1_RUNNING);

	/* Start the first task executing. */
	vPortRestoreTaskContext();
}
/*-----------------------------------------------------------*/

void smp_init(void)
{
	BaseType_t xCoreID;
	BaseType_t err;

#if ( configNUM_CORES > 1 )
	RTK_LOGS(TAG, "smp: Bringing up secondary CPUs ...\n");

	if (SYSCFG_CHIPType_Get() != CHIP_TYPE_RTLSIM) {//RTL sim shall not use delayus before core1 ready
		/* power on core1 to avoid km4 not open it */
		rtk_core1_power_on();
		DelayUs(50);
	}
#else
	/* power off core1 to avoid km4 has already open it */
	rtk_core1_power_off();
#endif

	for (xCoreID = 0; xCoreID < configNUM_CORES; xCoreID++) {
		if (xCoreID == portGET_CORE_ID()) {
			pmu_set_secondary_cpu_state(xCoreID, CPU1_RUNNING);
			continue;
		}

		err = psci_cpu_on(xCoreID, (unsigned long)_boot);
		if (err < 0) {
			RTK_LOGS(TAG, "CPU%d: failed to boot: %d\n", (int)xCoreID, (int)err);
		}
	}

}
