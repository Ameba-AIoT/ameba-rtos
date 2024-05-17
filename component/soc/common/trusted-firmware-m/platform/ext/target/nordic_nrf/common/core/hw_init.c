/*
 * Copyright (c) 2022 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <stdint.h>
#include <string.h>
#include <nrf.h>
#include "array.h"

#include <nrfx.h>
#include <hal/nrf_rtc.h>
#include <hal/nrf_uarte.h>
#include <hal/nrf_clock.h>
#include <hal/nrf_dppi.h>

/* Static asserts to verify that asm_core_cm33 defines matches core_cm33 */
#include <core_cm33.h>
#include "hw_init.h"

#if defined(NRF_HW_INIT_NRF_PERIPHERALS)
#if defined(NRF_RTC0) || defined(NRF_RTC1) || defined(NRF_RTC2)
static inline void nrf_cleanup_rtc(NRF_RTC_Type * rtc_reg)
{
    nrf_rtc_task_trigger(rtc_reg, NRF_RTC_TASK_STOP);
    nrf_rtc_event_disable(rtc_reg, 0xFFFFFFFF);
    nrf_rtc_int_disable(rtc_reg, 0xFFFFFFFF);
}
#endif

static void nrf_cleanup_uarte(NRF_UARTE_Type * uarte_reg)
{
/* All subscribe + reserved, i.e from SUBSCRIBE_STARTRX to EVENTS_CTS */
#define NRF_UARTE_SUBSCRIBE_CONF_OFFS offsetof(NRF_UARTE_Type, SUBSCRIBE_STARTRX)
#define NRF_UARTE_SUBSCRIBE_CONF_SIZE (offsetof(NRF_UARTE_Type, EVENTS_CTS) -\
                                       NRF_UARTE_SUBSCRIBE_CONF_OFFS)

/* All publish + reserved, i.e from PUBLISH_CTS to SHORTS */
#define NRF_UARTE_PUBLISH_CONF_OFFS offsetof(NRF_UARTE_Type, PUBLISH_CTS)
#define NRF_UARTE_PUBLISH_CONF_SIZE (offsetof(NRF_UARTE_Type, SHORTS) -\
                                     NRF_UARTE_PUBLISH_CONF_OFFS)

    nrf_uarte_disable(uarte_reg);
    nrf_uarte_int_disable(uarte_reg, 0xFFFFFFFF);
#if defined(NRF_DPPIC)
    /* Clear all SUBSCRIBE configurations. */
    memset((uint8_t *)uarte_reg + NRF_UARTE_SUBSCRIBE_CONF_OFFS, 0, NRF_UARTE_SUBSCRIBE_CONF_SIZE);
    /* Clear all PUBLISH configurations. */
    memset((uint8_t *)uarte_reg + NRF_UARTE_PUBLISH_CONF_OFFS, 0, NRF_UARTE_PUBLISH_CONF_SIZE);
#endif
}

static void nrf_cleanup_peripheral_interconnect(void)
{
#if defined(NRF_PPI)
    nrf_ppi_channels_disable_all(NRF_PPI);
#endif
#if defined(NRF_DPPIC)
    nrf_dppi_channels_disable_all(NRF_DPPIC);
#endif
}

static void nrf_cleanup_clock(void)
{
    nrf_clock_int_disable(NRF_CLOCK, 0xFFFFFFFF);
}

/* This routine resets a subset of nRF Peripherals.
 * The subset is based on what MCUBoot has used before TF-M has been booted.
 */
static void hw_init_nrf_peripherals(void)
{
#if defined(NRF_RTC0)
    nrf_cleanup_rtc(NRF_RTC0);
#endif
#if defined(NRF_RTC1)
    nrf_cleanup_rtc(NRF_RTC1);
#endif
#if defined(NRF_RTC2)
    nrf_cleanup_rtc(NRF_RTC2);
#endif

#if defined(NRF_UARTE0)
    nrf_cleanup_uarte(NRF_UARTE0);
#endif
#if defined(NRF_UARTE1)
    nrf_cleanup_uarte(NRF_UARTE1);
#endif

    nrf_cleanup_peripheral_interconnect();

    nrf_cleanup_clock();
}
#endif /* NRF_HW_INIT_NRF_PERIPHERALS */

/* This routine resets Cortex-M system control block components and core
 * registers.
 */
void hw_init_reset_on_boot(void)
{
	/* Disable interrupts */
	__disable_irq();

	/* Reset exception and interrupt mask state (PRIMASK handled by
	 * __enable_irq below)
	 */
	__set_FAULTMASK(0);
	__set_BASEPRI(0);

	/* Disable NVIC interrupts */
	for (int i = 0; i < ARRAY_SIZE(NVIC->ICER); i++) {
		NVIC->ICER[i] = 0xFFFFFFFF;
	}
	/* Clear pending NVIC interrupts */
	for (int i = 0; i < ARRAY_SIZE(NVIC->ICPR); i++) {
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}

#if defined(NRF_HW_INIT_NRF_PERIPHERALS)
	/* Reset nRF Peripherals that may not have been reset by MCUBoot.
	 * This should be done before interrupts are enabled again since these
	 * interrupts may be sent to the application before the application has
	 * been properly initialized.
	 */
	hw_init_nrf_peripherals();
#endif
	/* Restore Interrupts */
	__enable_irq();

	__DSB();
	__ISB();
}
