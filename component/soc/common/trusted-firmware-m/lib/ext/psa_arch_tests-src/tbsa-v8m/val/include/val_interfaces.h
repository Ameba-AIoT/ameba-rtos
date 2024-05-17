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

#ifndef _VAL_INTERFACES_H_
#define _VAL_INTERFACES_H_

#include "val_common.h"
#include "val_pe.h"
#include "val_infra.h"
#include "val_test.h"
#include "val_interrupt.h"
#include "val_peripherals.h"
#include "val_crypto.h"
#include "val_fuse.h"
#include "val_debug.h"

#define TBSA_TEST_TITLE_SIZE     256
#define TBSA_TEST_REFTAG_SIZE    256
#define TBSA_TEST_ENTRY_FUNS_CNT 3

/* typedef's */
typedef struct {
    tbsa_status_t (*print)                     (print_verbosity_t verbosity, char *string, uint32_t data);
    tbsa_status_t (*set_status)                (uint32_t status);
    uint32_t      (*get_status)                (void);
    tbsa_status_t (*err_check_set)             (uint32_t checkpoint, tbsa_status_t status);
    tbsa_status_t (*report_status)             (test_id_t test_id);
    void          (*memcpy)                    (void *dst, void *src, uint32_t size);
    void          (*memset)                    (void *dst, uint32_t str, uint32_t size);
    uint32_t      (*execute_in_trusted_mode)   (addr_t address);
    tbsa_status_t (*nvram_read)                (addr_t base, uint32_t offset, void *buffer, int size);
    tbsa_status_t (*nvram_write)               (addr_t base, uint32_t offset, void *buffer, int size);
    void          (*system_reset)              (system_reset_t reset_type);
    bool_t        (*system_reset_type)         (system_reset_t reset_type);
    tbsa_status_t (*firmware_version_update)   (uint32_t instance, firmware_version_type_t firmware_version_type, uint32_t fw_ver_cnt);
    uint32_t      (*firmware_version_read)     (uint32_t instance, firmware_version_type_t firmware_version_type);
    tbsa_status_t (*test_initialize)           (tbsa_test_init_t *init);
    tbsa_status_t (*mem_reg_read)              (uint32_t reg_name, uint32_t *rd_data);
    tbsa_status_t (*mem_read)                  (uint32_t *address, mem_access_size_t access_size, uint32_t *rd_data);
    tbsa_status_t (*mem_reg_write)             (uint32_t reg_name, uint32_t wr_data);
    tbsa_status_t (*mem_write)                 (uint32_t *address, mem_access_size_t access_size, uint32_t wr_data);
    bool_t        (*is_secure_address)         (addr_t   address);
    tbsa_status_t (*interrupt_setup_handler)   (uint32_t irq_num, uint32_t priority, tbsa_isr_vector handler);
    tbsa_status_t (*interrupt_restore_handler) (uint32_t irq_num);
    tbsa_status_t (*interrupt_route)           (uint32_t irq_num, cpu_state_t curr_state);
    tbsa_status_t (*interrupt_disable)         (uint32_t irq_num);
    tbsa_status_t (*interrupt_enable)          (uint32_t irq_num);
    tbsa_status_t (*interrupt_set_pending)     (uint32_t intr_num);
    tbsa_status_t (*interrupt_clear_pending)   (uint32_t intr_num);
    tbsa_status_t (*interrupt_get_pending_status) (uint32_t intr_num, uint32_t *status);
    tbsa_status_t (*target_get_config)         (cfg_id_t cfg_id, uint8_t **data, uint32_t *size);
    tbsa_status_t (*timer_init)                (addr_t base_addr, uint32_t imer_us, uint32_t timer_tick_us);
    tbsa_status_t (*timer_enable)              (addr_t base_addr);
    tbsa_status_t (*timer_disable)             (addr_t base_addr);
    tbsa_status_t (*timer_interrupt_clear)     (addr_t base_addr);
    tbsa_status_t (*wd_timer_init)             (addr_t base_addr, uint32_t imer_us, uint32_t timer_tick_us);
    tbsa_status_t (*wd_timer_enable)           (addr_t base_addr);
    tbsa_status_t (*wd_timer_disable)          (addr_t base_addr);
    tbsa_status_t (*is_wd_timer_enabled)       (addr_t base_addr);
    tbsa_status_t (*crypto_key_generate)       (uint8_t *key, crypt_t enc_type, uint32_t size);
    tbsa_status_t (*fuse_get_lcs)              (uint32_t *pLcs);
    tbsa_status_t (*crypto_validate_public_key) (crypt_t type, uint32_t *key, uint32_t size, addr_t addr, uint32_t *valid);
    tbsa_status_t (*crypto_get_key_info)       (key_desc_t **key_info_desc, key_type_t key_type, uint32_t *instance);
    tbsa_status_t (*crypto_set_base_addr)      (dev_attr_t attribute);
    tbsa_status_t (*crypto_revoke_key)         (uint32_t index, addr_t addr, uint32_t size);
    tbsa_status_t (*fuse_ops)                  (fuse_ops_t fuse_ops, addr_t addr, uint32_t *data, uint32_t size);
    tbsa_status_t (*get_fuse_info)             (fuse_desc_t **fuse_info_desc, fuse_type_t fuse_type, uint32_t instance);
    tbsa_status_t (*debug_get_status)          (dbg_access_t dbg_access);
    tbsa_status_t (*debug_set_status)          (dbg_access_t dbg_access, dbg_seq_status_t dbg_status);
    tbsa_status_t (*dpm_set_state)             (dpm_desc_t *dpm_desc, dpm_status_t dbg_status);
    tbsa_status_t (*dpm_get_state)             (uint32_t index, uint32_t *dbg_status);
    tbsa_status_t (*dpm_set_access_ns_only)    (uint32_t index, bool_t access_ns);
    tbsa_status_t (*mpc_configure_security_attribute)    (addr_t mpc, addr_t start_addr,addr_t end_addr, mem_tgt_attr_t sec_attr);
    tbsa_status_t (*crypto_validate_certificate)    (addr_t certificate_base_addr,addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
    tbsa_status_t (*crypto_get_uniqueID_from_certificate)    (addr_t certificate_base_addr,addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
    bool_t        (*is_rtc_trustable)          (addr_t base_addr);
    bool_t        (*is_rtc_synced_to_server)   (addr_t base_addr);
    bool_t        (*is_vtor_relocated_from_rom) (void);
    tbsa_status_t (*crypto_get_dpm_from_key)    (addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field);
    tbsa_status_t (*crypto_get_dpm_from_certificate)    (addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field);
}tbsa_val_api_t;

typedef void (*test_fptr_t)(tbsa_val_api_t *val);

typedef struct {
    test_id_t     test_id;
    char          title[TBSA_TEST_TITLE_SIZE];
    char          ref_tag[TBSA_TEST_REFTAG_SIZE];
    test_fptr_t   test_fun[TBSA_TEST_ENTRY_FUNS_CNT];
}tbsa_test_info_t;

/* NSC entry points */
tbsa_status_t val_print_nsc                   (print_verbosity_t verbosity, char *string, uint32_t data);
tbsa_status_t val_set_status_nsc              (uint32_t status);
uint32_t      val_get_status_nsc              (void);
tbsa_status_t val_err_check_set_nsc           (uint32_t checkpoint, tbsa_status_t status);
tbsa_status_t val_report_status_nsc           (test_id_t test_id);
void          val_memcpy_nsc                  (void *dst, void *src, uint32_t size);
void          val_memset_nsc                  (void *dst, uint32_t str, uint32_t size);
uint32_t      val_execute_in_trusted_mode_nsc (addr_t address);

tbsa_status_t val_test_initialize_nsc (tbsa_test_init_t *init);

tbsa_status_t val_mem_reg_read_nsc      (uint32_t reg_name, uint32_t *rd_data);
tbsa_status_t val_mem_read_nsc          (uint32_t *address, mem_access_size_t access_size, uint32_t *rd_data);
tbsa_status_t val_mem_reg_write_nsc     (uint32_t reg_name, uint32_t wr_data);
tbsa_status_t val_mem_write_nsc         (uint32_t *address, mem_access_size_t access_size, uint32_t wr_data);
bool_t        val_is_secure_address_nsc (addr_t   address);

tbsa_status_t val_interrupt_restore_handler_nsc (uint32_t irq_num);
tbsa_status_t val_interrupt_setup_handler_nsc   (uint32_t irq_num, uint32_t priority, tbsa_isr_vector handler);
tbsa_status_t val_interrupt_route_nsc           (uint32_t irq_num, cpu_state_t curr_state);
tbsa_status_t val_interrupt_disable_nsc         (uint32_t irq_num);
tbsa_status_t val_interrupt_enable_nsc          (uint32_t irq_num);
tbsa_status_t val_interrupt_set_pending_nsc     (uint32_t intr_num);
tbsa_status_t val_interrupt_clear_pending_nsc   (uint32_t intr_num);
tbsa_status_t val_interrupt_get_pending_status_nsc (uint32_t intr_num, uint32_t *status);

tbsa_status_t val_target_get_config_nsc (cfg_id_t cfg_id, uint8_t **data, uint32_t *size);

tbsa_status_t val_timer_init_nsc            (addr_t base_addr, uint32_t timer_us, uint32_t timer_tick_us);
tbsa_status_t val_timer_enable_nsc          (addr_t base_addr);
tbsa_status_t val_timer_disable_nsc         (addr_t base_addr);
tbsa_status_t val_timer_interrupt_clear_nsc (addr_t base_addr);

tbsa_status_t val_wd_timer_init_nsc       (addr_t base_addr, uint32_t timer_us, uint32_t timer_tick_us);
tbsa_status_t val_wd_timer_enable_nsc     (addr_t base_addr);
tbsa_status_t val_wd_timer_disable_nsc    (addr_t base_addr);
tbsa_status_t val_is_wd_timer_enabled_nsc (addr_t base_addr);

tbsa_status_t val_crypto_key_generate_nsc        (uint8_t *key, crypt_t enc_type, uint32_t size);
tbsa_status_t val_crypto_validate_public_key_nsc (crypt_t type, uint32_t *key, uint32_t size, addr_t addr, uint32_t *valid);
tbsa_status_t val_crypto_get_key_info_nsc        (key_desc_t **key_info_desc, key_type_t key_type, uint32_t *instance);
tbsa_status_t val_crypto_set_base_addr_nsc       (dev_attr_t attribute);
tbsa_status_t val_crypto_revoke_key_nsc          (uint32_t index, addr_t addr, uint32_t size);
tbsa_status_t val_fuse_get_lcs_nsc               (uint32_t *pLcs);
tbsa_status_t val_fuse_ops_nsc                   (fuse_ops_t fuse_ops, addr_t addr, uint32_t *data, uint32_t size);
tbsa_status_t val_get_fuse_info_nsc              (fuse_desc_t **fuse_info_desc, fuse_type_t fuse_type, uint32_t instance);

tbsa_status_t val_nvram_read_nsc  (addr_t base, uint32_t offset, void *buffer, int size);
tbsa_status_t val_nvram_write_nsc (addr_t base, uint32_t offset, void *buffer, int size);

void           val_system_reset_nsc      (system_reset_t reset_type);
bool_t         val_system_reset_type_nsc (system_reset_t reset_type);

tbsa_status_t val_firmware_version_update_nsc (uint32_t instance, firmware_version_type_t firmware_version_type, uint32_t fw_ver_cnt);
uint32_t      val_firmware_version_read_nsc   (uint32_t instance, firmware_version_type_t firmware_version_type);

tbsa_status_t val_debug_get_status_nsc  (dbg_access_t dbg_access);
tbsa_status_t val_debug_set_status_nsc  (dbg_access_t dbg_access, dbg_seq_status_t dbg_status);
tbsa_status_t val_dpm_set_state_nsc     (dpm_desc_t *dpm_desc, dpm_status_t dbg_status);
tbsa_status_t val_dpm_get_state_nsc     (uint32_t index, uint32_t *dbg_status);
tbsa_status_t val_dpm_set_access_ns_only_nsc(uint32_t index, bool_t access_ns);
tbsa_status_t val_mpc_configure_security_attribute_nsc    (addr_t mpc, addr_t start_addr,addr_t end_addr, mem_tgt_attr_t sec_attr);
tbsa_status_t val_crypto_validate_certificate_nsc    (addr_t certificate_base_addr,addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
tbsa_status_t val_crypto_get_uniqueID_from_certificate_nsc    (addr_t certificate_base_addr,addr_t public_key_addr, uint32_t certificate_size, uint32_t public_key_size);
tbsa_status_t val_crypto_get_dpm_from_key_nsc    (addr_t public_key_addr, uint32_t public_key_size, uint32_t *dpm_field);
tbsa_status_t val_crypto_get_dpm_from_certificate_nsc    (addr_t certificate_base_addr, uint32_t certificate_size, uint32_t *dpm_field);

bool_t        val_is_rtc_trustable_nsc        (addr_t base_addr);
bool_t        val_is_rtc_synced_to_server_nsc (addr_t base_addr);

bool_t        val_is_vtor_relocated_from_rom_nsc (void);

#endif /* _VAL_INTERFACES_H_ */
