#ifndef _RTW_NAN_VENDOR_OSDEP_H_
#define _RTW_NAN_VENDOR_OSDEP_H_

#include "user_os_info.h"

#define CONFIG_NAN_PAIRING

/* Define correspoding typedef and NAN Feature based on USER_PLATFORM type */

#ifdef USER_OS_INFO_LINUX
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int  u32;

#define NL80211_SUBCMD_NAN_RANGE_START 0x1900
#define NL80211_SUBCMD_NAN_RANGE_END   0x19FF

#define ETH_ALEN 6

#ifndef MAC_ALEN
#define MAC_ALEN ETH_ALEN
#endif

enum rtw_nan_vendor_subcmd {
	NAN_SUBCMD_SRVC_EXT_INFO = NL80211_SUBCMD_NAN_RANGE_START,	/* 0x1900 */
	NAN_SUBCMD_DATA_REQ,					/* 0x1901 */
	NAN_SUBCMD_DATA_RSP,					/* 0x1902 */
	NAN_SUBCMD_DATA_END,					/* 0x1903 */
	NAN_SUBCMD_DATA_UPDATE,					/* 0x1904 */
	NAN_SUBCMD_CUSTOMER_NANDOW,				/* 0x1905 */
	NAN_SUBCMD_FOLLOWUP,					/* 0x1906 */
	NAN_SUBCMD_PAIRING_SET_PW,                      	/* 0x1907 */
	NAN_SUBCMD_PASN_START,                          	/* 0x1908 */
	NAN_SUBCMD_PASN_SET_KEY,                          	/* 0x1909 */
};

#endif
#endif /* _RTW_NAN_VENDOR_OSDEP_H_ */
