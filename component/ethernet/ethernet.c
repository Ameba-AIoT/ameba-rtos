#include <cmsis.h>
#include "build_info.h"
#include "lwip_netconf.h"
#include "ethernet_api.h"
#include "wifi_api.h"
#include "wifi_intf_drv_to_lwip.h"
#include "ethernet.h"
#include "ethernet_ex_api.h"
#include "timer_api.h"
#include "kv.h"
#include "wifi_hal_eeprom.h"
static const char *const TAG = "ETH";

#define ETH_TX_DESC_CNT         8
#define ETH_RX_DESC_CNT         16

/* Configuration Flags */
int eth_dhcp_mode = 1;
int eth_is_default_if = 1;
volatile int eth_link_is_up = 0;

ETH_InitTypeDef eth_handle;

/* DMA Descriptors (Must be non-cacheable & 32-byte aligned) */
SRAM_NOCACHE_DATA_SECTION
static u8 eth_tx_desc_arr[ETH_TX_DESC_CNT][ETH_TX_DESC_SIZE] __attribute__((aligned(32)));

SRAM_NOCACHE_DATA_SECTION
static u8 eth_rx_desc_arr[ETH_RX_DESC_CNT][ETH_RX_DESC_SIZE] __attribute__((aligned(32)));

/* OS sema/mutex */
static rtos_mutex_t eth_tx_mutex;
static rtos_sema_t  eth_link_sema;
static rtos_sema_t  eth_rx_sema;
static rtos_sema_t  eth_init_done_sema;

/* Callbacks */
eth_rx_prehandler_t eth_rx_preprocess_cb = NULL;
link_up_down_callback eth_link_cb = NULL;

void eth_register_rx_patch(eth_rx_prehandler_t pfunc1)
{
	eth_rx_preprocess_cb = pfunc1;
}
/**
 * @brief  Configure Ethernet peripheral clock.
 */
static void eth_config_mac_clock(void)
{
	u8 gmac_ckd;
	const SocClk_Info_TypeDef *pSocClk_Info = &SocClk_Info[0];

	/* Select a clock source that can be divided up to 50MHz */
	gmac_ckd = PLL_ClkSrcGet(pSocClk_Info->SYSPLL_CLK, pSocClk_Info->USBPLL_CLK, CLK_LIMIT_GMAC);

	if (gmac_ckd & IS_SYS_PLL) {
		RCC_PeriphClockSourceSet(GMAC, SYS_PLL);
		RCC_PeriphClockDividerFENSet(SYS_PLL_GMAC, ENABLE);
		RCC_PeriphClockDividerSet(SYS_PLL_GMAC, GET_CLK_DIV(gmac_ckd));
	} else {
		RCC_PeriphClockSourceSet(GMAC, USB_PLL);
		RCC_PeriphClockDividerFENSet(USB_PLL_GMAC, ENABLE);
		RCC_PeriphClockDividerSet(USB_PLL_GMAC, GET_CLK_DIV(gmac_ckd));
	}
	RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, ENABLE);
}
/**
 * @brief  Mac output 50M/25M clk to phy
 * @param  pin: The pin to output clk
 * @param  clk: 0xb: 50M, 0xc:25M
 * @retval None
 */
void eth_config_phy_clock(u32 pin, u8 clk)
{
	Pinmux_Config(pin, PINMUX_FUNCTION_EXT_CLK_OUT);

	u32 temp = HAL_READ32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL);
	temp |= PAD_BIT_DBG_CLK_FORCE;
	temp &= ~PAD_MASK_DBG_CLK0_SEL;
	temp |= PAD_DBG_CLK0_SEL(clk);

	HAL_WRITE32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL, temp);
}
/**
 * @brief  Configure RMII GPIO pins.
 */
static void eth_config_gpio(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Disable PHY reset pin (PA_3) to prevent accidental reset */
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = _PA_3;
	GPIO_Init(&GPIO_InitStruct);

	/* Configure RMII function pins */
	for (int i = 0; i < 8; i++) {
		Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][i], PINMUX_FUNCTION_RMII);
	}

	/* Configure MDC/MDIO */
	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][8], PINMUX_FUNCTION_RMII_MDC);
	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][9], PINMUX_FUNCTION_RMII_MDIO);

#if defined(CONFIG_MAC_OUTPUT_50M)
	eth_config_phy_clock(ETHERNET_PAD[ETHERNET_Pin_Grp][10], EXT_CLK_50M);
