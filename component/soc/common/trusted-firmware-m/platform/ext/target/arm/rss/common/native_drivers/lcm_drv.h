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
 * \file lcm_drv.h
 * \brief Driver for Arm LCM.
 */

#ifndef __LCM_DRV_H__
#define __LCM_DRV_H__

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LCM_DCU_WIDTH_IN_BYTES (16)

struct lcm_otp_layout_t {
    volatile uint32_t huk[8];
    volatile uint32_t guk[8];
    volatile uint32_t kp_cm[8];
    volatile uint32_t kce_cm[8];
    volatile uint32_t kp_dm[8];
    volatile uint32_t kce_dm[8];
    volatile uint32_t rotpk[8];
    volatile uint32_t tp_mode_config;
    volatile uint32_t cm_config_1;
    volatile uint32_t cm_config_2;
    volatile uint32_t dm_config;
    volatile uint32_t cm_rma_flag;
    volatile uint32_t dm_rma_flag;
    volatile uint32_t user_data[];
};

/**
 * \brief ARM LCM LCS enumeration types
 *
 * \note These high hamming-weight constants are required by the hardware.
 */
enum lcm_lcs_t {
    LCM_LCS_CM      = (0xCCCC3C3Cu),
    LCM_LCS_DM      = (0xDDDD5A5Au),
    LCM_LCS_SE      = (0xEEEEA5A5u),
    LCM_LCS_RMA     = (0xFFFFC3C3u),
    LCM_LCS_INVALID = (0xDEADBEEFu),
};

/**
 * \brief ARM LCM TP mode enumeration types
 *
 * \note These high hamming-weight constants are required by the hardware.
 */
enum lcm_tp_mode_t {
    LCM_TP_MODE_VIRGIN  = (0x000033CCu),
    LCM_TP_MODE_TCI     = (0x111155AAu),
    LCM_TP_MODE_PCI     = (0x2222AA55u),
    LCM_TP_MODE_INVALID = (0xDEADBEEFu),
};

/**
 * \brief ARM LCM SP mode enumeration types
 *
 * \note These high hamming-weight constants are required by the hardware.
 */
enum lcm_bool_t {
    LCM_FALSE = (0x00000000u),
    LCM_TRUE  = (0xFFFFFFFFu),
};

/**
 * \brief ARM LCM error enumeration types
 */
enum lcm_error_t {
    LCM_ERROR_NONE = (0x0u),
    LCM_ERROR_INVALID_KEY,
    LCM_ERROR_INVALID_ALIGNMENT,
    LCM_ERROR_INVALID_LENGTH,
    LCM_ERROR_INVALID_OFFSET,
    LCM_ERROR_INVALID_WRITE,
    LCM_ERROR_INVALID_TRANSITION,
    LCM_ERROR_INVALID_LCS,
    LCM_ERROR_INVALID_TP_MODE,
    LCM_ERROR_INVALID_POINTER,
    LCM_ERROR_INTERNAL_ERROR,
    LCM_ERROR_WRITE_VERIFY_FAIL,
    LCM_ERROR_FATAL_ERR,
};

/**
 * \brief ARM LCM device configuration structure
 */
struct lcm_dev_cfg_t {
    const uint32_t base;                         /*!< LCM base address */
};

/**
 * \brief ARM LCM device structure
 */
struct lcm_dev_t {
    const struct lcm_dev_cfg_t* const cfg;       /*!< LCM configuration */
};


