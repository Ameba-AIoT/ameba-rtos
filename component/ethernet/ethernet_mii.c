
#include <cmsis.h>
#include "build_info.h"
#include "lwip_netconf.h"
#include "ethernet_api.h"
#include "wifi_api.h"
#include "wifi_intf_drv_to_lwip.h"
#include "ethernet_mii.h"
#include "ethernet_ex_api.h"
#include "timer_api.h"
#include "kv.h"
#include "wifi_hal_eeprom.h"
static const char *const TAG = "RMII";


ETH_InitTypeDef eth_initstruct;


#define ETH_TXDONE              EthTxDone
#define ETH_RXDONE              EthRxDone
#define ETH_LINKUP              EthLinkUp
#define ETH_LINKDOWN            EthLinkDown

#define ETH_LINKCHANGE          8

#define MII_TX_DESC_NO					4
#define MII_RX_DESC_NO					4

SRAM_NOCACHE_DATA_SECTION
u8 rmii_tx_desc[MII_TX_DESC_NO][ETH_TX_DESC_SIZE]__attribute__((aligned(32)));
SRAM_NOCACHE_DATA_SECTION
u8 rmii_rx_desc[MII_RX_DESC_NO][ETH_RX_DESC_SIZE]__attribute__((aligned(32)));

static rtos_mutex_t rmii_tx_mutex;
volatile u32 ethernet_unplug = 0;

extern struct netif  *pnetif_eth;

static u8 *pTmpTxPktBuf = NULL;
static u8 *pTmpRxPktBuf = NULL;

int dhcp_ethernet_mii = 1;
int ethernet_if_default = 1;
int link_is_up = 0;


link_up_down_callback p_link_change_callback = 0;

extern int lwip_init_done;

static rtos_sema_t mii_linkup_sema;
static rtos_sema_t mii_rx_sema;
rtos_sema_t ethernet_init_done;

typedef enum prehandler_ret(*prehandler_func_t)(struct pbuf **p, u8 *buf, u32 len);

prehandler_func_t rmii_rx_prehandler = NULL;

void mii_patch_function(prehandler_func_t pfunc1)
{
	rmii_rx_prehandler = pfunc1;
}

void mii_rx_thread(void *param)
{
	(void) param;
	u32 len = 0;
	struct pbuf *p = NULL;
	u8 *buf = NULL;
	enum prehandler_ret ret;

	while (1) {
		if (rtos_sema_take(mii_rx_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "%s, Take Semaphore Fail\n", __FUNCTION__);
			break;
		}
		// continues read the rx ring until its empty
		while (1) {
			buf = Ethernet_GetRXPktInfo(&eth_initstruct, &len);
			if (len > 2) {
				if (rmii_rx_prehandler) {
					ret = rmii_rx_prehandler(&p, buf, len);
					if (ret == UPLOAD_TO_LWIP && p != NULL) {
						ethernetif_rmii_netif_recv(p);
					} else if (p != NULL) {
						pbuf_free(p); // 释放未上传的 pbuf
						p = NULL;
					}
				} else {
					p = ethernetif_rmii_buf_copy(len - 2, buf);
					if (p != NULL) {
						ethernetif_rmii_netif_recv(p);
					}
				}
				Ethernet_UpdateRXDESC(&eth_initstruct);

			} else if (len == 0) {
				break;
			}
		}
	}

	rtos_sema_delete(mii_rx_sema);
	rtos_task_delete(NULL);
}

