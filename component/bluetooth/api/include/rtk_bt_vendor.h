/**
 * @file      rtk_bt_vendor.h
 * @author
 * @brief     Bluetooth vendor function definition.
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_VENDOR_H__
#define __RTK_BT_VENDOR_H__

#include <rtk_bt_def.h>
#include <bt_api_config.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @def       VENDOR_EVT_FW_EXCEPTION
 * @brief     Vendor event code reported by the BT controller when a firmware exception occurs.
 */
#define VENDOR_EVT_FW_EXCEPTION  0x34

/**
 * @typedef   rtk_bt_vendor_adv_tx_power_type_t
 * @brief     Set ADV TX power type.
 */
typedef enum {
	ADV_TX_POW_SET_1M,            /*!< Set the advertising TX power for the LE 1M PHY. */
	ADV_TX_POW_SET_2M,            /*!< Set the advertising TX power for the LE 2M secondary advertising PHY. */
	ADV_TX_POW_SET_1M_DEFAULT,    /*!< Set the advertising TX power for the LE 1M PHY to default value. */
	ADV_TX_POW_SET_2M_DEFAULT,    /*!< Set the advertising TX power for the LE 2M secondary advertising PHY to default value. */
} rtk_bt_vendor_adv_tx_power_type_t;

/**
 * @typedef   rtk_bt_vendor_conn_tx_power_is_reset_t
 * @brief     Set connect TX power type.
 */
typedef enum {
	CONN_TX_POW_USER_MODE,            /*!< Use the TX power set by the user. */
	CONN_TX_POW_RESET_TO_ORIGINAL,    /*!< Reset the TX power to the original value. */
} rtk_bt_vendor_conn_tx_power_is_reset_t;

/**
 * @typedef   rtk_bt_vendor_tx_power_param_t
 * @brief     Set ADV or connect TX power parameter.
 */
typedef struct {
	uint8_t tx_power_type;                                      /*!< Whether to set the advertising TX power or the connection TX power.
                                                                 *   0: Set the advertising TX power, adv_tx_power is used.
                                                                 *   1: Set the connection TX power, conn_tx_power is used. */
	union {
		struct adv_tx_power_param {
			rtk_bt_vendor_adv_tx_power_type_t type;             /*!< Advertising TX power type. */
		} adv_tx_power;                                         /*!< Advertising TX power parameter, used when tx_power_type is 0. */

		struct conn_tx_power_param {
			uint16_t conn_handle;                               /*!< Connection handle of the ACL link to set the TX power. */
			rtk_bt_vendor_conn_tx_power_is_reset_t is_reset;    /*!< Whether to use the user TX power or reset it to the original value. */
		} conn_tx_power;                                        /*!< Connection TX power parameter, used when tx_power_type is 1. */
	};
	uint8_t tx_gain;                                            /*!< TX power gain index. */
} rtk_bt_vendor_tx_power_param_t;

/**
 * @typedef   rtk_bt_vendor_tx_power_subcmd_type_t
 * @brief     Set TX power subcommand type.
 */
typedef enum {
	SUB_CMD_SET_ADV_TX_POWER  = 0x00,    /*!< Set the advertising TX power. */
	SUB_CMD_SET_CONN_TX_POWER = 0x0c,    /*!< Set the connection TX power. */
} rtk_bt_vendor_tx_power_subcmd_type_t;

/**
 * @struct    rtk_bt_vendor_free_run_clock_t
 * @brief     Free run clock samples of the host and the controller, used by @ref rtk_bt_get_hc_clock_offset.
 */
typedef struct {
	uint64_t host_free_run_clock[3];          /*!< Free run clock samples of the host. */
	uint64_t controller_free_run_clock[3];    /*!< Free run clock samples of the controller. */
	int64_t hc_clock_offset[3];               /*!< Clock offsets between the host and the controller, calculated from each pair of samples. */
	void *sem;                                /*!< Semaphore to wait for the samples to be collected. */
	uint8_t index;                            /*!< Index of the sample being collected. */
	bool enable;                              /*!< Whether the samples are being collected.
                                               *   - true: collecting.
                                               *   - false: not collecting. */
} rtk_bt_vendor_free_run_clock_t;

