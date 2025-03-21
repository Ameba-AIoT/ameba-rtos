/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef USBH_VERIFY_H
#define USBH_VERIFY_H

/* Includes ------------------------------------------------------------------*/

#include "usbh.h"

/* Exported defines ----------------------------------------------------------*/

/* Verify Class Codes */
#define VERIFY_CLASS_CODE					0xFFU
/* Verify SubClass Codes */
#define VERIFY_SUBCLASS_CODE				0x00U
/* Verify Protocol */
#define VERIFY_PROTOCOL						0x00U

#define VERIFY_CTRL_REQUEST_TYPE			(0x00)

//#define USBH_VERIFY_PIPE_COUNT			(16)
#define TRANSFER_DATA_VALUE_BASE			(0x00)

/* Exported types ------------------------------------------------------------*/
#define USBH_FORCE_IN_EXIT_SOF_COUNT_MAX	(100)
#define USBH_EP_COUNT_MAX					(10)
#define USBH_PIPE_VALID_VALUE				(0xFFU)

#define USBH_MAIN_PROCESS_MONITOR_COUNT		(1000*10000)

#define PIPE_IS_VALID(pipe)					(pipe != USBH_PIPE_VALID_VALUE)
#define USBH_VERIFY_FIFO_SIZE_ALIGNED4B(x)	((((u32)x + 3U) / 4U) * 4U ) /*FIFO read data set u32 as basic data length*/

/* USB Host Status */
typedef enum {
	USBH_VERIFY_IDLE = 0U,
	USBH_VERIFY_DETACHED,
	USBH_VERIFY_ATTACH,
	USBH_VERIFY_SETUP,
	USBH_VERIFY_MAX,
} usbh_verify_state_t;

/* States for Verify transfer Status */
typedef enum {
	VERIFY_STATE_IDLE = 0U,
	VERIFY_STATE_XFER,
	VERIFY_STATE_BUSY,
	VERIFY_STATE_ERROR,
} usbh_verify_ep_state_t;

/* USB Host transfer Status */
typedef enum {
	USBH_VERIFY_TRX_IDLE = 0U,
	USBH_VERIFY_TX,
	USBH_VERIFY_RX,
	USBH_VERIFY_TRX_MAX,
} usbh_verify_ep_trx_state_t;

/* Verify user callback interface */
typedef struct {
	int(* detach)(void);
	int(* setup)(void);
} usbh_verify_cb_t;

typedef struct {
	__IO usbh_verify_ep_state_t	state;
	u8                  init_done;
	u8                  pipe_id;

	usbh_verify_ep_trx_state_t ep_state;
	u8                  ep_match_addr;
	u32                 retry_time;

	/* descriptor infor*/
	u8                  ep_addr;
	u8                  ep_type;
	u16                 ep_mps; //MPS
	u32                 ep_interval;
	u16                 trans_size; //test transfer size

	/* xfer data infor */
	u8                  *xfer_bk_buf; //xfer_bk_buf = xfer_buf, used to loop send data out
	u8                  *xfer_buf;
	__IO u16            xfer_len;

	u8                  expect_data;
	__IO u32            error_cnt;//transfer data total mismatch count
	__IO u32            xfer_cnt;//transfer success count
	u32                 total_xfer_cnt;//transfer total success count


	//idle to retry,if idle too long, force to switch to xfer
	u16                 idle_count;

	__IO u32            tick;//period task time tick

	u32                 test_mask;
} usbh_verify_xfer_t;


/* Verify Class host */
typedef struct {
	usbh_verify_state_t  usbh_state;//host status

	/* start next loop for process */
	__IO u8             next_xfer;

	__IO u8             start_flag;
	__IO u32            finish_tick;

	/* ctrl loop info*/
	usbh_verify_xfer_t  ctrl_in_xfer;
	usbh_verify_xfer_t  ctrl_out_xfer;

	usbh_verify_ep_trx_state_t ctrl_trx_state;

	u8                  ep_count;
	usbh_verify_xfer_t  ep_array[USBH_EP_COUNT_MAX];

//	u8                  host_pipe[USBH_VERIFY_PIPE_COUNT];

	u8                  interface_id;//bInterfaceNumber
	u8                  alterset_id;//bAlternateSetting

	__IO u32            ep_mask;
	u32                 ep_all_mask;
	u32                 test_count_limit;
	u32                 loopcount;
	usbh_verify_cb_t    *cb;
	usb_host_t          *host;
} usbh_verify_host_t;


/* Exported macros -----------------------------------------------------------*/
typedef struct {
	/* Both direction xfer:
	 * 	loopback = 1: OUT->IN, OUT must OK, and then switch to IN.
	 * 	loopback = 0: start another OUT or IN when this OUT or IN done.
	 */
	u8 p_loopback : 1;//INTR and ISOC
	u8 bulk_loopback : 1;
	/* No loopback, single direction xfer: host only start IN or OUT xfer.
	 * In only: need USBD verify cooperate with USBH verify.
	 */
	u8 p_in_only : 1;
	u8 p_out_only : 1;
	u8 bulk_in_only : 1;
	u8 bulk_out_only : 1;
	u8 ctrl_in_only : 1;
	u8 ctrl_out_only : 1;
} usbh_verify_xfer_dir_t;

/* Exported variables --------------------------------------------------------*/
extern usbh_verify_xfer_dir_t usbh_verify_xfer;

/* Use 1us dbg_timer instead of SOF interrupt for FPGA USB interrupt transfer*/
extern u8 usbh_fpga_no_sof;

/* Exported functions --------------------------------------------------------*/
int usbh_verify_init(usbh_verify_cb_t *cb);
int usbh_verify_deinit(void);

int usbh_verify_ep_enable(usbh_verify_xfer_t *xfer);
void usbh_verify_ep_disable(usb_host_t *host, usbh_verify_xfer_t *xfer);

void usbh_verify_wait_enum_attach(void);
void usbh_verify_start_process(void);
void usbh_verify_set_test_count_max(u32 count);

int usbh_verify_xfer_process(usbh_verify_xfer_t *xfer);
void usbh_verify_transfer_by_eptype(u16 size, u8 type);
void usbh_verify_all_ep_transfer(u16 size);
void usbh_ctrl_test(u8 is_in);

/*set interface apis*/
int usbh_verify_switch_interface(u8 if_num, u8 if_alt);
void usbh_verify_ep_status_stop(void);
void usbh_verify_ep_status_start(void);
void usbh_verify_looprun_test(u16 size);
void usbh_verify_longrun_thread_stop(void);
void usbh_verify_ep_debug_dump(void);

#endif  /* USBH_VERIFY_H */

