/*
 * FreeRTOS Kernel V10.2.1
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the RISC-V RV32 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "portmacro.h"
#include "stdio.h"
#include "string.h"
#include "task.h"

#if (portUSING_MPU_WRAPPERS == 1)
/** Variable that contains the current privilege state */
volatile uint32_t privilege_status = ePortMACHINE_MODE;
#endif

#if( configENABLE_FPU == 1 )
/*
 * Setup the Floating Point Unit (FPU).
 */
void prvSetupFPU(void) PRIVILEGED_FUNCTION;
#endif /* configENABLE_FPU */

/*-----------------------------------------------------------*/

#if (portUSING_MPU_WRAPPERS == 1) || (portRISCV_STACK_PROTECT == 1)
PRIVILEGED_DATA pmp_info_t xPmpInfo = {0, 0};
#endif

#if (portUSING_MPU_WRAPPERS == 1)
void vResetPrivilege(void) __attribute__((naked));

BaseType_t xIsPrivileged(void)
{
	return (privilege_status == ePortMACHINE_MODE);
}

__attribute__((naked)) void vPortSyscall(unsigned int Value)
{
	/* Remove compiler warning about unused parameter. */
	(void)Value;

	__asm__ __volatile__("	ecall 		\n"
						 "	ret 		\n" ::
						 :);
}
/*-----------------------------------------------------------*/

__attribute__((naked)) void vRaisePrivilege(void)
{
	__asm__ __volatile__("	li	a0,%0 	\n"
						 "	ecall 		\n"
						 "	ret 		\n" ::"i"(portSVC_SWITCH_TO_MACHINE)
						 :);
}
/*-----------------------------------------------------------*/

__attribute__((naked)) void vResetPrivilege(void)
{
	__asm__ __volatile__("	li	a0,%0 	\n"
						 "	ecall 		\n"
						 "	ret 		\n" ::"i"(portSVC_SWITCH_TO_USER)
						 :);
}
/*-----------------------------------------------------------*/

/**
 * @brief Store PMP settings in Task TCB - the name this function
 * 		  is vPortStoreTaskMPUSettings in order to be MPU compliant
 *
 * @param[out]  xPMPSettings    PMP settings stored in Task TCB
 * @param[in]   xRegions        PMP configuration of the task
 * @param[in]   pxBottomOfStack address of bottom of stack
 * @param[in]   ulStackDepth    size of stack
 */