/**
 * \brief This function initializes the LCM
 *
 * \param[in] dev    The LCM device structure.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_init(struct lcm_dev_t *dev);

/**
 * \brief This function gets the TP mode
 *
 * \param[in]  dev     The LCM device structure.
 * \param[out] mode    The TP mode the device is currently in.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_get_tp_mode(struct lcm_dev_t *dev, enum lcm_tp_mode_t *mode);
/**
 * \brief This function sets the TP mode
 *
 * \param[in] dev     The LCM device structure.
 * \param[in] mode    The TP mode to transition the device to.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_set_tp_mode(struct lcm_dev_t *dev, enum lcm_tp_mode_t mode);

/**
 * \brief This function checks if secure provisioning mode is enabled
 *
 * \param[in]  dev         The LCM device structure.
 * \param[out] enabled     Whether secure provisioning mode is enabled.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_get_sp_enabled(struct lcm_dev_t *dev, enum lcm_bool_t *enabled);
/**
 * \brief This function enables secure provisioning mode.
 *
 * \param[in] dev    The LCM device structure.
 *
 * \note This function may cause some or all device components to be reset, and
 *       may begin executing code from the reset vector.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_set_sp_enabled(struct lcm_dev_t *dev);

/**
 * \brief This function checks if fatal error mode is enabled
 *
 * \param[in]  dev     The LCM device structure.
 * \param[out] error   Whether fatal error mode is enabled.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_get_fatal_error(struct lcm_dev_t *dev, enum lcm_bool_t *error);
/**
 * \brief This function enables fatal error mode.
 *
 * \param[in] dev   The LCM device structure.
 *
 * \note This function will cause the LCM to be inoperable until the device is
 *       reset.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_set_fatal_error(struct lcm_dev_t *dev);

/**
 * \brief This function gets the General purpose persistent configuration.
 *
 * \param[in]  dev     The LCM device structure.
 * \param[out] gppc    The value of the gppc.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_get_gppc(struct lcm_dev_t *dev, uint32_t *gppc);

/**
 * \brief This function gets the size of the OTP managed by the LCM.
 *
 * \param[in]  dev     The LCM device structure.
 * \param[out] size    The size (in bytes) of the OTP.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_get_otp_size(struct lcm_dev_t *dev, uint32_t *size);

/**
 * \brief This function gets Lifecycle State the LCM is currently in.
 *
 * \param[in]  dev    The LCM device structure.
 * \param[out] lcs    The LCS the device is currently in.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_get_lcs(struct lcm_dev_t *dev, enum lcm_lcs_t *lcs);
/**
 * \brief This function gets Lifecycle State the LCM is currently in.
 *
 * \param[in] dev      The LCM device structure.
 * \param[in] lcs      The LCS to transition to.
 * \param[in] gppc_val The value that the General Purpose Persistent
 *                     Configuration should be set to. Mandatory when lcs
 *                     parameter == LCM_LCS_DM, unused otherwise.
 *
 * \note Some LCS transitions are illegal
 * \note A cold system reset is required after this function returns for the new
 *       LCS to be entered.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_set_lcs(struct lcm_dev_t *dev, enum lcm_lcs_t lcs,
                             uint16_t gppc_val);

/**
 * \brief This function writes the OTP managed by the LCM.
 *
 * \param[in] dev       The LCM device structure.
 * \param[in] offset    The offset into the OTP to write. Must be 4 byte aligned
 * \param[in] len       The length of the OTP region to write. Must be 4 byte
 *                      aligned.
 * \param[in] buf       The buffer containing the data to write into OTP. Must
 *                      be 4 byte aligned and of a multiple of 4 bytes in length
 *
 * \note This function does not allow writes into OTP words that have already
 *       been written.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_otp_write(struct lcm_dev_t *dev, uint32_t offset, uint32_t len,
                               const uint8_t *buf);

/**
 * \brief This function reads the OTP managed by the LCM.
 *
 * \param[in] dev       The LCM device structure.
 * \param[in] offset    The offset into the OTP to write. Must be 4 byte aligned
 * \param[in] len       The length of the OTP region to write. Must be 4 byte
 *                      aligned.
 * \param[out] buf      The buffer that will be filled with the OTP content.
 *                      Must be 4 byte aligned and of a multiple of 4 bytes in
 *                      length.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_otp_read(struct lcm_dev_t *dev, uint32_t offset, uint32_t len,
                              uint8_t *buf);

/**
 * \brief This function gets the state of the Debug Control Unit.
 *
 * \param[in]  dev    The LCM device structure.
 * \param[out] val    The buffer into which to write the value of the DCU. Must
 *                    be LCM_DCU_WIDTH_IN_BYTES in size, and 4 byte aligned.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_dcu_get_enabled(struct lcm_dev_t *dev, uint8_t *val);
/**
 * \brief This function sets the state of the Debug Control Unit.
 *
 * \param[in]  dev    The LCM device structure.
 * \param[in]  val    The buffer whose value the DCU will be set to. Must
 *                    be LCM_DCU_WIDTH_IN_BYTES in size, and 4 byte aligned.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_dcu_set_enabled(struct lcm_dev_t *dev, uint8_t *val);

/**
 * \brief This function gets the state of the Debug Control Unit Lock.
 *
 * \param[in]  dev    The LCM device structure.
 * \param[out] val    The buffer into which to write the value of the DCU Lock.
 *                    Must be LCM_DCU_WIDTH_IN_BYTES in size, and 4 byte aligned
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_dcu_get_locked(struct lcm_dev_t *dev, uint8_t *val);
/**
 * \brief This function sets the state of the Debug Control Unit Lock.
 *
 * \param[in]  dev    The LCM device structure.
 * \param[in]  val    The buffer whose value the DCU lock will be set to. Must
 *                    be LCM_DCU_WIDTH_IN_BYTES in size, and 4 byte aligned.
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_dcu_set_locked(struct lcm_dev_t *dev, uint8_t *val);

/**
 * \brief This function gets the state of the Debug Control Unit Secure
 *        Provisioning Disable Mask. This mask controls which DCU bits cannot be
 *        enabled in secure provisioning mode. This mask is set in hardware.
 *
 * \param[in]  dev    The LCM device structure.
 * \param[out] val    The buffer into which to write the value of the DCU Lock.
 *                    Must be LCM_DCU_WIDTH_IN_BYTES in size, and 4 byte aligned
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_dcu_get_sp_disable_mask(struct lcm_dev_t *dev, uint8_t *val);
/**
 * \brief This function gets the state of the Debug Control Unit Disable Mask.
 *        This mask controls which DCU bits cannot be enabled. This mask is set
 *        in hardware
 *
 * \param[in]  dev    The LCM device structure.
 * \param[out] val    The buffer into which to write the value of the DCU Lock.
 *                    Must be LCM_DCU_WIDTH_IN_BYTES in size, and 4 byte aligned
 *
 * \return Returns error code as specified in \ref lcm_error_t
 */
enum lcm_error_t lcm_dcu_get_disable_mask(struct lcm_dev_t *dev, uint8_t *val);

#ifdef __cplusplus
}
#endif
#endif /* __LCM_DRV_H__ */
