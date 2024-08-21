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

#include "cmsis_cpu.h"
#include "mpu_config.h"

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
#include "ameba_data_flash.h"
#include "ameba_backup_reg.h"
#include "ameba_pinmap.h"
#include "ameba_ipc.h"
#include "ameba_ipccfg.h"
#include "ameba_sema_rom.h"
#include "sysreg.h"
#include "ameba_pinmux.h"
#include "ameba_otpc.h"
#include "ameba_arch.h"
#include "ameba_captouch.h"
#include "ameba_chipinfo.h"
#include "ameba_ppe.h"
#include "ameba_qspi.h"
#include "ameba_sdio.h"
#include "ameba_usb.h"
#include "ameba_ipc_api.h"
#include "ameba_trustzone.h"
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
#include "ameba_rtc_io.h"
#include "ameba_delay.h"
#include "ameba_ir.h"
#include "ameba_ledc.h"
#include "ameba_audio.h"
#include "ameba_sport.h"
#include "ameba_cache.h"
#include "ameba_debugtimer.h"
#include "ameba_trng.h"
#include "ameba_reset.h"
#include "ameba_psram.h"
#include "ameba_kscan.h"
#include "ameba_pmctimer.h"

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
void io_assert_failed(uint8_t *file, uint32_t line);
#else
#define assert_param(expr) ((void)0)
#endif /* USE_FULL_ASSERT */

#endif //_HAL_AMEBA_H_