/**
 * @struct    rtk_bt_le_iso_sync_ref_ap_info_t
 * @brief     Reference anchor point information of an ISO channel, used by @ref rtk_bt_get_le_iso_sync_ref_ap_info.
 */
typedef struct {
	uint32_t Valid: 1;                  /*!< bit[0]: whether the information is valid.
                                         *   1: Valid.
                                         *   0: Invalid. */
	uint32_t Type: 1;                   /*!< bit[1]: link type.
                                         *   1: ISO.
                                         *   0: ACL.
                                         *   Always 1. */
	uint32_t Role: 1;                   /*!< bit[2]: role of the local device.
                                         *   1: central.
                                         *   0: peripheral. */
	uint32_t Dir: 1;                    /*!< bit[3]: direction of the ISO path.
                                         *   1: TX.
                                         *   0: RX. */
	uint32_t Cis: 1;                    /*!< bit[4]: iSO channel type.
                                         *   1: CIS.
                                         *   0: BIS. */
	uint32_t Rsvd0: 3;                  /*!< bit[7:5]: reserved. */
	uint32_t Status: 8;                 /*!< bit[15:8]: status. */
	uint32_t Rsvd1: 16;                 /*!< bit[31:16]: reserved. */
	uint16_t Conn_Handle;               /*!< Connection handle of the ISO channel (CIS or BIS). */
	uint16_t Sdu_Seq_Num;               /*!< SDU sequence number. */
	uint32_t Sdu_Interval;              /*!< SDU interval.
                                         *   Units: microseconds. */
	uint32_t Cur_Sync_Ref_Point;        /*!< Current SDU synchronization reference point.
                                         *   Units: microseconds. */
	uint32_t Pre_Sync_Ref_Point;        /*!< Previous SDU synchronization reference point.
                                         *   Units: microseconds. */
	uint32_t Accumulate_Sw_Timer;       /*!< Accumulated software timer.
                                         *   Units: microseconds. */
	uint32_t Iso_Interval_us;           /*!< ISO interval.
                                         *   Units: microseconds. */
	uint32_t Group_Anchor_Point;        /*!< CIG or BIG anchor point. */
	uint32_t Cur_SDU_Sync_Ref_Point;    /*!< Current SDU synchronization reference point.
                                         *   Units: microseconds. */
} rtk_bt_le_iso_sync_ref_ap_info_t;

/**
 * @struct    rtk_bt_vendor_sync_ref_ap_info_t
 * @brief     Context used to wait for the ISO reference anchor point information reported by the controller.
 */
typedef struct {
	rtk_bt_le_iso_sync_ref_ap_info_t info;    /*!< Reference anchor point information reported by the controller. */
	void *sem;                                /*!< Semaphore to wait for the information. */
	bool enable;                              /*!< Whether the information is being waited for.
                                               *   - true: Waiting.
                                               *   - false: Not waiting. */
} rtk_bt_vendor_sync_ref_ap_info_t;

/**
 * @typedef   rtk_bt_le_clock_status_t
 * @brief     Status of getting the clock information of a peripheral connection link.
 */
typedef enum {
	RTK_BT_LE_CLOCK_STATUS_SUCCESS = 0,          /*!< The clock information is obtained successfully. */
	RTK_BT_LE_CLOCK_STATUS_CONN_HANDLE_ERROR,    /*!< The input connection handle is invalid. */
	RTK_BT_LE_CLOCK_STATUS_GET_FAIL,             /*!< Failed to get the clock information, because the peripheral did not receive the connection packet. */
} rtk_bt_le_clock_status_t;

/**
 * @struct    rtk_bt_le_conn_clock_info_t
 * @brief     Clock information of a peripheral connection link.
 */
