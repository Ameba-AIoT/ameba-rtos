// SPDX-License-Identifier: GPL-2.0-only
/*
* Realtek wireless local area network IC driver.
*   This is an interface between cfg80211 and firmware in other core. The
*   commnunication between driver and firmware is IPC（Inter Process
*   Communication）bus.
*
* Copyright (C) 2023, Realtek Corporation. All rights reserved.
*/

#ifndef __INIC_IPC_HOST_TRX_H__
#define __INIC_IPC_HOST_TRX_H__

/*TX reserve size before 802.3 pkt*/
#define WLAN_ETHHDR_LEN		14
#define TXDESC_SIZE		40
#define WLAN_HDR_A4_QOS_HTC_LEN	36
#define WLAN_MAX_IV_LEN		8
#define WLAN_SNAP_HEADER	8
#define WLAN_MAX_MIC_LEN	8
#define WLAN_MAX_ICV_LEN	8

#define SKB_WLAN_TX_EXTRA_LEN	(TXDESC_SIZE+WLAN_HDR_A4_QOS_HTC_LEN+WLAN_MAX_IV_LEN+WLAN_SNAP_HEADER-WLAN_ETHHDR_LEN)
#define WLAN_MAX_PROTOCOL_OVERHEAD (WLAN_HDR_A4_QOS_HTC_LEN+WLAN_MAX_IV_LEN+\
							WLAN_SNAP_HEADER+WLAN_MAX_MIC_LEN+WLAN_MAX_ICV_LEN)/*=68*/

/* should  sync with autoconf_8730e.h */
#define SKB_CACHE_SZ 64
#define SKB_ALIGNMENT	__attribute__((aligned(SKB_CACHE_SZ)))
#define WLAN_HW_INFO_LEN	40/*max(TXDESC_SIZE, RXDESC_SIZE+RXPHYSTS_SIZE)*/
#define WLAN_MAX_ETHFRM_LEN	1904/*max payload size of wifi frame*/

#define MAX_SKB_BUF_SIZE	(((WLAN_HW_INFO_LEN+WLAN_MAX_PROTOCOL_OVERHEAD+WLAN_MAX_ETHFRM_LEN+8)\
							+ SKB_CACHE_SZ) & ~(SKB_CACHE_SZ-1))


#define MAX_LENGTH_OF_TX_QUEUE	(200)
#define MAX_TIMES_TO_TRY_TX	(5)

/* -------------------------- Function declaration -------------------------- */

/* ---------------------------- Global Variables ---------------------------- */
#define QUEUE_STOP_THRES	3
#define QUEUE_WAKE_THRES	6

struct  dev_sk_buff_head {
	struct list_head	*next, *prev;
	unsigned int 		qlen;
};

struct skb_raw_para {
	unsigned char enable;
	unsigned char rate;
	unsigned char retry_limit;
	unsigned char ac_queue;		/*0/3 for BE, 1/2 for BK, 4/5 for VI, 6/7 for VO*/
	unsigned char sgi;		/* 1 for enable data short */
	unsigned char agg_en;
};

struct dev_sk_buff {
	/* These two members must be first. */
	struct dev_sk_buff	*next;		/* Next buffer in list */
	struct dev_sk_buff	*prev;		/* Previous buffer in list */

	struct dev_sk_buff_head	*list;		/* List we are on */
	unsigned char		*head;		/* Head of buffer */
	unsigned char		*data;		/* Data head pointer */
	unsigned char		*tail;		/* Tail pointer	*/
	unsigned char		*end;		/* End pointer */
	void	*dev;		/* Device we arrived on/are leaving by */
	unsigned int 		len;		/* Length of actual data */

	int 			dyalloc_flag;
	unsigned char		busy;
	unsigned char		no_free;
	struct skb_raw_para		tx_raw;
};

struct skb_data {
	struct list_head        list;
	unsigned char           buf[MAX_SKB_BUF_SIZE] SKB_ALIGNMENT;
	atomic_t ref;
};

struct skb_info {
	struct list_head	list;
	struct dev_sk_buff	skb;
} SKB_ALIGNMENT;

static inline unsigned char *dev_skb_put(struct dev_sk_buff *skb, unsigned int len)
{
	unsigned char *tmp = skb->tail;
	skb->tail += len;
	skb->len += len;
	if (unlikely(skb->tail > skb->end)) {
		panic("%s: skb %p, len %d, data %p, end %p, %p.\n", __func__, skb, len, skb->data, skb->end, __builtin_return_address(0));
	}
	return tmp;
}

#endif /* __INIC_IPC_HOST_TRX_H__ */
