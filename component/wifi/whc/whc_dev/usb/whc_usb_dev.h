#ifndef _WHC_USB_DEV_H_
#define _WHC_USB_DEV_H_

#define CONFIG_USBD_WHC_HOTPLUG					          0

#define whc_dev_tx_path_avail    whc_usb_dev_tx_path_avail
#define whc_dev_send             whc_usb_dev_send
#define whc_dev_init             whc_usb_dev_init
#define whc_dev_trigger_rx_handle   whc_usb_dev_trigger_rx_handle
#define _whc_dev_api_bus_is_idle       whc_usb_dev_bus_is_idle

#define DEV_DMA_ALIGN			CACHE_LINE_SIZE
#define USB_DMA_ALIGN(x)	(((x + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE)
#define USB_BUFSZ		(USB_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
#define USB_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), 4)

#define WIFI_WHC_USB_STATUS_ACTIVE                       0
#define WIFI_WHC_USB_STATUS_SUSPEND                      1
#define WIFI_WHC_USB_STATUS_DISABLED                     2
#define WIFI_WHC_USB_WAKE_HOST_MAX_WAIT_CNT              10
#define WIFI_WHC_USB_WAKE_HOST_UNIT                      10

#if defined (CONFIG_AMEBADPLUS)
#define WIFI_WHC_USB_BULKIN_EP  USBD_WHC_WIFI_EP3_BULK_IN
#define WIFI_WHC_USB_BULKOUT_1  USBD_WHC_WIFI_EP2_BULK_OUT
#define WIFI_WHC_USB_BULKOUT_2  USBD_WHC_WIFI_EP4_BULK_OUT
#define WIFI_WHC_USB_BULKOUT_3  0xFF /*invalid*/
#define WIFI_WHC_USB_BULKOUT_EP_NUM 2
#define WIFI_WHC_USB_SPEED      USB_SPEED_FULL
#elif defined(CONFIG_AMEBAGREEN2)
#define WIFI_WHC_USB_BULKIN_EP  USBD_WHC_WIFI_EP4_BULK_IN
#define WIFI_WHC_USB_BULKOUT_1  USBD_WHC_WIFI_EP5_BULK_OUT
#define WIFI_WHC_USB_BULKOUT_2  USBD_WHC_WIFI_EP6_BULK_OUT
#define WIFI_WHC_USB_BULKOUT_3  USBD_WHC_WIFI_EP7_BULK_OUT
#define WIFI_WHC_USB_BULKOUT_EP_NUM 3
#define WIFI_WHC_USB_SPEED      USB_SPEED_HIGH
#endif

#define EPNUM_TO_IDX(ep_num)\
    ((ep_num) == WIFI_WHC_USB_BULKOUT_1 ? 0 : \
     (ep_num) == WIFI_WHC_USB_BULKOUT_2 ? 1 : \
     (ep_num) == WIFI_WHC_USB_BULKOUT_3 ? 2 : -1)

struct whc_usb_irq_info {
	/* device->host */
	u8 txdone;
	/* host->device */
	u8 rxdone_epnum[WIFI_WHC_USB_BULKOUT_EP_NUM];  // store the usb rx endpoint num in interrupt order
	u16 len[WIFI_WHC_USB_BULKOUT_EP_NUM];
	u8 intr_widx; // idx of the rxdone_epnum list to write for usb interrupt
	u8 task_ridx; // idx of the rxdone_epnum list to read for device task
	u8 wait_xmit_skb;  // set 1 means when free skb, sema up device task to deal blocked usb rx data
};

struct whc_usb_priv_t {
	struct whc_usb_irq_info irq_info;
	/* device->host */
	u8 *tx_buf;
	/* host->device, store skb addr.
	Implements USB endpoint num to rx_skb_addr list idx mapping through the macro EPNUM_TO_IDX, e.g,
	for endpoint WIFI_WHC_USB_BULKOUT_1: ep_num = 0x05U, idx = EPNUM_TO_IDX(ep_num) = 0, skb = rx_skb_addr[idx] */
	u8 *rx_skb_addr[WIFI_WHC_USB_BULKOUT_EP_NUM];
	rtos_mutex_t tx_lock;
	rtos_sema_t usb_tx_sema;
	rtos_sema_t usb_irq_sema;
#if defined (CONFIG_USBD_WHC_HOTPLUG)
	rtos_sema_t usb_attach_status_sema;
#endif
};

struct whc_buf_info;
void whc_usb_dev_init(void);
void whc_usb_dev_event_int_hdl(u8 *rxbuf, struct sk_buff *skb);
void whc_usb_dev_send(struct whc_buf_info *pbuf);
u8 whc_usb_dev_tx_path_avail(void);
void whc_usb_dev_trigger_rx_handle(void);
u8 whc_usb_dev_bus_is_idle(void);

#endif

