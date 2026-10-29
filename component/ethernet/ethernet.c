#include <cmsis.h>
#include "ethernet_api.h"
#include "ethernet.h"
#include "ethernet_ex_api.h"
#include "timer_api.h"

static const char *const TAG = "ETH";
/* Descriptor number */
#define ETH_TX_DESC_CNT         CONFIG_ETH_TX_DESC_CNT
#define ETH_RX_DESC_CNT         CONFIG_ETH_RX_DESC_CNT
/* The RX ring size register takes the count minus one as a mask */
#if (ETH_RX_DESC_CNT & (ETH_RX_DESC_CNT - 1))
#error "CONFIG_ETH_RX_DESC_CNT must be a power of two"
#endif
/* Retry count for transmission */
#define ETH_TX_RETRY_CNT        3

/* Only SoCs whose fwlib provides Ethernet_SwapRXSegBuf() define ETH_RX_ZERO_COPY. */
#if defined(ETH_RX_ZERO_COPY) && ETH_RX_ZERO_COPY
#define ETH_RX_ZC               1
#else
#define ETH_RX_ZC               0
#endif

/* Configuration Flags */
volatile int eth_link_is_up = 0;

ETH_InitTypeDef eth_handle;

/* DMA Descriptors (Must be non-cacheable & 32-byte aligned) */
SRAM_NOCACHE_DATA_SECTION
static u8 eth_tx_desc_arr[ETH_TX_DESC_CNT][ETH_TX_DESC_SIZE] __attribute__((aligned(32)));

SRAM_NOCACHE_DATA_SECTION
static u8 eth_rx_desc_arr[ETH_RX_DESC_CNT][ETH_RX_DESC_SIZE] __attribute__((aligned(32)));

/* OS sema/mutex */
static rtos_mutex_t eth_tx_mutex;
static rtos_sema_t  eth_rx_sema;

#if ETH_RX_ZC
/* One RX buffer. While its address sits in a descriptor it belongs to DMA; once the
 * frame is received it goes up to lwIP as pc and comes back to the heap through
 * eth_rx_blk_free(). pc gets a cache line of its own so invalidating dma[] before
 * handing it to DMA never discards what the CPU wrote into the pbuf. */
struct eth_rx_blk {
	struct pbuf_custom pc;          /* must stay first: the pbuf address is the block address */
	u8 rsvd[CACHE_LINE_SIZE - sizeof(struct pbuf_custom)];
	u8 dma[ETH_MAX_BUF_SIZE];       /* cache-line aligned, whole lines only */
};

#define ETH_RX_BLK(dma_addr)    ((struct eth_rx_blk *)((u32)(dma_addr) - offsetof(struct eth_rx_blk, dma)))

static void eth_rx_blk_free(struct pbuf *p)
{
	rtos_mem_free(p);
}

static struct eth_rx_blk *eth_rx_blk_alloc(void)
{
	/* rtos_mem_malloc returns cache-line aligned memory. */
	struct eth_rx_blk *blk = (struct eth_rx_blk *)rtos_mem_malloc(sizeof(struct eth_rx_blk));

	if (blk != NULL) {
		blk->pc.custom_free_function = eth_rx_blk_free;
		/* Drop whatever the previous owner of this memory left in the cache, or an
		 * eviction could land on top of what DMA writes. */
		DCache_Invalidate((u32)blk->dma, sizeof(blk->dma));
	}

	return blk;
}

/**
  * @brief  Take the current RX frame out of the ring without copying it.
  * @retval The frame as a pbuf chain, one pbuf per descriptor, or NULL when out of memory.
  * @note   Every descriptor of the frame gets a fresh buffer, and the received ones become
  *         the pbufs. All or nothing: if any allocation fails the old buffers stay in the
  *         ring and the frame is dropped.
  */
static struct pbuf *eth_rx_zc_take(void)
{
	struct pbuf *head = NULL, *q;
	struct eth_rx_blk *blk;
	/* The MAC puts 2 bytes ahead of the frame in its first buffer. The descriptor length
	 * counts frame bytes only (CRC included, those 2 not), so they shift the payload but
	 * never shorten it. */
	u32 seg, seg_len, off = 2;

