#include "ameba_soc.h"
#include "ameba_v8m_crashdump.h"
#include "ameba_fault_handle.h"

static const char *TAG = "FAULT";

#if defined(CONFIG_ARM_CORE_CM4)
/* Redefine secure fault handler and replace old INT_SecureFault in rom code. */
void HANDLER_SecureFault(void)
{
	__ASM volatile(
		"MRS 		R0, MSP					\n\t"
		"MRS 		R1, PSP					\n\t"
		"MOV 		R2, LR					\n\t"
		"MOV 		R3, #4					\n\t"
		"PUSH 		{R4-R11}				\n\t"
		"B			Fault_Handler		    \n\t"
	);
}

void HANDLER_MemFault(void)
{
	__ASM volatile(
		"MRS 		R0, MSP					\n\t"
		"MRS 		R1, PSP					\n\t"
		"MOV 		R2, LR					\n\t"
		"MOV 		R3, #3					\n\t"
		"PUSH 		{R4-R11}				\n\t"
		"B			Fault_Handler		    \n\t"
	);
}

void HANDLER_BusFault(void)
{
	__ASM volatile(
		"MRS 		R0, MSP					\n\t"
		"MRS 		R1, PSP					\n\t"
		"MOV 		R2, LR					\n\t"
		"MOV 		R3, #2					\n\t"
		"PUSH 		{R4-R11}				\n\t"
		"B			Fault_Handler		    \n\t"
	);
}

void HANDLER_UsageFault(void)
{
	__ASM volatile(
		"MRS 		R0, MSP					\n\t"
		"MRS 		R1, PSP					\n\t"
		"MOV 		R2, LR					\n\t"
		"MOV 		R3, #1					\n\t"
		"PUSH 		{R4-R11}				\n\t"
		"B			Fault_Handler		    \n\t"
	);
}

void HANDLER_HardFault(void)
{
	__ASM volatile(
		"MRS 		R0, MSP					\n\t"
		"MRS 		R1, PSP					\n\t"
		"MOV 		R2, LR					\n\t"
		"MOV 		R3, #0					\n\t"
		"PUSH 		{R4-R11}				\n\t"
		"B			Fault_Handler		    \n\t"
	);
}
void Fault_Handler(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id)
{
	uint32_t *cstack;

	uint32_t *extra_regs = mstack;

	uint32_t control_ns = 0;

	uint32_t regs[17] = {0x0};

	u8 is_psp = 0;

	enum {
		REG_EXCR = 0, REG_R4, REG_R5, REG_R6, REG_R7, REG_R8, REG_R9, REG_R10, REG_R11, \
		REG_R0, REG_R1, REG_R2, REG_R3, REG_R12, REG_LR, REG_EPC, REG_xPSR, REG_END
	};
	/* If Bit[0] is 1, here is Secure world. */
	if (lr_value & EXC_RETURN_ES) {
		/* 1.1 Exception cames from secure world. */
		if (lr_value & EXC_RETURN_S) {
			/* Whether PSP_S has been used*/
			if ((lr_value & EXC_RETURN_MODE) && (lr_value & EXC_RETURN_SPSEL)) {
				is_psp = 1;
			}
			/* 1.2 Exception cames from non-secure world.*/
		} else {
			/* what stack was used according to control_ns[1] in non-secure world!*/
			control_ns = __TZ_get_CONTROL_NS();
			if ((lr_value & EXC_RETURN_MODE) && (control_ns & CONTROL_SPSEL_Msk)) {
				is_psp = 1;
			}
		}

	} else {
		/* 2.1 Non-Secure world cannot dump registers about exception from secure world. */
		if (lr_value & EXC_RETURN_S) {
			RTK_LOGA(TAG, "\nNon-Secure world cannot dump registers about exception from secure world!\n");
			while (1);
			/* 2.2 Non-secure exception from non-secure world!*/
		} else {
			if ((lr_value & EXC_RETURN_MODE) && (lr_value & EXC_RETURN_SPSEL)) {
				is_psp = 1;
			}
		}
	}

	regs[REG_EXCR] = lr_value;
	/* MSP stack
	High addr -> |  xxx  | <--- &extra_regs[0] is mstack;
	  ^          |  R11  | <--- extra_regs[-1]
	  |	         |  R10  |
	  |	         |  ...  |
	Low addr  -> |  R4   | <--- extra_regs[-8]*/

	extra_regs--; //point to R11
	for (int i = REG_R4; i <= REG_R11; i++) {
		regs[i] = extra_regs[-REG_R11 + i];
	}

	cstack = is_psp ? pstack : mstack;

	for (int i = REG_R0; i < REG_END; i++) {
		regs[i] = cstack[i - REG_R0];
	}

	crash_dump((uint32_t *)cstack[REG_EPC], cstack, regs);

	if (fault_id == SECUREFAULT_ID) {
		RTK_LOGA(TAG, "SFSR  = 0x%08lx\r\n", SAU->SFSR);
		RTK_LOGA(TAG, "SFAR  = 0x%08lx\r\n", SAU->SFAR);
	}
	if (control_ns) {
		RTK_LOGA(TAG, "CONTROL_NS = 0x%08lX\r\n", control_ns);
	}
	RTK_LOGA(TAG, "SHCSR = 0x%08lx\r\n", SCB->SHCSR);
	RTK_LOGA(TAG, "AIRCR = 0x%08lx\r\n", SCB->AIRCR);
	RTK_LOGA(TAG, "CONTROL = 0x%08lx\r\n", __get_CONTROL());

	extern void INT_HardFault_C(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id);
	INT_HardFault_C(mstack, pstack, lr_value, fault_id);
}
#else
void HANDLER_HardFault(void)
{
	__ASM volatile(
		"MRS 		R0, MSP					\n\t"
		"MRS 		R1, PSP					\n\t"
		"MOV 		R2, LR					\n\t" /* Third parameter is LR current value */
		"MOV 		R3, #0					\n\t"
		"PUSH 		{R4-R7}					\n\t"
		"B			Fault_Handler			\n\t"
	);
}

