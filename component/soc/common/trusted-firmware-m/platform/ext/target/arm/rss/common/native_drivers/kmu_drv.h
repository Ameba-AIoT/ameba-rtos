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
 * \file kmu_drv.h
 * \brief Driver for Arm KMU.
 */

#ifndef __KMU_DRV_H__
#define __KMU_DRV_H__

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Must be at least 16 */
#define KMU_PRBG_SEED_LEN (32)

#define KMU_GET_NKS(kmu_base) ( \
    1 << (uint8_t)(((kmu_base)->kmubc & KMU_KMUBC_NKS_MASK) >> KMU_KMUBC_NKS_OFF))

#define KMU_GET_NHWKSLTS(kmu_base) ( \
    (uint8_t)(((kmu_base)->kmubc & KMU_KMUBC_NHWKSLTS_MASK) >> \
               KMU_KMUBC_NHWKSLTS_OFF))

#define KMU_KMUBC_NKS_OFF                    16u
     /*!< KMU Build Configuration Register Number of Key Slots bit field offset
      */
#define KMU_KMUBC_NKS_MASK                   (0x7u<<KMU_KMUBC_NKS_OFF)
     /*!< KMU Build Configuration Register Number of Key Slots bit field mask */
#define KMU_KMUBC_NHWKSLTS_OFF               19u
     /*!< KMU Build Configuration Register Number of Hardware Key Slots bit
      *   field offset */
#define KMU_KMUBC_NHWKSLTS_MASK              (0xFu<<KMU_KMUBC_NHWKSLTS_OFF)
     /*!< KMU Build Configuration Register Number of Hardware Key Slots bit
      *   field mask */

#define KMU_KMISR_KEC_MASK                   (0x1u << 0u)
     /*!< KMU Interrupt Status Register Key Export Complete bit field mask */
#define KMU_KMISR_AWBE_MASK                  (0x1u << 1u)
     /*!< KMU Interrupt Status Register Activation While Busy Error bit field
      *   mask */
#define KMU_KMISR_IPE_MASK                   (0x1u << 2u)
     /*!< KMU Interrupt Status Register Internal Parity Error bit field mask */
#define KMU_KMISR_INPPE_MASK                 (0x1u << 3u)
     /*!< KMU Interrupt Status Register Input Parity Error bit field mask */
#define KMU_KMISR_WDADDKPA_MASK              (0x1u << 4u)
     /*!< KMU Interrupt Status Register Wrong Destination Address Detected
      *   bit field mask */
#define KMU_KMISR_WTE_MASK                   (0x1u << 5u)
     /*!< KMU Interrupt Status Register Write Transaction Error bit field mask
      */
#define KMU_KMISR_AEWNR_MASK                 (0x1u << 6u)
     /*!< KMU Interrupt Status Register Attempted to Export While Not Ready bit
      *   field mask */
#define KMU_KMISR_KSNL_MASK                  (0x1u << 7u)
     /*!< KMU Interrupt Status Register Key Slot Not Locked bit field mask */
#define KMU_KMISR_KSKRSM_MASK                (0x1u << 8u)
     /*!< KMU Interrupt Status Register Key Slot Register Size Mismatch bit
      *   field mask */
#define KMU_KMISR_KSDPANS_MASK               (0x1u << 9u)
     /*!< KMU Interrupt Status Register Key Slot Destination Port Register Not
      *   Set bit field mask */
#define KMU_KMISR_AIKSWE_MASK                (0x1u << 10u)
     /*!< KMU Interrupt Status Register Attempted to Invalidate Key Slot While
      *   Exporting bit field mask */
#define KMU_KMISR_MWKSW_MASK                 (0x1u << 11u)
     /*!< KMU Interrupt Status Register Multiple Writes to Key Slot Word bit
      *   field mask */
#define KMU_KMISR_AKSWPI_MASK                (0x1u << 12u)
     /*!< KMU Interrupt Status Register Attempt to Write to Key Slot Word
      *   Register when it is Permanently Invalidated bit field mask */
#define KMU_KMISR_AWBHKSKR_MASK              (0x1u << 13u)
     /*!< KMU Interrupt Status Register Attempt to Write from the Private APB HW
      *   Keys Port to Beyond the Hardware Key Slot Registers bit field mask */
#define KMU_KMISR_WDALSBDKPA_MASK            (0x1u << 14u)
     /*!< KMU Interrupt Status Register Wrong Destination Address LS Bits
      *   Detected bit field mask */

#define KMU_KMUKSC_DPWD_OFF                  0u
     /*!< KMU Key Slot Configuration Register Destination Port Write Delay bit
      *   field offset */
#define KMU_KMUKSC_DPWD_MASK                 (0xFFu<<KMU_KMUKSC_DPWD_OFF)
     /*!< KMU Key Slot Configuration Register Destination Port Write Delay bit
      *   field mask */
