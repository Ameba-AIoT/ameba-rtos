/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "uart_cmsdk.h"
#include "timer_cmsdk.h"
#include "wd_cmsdk.h"
#include "pal_dpm_fvp.h"
#include "nvram.h"
#include "pal_database.h"

void *pal_get_target_cfg_start(void)
{
    return (void *)database;
}

void pal_NVIC_EnableIRQ(uint32_t intr_num)
{
    NVIC_EnableIRQ(intr_num);
}

void pal_NVIC_DisableIRQ(uint32_t intr_num)
{
    NVIC_DisableIRQ(intr_num);
}

uint32_t pal_NVIC_ClearTargetState(uint32_t intr_num)
{
    return NVIC_ClearTargetState(intr_num);
}

uint32_t pal_NVIC_SetTargetState(uint32_t intr_num)
{
    return NVIC_SetTargetState(intr_num);
}

void pal_NVIC_SetPriority(uint32_t intr_num, uint32_t priority)
{
    return NVIC_SetPriority(intr_num, priority);
}

uint32_t pal_NVIC_GetPriority(uint32_t intr_num)
{
    return NVIC_GetPriority(intr_num);
}

void pal_NVIC_SetPendingIRQ(uint32_t intr_num)
{
    NVIC_SetPendingIRQ(intr_num);
}

void pal_NVIC_ClearPendingIRQ(uint32_t intr_num)
{
    NVIC_ClearPendingIRQ(intr_num);
}

uint32_t pal_NVIC_GetPendingIRQ(uint32_t intr_num)
{
    return NVIC_GetPendingIRQ(intr_num);
}

uint32_t pal_NVIC_GetActive(uint32_t intr_num)
{
    return NVIC_GetActive(intr_num);
}

int32_t pal_uart_init(addr_t addr)
{
    if (uart_init((void*)addr) != PAL_STATUS_SUCCESS) {
        return PAL_STATUS_ERROR;
    } else {
        return PAL_STATUS_SUCCESS;
    }
}

int32_t pal_uart_tx(addr_t addr, const void *data, uint32_t num)
{
    if (uart_putc((void*)addr, data, num) != PAL_STATUS_SUCCESS) {
        return PAL_STATUS_ERROR;
    } else {
        return PAL_STATUS_SUCCESS;
    }
}

int32_t pal_i2c_init(addr_t addr)
{
    return PAL_STATUS_ERROR;
}

int32_t pal_i2c_read(addr_t slv_addr, uint8_t *rd_data, uint32_t len)
{
    return PAL_STATUS_ERROR;
}

int32_t pal_i2c_write(addr_t slv_addr, uint8_t *wr_data, uint32_t len)
{
    return PAL_STATUS_ERROR;
}

int32_t pal_spi_init(addr_t addr)
{
   return PAL_STATUS_ERROR;
}

int32_t pal_spi_read(addr_t addr, void *data, uint32_t num)
{
    return PAL_STATUS_ERROR;
}

int32_t pal_spi_write(addr_t addr, const void *data, uint32_t num)
{
    return PAL_STATUS_ERROR;
}

int pal_timer_init (addr_t addr, uint32_t time_us, uint32_t timer_tick_us)
{
    return timer_cmsdk_init(addr, time_us, timer_tick_us);
}

int pal_timer_enable (addr_t addr)
{
    return timer_cmsdk_enable(addr);
}

int pal_timer_disable (addr_t addr)
{
    return timer_cmsdk_disable(addr);
}

int pal_timer_interrupt_clear (addr_t addr)
{
    return timer_cmsdk_interrupt_clear(addr);
}

int pal_crypto_aes_generate_key(uint8_t *key, uint32_t size)
{
    return PAL_STATUS_ERROR;
}

int pal_crypto_compute_hash(unsigned char *input, uint32_t ilen, unsigned char *output, int algo)
{
    return PAL_STATUS_ERROR;
}

