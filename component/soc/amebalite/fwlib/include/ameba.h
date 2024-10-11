/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _HAL_AMEBA_H_
#define _HAL_AMEBA_H_

#include "basic_types.h"
#include "section_config.h"
#include "memproc.h"
#include "strproc.h"
#include "rand.h"
#include "diag.h"
#include "hal_platform.h"
#include "ameba_vector.h"
#include "ameba_loguart.h"
#include "ameba_soc_ps.h"
#include "shell.h"
#include "monitor_rom.h"
#include "ameba_syscfg.h"
#include "ameba_pmc.h"
#include "ameba_clk.h"
#include "ameba_bor.h"
#include "ameba_ipsec.h"
#include "ameba_crypto_api.h"
#include "ameba_boot.h"
#include "ameba_rsip.h"
#include "ameba_spic.h"
#include "ameba_backup_reg.h"
#include "ameba_pinmap.h"
#include "ameba_ipc.h"
#include "sysreg.h"
#include "ameba_pinmux.h"
#include "ameba_otpc.h"
#include "ameba_rsip.h"
#include "ameba_arch.h"
#include "ameba_captouch.h"
#include "ameba_vadbuf.h"
#include "ameba_chipinfo.h"
#include "ameba_swr_calib.h"
#include "ameba_ipc_api.h"




/* ===========================  Configuration of the ARM ARMV8MBL Processor and Core Peripherals  ============================ */

#if defined (CONFIG_ARM_CORE_CM4)
#define __CM3_REV                      0x0200    /**< Core revision r0p0 */
#define __MPU_PRESENT                  1         /**< Defines if an MPU is present or not */
#define __NVIC_PRIO_BITS               3         /**< Number of priority bits implemented in the NVIC */
#define __Vendor_SysTickConfig         1         /**< Vendor specific implementation of SysTickConfig is defined *///see vPortSetupTimerInterrupt
#define __SAUREGION_PRESENT            1        /*!< SAU present or not                                                        */
#define DCACHE_4WAY

#define __FPU_PRESENT             1       /*!< FPU present                                   */
#define __VFP_FP__	1
#define __DSP_PRESENT             1

#define __PMU_PRESENT             1
#define __PMU_NUM_EVENTCNT        4       /*!< __PMU_NUM_EVENTCNT range is [2, 31]           */

#ifndef __ARM_FEATURE_CMSE
#define __ARM_FEATURE_CMSE	3
#endif
#include <arm_cmse.h>   /* Use CMSE intrinsics */
#include "core_armv81mml.h"
#include "core_cache.h"
#elif defined (CONFIG_RSICV_CORE_KR4)
#define __RISCV_REV                 0x0000U  /*!< RISCV Core Revision                                                    */

/* ----------------------------------------------------------------------------
   -- RISC-V MMCR register offset
   ---------------------------------------------------------------------------- */
#define RV_INTC_BASE (0x80000000u)

#include "core_riscv.h"
#include "cache_riscv.h"
#endif
#include "ameba_trustzone.h"
#include "mpu_config.h"

#include "ameba_gdma.h"
#include "ameba_pwmtimer.h"
#include "ameba_ups.h"
#include "ameba_gpio.h"
#include "ameba_spi.h"
#include "ameba_uart.h"
#include "ameba_i2c.h"
#include "ameba_adc.h"
#include "ameba_comp.h"
#include "ameba_wdg.h"
#include "ameba_rtc.h"
#include "ameba_delay.h"
#include "ameba_ledc.h"
#include "ameba_audio.h"
#include "ameba_sport.h"
#ifdef CONFIG_ARM_CORE_CM4
#include "ameba_cache.h"
#elif CONFIG_RSICV_CORE_KR4
#include "ameba_cache_kr4.h"
#endif
#include "ameba_thermal.h"
#include "ameba_debugtimer.h"
#include "ameba_trng.h"
#include "ameba_ecdsa.h"
#include "ameba_reset.h"
#include "ameba_psram.h"
#include "ameba_psphy.h"

#include "ameba_rcc.h"
#include "ameba_rom_patch.h"
#include "ameba_usrcfg.h"
#include "log.h"
#include "sscanf_minimal.h"

#define IMAGE_HEADER_LEN		0x20
typedef struct {
	u32 signature[2];
	u32 image_size;
	u32 image_addr;

	/* reserved for extention */
	u32 sb_header;
	u32 reserved[3];
} IMAGE_HEADER;

typedef enum  _HAL_Status {
	HAL_OK            = 0x00,
	HAL_BUSY          = 0x01,
	HAL_TIMEOUT       = 0x02,
	HAL_ERR_PARA      = 0x03,     // error with invaild parameters
	HAL_ERR_MEM       = 0x04,     // error with memory allocation failed
	HAL_ERR_HW        = 0x05,     // error with hardware error

	HAL_ERR_UNKNOWN   = 0xee      // unknown error

} HAL_Status;


#define USE_FULL_ASSERT

#ifdef  USE_FULL_ASSERT
/**
  * @brief  The assert_param macro is used for function's parameters check.
  * @param  expr: If expr is false, it calls assert_failed function which reports
  *         the name of the source file and the source line number of the call
  *         that failed. If expr is true, it returns no value.
  * @retval None
  */
#define assert_param(expr) ((expr) ? (void)0 : io_assert_failed((uint8_t *)__FILE__, __LINE__))
/* Exported functions ------------------------------------------------------- */
__NO_RETURN void io_assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif //_HAL_AMEBA_H_