void vPortStoreTaskMPUSettings(xMPU_SETTINGS *xPMPSettings,
							   const struct xMEMORY_REGION *const xRegions,
							   StackType_t *pxBottomOfStack,
							   uint32_t ulStackDepth) PRIVILEGED_FUNCTION {
	extern uint32_t __SRAM_segment_start__[];
	extern uint32_t __SRAM_segment_end__[];
	int32_t lIndex;
	uint32_t ul;

	/**
	 *  considered as unused in certain cases because of macro
	 * configASSERT_DEFINED
	 */
	int32_t lResult __attribute__((unused)) = PMP_DEFAULT_ERROR;
	size_t uxBaseAddressChecked = 0;

	if (0 == xPmpInfo.granularity)
	{
		lResult = init_pmp(&xPmpInfo);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif
	}

	memset(xPMPSettings, 0, sizeof(xMPU_SETTINGS));

	/* configure PMP for Task Stack buffer */
	if (ulStackDepth > 0)
	{
		/* Config stack start address */
		uxBaseAddressChecked = 0;
		lResult = addr_modifier(xPmpInfo.granularity, (size_t)pxBottomOfStack,
								&uxBaseAddressChecked);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		xPMPSettings->uxRegionBaseAddress[0] = uxBaseAddressChecked;

		xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
				portSTACK_REGION_START)] +=
					((UBaseType_t)((portPMP_REGION_READ_WRITE) |
								   (portPMP_REGION_ADDR_MATCH_NA4))
					 << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

		xPMPSettings
		->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_START)] +=
			((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_START));

		/* Config stack end address and TOR */
		uxBaseAddressChecked = 0;
		lResult = addr_modifier(xPmpInfo.granularity,
								(size_t)pxBottomOfStack +
								(size_t)ulStackDepth * sizeof(StackType_t),
								&uxBaseAddressChecked);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		xPMPSettings->uxRegionBaseAddress[1] = uxBaseAddressChecked;

		xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
				portSTACK_REGION_END)] +=
					((UBaseType_t)((portPMP_REGION_READ_WRITE) |
								   (portPMP_REGION_ADDR_MATCH_TOR))
					 << portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));

		xPMPSettings
		->uxPmpConfigRegMask[portGET_PMPCFG_IDX(portSTACK_REGION_END)] +=
			((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(portSTACK_REGION_END));
	}

	if (xRegions == NULL)
	{
		/* No PMP regions are specified so allow access to all data section */
		/* Config a Region for the whole RAM access permission */
		uxBaseAddressChecked = 0;
		lResult =
			addr_modifier(xPmpInfo.granularity, (size_t)__SRAM_segment_start__,
						  &uxBaseAddressChecked);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		xPMPSettings->uxRegionBaseAddress[2] = uxBaseAddressChecked;

		xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
				portFIRST_CONFIGURABLE_REGION)] +=
					((UBaseType_t)((portPMP_REGION_READ_WRITE) |
								   (portPMP_REGION_ADDR_MATCH_NA4))
					 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION));

		xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(
											 portFIRST_CONFIGURABLE_REGION)] +=
													 ((UBaseType_t)0xFF
															 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION));

		/* Config RAM end address and TOR */
		uxBaseAddressChecked = 0;
		lResult =
			addr_modifier(xPmpInfo.granularity, (size_t)__SRAM_segment_end__,
						  &uxBaseAddressChecked);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		xPMPSettings->uxRegionBaseAddress[3] = uxBaseAddressChecked;

		xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
				portFIRST_CONFIGURABLE_REGION + 1)] +=
					((UBaseType_t)((portPMP_REGION_READ_WRITE) |
								   (portPMP_REGION_ADDR_MATCH_TOR))
					 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION + 1));

		xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(
											 portFIRST_CONFIGURABLE_REGION + 1)] +=
													 ((UBaseType_t)0xFF
															 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION + 1));

		/* Invalidate all other configurable regions. */
		for (ul = 2; ul < portNUM_CONFIGURABLE_REGIONS_REAL(xPmpInfo.nb_pmp);
			 ul++) {
			xPMPSettings->uxRegionBaseAddress[2 + ul] = 0UL;
			xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(
												 portFIRST_CONFIGURABLE_REGION + ul)] +=
														 ((UBaseType_t)0xFF
																 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION + ul));
		}
	} else
	{
		/* This function is called automatically when the task is created - in
		which case the stack region parameters will be valid.  At all other
		times the stack parameters will not be valid and it is assumed that the
		stack region has already been configured. */
		lIndex = 0;

		for (ul = 0; ul < (portNUM_CONFIGURABLE_REGIONS_REAL(xPmpInfo.nb_pmp));
			 ul++) {
			if (((xRegions[lIndex]).ulLengthInBytes > 0UL) ||
				((xRegions[lIndex].ulParameters & 0xFF) != 0)) {
				uint32_t AddrMatch;

				AddrMatch = xRegions[lIndex].ulParameters &
							portPMP_REGION_ADDR_MATCH_MASK;
				if ((AddrMatch == portPMP_REGION_ADDR_MATCH_TOR) ||
					(AddrMatch == portPMP_REGION_ADDR_MATCH_NA4)) {
					lResult =
						addr_modifier(xPmpInfo.granularity,
									  (size_t)xRegions[lIndex].pvBaseAddress,
									  &uxBaseAddressChecked);

				} else if (AddrMatch == portPMP_REGION_ADDR_MATCH_NAPOT) {
#if (configASSERT_DEFINED == 1)
					{
						configASSERT(0 < xRegions[lIndex].ulLengthInBytes);
					}
#endif
					lResult = napot_addr_modifier(
								  xPmpInfo.granularity,
								  (size_t)xRegions[lIndex].pvBaseAddress,
								  &uxBaseAddressChecked,
								  xRegions[lIndex].ulLengthInBytes);
				} else {
					uxBaseAddressChecked = 0;
				}

#if (configASSERT_DEFINED == 1)
				{
					configASSERT(0 <= lResult);
				}
#endif
				xPMPSettings->uxRegionBaseAddress[2 + ul] =
					uxBaseAddressChecked;

				if ((xRegions[lIndex].ulParameters & tskMPU_REGION_READ_ONLY) != 0) {
					xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
							portFIRST_CONFIGURABLE_REGION + ul)] |=
								(((UBaseType_t)(xRegions[lIndex].ulParameters) | portPMP_REGION_READ_ONLY)
								 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION +
														 ul));
				}

				if ((xRegions[lIndex].ulParameters & tskMPU_REGION_READ_WRITE) != 0) {
					xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
							portFIRST_CONFIGURABLE_REGION + ul)] |=
								(((UBaseType_t)(xRegions[lIndex].ulParameters) | portPMP_REGION_READ_WRITE)
								 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION +
														 ul));
				}

				if ((xRegions[lIndex].ulParameters & tskMPU_REGION_EXECUTE_NEVER) != 0) {
					xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
							portFIRST_CONFIGURABLE_REGION + ul)] |=
								(((UBaseType_t)(xRegions[lIndex].ulParameters) & (!portPMP_REGION_EXECUTE))
								 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION +
														 ul));
				}

				if ((xRegions[lIndex].ulParameters & tskMPU_REGION_EXECUTE_NEVER) == 0) {
					xPMPSettings->uxPmpConfigRegAttribute[portGET_PMPCFG_IDX(
							portFIRST_CONFIGURABLE_REGION + ul)] |=
								(((UBaseType_t)(xRegions[lIndex].ulParameters) | portPMP_REGION_EXECUTE)
								 << portPMPCFG_BIT_SHIFT(portFIRST_CONFIGURABLE_REGION +
														 ul));
				}

				xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(
													 portFIRST_CONFIGURABLE_REGION + ul)] +=
															 ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(
																	 portFIRST_CONFIGURABLE_REGION + ul));
			} else {
				/* Invalidate the region. */
				xPMPSettings->uxRegionBaseAddress[2 + ul] = 0UL;
				xPMPSettings->uxPmpConfigRegMask[portGET_PMPCFG_IDX(
													 portFIRST_CONFIGURABLE_REGION + ul)] +=
															 ((UBaseType_t)0xFF << portPMPCFG_BIT_SHIFT(
																	 portFIRST_CONFIGURABLE_REGION + ul));
			}

			lIndex++;
		}

