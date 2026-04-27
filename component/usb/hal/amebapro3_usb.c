/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_soc.h"
#include "usb_hal.h"

/* Private defines -----------------------------------------------------------*/

#define USB_CAL_DATA_LEN									18U
static const char *const TAG = "USB";
static usb_cal_data_t usb_cal_data[USB_CAL_DATA_LEN];

/* USB OTG addon control register */
#define USB_OTG_ADDON_REG_CTRL									(USB_REG_BASE + 0x30004UL)
/* Private types -------------------------------------------------------------*/

/* Private macros ------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
#if defined(CONFIG_RL7013_FPGA)
#define USB_PHY_0599_NUM (26)
static const u8 USB_PHY_0599[USB_PHY_0599_NUM][2] = {
	//addr, val	  // hw default
	{0xF4, 0x9B}, // (0x9b) //< set page 0
	{0xE0, 0xE1}, // (0xe0)
	{0xE1, 0x5F}, // (0x2f) // reg_cmu_pi_bypass 設1,reg_cmu_prediv[1:0] 設2'b01，除2
	{0xE2, 0xB5}, // (0x39)
	{0xE3, 0x8B}, // (0x8b)
	{0xE4, 0x9B}, // (0x98)
	{0xE5, 0x5D}, // (0x5d)
	{0xE6, 0xCA}, // (0xc0)
	{0xE7, 0x91}, // (0x91)
	{0xF4, 0xBB}, // (0x9b) //<set page 1
	{0xE0, 0x25}, // (0x3d)
	{0xE1, 0xEF}, // (0x4f)
	{0xE2, 0x00}, // (0x00)
	{0xE3, 0x00}, // (0x00)
	{0xE4, 0x00}, // (0x00)
	{0xE5, 0x16}, // (0x0a)// REG_N_SSC[7:0]   d'22
	{0xE6, 0x00}, // (0x00)// REG_F_SSC[12:8]
	{0xE7, 0x00}, // (0x00)// REG_F_SSC[7:0]
	{0xF0, 0xFC}, // (0xfc)
	{0xF1, 0X0c}, // (0x8c)
	{0xF2, 0x00}, // (0x00)
	{0xF3, 0x11}, // (0x11)
	{0xF4, 0x9B}, // (0x9b) // set page 0
	{0xF5, 0x00}, // (0x00)
	{0xF6, 0x00}, // (0x00)
	{0xF7, 0x0A}  // (0x0a)
};
#endif
typedef union {
	uint32_t w;                         /*!< Type      used for word access */
	struct {
		uint32_t pwr_sav_en: 1;         /*!< bit:   0       power saving enable */
		uint32_t susp_dis: 1;           /*!< bit:   1       suspend signal disable */
		uint32_t susp_to_one: 1;        /*!< bit:   2       suspend signal to 1 */
		uint32_t rsvd: 1;               /*!< bit:   3       reserved */
		uint32_t suspendm: 1;           /*!< bit:   4       suspendm signal (RO) */
		uint32_t phy_clk_rdy: 1;        /*!< bit:   5       PHY clock ready (RO) */
		uint32_t ls: 2;                 /*!< bit:   6 to 7  line state (RO) */
		uint32_t otg_co_en: 1;          /*!< bit:   8       enable otg core */
		uint32_t phy_en: 1;             /*!< bit:   9       enable PHY */
		uint32_t gate_ls_en: 1;         /*!< bit:   10      enable gate USB line state function */
	} b;
} usbaddon_pwr_ctrl_t, *pusbaddon_pwr_ctrl_t;

#define USB_OTG_ADDON_PWR_CTRL		((volatile usbaddon_pwr_ctrl_t *) (USB_OTG_ADDON_REG_CTRL))

static int usb_chip_init(u8 mode);
static int usb_chip_deinit(void);
static usb_cal_data_t *usb_chip_get_cal_data(u8 mode);
static void usb_chip_enable_interrupt(u8 priority);
static void usb_chip_disable_interrupt(void);
static void usb_chip_register_irq_handler(void *handler, u8 priority);
static void usb_chip_unregister_irq_handler(void);

usb_hal_driver_t usb_hal_driver = {
	.init = usb_chip_init,
	.deinit = usb_chip_deinit,
	.get_cal_data = usb_chip_get_cal_data,
	.enable_interrupt = usb_chip_enable_interrupt,
	.disable_interrupt = usb_chip_disable_interrupt,
	.register_irq_handler = usb_chip_register_irq_handler,
	.unregister_irq_handler = usb_chip_unregister_irq_handler,
//#if CONFIG_USB_PM
	//.cg = usb_chip_cg,
//#endif
};