typedef struct {
	uint64_t clock_counter;    /*!< Clock counter of the controller.
                                *   Wraps around from 83886079999 to 0. */
	uint16_t conn_handle;      /*!< Connection handle of the ACL link. */
	uint16_t event_counter;    /*!< Connection event counter.
                                *   Wraps around from 65535 to 0. */
	uint16_t ce_interval;      /*!< Connection interval used on this connection.
                                *   Range: 0x0006 to 0x0C80.
                                *   Time = N * 1.25 ms.
                                *   Time Range: 7.5 ms to 4000 ms. */
	uint8_t  status;           /*!< Status of getting the clock information, see @ref rtk_bt_le_clock_status_t. */
} rtk_bt_le_conn_clock_info_t;

/**
 * @struct    rtk_bt_vendor_conn_clock_info_t
 * @brief     Connection clock information.
 */
typedef struct {
	rtk_bt_le_conn_clock_info_t info;    /*!< Clock information reported by the controller. */
	void *sem;                           /*!< Semaphore to wait for the information. */
} rtk_bt_vendor_conn_clock_info_t;

/**
 * @typedef   rtk_bt_vendor_get_slave_conn_clock_subcmd_type_t
 * @brief     Subcommand type of the clock information of a peripheral connection link.
 */
typedef enum {
	SUB_CMD_READ_ACC_HIT_COUNTER    = 0x14,    /*!< Read the acc hit counter. */
} rtk_bt_vendor_get_slave_conn_clock_subcmd_type_t;

/**
 * @typedef   rtk_bt_vendor_free_run_clock_latch_subcmd_type_t
 * @brief     Free run clock latch subcommand type.
 */
typedef enum {
	SUB_CMD_FREE_RUN_CLOCK_LATCH_ENABLE = 0x00,    /*!< Enable or disable the free run clock latch. */
	SUB_CMD_FREE_RUN_CLOCK_LATCH        = 0x01,    /*!< Latch the free run clock. */
} rtk_bt_vendor_free_run_clock_latch_subcmd_type_t;

/**
 * @defgroup  bt_vendor BT Vendor APIs
 * @brief     BT vendor function APIs.
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     Power on the BT controller. This configures the external firmware log pin and resets the controller.
 */
void rtk_bt_controller_power_on(void);

/**
 * @brief     Power off the BT controller.
 */
void rtk_bt_controller_power_off(void);

/**
 * @brief     Set the TX power gain index of the BT controller.
 * @param[in] index: TX power gain index.
 */
void rtk_bt_set_bt_tx_power_gain_index(uint32_t index);

/**
 * @brief     Set the BT antenna.
 * @param[in] ant: BT antenna.
 *                 0: ANT_S0.
 *                 1: ANT_S1.
 * @note
 *            - This API is only usable on the AmebaSmart platform.
 */
void rtk_bt_set_bt_antenna(uint8_t ant);

/**
 * @brief     Enable the HCI debug.
 * @note
 *            - The platform hook used by this API is a weak function. If the platform does not implement it, an error log is printed and nothing is done.
 */
void rtk_bt_hci_debug_enable(void);

/**
 * @brief     Enable the BT Debug Ports selected by the mask.
 * @param[in] bt_sel: BT Debug Port selection.
 *                    0: BT vendor.
 *                    1: BT on.
 * @param[in] bt_bdg_mask: BT Debug Port mask.
 *                         Each bit set to 1 enables the BT Debug Port of the same index, from 0 to 31.
 * @note
 *            - The platform hook used by this API is a weak function. If the platform does not implement it, an error log is printed and nothing is done.
 */
void rtk_bt_debug_port_mask_enable(uint8_t bt_sel, uint32_t bt_bdg_mask);

/**
 * @brief     Enable a BT Debug Port and output it to a pad.
 * @param[in] bt_sel: BT Debug Port selection.
 *                    0: BT vendor.
 *                    1: BT on.
 * @param[in] bt_dbg_port: BT Debug Port index.
 *                         Range: 0 to 31.
 * @param[in] pad: Name of the pad that outputs the BT Debug Port.
 *                 Must not be NULL.
 * @note
 *            - The platform hook used by this API is a weak function. If the platform does not implement it, an error log is printed and nothing is done.
 */
void rtk_bt_debug_port_pad_enable(uint8_t bt_sel, uint8_t bt_dbg_port, char *pad);