#define KMU_KMUKSC_DPAI_OFF                  8u
     /*!< KMU Key Slot Configuration Register Destination Port Address Incrememt
      *   bit field offset */
#define KMU_KMUKSC_DPAI_MASK                 (0xFFu<<KMU_KMUKSC_DPAI_OFF)
     /*!< KMU Key Slot Configuration Register Destination Port Address Incrememt
      *   bit field mask */
#define KMU_KMUKSC_DPDW_OFF                  16u
     /*!< KMU Key Slot Configuration Register Destination Port Data Width bit
      *   field offset */
#define KMU_KMUKSC_DPDW_MASK                 (0x3u<<KMU_KMUKSC_DPDW_OFF)
     /*!< KMU Key Slot Configuration Register Destination Port Data Width bit
      *   field mask */
#define KMU_KMUKSC_NDPW_OFF                  18u
     /*!< KMU Key Slot Configuration Register Num Destination Port Writes bit
      *   field offset */
#define KMU_KMUKSC_NDPW_MASK                 (0x3u<<KMU_KMUKSC_NDPW_OFF)
     /*!< KMU Key Slot Configuration Register Num Destination Port Writes bit
      *   field mask */
#define KMU_KMUKSC_NMNKW_OFF                 20u
     /*!< KMU Key Slot Configuration Register New Mask for Next Kew Writes bit
      *   field offset */
#define KMU_KMUKSC_NMNKW_MASK                (0x1u<<KMU_KMUKSC_NMNKW_OFF)
     /*!< KMU Key Slot Configuration Register New Mask for Next Kew Writes bit
      *   field mask */
#define KMU_KMUKSC_WMD_OFF                   21u
     /*!< KMU Key Slot Configuration Register Write Mask Disable bit field
      *   offset */
#define KMU_KMUKSC_WMD_MASK                  (0x1u<<KMU_KMUKSC_WMD_OFF)
     /*!< KMU Key Slot Configuration Register Write Mask Disable bit field
      *   mask */
#define KMU_KMUKSC_WMD_OFF                   21u
     /*!< KMU Key Slot Configuration Register Write Mask Disable bit field
      *   offset */
#define KMU_KMUKSC_WMD_MASK                  (0x1u<<KMU_KMUKSC_WMD_OFF)
     /*!< KMU Key Slot Configuration Register Write Mask Disable bit field
      *   mask */
#define KMU_KMUKSC_LKS_OFF                   22u
     /*!< KMU Key Slot Configuration Register Lock Key Slot bit field offset */
#define KMU_KMUKSC_LKS_MASK                  (0x1u<<KMU_KMUKSC_LKS_OFF)
     /*!< KMU Key Slot Configuration Register Lock Key Slot bit field mask */
#define KMU_KMUKSC_LKSKR_OFF                 23u
     /*!< KMU Key Slot Configuration Register Lock Key Slot Key Registers bit
      * field offset */
#define KMU_KMUKSC_LKSKR_MASK                (0x1u<<KMU_KMUKSC_LKSKR_OFF)
     /*!< KMU Key Slot Configuration Register Lock Key Slot Key Registers bit
      * field mask */
#define KMU_KMUKSC_VKS_OFF                   24u
     /*!< KMU Key Slot Configuration Register Verify Key Slot bit field offset
      */
#define KMU_KMUKSC_VKS_MASK                  (0x1u<<KMU_KMUKSC_VKS_OFF)
     /*!< KMU Key Slot Configuration Register Verify Key Slot bit field mask */
#define KMU_KMUKSC_KSR_OFF                   25u
     /*!< KMU Key Slot Configuration Register Key Slot Ready bit field offset */
#define KMU_KMUKSC_KSR_MASK                  (0x1u<<KMU_KMUKSC_KSR_OFF)
     /*!< KMU Key Slot Configuration Register Key Slot Ready bit field mask */
#define KMU_KMUKSC_IKS_OFF                   26u
     /*!< KMU Key Slot Configuration Register Invalidate Key Slot bit field
      *   offset */
#define KMU_KMUKSC_IKS_MASK                  (0x1u<<KMU_KMUKSC_IKS_OFF)
     /*!< KMU Key Slot Configuration Register Invalidate Key Slot bit field
      *   mask */
#define KMU_KMUKSC_KSIP_OFF                  27u
     /*!< KMU Key Slot Configuration Register Key Slot Invalidated Permanently
      *   bit field offset */
#define KMU_KMUKSC_KSIP_MASK                 (0x1u<<KMU_KMUKSC_KSIP_OFF)
     /*!< KMU Key Slot Configuration Register Key Slot Invalidated Permanently
      *   bit field mask */
#define KMU_KMUKSC_EK_OFF                    28u
     /*!< KMU Key Slot Configuration Register Export Key bit field offset */
#define KMU_KMUKSC_EK_MASK                   (0x1u<<KMU_KMUKSC_EK_OFF)
     /*!< KMU Key Slot Configuration Register Export Key bit field mask */
