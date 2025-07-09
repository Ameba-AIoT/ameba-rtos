#include "ameba_soc.h"

#ifdef CONFIG_PLATFORM_FREERTOS_ROM
#include "os_cfg_ram.h"

extern u32 xTickCount;
extern u32 xPendedTicks;

ALIGNMTO(32) u8 ucStaticTimerQueueStorage_ram[(size_t) configTIMER_QUEUE_LENGTH_RAM * DAEMONTASKMESSAGE_T_SIZE];
ALIGNMTO(32) StaticTask_t xIdleTaskTCB_ram;
ALIGNMTO(32) StackType_t uxIdleTaskStack_ram[configIDLE_TASK_STACK_DEPTH_RAM];
ALIGNMTO(32) StaticTask_t xTimerTaskTCB_ram;
ALIGNMTO(32) StackType_t uxTimerTaskStack_ram[configTIMER_TASK_STACK_DEPTH_RAM];


/* Realtek add */
void vTaskCompTick(const TickType_t xTicksToComp)
{
	if (xTicksToComp > 1) {
		if (xTickCount + xTicksToComp > xTickCount) {
			xTickCount += (xTicksToComp - 1);
			/* not overflow, just trigger task list update */
			++xPendedTicks;
		} else {
			if (xTickCount <= 0xFFFFFFFE) {
				/* need to trigger task list update and then trigger task list switch before tick overflow*/
				xPendedTicks += (xTicksToComp - (0xFFFFFFFE - xTickCount));
				xTickCount = 0xFFFFFFFE;
			} else {
				xPendedTicks += xTicksToComp;
			}
		}
	} else {
		xPendedTicks += xTicksToComp;
	}
}

u32 vApplicationIdleHook_RAM(void) {
#if !defined(CONFIG_MP_SHRINK) && defined (CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
	extern int32_t driver_suspend_ret;
#if defined (CONFIG_WLAN)
	extern int32_t wlan_driver_check_and_suspend(void);
	driver_suspend_ret = wlan_driver_check_and_suspend();
#endif
#endif

#if defined(CONFIG_WIFI_FW_EN) && CONFIG_WIFI_FW_EN
#if defined(CONFIG_FW_DRIVER_COEXIST) && CONFIG_FW_DRIVER_COEXIST
	extern void wififw_task_idle(void);
	wififw_task_idle();
#endif
#endif
	return 0;
}

/* NVIC will power off under sleep power gating mode, so we can */
/* not use systick like FreeRTOS default implementation */
void vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
	(void) xExpectedIdleTime;
	uint32_t tick_before_sleep;
	eSleepModeStatus eSleepStatus;

	/* Enter a critical section but don't use the taskENTER_CRITICAL()
	method as that will mask interrupts that should exit sleep mode. */
	/* only np need mask/unmask hw irq here */
	SOCPS_HWSleepFilter(ENABLE);
	__disable_irq();

	system_can_yield = 0;

	eSleepStatus = eTaskConfirmSleepModeStatus();

	/* If a context switch is pending or a task is waiting for the scheduler
	to be unsuspended then abandon the low power entry. */
	if (eSleepStatus == eAbortSleep) {
	} else if (pmu_ready_to_sleep()) {
		/* Sleep until something happens.  configPRE_SLEEP_PROCESSING() can
		set its parameter to 0 to indicate that its implementation contains
		its own wait for interrupt or wait for event instruction, and so wfi
		should not be executed again.  However, the original expected idle
		time variable must remain unmodified, so a copy is taken. */
		pmu_pre_sleep_processing(&tick_before_sleep);
		pmu_post_sleep_processing(&tick_before_sleep);
	} else {

		SOCPS_HWSleepFilter(DISABLE);
		__DSB();
		__WFI();
		__NOP();	 //fix cpu fused instruction issue
		__ISB();
	}

	system_can_yield = 1;

	/* Re-enable interrupts - see comments above the cpsid instruction()
	above. */
	SOCPS_HWSleepFilter(DISABLE);
	__enable_irq();

	(void)tick_before_sleep;
}

void pmu_post_sleep_processing(TickType_t *tick_before_sleep)
{
	uint32_t tick_passed;
	volatile uint32_t ms_passed = 0;
	static uint32_t missing_tick = 0;
	/*  update kernel tick by calculating passed tick from gtimer */
	/*  get current gtimer timestamp */
	tick_passed = SYSTIMER_GetPassTick(*tick_before_sleep);

	tick_passed += missing_tick;
	missing_tick = tick_passed & 0x1F;

	/* ms =x*1000/32768 = (x *1000) >>15 */
	ms_passed = (uint32_t)((((u64)tick_passed) * 1000) >> 15);

	/* update xTickCount and mark to trigger task list update in xTaskResumeAll */
	vTaskCompTick(ms_passed);

	RTK_LOGD(NOTAG, "%s sleeped:[%d] ms\n", (SYS_CPUID() == 1) ? "NP" : "AP", ms_passed);
}

void vPortCleanUpTCB(uint32_t *pulTCB)
{
	UNUSED(pulTCB);

#if( configENABLE_TRUSTZONE == 1 )
	extern void vPortFreeSecureContext(uint32_t *pulTCB);
	vPortFreeSecureContext(pulTCB);
#endif
}

void newlib_assign_1(uint32_t * pxNewLib_reent)
{
	_impure_ptr = (struct _reent *)pxNewLib_reent;
}

void newlib_assign_2(uint32_t * pxNewLib_reent)
{
	_impure_ptr = (struct _reent *)pxNewLib_reent;
}

