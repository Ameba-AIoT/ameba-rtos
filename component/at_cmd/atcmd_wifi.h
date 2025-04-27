/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __ATCMD_WIFI_H__
#define __ATCMD_WIFI_H__

#define RTW_AT_COMMON_ERR_NO_BASE		0x0
#define RTW_AT_UNIQUE_ERR_NO_BASE		0x10

/**
 * @brief  The enumeration lists wifi at_cmd error codes.
 */
enum rtw_atcmd_err_no {
	RTW_AT_OK = 0,                                                              /**< atcmd success */

	/* 0x1~0x10 for common error codes */
	RTW_AT_ERR_REQUIRED_PARAM_MISS    = RTW_AT_COMMON_ERR_NO_BASE + 1,           /**< Required parameters not provided */
	RTW_AT_ERR_INVALID_PARAM_VALUE    = RTW_AT_COMMON_ERR_NO_BASE + 2,           /**< Invalid parameters value */
	RTW_AT_ERR_PARAM_NUM_ERR          = RTW_AT_COMMON_ERR_NO_BASE + 3,           /**< Parameters number error */
	RTW_AT_ERR_MEM_ALLOC_FAIL         = RTW_AT_COMMON_ERR_NO_BASE + 4,           /**< Insufficient memory */
	RTW_AT_ERR_FUNC_NOT_SUPPORT       = RTW_AT_COMMON_ERR_NO_BASE + 5,           /**< Function not support */
	RTW_AT_ERR_UNKNOWN_ERR            = RTW_AT_COMMON_ERR_NO_BASE + 6,           /**< Unknown system error */

	/* 0x11~ for unique error codes related to different at_cmd */
	RTW_AT_ERR_CONNECT_FAILED         = RTW_AT_UNIQUE_ERR_NO_BASE + 1,           /**< at+wlconn: connect failed */
	RTW_AT_ERR_RESULT_GET_FAILED      = RTW_AT_UNIQUE_ERR_NO_BASE + 2,           /**< at+wldisconn: result retrieval failed */
	RTW_AT_ERR_INVALID_WIFI_STATUS    = RTW_AT_UNIQUE_ERR_NO_BASE + 3,           /**< at+wlscan: invalid wifi status */
	RTW_AT_ERR_INVALID_SCAN_PARAM     = RTW_AT_UNIQUE_ERR_NO_BASE + 4,           /**< at+wlscan: invalid scan parameters */
	RTW_AT_ERR_SAP_START_FAILED       = RTW_AT_UNIQUE_ERR_NO_BASE + 5,           /**< at+wlstartap: ap start failed */
	RTW_AT_ERR_SAP_START_TIMEOUT      = RTW_AT_UNIQUE_ERR_NO_BASE + 6,           /**< at+wlstartap: ap start timeout */
};

void print_wifi_at(void);
void at_wifi_init(void);

#endif