#define KMU_KMUKSC_L_KMUPRBGSI_OFF           31u
     /*!< KMU Key Slot Configuration Register Lock KMUPRBGSI bit field offset */
#define KMU_KMUKSC_L_KMUPRBGSI_MASK          (0x1u<<KMU_KMUKSC_L_KMUPRBGSI_OFF)
     /*!< KMU Key Slot Configuration Register Lock KMUPRBGSI bit field mask */

/**
 * \brief ARM KMU error enumeration types
 */
enum kmu_error_t {
    KMU_ERROR_NONE = (0x0u),
    KMU_ERROR_INVALID_ALIGNMENT,
    KMU_ERROR_INVALID_LENGTH,
    KMU_ERROR_INVALID_SLOT,
    KMU_ERROR_INVALID_EXPORT_ADDR,
    KMU_ERROR_SLOT_LOCKED,
    KMU_ERROR_SLOT_NOT_LOCKED,
    KMU_ERROR_SLOT_INVALIDATED,
    KMU_ERROR_SLOT_ALREADY_WRITTEN,
    KMU_ERROR_NOT_READY,
    KMU_ERROR_INTERNAL_ERROR,
};

enum kmu_hardware_keyslot_t {
    KMU_HW_SLOT_KRTL = (0u),
    KMU_HW_SLOT_HUK,
    KMU_HW_SLOT_GUK,
    KMU_HW_SLOT_KP_CM,
    KMU_HW_SLOT_KCE_CM,
    KMU_HW_SLOT_KP_DM,
    KMU_HW_SLOT_KCE_DM,
    KMU_HW_SLOT_RESERVED,
    KMU_USER_SLOT_MIN = (8u),
    KMU_USER_SLOT_MAX = (31u),
};

enum kmu_destination_port_data_width_t {
    KMU_DESTINATION_PORT_WIDTH_8_BITS = (0x0u),
    KMU_DESTINATION_PORT_WIDTH_16_BITS,
    KMU_DESTINATION_PORT_WIDTH_32_BITS,
};

enum kmu_destination_port_data_writes_t {
    KMU_DESTINATION_PORT_WIDTH_4_WRITES = (0x0u),
    KMU_DESTINATION_PORT_WIDTH_8_WRITES,
    KMU_DESTINATION_PORT_WIDTH_16_WRITES,
    KMU_DESTINATION_PORT_WIDTH_32_WRITES,
};

/**
 * \brief ARM KMU export policy configuration structure
 */
struct kmu_key_export_config_t {
    uint8_t destination_port_write_delay;
    uint8_t destination_port_address_increment;
    enum kmu_destination_port_data_width_t destination_port_data_width_code;
    enum kmu_destination_port_data_writes_t destination_port_data_writes_code;
    bool new_mask_for_next_key_writes;
    bool write_mask_disable;
};

/**
 * \brief ARM KMU device configuration structure
 */
struct kmu_dev_cfg_t {
    const uint32_t base;                         /*!< KMU base address */
};

/**
 * \brief ARM KMU device structure
 */
struct kmu_dev_t {
    const struct kmu_dev_cfg_t *const cfg;       /*!< KMU configuration */
};

enum kmu_error_t kmu_init(struct kmu_dev_t *dev, uint8_t *prbg_seed);

enum kmu_error_t kmu_key_get_export_config(struct kmu_dev_t *dev, uint32_t slot,
                                           struct kmu_key_export_config_t *config);
enum kmu_error_t kmu_key_set_export_config(struct kmu_dev_t *dev, uint32_t slot,
                                           struct kmu_key_export_config_t *config);

enum kmu_error_t kmu_set_key_locked(struct kmu_dev_t *dev, uint32_t slot);
enum kmu_error_t kmu_get_key_locked(struct kmu_dev_t *dev, uint32_t slot);

enum kmu_error_t kmu_set_key_export_config_locked(struct kmu_dev_t *dev,
                                                  uint32_t slot);
enum kmu_error_t kmu_get_key_export_config_locked(struct kmu_dev_t *dev,
                                                  uint32_t slot);

enum kmu_error_t kmu_set_slot_invalid(struct kmu_dev_t *dev, uint32_t slot);
enum kmu_error_t kmu_get_slot_invalid(struct kmu_dev_t *dev, uint32_t slot);

enum kmu_error_t kmu_set_key(struct kmu_dev_t *dev, uint32_t slot, uint8_t *key,
                             size_t key_len);
enum kmu_error_t kmu_get_key(struct kmu_dev_t *dev, uint32_t slot, uint8_t *buf,
                             size_t buf_len);

enum kmu_error_t kmu_reset_slot(struct kmu_dev_t *dev, uint32_t slot);

enum kmu_error_t kmu_export_key(struct kmu_dev_t *dev, uint32_t slot);

#ifdef __cplusplus
}
#endif

#endif /* __KMU_DRV_H__ */
