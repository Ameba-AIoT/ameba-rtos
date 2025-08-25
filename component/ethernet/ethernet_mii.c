
#include <cmsis.h>
#include "build_info.h"
#include "lwip_netconf.h"
#include "ethernet_api.h"
#include "wifi_api.h"
#include "wifi_intf_drv_to_lwip.h"
#include "ethernet_mii.h"
#include "ethernet_ex_api.h"
#include "timer_api.h"
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

extern struct netif  xnetif[NET_IF_NUM];
extern struct netif  eth_netif;

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
u8 mac_id[6] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC};

extern u8 ETHERNET_Pin_Grp;
extern const u8 ETHERNET_PAD[4][11];

typedef enum prehandler_ret(*prehandler_func_t)(struct pbuf **p, u8 *buf, u32 len);

prehandler_func_t rmii_rx_prehandler = NULL;

void mii_patch_function(prehandler_func_t (*pfunc1))
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
			netif_set_link_up(&eth_netif);
			if (dhcp_ethernet_mii == 1) {
				dhcp_status = LwIP_DHCP(ETHERNET_IDX, DHCP_START);
			}

			if (DHCP_ADDRESS_ASSIGNED == dhcp_status) {
				if (1 == ethernet_if_default) {
					netif_set_default(&eth_netif);    //Set default gw to ether netif
				} else {
					netif_set_default(&eth_netif);
				}
			}
			break;
		} else {
			netif_set_link_down(&eth_netif);
			netif_set_default(&eth_netif);
#if defined(CONFIG_LWIP_LAYER) && CONFIG_LWIP_LAYER
			LwIP_ReleaseIP(ETHERNET_IDX);
#endif
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

	memcpy((void *)(peth_initstruct->ETH_MacAddr), (void *)mac_id, ETH_MAC_ADDR_LEN);

	InterruptRegister((IRQ_FUN)RMII_IRQHandler, (IRQn_Type)RMII_IRQ, (u32)&eth_initstruct, 5);
	InterruptEn(RMII_IRQ, 5);

	Ethernet_init(&eth_initstruct);

	memcpy((void *)eth_netif.hwaddr, (void *)mac_id, ETH_MAC_ADDR_LEN);

	rtos_sema_create_binary(&mii_rx_sema);
	rtos_mutex_create(&rmii_tx_mutex);

#ifndef CONFIG_RMII_VERIFY
	netif_set_up(&eth_netif);
#endif

	rtos_sema_give(ethernet_init_done);

	if (RTK_SUCCESS != rtos_task_create(NULL, "mii_rx_thread", mii_rx_thread, NULL, 1024, 5)) {
		RTK_LOGE(TAG, "\n\r%s xTaskCreate(mii_rx_thread) failed", __FUNCTION__);
	}

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

void ethernet_pincig(void)
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
#ifdef CONFIG_MAC_EXT_50M
	temp |= PAD_DBG_CLK0_SEL(EXT_CLK_50M);
#else
	temp |= PAD_DBG_CLK0_SEL(EXT_CLK_25M);
#endif

	HAL_WRITE32(PINMUX_REG_BASE, REG_PINMUX_SUB_CTRL, temp);
#endif
#endif
}

void ethernet_setclk_to_50m(void)
{
#ifdef CONFIG_AMEBAGREEN2
	uint32_t clk_source;
	uint32_t pll_clk;
	uint32_t div_value;

	clk_source = RCC_PeriphClockSourceGet(GMAC);

	switch (clk_source) {
	case CKSL_GMAC_EXT50M:
		return;

	case CKSL_GMAC_USB_PLL:
		// enable GMAC FENCLK
		RCC_PeriphClockDividerFENSet(USB_PLL_GMAC, ENABLE);
		/*Brought about by SOC clock structure or other configurations, DD is known*/
		RCC_PeriphClockDividerFENSet(SYS_PLL_GMAC, ENABLE);
		pll_clk = USB_PLL_ClkGet();
		break;

	case CKSL_GMAC_SYS_PLL:
		// enable GMAC FENCLK
		RCC_PeriphClockDividerFENSet(SYS_PLL_GMAC, ENABLE);

		RCC_PeriphClockDividerFENSet(USB_PLL_GMAC, ENABLE);
		pll_clk = SYS_PLL_ClkGet();
		break;

	default:
		RTK_LOGI(TAG, "Error: Unknown GMAC CLK Source 0x%x\n", clk_source);
		return;
	}

	if (pll_clk == 0) {
		RTK_LOGI(TAG, "Error: PLL Clock is 0\n");
		return;
	}
	div_value = pll_clk / 50000000;

	if (clk_source == CKSL_GMAC_USB_PLL) {
		RCC_PeriphClockDividerSet(USB_PLL_GMAC, div_value);
	} else if (clk_source == CKSL_GMAC_SYS_PLL) {
		RCC_PeriphClockDividerSet(SYS_PLL_GMAC, div_value);
	}
#endif
}


void ethernet_mii_init(void)
{
	/*only test ok for 7005*/
	ethernet_pincig();
	RCC_PeriphClockCmd(APBPeriph_GMAC, APBPeriph_GMAC_CLOCK, ENABLE);
	// RCC_PeriphClockSourceSet(GMAC, SYS_PLL);
	ethernet_setclk_to_50m();

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
