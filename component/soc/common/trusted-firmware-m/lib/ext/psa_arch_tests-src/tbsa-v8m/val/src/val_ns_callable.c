/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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

#include "val_interfaces.h"

/* NSC entry points */

#define TRANSITION_NS_TO_S(fn_ret, fn_name, ...) \
    __attribute__((section(".tbsa_nsc_entry_points"), naked)) \
    fn_ret fn_name ## _nsc(__VA_ARGS__) { \
        __asm volatile( \
            "sg                   \n" \
            "push {r7}            \n" \
            "push {lr}            \n" \
            "ldr r7,=" #fn_name " \n" \
            "blx r7               \n" \
            "pop {r1, r7}         \n" \
            "bxns r1              \n" \
        ); \
        __builtin_unreachable(); \
    }

TRANSITION_NS_TO_S(tbsa_status_t, val_print, print_verbosity_t verbosity, char *string, uint32_t data);
TRANSITION_NS_TO_S(tbsa_status_t, val_set_status, uint32_t status);
TRANSITION_NS_TO_S(uint32_t,      val_get_status, void);
TRANSITION_NS_TO_S(tbsa_status_t, val_err_check_set, uint32_t checkpoint, tbsa_status_t status);
TRANSITION_NS_TO_S(tbsa_status_t, val_report_status, test_id_t test_id);
TRANSITION_NS_TO_S(void,          val_memcpy, void *dst, void *src, uint32_t size);
TRANSITION_NS_TO_S(void,          val_memset, void *dst, uint32_t str, uint32_t size);
TRANSITION_NS_TO_S(uint32_t,      val_execute_in_trusted_mode, addr_t address);
TRANSITION_NS_TO_S(tbsa_status_t, val_nvram_read, addr_t base, uint32_t offset, void *buffer, int size);
TRANSITION_NS_TO_S(tbsa_status_t, val_nvram_write, addr_t base, uint32_t offset, void *buffer, int size);
TRANSITION_NS_TO_S(void,          val_system_reset, system_reset_t reset_type);
TRANSITION_NS_TO_S(bool_t,        val_system_reset_type, system_reset_t reset_type);
TRANSITION_NS_TO_S(tbsa_status_t, val_firmware_version_update, uint32_t instace, firmware_version_type_t firmware_version_type, uint32_t fw_ver_cnt);
TRANSITION_NS_TO_S(uint32_t,      val_firmware_version_read, uint32_t instance, firmware_version_type_t firmware_version_type);
TRANSITION_NS_TO_S(tbsa_status_t, val_test_initialize, tbsa_test_init_t *init);
TRANSITION_NS_TO_S(tbsa_status_t, val_mem_reg_read, uint32_t reg_name, uint32_t *rd_data);
TRANSITION_NS_TO_S(tbsa_status_t, val_mem_read, uint32_t *address, mem_access_size_t access_size, uint32_t *rd_data);
TRANSITION_NS_TO_S(tbsa_status_t, val_mem_reg_write, uint32_t reg_name, uint32_t wr_data);
TRANSITION_NS_TO_S(tbsa_status_t, val_mem_write, uint32_t *address, mem_access_size_t access_size, uint32_t wr_data);
TRANSITION_NS_TO_S(bool_t,        val_is_secure_address, addr_t address);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_setup_handler, uint32_t irq_num, uint32_t priority, tbsa_isr_vector handler);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_restore_handler, uint32_t irq_num);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_route, uint32_t irq_num, cpu_state_t curr_state);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_disable, uint32_t irq_num);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_enable, uint32_t irq_num);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_set_pending, uint32_t intr_num);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_clear_pending, uint32_t intr_num);
TRANSITION_NS_TO_S(tbsa_status_t, val_interrupt_get_pending_status, uint32_t intr_num, uint32_t *status);
TRANSITION_NS_TO_S(tbsa_status_t, val_target_get_config, cfg_id_t cfg_id, uint8_t **data, uint32_t *size);
TRANSITION_NS_TO_S(tbsa_status_t, val_timer_init, addr_t base_addr, uint32_t time_ms, uint32_t timer_tick_us);
TRANSITION_NS_TO_S(tbsa_status_t, val_timer_enable, addr_t base_addr);
TRANSITION_NS_TO_S(tbsa_status_t, val_timer_disable, addr_t base_addr);
TRANSITION_NS_TO_S(tbsa_status_t, val_timer_interrupt_clear, addr_t base_addr);
TRANSITION_NS_TO_S(tbsa_status_t, val_wd_timer_init, addr_t base_addr, uint32_t time_ms, uint32_t timer_tick_us);
TRANSITION_NS_TO_S(tbsa_status_t, val_wd_timer_enable, addr_t base_addr);
TRANSITION_NS_TO_S(tbsa_status_t, val_wd_timer_disable, addr_t base_addr);
TRANSITION_NS_TO_S(tbsa_status_t, val_is_wd_timer_enabled, addr_t base_addr);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_key_generate, uint8_t *key, crypt_t enc_type, uint32_t size);
TRANSITION_NS_TO_S(tbsa_status_t, val_fuse_get_lcs, uint32_t *pLcs);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_validate_public_key, crypt_t type, uint32_t *key, uint32_t size, addr_t addr, uint32_t *valid);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_get_key_info, key_desc_t **key_info_desc, key_type_t key_type, uint32_t *instance);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_set_base_addr, dev_attr_t attribute);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_revoke_key, uint32_t index, addr_t addr, uint32_t size);
TRANSITION_NS_TO_S(tbsa_status_t, val_fuse_ops, fuse_ops_t fuse_ops, addr_t addr, uint32_t *data, uint32_t size);
TRANSITION_NS_TO_S(tbsa_status_t, val_get_fuse_info, fuse_desc_t **fuse_info_desc, fuse_type_t fuse_type, uint32_t instance);
TRANSITION_NS_TO_S(tbsa_status_t, val_debug_get_status, dbg_access_t dbg_access);
TRANSITION_NS_TO_S(tbsa_status_t, val_debug_set_status, dbg_access_t dbg_access, dbg_seq_status_t dbg_status);
TRANSITION_NS_TO_S(tbsa_status_t, val_dpm_set_state, dpm_desc_t *dpm_desc, dpm_status_t dpm_status);
TRANSITION_NS_TO_S(tbsa_status_t, val_dpm_get_state, uint32_t index, uint32_t *dpm_status);
TRANSITION_NS_TO_S(tbsa_status_t, val_dpm_set_access_ns_only, uint32_t index, bool_t access_ns);
TRANSITION_NS_TO_S(tbsa_status_t, val_mpc_configure_security_attribute, addr_t mpc, addr_t start_addr,addr_t end_addr, mem_tgt_attr_t sec_attr);
TRANSITION_NS_TO_S(bool_t,        val_is_rtc_trustable, addr_t base_addr);
TRANSITION_NS_TO_S(bool_t,        val_is_rtc_synced_to_server, addr_t base_addr);
TRANSITION_NS_TO_S(bool_t,        val_is_vtor_relocated_from_rom, void);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_validate_certificate, addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_get_uniqueID_from_certificate, addr_t certificate_base_addr, addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_get_dpm_from_key, addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field);
TRANSITION_NS_TO_S(tbsa_status_t, val_crypto_get_dpm_from_certificate, addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field);