	for (seg = 0; seg < eth_handle.ETH_RxSegmentCount; seg++) {
		blk = eth_rx_blk_alloc();
		if (blk == NULL) {
			goto undo;
		}

		blk = ETH_RX_BLK(Ethernet_SwapRXSegBuf(&eth_handle, seg, (u32)blk->dma, &seg_len));
		q = pbuf_alloced_custom(PBUF_RAW, (u16)seg_len, PBUF_REF, &blk->pc, blk->dma + off, (u16)(sizeof(blk->dma) - off));
		off = 0;

		if (head == NULL) {
			head = q;
		} else {
			pbuf_cat(head, q);
		}
	}

	return head;

undo:
	/* The pbufs live inside the old blocks, so there is nothing to free but the new ones. */
	for (seg = 0, q = head; q != NULL; seg++, q = q->next) {
		rtos_mem_free(ETH_RX_BLK(Ethernet_SwapRXSegBuf(&eth_handle, seg, (u32)((struct eth_rx_blk *)q)->dma, &seg_len)));
	}

	return NULL;
}
#endif /* ETH_RX_ZC */

/* Callbacks */
eth_rx_prehandler_t eth_rx_preprocess_cb = NULL;
link_up_down_callback eth_link_cb = NULL;

void eth_register_rx_patch(eth_rx_prehandler_t pfunc1)
{
	eth_rx_preprocess_cb = pfunc1;
}

void eth_register_link_cb(link_up_down_callback pfunc)
{
	eth_link_cb = pfunc;
}

/**
 * @brief  Configure the PHY reset GPIO, the RMII pinmux and the MDC/MDIO pinmux.
 */
static void eth_config_pins(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* PHY hardware reset pin: park it as an input so it cannot reset the PHY by accident */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = _PA_3;
	GPIO_Init(&GPIO_InitStruct);

	/* RMII data pins */
	for (int i = 0; i < 8; i++) {
		Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][i], PINMUX_FUNCTION_RMII);
	}

	/* MDC/MDIO */
	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][8], PINMUX_FUNCTION_RMII_MDC);
	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][9], PINMUX_FUNCTION_RMII_MDIO);
}
/**
 * @brief Configure the Ethernet PHY and MAC clocks.
 * @note EXTCLK supplies an external 50MHz MAC clock, or outputs a
 *       SoC-generated 50MHz/25MHz PHY clock. These modes are exclusive.
 * @return Result returned by Ethernet_MacClkConfig().
 */
static int eth_config_clocks(void)
{
	/* Select the MAC clock source */
#if defined(CONFIG_MAC_CLK_EXT)
	return Ethernet_MacClkConfig(ETH_MAC_CLK_EXT50M, ETHERNET_PAD[ETHERNET_Pin_Grp][10]);
#else
	/* Configure the SoC clock output when the PHY requires it. */
#if defined(CONFIG_SOC_OUTPUT_50M)
	Ethernet_PhyClkConfig(ETHERNET_PAD[ETHERNET_Pin_Grp][10], ETH_PHY_CLK_50M);
#elif defined(CONFIG_SOC_OUTPUT_25M)
	Ethernet_PhyClkConfig(ETHERNET_PAD[ETHERNET_Pin_Grp][10], ETH_PHY_CLK_25M);
#endif

	return Ethernet_MacClkConfig(ETH_MAC_CLK_INT50M, 0);
#endif
}

