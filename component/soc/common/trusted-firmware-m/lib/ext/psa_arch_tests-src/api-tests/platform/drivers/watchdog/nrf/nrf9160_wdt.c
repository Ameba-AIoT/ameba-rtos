 /** @file
  * Copyright (c) 2021 Nordic Semiconductor ASA.
  * SPDX-License-Identifier : Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *  http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
 **/

#include "nrf_wdt.h"

/* Following defines should be used for structure members */
#define     __IM     volatile const      /*! Defines 'read only' structure member permissions */
#define     __OM     volatile            /*! Defines 'write only' structure member permissions */
#define     __IOM    volatile            /*! Defines 'read / write' structure member permissions */

/**
  * @brief Watchdog Timer
  */
struct NRF_WDT_Type {              /*!< (@ 0x40018000) WDT_NS Structure */
  __OM  uint32_t  TASKS_START;     /*!< (@ 0x00000000) Start the watchdog */
  __IM  uint32_t  RESERVED[31];
  __IOM uint32_t  SUBSCRIBE_START; /*!< (@ 0x00000080) Subscribe configuration for task START */
  __IM  uint32_t  RESERVED1[31];
  __IOM uint32_t  EVENTS_TIMEOUT;  /*!< (@ 0x00000100) Watchdog timeout   */
  __IM  uint32_t  RESERVED2[31];
  __IOM uint32_t  PUBLISH_TIMEOUT; /*!< (@ 0x00000180) Publish configuration for event TIMEOUT  */
  __IM  uint32_t  RESERVED3[96];
  __IOM uint32_t  INTENSET;        /*!< (@ 0x00000304) Enable interrupt  */
  __IOM uint32_t  INTENCLR;        /*!< (@ 0x00000308) Disable interrupt */
  __IM  uint32_t  RESERVED4[61];
  __IM  uint32_t  RUNSTATUS;       /*!< (@ 0x00000400) Run status        */
  __IM  uint32_t  REQSTATUS;       /*!< (@ 0x00000404) Request status    */
  __IM  uint32_t  RESERVED5[63];
  __IOM uint32_t  CRV;             /*!< (@ 0x00000504) Counter reload value */
  __IOM uint32_t  RREN;       /*!< (@ 0x00000508) Enable register for reload request registers */
  __IOM uint32_t  CONFIG;          /*!< (@ 0x0000050C) Configuration register                */
  __IM  uint32_t  RESERVED6[60];
  __OM  uint32_t  RR[8];           /*!< (@ 0x00000600) Description collection: Reload request n */
};


/* Register: WDT_CONFIG */
/* Description: Configuration register */
#define WDT_CONFIG_SLEEP_Pos (0UL) /*!< Position of SLEEP field. */
#define WDT_CONFIG_SLEEP_Pause (0UL) /*!< Pause WDT while the CPU is sleeping */
#define WDT_CONFIG_SLEEP_Run (1UL) /*!< Keep WDT running while the CPU is sleeping */

/* Register: WDT_RUNSTATUS */
/* Description: Run status */
#define WDT_RUNSTATUS_RUNSTATUSWDT_Pos (0UL) /*!< Position of RUNSTATUSWDT field. */
 /*!< Bit mask of RUNSTATUSWDT field. */
#define WDT_RUNSTATUS_RUNSTATUSWDT_Msk (0x1UL << WDT_RUNSTATUS_RUNSTATUSWDT_Pos)
#define WDT_RUNSTATUS_RUNSTATUSWDT_NotRunning (0UL) /*!< Watchdog is not running */
#define WDT_RUNSTATUS_RUNSTATUSWDT_Running (1UL) /*!< Watchdog is running */

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
    @return          - SUCCESS, FAILURE
**/
int nrf_wdt_init(addr_t base_addr, uint32_t time_us)
{
    struct NRF_WDT_Type *nrf_wdt = (struct NRF_WDT_Type *)base_addr;

    /* The FF framework is written with the assumption that the watchdog timer
     * can be stopped and reconfigured, but this is not possible on the nRF91.
     * Therefore, the timeout value will only be written to the timer before
     * the timer is enabled. It is recommended to only use the highest duration
     * needed for the timeout value for all tests, as using a shorter duration
     * might result in an unexpected timeout.
     */
    if (!nrf_wdt_is_enabled(base_addr)) {
        /* From nRF91 documentation, the timeout in seconds is
        *       timeout [s] = (CRV + 1) / 32768
        */
        uint32_t ticks = ((time_us / 1000000) * 32768) - 1;

        if ((ticks >= 0xFFFFFFFFUL) || (ticks < 0xFUL))
        {
            // Maximum and minimum values defined in nRF91 documentation
            return PAL_STATUS_ERROR;
        }

        nrf_wdt->CRV = ticks;

        // Enable Stop and keep the watchdog running while CPU is sleeping
        nrf_wdt->CONFIG = (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);

        // Enable the reload request register to allow reloading the timer
        nrf_wdt->RREN = 0x01UL;
    }

    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS, FAILURE
**/
int nrf_wdt_enable(addr_t base_addr)
{
    struct NRF_WDT_Type *nrf_wdt = (struct NRF_WDT_Type *)base_addr;
    if (!nrf_wdt_is_enabled(base_addr)) {
        nrf_wdt->TASKS_START = 0x01UL; // Trigger start task
    } else {
        /* If the timer has already been started reload the timer instead of
         * triggering the start task, as the start task will not do anything
         * once the timer has already been started.
         */
        nrf_wdt->RR[0] = 0x6E524635UL; // Reload WDT
    }

    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS, FAILURE
**/
int nrf_wdt_disable(addr_t base_addr)
{
    struct NRF_WDT_Type *nrf_wdt = (struct NRF_WDT_Type *)base_addr;

    /* As the nRF91 WDT cannot be stopped, the timer will be reloaded to the
     * value in the CRV register. This should work in all cases, as the timer
     * will be reloaded at the beginning of each test case, and the time it
     * takes to tear down and set up a new test case should never exceed the
     * timeout duration.
     */
    if (nrf_wdt_is_enabled(base_addr)) {
        nrf_wdt->RR[0] = 0x6E524635UL;
    }

    return PAL_STATUS_SUCCESS;
}

/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr       : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int nrf_wdt_is_enabled(addr_t base_addr)
{
    struct NRF_WDT_Type *nrf_wdt = (struct NRF_WDT_Type *)base_addr;
    return (nrf_wdt->RUNSTATUS & WDT_RUNSTATUS_RUNSTATUSWDT_Msk);
}