void usb_otg_select_mode(int mode)
{
	if (mode == 1) { //Setup the host mode
		AON_BASE->AON_SW_BOOT4 |= AON_SW4(1); //Bit 0 setup 1
		RTK_LOGS(TAG, RTK_LOG_INFO, "host mode\r\n");
	} else { //Setup the device mode
		AON_BASE->AON_SW_BOOT4 &= ~(AON_SW4(1)); //Bit 0 setup 0
		RTK_LOGS(TAG, RTK_LOG_INFO, "Device mode\r\n");
	}
	RTK_LOGS(TAG, RTK_LOG_INFO, "AON_BASE->AON_SW_BOOT4 %x\r\n", AON_BASE->AON_SW_BOOT4);
}

u32 usb_otg_uphy_init(u32 usb_phy_len, const u8 usb_phy_table[][2])
{
	u32 idx;
	int ret = 0;
	unsigned char *value;
	for (idx = 0; idx < usb_phy_len; idx++) {
		ret = usb_hal_write_phy_register(usb_phy_table[idx][0], usb_phy_table[idx][1]);
		if (ret != HAL_OK) {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Write phy init fail\r\n");
			break;
		}
		ret = usb_hal_read_phy_register(usb_phy_table[idx][0], value);
		if (ret == HAL_OK) {
			if (*value != usb_phy_table[idx][1]) {
				RTK_LOGS(TAG, RTK_LOG_INFO, "The phy write fail addr %x value %x wrong %x\r\n", usb_phy_table[idx][1], usb_phy_table[idx][1], *value);
				break;
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "Read phy fail %x\r\n");
			break;
		}
	}
	return ret;
} /* CmdOtgUPHYInit */

static void pro3_usb_init(void)
{

	/* BANDGAP_BASE->USB2_BG_CTRL |= (BANDGAP_BIT_BG_ON_USB2 | BANDGAP_BIT_BG_ON_MIPI | BANDGAP_BIT_MAINPWR | BANDGAP_BG(0x01) | BANDGAP_BIT_BG_LPF); */

	BANDGAP_BASE->USB2_BG_CTRL |= (BANDGAP_BIT_BG_ON_USB2 | BANDGAP_BIT_BG_ON_MIPI | BANDGAP_BIT_MAINPWR | BANDGAP_BIT_BG_LPF);

	RCC_PeriphClockCmd(NULL, APBPeriph_CBUS_SYS_MEM_CLOCK, ENABLE);


	CORE_S_BASE->SYS_USBOTG_CLK_CTRL |= CORE_S_BIT_USBOTG_CLK_DIVTRG;
	CORE_S_BASE->SYS_USBOTG_CLK_CTRL &= ~(CORE_S_MASK_USBOTG_CLK_DIVN);

	RCC_PeriphClockCmd(NULL, APBPeriph_USBOTG_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_USBOTG, NULL, ENABLE);




	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UABG_EN;
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UABG_EN;

	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UABG_EN;// 0x5000_0920[19] = 1; // USB Bandgap enable
	DelayUs(500);
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_USB_PDN33;// 0x5000_0920[20] = 1; // power down USB
	DelayUs(1000);
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_UAHV_EN;// 0x5000_0920[18] = 1; // USB analog hv power cut enable
	DelayUs(100);
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_USB_IBX2USB_EN;// 0x5000_0920[21] = 1; // enable IBX to USB
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_USBD_EN;// 0x5000_0920[16] = 1; // USB digital power cut enable

	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_ISO_USBD_EN);// 0x5000_0920[24] = 1;	// enable USB digital iso
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_ISO_USBA_EN);// 0x5000_0920[25] = 1;	// enable USB analog iso
	DelayUs(1);

	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_UTMI_CLK_EN;// 0x5000_0920[5] = 1;	// USB OTG UTMI CLK enable
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_CLK_EN;// 0x5000_0920[4] = 1;	// enable OTG clock
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_EN;// 0x5000_0920[0] = 1;	// enable OTG IP
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_OTG_RST_MUX;// 0x5000_0920[1] = 1;	// mask watch dog reset
	DelayUs(200);


	USB_OTG_ADDON_PWR_CTRL->b.phy_en = 1;				// 0x400F_0004[9] = 1;	// enable PHY
	DelayUs(200);
	/* wait for phy clock ready */
	while (!USB_OTG_ADDON_PWR_CTRL->b.phy_clk_rdy);	// polling 0x400F_0004[5]==1;	// PHY clock ready (RO)
	USB_OTG_ADDON_PWR_CTRL->b.otg_co_en = 1;			// 0x400F_0004[8] = 1;	// enable otg core
	DelayUs(200);
