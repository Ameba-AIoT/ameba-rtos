/*
*******************************************************************************
* Copyright(c) 2023, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef __THROUGHPUT_INCLUDE_H__
#define __THROUGHPUT_INCLUDE_H__


#ifdef __cplusplus
extern "C" {
#endif

#include <rtk_bt_common.h>

/************************Common for Throughput Server & Client************************/

/* Define the first bytes quantity of data sequence */
#define THROUGHPUT_CMP_DATA_BYTES    4
/* Define the default print period, in sec (s) */
#define THROUGHPUT_PRINT_PERIOD      5
/* Define the default maximum data length */
#define THROUGHPUT_DATA_LEN_MAX      244
/* Define the default maximum throughput duration, in sec (s) */
#define THROUGHPUT_TIME_MAX          0xfffffffful
/* Define the default supevision timeout */
#define THROUGHPUT_SUPV_TIMEOUT      1000

/* Define the stack size of TX task */
#define THROUGHPUT_TASK_STACK_SIZE   256 * 4
/* Define the priority of TX task */
#define THROUGHPUT_TASK_PRIORITY     2

/* Define the default time interval, 1000ms */
#define THROUGHPUT_TIME_INTERVAL     1000
/* Define the throughput client whether show discovery detail*/
#define THROUGHPUT_CLIENT_SHOW_DETAIL         1

/**
 * @def       THROUGHPUT_UUID_SRV
 * @brief     Bluetooth LE Throughput service & characteristic UUID.
 */
#define THROUGHPUT_UUID_SRV                     0xA00D
#define THROUGHPUT_UUID_WRITE_REQ_PARAM         0xC001
#define THROUGHPUT_UUID_WRITE_REQ               0xC002
#define THROUGHPUT_UUID_WRITE_CMD               0xC003
#define THROUGHPUT_UUID_NOTIFY                  0xC004
#define THROUGHPUT_UUID_INDICATE                0xC005
#define THROUGHPUT_UUID_WRITE_CMD_TEST          0xC006

/* Define the LE PHY preference */
#define THROUGHPUT_PHYS_PREFER_1M_BIT           0x01 /*!< Prefer to use the LE 1M transmitter PHY (possibly among others)  */
#define THROUGHPUT_PHYS_PREFER_2M_BIT           0x02 /*!< Prefer to use the LE 2M transmitter PHY (possibly among others)  */
#define THROUGHPUT_PHYS_PREFER_CODED_BIT        0x04 /*!< Prefer to use the LE Coded transmitter PHY (possibly among others)  */
/* Define the LE PHY preference for TX or RX */
#define THROUGHPUT_PHYS_PREFER_ALL              0x00
#define THROUGHPUT_PHYS_NO_PREFER_TX_BIT        0x01 /*!< Has no preference among the transmitter PHYs supported by the Controller */
#define THROUGHPUT_PHYS_NO_PREFER_RX_BIT        0x02 /*!< Has no preference among the receiver PHYs supported by the Controller */

/**
 * @typedef   throughput_phy_options_t
 * @brief     Bluetooth LE Throughput Coded PHY preference options.
 */
typedef enum {
	THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_NO   = 0x00, /*!< Has no preferred coding when transmitting on the LE Coded PHY */
	THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_S2   = 0x01, /*!< Prefer that S=2 coding be used when transmitting on the LE Coded PHY */
	THROUGHPUT_PHYS_OPTIONS_CODED_PREFER_S8   = 0x02, /*!< Prefer that S=8 coding be used when transmitting on the LE Coded PHY */
} throughput_phy_options_t;

/**
 * @typedef   throughput_phy_t
 * @brief     Bluetooth LE Throughput PHY type.
 */
typedef enum {
	THROUGHPUT_PHY_1M             = 0x01, /*!< LE PHY 1M used */
	THROUGHPUT_PHY_2M             = 0x02, /*!< LE PHY 2M used */
	THROUGHPUT_PHY_CODED_S2       = 0x03, /*!< LE Coded PHY (S=2) used */
	THROUGHPUT_PHY_CODED_S8       = 0x04, /*!< LE Coded PHY (S=8) used */
} throughput_phy_t;

/**
 * @typedef   throughput_tx_end_t
 * @brief     Bluetooth LE Throughput TX end type.
 */
typedef enum {
	THROUGHPUT_TX_END_SERVER          = 0x01, /*!< Only server send data */
	THROUGHPUT_TX_END_CLIENT          = 0x10, /*!< Only client send data */
	THROUGHPUT_TX_END_SERVER_CLIENT   = 0x11, /*!< Server & client both send data */
} throughput_tx_end_t;

/**
 * @typedef   throughput_test_mode_t
 * @brief     Bluetooth LE Throughput test mode type.
 */
