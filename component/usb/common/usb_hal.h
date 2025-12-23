/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USB_HAL_H
#define USB_HAL_H

/* Includes ------------------------------------------------------------------*/

#include "usb_os.h"
#include "usb_ch9.h"
#include "basic_types.h"
#include "ameba_soc.h"

#define USB_IN_TOKEN_QUEUE_DEPTH         8U  /**< Depth of the IN token queue for periodic transfers (Shared FIFO mode only). */

/**
 * @brief Defines the operational speeds for the USB controller.
 */
typedef enum {
	USB_SPEED_HIGH = 0,                 /**< High Speed (480 Mbps). */
	USB_SPEED_HIGH_IN_FULL,             /**< High Speed core running in Full Speed mode. */
	USB_SPEED_LOW,                      /**< Low Speed (1.5 Mbps). */
	USB_SPEED_FULL                      /**< Full Speed (12 Mbps). */
} usb_speed_type_t;

/**
 * @brief USB mode.
 */
typedef enum {
	USB_OTG_MODE_DEVICE = 0,            /**< USB device mode */
	USB_OTG_MODE_HOST                   /**< USB host mode */
} usb_otg_mode_t;

/**
 * @brief Defines the status of the USB device bus lines (D+ and D-).
 */
typedef enum {
	USB_DEV_BUS_STATUS_DN = BIT0,       /**< D- line status bit. */
	USB_DEV_BUS_STATUS_DP = BIT1,       /**< D+ line status bit. */
	USB_DEV_BUS_STATUS_SUSPEND = BIT2,  /**< Suspend indication bit. */
} usb_dev_bus_state_t;

/**
 * @brief Structure to hold USB calibration data for a PHY register.
 */
typedef struct {
	u8 page;        /**< Page number for the PHY register. */
	u8 addr;        /**< Register address. */
	u8 val;         /**< Value to be written to the register. */
} usb_cal_data_t;

/**
 * @brief Structure defining the platform-specific HAL driver interface.
 * @details This structure holds pointers to functions that are implemented
 *          at the platform level to control the USB hardware.
 */
typedef struct {
	/**
	 * @brief Initializes the underlying SoC-specific hardware for the USB controller.
	 * @param[in] mode: The USB mode to initialize, see @ref usb_otg_mode_t.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* init)(u8 mode);

	/**
	 * @brief De-initializes the underlying SoC-specific hardware.
	 * @return 0 on success, non-zero on failure.
	 */
	int(* deinit)(void);

	/**
	 * @brief Enables the USB interrupt with a specified priority.
	 * @param[in] priority: The interrupt priority to set.
	 */
	void(* enable_interrupt)(u8 priority);

	/**
	 * @brief Disables the USB interrupt.
	 */
	void(* disable_interrupt)(void);

	/**
	 * @brief Registers the ISR handler for USB interrupts.
	 * @param[in] handler: Pointer to the ISR function.
	 * @param[in] priority: The interrupt handler priority to set.
	 */
	void(* register_irq_handler)(void *handler, u8 priority);

	/**
	 * @brief Unregisters the USB ISR handler.
	 */
	void(* unregister_irq_handler)(void);

	/**
	 * @brief Retrieves the SoC-specific USB PHY calibration data for a specific USB mode.
	 * @param[in] mode: The USB mode for which to get calibration data, see ` USB_OTG_MODE_XX`
	 * @return Pointer to an array of `usb_cal_data_t` structures.
	 */
	usb_cal_data_t *(* get_cal_data)(u8 mode);

	/**
	 * @brief Clock gating process for USB power management.
	 * @param[in] ms: Time in milliseconds.
	 */
	void (*cg)(u32 ms);
} usb_hal_driver_t;

/* Exported variables --------------------------------------------------------*/

extern usb_hal_driver_t usb_hal_driver;

/* Exported functions --------------------------------------------------------*/

int usb_hal_core_init(u8 dma_enable);
void usb_hal_enable_global_interrupt(void);
void usb_hal_disable_global_interrupt(void);
u32 usb_hal_read_clear_interrupts(void);
void usb_hal_clear_interrupts(u32 interrupt);
int usb_hal_set_otg_mode(usb_otg_mode_t mode);
usb_otg_mode_t usb_hal_get_otg_mode(void);
int usb_hal_flush_tx_fifo(u32 num);
int usb_hal_flush_rx_fifo(void);
int usb_hal_set_rx_fifo(u16 size);
int usb_hal_set_np_tx_fifo(u16 size);
int usb_hal_set_ptx_fifo(u8 fifo, u16 size);
int usb_hal_write_packet(u8 *src, u8 ep_ch_num, u16 len);
int usb_hal_read_packet(u8 *dest, u8 ep_ch_num, u16 len);
int usb_hal_get_device_bus_status(u32 *bus_status);
void usb_hal_device_disconnect(u8 en);
void usb_hal_dump_registers(void);

u64 usb_hal_get_timestamp_us(void);
u32 usb_hal_get_timestamp_ms(void);
u32 usb_hal_get_time_tick(u8 speed);

#ifndef CONFIG_SUPPORT_USB_NO_PHY
int usb_hal_read_phy_register(u8 addr, u8 *value);
int usb_hal_write_phy_register(u8 addr, u8 value);
int usb_hal_calibrate(u8 mode);
int usb_hal_enable_otg(void);
#endif

#endif /* USB_HAL_H */