#if defined(CONFIG_RL7013_FPGA)
	usb_otg_uphy_init(sizeof(USB_PHY_0599) / sizeof(USB_PHY_0599[0]), USB_PHY_0599);
#else
	//TODO For Asci
#endif

	return;
} /* RL7013_USB_pre_init */
///////////////

/* Private functions ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Get USB chip specific calibration data
  * @param  mode: 0 - device; 1 - host
  * @retval Pointer to calibration data buffer
  */

static usb_cal_data_t *usb_chip_get_cal_data(u8 mode)
{
	usb_cal_data_t *data = NULL;

	UNUSED(mode);

	if (mode == 1) {
		data = &usb_cal_data[0];
		data->page = 0x01;
		data->addr = 0xF7;
		data->val = 0x47;
		data = &usb_cal_data[1];
		data->page = 0xFF;
		data->addr = 0x00;
		data->val = 0x00;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Host mode\r\n");
	} else {
		data = &usb_cal_data[0];
		data->page = 0x01;
		data->addr = 0xF7;
		data->val = 0x44;
		data = &usb_cal_data[1];
		data->page = 0x01;
		data->addr = 0xE0;
		data->val = 0x26;
		data = &usb_cal_data[2];
		data->page = 0x02;
		data->addr = 0xE7;
		data->val = 0x40;
		data = &usb_cal_data[3];
		data->page = 0xFF;
		data->addr = 0x00;
		data->val = 0x00;
		RTK_LOGS(TAG, RTK_LOG_INFO, "Device mode\r\n");
	}
	data = &usb_cal_data[0];

	return data;
}

/**
  * @brief  USB chip specific initialization
  * @param  void
  * @retval Status
  */

static int usb_chip_init(u8 mode)
{
	UNUSED(mode);
	pro3_usb_init();
	return HAL_OK;
}

static void
pro3_usb_deinit(
	void
)
{
	USB_OTG_ADDON_PWR_CTRL->b.otg_co_en = 0;        // 0x6024_0004[8] = 0;  // disable otg core
	USB_OTG_ADDON_PWR_CTRL->b.phy_en = 0;           // 0x6024_0004[9] = 0;  // disable PHY
	DelayMs(5);
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_OTG_EN);            // 0x6025_0120[0] = 1;  // disable OTG IP
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_OTG_CLK_EN);        // 0x6025_0120[4] = 0;  // disable OTG clock
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_ISO_USBA_EN;       // 0x6025_0120[25] = 1; // enable USB analog iso
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_SYS_ISO_USBD_EN;           // 0x6025_0120[24] = 1; // enable USB digital iso
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_USBD_EN);        // 0x6025_0120[16] = 0; // USB digital power cut disable
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_USB_IBX2USB_EN);        // 0x6025_0120[21] = 0; // disable IBX to USB
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_UAHV_EN);         // 0x6025_0120[18] = 0; // USB analog hv power cut disable
	CORE_S_BASE->SYS_OTG_CTRL |= CORE_S_BIT_USB_PDN33;               // 0x6025_0120[20] = 1; // power down USB
	CORE_S_BASE->SYS_OTG_CTRL &= ~(CORE_S_BIT_SYS_UABG_EN);             // 0x6025_0120[19] = 0; // USB Bandgap disable
	return;
} /* RL7013_USB_post_deinit */

/**
  * @brief  USB chip specific deinitialization
  * @param  void
  * @retval Status
  */

static int usb_chip_deinit(void)
{
	pro3_usb_deinit();
	RTK_LOGS(TAG, RTK_LOG_INFO, "usb deinit\r\n");
	return HAL_OK;
}

/**
  * @brief  Enable USB interrupt
  * @param  priority: IRQ priority
  * @retval void
  */
static void usb_chip_enable_interrupt(u8 priority)
{
	UNUSED(priority);
	InterruptEn(USB_IRQ, priority);
}

/**
  * @brief  Disable USB interrupt
  * @retval void
  */
static void usb_chip_disable_interrupt(void)
{
	InterruptDis(USB_IRQ);
}

/**
  * @brief  Register USB IRQ handler
  * @param  handler: IRQ handler
  * @param  priority: IRQ priority
  * @retval void
  */
static void usb_chip_register_irq_handler(void *handler, u8 priority)
{
	if (handler != NULL) {
		InterruptRegister((IRQ_FUN)handler, USB_IRQ, NULL, priority);
	}
}

/**
  * @brief  Unregister USB IRQ handler
  * @retval void
  */
static void usb_chip_unregister_irq_handler(void)
{
	InterruptUnRegister(USB_IRQ);
}