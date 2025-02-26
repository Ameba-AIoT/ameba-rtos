#ifndef _WHC_USB_DEV_H_
#define _WHC_USB_DEV_H_

#define whc_dev_tx_path_avail    whc_usb_dev_tx_path_avail
#define whc_dev_send             whc_usb_dev_send
#define whc_dev_init             whc_usb_dev_init

#define DEV_DMA_ALIGN			CACHE_LINE_SIZE
#define USB_DMA_ALIGN(x)	(((x + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE)
#define USB_BUFSZ		(USB_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
#define USB_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), 4)

#define WIFI_INIC_USB_STATUS_ACTIVE                       0
#define WIFI_INIC_USB_STATUS_SUSPEND                      1
#define WIFI_INIC_USB_STATUS_DISABLED                     2
#define WIFI_INIC_USB_WAKE_HOST_MAX_WAIT_CNT              10
#define WIFI_INIC_USB_WAKE_HOST_UNIT                      10

struct whc_usb_priv_t {
	rtos_mutex_t tx_lock;
	rtos_sema_t usb_tx_sema;
};

struct whc_buf_info;
void whc_usb_dev_init(void);
void whc_usb_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
void whc_usb_dev_send(struct whc_buf_info *pbuf);
u8 whc_usb_dev_tx_path_avail(void);



#endif