#if (configASSERT_DEFINED == 1)
		{
			// check we do not want to configure unavailable regions
			if (xPmpInfo.nb_pmp < MAX_PMP_REGION) {
				configASSERT(xRegions[lIndex].ulLengthInBytes == 0UL);
			}
		}
#endif
	}
}
/*-----------------------------------------------------------*/

/**
 * @brief Setup of base region (first 3 regions)
 * @details those regions won't be reconfigured during context switch
 *
 */
static void prvSetupPMP(void) PRIVILEGED_FUNCTION {
	extern uint32_t __unprivileged_section_start__[];
	extern uint32_t __unprivileged_section_end__[];

	uint8_t ucDefaultAttribute;
	size_t uxDefaultBaseAddr;
	/**
	 *  considered as unused in certain cases because of macro
	 * configASSERT_DEFINED
	 */
	int32_t lResult __attribute__((unused)) = PMP_DEFAULT_ERROR;

	if (0 == xPmpInfo.granularity)
	{
		lResult = init_pmp(&xPmpInfo);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif
	}

	/* Check the expected PMP is present. */
	if (portMINIMAL_NB_PMP <= xPmpInfo.nb_pmp)
	{
		/* First setup the start address of the unprivilleged flash */
		ucDefaultAttribute = 0;
		uxDefaultBaseAddr = 0;

		lResult = addr_modifier(xPmpInfo.granularity,
								(size_t)__unprivileged_section_start__,
								&uxDefaultBaseAddr);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		ucDefaultAttribute =
			((portPMP_REGION_READ_ONLY) | (portPMP_REGION_EXECUTE) |
			 (portPMP_REGION_ADDR_MATCH_NA4));

		lResult =
			write_pmp_config(&xPmpInfo, portUNPRIVILEGED_EXECUTE_REGION_START,
							 ucDefaultAttribute, uxDefaultBaseAddr);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		/* Setup the end address of the unprivilleged flash */
		ucDefaultAttribute = 0;
		uxDefaultBaseAddr = 0;
		lResult = addr_modifier(xPmpInfo.granularity,
								(size_t)__unprivileged_section_end__,
								&uxDefaultBaseAddr);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		ucDefaultAttribute =
			((portPMP_REGION_READ_ONLY) | (portPMP_REGION_EXECUTE) |
			 (portPMP_REGION_ADDR_MATCH_TOR));

		lResult =
			write_pmp_config(&xPmpInfo, portUNPRIVILEGED_EXECUTE_REGION_END,
							 ucDefaultAttribute, uxDefaultBaseAddr);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		/* Allow read only on the privilege status varibale */
		ucDefaultAttribute = 0;
		uxDefaultBaseAddr = 0;

		lResult = addr_modifier(xPmpInfo.granularity, (size_t)&privilege_status,
								&uxDefaultBaseAddr);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif

		ucDefaultAttribute =
			((portPMP_REGION_READ_ONLY) | (portPMP_REGION_ADDR_MATCH_NA4));

		lResult = write_pmp_config(&xPmpInfo, portPRIVILEGE_STATUS_REGION,
								   ucDefaultAttribute, uxDefaultBaseAddr);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif
	}
}