void mii_intr_thread(void *param)
{
	(void) param;
	u32 dhcp_status = 0;
	uint8_t eth_iptab[4];

	while (1) {

#ifdef CONFIG_AMEBAGREEN2
		if (rtos_sema_take(mii_linkup_sema, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "%s, Take Semaphore Fail\n", __FUNCTION__);
			break;
		}
#endif

		if (rtos_sema_take(ethernet_init_done, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "%s, Take Semaphore Fail\n", __FUNCTION__);
			break;
		}

		if (link_is_up) {
			netif_set_link_up(pnetif_eth);
			if (rt_kv_size("eth_ip") > 0) {
				u32_t ip, gw = 0, netmask = 0;
				rt_kv_get("eth_ip", &ip, 4);
				rt_kv_get("eth_gw", &gw, 4);
				rt_kv_get("eth_netmask", &netmask, 4);
				if (!netif_is_up(pnetif_eth)) {
					netifapi_netif_set_up(pnetif_eth);
				}
				LwIP_SetIP(NETIF_ETH_INDEX, ip, netmask, gw);
				netif_set_default(pnetif_eth);
				eth_iptab[3] = (uint8_t)(ip >> 24);
				eth_iptab[2] = (uint8_t)(ip >> 16);
				eth_iptab[1] = (uint8_t)(ip >> 8);
				eth_iptab[0] = (uint8_t)(ip);
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n\rSet Ethernet static IP : %d.%d.%d.%d\n", eth_iptab[3], eth_iptab[2], eth_iptab[1], eth_iptab[0]);
			} else {
				if (dhcp_ethernet_mii == 1) {
					dhcp_status = LwIP_IP_Address_Request(NETIF_ETH_INDEX);
				}

				if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
					if (1 == ethernet_if_default) {
						netif_set_default(pnetif_eth);    //Set default gw to ether netif
					} else {
						netif_set_default(pnetif_eth);
					}
				}
			}
			break;
		} else {
			netif_set_link_down(pnetif_eth);
			netif_set_default(pnetif_eth);
			LwIP_ReleaseIP(NETIF_ETH_INDEX);
		}

		if (p_link_change_callback) {
			p_link_change_callback(link_is_up);
		}
	}

	rtos_sema_delete(mii_linkup_sema);
	rtos_sema_delete(ethernet_init_done);
	rtos_task_delete(NULL);
}


void mii_intr_handler(u32 Event, u32 Data)
{
	(void) Data;
	switch (Event) {
	case ETH_TXDONE:
		// RTK_LOGI(TAG, "ETH_TXDONE\n");
		break;
	case ETH_RXDONE:
		// RTK_LOGI(TAG, "ETH_RXDONE\n");
		// wake up rx thread to receive data
		rtos_sema_give(mii_rx_sema);
		break;
	case ETH_LINKCHANGE:
		RTK_LOGI(TAG, "Link change\n");
		link_is_up = 1;
		rtos_sema_give(mii_linkup_sema);
		break;
	case ETH_LINKUP:
		RTK_LOGI(TAG, "Link Up\n");
		link_is_up = 1;
		rtos_sema_give(mii_linkup_sema);
		break;
	case ETH_LINKDOWN:
		RTK_LOGI(TAG, "Link Down\n");
		link_is_up = 0;
		rtos_sema_give(mii_linkup_sema);
		break;
	default:
		RTK_LOGI(TAG, "Unknown event !!\n");
		break;
	}
}


