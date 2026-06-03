#ifndef __RTW_LLHW_EVENT_H__
#define __RTW_LLHW_EVENT_H__

#define RTW_JOIN_TIMEOUT		20000
#define RTW_SCAN_TIMEOUT		12000
#define RTW_SCAN_ABORT_TIMEOUT	20
#define DEV_REQ_NETWORK_INFO_MAX_LEN	6

/**
 * @brief  The enumeration is join block param.
 */
struct internal_block_param {
	struct completion	sema;
};

#endif //__RTW_LLHW_EVENT_H__
