/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RTK_STATUS_H__
#define __RTK_STATUS_H__

/* The following wifi doxygen groups are workaround for online docs display. SoC need replace these with SoC's own. */
/** @addtogroup WIFI_API Wi-Fi APIs
 *  @brief      WIFI_API module
 *  @{
 */
/** @addtogroup WIFI_Exported_Constants Wi-Fi Exported Constants
 * @{
 */

#define RTK_SUCCESS	0
#define RTK_FAIL	(-1)

#define	RTK_ERR_BADARG            (2)	/**< Bad Argument. */
#define	RTK_ERR_BUSY              (3)	/**< Busy. */
#define	RTK_ERR_NOMEM             (4)	/**< No Memory. */
#define	RTK_ERR_TIMEOUT           (5)	/**< Timeout. */
#define	RTK_ERR_BUFFER_OVERFLOW   (6)	/**< Data exceeds allocated buffer size. */

#define RTK_ERR_DIAG_BASE           0x60
#define RTK_ERR_DIAG_UNINIT         (RTK_ERR_DIAG_BASE + 0x01)  // diag uninitialize
#define RTK_ERR_DIAG_SEM_FAIL       (RTK_ERR_DIAG_BASE + 0x02)  // ipc sema get fail
#define RTK_ERR_DIAG_MALLOC         (RTK_ERR_DIAG_BASE + 0x03)  // malloc fail
#define RTK_ERR_DIAG_FREE           (RTK_ERR_DIAG_BASE + 0x04)  // free fail
#define RTK_ERR_DIAG_TOO_LARGE_BUFF (RTK_ERR_DIAG_BASE + 0x05)  // Too large a buffer
#define RTK_ERR_DIAG_TOO_SMALL_BUFF (RTK_ERR_DIAG_BASE + 0x06)  // Too small a buffer
#define RTK_ERR_DIAG_SEND_TIMEOUT   (RTK_ERR_DIAG_BASE + 0x07)  // timeout in transfer of big event

#define RTK_ERR_DIAG_EVT_ADD_FAIL   (RTK_ERR_DIAG_BASE + 0x10)  // add event fail
#define RTK_ERR_DIAG_EVT_NO_MORE    (RTK_ERR_DIAG_BASE + 0x11)  // no more event
#define RTK_ERR_DIAG_EVT_FIND_FAIL  (RTK_ERR_DIAG_BASE + 0x12)  // find event fail

#define RTK_ERR_WIFI_BASE   0x1000
#define	RTK_ERR_WIFI_CONN_INVALID_KEY           (RTK_ERR_WIFI_BASE + 11)	/**< Invalid key. */
#define	RTK_ERR_WIFI_CONN_SCAN_FAIL             (RTK_ERR_WIFI_BASE + 12)
#define	RTK_ERR_WIFI_CONN_AUTH_FAIL             (RTK_ERR_WIFI_BASE + 13)
#define	RTK_ERR_WIFI_CONN_AUTH_PASSWORD_WRONG   (RTK_ERR_WIFI_BASE + 14)
#define	RTK_ERR_WIFI_CONN_ASSOC_FAIL            (RTK_ERR_WIFI_BASE + 15)
#define	RTK_ERR_WIFI_CONN_4WAY_HANDSHAKE_FAIL   (RTK_ERR_WIFI_BASE + 16)
#define	RTK_ERR_WIFI_CONN_4WAY_PASSWORD_WRONG   (RTK_ERR_WIFI_BASE + 17)

#define	RTK_ERR_WIFI_POWEROFF                   (RTK_ERR_WIFI_BASE + 32)	/**< Wi-Fi is powered off in IPS(Inactive Power Save) mode, unable to access Wi-Fi registers. */
#define	RTK_ERR_WIFI_TX_BUF_FULL                (RTK_ERR_WIFI_BASE + 33)	/**< tx buffer full(hw buffer & skb buffer). */
#define	RTK_ERR_WIFI_NOT_INIT                   (RTK_ERR_WIFI_BASE + 34)
/** @}*/
/** @}*/

#endif