/*-----------------------------------------------------------*/

/**
 * @brief
 *
 * @param xIsrTop
 */
BaseType_t xPortRISCVPmpInit(void) PRIVILEGED_FUNCTION {
	extern BaseType_t xPortMoveISRStackTop(StackType_t *xISRStackTop);

	/**
	 * We do the initialization of FreeRTOS priviledged data section
	 */
#if (portUSING_MPU_WRAPPERS == 1)
	{
		/**
		 * Initial all privileged data as 0
		 * Considered by the compiler as unused because of the inline asm block
		 */
		extern uint32_t __privileged_data_start__[] __attribute__((unused));
		extern uint32_t __privileged_data_end__[] __attribute__((unused));
		__asm__ __volatile__(
			/* Zero the privileged_data segment. */
			"la t1, __privileged_data_start__ \n"
			"la t2, __privileged_data_end__ \n"

			"bge t1, t2, 2f \n"

			"1: \n"
#if __riscv_xlen == 32
			"sw   x0, 0(t1) \n"
			"addi t1, t1, 4 \n"
			"blt  t1, t2, 1b \n"
#else
			"sd   x0, 0(t1) \n"
			"addi t1, t1, 8 \n"
			"blt  t1, t2, 1b \n"
#endif
			"2: \n" ::
			: "t1", "t2"

		);
	}
#endif

	/*
	 * xIsrStack Is a Buffer Allocated into Application
	 * xIsrStack_top is the Top adress of this Buffer
	 * it will contain  the isrstack and space or the registeries backup
	 *
	 *                 Top +----------------------+ xIsrTop
	 *                     | ....                 |
	 *                     |                      |
	 *              Bottom +----------------------+ xISRStack
	 *
	 * stack mapping after :
	 *                 Top +----------------------+ xIsrTop
	 *                     | Space to store       |
	 *                     | context before       |
	 *                     | FreeRtos scheduling  |
	 *                     | ....                 |
	 *                     +----------------------+ xISRStackTop
	 *                     | stack space for      |
	 *                     | ISR execution        |
	 *                     | ....                 |
	 *                     |                      |
	 *              Bottom +----------------------+ xISRStack
	 */

#if (configCLINT_BASE_ADDRESS != 0)
	/* There is a clint then interrupts can branch directly to the FreeRTOS
	 * trap handler.
	 */
	__asm__ __volatile__("la t0, freertos_risc_v_trap_handler\n"
						 "csrw mtvec, t0\n");
#else
#warning                                                                       \
"*** The interrupt controller must to be configured before (ouside of this file). ***"
#endif

#if (portUSING_MPU_WRAPPERS == 1)
	/* Configure the regions in the PMP that are common to all tasks. */
	prvSetupPMP();
#endif

	return 0;
}

/**
 * @brief Do the PMP config switch when switching task
 * @param ulNbPmp       number of configurable PMP
 * @param xPMPSettings  PMP configs stored in task TCB
 * @warning the number of configurable PMP is not the total number of PMP
 *
 */