int pal_wd_timer_init (addr_t addr, uint32_t time_us, uint32_t timer_tick_us)
{
    return wd_cmsdk_init(addr, time_us, timer_tick_us);
}

int pal_wd_timer_enable (addr_t addr)
{
    return wd_cmsdk_enable(addr);
}

int pal_wd_timer_disable (addr_t addr)
{
    return wd_cmsdk_disable(addr);
}

int pal_is_wd_timer_enabled (addr_t addr)
{
    return wd_cmsdk_is_enabled(addr);
}

int pal_dpm_set_access_ns_only(uint32_t index, bool_t access_ns)
{
    return pal_dpm_fvp_set_access_ns_only(index, access_ns);
}

int pal_mpc_configure_mem_region(addr_t mpc, addr_t start_addr,addr_t end_addr, uint32_t attr)
{
    return PAL_STATUS_ERROR;
}

void pal_crypto_init(addr_t crypto_base_addr)
{
    return;
}

int pal_fuse_read(addr_t addr, uint32_t *data, uint32_t size)
{
    return PAL_STATUS_ERROR;
}

int pal_fuse_write(addr_t addr, uint32_t *data, uint32_t size)
{
    return PAL_STATUS_ERROR;
}

int pal_fuse_count_zeros_in_rotpk(uint32_t *zero_cnt)
{
    return PAL_STATUS_ERROR;
}

void pal_fuse_count_zeros(uint32_t value, uint32_t *zero_cnt)
{
    return;
}

int pal_nvram_write(addr_t base, uint32_t offset, void *buffer, int size)
{
    return nvram_write(base, offset, buffer, size);
}

int pal_nvram_read(addr_t base, uint32_t offset, void *buffer, int size)
{
    return nvram_read(base, offset, buffer, size);
}

void pal_system_warm_reset(void)
{
    NVIC_SystemReset();
}

void pal_system_cold_reset(void)
{
    return;
}

int pal_is_cold_reset(void)
{
    return 0;
}

int pal_is_warm_reset(void)
{
    /* Simulating as if we get this information from trusted world */
    int x = *(uint32_t*)0x50000000UL;
    return x;
}

int pal_is_wdog_reset(void)
{
    return 0;
}

int pal_fuse_get_lcs(uint32_t *pLcs)
{
    return PAL_STATUS_ERROR;
}

int pal_rtc_init(addr_t addr)
{
    return PAL_STATUS_ERROR;
}

int pal_is_rtc_trustable(addr_t addr)
{
    return PAL_STATUS_ERROR;
}

int pal_is_rtc_synced_to_server(addr_t addr)
{
    return PAL_STATUS_ERROR;
}

int pal_crypto_validate_certificate(addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size)
{
    return PAL_STATUS_ERROR;
}

int pal_crypto_get_uniqueID_from_certificate(addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size)
{
    return PAL_STATUS_ERROR;
}

int pal_crypto_get_dpm_from_key(addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field)
{
    return PAL_STATUS_ERROR;
}

int pal_crypto_get_dpm_from_certificate(addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field)
{
    return PAL_STATUS_ERROR;
}

int pal_firmware_version_update(uint32_t instance, uint32_t firmware_version_type, uint32_t fw_ver_cnt)
{
    return PAL_STATUS_ERROR;
}

int pal_firmware_version_read(uint32_t instance, uint32_t firmware_version_type)
{
    return PAL_STATUS_ERROR;
}

int pal_crypto_revoke_key(uint32_t index, addr_t addr, uint32_t size)
{
    return PAL_STATUS_ERROR;
}

int pal_dpm_set_state(uint32_t index, uint32_t dpm_status, uint32_t unlock_token, void *token)
{
    return PAL_STATUS_ERROR;
}

int pal_dpm_get_state(uint32_t index, uint32_t *dpm_status)
{
    return PAL_STATUS_ERROR;
}
