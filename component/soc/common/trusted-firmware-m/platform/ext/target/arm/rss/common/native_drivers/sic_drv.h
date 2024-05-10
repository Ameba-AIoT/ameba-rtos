/*
 * Copyright (c) 2022, Arm Limited. All rights reserved.
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


/**
 * \file sic_drv.h
 * \brief Driver for Arm Secure Instruction Cache (SIC).
 */

#ifndef __SIC_DRV_H__
#define __SIC_DRV_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Arm SIC error enumeration types
 */
enum sic_error_t {
    SIC_ERROR_NONE,
    SIC_ERROR_INVALID_REGION,
    SIC_ERROR_INVALID_ADDRESS,
    SIC_ERROR_INVALID_SIZE,
    SIC_ERROR_INVALID_ALIGNMENT,
    SIC_ERROR_NO_HW_SUPPORT,
    SIC_ERROR_INVALID_OP_WHILE_ENABLED,
    SIC_ERROR_RBG_SEED_ALREADY_SET,
};

/**
 * \brief Arm SIC Authentication AXI manager Priviliged/Unpriviliged config types
 */
enum sic_axim_priv_t {
    SIC_AXIM_PRIV_FORWARD = 0b00,
    SIC_AXIM_PRIV_UNPRIV  = 0b10,
    SIC_AXIM_PRIV_PRIV    = 0b11,
};

/**
 * \brief Arm SIC Authentication AXI manager Secure/Non-Secure config types
 */
enum sic_axim_secure_t {
    SIC_AXIM_SECURE_FORWARD = 0b00,
    SIC_AXIM_SECURE_SEC     = 0b10,
    SIC_AXIM_SECURE_NONSEC  = 0b11,
};

/**
 * \brief Arm SIC Authentication AXI manager Data/Instruction config types
 */
enum sic_axim_dinst_t {
    SIC_AXIM_DINST_FORWARD     = 0b00,
    SIC_AXIM_DINST_DATA        = 0b10,
    SIC_AXIM_DINST_INSTRUCTION = 0b11,
};

/**
 * \brief Arm SIC Authentication AXI manager Cacheability config types
 */
enum sic_axim_cache_t {
    SIC_AXIM_CACHE_FORWARD = 0b00,
    SIC_AXIM_CACHE_FALSE   = 0b10,
    SIC_AXIM_CACHE_TRUE    = 0b11,
};

/**
 * \brief Arm SIC Authentication AXI manager configuration struct
 */
struct sic_auth_axim_config_t {
    enum sic_axim_priv_t   priv;
    enum sic_axim_secure_t secure;
    enum sic_axim_dinst_t  dinst;
    enum sic_axim_cache_t  bufferable;
    enum sic_axim_cache_t  modifiable;
    enum sic_axim_cache_t  allocate;
    enum sic_axim_cache_t  other_allocate;
};

/**
 * \brief Arm SIC digest size configuration types
 */
enum sic_digest_size_t {
    SIC_DIGEST_SIZE_128 = 0x1,
    SIC_DIGEST_SIZE_256 = 0x2,
};

/**
 * \brief Arm SIC digest size configuration types
 */
enum sic_digest_config_t {
    SIC_DIGEST_COMPARE_FIRST_QWORD = 0x0,
    SIC_DIGEST_COMPARE_LAST_QWORD = 0x1,
    SIC_DIGEST_COMPARE_ALL,
};

/**
 * \brief Arm SIC Decryption keysize configuration types
 */
enum sic_decrypt_keysize_t {
    SIC_DECRYPT_KEYSIZE_128 = 0x0,
    SIC_DECRYPT_KEYSIZE_256 = 0x1
};

/**
 * \brief Arm SIC Decryption mitigation config struct
 */
struct sic_decrypt_mitigations_config_t {
    bool aes_dfa_enable;
    bool aes_dummy_enable;
    bool aes_dummy_valid_phase_enable;
    bool aes_dummy_valid_key_enable;
    uint8_t aes_dr_pre_rounds_max;
    uint8_t aes_dr_post_rounds_max;
    uint8_t aes_dr_valid_rounds_max;
};

/**
 * \brief Arm SIC PMON mode configuration types
 */
enum sic_pmon_counting_mode_t {
    SIC_PMON_COUNT_EVENTS = 0x0,
    SIC_PMON_COUNT_WORDS  = 0x1
};


/**
 * \brief Arm SIC PMON counters struct
 */
struct sic_pmon_counters_t {
    uint32_t page_hit_counter;
    uint32_t page_miss_counter;
    uint32_t bypass_counter;
};