__attribute__((naked)) void
vPortPmpSwitch(uint32_t ulNbPmp,
			   xMPU_SETTINGS *xPMPSettings) PRIVILEGED_FUNCTION {
	/* Compute jump offset to avoid configure unuse PMP */
	__asm__ __volatile__("li t0, 13 \n" /* maximum number (13 = 16 - 3) of
                                           reconfigurable PMP for a core */
						 "sub a2, t0, %0 \n"
						 "slli a2, a2, 3 \n" /* compute the jump offset */
						 ::"r"(ulNbPmp)
						 : "a2", "t0");

	/* clear pmp config before setting addr */
#if __riscv_xlen == 32
	__asm__ __volatile__("li t1, 0x18181818 \n"
						 /**
						  * first 3 PMPs are fix configured
						  * we avoid disabling permanent PMP config, therefore
						  * those region mask are set to 0
						  */
						 "li t2, 0x18000000 \n"
						 "csrc pmpcfg0, t2 \n"
						 "csrc pmpcfg1, t1 \n"
						 "csrc pmpcfg2, t1 \n"
						 "csrc pmpcfg3, t1 \n" ::
						 : "t1", "t2");
#elif __riscv_xlen == 64
	__asm__ __volatile__("li t1, 0x1818181818181818 \n"
						 /**
						  * we avoid disabling permanent PMP config, therefore
						  * those region mask are set to 0
						  */
						 "li t2, 0x1818181818000000 \n"
						 "csrc pmpcfg0, t2 \n"
						 "csrc pmpcfg2, t1 \n" ::
						 : "t1", "t2");
#endif

	/**
	 * Save pmp address in pmpaddrx registers
	 * Please note that pmpaddr0, pmpaddr1, pmpaddr2 are not reconfigured,
	 * they are configured once at the initialization of the scheduler
	 */
#if __riscv_xlen == 32
	__asm__ __volatile__(
		"add t0, %0, 32 \n" /* t0 = xPMPSettings->uxRegionBaseAddress */
		"la t1, 1f \n"      /* compute the jump address */
		"add t2, t1, a2 \n"
		"jr t2 \n"
		"1: \n"
		"lw t1, 48(t0) \n"
		"csrw pmpaddr15, t1 \n"
		"lw t2, 44(t0) \n"
		"csrw pmpaddr14, t2 \n"
		"lw t1, 40(t0) \n"
		"csrw pmpaddr13, t1 \n"
		"lw t2, 36(t0) \n"
		"csrw pmpaddr12, t2 \n"
		"lw t1, 32(t0) \n"
		"csrw pmpaddr11, t1 \n"
		"lw t2, 28(t0) \n"
		"csrw pmpaddr10, t2 \n"
		"lw t1, 24(t0) \n"
		"csrw pmpaddr9, t1 \n"
		"lw t2, 20(t0) \n"
		"csrw pmpaddr8, t2 \n"
		"lw t1, 16(t0) \n"
		"csrw pmpaddr7, t1 \n"
		"lw t2, 12(t0) \n"
		"csrw pmpaddr6, t2 \n"
		"lw t1, 8(t0) \n"
		"csrw pmpaddr5, t1 \n"
		"lw t2, 4(t0) \n"
		"csrw pmpaddr4, t2 \n"
		"lw t1, 0(t0) \n"
		"csrw pmpaddr3, t1 \n" ::"r"(xPMPSettings)
		: "t0", "t1", "t2");
#elif __riscv_xlen == 64
	__asm__ __volatile__("add t5, %0, 32 \n" /* get pmp address configs */
						 "la t1, 1f \n"      /* compute the jump address */
						 "add t2, t1, a2 \n"
						 "jr t2 \n"
						 "1: \n"
						 "ld t4, 96(t5) \n"
						 "csrw pmpaddr15, t4 \n"
						 "ld t3, 88(t5) \n"
						 "csrw pmpaddr14, t3 \n"
						 "ld t2, 80(t5) \n"
						 "csrw pmpaddr13, t2 \n"
						 "ld t1, 72(t5) \n"
						 "csrw pmpaddr12, t1 \n"
						 "ld t4, 64(t5) \n"
						 "csrw pmpaddr11, t4 \n"
						 "ld t3, 56(t5) \n"
						 "csrw pmpaddr10, t3 \n"
						 "ld t2, 48(t5) \n"
						 "csrw pmpaddr9, t2 \n"
						 "ld t1, 40(t5) \n"
						 "csrw pmpaddr8, t1 \n"
						 "ld t4, 32(t5) \n"
						 "csrw pmpaddr7, t4 \n"
						 "ld t3, 24(t5) \n"
						 "csrw pmpaddr6, t3 \n"
						 "ld t2, 16(t5) \n"
						 "csrw pmpaddr5, t2 \n"
						 "ld t1, 8(t5) \n"
						 "csrw pmpaddr4, t1 \n"
						 "ld t4, 0(t5) \n"
						 "csrw pmpaddr3, t4 \n" ::"r"(xPMPSettings)
						 : "t1", "t2", "t3", "t4");
#endif

#if __riscv_xlen == 32
	/* Compute jump offset to avoid configure unuse PMP 32bits version */
	__asm__ __volatile__("addi a0, a0, 2 \n" /* 2 regions for stack */
						 "srli t1, a0, 2 \n" /* divide by 4 */
						 "li t2, 3 \n"       /* number of config regs (4) */
						 "sub t2, t2, t1 \n"
						 "slli t2, t2, 4 \n" ::
						 : "a0", "t1", "t2");
#elif __riscv_xlen == 64
	/* Compute jump offset to avoid configure unuse PMP 64bits version */
	__asm__ __volatile__("addi a0, a0, 2 \n" /* 2 regions for stack */
						 "srli t1, a0, 3 \n" /* divide by 8 */
						 "li t2, 1 \n"       /* number of config regs (2) */
						 "sub t2, t2, t1 \n"
						 "slli t2, t2, 4 \n" ::
						 : "a0", "t1", "t2");
#endif

	/* Configure PMP mode (rights and mode) */
#if __riscv_xlen == 32
	__asm__ __volatile__("add t0, a1, 16 \n" /* get pmp config mask */

						 "la t1, 1f \n"
						 "add t1, t1, t2 \n"
						 "jr t1 \n"
						 "1: \n"

						 "lw t1, 12(t0) \n"
						 "lw t2, 12(a1)\n"
						 "csrc pmpcfg3, t1 \n"
						 "csrs pmpcfg3, t2 \n"

						 "lw t1, 8(t0) \n"
						 "lw t2, 8(a1)\n"
						 "csrc pmpcfg2, t1 \n"
						 "csrs pmpcfg2, t2 \n"

						 "lw t1, 4(t0) \n"
						 "lw t2, 4(a1)\n"
						 "csrc pmpcfg1, t1 \n"
						 "csrs pmpcfg1, t2 \n"

						 "lw t1, 0(t0) \n"
						 "lw t2, 0(a1)\n"
						 "csrc pmpcfg0, t1 \n"
						 "csrs pmpcfg0, t2 \n" ::
						 : "t0", "t1", "t2");
#elif __riscv_xlen == 64
	__asm__ __volatile__("add t3, a1, 16 \n" /* get pmp config mask */

						 "la t0, 1f \n"
						 "add t0, t0, t2 \n"
						 "jr t0 \n"
						 "1: \n"

						 "ld t1, 8(t3) \n"
						 "ld t2, 8(a1)\n"
						 "csrc pmpcfg2, t1 \n"
						 "csrs pmpcfg2, t2 \n"

						 "ld t1, 0(t3) \n"
						 "ld t2, 0(a1)\n"
						 "csrc pmpcfg0, t1 \n"
						 "csrs pmpcfg0, t2 \n" ::
						 : "t0", "t1", "t2", "t3", "t4");
#endif
	__asm__ __volatile__("ret \n" :::);
}
#endif // #if( portUSING_MPU_WRAPPERS == 1 )