#elif defined(CONFIG_MAC_OUTPUT_25M)
	eth_config_phy_clock(ETHERNET_PAD[ETHERNET_Pin_Grp][10], EXT_CLK_25M);
#endif
}

int eth_send(struct pbuf *p)
{
	int ret = RTK_FAIL;
	u32 total_size = 0;
	struct pbuf *q;
	u8 *dma_buffer = NULL;

	if (p == NULL) {
		return ret;
	}

	/* Lock TX to prevent race conditions */
	rtos_mutex_take(eth_tx_mutex, MUTEX_WAIT_TIMEOUT);

	/* Get a free descriptor buffer from hardware driver */
	dma_buffer = Ethernet_GetTXPktInfo(&eth_handle);

	if (dma_buffer != NULL) {
		/* Copy data from pbuf chain to flat DMA buffer (Zero-Copy to HW) */
		for (q = p; q != NULL; q = q->next) {
			if (total_size + q->len > ETH_MAX_BUF_SIZE) {
				RTK_LOGE(TAG, "Packet too large for buffer!\n");
				break;
			}
			memcpy(dma_buffer, (void *)q->payload, q->len);
			dma_buffer += q->len;
			total_size += q->len;
		}

		/* Trigger Hardware Transmission */
		Ethernet_UpdateTXDESCAndSend(&eth_handle, total_size);
		ret = RTK_SUCCESS;
	}

	rtos_mutex_give(eth_tx_mutex);
	return ret;
}

