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
struct NRF_WDT_Type{              /*!< (@ 0x40018000) WDT0_NS Structure                       */
__OM  uint32_t  TASKS_START;      /*!< (@ 0x00000000) Start WDT                               */
__OM  uint32_t  TASKS_STOP;       /*!< (@ 0x00000004) Stop WDT                                */
__IM  uint32_t  RESERVED[30];
__IOM uint32_t  SUBSCRIBE_START;  /*!< (@ 0x00000080) Subscribe configuration for task START  */
__IOM uint32_t  SUBSCRIBE_STOP;   /*!< (@ 0x00000084) Subscribe configuration for task STOP   */
__IM  uint32_t  RESERVED1[30];
__IOM uint32_t  EVENTS_TIMEOUT;   /*!< (@ 0x00000100) Watchdog timeout                        */
__IOM uint32_t  EVENTS_STOPPED;   /*!< (@ 0x00000104) Watchdog stopped                        */
__IM  uint32_t  RESERVED2[30];
__IOM uint32_t  PUBLISH_TIMEOUT;  /*!< (@ 0x00000180) Publish configuration for event TIMEOUT */
__IOM uint32_t  PUBLISH_STOPPED;  /*!< (@ 0x00000184) Publish configuration for event STOPPED */
__IM  uint32_t  RESERVED3[95];
__IOM uint32_t  INTENSET;         /*!< (@ 0x00000304) Enable interrupt                        */
__IOM uint32_t  INTENCLR;         /*!< (@ 0x00000308) Disable interrupt                       */
__IM  uint32_t  RESERVED4[6];
__IOM uint32_t  NMIENSET;         /*!< (@ 0x00000324) Enable interrupt                        */
__IOM uint32_t  NMIENCLR;         /*!< (@ 0x00000328) Disable interrupt                       */
__IM  uint32_t  RESERVED5[53];
__IM  uint32_t  RUNSTATUS;        /*!< (@ 0x00000400) Run status                              */
__IM  uint32_t  REQSTATUS;        /*!< (@ 0x00000404) Request status                          */
__IM  uint32_t  RESERVED6[63];
__IOM uint32_t  CRV;              /*!< (@ 0x00000504) Counter reload value                    */
__IOM uint32_t  RREN;             /*!< (@ 0x00000508) Enable register for reload request
                                        registers */
__IOM uint32_t  CONFIG;           /*!< (@ 0x0000050C) Configuration register                  */
__IM  uint32_t  RESERVED7[4];
__OM  uint32_t  TSEN;             /*!< (@ 0x00000520) Task stop enable                        */
__IM  uint32_t  RESERVED8[55];
__OM  uint32_t  RR[8];            /*!< (@ 0x00000600) Description collection: Reload request n*/
};                                /*!< Size = 1568 (0x620)                                    */

/* Register: WDT_CONFIG */
/* Description: Configuration register */
#define WDT_CONFIG_STOPEN_Pos (6UL) /*!< Position of STOPEN field. */
#define WDT_CONFIG_STOPEN_Disable (0UL) /*!< Do not allow stopping WDT */
#define WDT_CONFIG_STOPEN_Enable (1UL) /*!< Allow stopping WDT */

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

    /* From nRF53 documentation, the timeout in seconds is
     *       timeout [s] = (CRV + 1) / 32768
     */
    uint32_t ticks = ((time_us / 1000000) * 32768) - 1;

    if ((ticks >= 0xFFFFFFFFUL) || (ticks < 0xFUL))
    {
        // Maximum and minimum values defined in nRF53 documentation
        return PAL_STATUS_ERROR;
    }

    nrf_wdt->CRV = ticks;

    // Enable Stop and keep the watchdog running while CPU is sleeping
    nrf_wdt->CONFIG = ((WDT_CONFIG_STOPEN_Enable << WDT_CONFIG_STOPEN_Pos) |
                       (WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos));

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
    nrf_wdt->TASKS_START = 0x01UL; // Trigger start task

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
    nrf_wdt->TSEN = 0x6E524635UL; // Special value to enable stop task
    nrf_wdt->TASKS_STOP = 0x01UL; // Trigger stop task

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