#if (portRISCV_STACK_PROTECT == 1)
Task_Stack_Guard_t Task_Stack_Guard[MAX_PMP_REGION];

void vPortStoreTaskStackGuardSettings(portUBASE_TYPE StackBottom,
									  __UNUSED uint32_t StackDepth) PRIVILEGED_FUNCTION {
	uint32_t pmp_idx;
	portUBASE_TYPE pmp_addr;
	uint8_t pmp_config;

	/**
	 *  considered as unused in certain cases because of macro
	 * configASSERT_DEFINED
	 */
	int32_t lResult __attribute__((unused)) = PMP_DEFAULT_ERROR;

	if (0 == xPmpInfo.granularity)
	{
		lResult = init_pmp(&xPmpInfo);
#if (configASSERT_DEFINED == 1)
		{
			configASSERT(0 <= lResult);
		}
#endif
		memset((void *)Task_Stack_Guard, 0, sizeof(Task_Stack_Guard));
	}

	// find a free stack guard buffer
	for (pmp_idx = 0; pmp_idx < MAX_PMP_REGION; pmp_idx++)
	{
		if (Task_Stack_Guard[pmp_idx].pmp_addr == 0) {
			break;
		}
	}

#if (configASSERT_DEFINED == 1)
	{
		configASSERT(pmp_idx < MAX_PMP_REGION);
	}
#endif

	if (pmp_idx >= MAX_PMP_REGION)
	{
		return;
	}

	/* make the guard region start address is located in stack memory and
	  is aligned to the TASK_STACK_GUARD_REGION_SIZE */
	StackBottom = (((StackBottom - 1) / TASK_STACK_GUARD_REGION_SIZE) + 1) *
	TASK_STACK_GUARD_REGION_SIZE;
	napot_addr_modifier(xPmpInfo.granularity, (size_t)StackBottom,
						(size_t *)&pmp_addr, TASK_STACK_GUARD_REGION_SIZE);
	pmp_config =
	((uint8_t)((portPMP_REGION_READ_ONLY) |
			   (portPMP_REGION_ADDR_MATCH_NAPOT)) |
	 portPMP_REGION_LOCK); // set the lock bit to enable PMP for M-mode
	lResult = write_pmp_config(&xPmpInfo, pmp_idx, pmp_config, pmp_addr);
#if (configASSERT_DEFINED == 1)
	{
		configASSERT(0 <= lResult);
	}
#endif
	Task_Stack_Guard[pmp_idx].pmp_addr = pmp_addr;
}
#endif // #if ( portRISCV_STACK_PROTECT == 1 )

