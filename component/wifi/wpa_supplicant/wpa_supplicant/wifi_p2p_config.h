/**
  ******************************************************************************
  * @file    wifi_p2p_config.h
  * @author
  * @version
  * @brief   This file provides user interfaces for Wi-Fi Direct functionality.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

/** @addtogroup wpsp2p WPS/P2P
 *  @ingroup    wlan
 *  @brief      WPS/P2P functions
 *  @{
 */

/**
 * @brief  Initialize P2P data.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P).
 * @param[in]  dev_addr: The device address.
 * @param[in]  go_intent: The GO intent.
 * @param[in]  listen_ch: The listen channel.
 * @param[in]  op_ch: The operation channel.
 * @return  0 on success, otherwise return -1.
 * @note  You can reference @ref cmd_wifi_p2p_start() to understand the process of starting P2P mode.
 */
int wifi_p2p_init(u8 *dev_addr, int go_intent, u8 listen_ch, u8 op_ch);

/**
 * @brief  Deinitialize P2P data.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P).
 * @param  None
 * @return  None
 */
void wifi_p2p_deinit(void);

/**
 * @brief  Command to start WiFi Direct mode for commonly use. Can refer to ATWG in atcmd_wifi.c.\n
 *		After executing this command, P2P resource will be allocated and the driver enter P2P mode.
 * @warning  If the started Wi-Fi Direct mode is no longer needed, please make sure to stop p2p before doing other procedures.
 * @return  None
 */
void cmd_wifi_p2p_start(void);

/**
 * @brief  Command to stop WiFi Direct mode for commonly use. Can refer to ATWH in atcmd_wifi.c.\n
 *			After execute this command, WLAN driver will release P2P resources and turn off Wi-Fi.
 * @warning  This command must be invoked if the enabled Wi-Fi Direct mode is no longer used.
 * @return  None
 */
void cmd_wifi_p2p_stop(void);

/**
 * @brief  Set the device name in WPS ie and P2P ie.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  dev_name: Device name to be set. The max string length is 32 bytes.
 * @return  None
 */
void wifi_p2p_set_dev_name(const char *dev_name);

/**
 * @brief  Set the manufacturer in WPS ie.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  manufacturer: Manufacturer to be set. The max string length is 64 bytes.
 * @return  None
 */
void wifi_p2p_set_manufacturer(const char *manufacturer);

/**
 * @brief  Set the model name in WPS ie.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  model_name: Model name to be set. The max string length is 32 bytes.
 * @return  None
 */
void wifi_p2p_set_model_name(const char *model_name);

/**
 * @brief  Set the model number in WPS ie.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  model_number: Model number to be set. The max string length is 32 bytes.
 * @return  None
 */
void wifi_p2p_set_model_number(const char *model_number);

/**
 * @brief  Set the serial number in WPS ie.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  serial_number: Serial number to be set. The max string length is 32 bytes.
 * @return  None
 */
void wifi_p2p_set_serial_number(const char *serial_number);

/**
 * @brief  Set the primary device type in WPS ie and P2P ie.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  pri_dev_type: Primary device type to be set. The string length is 8 bytes.\n
 *				The first two bytes are category ID, and the last two bytes are subcategory ID.
 *				The middle four bytes are 0x 00-50-F2-04.
 * @return  None
 */
void wifi_p2p_set_pri_dev_type(const u8 *pri_dev_type);

/**
 * @brief  Set the GO SSID.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[in]  ssid_in: Ssid_in will be set as GO SSID. The string length is between 1 byte and 32 bytes.
 * @return  None
 */
void wifi_p2p_set_ssid(const char *ssid_in);

/**
 * @brief  Initialize the Wi-Fi Direct GO parameters.
 * @warning  Before invoking this function, the Wi-Fi should be enabled by using @ref wifi_on(RTW_MODE_P2P) and P2P be initialized by using @ref wifi_p2p_init().
 * @warning  You can reference @ref cmd_wifi_p2p_start() to initialize them.
 * @param[out]  res: The location where the set parameters will be stored. (@ref p2p_go_neg_results)
 * @param[in]  passphrase: The GO passphrase.
 * @param[in]  channel: The operation channel.
 * @return  None
 */
void wifi_p2p_init_auto_go_params(void *res, u8 *passphrase, u8 channel);

void cmd_p2p_connect(int argc, char **argv);
void cmd_p2p_find(int argc, char **argv);
void cmd_p2p_listen(int argc, char **argv);

/*\@}*/
