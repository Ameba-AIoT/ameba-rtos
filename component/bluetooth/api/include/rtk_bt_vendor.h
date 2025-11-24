/**
 * @file      rtk_bt_vendor.h
 * @author
 * @brief     Bluetooth Common function definition
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
 * @typedef   rtk_bt_vendor_adv_tx_power_type_t
 * @brief     Set ADV TX power type.
 */
typedef enum {
	ADV_TX_POW_SET_1M,
	ADV_TX_POW_SET_2M,
	ADV_TX_POW_SET_1M_DEFAULT,
	ADV_TX_POW_SET_2M_DEFAULT,
} rtk_bt_vendor_adv_tx_power_type_t;

/**
 * @typedef   rtk_bt_vendor_conn_tx_power_is_reset_t
 * @brief     Set connect TX power type.
 */
typedef enum {
	CONN_TX_POW_USER_MODE,
	CONN_TX_POW_RESET_TO_ORIGINAL,
} rtk_bt_vendor_conn_tx_power_is_reset_t;

/**
 * @typedef   rtk_bt_vendor_tx_power_param_t
 * @brief     Set ADV or connect TX power parameter.
 */
typedef struct {
	uint8_t tx_power_type;                  /*!< 0: Set ADV TX power, 1: Set connect TX power */
	union {
		struct adv_tx_power_param {
			rtk_bt_vendor_adv_tx_power_type_t type;                   /*!< ADV TX power type */
		} adv_tx_power;

		struct conn_tx_power_param {
			uint16_t conn_handle;           /*!< Connect handle */
			rtk_bt_vendor_conn_tx_power_is_reset_t is_reset;          /*!< 0: User mode, 1: Reset to original */
		} conn_tx_power;
	};
	uint8_t tx_gain;                        /*!< Ref gain index */
} rtk_bt_vendor_tx_power_param_t;

/**
 * @typedef   rtk_bt_vendor_tx_power_subcmd_type_t
 * @brief     Set TX power subcmd type.
 */
typedef enum {
	SUB_CMD_SET_ADV_TX_POWER  = 0x00,
	SUB_CMD_SET_CONN_TX_POWER = 0x0c,
} rtk_bt_vendor_tx_power_subcmd_type_t;

typedef struct {
	uint64_t host_free_run_clock[3];
	uint64_t controller_free_run_clock[3];
	int64_t hc_clock_offset[3];
	void *sem;
	uint8_t index;
	bool enable;
} rtk_bt_vendor_free_run_clock_t;

typedef struct {
	uint32_t Valid: 1;    /* bit[0]: 1 is valid and 0 is invalid */
	uint32_t Type: 1;     /* bit[1]: 1 is the ISO and 0 is the ACL (always 1) */
	uint32_t Role: 1;     /* bit[2]: 1 is the master and 0 is the slave */
	uint32_t Dir: 1;      /* bit[3]: 1 is Tx, 0 is Rx */
	uint32_t Cis: 1;      /* bit[4]: 1 is the CIS, 0 is the BIS */
	uint32_t Rsvd0: 3;    /* bit[7:5]: reserved */
	uint32_t Status: 8;   /* bit[15:8]: status */
	uint32_t Rsvd1: 16;   /* bit[31:16]: reserved */
	uint16_t Conn_Handle;         /* iso (cis or bis) connection handle */
	uint16_t Sdu_Seq_Num;         /* sdu sequence number */
	uint32_t Sdu_Interval;        /* sdu interval (us) */
	uint32_t Cur_Sync_Ref_Point;  /* current sdu sync ref point (us) */
	uint32_t Pre_Sync_Ref_Point;  /* previous sdu sync ref point (us) */
	uint32_t Accumulate_Sw_Timer; /* accumulate sw timer (us) */
	uint32_t Iso_Interval_us;     /* iso interval (us)*/
	uint32_t Group_Anchor_Point;  /* cig or big anchor point */
	uint32_t Cur_SDU_Sync_Ref_Point; /* current sdu sync ref point (us) */
} rtk_bt_le_iso_sync_ref_ap_info_t;

typedef struct {
	rtk_bt_le_iso_sync_ref_ap_info_t info;
	void *sem;
	bool enable;
} rtk_bt_vendor_sync_ref_ap_info_t;

/**
 * @typedef   rtk_bt_vendor_free_run_clock_latch_subcmd_type_t
 * @brief     Free run clock latch subcmd type.
 */