int eth_send(struct pbuf *p)
{
	int ret = RTK_FAIL;
	ETH_PktMetaDef meta;
	struct pbuf *q;
	u8 *dma_buffer = NULL;
	const u8 *src;
	u32 len;
	u32 off;
	u32 retry;

	if (p == NULL) {
		return ret;
	}
	off = (u32)p->payload & 3;
	/* Checked before taking a descriptor, so a rejected frame leaves the ring untouched. */
	if (off + p->tot_len > ETH_MAX_BUF_SIZE) {
		RTK_LOGE(TAG, "TX frame too large: %d\n", p->tot_len);
		return ret;
	}
	memset(&meta, 0, sizeof(ETH_PktMetaDef));
	/* Lock TX to prevent race conditions */
	rtos_mutex_take(eth_tx_mutex, MUTEX_WAIT_TIMEOUT);

	/* Get a free descriptor buffer from hardware driver. When the ring is full, give up the CPU for a tick
	 * and retry a few times before failing the frame. This runs in tcpip_thread under the core lock, so keep
	 * the retry count small: it bounds how long the stack can stall. */
	dma_buffer = Ethernet_GetTXPktInfo(&eth_handle);
	for (retry = 0; dma_buffer == NULL && retry < ETH_TX_RETRY_CNT; retry++) {
		rtos_time_delay_ms(1);
		dma_buffer = Ethernet_GetTXPktInfo(&eth_handle);
	}

	if (dma_buffer != NULL) {
		/* Start the frame at the first pbuf's word offset and point the descriptor there (TX DMA takes any
		 * byte address), so src and dst share a phase and ROM memcpy, which copies by words only when both
		 * are aligned, does not fall back to bytes. */
		dma_buffer += off;
		eth_handle.ETH_TxDesc[eth_handle.ETH_TxDescCurrentNum].addr = (u32)dma_buffer;
		/* Copy data from pbuf chain to flat DMA buffer */
		for (q = p; q != NULL; q = q->next) {
			src = (const u8 *)q->payload;
			len = q->len;
			/* First pbuf only: dst shares its phase, so both are aligned after these bytes. Later pbufs are
			 * expected to land aligned; if one does not, memcpy byte-copies it, slower but still correct. */
			if (q == p) {
				while (((u32)dma_buffer & 3) && len) {
					*dma_buffer++ = *src++;
					len--;
				}
			}
			memcpy(dma_buffer, src, len);
			dma_buffer += len;
			meta.pkt_len += q->len;
		}

		/* Trigger Hardware Transmission */
		Ethernet_UpdateTXDESCAndSend(&eth_handle, &meta);
		ret = RTK_SUCCESS;
	}

	rtos_mutex_give(eth_tx_mutex);
	return ret;
}

u32 eth_isr_handler(void *data)
{
	UNUSED(data);
	/* Read the status once and clear every bit read, including the ones not acted on below. */
	u16 status = Ethernet_GetPendingINT();

	Ethernet_ClearINT(status);

	RTK_LOGD(TAG, "int status = %x\n", status);

	/* 1. RX done, or the RX ring ran dry: both mean the RX task should drain and
	 * recycle descriptors. */
	if (status & (BIT_ISR_ROK | BIT_ISR_RDU1)) {
		rtos_sema_give(eth_rx_sema);
	}

	/* 2. Link change. A failed post (mbox full) drops this event only; the handler reads MSR afresh each time. */
	if (status & BIT_ISR_LINKCHG) {
		netif_adapter_eth_link_notify();
	}

	/* RX/TX errors and TX done need no action beyond the acknowledge above. */

	return 0;
}
#if ETH_CSUM_OFFLOAD
/**
  * @brief  Report whether the MAC failed a frame's IPv4/L4 checksum.
  * @param  meta: RX metadata filled by Ethernet_GetRXPktInfo().
  * @retval 1 if verification failed and the frame must be discarded, 0 to accept.
  * @note   csum_ip/csum_l4 hold the IPV4CSF/L4CSF fail bits, only meaningful for
  *         the packet types the classifier parsed, hence the rx_pkt_type switch.
  */