u32 eth_isr_handler(void *data)
{
	UNUSED(data);
	u32 int_event = Ethernet_GetPendingINT();
	RTK_LOGD(TAG, "int status = %x\n", int_event);
	/* 1. RX Done */
	if (int_event & ETH_EVT_RX_DONE) {
		/* Notify RX task to process data */
		rtos_sema_give(eth_rx_sema);
		Ethernet_ClearINT(ETH_EVT_RX_DONE);
	}

	/* 2. RX Errors (Overflow/Runt) */
	if (int_event & ETH_EVT_RX_ERROR) {
		Ethernet_ClearINT(ETH_EVT_RX_ERROR);
	}

	/* 3. TX Done */
	if (int_event & ETH_EVT_TX_DONE) {
		Ethernet_ClearINT(ETH_EVT_TX_DONE);
	}

	/* 4. Link Change */
	if (int_event & ETH_EVT_LINK_CHG) {
		/* Check physical link status using HAL API */
		if (Ethernet_GetLinkStatus()) {
			eth_link_is_up = 1;
		} else {
			eth_link_is_up = 0;
		}

		/* Notify Link Monitor task */
		rtos_sema_give(eth_link_sema);
		Ethernet_ClearINT(ETH_EVT_LINK_CHG);
	}

	/* 5. RX Descriptor Unavailable (Optional) */
	if (int_event & ETH_EVT_RDU_RING1) {
		/* Wake up RX task to potentially free up descriptors */
		rtos_sema_give(eth_rx_sema);
		Ethernet_ClearINT(ETH_EVT_RDU_RING1);
	}

	return 0;
}
void eth_rx_thread(void *param)
{
	(void)param;
	u32 len = 0;
	struct pbuf *p = NULL;
	u8 *buf = NULL;
	enum prehandler_ret ret;

	while (1) {
		/* Wait for RX interrupt signal */
		if (rtos_sema_take(eth_rx_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			continue;
		}

		while (1) {
			buf = Ethernet_GetRXPktInfo(&eth_handle, &len);

			if ((len == 0) || (buf == NULL)) {
				/* Ring buffer empty */
				break;
			}

			if (len > 2) {
				/* Optional: Custom Pre-processing for test*/
				if (eth_rx_preprocess_cb) {
					ret = eth_rx_preprocess_cb(&p, buf, len);
					if (ret == UPLOAD_TO_LWIP && p != NULL) {
						ethernetif_rmii_netif_recv(p);
					} else if (p != NULL) {
						pbuf_free(p);
						p = NULL;
					}
				} else {
					/* Standard Path: Copy to pbuf and send to LwIP */
					/* Note: len-2 excludes CRC usually, depending on HW setting */
					p = ethernetif_rmii_buf_copy(len - 2, buf);
					if (p != NULL) {
						ethernetif_rmii_netif_recv(p);
					}
				}

				/* Return descriptor to hardware */
				Ethernet_UpdateRXDESC(&eth_handle);
			}
		}
	}

	rtos_sema_delete(eth_rx_sema);
	rtos_task_delete(NULL);
}

/**
 * @brief  Link Status Monitor & IP Configuration Task.
 *         Handles DHCP/Static IP when cable is plugged/unplugged.
 */
static void eth_link_monitor_thread(void *param)
{
	(void)param;
	u32 dhcp_status = 0;
	uint8_t eth_iptab[4];


	if (rtos_sema_take(eth_init_done_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Ethernet Init Timeout!\n");
		goto exit;
	}

	while (1) {
		/* Wait for Link Change Event */
		if (rtos_sema_take(eth_link_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			continue;
		}

		if (eth_link_is_up) {
			RTK_LOGI(TAG, "Link Up\n");
			Ethernet_GetSpeedDuplex();
			/* 1. Notify LwIP Link is Up */
			netif_set_link_up(pnetif_eth);

			/* 2. Configure IP (Static or DHCP) */
			if (rt_kv_size("eth_ip") > 0) {
				/* Static IP Mode */
				u32_t ip = 0, gw = 0, netmask = 0xFFFFFF00;

				rt_kv_get("eth_ip", &ip, 4);
				rt_kv_get("eth_gw", &gw, 4);
				rt_kv_get("eth_netmask", &netmask, 4);

				if (gw == 0) {
					gw = (ip & 0xFFFFFF00) | 0x01; /* Guess Gateway if missing */
				}

				if (!netif_is_up(pnetif_eth)) {
					netifapi_netif_set_up(pnetif_eth);
				}

				LwIP_SetIP(NETIF_ETH_INDEX, ip, netmask, gw);
				netif_set_default(pnetif_eth);

				/* Debug Print */
				eth_iptab[3] = (uint8_t)(ip >> 24);
				eth_iptab[2] = (uint8_t)(ip >> 16);
				eth_iptab[1] = (uint8_t)(ip >> 8);
				eth_iptab[0] = (uint8_t)(ip);
				RTK_LOGI(TAG, "Static IP: %d.%d.%d.%d\n", eth_iptab[3], eth_iptab[2], eth_iptab[1], eth_iptab[0]);

			} else {
				/* DHCP Mode */
				if (eth_dhcp_mode) {
					/* This might block, better to be in state machine, but OK for now */
					dhcp_status = LwIP_IP_Address_Request(NETIF_ETH_INDEX);
				}

				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					netif_set_default(pnetif_eth);
				}
			}
		} else {
			/* Link Down Handling */
			RTK_LOGI(TAG, "Link Down\n");

			netif_set_link_down(pnetif_eth);

			LwIP_ReleaseIP(NETIF_ETH_INDEX);
		}

		/* 3. Execute User Callback */
		if (eth_link_cb) {
			eth_link_cb(eth_link_is_up);
		}
	}

exit:
	rtos_sema_delete(eth_link_sema);
	rtos_sema_delete(eth_init_done_sema);
	rtos_task_delete(NULL);
}

/**
 * @brief  Hardware Initialization Task.
 *         Allocates memory, configures PHY, sets MAC, starts ETH controller.
 */
static void eth_init_thread(void *param)
{
	(void)param;
	u8 random_byte = 0;
	u8 *hwinfo_buf;
	u8 eth_mac[6];
	/* Buffers */
	u8 *eth_tx_pkt_buf = NULL;
	u8 *eth_rx_pkt_buf = NULL;

	/* Default Fake MAC if OTP is empty */
	u8 default_mac[6] = {0x00, 0xE0, 0x4C, 0xb7, 0x23, 0x00};



	/* PHY Configuration */
	const struct eth_mdio_ops eth_mdio_bus = {
		.mdio_read  = Ethernet_ReadPhyReg,
		.mdio_write = Ethernet_WritePhyReg,
	};
	struct eth_phy_dev phy_dev = {
		.bus  = &eth_mdio_bus,
		.addr = 0x01,
		.ops  = &phy_rtl8201fr_ops,
	};

	/* Initialize LwIP Stack (One-time) */
#ifndef CONFIG_RMII_VERIFY
	if (!lwip_init_done) {
		LwIP_Init();
	}
#endif

	Ethernet_StructInit(&eth_handle, &phy_dev);

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
	eth_rx_pkt_buf = (u8 *)rtos_mem_zmalloc(ETH_RX_DESC_CNT * ETH_MAX_BUF_SIZE);

	if (eth_tx_pkt_buf == NULL || eth_rx_pkt_buf == NULL) {
		RTK_LOGE(TAG, "Memory allocation failed for descriptors!\n");
		goto error;
	}

	/* Clear Descriptor Arrays */
	memset(eth_tx_desc_arr, 0, sizeof(eth_tx_desc_arr));
	memset(eth_rx_desc_arr, 0, sizeof(eth_rx_desc_arr));

	/* Assign Buffers to Handle */
	eth_handle.ETH_TxDescNum = ETH_TX_DESC_CNT;
	eth_handle.ETH_RxDescNum = ETH_RX_DESC_CNT;
	eth_handle.ETH_TxDesc    = (ETH_TxDescTypeDef *)eth_tx_desc_arr;
	eth_handle.ETH_RxDesc    = (ETH_RxDescTypeDef *)eth_rx_desc_arr;
	eth_handle.ETH_TxPktBuf  = eth_tx_pkt_buf;
	eth_handle.ETH_RxPktBuf  = eth_rx_pkt_buf;

	/* MAC Address Logic */
	hwinfo_buf = (u8 *)rtos_mem_zmalloc(OTP_LMAP_LEN);
	if (hwinfo_buf) {
		OTP_LogicalRead(hwinfo_buf, 0, OTP_LMAP_LEN);
		memcpy(eth_mac, &hwinfo_buf[EEPROM_MAC_ADDR], ETH_ALEN);

		/* Check if MAC is invalid (all 0xFF) */
		if (!memcmp(&hwinfo_buf[EEPROM_MAC_ADDR], "\xff\xff\xff\xff\xff\xff", ETH_ALEN)) {
			/* Generate Random MAC based on default prefix */
			random_byte = (u8)(_rand() % 1000) & 0xFE;
			default_mac[5] = random_byte;
			memcpy(eth_mac, default_mac, 6);
		} else {
			/* Apply SoftAP offset logic (Realtek specific) */
			if (wifi_user_config.softap_addr_offset_idx == 0) {
				eth_mac[wifi_user_config.softap_addr_offset_idx] += (3 << 1);
			} else {
				eth_mac[wifi_user_config.softap_addr_offset_idx] += 3;
			}
		}
		rtos_mem_free(hwinfo_buf);
	} else {
		RTK_LOGE(TAG, "OTP malloc fail, using default MAC\n");
		memcpy(eth_mac, default_mac, 6);
	}

	/* Set MAC in Handle and LwIP struct */
	memcpy((void *)(eth_handle.ETH_MacAddr), (void *)eth_mac, ETH_MAC_ADDR_LEN);
	memcpy((void *)pnetif_eth->hwaddr, (void *)eth_mac, ETH_MAC_ADDR_LEN);

	Ethernet_Init(&eth_handle);

	/* Interrupt Init */
	Ethernet_ConfigINT(ETH_EVT_RX_DONE | ETH_EVT_TX_DONE | ETH_EVT_LINK_CHG | ETH_EVT_RX_ERROR | ETH_EVT_TX_ERROR | ETH_EVT_RDU_RING1, ENABLE);
	InterruptRegister((IRQ_FUN)eth_isr_handler, (IRQn_Type)RMII_IRQ, (u32)&eth_handle, 5);
	InterruptEn(RMII_IRQ, 5);
	/* Sync Objects Creation */
	rtos_sema_create_binary(&eth_rx_sema);
	rtos_mutex_create(&eth_tx_mutex);

#ifndef CONFIG_RMII_VERIFY
	netif_set_up(pnetif_eth);
#endif

	/* Signal Init Done */
	rtos_sema_give(eth_init_done_sema);

	if (RTK_SUCCESS != rtos_task_create(NULL, "ETH_RX", eth_rx_thread, NULL, 512, 5)) {
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
	rtos_task_delete(NULL);
}

void eth_init(void)
{
	RTK_LOGI(TAG, "Initializing Ethernet Driver...\n");

	/* 1. Hardware Pin Config */
	eth_config_gpio();

	/* 2. Clock Config */
	eth_config_mac_clock();

	/* 3. Initialize Synchronization */
	eth_is_default_if = 1;
	rtos_sema_create_binary(&eth_init_done_sema);
	rtos_sema_create_binary(&eth_link_sema);

	/* 4. Start Background Tasks */
	if (RTK_SUCCESS != rtos_task_create(NULL, "ETH_INIT", eth_init_thread, NULL, 1536, 2)) {
		RTK_LOGE(TAG, "Create ETH_INIT task failed!\n");
	}
#ifndef CONFIG_RMII_VERIFY
	if (RTK_SUCCESS != rtos_task_create(NULL, "ETH_LINK", eth_link_monitor_thread, NULL, 1536, 3)) {
		RTK_LOGE(TAG, "Create ETH_LINK task failed!\n");
	}
#endif
}