void newlib_reent_init_ptr(uint32_t * pxNewLib_reent)
{
	_REENT_INIT_PTR(((struct _reent *)pxNewLib_reent));
}

void newlib_reclaim_reent(uint32_t * pxNewLib_reent)
{
	struct _reent * reent = (struct _reent *)pxNewLib_reent;
	_reclaim_reent(reent);
	#if defined(__NEWLIB__) && defined (_REENT_SMALL) && !defined(_REENT_GLOBAL_STDIO_STREAMS)
		if (reent->_stdout) {
			_fclose_r(reent, reent->_stdout);
		}
		if (reent->_stderr) {
			_fclose_r(reent, reent->_stderr);
		}
		if (reent->_stdin)  {
			_fclose_r(reent, reent->_stdin);
		}
	#endif

}

void os_rom_init(void)
{
	/* Toolchain should not changed because size of struct _reent in TCB in ROM OS is fixed, or user shold use RAM OS */
	assert_param(sizeof(struct _reent) == ROM_OS_NEWLIB_REENT_SIZE);

	os_cfg.system_clock = SystemCoreClock;
	os_cfg.vPortSleepHook = vPortSuppressTicksAndSleep;
	os_cfg.timer_queue_length = configTIMER_QUEUE_LENGTH_RAM;
	os_cfg.timer_task_stack_size = configTIMER_TASK_STACK_DEPTH_RAM;
	os_cfg.idle_task_stack_size = configIDLE_TASK_STACK_DEPTH_RAM;
	os_cfg.tick_rate_hz = configTICK_RATE_HZ_RAM;
	os_cfg.port_initial_exc_return = portINITIAL_EXC_RETURN_RAM;
	os_cfg.patch_pvPortMalloc=pvPortMalloc;
	os_cfg.patch_vPortFree=vPortFree;

	ucStaticTimerQueueStorage = ucStaticTimerQueueStorage_ram;
	xIdleTaskTCB = &xIdleTaskTCB_ram;
	uxIdleTaskStack = uxIdleTaskStack_ram;
	xTimerTaskTCB = &xTimerTaskTCB_ram;
	uxTimerTaskStack = uxTimerTaskStack_ram;

#if defined(CONFIG_ARM_CORE_CM4_KM4TZ) && (CONFIG_ARM_CORE_CM4_KM4TZ == 1)
#if( configENABLE_TRUSTZONE == 0 )
	msp_addr = MSP_RAM_HP;
#else
	msp_addr = MSP_RAM_HP_NS;
#endif
	/* all in km4tz when wifi host control */
#ifdef CONFIG_WIFI_HOST_CONTROL
	os_cfg.patch_vApplicationIdleHook = vApplicationIdleHook_RAM;
#endif

#else
	msp_addr = MSP_RAM_LP;
	os_cfg.patch_vApplicationIdleHook = vApplicationIdleHook_RAM;
#endif

	SVCall_irqfunc_set(vPortSVCHandler);
	PendSV_irqfunc_set(xPortPendSVHandler);
	Systick_irqfunc_set(xPortSysTickHandler);

	patch_vPortCleanUpTCB = vPortCleanUpTCB;
	patch_traceTaskCreate = traceTaskCreate_rom;
	patch_traceTaskDelete = traceTaskDelete_rom;
	patch_newlib_assign_1 = newlib_assign_1;
	patch_newlib_assign_2 = newlib_assign_2;
	patch_newlib_reent_init_ptr = newlib_reent_init_ptr;
	patch_newlib_reclaim_reent = newlib_reclaim_reent;
	patch_portDisable_Interrupts = ulPortSetInterruptMask;
	patch_portEnable_Interrupts = vPortClearInterruptMask;

#if( configENABLE_TRUSTZONE == 0 )
	patch_SecureContext_AllocateContext = SecureContext_AllocateContext_rom;
	patch_SecureContext_FreeContext = SecureContext_FreeContext_rom;
	patch_SecureInit_DePrioritizeNSExceptions = SecureInit_DePrioritizeNSExceptions_rom;
	patch_SecureContext_LoadContext = SecureContext_LoadContext_rom;
	patch_SecureContext_SaveContext = SecureContext_SaveContext_rom;
	patch_SecureContext_Init = SecureContext_Init_rom;
	patch_vPortAllocateSecureContext = vPortAllocateSecureContext_rom;
#else
	extern SecureContextHandle_t SecureContext_AllocateContext( uint32_t ulSecureStackSize );
	extern void SecureContext_FreeContext( SecureContextHandle_t xSecureContextHandle );
	extern void SecureInit_DePrioritizeNSExceptions( void );
	extern void SecureContext_LoadContext( SecureContextHandle_t xSecureContextHandle );
	extern void SecureContext_SaveContext( SecureContextHandle_t xSecureContextHandle );
	extern void SecureContext_Init( void );
	extern void vPortAllocateSecureContext(uint32_t ulSecureStackSize);
	patch_SecureContext_AllocateContext = SecureContext_AllocateContext;
	patch_SecureContext_FreeContext = SecureContext_FreeContext;
	patch_SecureInit_DePrioritizeNSExceptions = SecureInit_DePrioritizeNSExceptions;
	patch_SecureContext_LoadContext = SecureContext_LoadContext;
	patch_SecureContext_SaveContext = SecureContext_SaveContext;
	patch_SecureContext_Init = SecureContext_Init;
	patch_vPortAllocateSecureContext = vPortAllocateSecureContext;
#endif
}

#endif
