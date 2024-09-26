#ifndef __INIC_USB_H__
#define __INIC_USB_H__

#define SIZE_RX_DESC			0
#define SIZE_TX_DESC			0

#define BUF_ALIGN_SZ			0

#define WIFI_OUT_EP_NUM_TOTAL	1
#define RTW_USB_MAX_SKB_SIZE				1600

#define CONFIG_INIC_USB_ASYNC_SEND

struct inic_usb {
	struct mutex	lock; /* mutex to protect send host event_priv message */

	struct usb_device *usb_dev;

	struct device *dev;
	s32 bSurpriseRemoved;

	struct semaphore dev_rdy_sema;

	u8 dev_state;

	bool rx_pending;

	int (*rx_process_func)(struct sk_buff *pskb);

	unsigned int rx_pipe;
	unsigned int tx_pipe[WIFI_OUT_EP_NUM_TOTAL];
	unsigned int tx_mps;
	u8	out_pipe_idx;

	struct list_head rx_freeq;
	struct list_head tx_freeq;
	struct rtw_usbreq *rxreqs;
	struct rtw_usbreq *txreqs;
	struct sk_buff_head rxnode_list;

	spinlock_t usb_qlock;
	spinlock_t usb_rxskb_lock;

	atomic_t tx_inflight;
	u8 usb_disconnecting;
	u8 usb_deregistering;
};

struct rtw_usbreq {
	struct list_head list;
	struct urb *urb;
	void  *skb;
	u8	is_buf;
};

extern struct inic_usb inic_usb_priv;
extern struct hci_ops_t usb_intf_ops;

void rtw_usb_rx_complete(struct urb *urb);
void rtw_usb_enqueue(struct list_head *q, struct rtw_usbreq *req, int *counter);
struct rtw_usbreq *rtw_usb_dequeue(struct list_head *q, int *counter);
void rtw_usb_recv_data_process(void *intf_priv);
int rtw_usb_send_event_check(u32 event_id);
#endif /* __INIC_USB_H__ */