typedef enum {
	THROUGHPUT_MODE_NOTIFY               = 0x01, /*!< Test mode: Notification */
	THROUGHPUT_MODE_WRITE_CMD            = 0x10, /*!< Test mode: Write Command */
	THROUGHPUT_MODE_NOTIFY_WRITE_CMD     = 0x11, /*!< Test mode: Notification & Write Command */
	THROUGHPUT_MODE_INDICATE             = 0x02, /*!< Test mode: Indication */
	THROUGHPUT_MODE_WRITE_REQ            = 0x20, /*!< Test mode: Write Request */
	THROUGHPUT_MODE_INDICATE_WRITE_REQ   = 0x22, /*!< Test mode: Indication & Write Request */
} throughput_test_mode_t;

/**
 * @typedef   throughput_test_cmd_t
 * @brief     Bluetooth LE Throughput Test Enable type.
 */
typedef enum {
	THROUGHPUT_TEST_STOP  = 0x00, /*!< Indicate test stop & disconnection*/
	THROUGHPUT_TEST_START = 0x01, /*!< Indicate test start */
} throughput_test_cmd_t;

/**
 * @typedef   throughput_config_param_t
 * @brief     Bluetooth LE Throughput configuration parameters struct definition.
 */
typedef struct {
	throughput_test_mode_t mode;        /*!< Config param: test mode*/
	throughput_phy_t phy;               /*!< Config param: PHY type*/
	uint16_t conn_interval;             /*!< Config param: connection interval*/
	uint16_t conn_peripheral_latency;   /*!< Config param: peripheral latency*/
	uint16_t conn_supervision_timeout;  /*!< Config param: supervision timeout*/
	uint16_t length_s;                  /*!< Config param: data length of Notification or Indication*/
	uint16_t length_c;                  /*!< Config param: data length of Write Command or Write Request*/
	uint16_t period;                    /*!< Config param: periodic printing throughput result interval, in sec (s)*/
	uint32_t duration;                  /*!< Config param: throughput duration for the link, in sec (s)*/
} throughput_config_param_t;

/**
 * @typedef   throughput_result_t
 * @brief     Bluetooth LE Throughput result struct definition.
 */
typedef struct {
	uint32_t start_time;     /*!< Result: start time*/
	uint32_t end_time;       /*!< Result: end time*/
	uint32_t elapsed_time;   /*!< Result: elapsed time*/

	uint32_t rx_period_pkts; /*!< Result: packets quantity in a print period*/
	uint32_t rx_all_pkts;    /*!< Result: all packets quantity in the whole throughput duration*/
	uint32_t rx_error_pkts;  /*!< Result: error packets quantity in the whole throughput duration*/

	uint32_t rx_period_Bps;  /*!< Result: data rate in a print period, in Byte/sec (Bps)*/
	uint32_t rx_all_Bps;     /*!< Result: data rate in the whole throughput duration, in Byte/sec (Bps)*/
} throughput_result_t;

void ble_throughput_inc_sequence_generate(uint8_t *data, uint8_t data_bytes);
void ble_throughput_check_calcu(throughput_result_t *p_result,
								uint16_t valid_len,
								uint16_t write_len,
								uint8_t *p_data,
								uint8_t count);
uint32_t ble_throughput_get_elapsed_time(uint32_t start_time, uint32_t end_time);
throughput_tx_end_t ble_throughput_get_tx_end(throughput_test_mode_t mode);

/********************************Throughput Server related*******************************/
rtk_bt_evt_cb_ret_t ble_throughput_server_gatts_app_callback(uint8_t event, void *data, uint32_t len);
void ble_throughput_server_update_phy(uint16_t conn_handle);
uint16_t ble_throughput_server_link_connected(uint16_t conn_handle);
uint16_t ble_throughput_server_link_disconnected(uint16_t conn_handle);
uint16_t ble_throughput_server_add(void);
uint16_t ble_throughput_server_deinit(void);

/********************************Throughput Client related*******************************/
rtk_bt_evt_cb_ret_t ble_throughput_client_gattc_app_callback(uint8_t event, void *data, uint32_t len);
uint16_t ble_throughput_client_send_test_cmd(uint16_t conn_handle, uint8_t *data, uint16_t len);
uint16_t ble_throughput_client_select_tx_mode(uint16_t conn_handle);
uint16_t ble_throughput_client_conn(int argc, char *argv[]);
uint16_t ble_throughput_client_test_cmd(int argc, char *argv[]);
uint16_t ble_throughput_client_link_connected(uint16_t conn_handle);
uint16_t ble_throughput_client_link_disconnected(uint16_t conn_handle);
uint16_t ble_throughput_client_add(void);
uint16_t ble_throughput_client_delete(void);

#ifdef __cplusplus
}
#endif

#endif  /* __THROUGHPUT_INCLUDE_H__ */