void ethernet_demo(void *param)
{
	(void) param;
	ETH_InitTypeDef *peth_initstruct = &eth_initstruct;
	u8 sMacAddr[6] = {0x00, 0xE0, 0x4C, 0xb7, 0x23, 0x00};
	u8 eth_mac[6];
	u8 random = 0;
	u8 *hwinfo;

	/* Initilaize the LwIP stack */
	// can not init twice
#ifndef CONFIG_RMII_VERIFY
	if (!lwip_init_done) {
		LwIP_Init();
	}
#endif
	Ethernet_StructInit(&eth_initstruct);

	/*ethernet_irq_hook*/
	peth_initstruct->callback = (eth_callback_t)mii_intr_handler;

	if (pTmpTxPktBuf) {
		free(pTmpTxPktBuf);
		pTmpTxPktBuf = NULL;
	}
	if (pTmpRxPktBuf) {
		free(pTmpRxPktBuf);
		pTmpRxPktBuf = NULL;
	}

	pTmpTxPktBuf = (u8 *)rtos_mem_zmalloc(/*MII_TX_DESC_CNT*/MII_TX_DESC_NO * ETH_PKT_BUFF_SZ);
	pTmpRxPktBuf = (u8 *)rtos_mem_zmalloc(/*MII_RX_DESC_CNT*/MII_RX_DESC_NO * ETH_PKT_BUFF_SZ);


	if (pTmpTxPktBuf == NULL || pTmpRxPktBuf == NULL) {
		RTK_LOGE(TAG, "TX/RX descriptor malloc fail\n");
		return;
	}
	memset(rmii_tx_desc, 0, MII_TX_DESC_NO * ETH_TX_DESC_SIZE);
	memset(rmii_rx_desc, 0, MII_RX_DESC_NO * ETH_RX_DESC_SIZE);

	peth_initstruct->ETH_TxDescNum = MII_TX_DESC_NO;
	peth_initstruct->ETH_RxDescNum = MII_RX_DESC_NO;

	peth_initstruct->ETH_TxDesc = (ETH_TxDescTypeDef *)rmii_tx_desc;
	peth_initstruct->ETH_RxDesc = (ETH_RxDescTypeDef *)rmii_rx_desc;
	peth_initstruct->ETH_TxPktBuf = (u8 *)pTmpTxPktBuf;
	peth_initstruct->ETH_RxPktBuf = (u8 *)pTmpRxPktBuf;

	hwinfo = (u8 *)rtos_mem_zmalloc(OTP_LMAP_LEN);
	OTP_LogicalMap_Read(hwinfo, 0, OTP_LMAP_LEN);

	memcpy(eth_mac, &hwinfo[EEPROM_MAC_ADDR], ETH_ALEN);

	if (!memcmp(&hwinfo[EEPROM_MAC_ADDR], "\xff\xff\xff\xff\xff\xff", ETH_ALEN)) {
		random = (u8)(_rand() % 1000) & 0xFE;
		sMacAddr[5] = random;
		for (int i = 0; i < 6; i++) {
			eth_mac[i] = sMacAddr[i];
		}
	} else {
		if (wifi_user_config.softap_addr_offset_idx == 0) {
			eth_mac[wifi_user_config.softap_addr_offset_idx] += (3 << 1);
		} else {
			eth_mac[wifi_user_config.softap_addr_offset_idx] += 3;
		}
	}

	memcpy((void *)(peth_initstruct->ETH_MacAddr), (void *)eth_mac, ETH_MAC_ADDR_LEN);

	InterruptRegister((IRQ_FUN)RMII_IRQHandler, (IRQn_Type)RMII_IRQ, (u32)&eth_initstruct, 5);
	InterruptEn(RMII_IRQ, 5);

	Ethernet_init(&eth_initstruct);

	memcpy((void *)pnetif_eth->hwaddr, (void *)eth_mac, ETH_MAC_ADDR_LEN);

	rtos_sema_create_binary(&mii_rx_sema);
	rtos_mutex_create(&rmii_tx_mutex);

#ifndef CONFIG_RMII_VERIFY
	netif_set_up(pnetif_eth);
#endif

	rtos_sema_give(ethernet_init_done);

	if (RTK_SUCCESS != rtos_task_create(NULL, "mii_rx_thread", mii_rx_thread, NULL, 1024, 5)) {
		RTK_LOGE(TAG, "\n\r%s xTaskCreate(mii_rx_thread) failed", __FUNCTION__);
	}

	rtos_mem_free(hwinfo);

	rtos_task_delete(NULL);
}

int ethernet_is_unplug(void)
{
	if (ethernet_unplug == 1) {
		return TRUE;
	} else {
		return FALSE;
	}
}

void ethernet_pin_config(void)
{
#ifdef CONFIG_AMEBAGREEN2
	/*disable phy reset pin*/
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_Pin = _PA_3;
	GPIO_Init(&GPIO_InitStruct);

	/*config fid*/
	for (int pinid = 0; pinid < 8; pinid++) {
		Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][pinid], PINMUX_FUNCTION_RMII);
	}

	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][8], PINMUX_FUNCTION_RMII_MDC);
	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][9], PINMUX_FUNCTION_RMII_MDIO);