__attribute__((weak)) void
vPortMExceptionHanler(portUBASE_TYPE mcause, portUBASE_TYPE mepc,
					  portUBASE_TYPE mstatus,
					  portUBASE_TYPE mtval) PRIVILEGED_FUNCTION {
#if (portUSING_MPU_WRAPPERS == 1)
	uint32_t pmp_cfg[4];
	uint32_t pmp_addr[16];
	uint32_t i;
#endif

	printf("===== Exception ====\r\n");
	printf("mcause = 0x%x \r\n", (unsigned int)mcause);
	printf("mepc = 0x%x \r\n", (unsigned int)mepc);
	printf("mtval = 0x%x \r\n", (unsigned int)mtval);
	printf("mstatus = 0x%x \r\n", (unsigned int)mstatus);
#if (portUSING_MPU_WRAPPERS == 1)
	read_pmp_configs((size_t *)pmp_cfg, 16);
	read_pmp_addrs((size_t *)pmp_addr, 16);
	printf(" pmp_cfg0 | pmp_cfg1 | pmp_cfg2 | pmp_cfg3\r\n");
	printf("0x%08x 0x%08x 0x%08x 0x%08x\r\n", (unsigned int)pmp_cfg[0],
		   (unsigned int)pmp_cfg[1], (unsigned int)pmp_cfg[2],
		   (unsigned int)pmp_cfg[3]);
	for (i = 0; i < 16; i++)
	{
		printf("pmp_addr%u = 0x%x\r\n", (unsigned int)i,
			   (unsigned int)((pmp_addr[i] & ~0x03) << 2));
	}
#endif

#if defined(DEBUG) &&  DEBUG
	// break for debugger is connected case
	__asm__ volatile("ebreak \n");
#else
	while (1)
	{
	};
#endif
}
/*-----------------------------------------------------------*/

#if( configENABLE_FPU == 1 )
void prvSetupFPU(void)   /* PRIVILEGED_FUNCTION */
{
	__asm__ __volatile__(
		"csrr t0, misa \n"			/* Get misa */
		"li   t1, 0x10028 \n"		/* 0x10028 = Q,F,D Quad, Single or Double precission floating point */
		"and  t0, t0, t1 \n"
		"beqz t0, 1f \n"			/* check if Q,F or D is present into misa */
		"csrr t0, mstatus \n"		/* Floating point unit is present so need to put it into initial state */
		"lui  t1, 0x2 \n"			/* t1 =  0x1 << 12 */
		"or   t0, t0, t1 \n"
		"csrw mstatus, t0 \n"		/* Set FS to initial state */
		"1: \n"
		:::
	);
}
#endif /* configENABLE_FPU */
/*-----------------------------------------------------------*/