void Fault_Handler(uint32_t mstack[], uint32_t pstack[], uint32_t lr_value, uint32_t fault_id)
{
	RTK_LOGA(TAG, "Hard Fault Patch (KM0)\r\n");
	if (fault_id == 0) {
		RTK_LOGA(TAG, "Hard Fault: \r\n");
	} else {
		RTK_LOGA(TAG, "Unsupported faults\r\n");
		while (1);
	}
	uint32_t *cstack;

	uint32_t *extra_regs = mstack;

	uint32_t regs[17] = {0x0};

	u8 is_psp = 0;

	enum {
		REG_EXCR = 0, REG_R4, REG_R5, REG_R6, REG_R7, REG_R8, REG_R9, REG_R10, REG_R11, \
		REG_R0, REG_R1, REG_R2, REG_R3, REG_R12, REG_LR, REG_EPC, REG_xPSR, REG_END
	};

	if (lr_value & BIT(3)) {		//Thread Mode
		if (lr_value & BIT(2)) { 	//PSP
			is_psp = 1;
		}
	}
	regs[REG_EXCR] = lr_value;
	/* MSP stack
	High addr -> |  xxx  | <--- &extra_regs[0] is mstack;
	  ^          |  R7   | <--- extra_regs[-1]
	  |	         |  R6   |
	  |	         |  ...  |
	Low addr  -> |  R4   | <--- extra_regs[-4]*/

	//point to R7, R8 ~ R11 were not saved.
	extra_regs--;
	for (int i = REG_R4; i <= REG_R7; i++) {
		regs[i] = extra_regs[-REG_R7 + i];
	}

	cstack = is_psp ? pstack : mstack;

	for (int i = REG_R0; i < REG_END; i++) {
		regs[i] = cstack[i - REG_R0];
	}

	crash_dump((uint32_t *)cstack[REG_EPC], cstack, regs);

	RTK_LOGA(TAG, "MSP     = %p\r\n", mstack);
	RTK_LOGA(TAG, "PSP     = %p\r\n", pstack);
	RTK_LOGA(TAG, "CONTROL = 0x%08lX\r\n", __get_CONTROL());
	RTK_LOGA(TAG, "SVC priority: 	 %lu\r\n", __NVIC_GetPriority(SVCall_IRQn));
	RTK_LOGA(TAG, "PendSVC priority:  %lu\r\n", __NVIC_GetPriority(PendSV_IRQn));
	RTK_LOGA(TAG, "Systick priority:  %lu\r\n", __NVIC_GetPriority(SysTick_IRQn));
	RTK_LOGA(TAG, "\r\n");

	extern void INT_HardFault_C(u32 * stack);
	INT_HardFault_C(cstack);
}

#endif

void Fault_Hanlder_Redirect(crash_on_task crash_on_task_func)
{
#ifdef CONFIG_ARM_CORE_CM4
#ifdef IMAGE2_BUILD
	NewVectorTable[3] = (HAL_VECTOR_FUN)HANDLER_HardFault;
	NewVectorTable[4] = (HAL_VECTOR_FUN)HANDLER_MemFault;
	NewVectorTable[5] = (HAL_VECTOR_FUN)HANDLER_BusFault;
	NewVectorTable[6] = (HAL_VECTOR_FUN)HANDLER_UsageFault;
	crash_task_info = crash_on_task_func;
#else
	(void)crash_on_task_func;
	NewVectorTable[7] = (HAL_VECTOR_FUN)HANDLER_SecureFault;
#endif
#else
	NewVectorTable[3] = (HAL_VECTOR_FUN)HANDLER_HardFault;
	crash_task_info = crash_on_task_func;
#endif
}