#ifndef CONFIG_PHY_INT_XTAL
	Pinmux_Config(ETHERNET_PAD[ETHERNET_Pin_Grp][10], PINMUX_FUNCTION_EXT_CLK_OUT);

	/*phy uses external clk*/
	u32 temp = HAL_READ32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL);
	temp |= PAD_BIT_DBG_CLK_FORCE;
	temp &= ~PAD_MASK_DBG_CLK0_SEL;
#ifdef CONFIG_MAC_OUTPUT_50M
	temp |= PAD_DBG_CLK0_SEL(EXT_CLK_50M);
#else
	temp |= PAD_DBG_CLK0_SEL(EXT_CLK_25M);
#endif

	HAL_WRITE32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL, temp);
#endif
#endif
}

void ethernet_clock_set(void)
{
	u8 gmac_ckd;

	const SocClk_Info_TypeDef *pSocClk_Info = &SocClk_Info[0];
	const u32 usb_pll_clk = pSocClk_Info->USBPLL_CLK;
	const u32 sys_pll_clk = pSocClk_Info->SYSPLL_CLK;

	/*Select a clock source that can be divided up to 50MHz.*/
	gmac_ckd = PLL_ClkSrcGet(sys_pll_clk, usb_pll_clk, CLK_LIMIT_GMAC);
	if (gmac_ckd & IS_SYS_PLL) {
		RCC_PeriphClockSourceSet(GMAC, SYS_PLL);
		RCC_PeriphClockDividerFENSet(SYS_PLL_GMAC, ENABLE);
		RCC_PeriphClockDividerSet(SYS_PLL_GMAC, GET_CLK_DIV(gmac_ckd));
	} else {
		RCC_PeriphClockSourceSet(PSRAM, USB_PLL);
		RCC_PeriphClockDividerFENSet(USB_PLL_GMAC, ENABLE);
		RCC_PeriphClockDividerSet(USB_PLL_GMAC, GET_CLK_DIV(gmac_ckd));
	}
}


void ethernet_mii_init(void)
{
	/*only test ok for 7005*/
	ethernet_pin_config();
	/*Ennale GMAC Function and Clock */
	RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, ENABLE);

	ethernet_clock_set();

	// set the ethernet interface as default
	ethernet_if_default = 1;
	rtos_sema_create_binary(&ethernet_init_done);
	rtos_sema_create_binary(&mii_linkup_sema);

#ifndef CONFIG_RMII_VERIFY
	if (RTK_SUCCESS != rtos_task_create(NULL, "DHCP_START_MII", mii_intr_thread, NULL, 2048, 3)) {
		RTK_LOGE(TAG, "\n\r%s Create simulation_task Err!");
	}
#endif
	if (RTK_SUCCESS != rtos_task_create(NULL, "ETHERNET DEMO", ethernet_demo, NULL, 2048, 2)) {
		RTK_LOGE(TAG, "\n\r%s Create simulation_task Err!!");
	}

}

int rltk_mii_send(struct pbuf *p)
{
	u32 ret = 0;
	u32 size = 0;
	struct pbuf *q;
	u8 *rmii_buf = NULL;

	rtos_mutex_take(rmii_tx_mutex, MUTEX_WAIT_TIMEOUT);
	rmii_buf = Ethernet_GetTXPktInfo(&eth_initstruct);
	if (rmii_buf) {/*copy from lwip to rmii*/
		for (q = p; q != NULL; q = q->next) {
			memcpy(rmii_buf, (void *)q->payload, q->len);
			rmii_buf += q->len;
			size += q->len;
		}
		Ethernet_UpdateTXDESCAndSend(&eth_initstruct, size);
		ret = 0;
	} else {
		ret = -1;
	}

	rtos_mutex_give(rmii_tx_mutex);

	return ret;
}