static u8 eth_rx_cksum_failed(const ETH_PktMetaDef *meta)
{
	switch (meta->rx_pkt_type) {
	case ETH_PKT_IPV4:
	case ETH_PKT_IPV4_PPTP:
	case ETH_PKT_IPV4_ICMP:
	case ETH_PKT_IPV4_IGMP:
		/* IPv4 header checksum only. */
		return meta->csum_ip;

	case ETH_PKT_IPV4_TCP:
	case ETH_PKT_IPV4_UDP:
		/* A UDP sender may leave the checksum field at 0 to mean "not computed"
		 * (RFC 768); the engine ignores those frames rather than failing them, so
		 * no software exemption is needed here. TCP has no such escape, and a
		 * zero there is a genuine failure. */
		return (meta->csum_ip || meta->csum_l4);

	case ETH_PKT_IPV6_TCP:
	case ETH_PKT_IPV6_UDP:
		/* IPv6 has no header checksum, so only the L4 verdict applies. */
		return meta->csum_l4;

	default:
		/* Plain ethernet, IPv6 without L4, ICMPv6: nothing was verified. */
		return 0;
	}
}
#endif /* ETH_CSUM_OFFLOAD */

void eth_rx_thread(void *param)
{
	(void)param;
	ETH_PktMetaDef meta;
	struct pbuf *p = NULL;
	u8 *buf = NULL;
	enum prehandler_ret ret;

	while (1) {
		/* Wait for RX interrupt signal */
		if (rtos_sema_take(eth_rx_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			continue;
		}

		while (1) {
			buf = Ethernet_GetRXPktInfo(&eth_handle, &meta);

			if ((meta.pkt_len == 0) || (buf == NULL)) {
				/* Ring buffer empty */
				break;
			}

			/* Nothing left to hand up once the MAC's 2-byte header is removed.
			 * The default filter drops runt frames, but ETH_FILTER_DIAGNOSTIC_ALL
			 * lets them through. Must still fall through to release the descriptor,
			 * or the ring index parks here and the loop spins forever. */
			if (meta.pkt_len > 2) {
#if ETH_CSUM_OFFLOAD
				/* The MAC verified the checksums, so a bad frame must be dropped
				 * here: lwIP no longer re-checks them on this netif. */
				if (!eth_rx_cksum_failed(&meta))
#endif
				{
					/* Optional: Custom Pre-processing for test*/
					if (eth_rx_preprocess_cb) {
						ret = eth_rx_preprocess_cb(&p, buf + 2, meta.pkt_len);
						if (ret == UPLOAD_TO_LWIP && p != NULL) {
							netif_adapter_eth_recv(p);
						} else if (p != NULL) {
							pbuf_free(p);
							p = NULL;
						}
					} else {
#if ETH_RX_ZC
						p = eth_rx_zc_take();
#else
						/* buf is the word-aligned buffer start, 2 bytes ahead of the
						 * frame; the copy keeps those 2 and strips them in the pbuf. */
						p = netif_adapter_eth_buf_copy(meta.pkt_len - 2, buf);
#endif
						if (p != NULL) {
							netif_adapter_eth_recv(p);
						}
					}
				}
			}

			/* Return the frame's descriptors to hardware; this is what advances the ring index. */
			Ethernet_ReleaseRXDesc(&eth_handle, eth_handle.ETH_RxSegmentCount);
		}
	}

	rtos_sema_delete(eth_rx_sema);
	rtos_task_delete(NULL);
}

#ifndef CONFIG_RMII_VERIFY
/**
 * @brief  Link change handler, runs in tcpip_thread with the core lock held.
 *         The netif may be touched directly, but nothing here may block or call netifapi.
 */
static void eth_link_handler(void *ctx)
{
	u32 speed, duplex;
	int link_up = Ethernet_GetLinkStatus() ? 1 : 0;

	UNUSED(ctx);
	/* The same message can be queued twice before it runs; report each state once. */
	if (link_up == eth_link_is_up) {
		return;
	}
	eth_link_is_up = link_up;

	if (link_up) {
		Ethernet_GetSpeedDuplex(&speed, &duplex);
		RTK_LOGI(TAG, "Link Up: %s, %s\n", (speed == ETH_SPEED_100M) ? "100M" : (speed == ETH_SPEED_10M) ? "10M" : "Unknown",
				 (duplex == ETH_FULL_DUPLEX) ? "Full Duplex" : "Half Duplex");
	} else {
		RTK_LOGI(TAG, "Link Down\n");
	}
	netif_adapter_eth_set_link(link_up);

	if (eth_link_cb) {
		eth_link_cb(link_up);
	}
}
#endif

/**
 * @brief  Hardware Initialization Task.
 *         Allocates memory, configures PHY, sets MAC, starts ETH controller.
 */
static void eth_init_thread(void *param)
{
	(void)param;
	int ret;
	/* Used when the OTP holds no MAC: fixed prefix, random last byte */
	const u8 default_mac[ETH_MAC_ADDR_LEN] = {0x00, 0xE0, 0x4C, 0xb7, 0x23, 0x00};
	const u8 blank_mac[ETH_MAC_ADDR_LEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
	u8 eth_mac[ETH_MAC_ADDR_LEN];
	/* Buffers */
	u8 *eth_tx_pkt_buf = NULL;
	u8 *eth_rx_pkt_buf = NULL;

	/* PHY Configuration */
	const struct eth_mdio_ops eth_mdio_bus = {
		.mdio_read  = Ethernet_ReadPhyReg,
		.mdio_write = Ethernet_WritePhyReg,
	};
	struct eth_phy_dev phy_dev = {
		.bus  = &eth_mdio_bus,
		.addr = 0x01,
		.ops  = &phy_rtl8201f_ops,
	};

	/* Initialize LwIP Stack (One-time) */
#ifndef CONFIG_RMII_VERIFY
	if (!lwip_init_done) {
		lwip_module_init();
	}
#endif

	Ethernet_StructInit(&eth_handle, &phy_dev);
	/* Ensure there is only one source for the RMII reference clock. */
#if defined(CONFIG_PHY_INT_XTAL) || defined(CONFIG_MAC_CLK_EXT)
	eth_handle.MacConfig.Bits.RefClkDir = ETH_REFCLK_PHY2MAC;
#endif

	/* Memory Allocation for Descriptors */
	if (eth_tx_pkt_buf) {
		rtos_mem_free(eth_tx_pkt_buf);
		eth_tx_pkt_buf = NULL;
	}
	if (eth_rx_pkt_buf) {
		rtos_mem_free(eth_rx_pkt_buf);
		eth_rx_pkt_buf = NULL;
	}

	eth_tx_pkt_buf = (u8 *)rtos_mem_zmalloc(ETH_TX_DESC_CNT * ETH_MAX_BUF_SIZE);
#if !ETH_RX_ZC
	eth_rx_pkt_buf = (u8 *)rtos_mem_zmalloc(ETH_RX_DESC_CNT * ETH_MAX_BUF_SIZE);
#endif

	if (eth_tx_pkt_buf == NULL || (!ETH_RX_ZC && eth_rx_pkt_buf == NULL)) {
		RTK_LOGE(TAG, "Memory allocation failed for descriptors!\n");
		goto error;
	}

	/* Clear Descriptor Arrays */
	memset(eth_tx_desc_arr, 0, sizeof(eth_tx_desc_arr));
	memset(eth_rx_desc_arr, 0, sizeof(eth_rx_desc_arr));

#if ETH_RX_ZC
	/* Each RX descriptor owns a block of its own. ETH_RxPktBuf stays NULL so that
	 * Ethernet_Init() keeps these addresses. */
	for (u32 i = 0; i < ETH_RX_DESC_CNT; i++) {
		struct eth_rx_blk *blk = eth_rx_blk_alloc();

		if (blk == NULL) {
			RTK_LOGE(TAG, "Memory allocation failed for RX buffers!\n");
			goto error;
		}
		((ETH_RxDescTypeDef *)eth_rx_desc_arr)[i].addr = (u32)blk->dma;
	}
#endif

	/* Assign Buffers to Handle */
	eth_handle.ETH_TxDescNum = ETH_TX_DESC_CNT;
	eth_handle.ETH_RxDescNum = ETH_RX_DESC_CNT;
	eth_handle.ETH_TxDesc    = (ETH_TxDescTypeDef *)eth_tx_desc_arr;
	eth_handle.ETH_RxDesc    = (ETH_RxDescTypeDef *)eth_rx_desc_arr;
	eth_handle.ETH_TxPktBuf  = eth_tx_pkt_buf;
	eth_handle.ETH_RxPktBuf  = eth_rx_pkt_buf;

	/* Start from all 0xFF so that a failed read counts as a blank OTP */
	memset(eth_mac, 0xFF, sizeof(eth_mac));
	OTP_LogicalRead(eth_mac, EFUSE_ETH_MAC_ADDR, sizeof(eth_mac));
	if (memcmp(eth_mac, blank_mac, sizeof(eth_mac)) == 0) {
		memcpy(eth_mac, default_mac, sizeof(eth_mac));
		/* Bit 0 stays 0 so the address is unicast */
		eth_mac[ETH_MAC_ADDR_LEN - 1] = (u8)(_rand() % 1000) & 0xFE;
	}

	/* Set MAC in Handle and LwIP struct */
	memcpy((void *)(eth_handle.ETH_MacAddr), (void *)eth_mac, ETH_MAC_ADDR_LEN);
	memcpy((void *)pnetif_eth->hwaddr, (void *)eth_mac, ETH_MAC_ADDR_LEN);

	ret = Ethernet_Init(&eth_handle);
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Ethernet_Init failed: %d\n", ret);
		goto error;
	}

	/* Sync Objects Creation, before the ISR and the netif can use them */
	rtos_sema_create_binary(&eth_rx_sema);
	rtos_mutex_create(&eth_tx_mutex);

#ifndef CONFIG_RMII_VERIFY
	/* Before the IRQ is enabled, so the ISR never sees a half-made message */
	if (netif_adapter_eth_link_init(eth_link_handler) != 0) {
		RTK_LOGE(TAG, "Link msg alloc failed\n");
	}
#endif

	/* Interrupt Init */
	InterruptRegister((IRQ_FUN)eth_isr_handler, (IRQn_Type)RMII_IRQ, (u32)&eth_handle, 5);
	InterruptEn(RMII_IRQ, 5);

#ifndef CONFIG_RMII_VERIFY
	netifapi_netif_set_up(pnetif_eth);
#endif

	/* Must stay below tcpip_thread: higher starves the socket reader, lower drains
	 * the ring too slowly. */
	if (RTK_SUCCESS != rtos_task_create(NULL, "ETH_RX", eth_rx_thread, NULL, 512, 6)) {
		RTK_LOGE(TAG, "Create ETH_RX task failed\n");
	}
	rtos_task_delete(NULL);
	return;

error:
	if (eth_tx_pkt_buf) {
		rtos_mem_free(eth_tx_pkt_buf);
	}
	if (eth_rx_pkt_buf) {
		rtos_mem_free(eth_rx_pkt_buf);
	}
#if ETH_RX_ZC
	for (u32 i = 0; i < ETH_RX_DESC_CNT; i++) {
		u32 addr = ((ETH_RxDescTypeDef *)eth_rx_desc_arr)[i].addr;

		if (addr) {
			rtos_mem_free(ETH_RX_BLK(addr));
		}
	}
#endif
	rtos_task_delete(NULL);
}

void eth_init(void)
{
	int ret = RTK_FAIL;

	RTK_LOGI(TAG, "Initializing Ethernet Driver...\n");

	/* 1. Pins */
	eth_config_pins();

	/* 2. Configure Ethernet PHY and MAC clocks. */
	ret = eth_config_clocks();
	if (ret != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Ethernet clock config failed, Ethernet not started\n");
		return;
	}

	/* 3. Start Background Tasks */
	if (RTK_SUCCESS != rtos_task_create(NULL, "ETH_INIT", eth_init_thread, NULL, 1536, 2)) {
		RTK_LOGE(TAG, "Create ETH_INIT task failed!\n");
	}
}
