#ifndef _WHC_USB_DEV_H_
#define _WHC_USB_DEV_H_

//#define CONFIG_USBD_WHC_HOTPLUG

#define whc_dev_send                  whc_usb_dev_send
#define whc_dev_intf_init             whc_usb_dev_init
#define whc_dev_trigger_rx_handle()   whc_usb_dev_trigger_rx_handle()
#define whc_dev_bus_is_idle           whc_usb_dev_bus_is_idle
#define whc_dev_flowctrl(a, b)

#ifdef WHCH_TXAGG
#define WHCH_USB_RXBUF_NUM	(WIFI_WHC_USB_BULKOUT_EP_NUM + 12)
#endif

#define DEV_DMA_ALIGN			CACHE_LINE_SIZE
#define USB_DMA_ALIGN(x)	(((x + CACHE_LINE_SIZE - 1) / CACHE_LINE_SIZE) * CACHE_LINE_SIZE)
#ifdef CONFIG_WHCH
#ifdef WHCH_TXAGG
#define USB_BUFSZ		(USB_DMA_ALIGN(WHCH_TXAGG_NUM * (TXDESC_SIZE + WLAN_MAX_PROTOCOL_OVERHEAD + MAXIMUM_ETHERNET_PACKET_SIZE) + sizeof(struct whc_msg_info)))
#else
#define USB_BUFSZ		(USB_DMA_ALIGN(TXDESC_SIZE + WLAN_MAX_PROTOCOL_OVERHEAD + MAXIMUM_ETHERNET_PACKET_SIZE  + sizeof(struct whc_msg_info)))
#endif
#define USB_SKB_RSVD_LEN	0
#else
#define USB_BUFSZ		(USB_DMA_ALIGN(MAXIMUM_ETHERNET_PACKET_SIZE + sizeof(struct whc_msg_info)))
#define USB_SKB_RSVD_LEN	N_BYTE_ALIGMENT(SKB_WLAN_TX_EXTRA_LEN - sizeof(struct whc_msg_info), 4)
#endif

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

#define EPIDX_TO_NUM(ep_idx)\
    ((ep_idx) == 0 ? WIFI_WHC_USB_BULKOUT_1 : \
     (ep_idx) == 1 ? WIFI_WHC_USB_BULKOUT_2 : \
     (ep_idx) == 2 ? WIFI_WHC_USB_BULKOUT_3 : 0xFFU) /* 0xFF: invalid ep (endpoint nums are unsigned) */

struct whc_usb_irq_info {
	/* device->host */
	u8 txdone;
	/* host->device */
	u8 rxdone_epnum[WIFI_WHC_USB_BULKOUT_EP_NUM];  // store the usb rx endpoint num in interrupt order
	u32 len[WIFI_WHC_USB_BULKOUT_EP_NUM];
	u8 intr_widx; // idx of the rxdone_epnum list to write for usb interrupt
	u8 task_ridx; // idx of the rxdone_epnum list to read for device task
	u8 wait_xmit_skb;  // set 1 means when free skb, sema up device task to deal blocked usb rx data
};

#ifdef WHCH_TXAGG
struct whch_usb_txbuf {
	struct whch_buff *usb_rxbuff_pool;		// rxbuffer used to store wifi txpkt
	struct list_head usb_rxbuf_list;		// free rxbuf list (in-flight buffers stay off-list)
	int usb_rxbuff_num;						// number of rxbufs currently on usb_rxbuf_list (++ on list_add, -- on list_del)
};
#endif	/* WHCH_TXAGG */

struct whc_usb_priv_t {
	struct whc_usb_irq_info irq_info;
	/* device->host */
	u8 *tx_buf;
#ifdef WHCH_TXAGG
	struct whch_usb_txbuf	usb_rxbuf;	/* host->device, whch_buff pool */
	struct whch_buff	*cur_rxbuff[WIFI_WHC_USB_BULKOUT_EP_NUM];	/* buffer currently armed on each BULKOUT EP (NULL = starved) */
#else
	/* host->device, store skb addr.
	Implements USB endpoint num to rx_skb_addr list idx mapping through the macro EPNUM_TO_IDX, e.g,
	for endpoint WIFI_WHC_USB_BULKOUT_1: ep_num = 0x05U, idx = EPNUM_TO_IDX(ep_num) = 0, skb = rx_skb_addr[idx] */
	u8 *rx_skb_addr[WIFI_WHC_USB_BULKOUT_EP_NUM];
#endif
	rtos_mutex_t tx_lock;
	rtos_sema_t usb_tx_sema;
	rtos_sema_t usb_irq_sema;
#if defined(CONFIG_USBD_WHC_HOTPLUG)
	rtos_sema_t usb_attach_status_sema;
#endif
};

struct whc_buf_info;
void whc_usb_dev_init(void);
void whc_usb_dev_send(u8 *buf, u16 len, void *buf_alloc, u8 is_skb);
u8 whc_usb_dev_bus_is_idle(void);
void whc_usb_dev_trigger_rx_handle(void);

#ifdef WHCH_TXAGG
int whch_usb_dev_txagg_buf_num(void);
void whch_usb_dev_txagg_buff_free(u32 idx, u8 pktidx);
void whch_usb_dev_txagg_buf_busy(struct whch_buff *buff, u8 delivered, u8 agg_num);
#endif

#endif /* _WHC_USB_DEV_H_ */

