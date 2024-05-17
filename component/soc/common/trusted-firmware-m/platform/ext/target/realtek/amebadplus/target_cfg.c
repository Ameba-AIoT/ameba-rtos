/*
 * Copyright (c) 2017-2022 Arm Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cmsis.h"
#include "fih.h"
#include "target_cfg.h"
#include "Driver_MPC.h"
#include "region_defs.h"
#include "tfm_plat_defs.h"
#include "region.h"

#ifdef PSA_API_TEST_IPC
#define PSA_FF_TEST_SECURE_UART2
#endif

#define ARRAY_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))

/* The section names come from the scatter file */
REGION_DECLARE(Load$$LR$$, LR_NS_PARTITION, $$Base);
REGION_DECLARE(Image$$, ER_VENEER, $$Base);
REGION_DECLARE(Image$$, VENEER_ALIGN, $$Limit);

const struct memory_region_limits memory_regions = {
	.non_secure_code_start =
	(uint32_t) &REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
	BL2_HEADER_SIZE,

	.non_secure_partition_base =
	(uint32_t) &REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base),

	.non_secure_partition_limit =
	(uint32_t) &REGION_NAME(Load$$LR$$, LR_NS_PARTITION, $$Base) +
	NS_PARTITION_SIZE - 1,

	.veneer_base = (uint32_t) &REGION_NAME(Image$$, ER_VENEER, $$Base),
	.veneer_limit = (uint32_t) &REGION_NAME(Image$$, VENEER_ALIGN, $$Limit),
};

/* Allows software, via SAU, to define the code region as a NSC */
#define NSCCFG_CODENSC  1

/* Import MPC driver */
extern ARM_DRIVER_MPC Driver_SRAM1_MPC, Driver_SRAM2_MPC;

/* Define Peripherals NS address range for the platform */
#define PERIPHERALS_BASE_NS_START (0x40000000)
#define PERIPHERALS_BASE_NS_END   (0x4FFFFFFF)

/* Enable system reset request for CPU 0 */
#define ENABLE_CPU0_SYSTEM_RESET_REQUEST (1U << 4U)

/* To write into AIRCR register, 0x5FA value must be write to the VECTKEY field,
 * otherwise the processor ignores the write.
 */
#define SCB_AIRCR_WRITE_MASK ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos))

/* Debug configuration flags */
#define SPNIDEN_SEL_STATUS (0x01u << 7)
#define SPNIDEN_STATUS     (0x01u << 6)
#define SPIDEN_SEL_STATUS  (0x01u << 5)
#define SPIDEN_STATUS      (0x01u << 4)
#define NIDEN_SEL_STATUS   (0x01u << 3)
#define NIDEN_STATUS       (0x01u << 2)
#define DBGEN_SEL_STATUS   (0x01u << 1)
#define DBGEN_STATUS       (0x01u << 0)

#define All_SEL_STATUS (SPNIDEN_SEL_STATUS | SPIDEN_SEL_STATUS | \
                        NIDEN_SEL_STATUS | DBGEN_SEL_STATUS)

struct platform_data_t tfm_peripheral_std_uart = {
        UART0_REG_BASE,
        UART0_REG_BASE + 0xFFF,
        PPC_SP_DO_NOT_CONFIGURE,
        -1
};

#ifdef PSA_API_TEST_IPC

/* Below data structure are only used for PSA FF tests, and this pattern is
 * definitely not to be followed for real life use cases, as it can break
 * security.
 */

struct platform_data_t
	tfm_peripheral_FF_TEST_UART_REGION = {
	UART2_BASE_S,
	UART2_BASE_S + 0xFFF,
	PPC_SP_APB_PPC_EXP2,
	CMSDK_UART2_APB_PPC_POS
};

struct platform_data_t
	tfm_peripheral_FF_TEST_WATCHDOG_REGION = {
	APB_WATCHDOG_BASE_S,
	APB_WATCHDOG_BASE_S + 0xFFF,
	PPC_SP_DO_NOT_CONFIGURE,
	-1
};

#define FF_TEST_NVMEM_REGION_START            0x102FFC00
#define FF_TEST_NVMEM_REGION_END              0x102FFFFF
#define FF_TEST_SERVER_PARTITION_MMIO_START   0x3801FC00
#define FF_TEST_SERVER_PARTITION_MMIO_END     0x3801FD00
#define FF_TEST_DRIVER_PARTITION_MMIO_START   0x3801FE00
#define FF_TEST_DRIVER_PARTITION_MMIO_END     0x3801FF00

struct platform_data_t
	tfm_peripheral_FF_TEST_NVMEM_REGION = {
	FF_TEST_NVMEM_REGION_START,
	FF_TEST_NVMEM_REGION_END,
	PPC_SP_DO_NOT_CONFIGURE,
	-1
};

struct platform_data_t
	tfm_peripheral_FF_TEST_SERVER_PARTITION_MMIO = {
	FF_TEST_SERVER_PARTITION_MMIO_START,
	FF_TEST_SERVER_PARTITION_MMIO_END,
	PPC_SP_DO_NOT_CONFIGURE,
	-1
};

struct platform_data_t
	tfm_peripheral_FF_TEST_DRIVER_PARTITION_MMIO = {
	FF_TEST_DRIVER_PARTITION_MMIO_START,
	FF_TEST_DRIVER_PARTITION_MMIO_END,
	PPC_SP_DO_NOT_CONFIGURE,
	-1
};
#endif

enum tfm_plat_err_t enable_fault_handlers(void)
{
	/* Explicitly set secure fault priority to the highest */
	NVIC_SetPriority(SecureFault_IRQn, 0);

	/* Enables BUS, MEM, USG and Secure faults */
	SCB->SHCSR |= SCB_SHCSR_USGFAULTENA_Msk
				  | SCB_SHCSR_BUSFAULTENA_Msk
				  | SCB_SHCSR_MEMFAULTENA_Msk
				  | SCB_SHCSR_SECUREFAULTENA_Msk;
	return TFM_PLAT_ERR_SUCCESS;
}

enum tfm_plat_err_t system_reset_cfg(void)
{
#ifdef AMEBA_NONEED
	struct sysctrl_t *sysctrl = (struct sysctrl_t *)CMSDK_SYSCTRL_BASE_S;

	/* Enable system reset request for CPU 0, to be triggered via
	 * NVIC_SystemReset function.
	 */
	sysctrl->resetmask |= ENABLE_CPU0_SYSTEM_RESET_REQUEST;
#endif

	uint32_t reg_value = SCB->AIRCR;

	/* Clear SCB_AIRCR_VECTKEY value */
	reg_value &= ~(uint32_t)(SCB_AIRCR_VECTKEY_Msk);

	/* Enable system reset request only to the secure world */
	reg_value |= (uint32_t)(SCB_AIRCR_WRITE_MASK | SCB_AIRCR_SYSRESETREQS_Msk);

	SCB->AIRCR = reg_value;

	return TFM_PLAT_ERR_SUCCESS;
}