typedef enum {
	SUB_CMD_FREE_RUN_CLOCK_LATCH_ENABLE = 0x00,
	SUB_CMD_FREE_RUN_CLOCK_LATCH        = 0x01,
} rtk_bt_vendor_free_run_clock_latch_subcmd_type_t;

/**
 * @defgroup  bt_vendor BT Vendor APIs
 * @brief     BT vendor function APIs
 * @ingroup   BT_APIs
 * @{
 */

/**
 * @brief     BT controller power on.
 * @return    None
 */
void rtk_bt_controller_power_on(void);

/**
 * @brief     BT controller power off.
 * @return    None
 */
void rtk_bt_controller_power_off(void);

/**
 * @brief     Set BT TX power gain index.
 * @param[in] index: TX power gain index.
 * @return    None
 */
void rtk_bt_set_bt_tx_power_gain_index(uint32_t index);

/**
 * @brief     Set BT antenna. This API is only useable for AmebaSmart Platform.
 * @param[in] ant: BT antenna, 0 for ANT_S0, 1 for ANT_S1.
 * @return    None
 */
void rtk_bt_set_bt_antenna(uint8_t ant);

/**
 * @brief     Enable HCI debug.
 * @return    None
 */
void rtk_bt_hci_debug_enable(void);

/**
 * @brief     Enable BT Debug Port.
 * @param[in] bt_sel: BT Debug Port selection, 0 for BT vendor, 1 for BT on.
 * @param[in] bt_bdg_mask: BT debug mask, each bit set 1 means enable BT Debug Port[0-31].
 * @return    None
 */
void rtk_bt_debug_port_mask_enable(uint8_t bt_sel, uint32_t bt_bdg_mask);

/**
 * @brief     Enable BT Debug Port.
 * @param[in] bt_sel: BT Debug Port selection, 0 for BT vendor, 1 for BT on.
 * @param[in] bt_dbg_port: BT debug port bit, range 0~31.
 * @param[in] pad: PAD name.
 * @return    None
 */
void rtk_bt_debug_port_pad_enable(uint8_t bt_sel, uint8_t bt_dbg_port, char *pad);

/**
 * @brief     Shift BT Debug Port.
 * @param[in] original: Original BT Debug Port which is shift from.
 * @param[in] mapping: Target BT Debug Port mapping which is shift to.
 * @return    None
 */
void rtk_bt_debug_port_shift(uint8_t original, uint8_t mapping);

/**
 * @brief     Enable BT GPIO.
 * @param[in] bt_gpio: BT GPIO name.
 * @param[in] pad: PAD name.
 * @return    None
 */
void rtk_bt_gpio_enable(uint8_t bt_gpio, char *pad);

/**
 * @brief     Set BT TX power.
 * @param[in] tx_power: Pointer to TX power parameter.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_set_tx_power(rtk_bt_vendor_tx_power_param_t *tx_power);

#if defined(RTK_BT_HC_CLOCK_OFFSET_SUPPORT) && RTK_BT_HC_CLOCK_OFFSET_SUPPORT

rtk_bt_vendor_free_run_clock_t *rtk_bt_get_hc_free_run_clock(void);

/**
 * @brief      Get the free run clock offset between BT host & controller.
 * @param[out] offset: The free run clock offset.
 * @note       This API shall NOT be called in bt_api_task.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_get_hc_clock_offset(int64_t *offset);
#endif

#if defined(RTK_BT_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT) && RTK_BT_GET_LE_ISO_SYNC_REF_AP_INFO_SUPPORT

rtk_bt_vendor_sync_ref_ap_info_t *rtk_bt_get_ref_ap_info(void);

/**
 * @brief      Get the CIG/BIG reference anchor point of controller.
 * @param[in] conn_handle: connection handle
 * @param[in] dir: The ISO path direction
 * @param[out] p_info: The information include CIG/BIG referece anchor point
 * @note       This API shall NOT be called in bt_api_task.
 * @return
 *            - 0  : Succeed
 *            - Others: Error code
 */
uint16_t rtk_bt_get_le_iso_sync_ref_ap_info(uint16_t conn_handle, uint8_t dir, rtk_bt_le_iso_sync_ref_ap_info_t *p_info);
#endif
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __RTK_BT_VENDOR_H__ */
