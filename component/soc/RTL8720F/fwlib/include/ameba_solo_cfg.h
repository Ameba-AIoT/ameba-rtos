/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file    ameba_solo_cfg.h
 * @brief   SOLO dual-core isolation configuration interface.
 *
 * In SOLO mode the two application cores (secure KM4TZ = mcu_app, non-secure
 * KM4NS = iot_app) share one die. With TrustZone compiled out the boot code
 * would otherwise leave every memory region / peripheral shared, so KM4NS
 * could read KM4TZ's private SRAM/PSRAM/flash and reach every peripheral.
 *
 * This module lets the user describe the isolation in ONE plain configuration
 * file (ameba_solocfg.c) using only two kinds of entries:
 *   - memory regions (start/end addresses) the KM4NS core may access;
 *   - peripherals reserved for the KM4TZ core.
 * Everything not listed stays private to KM4TZ. The user never deals with the
 * underlying hardware protection controllers - that mapping is done in the
 * (closed) library ameba_solo_cfg_lib.c and applied once at boot by the secure
 * KM4TZ image1 (BOOT_RAM_SoloCfg), before the KM4NS core is released.
 */

#ifndef _AMEBA_SOLO_CFG_H_
#define _AMEBA_SOLO_CFG_H_

#include "basic_types.h"
#include "rom/fault_injection_hardening.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Max memory regions the user may expose to KM4NS per storage type. */
#define SOLO_REGION_NUM		4

/** One memory region (address range) the non-secure KM4NS core may access. */
typedef struct {
	u32 start;			/*!< region base address */
	u32 end;			/*!< region end address (inclusive) */
} SOLO_REGION_TypeDef;

/**
 * Peripherals that may be reserved for the secure KM4TZ core.
 * The names below are the only handle the user needs; the mapping to the
 * hardware security controller is internal to ameba_solo_cfg_lib.c.
 * NOTE: system peripherals that must always stay shared (inter-core IPC,
 * SYSON, RSIP, RXI300) are intentionally NOT listed here.
 */
typedef enum {
	/* crypto / security engines (typical KM4TZ-only) */
	SOLO_PERIPH_PKE = 0,	/*!< public-key (PKA) engine */
	SOLO_PERIPH_LALU_KEY,	/*!< LALU key management */
	SOLO_PERIPH_AES,		/*!< AES */
	SOLO_PERIPH_SHA,		/*!< SHA / HMAC */
	SOLO_PERIPH_AES_SHA_DMA,/*!< AES/SHA DMA */
	SOLO_PERIPH_TRNG,		/*!< true RNG */
	SOLO_PERIPH_OTPC,		/*!< OTP controller */
	/* general peripherals */
	SOLO_PERIPH_WIFI,		/*!< WiFi (runs on KM4NS in SOLO) */
	SOLO_PERIPH_SDN,		/*!< SDN */
	SOLO_PERIPH_SDIO_DEVICE,/*!< SDIO device */
	SOLO_PERIPH_SPORT0_I2S,	/*!< SPORT0 / I2S */
	SOLO_PERIPH_GDMA0,		/*!< GDMA0 */
	SOLO_PERIPH_SPI0,		/*!< SPI0 */
	SOLO_PERIPH_SPI1,		/*!< SPI1 */
	SOLO_PERIPH_UART0,		/*!< UART0 */
	SOLO_PERIPH_UART1,		/*!< UART1 */
	SOLO_PERIPH_UART2,		/*!< UART2 */
	SOLO_PERIPH_UART_LOG,	/*!< log UART */
	SOLO_PERIPH_TIMER_4_8,	/*!< pulse / PWM timers 4~8 */
	SOLO_PERIPH_I2C0,		/*!< I2C0 */
	SOLO_PERIPH_I2C1,		/*!< I2C1 */
	SOLO_PERIPH_PSRAM_PHY,	/*!< PSRAM PHY */
	SOLO_PERIPH_PSRAM_SPIC,	/*!< PSRAM SPIC user mode */
	SOLO_PERIPH_SPIC,		/*!< flash SPIC user mode */
	SOLO_PERIPH_IR,			/*!< IR */
	SOLO_PERIPH_DBG_TIMER,	/*!< debug timer */
	SOLO_PERIPH_PMC_TIMER,	/*!< PMC timer 0/1 */
	SOLO_PERIPH_TIMER_0_3,	/*!< basic timers 0~3 */
	SOLO_PERIPH_ADC,		/*!< ADC / comparator */
	SOLO_PERIPH_GPIO,		/*!< GPIO A/B/C */
	/* BPC-governed peripherals (LSYS SEC_BPC_CTRL / SEC_PPC_CTRL) that the RXI300
	 * PPC above does not reach. System-level BPC domains (IPC, CPU0/CPU1, SWRST,
	 * PMC) are intentionally omitted here - they must stay shared for both cores. */
	SOLO_PERIPH_GMAC,		/*!< Ethernet MAC */
	SOLO_PERIPH_BT,			/*!< Bluetooth */
	SOLO_PERIPH_WDG,		/*!< watchdog */
	SOLO_PERIPH_RTC,		/*!< RTC */
	SOLO_PERIPH_BOR,		/*!< brown-out reset */
	SOLO_PERIPH_ATIM,		/*!< ATIM */
	SOLO_PERIPH_CHIPEN,		/*!< chip-enable control */
	SOLO_PERIPH_MAX,
} SOLO_PERIPH_TypeDef;

/* User configuration, defined in ameba_solocfg.c. The peripheral list is
 * terminated by SOLO_PERIPH_MAX; the memory tables are fixed at SOLO_REGION_NUM
 * entries each, with unused entries set to {0xFFFFFFFF, 0xFFFFFFFF}. */
extern const SOLO_PERIPH_TypeDef solo_km4tz_periph[];				/*!< peripherals kept private to KM4TZ (SOLO_PERIPH_MAX terminated) */
extern const SOLO_REGION_TypeDef solo_km4ns_flash[SOLO_REGION_NUM];	/*!< flash regions KM4NS may access */
extern const SOLO_REGION_TypeDef solo_km4ns_sram[SOLO_REGION_NUM];	/*!< SRAM regions KM4NS may access */
extern const SOLO_REGION_TypeDef solo_km4ns_psram[SOLO_REGION_NUM];	/*!< PSRAM regions KM4NS may access */

/**
 * @brief  Apply the SOLO isolation configured in ameba_solocfg.c.
 * @note   Called from the secure KM4TZ image1 in place of the TrustZone path
 *         (SOLO and TrustZone are mutually exclusive) before KM4NS is released.
 *         Programs and locks the memory / peripheral protection so KM4NS cannot
 *         reach KM4TZ's private memory or reserved peripherals.
 * @retval FIH_SUCCESS on success.
 */
fih_ret BOOT_RAM_SoloCfg(void);

/** Number of SOLO MPU entries the user can populate in ameba_solocfg.c. */
#define SOLO_MPU_ENTRY_NUM	2

/* Memory regions the KM4TZ app must NOT write. Filled in ameba_solocfg.c and applied
 * by app_start via app_solo_mpu_init(). Set unused entries to {0xFFFFFFFF, 0xFFFFFFFF}. */
extern const SOLO_REGION_TypeDef solo_km4tz_ro_sram[SOLO_MPU_ENTRY_NUM];

#ifdef __cplusplus
}
#endif

#endif /* _AMEBA_SOLO_CFG_H_ */