/**
 * \brief Arm SIC device configuration structure
 */
struct sic_dev_cfg_t {
    const uint32_t base;                         /*!< SIC base address */
};

/**
 * \brief Arm SIC device structure
 */
struct sic_dev_t {
    const struct sic_dev_cfg_t* const cfg;       /*!< SIC configuration */
};

#define SIC_DECRYPT_DR_NONCE_BYTE_LEN (8)
#define SIC_DECRYPT_RBG_SEED_LEN      (16)

/**
 * \brief                  Enable the SIC. Setup should have been performed
 *                         prior to this, as many registers cannot be written
 *                         when the SIC is enabled.
 *
 * \param[in]  dev         The SIC device.
 *
 * \return                 SIC_ERROR_NONE on success, otherwise a different
 *                         sic_error_t.
 */
enum sic_error_t sic_enable(struct sic_dev_t *dev);
/**
 * \brief                  Disable the SIC. Does not alter device configuration.
 *
 * \param[in]  dev         The SIC device.
 *
 * \return                 SIC_ERROR_NONE on success, otherwise a different
 *                         sic_error_t.
 */
enum sic_error_t sic_disable(struct sic_dev_t *dev);

/**
 * \brief                  Get the page size of the SIC in bytes.
 *
 * \param[in]  dev         The SIC device.
 *
 * \return                 The page size of the SIC in bytes.
 */
size_t sic_page_size_get(struct sic_dev_t *dev);
/**
 * \brief                  Get the page count of the SIC in bytes.
 *
 * \param[in]  dev         The SIC device.
 *
 * \return                 The page count of the SIC in bytes.
 */
uint32_t sic_page_count_get(struct sic_dev_t *dev);

/**
 * \brief                    Setup the SIC authentication engine.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[in]  digest_size   The size of the hash digest the SIC should use, as
 *                           a sic_digest_size_t.
 *
 * \param[in]  digest_config The configuration value for the hash digest the SIC
 *                           should use, as a sic_digest_config_t.
 *
 * \param[in]  base          The RSS address which should be the start of the
 *                           authentication region.
 *
 * \param[in]  size          The size in bytes of the authentication region.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_init(struct sic_dev_t *dev,
                               enum sic_digest_size_t digest_size,
                               enum sic_digest_config_t digest_config,
                               uintptr_t base, size_t size);

/**
 * \brief                    Enable the SIC authentication engine. This must be
 *                           done while the SIC is disabled.
 *
 * \param[in]  dev           The SIC device.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_enable(struct sic_dev_t *dev);

/**
 * \brief                    Disable the SIC authentication engine. This must be
 *                           done while the SIC is disabled.
 *
 * \param[in]  dev           The SIC device.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_disable(struct sic_dev_t *dev);

/**
 * \brief                    Invalidate any pages in the authenticated page
 *                           cache. This is done when the authentication engine
 *                           is disabled, and must be done before any part of
 *                           the HTR is updated (but while the SIC is disabled).
 *
 * \param[in]  dev           The SIC device.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_invalidate_pages(struct sic_dev_t *dev);

/**
 * \brief                    Set the configuration of the SIC AXI manager.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[in]  cfg           The configuration of the AXI manager, as a
 *                           sic_auth_axim_config_t.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_axim_cfg_set(struct sic_dev_t *dev,
                                       const struct sic_auth_axim_config_t *cfg);

/**
 * \brief                    Get the configuration of the SIC AXI manager.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[out] cfg           The configuration of the AXI manager, as a
 *                           sic_auth_axim_config_t.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_axim_cfg_get(struct sic_dev_t *dev,
                                       struct sic_auth_axim_config_t *cfg);

/**
 * \brief                    Set part of the SIC HTR that is used to
 *                           authenticate pages.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[in] data           The data that should be loaded into the HTR.
 * \param[in] data_len       The size of the data that should be loaded into the
 *                           HTR in bytes.
 * \param[in] table_offset   The offset (in bytes) into the HTR that the data
 *                           should be loaded to.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_auth_table_set(struct sic_dev_t *dev, uint32_t *data,
                                    size_t data_len_bytes, size_t table_offset);

/**
 * \brief                             Setup the SIC decryption engine.
 *
 * \param[in]  dev                    The SIC device.
 *
 * \param[in]  decrypt_keysize        The size of the key the decryption engine
 *                                    will use, as a sic_decrypt_keysize_t.
 *
 * \param[in]  decrypt_padding_enable Whether decryption padding is enabled.
 *
 * \return                            SIC_ERROR_NONE on success, otherwise a
 *                                    different sic_error_t.
 */