/**
 * @brief     Shift a BT Debug Port to another BT Debug Port.
 * @param[in] original: BT Debug Port to shift from.
 * @param[in] mapping: BT Debug Port to shift to.
 * @note
 *            - The platform hook used by this API is a weak function. If the platform does not implement it, an error log is printed and nothing is done.
 */
void rtk_bt_debug_port_shift(uint8_t original, uint8_t mapping);

/**
 * @brief     Enable a BT GPIO and output it to a pad.
 * @param[in] bt_gpio: BT GPIO name.
 * @param[in] pad: Name of the pad that outputs the BT GPIO.
 *                 Must not be NULL.
 * @note
 *            - The platform hook used by this API is a weak function. If the platform does not implement it, an error log is printed and nothing is done.
 */
void rtk_bt_gpio_enable(uint8_t bt_gpio, char *pad);

/**
 * @brief     Set the advertising TX power or the connection TX power of the BT controller by a vendor command.
 * @param[in] tx_power: TX power parameters.
 *                      Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - If VENDOR_CMD_SET_TX_POWER_SUPPORT is not enabled, this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_set_tx_power(rtk_bt_vendor_tx_power_param_t *tx_power);

#if defined(RTK_BT_HC_CLOCK_OFFSET_SUPPORT) && RTK_BT_HC_CLOCK_OFFSET_SUPPORT

/**
 * @brief     Get the free run clock samples context shared with the stack.
 * @return
 *            - Pointer to the free run clock context.
 */
rtk_bt_vendor_free_run_clock_t *rtk_bt_get_hc_free_run_clock(void);

/**
 * @brief     Get the free run clock offset between the BT host and the BT controller.
 * @param[out] offset: Free run clock offset, host clock minus controller clock.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall NOT be called in the BT API task.
 *            - If vendor command VENDOR_CMD_FREE_RUN_CLOCK_LATCH_SUPPORT is not enabled, this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_get_hc_clock_offset(int64_t *offset);
#endif

#if defined(RTK_BT_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT) && RTK_BT_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT

/**
 * @brief     Get the ISO reference anchor point context shared with the stack.
 * @return
 *            - Pointer to the ISO reference anchor point context.
 */
rtk_bt_vendor_sync_ref_ap_info_t *rtk_bt_get_ref_ap_info(void);

/**
 * @brief     Get the CIG or BIG reference anchor point from the BT controller.
 * @param[in]  conn_handle: Connection handle of the ISO channel (CIS or BIS).
 * @param[in]  dir: Direction of the ISO path.
 * @param[out] p_info: Reference anchor point information.
 *                     Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall NOT be called in the BT API task.
 *            - If vendor command VENDOR_CMD_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT is not enabled, this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_get_le_iso_sync_ref_ap_info(uint16_t conn_handle, uint8_t dir, rtk_bt_le_iso_sync_ref_ap_info_t *p_info);
#endif

#if defined(RTK_BLE_GET_SLAVE_CONN_CLOCK_SUPPORT) && RTK_BLE_GET_SLAVE_CONN_CLOCK_SUPPORT

/**
 * @brief     Get the connection clock information of the ACL link with the peripheral.
 * @return
 *            - Pointer to the connection clock information of the ACL link with the peripheral.
 */
rtk_bt_vendor_conn_clock_info_t *rtk_bt_get_slave_clock_info(void);

/**
 * @brief     Get the clock information of a connection link where the local device is the peripheral.
 * @param[in]  conn_handle: Connection handle of this ACL link.
 * @param[out] clock_info: Clock information of the ACL link.
 *                         Must not be NULL.
 * @return
 *            - @ref rtk_bt_err_common
 * @note
 *            - This API shall NOT be called in the BT API task.
 *            - If vendor command VENDOR_CMD_READ_ACC_HIT_COUNTER_SUPPORT is not enabled, this API fails with @ref RTK_BT_FAIL.
 */
uint16_t rtk_bt_le_get_slave_conn_clock(uint16_t conn_handle, rtk_bt_le_conn_clock_info_t *clock_info);
#endif
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_VENDOR_H__ */
