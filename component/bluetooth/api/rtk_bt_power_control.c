/*
 *******************************************************************************
 * Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
 *******************************************************************************
 */

#include "platform_autoconf.h"
#include <bt_api_config.h>
#include "bt_debug.h"
#if defined(RTK_BT_POWER_CONTROL_SUPPORT) && RTK_BT_POWER_CONTROL_SUPPORT
#include <stdio.h>
#include <string.h>
#include <osif.h>
#include <basic_types.h>
#include "rtk_bt_power_control.h"
#include "ameba_soc.h"

rtk_bt_ps_callback rtk_bt_suspend_callback = NULL;
rtk_bt_ps_callback rtk_bt_resume_callback = NULL;

_WEAK void hci_platform_force_uart_rts(bool op)
{
	(void) op;
}

static bool rtk_bt_get_wakelock_status(void)
{
	uint32_t lock_status = pmu_get_wakelock_status();
	BT_LOGA("[BT_PS] lock_status = 0x%x\r\n", lock_status);

	if (lock_status & ((0x01) << PMU_BT_DEVICE)) {
		return true;    //Already acquire bt wake lock
	} else {
		return false;   //Already release bt wake lock
	}
}

void rtk_bt_release_wakelock(void)
{
	if (rtk_bt_get_wakelock_status() == true) {
		BT_LOGA("[BT_PS] pmu_release_wakelock PMU_BT_DEVICE\r\n");
		pmu_release_wakelock(PMU_BT_DEVICE);
	} else {
		BT_LOGA("[BT_PS] already release PMU_BT_DEVICE\r\n");
	}
}

void rtk_bt_acquire_wakelock(void)
{
	if (rtk_bt_get_wakelock_status() == false) {
		BT_LOGA("[BT_PS] pmu_acquire_wakelock PMU_BT_DEVICE\r\n");
		pmu_acquire_wakelock(PMU_BT_DEVICE);
	} else {
		BT_LOGA("[BT_PS] already acquire PMU_BT_DEVICE\r\n");
	}
}

static void rtk_bt_enable_bt_wake_host(void)
{
	if (irq_get_pending(BT_WAKE_HOST_IRQ)) {
		irq_clear_pending(BT_WAKE_HOST_IRQ);
	}

	/* enable BT_WAKE_HOST interrupt */
	InterruptEn(BT_WAKE_HOST_IRQ, INT_PRI_LOWEST);
}

static void rtk_bt_disable_bt_wake_host(void)
{
	/* disable BT_WAKE_HOST interrupt */
	InterruptDis(BT_WAKE_HOST_IRQ);

	if (irq_get_pending(BT_WAKE_HOST_IRQ)) {
		irq_clear_pending(BT_WAKE_HOST_IRQ);
	}
}

static uint32_t rtk_bt_suspend(uint32_t expected_idle_time, void *param)
{
	(void)expected_idle_time;
	(void)param;

	BT_LOGA("[BT_PS] Enter rtk_bt_suspend\r\n");

	hci_platform_force_uart_rts(true);

	rtk_bt_enable_bt_wake_host();

	if (rtk_bt_suspend_callback) {
		rtk_bt_suspend_callback();
	}

	return 1;
}

static uint32_t rtk_bt_resume(uint32_t expected_idle_time, void *param)
{
	(void)expected_idle_time;
	(void)param;

	BT_LOGA("[BT_PS] Enter rtk_bt_resume\r\n");

	rtk_bt_acquire_wakelock();

	rtk_bt_disable_bt_wake_host();

	if (rtk_bt_resume_callback) {
		rtk_bt_resume_callback();
	}

	hci_platform_force_uart_rts(false);

	return 1;
}

static uint32_t rtk_bt_wake_host_irq_handler(void *data)
{
	(void)data;

	return 1;
}

void rtk_bt_power_save_init(rtk_bt_ps_callback p_suspend_callback, rtk_bt_ps_callback p_resume_callback)
{
	rtk_bt_suspend_callback = p_suspend_callback;
	rtk_bt_resume_callback = p_resume_callback;

	/* register callback before entering ps mode and after exiting ps mode */
	pmu_register_sleep_callback(PMU_BT_DEVICE, (PSM_HOOK_FUN)rtk_bt_suspend, NULL, (PSM_HOOK_FUN)rtk_bt_resume, NULL);

	InterruptRegister((IRQ_FUN)rtk_bt_wake_host_irq_handler, BT_WAKE_HOST_IRQ, NULL, INT_PRI_LOWEST);
}

void rtk_bt_power_save_deinit(void)
{
	InterruptDis(BT_WAKE_HOST_IRQ);
	InterruptUnRegister(BT_WAKE_HOST_IRQ);

	pmu_unregister_sleep_callback(PMU_BT_DEVICE);

	rtk_bt_suspend_callback = NULL;
	rtk_bt_resume_callback = NULL;
}
#endif