enum sic_error_t sic_decrypt_init(struct sic_dev_t *dev,
                                  enum sic_decrypt_keysize_t decrypt_keysize,
                                  bool decrypt_padding_enable);

/**
 * \brief                  Setup a SIC decryption region.
 *
 * \param[in]  dev         The SIC device.
 *
 * \param[in]  region_idx  Which decryption region should be enabled.
 *
 * \param[in]  base        The RSS address of the start of the decryption
 *                         region.
 * \param[in]  size        The size in bytes of the decryption region.
 * \param[in]  fw_revision The firmware revision of the image pointed to by the
 *                         decryption region. This is used as part of the CTR
 *                         mode IV.
 * \param[in]  nonce       The remainder of the the CTR mode IV for this
 *                         decryption region. This must be
 *                         SIC_DECRYPT_DR_NONCE_BYTE_LEN bytes in length.
 *
 * \param[in]  key         The key for the decrypt region. This must be 16
 *                         bytes in length if the decryption engine was set up
 *                         with SIC_DECRYPT_KEYSIZE_128, or 32 bytes in length
 *                         if using SIC_DECRYPT_KEYSIZE_256.
 *
 * \return                 SIC_ERROR_NONE on success, otherwise a different
 *                         sic_error_t.
 */
enum sic_error_t sic_decrypt_region_enable(struct sic_dev_t *dev,
                                           uint8_t region_idx,
                                           uintptr_t base, size_t size,
                                           uint32_t fw_revision,
                                           uint32_t *nonce, uint32_t *key);

/**
 * \brief                  disable a SIC decryption region.
 *
 * \param[in]  dev         The SIC device.
 *
 * \param[in]  region_idx  Which decryption region should be disabled.
 *
 * \return                 SIC_ERROR_NONE on success, otherwise a different
 *                         sic_error_t.
 */
enum sic_error_t sic_decrypt_region_disable(struct sic_dev_t *dev,
                                            uint8_t region_idx);


/**
 * \brief                    Set the side-channel mitigation configuration of
 *                           the SIC decryption engine.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[in] cfg            The configuration of the SIC decryption engine
 *                           side-channel mitigations, as a
 *                           sic_decrypt_mitigations_config_t.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_decrypt_mitigations_set(struct sic_dev_t *dev,
                                             const struct sic_decrypt_mitigations_config_t *cfg);
/**
 * \brief                    Get the side-channel mitigation configuration of
 *                           the SIC decryption engine.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[out] cfg           The configuration of the SIC decryption engine
 *                           side-channel mitigations, as a
 *                           sic_decrypt_mitigations_config_t.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_decrypt_mitigations_get(struct sic_dev_t *dev,
                                             struct sic_decrypt_mitigations_config_t *cfg);

/**
 * \brief                    Set the seed from which the SIC decryption engine
 *                           DRBG will be instanciated. This must be provided
 *                           from a hardware TRNG.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[in]  seed          The seed value.
 *
 * \param[in]  seed_len      The seed length. In total, the seed must recieve
 *                           SIC_DECRYPT_RBG_SEED_LEN bytes of seed input.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_decrypt_rbg_seed_set(struct sic_dev_t *dev,
                                          const uint8_t *seed,
                                          size_t seed_len);

/**
 * \brief                    Enable the SIC performance monitoring unit.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[in]  counting_mode Which mode the PMON should count in, as a
 *                           sic_pmon_counting_mode_t.
 *
 * \param[in]  timer_enable  Whether the PMON should count forever, or for a set
 *                           interval.
 *
 * \param[in]  timer_val     How long the PMON should count for (must be 0
 *                           when timer_enable is not set).
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_pmon_enable(struct sic_dev_t *dev,
                                 enum sic_pmon_counting_mode_t counting_mode,
                                 bool timer_enable, uint32_t timer_val);

/**
 * \brief                    Disable the SIC performance monitoring unit.
 *
 * \param[in]  dev           The SIC device.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_pmon_disable(struct sic_dev_t *dev);

/**
 * \brief                    Fetch the counter values from the SIC PMON.
 *
 * \param[in]  dev           The SIC device.
 *
 * \param[out] dev           The combined counter values as a
 *                           sic_pmon_counters_t.
 *
 * \return                   SIC_ERROR_NONE on success, otherwise a different
 *                           sic_error_t.
 */
enum sic_error_t sic_pmon_get_stats(struct sic_dev_t *dev,
                                    struct sic_pmon_counters_t *counters);

#ifdef __cplusplus
}
#endif

#endif /* __SIC_DRV_H__ */
