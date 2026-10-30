/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

/* uac 2.0 */
#include "usbd_uac.h"
#ifdef CONFIG_USBD_COMPOSITE
#include "usbd_composite.h"
#endif
#include "usb_uac2.h"

/* Private defines -----------------------------------------------------------*/

#define UABD_UAC_DESC_DUMP (0)
#if USBD_UAC_DEBUG
#define USBD_UAC_DEBUG_LOOP_TIME   (10U*1000U)
#define USBD_UAC_DEBUG_STEP_TIME   100U
#endif
#define UABD_UAC_VOL_ERR_VAL       255U
#define USBD_UAC_HS_ISOC_MPS                        1024U   /**< High speed ISOC IN & OUT maximum packet size */
#define USBD_UAC_FS_ISOC_MPS                        1023U   /**< Full speed ISOC IN & OUT maximum packet size */
#define USBD_UAC_FS_DEFAULT_BINTERVAL               1U      /**< FS ISOC bInterval used in EP descriptors */
#define USBD_UAC_HS_DEFAULT_BINTERVAL               1U      /**< HS ISOC bInterval used in EP descriptors */
/* 1ms 8 frame in high speed */
#define USBD_UAC_HS_SOF_COUNT_PER_MS                8U
#define USBD_UAC_ONE_KHZ                            1000U
/**
 * Audio sample frequency
 */
#define USBD_UAC_SAMPLING_FREQ_MAX_COUNT             4U
/* Period of the warning usbd_uac_ep_buf_ctrl_deinit() logs while a ring user does not leave */
#define USBD_UAC_USER_EXIT_WARN_MS                   1000U
/**
 * Defines USB Audio Headphone entity id.
 */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES            0x15U /**< Define headphone clock id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES    0x01U /**< Define headphone input terminal id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT          0x05U /**< Define headphone feature unit id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES   0x09U /**< Define headphone output terminal id. */
/**
 * Defines USB Audio Microphone entity id.
 */
#define USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE            0x12U /**< Define microphone clock id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE    0x02U /**< Define microphone input terminal id. */
#define USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT           0x08U /**< Define microphone feature unit id. */
#define USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE   0x10U /**< Define microphone output terminal id. */
/* The mic AS interface descriptor only defines alt setting 0 (zero-bandwidth) and 1 (streaming) */
#define USBD_UAC_AS_MIC_ALT_SETTING_MAX                            0x01U
/* Mic channels the descriptor templates reserve, cfg->in.ch_cnt may be 1 up to it. The mic FU in
 * both templates carries USBD_UAC_IN_MAX_CH_CNT + 1 bmaControls entries, extend them with it. */
#define USBD_UAC_IN_MAX_CH_CNT                                     USBD_UAC_CH_CNT_2
#if USBD_UAC_IN_DEFAULT_CH_CNT > USBD_UAC_IN_MAX_CH_CNT
#error "USBD_UAC_IN_DEFAULT_CH_CNT exceeds the mic channels reserved in the descriptor templates"
#endif
/* Offset of bmaControls(0) in a UAC 2.0 Feature Unit descriptor, ref UAC 2.0 4.7.2.8 */
#define USBD_UAC_AC_FU_BMA_CONTROLS_OFFSET                         5U
/* Size of one bmaControls entry in a UAC 2.0 Feature Unit descriptor */
#define USBD_UAC_AC_FU_BMA_CONTROLS_SIZE                           4U
/* AC mic entities (clock + input terminal + feature unit + output terminal) descriptor length */
#define USBD_UAC_AC_MIC_IF_LEN \
    (USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_MAX_CH_CNT) + USB_UAC2_LEN_AC_OUT_TTY_DESC)
/* len of each Audio stream interface/altsetting (one EP) */
#define USBD_UAC_AS_EIF_LEN                        (USB_LEN_IF_DESC + USB_UAC2_LEN_AS_IF_ALT_SET_DESC + USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC \
                                                   + USB_LEN_EP_DESC + USB_UAC2_LEN_DATA_EP_DESC)
/* full speed AS alt setting num: alt 0 plus one alt per channel count from 2 up to the max */
#define USBD_UAC_FS_AS_ALT_SETTING_NUM              ((USBD_UAC_DEFAULT_CH_CNT / 2U) + 1U)
/* high speed AS alt setting num */
#define USBD_UAC_HS_AS_ALT_SETTING_NUM              USBD_UAC_FS_AS_ALT_SETTING_NUM
/* microphone AS alt setting num (alt0 zero-bw + alt1 mono streaming) */
#define USBD_UAC_MIC_AS_ALT_SETTING_NUM             2U

/* Private types -------------------------------------------------------------*/

/* Record packetizer: splits the PCM stream the application submits into one packet per
   (micro)frame of the nominal size, refer to usbd_uac_transmit_data() */
typedef struct {
	u8 *buf;            /* One packet being assembled, isoc_mps bytes */
	u32 rate_rem;       /* sampling_freq % pkt_per_sec */
	u32 acc;            /* Fractional frame accumulator, in 1/pkt_per_sec frames */
	u16 pkt_per_sec;    /* Packets per second: 8000 at High Speed, 1000 at Full Speed */
	u16 base_len;       /* Nominal packet length in bytes, without the fractional frame */
	u16 fill;           /* Bytes already in buf */
	u16 target;         /* Length of the packet being assembled */
	u8 frame_len;       /* Bytes per audio frame: ch_cnt * byte_width */
} usbd_uac_rec_pktz_t;

/* Private macros ------------------------------------------------------------*/

/**
 * Defines Audio trx buffer MAX count.
 */

/**
 * Defines UAC 2.0 device volume MAX & MIN.
 */

/* bit_width */
#define USBD_UAC_BIT_WIDTH(byte_width)              (8U * (byte_width))

/* UAC channel config */
#define USBD_UAC_GET_CH_CONFIG(ch_cnt) \
    ((ch_cnt) == 2 ? 0x03 : \
     (ch_cnt) == 4 ? 0x0F : \
     (ch_cnt) == 6 ? 0x3F : \
     (ch_cnt) == 8 ? 0xFF : \
     (ch_cnt) == 16 ? 0xFFFF : 0x03)

/* UAC terminal type */
#define USBD_UAC_GET_OT_TYPE(ch_cnt) \
    ((ch_cnt) == 2 ? 0x0301 : \
     (ch_cnt) == 4 ? 0x0304 : \
     (ch_cnt) == 6 ? 0x0304 : \
     (ch_cnt) == 8 ? 0x0307 : \
     (ch_cnt) == 16 ? 0x0307 : 0x0301)

/* AC IF header interface num */

/* AC feature uint descriptor length */
#define USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt)        (USB_UAC2_LEN_FUNC_UNIT_DESC + 1 + 4 * (ch_cnt))
/* AC IF header descriptor length */
#define USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt)         \
    (USB_UAC2_LEN_CTRL_IF_HEADER + USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC + \
	USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt) + USB_UAC2_LEN_AC_OUT_TTY_DESC + USBD_UAC_AC_MIC_IF_LEN)

/* len of total Audio control interface */
#define USBD_UAC_AC_IF_LEN(ch_cnt)                 (USB_LEN_IF_DESC + USBD_UAC_AC_IF_HEAD_DESC_LEN(ch_cnt))

/* len of total Audio stream interface */
#define USBD_UAC_AS_TIF_LEN(alt_num)               (USBD_UAC_AS_EIF_LEN * (alt_num - 1) + USB_LEN_IF_DESC)

/**
 * High Speed
 */
/* calculate high speed MPS */
#define USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz) \
  ((ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS + 1U))
/* check MPS */
#define USBD_UAC_IS_HS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_HS_ISOC_MPS)
/* Ref UAC 2.0 5.2.5.1: the headphone clock RANGE is shared by every alt setting, so a rate is
   declared at High Speed only if the widest alt setting carries it, refer to the checks below */
#define USBD_UAC_HS_RATE_DECLARED(freq) \
    ((((freq) <= USBD_UAC_OUT_MAX_SAMPLING_FREQ) && USBD_UAC_IS_HS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, freq)) ? 1U : 0U)
/* number of rates of usbd_uac_sampling_rates[] declared at High Speed */
#define USBD_UAC_HS_SAMPLING_FREQ_COUNT \
    (USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_44K) + USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_48K) + \
     USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_96K) + USBD_UAC_HS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_192K))
/* highest rate declared at High Speed */
#define USBD_UAC_HS_MAX_SAMPLING_FREQ \
    ((USBD_UAC_HS_SAMPLING_FREQ_COUNT == 4U) ? USBD_UAC_SAMPLING_FREQ_192K : \
     ((USBD_UAC_HS_SAMPLING_FREQ_COUNT == 3U) ? USBD_UAC_SAMPLING_FREQ_96K : USBD_UAC_SAMPLING_FREQ_48K))
/* get high speed MPS of an alt setting, sized for the highest rate declared at High Speed */
#define USBD_UAC_GET_HS_MPS(ch_cnt, byte_width)     USBD_UAC_CALC_HS_MPS(ch_cnt, byte_width, USBD_UAC_HS_MAX_SAMPLING_FREQ)

/* len of high speed total configuration descriptor buf */
#define USBD_UAC_HS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_HS_AS_ALT_SETTING_NUM) \
	+ USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM))

/*
*	Full Speed
*/
/* calculate full speed MPS */
#define USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz) \
    ((ch_cnt) * (byte_width) * ((sampling_freq_hz) / USBD_UAC_ONE_KHZ + 1U))
/* check MPS */
#define USBD_UAC_IS_FS_MPS_VALID(ch_cnt, byte_width, sampling_freq_hz) \
    ((USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, sampling_freq_hz)) <= USBD_UAC_FS_ISOC_MPS)
/* A rate is declared at Full Speed only if the widest alt setting carries it, as for High Speed */
#define USBD_UAC_FS_RATE_DECLARED(freq) \
    ((((freq) <= USBD_UAC_OUT_MAX_SAMPLING_FREQ) && USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, freq)) ? 1U : 0U)
/* number of rates of usbd_uac_sampling_rates[] declared at Full Speed */
#define USBD_UAC_FS_SAMPLING_FREQ_COUNT \
    (USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_44K) + USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_48K) + \
     USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_96K) + USBD_UAC_FS_RATE_DECLARED(USBD_UAC_SAMPLING_FREQ_192K))
/* highest rate declared at Full Speed */
#define USBD_UAC_FS_MAX_SAMPLING_FREQ \
    ((USBD_UAC_FS_SAMPLING_FREQ_COUNT == 4U) ? USBD_UAC_SAMPLING_FREQ_192K : \
     ((USBD_UAC_FS_SAMPLING_FREQ_COUNT == 3U) ? USBD_UAC_SAMPLING_FREQ_96K : USBD_UAC_SAMPLING_FREQ_48K))
/* get full speed MPS of an alt setting, sized for the highest rate declared at Full Speed */
#define USBD_UAC_GET_FS_MPS(ch_cnt, byte_width)     USBD_UAC_CALC_FS_MPS(ch_cnt, byte_width, USBD_UAC_FS_MAX_SAMPLING_FREQ)

/* len of full speed total configuration descriptor buf */
#define USBD_UAC_FS_CFG_DESC_BUF_LEN(ch_cnt) \
    (USB_LEN_CFG_DESC + USB_LEN_IAD_DESC  + USBD_UAC_AC_IF_LEN(ch_cnt) + USBD_UAC_AS_TIF_LEN(USBD_UAC_FS_AS_ALT_SETTING_NUM) \
	+ USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM))

/* Build-time checks of the descriptor templates compiled into the image. The connection speed
   is only known at run time, e.g. an HS device behind an FS hub uses the FS template, so every
   template must be valid on its own. Ref USB 2.0 5.6.3: ISOC packet <= 1023 bytes at Full Speed,
   <= 1024 bytes per microframe at High Speed. */
#if !USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_48K)
#error "UAC2: channels x bytes per sample shall be <= 20 to fit FS 48kHz (1023B). Lower 'Playback max channels' or 'Playback bit width'"
#endif
#if defined(CONFIG_SUPPORT_USB_FS_ONLY) && !USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_OUT_MAX_SAMPLING_FREQ)
#if USBD_UAC_IS_FS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_SAMPLING_FREQ_96K)
#error "UAC2: FS-only SoC, 'Playback max sampling rate' exceeds FS ISOC 1023B. Set it to 96 kHz, or lower channels or bit width"
#else
#error "UAC2: FS-only SoC, 'Playback max sampling rate' exceeds FS ISOC 1023B. Set it to 48 kHz, or lower channels or bit width"
#endif
#endif
#if !defined(CONFIG_SUPPORT_USB_FS_ONLY) && !USBD_UAC_IS_HS_MPS_VALID(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH, USBD_UAC_OUT_MAX_SAMPLING_FREQ)
#error "UAC2: 'Playback max sampling rate' exceeds HS ISOC 1024B. Lower it, or lower 'Playback max channels' or 'Playback bit width'"
#endif

/* Input terminal */
#define USBD_UAC_CH_CONFIG_TYPE_LOW(ch_cnt)         (USB_LOW_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))
#define USBD_UAC_CH_CONFIG_TYPE_HIGH(ch_cnt)        (USB_HIGH_BYTE(USBD_UAC_GET_CH_CONFIG(ch_cnt)))

/* Output terminal */
#define USBD_UAC_OT_DESC_TYPE_LOW(ch_cnt)           (USB_LOW_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))
#define USBD_UAC_OT_DESC_TYPE_HIGH(ch_cnt)          (USB_HIGH_BYTE(USBD_UAC_GET_OT_TYPE(ch_cnt)))

#define USBD_UAC_INIT_SUB_RANGE(sub_range, min_sampling_freq, max_sampling_freq, res) \
    do {                                               \
        (sub_range).dMIN = (min_sampling_freq);        \
        (sub_range).dMAX = (max_sampling_freq);        \
        (sub_range).dRES = (res);                      \
    } while(0)

/* Private function prototypes -----------------------------------------------*/

static int usbd_uac_set_config(usb_dev_t *dev, u8 config);
static void usbd_uac_clear_config(usb_dev_t *dev, u8 config);
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req);
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len);
static void usbd_uac_handle_sof(usb_dev_t *dev);
static void usbd_uac_append_data(usbd_uac_buf_ctrl_t *pdata_ctrl);
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status);
static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len);
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev);
static void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status);
#if UABD_UAC_DESC_DUMP
static int usbd_uac_desc_dump(const u8 *pbuf, int len);
#endif
#if USBD_UAC_DEBUG
static void usbd_uac_status_dump_thread(void *param);
#endif
static inline void usbd_uac_get_audio_data_cnt(u32 audio_len);
static u16 usbd_uac_get_ring_buf_cnt(u8 speed, u8 is_record);
static u16 usbd_uac_get_mps(const usbd_audio_cfg_t *params, u8 speed);
static u16 usbd_uac_get_play_mps(u8 speed);
static int usbd_uac_check_play_fmt(const usbd_audio_cfg_t *fmt, u8 speed);
static u32 usbd_uac_read_locked(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag);
static u8 usbd_uac_get_ch_config(u8 ch_cnt);
static inline u8 usbd_uac_ep_enable(const usbd_audio_cfg_t *ep);
static int usbd_uac_clk_valid_request(usb_dev_t *dev, u8 flag);
static u8 usbd_uac_is_owned_interface(const usbd_uac_dev_t *cdev, u16 interface_number);
static u8 usbd_uac_is_mic_entity(u8 entity_id);
static int usbd_uac_receive_control_data(usb_dev_t *dev, const usb_setup_req_t *req, u16 expected_length);
static int usbd_uac_cur_sampling_freq_request(usb_dev_t *dev, u32 frequency);
static int usbd_uac_sampling_freq_ctrl_range_request(usb_dev_t *dev, u16 max_len);
static int usbd_uac_mic_sampling_freq_range_request(usb_dev_t *dev, u16 max_len);
static const usbd_audio_cfg_t *usbd_uac_get_mic_fmt(const usbd_uac_dev_t *cdev);
static u32 usbd_uac_get_mic_sampling_freq(const usbd_uac_dev_t *cdev);
static int usbd_uac_check_mic_fmt(const usbd_audio_cfg_t *fmt);
static int usbd_uac_isoc_in_ep_open(usb_dev_t *dev);
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed);
static void usbd_uac_isoc_in_process_xfer(usb_dev_t *dev);
static void usbd_uac_isoc_in_process_complete(usb_dev_t *dev);
static int usbd_uac_rec_pktz_init(const usbd_audio_cfg_t *fmt, u8 speed, u16 mps);
static void usbd_uac_rec_pktz_deinit(void);
static void usbd_uac_rec_pktz_reset(void);
static void usbd_uac_rec_pktz_next(void);

/* Private variables ---------------------------------------------------------*/

static const char *const TAG = "UAC";

/* Ascending: the first USBD_UAC_xS_SAMPLING_FREQ_COUNT entries are the rates declared at that speed */
static const u32 usbd_uac_sampling_rates[USBD_UAC_SAMPLING_FREQ_MAX_COUNT] = {
	USBD_UAC_SAMPLING_FREQ_44K, USBD_UAC_SAMPLING_FREQ_48K, USBD_UAC_SAMPLING_FREQ_96K, USBD_UAC_SAMPLING_FREQ_192K
};

/* Mic format used when the record direction is disabled: the mic is then stripped from the
   reported descriptor, this only keeps the mic accessors on a valid template format. */
static const usbd_audio_cfg_t usbd_uac_mic_default_fmt = {
	.sampling_freq = USBD_UAC_IN_DEFAULT_SAMPLING_FREQ,
	.byte_width = USBD_UAC_IN_DEFAULT_BYTE_WIDTH,
	.ch_cnt = USBD_UAC_IN_DEFAULT_CH_CNT,
	.enable = 0U,
};

/* Cache-aligned ISOC OUT DMA scratch (one packet), copied into the ring buffer. */
static u8 usbd_uac_rx_buf[USBD_UAC_HS_ISOC_MPS] USB_DMA_ALIGNED;

/* Cache-aligned ISOC IN DMA scratch (one packet), staged from the ring buffer. */
static u8 usbd_uac_tx_buf[USBD_UAC_HS_ISOC_MPS] USB_DMA_ALIGNED;

/* Record packetizer, its buffer lives as long as the record ring, refer to usbd_uac_ep_buf_ctrl_init() */
static usbd_uac_rec_pktz_t usbd_uac_rec_pktz;

/* USB Standard Device Descriptor */
static const u8 usbd_uac_dev_desc[USB_LEN_DEV_DESC] = {
	USB_LEN_DEV_DESC,            /* bLength */
	USB_DESC_TYPE_DEVICE,        /* bDescriptorType */
	0x00,                        /* bcdUSB */
	0x02,
	0x00,                        /* bDeviceClass */
	0x00,                        /* bDeviceSubClass */
	0x00,                        /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,            /* bMaxPacketSize */
	USB_LOW_BYTE(USB_VID),  /* idUAC */
	USB_HIGH_BYTE(USB_VID),
	USB_LOW_BYTE(USB_PID),  /* idProduct */
	USB_HIGH_BYTE(USB_PID),
	0x00,                        /* bcdDevice */
	0x02,
	USBD_IDX_MFC_STR,            /* iManufacturer */
	USBD_IDX_PRODUCT_STR,        /* iProduct */
	USBD_IDX_SERIAL_STR,         /* iSerialNumber */
	0x01                         /* bNumConfigurations */
}; /* usbd_uac_dev_desc */

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
/* USB Standard Device Qualifier Descriptor */
static const u8 usbd_uac_device_qualifier_desc[USB_LEN_DEV_QUALIFIER_DESC] = {
	USB_LEN_DEV_QUALIFIER_DESC,        /* bLength */
	USB_DESC_TYPE_DEVICE_QUALIFIER,    /* bDescriptorType */
	0x00,                              /* bcdUSB */
	0x02,
	0x00,                              /* bDeviceClass */
	0x00,                              /* bDeviceSubClass */
	0x00,                              /* bDeviceProtocol */
	0x40,                              /* bMaxPacketSize */
	0x01,                              /* bNumConfigurations */
	0x00,                              /* Reserved */
}; /* usbd_uac_device_qualifier_desc */

static const u8 usbd_uac_hs_config_desc[] = {
	/* USB UAC Device Configuration Descriptor */
	/* USB Standard Configuration Descriptor */
	USB_LEN_CFG_DESC,                  /* bLength */
	USB_DESC_TYPE_CONFIGURATION,       /* bDescriptorType */
	0x00,                              /* wTotalLength: patched at runtime */
	0x00,
	0x03,                              /* Number of interfaces supported by this configuration */
	0x01,                              /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                              /* Index of string descriptor describing this configuration */
	0x80,                              /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                              /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USB_UAC_IAD_DESCRIPTOR_TYPE,            /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	3,                /* Audio Control Interface, Audio Streaming Interfaces () */
	USB_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USB_UAC2_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USB_UAC_CLASS_CODE,               /* Audio Device Class */
	USB_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USB_UAC2_LEN_CTRL_IF_HEADER,                                         /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                               /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_HEADER,                                  /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                         /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),/* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                         the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* Constant uniquely identifying the Clock Source Entity within
                                                             the audio funcion */
	0x03,                                                    /* D1..0: 11: Internal Programmable Clock
                                                             D2: 0 Clock is not synchronized to SOF
                                                             D7..3: Reserved, should set to 0   */
	0x07,                                                    /* D1..0: Clock Frequency Control is present and Host programmable
                                                             D3..2: Clock Validity Control is present but read-only
                                                             D7..4: Reserved, should set to 0 */
	0x00,                                                    /* This Clock Source has no association   */
	0x00,                                                    /* Index of a string descriptor, describing the Clock Source Entity  */

	/* Headphone Audio Control Input Terminal Descriptor 2.0 */
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                    /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 8 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels:: Mono, no spatial location */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls D1..0: Copy Protect Control is not present
                                                                D3..2: Connector Control is not present
                                                                D5..4: Overload Control is not present
                                                                D7..6: Cluster Control is not present
                                                                D9..8: Underflow Control is not present
                                                                D11..10: Overflow Control is not present
                                                                D15..12: Reserved, should set to 0*/
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),         /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F):  D1..0: Mute Control is present and host
                                                                programmable D3..2: Volume Control is present and host programmable D5..4: Bass
                                                                Control is not present D7..6: Mid Control is not present D9..8: Treble Control is not
                                                                present D11..10: Graphic Equalizer Control is not present D13..12: Automatic Gain
                                                                Control is not present D15..14: Delay Control is not present D17..16: Bass Control is
                                                                not present D19..18: Loudness Control is not present D21..20: Input Gain Control is
                                                                not present D23..22: Input Gain Pad Control is not present D25..24: Phase Inverter
                                                                Control is not present D27..26: Underflow Control is not present D29..28: Overflow
                                                                Control is not present D31..30: Reserved, should set to 0 */
	0x00,                                                    /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[2](0x00000000) */
	0x00,
	0x00,
	0x00,
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[3](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[4](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[5](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[6](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[7](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[8](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
	0x00,                                                     /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */
	USBD_UAC_OT_DESC_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_OT_DESC_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                 AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,        /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls:   D1..0: Copy Protect Control is not present
                                                                 D3..2: Connector Control is not present
                                                                 D5..4: Overload Control is not present
                                                                 D7..6: Underflow Control is not present
                                                                 D9..8: Overflow Control is not present
                                                                 D15..10: Reserved, should set to 0   */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */

	/* Audio Control Clock Source Unit Descriptor 2.0 (Microphone) */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* Constant uniquely identifying the Clock Source Entity within
	                                                         the audio funcion */
	0x03,                                                    /* D1..0: 11: Internal Programmable Clock
	                                                         D2: 0 Clock is not synchronized to SOF
	                                                         D7..3: Reserved, should set to 0   */
	0x07,                                                    /* D1..0: Clock Frequency Control is present and Host programmable
	                                                         D3..2: Clock Validity Control is present but read-only
	                                                         D7..4: Reserved, should set to 0 */
	0x00,                                                    /* This Clock Source has no association   */
	0x00,                                                    /* Index of a string descriptor, describing the Clock Source Entity  */

	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,               /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x02,                                                    /* Microphone terminal type (0x0201) */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* This Terminal's output audio channel cluster has 2 logical channels (stereo)  */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls: no controls present */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* Microphone FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_MAX_CH_CNT),    /* Size of the descriptor, in byte_width, trimmed to cfg->in at runtime */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                 /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,        /* Constant uniquely identifying the Unit within the audio function. This
	                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F): Mute + Volume Control present */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(1) logical channel 1: no controls */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(2) logical channel 2: no controls */
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* Microphone OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,               /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                 function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                     /* USB Streaming terminal type (0x0101) */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls: no controls present */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */

	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_2,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_2),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_2),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_4,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_4),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_4),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_6,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_6),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_6),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_8,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_8),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_8),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_HS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

	/* 4.9.1 Standard AS Interface Descriptor 1/2 (Microphone) */
	/* Interface2 Descriptor alt setting 0 zero-bandwidth */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 2/2 (Microphone) */
	/* Interface2 Descriptor alt setting 1 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 1 */
	0x01,                                                     /* The number of endpoints used by this interface is 1 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* Number of physical channels in the AS Interface audio channel cluster */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Microphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_IN_DEFAULT_BYTE_WIDTH,                          /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_IN_DEFAULT_BYTE_WIDTH),      /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_D2H,                                                  /* bEndpointAddress: dir IN (placeholder) */
	0x05,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	/* wMaxPacketSize: patched with cfg->in */
	USB_LOW_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ)),
	USB_HIGH_BYTE(USBD_UAC_CALC_HS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
};
/* The length is computed by USBD_UAC_HS_CFG_DESC_BUF_LEN() from the channel count and the template is trimmed by
   #if on the same count: a mismatch would leave zero padding counted in wTotalLength */
_Static_assert(sizeof(usbd_uac_hs_config_desc) == USBD_UAC_HS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT), "UAC cfg desc len");
#endif

/* USB UAC Device Configuration Descriptor */
/* USB Standard Configuration Descriptor */
static const u8 usbd_uac_fs_config_desc[] = {
	USB_LEN_CFG_DESC,                                        /* bLength */
	USB_DESC_TYPE_CONFIGURATION,                             /* bDescriptorType */
	0x00,                                                    /* wTotalLength: patched at runtime */
	0x00,
	0x03,                                                    /* Number of interfaces supported by this configuration */
	0x01,                                                    /* Value to use as an argument to the SetConfiguration() request to select this configuration */
	0x00,                                                    /* Index of string descriptor describing this configuration */
	0x80,                                                    /* bmAttributes (patched at runtime for self_powered/remote_wakeup) */
	0x32,                                                    /* bMaxPower */

	/* 4.6 Interface Association Descriptor */
	/* IAD Descriptor */
	USB_LEN_IAD_DESC,                  /* Size of this descriptor (byte_width) */
	USB_UAC_IAD_DESCRIPTOR_TYPE,      /* Interface Association Descriptor type */
	0x00,                              /* First Index: Audio Control Interface Index - Headset (0) */
	0x03,                              /* Audio Control Interface, Audio Streaming Interfaces () */
	USB_UAC_IF_CLASS_AUDIO,           /* Audio Device Class  */
	0x00,                              /* No subclass */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Function string descriptor index (0) */

	/* 4.7.1 Standard AC Interface Descriptor */
	/* Interface 0 Descriptor */
	USB_LEN_IF_DESC,                   /* bLength */
	USB_DESC_TYPE_INTERFACE,           /* Interface Descriptor type */
	USB_UAC2_IF_IDX_AC_HEADSET,        /* Audio Control Interface Index - Headset (0) */
	0x00,                              /* bAlternateSetting */
	0x00,                              /* No associated endpoints with this interface (uses endpoint 0) */
	USB_UAC_CLASS_CODE,               /* Audio Device Class */
	USB_UAC_SUBCLASS_AUDIOCONTROL,    /* Audio Control Interface */
	USB_UAC_IP_VERSION_2,            /* Audio Protocol IP version 2.00 */
	0x00,                              /* Interface string descriptor index (0) */

	/* 4.7.2 Class-Specific AC Interface Descriptor */
	/* Audio headset */
	USB_UAC2_LEN_CTRL_IF_HEADER,                                          /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                                /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_HEADER,                                   /* HEADER descriptor subtype  */
	USB_LOW_BYTE(0x0200),                                                 /* bcdUSB */
	USB_HIGH_BYTE(0x0200),
	USB_UAC2_FUNC_CATEGORY_CODE_DESKTOP_SPEAKER,                          /* DESKTOP_SPEAKER(0x01) : Indicating the primary use of this audio function   */
	USB_LOW_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)),
	USB_HIGH_BYTE(USBD_UAC_AC_IF_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT)), /* Total number of byte_width returned for the class-specific AudioControl interface descriptor. Includes
                                                                           the combined length of this descriptor header and all Unit and Terminal descriptors.   */
	0x00,                                                                 /* D1..0: Latency Control  */

	/* Audio Control Clock Source Unit Descriptor 2.0 */
	USB_UAC2_LEN_CLK_SRC_DESC,                         /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,             /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,          /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,   /* Constant uniquely identifying the Clock Source Entity within
                                                          the audio funcion */
	0x03,                                              /* D1..0: 11: Internal Programmable Clock
                                                          D2: 0 Clock is not synchronized to SOF
                                                          D7..3: Reserved, should set to 0   */
	0x07,                                              /* D1..0: Clock Frequency Control is present and Host programmable
                                                          D3..2: Clock Validity Control is present but read-only
                                                          D7..4: Reserved, should set to 0 */
	0x00,                                              /* This Clock Source has no association   */
	0x00,                                              /* Index of a string descriptor, describing the Clock Source Entity  */

	/* Headphone Audio Control Input Terminal Descriptor 2.0 */
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,              /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                    /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field. */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_DEFAULT_CH_CNT,                                 /* This Terminal's output audio channel cluster has 4 logical output channels   */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels:: Mono, no spatial location */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls D1..0: Copy Protect Control is not present
                                                                D3..2: Connector Control is not present
                                                                D5..4: Overload Control is not present
                                                                D7..6: Cluster Control is not present
                                                                D9..8: Underflow Control is not present
                                                                D11..10: Overflow Control is not present
                                                                D15..12: Reserved, should set to 0*/
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_DEFAULT_CH_CNT),   /* Size of the descriptor, in byte_width  : 6 + (4 + 1) * 4 */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,       /* Constant uniquely identifying the Unit within the audio function. This
                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F):  D1..0: Mute Control is present and host
                                                                programmable D3..2: Volume Control is present and host programmable D5..4: Bass
                                                                Control is not present D7..6: Mid Control is not present D9..8: Treble Control is not
                                                                present D11..10: Graphic Equalizer Control is not present D13..12: Automatic Gain
                                                                Control is not present D15..14: Delay Control is not present D17..16: Bass Control is
                                                                not present D19..18: Loudness Control is not present D21..20: Input Gain Control is
                                                                not present D23..22: Input Gain Pad Control is not present D25..24: Phase Inverter
                                                                Control is not present D27..26: Underflow Control is not present D29..28: Overflow
                                                                Control is not present D31..30: Reserved, should set to 0 */
	0x00,                                                    /*  bmaControls[1](0x00000000), eg. 8 channels means bmaControls[1] ~ bmaControls[8] */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[2](0x00000000) */
	0x00,
	0x00,
	0x00,
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[3](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[4](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[5](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[6](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	0x00,                                                    /*  bmaControls[7](0x00000000)*/
	0x00,
	0x00,
	0x00,
	0x00,                                                    /*  bmaControls[8](0x00000000) */
	0x00,
	0x00,
	0x00,
#endif
	0x00,                                                     /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,              /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_HEADPHONES, /* Constant uniquely identifying the Terminal within the audio
                                                                 function. This value is used in all requests to address this Terminal.   */
	USBD_UAC_OT_DESC_TYPE_LOW(USBD_UAC_DEFAULT_CH_CNT),
	USBD_UAC_OT_DESC_TYPE_HIGH(USBD_UAC_DEFAULT_CH_CNT),      /* A Terminal dealing with a signal carried over an endpoint in an AudioStreaming interface. The
                                                                 AudioStreaming interface descriptor points to the associated Terminal through the bTerminalLink field.  */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT,        /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls:   D1..0: Copy Protect Control is not present
                                                                 D3..2: Connector Control is not present
                                                                 D5..4: Overload Control is not present
                                                                 D7..6: Underflow Control is not present
                                                                 D9..8: Overflow Control is not present
                                                                 D15..10: Reserved, should set to 0   */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */

	/* Audio Control Clock Source Unit Descriptor 2.0 (Microphone) */
	USB_UAC2_LEN_CLK_SRC_DESC,                               /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC2_AC_IF_DESC_SUBTYPE_CLOCK_SOURCE,                /* CLOCK_SOURCE descriptor subtype  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* Constant uniquely identifying the Clock Source Entity within
	                                                         the audio funcion */
	0x03,                                                    /* D1..0: 11: Internal Programmable Clock
	                                                         D2: 0 Clock is not synchronized to SOF
	                                                         D7..3: Reserved, should set to 0   */
	0x07,                                                    /* D1..0: Clock Frequency Control is present and Host programmable
	                                                         D3..2: Clock Validity Control is present but read-only
	                                                         D7..4: Reserved, should set to 0 */
	0x00,                                                    /* This Clock Source has no association   */
	0x00,                                                    /* Index of a string descriptor, describing the Clock Source Entity  */

	/* Microphone Audio Control Input Terminal Descriptor 2.0 */
	USB_UAC2_LEN_AC_IN_TTY_DESC,                             /* Size of the descriptor, in byte_width  */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_INPUT_TERMINAL,               /* INPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                  function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x02,                                                    /* Microphone terminal type (0x0201) */
	0x00,                                                    /* This Input Terminal has no association   */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,         /* ID of the Clock Entity to which this Input Terminal is connected.  */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* This Terminal's output audio channel cluster has 2 logical channels (stereo)  */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* Index of a string descriptor, describing the name of the first logical channel.  */
	0x00,
	0x00,                                                    /* bmControls: no controls present */
	0x00,                                                    /* Index of a string descriptor, describing the Input Terminal.  */

	/* Microphone FEATURE_UNIT descriptor subtype */
	USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_MAX_CH_CNT),    /* Size of the descriptor, in byte_width, trimmed to cfg->in at runtime */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AC_FEATURE_UNIT,                 /* FEATURE_UNIT descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,        /* Constant uniquely identifying the Unit within the audio function. This
	                                                                value is used in all requests to address this Unit.  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE, /* ID of the Unit or Terminal to which this Feature Unit is connected. */
	0x0F,
	0x00,
	0x00,
	0x00,                                                    /* logic channel 0 bmaControls(0)(0x0000000F): Mute + Volume Control present */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(1) logical channel 1: no controls */
	0x00,
	0x00,
	0x00,
	0x00,                                                    /* bmaControls(2) logical channel 2: no controls */
	0x00,                                                    /* iFeature, Index of a string descriptor, describing this Feature Unit.*/

	/* Microphone OUTPUT_TERMINAL descriptor subtype   */
	USB_UAC2_LEN_AC_OUT_TTY_DESC,                             /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                    /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AC_OUTPUT_TERMINAL,               /* OUTPUT_TERMINAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* Constant uniquely identifying the Terminal within the audio
	                                                                 function. This value is used in all requests to address this Terminal.   */
	0x01,
	0x01,                                                     /* USB Streaming terminal type (0x0101) */
	0x00,                                                     /* This Output Terminal has no association  */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT,         /* ID of the Unit or Terminal to which this Terminal is connected.  */
	USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE,          /* ID of the Clock Entity to which this Output Terminal is connected  */
	0x00,
	0x00,                                                     /* bmControls: no controls present */
	0x00,                                                     /* Index of a string descriptor, describing the Output Terminal.  */

	/* 4.9.1 Standard AS Interface Descriptor 1/0*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 1/1*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                      /* The number of this interface is 1.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_HEADPHONES, /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_2,                                 /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_2),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_2),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),   /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_2, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/2*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x02,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_4,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_4),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_4),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_4, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/3*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x03,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1,                                                     /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                     /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                     /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_6,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_6),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_6),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_6, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
	/* 4.9.1 Standard AS Interface Descriptor 1/4*/
	/* Interface1 Descriptor alt setting 0 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	0x01,                                                     /* The number of this interface is 1.   */
	0x04,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x01,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Headphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	0x1, 													 /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_CH_CNT_8,                                       /* Number of physical channels in the AS Interface audio channel cluster */
	USBD_UAC_CH_CONFIG_TYPE_LOW(USBD_UAC_CH_CNT_8),
	USBD_UAC_CH_CONFIG_TYPE_HIGH(USBD_UAC_CH_CNT_8),
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels: */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Headphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_DEFAULT_BYTE_WIDTH,                             /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_DEFAULT_BYTE_WIDTH),         /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Streaming ISO OUT Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_H2D,                                                  /* bEndpointAddress: dir OUT (placeholder) */
	0x09,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Adaptive
                                                                Usage: Data EP  */
	USB_LOW_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),    /* wMaxPacketSize: */
	USB_HIGH_BYTE(USBD_UAC_GET_FS_MPS(USBD_UAC_CH_CNT_8, USBD_UAC_DEFAULT_BYTE_WIDTH)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Headphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
#endif

	/* 4.9.1 Standard AS Interface Descriptor 1/2 (Microphone) */
	/* Interface2 Descriptor alt setting 0 zero-bandwidth */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x00,                                                     /* The value used to select the alternate setting for this interface is 0 */
	0x00,                                                     /* The number of endpoints used by this interface is 0 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.1 Standard AS Interface Descriptor 2/2 (Microphone) */
	/* Interface2 Descriptor alt setting 1 streaming */
	USB_LEN_IF_DESC,                                          /* Descriptor size is 9 byte_width   */
	USB_DESC_TYPE_INTERFACE,                                  /* INTERFACE Descriptor Type   */
	USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE,                    /* The number of this interface is 2.   */
	0x01,                                                     /* The value used to select the alternate setting for this interface is 1 */
	0x01,                                                     /* The number of endpoints used by this interface is 1 (excluding endpoint zero)   */
	USB_UAC_IF_CLASS_AUDIO,                                  /* The interface implements the Audio Interface class   */
	USB_UAC_SUBCLASS_AUDIOSTREAMING,                         /* The interface implements the AUDIOSTREAMING Subclass   */
	USB_UAC_IP_VERSION_2,                                   /* The Protocol code is 32   */
	0x00,                                                     /* The interface string descriptor index is 0   */

	/* 4.9.2 Class-Specific AS Interface Descriptor */
	/* Microphone Class Specific Audio Streaming Interface Alt Setting 1 */
	USB_UAC2_LEN_AS_IF_ALT_SET_DESC,                         /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type  */
	USB_UAC_AS_GENERAL,                  /* AS_GENERAL descriptor subtype   */
	USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE, /* The Terminal ID of the terminal to which this interface is connected */
	0x00,                                                    /* bmControls : D1..0: Active Alternate Setting Control is not present
                                                                D3..2: Valid Alternate Settings Control is not present
                                                                D7..4: Reserved, should set to 0   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	0x01,
	0x00,
	0x00,
	0x00,                                                    /* The Audio Data Format that can be Used to communicate with this interface, D0:PCM */
	USBD_UAC_IN_DEFAULT_CH_CNT,                              /* Number of physical channels in the AS Interface audio channel cluster */
	0x03,                                                    /* bmChannelConfig: Front Left + Front Right (stereo) */
	0x00,
	0x00,
	0x00,                                                    /* Describes the spatial location of the logical channels */
	0x00,                                                    /* Index of a string descriptor, describing the name of the first physical channel   */

	/* 4.9.3 Class-Specific AS Format Type Descriptor */
	/* Microphone Audio Streaming Format Type Descriptor 2.0 */
	USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC,                   /* Size of the descriptor, in byte_width   */
	USB_UAC_CS_INTERFACE,                   /* CS_INTERFACE Descriptor Type   */
	USB_UAC_AS_FORMAT_TYPE,                 /* FORMAT_TYPE descriptor subtype   */
	0x01,                                                    /* The format type AudioStreaming interfae using is FORMAT_TYPE_I (0x01)   */
	USBD_UAC_IN_DEFAULT_BYTE_WIDTH,                          /* The number of byte_width occupied by one audio subslot. Can be 1, 2, 3 or 4.   */
	USBD_UAC_BIT_WIDTH(USBD_UAC_IN_DEFAULT_BYTE_WIDTH),      /* The number of effectively used bits from the available bits in an audio subslot   */

	/* 4.10.1.1 Standard AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Streaming ISO IN Endpoint Descriptor */
	USB_LEN_EP_DESC,                                         /* Descriptor size is 7 byte_width */
	USB_DESC_TYPE_ENDPOINT,                                  /* ENDPOINT Descriptor Type*/
	USB_D2H,                                                  /* bEndpointAddress: dir IN (placeholder) */
	0x05,                                                    /* Types -
                                                                Transfer: ISOCHRONOUS
                                                                Sync: Async
                                                                Usage: Data EP  */
	/* wMaxPacketSize: patched with cfg->in */
	USB_LOW_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ)),
	USB_HIGH_BYTE(USBD_UAC_CALC_FS_MPS(USBD_UAC_IN_DEFAULT_CH_CNT, USBD_UAC_IN_DEFAULT_BYTE_WIDTH, USBD_UAC_IN_DEFAULT_SAMPLING_FREQ)),
	0x01,                                                    /* bInterval */

	/* 4.10.1.2 Class-Specific AS Isochronous Audio Data Endpoint Descriptor */
	/* Microphone Audio Data Endpoint Descriptor */
	USB_UAC2_LEN_DATA_EP_DESC,                               /* Size of this descriptor (byte_width) */
	USB_UAC_CS_ENDPOINT,                         /* Class-specific Endpoint Descriptor type */
	USB_UAC_AS_EP_GENERAL,                  /* EP_GENERAL Descriptor subtype */
	0x00,                                                    /* Can handle short packets (D7 = 0) */
	0x00,                                                    /* No controls */
	0x00,                                                    /* Undefined lock delay units */
	0x00,                                                    /* Time it takes this endpoint to lock its */
	0x00,                                                    /* internal clock recovery circuitry */
};
/* The length is computed by USBD_UAC_FS_CFG_DESC_BUF_LEN() from the channel count and the template is trimmed by
   #if on the same count: a mismatch would leave zero padding counted in wTotalLength */
_Static_assert(sizeof(usbd_uac_fs_config_desc) == USBD_UAC_FS_CFG_DESC_BUF_LEN(USBD_UAC_DEFAULT_CH_CNT), "UAC cfg desc len");
/* UAC Class Driver */
static const usbd_class_driver_t usbd_uac_driver = {
	.get_descriptor = usbd_uac_get_descriptor,
	.set_config = usbd_uac_set_config,
	.clear_config = usbd_uac_clear_config,
	.setup = usbd_uac_setup,
	.ep0_data_out = usbd_uac_handle_ep0_data_out,
	.ep_data_in = usbd_uac_handle_ep_data_in,
	.ep_data_out = usbd_uac_handle_ep_data_out,
	.status_changed = usbd_uac_status_changed,
	.sof = usbd_uac_handle_sof,
};

/* UAC Device */
static usbd_uac_dev_t usbd_uac_dev;

/* UAC volume data from windows10 PC*/
/* Different OS transmit the same vol level, but the corresponding driver vol values are different. */
static const u8 usbd_uac_pc_vol_lvl[] = {0, 5, 10, 20, 30, 40, 50, 60, 65, 75, 80, 85, 90, 95, 100};
static const s16 usbd_uac_drv_vol[] = {-190, -170, -151, -112, -74, -37, 0, 37, 56, 93, 112, 132, 151, 170, 190};

/* Exported variables --------------------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

#if UABD_UAC_DESC_DUMP
/**
  * @brief  Dump UAC descriptor information.
  * @param  pbuf: Pointer to the descriptor buffer.
  * @param  len: Length of the descriptor buffer.
  * @retval Status
  */
static int usbd_uac_desc_dump(const u8 *pbuf, int len)
{
	int i = 0;
	int j;
	u8 len_val = 0;
	for (i = 0; i < len;) {
		/* Never read a header or body that runs past the buffer, a malformed bLength stops the walk */
		if ((len - i) < (int)USB_LEN_DESC_HEADER) {
			break;
		}
		len_val = pbuf[i];
		if ((len_val < USB_LEN_DESC_HEADER) || ((int)len_val > (len - i))) {
			break;
		}
		for (j = 0; j < (int)len_val; j++) {
			RTK_LOGS(NOTAG, RTK_LOG_INFO, "%02x ", pbuf[i + j]);
		}
		i += len_val;
		RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
		if (len_val < 6) {
			break;
		}
	}

	RTK_LOGS(NOTAG, RTK_LOG_INFO, "\n");
	return RTK_SUCCESS;
}
#endif

/**
  * @brief  Get the ring buffer count based on USB speed
  * @param  speed: USB connection speed
  * @param  is_record: 1 for the record (ISOC IN, tx_buf_cnt) ring, 0 for the playback (ISOC OUT, rx_buf_cnt) one
  * @retval Number of buffer segments for audio data
  */
static u16 usbd_uac_get_ring_buf_cnt(u8 speed, u8 is_record)
{
	const usbd_uac_dev_t *cdev = &usbd_uac_dev;
	u16 cnt;

	if (is_record != 0U) {
		cnt = (cdev->cfg->tx_buf_cnt != 0U) ? cdev->cfg->tx_buf_cnt : USBD_UAC_DEFAULT_TX_BUF_CNT;
	} else {
		cnt = (cdev->cfg->rx_buf_cnt != 0U) ? cdev->cfg->rx_buf_cnt : USBD_UAC_DEFAULT_RX_BUF_CNT;
	}

	if (speed == USB_SPEED_HIGH) { //high speed
		return (USBD_UAC_HS_SOF_COUNT_PER_MS * cnt) / USBD_UAC_POW2(USBD_UAC_HS_DEFAULT_BINTERVAL - 1U);
	}

	return cnt / USBD_UAC_POW2(USBD_UAC_FS_DEFAULT_BINTERVAL - 1U);
}

/**
  * @brief  Get the MPS based on configuration parameters and speed
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Calculated MPS
  */
static u16 usbd_uac_get_mps(const usbd_audio_cfg_t *params, u8 speed)
{
	u32 mps_value;
	if (NULL == params) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "No Param\n");
		return 0U;
	}

	/* Range-check in 32 bits before narrowing, else an oversized config could wrap below the
	   limit and pass the check. */
	if (speed == USB_SPEED_HIGH) {
		mps_value = (u32)params->ch_cnt * params->byte_width * ((params->sampling_freq / USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS) + 1U);
		if (mps_value > USBD_UAC_HS_ISOC_MPS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid HS MPS %d-%d\n", mps_value, USBD_UAC_HS_ISOC_MPS);
			return 0U;
		}
	} else {
		/* for 44.1khz or the host clk is bigger than the device */
		mps_value = (u32)params->ch_cnt * params->byte_width * ((params->sampling_freq / USBD_UAC_ONE_KHZ) + 1U);
		if (mps_value > USBD_UAC_FS_ISOC_MPS) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid FS MPS %d-%d\n", mps_value, USBD_UAC_FS_ISOC_MPS);
			return 0U;
		}
	}

	return (u16)mps_value;
}

/**
  * @brief  Get the size of a playback (ISOC OUT) ring node
  * @note   Ref UAC 2.0 3.16.2: the host may switch the alt setting and the rate at any time and
  *         then sends up to the wMaxPacketSize of the selected alt setting. The node is therefore
  *         the MPS of the widest alt setting at the highest rate declared at this speed, so a format
  *         switch never reallocates the ring. Checked against the ISOC limits at build time.
  * @param  speed: USB connection speed
  * @retval Node size in bytes
  */
static u16 usbd_uac_get_play_mps(u8 speed)
{
	if (speed == USB_SPEED_HIGH) {
		return (u16)USBD_UAC_GET_HS_MPS(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH);
	}

	return (u16)USBD_UAC_GET_FS_MPS(USBD_UAC_DEFAULT_CH_CNT, USBD_UAC_DEFAULT_BYTE_WIDTH);
}

/**
  * @brief  Get the channel configuration based on UAC channel cnt
  * @param  ch_cnt: UAC channel cnt
  * @retval Bitmask representing the channel configuration
  */
static u8 usbd_uac_get_ch_config(u8 ch_cnt)
{

	switch (ch_cnt) {
	case 1:
		/* Ref UAC 2.0 4.1: a mono cluster is the Front Center channel, the usual mono layout */
		return 0x04;
	case 2:
		return 0x03;
	case 4:
		return 0x0F;
	case 6:
		return 0x3F;
	case 8:
		return 0xFF;
	default:
		return 0x03;
	}
}

/**
  * @brief  Start the next record packet, sizing it from the fractional frame accumulator
  * @note   Ref UAC 2.0 2.3.1.1 and USB 2.0 5.12.4.2: an asynchronous source sends, in every
  *         (micro)frame, the nominal number of audio frames or one more, so that the long
  *         term average matches the sampling rate, e.g. 5 or 6 frames per microframe at 44.1kHz.
  * @retval void
  */
static void usbd_uac_rec_pktz_next(void)
{
	usbd_uac_rec_pktz_t *pktz = &usbd_uac_rec_pktz;

	pktz->fill = 0U;
	pktz->target = pktz->base_len;
	pktz->acc += pktz->rate_rem;
	if (pktz->acc >= pktz->pkt_per_sec) {
		pktz->acc -= pktz->pkt_per_sec;
		pktz->target = (u16)(pktz->target + pktz->frame_len);
	}
}

/**
  * @brief  Restart the record packetizer, dropping a partly assembled packet
  * @retval void
  */
static void usbd_uac_rec_pktz_reset(void)
{
	usbd_uac_rec_pktz.acc = 0U;
	usbd_uac_rec_pktz_next();
}

/**
  * @brief  Set up the record packetizer for the mic format at the connection speed
  * @param  fmt: Mic format, the one the descriptors report
  * @param  speed: USB connection speed
  * @param  mps: ISOC IN max packet size, which holds the nominal packet plus one frame
  * @retval RTK_SUCCESS, or -RTK_ERR_NOMEM when the packet buffer can not be allocated
  */
static int usbd_uac_rec_pktz_init(const usbd_audio_cfg_t *fmt, u8 speed, u16 mps)
{
	usbd_uac_rec_pktz_t *pktz = &usbd_uac_rec_pktz;

	usbd_uac_rec_pktz_deinit();

	pktz->buf = (u8 *)usb_os_malloc(mps);
	if (pktz->buf == NULL) {
		return -RTK_ERR_NOMEM;
	}

	pktz->pkt_per_sec = (u16)((speed == USB_SPEED_HIGH) ? (USBD_UAC_ONE_KHZ * USBD_UAC_HS_SOF_COUNT_PER_MS) : USBD_UAC_ONE_KHZ);
	pktz->frame_len = (u8)(fmt->ch_cnt * fmt->byte_width);
	pktz->base_len = (u16)((fmt->sampling_freq / pktz->pkt_per_sec) * pktz->frame_len);
	pktz->rate_rem = fmt->sampling_freq % pktz->pkt_per_sec;
	usbd_uac_rec_pktz_reset();

	return RTK_SUCCESS;
}

/**
  * @brief  Release the record packetizer buffer
  * @retval void
  */
static void usbd_uac_rec_pktz_deinit(void)
{
	if (usbd_uac_rec_pktz.buf != NULL) {
		usb_os_mfree((void *)usbd_uac_rec_pktz.buf);
	}
	usb_os_memset((void *)&usbd_uac_rec_pktz, 0, sizeof(usbd_uac_rec_pktz));
}

/**
  * @brief  Deinitialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @retval void
  */
static void usbd_uac_ep_buf_ctrl_deinit(usbd_uac_buf_ctrl_t *buf_ctrl)
{
	u32 wait_ms;

	/* Close the gates first, under the lock the ISRs test next_xfer with before touching the
	   ring and the task APIs test is_closing with, refer to usbd_uac_ringbuf_push() */
	usb_os_enter_critical();
	buf_ctrl->next_xfer = 0;
	buf_ctrl->is_closing = 1U;
	usb_os_exit_critical();

	buf_ctrl->isoc_mps = 0;
	buf_ctrl->buf_list_cnt = 0;
	buf_ctrl->sof_idx = 0;
	buf_ctrl->data_idx = 0;
	buf_ctrl->xfer_cnt = 0;
	buf_ctrl->last_xfer_len = 0;

	if (buf_ctrl->uac_sema_valid != 0) {
		buf_ctrl->uac_sema_valid = 0;

		/* Wait for every task still inside usbd_uac_read() or usbd_uac_transmit_data() on this
		   ring to leave. Not bounded: freeing the semaphore or the ring under such a task would be
		   a use after free, so a starved one is reported once per USBD_UAC_USER_EXIT_WARN_MS
		   instead. Sleep rather than spin, the woken task may have a lower priority. */
		wait_ms = 0U;
		while (buf_ctrl->xfer_users != 0U) {
			rtos_sema_give(buf_ctrl->uac_isoc_sema);
			usb_os_sleep_ms(1U);
			wait_ms++;
			if (wait_ms >= USBD_UAC_USER_EXIT_WARN_MS) {
				RTK_LOGS(TAG, RTK_LOG_WARN, "Wait ring user exit\n");
				wait_ms = 0U;
			}
		}
		rtos_sema_delete(buf_ctrl->uac_isoc_sema);
		buf_ctrl->uac_isoc_sema = NULL;
	}

	usb_ringbuf_manager_deinit(&(buf_ctrl->buf_list));

	/* No usbd_uac_transmit_data() caller is left, refer to the wait above */
	if (buf_ctrl == &(usbd_uac_dev.uac_isoc_in)) {
		usbd_uac_rec_pktz_deinit();
	}
}

/**
  * @brief  Initialize UAC endpoint buffer control structure
  * @param  buf_ctrl: Pointer to the UAC buffer control structure
  * @param  params: Pointer to audio configuration parameters structure
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_uac_ep_buf_ctrl_init(usbd_uac_buf_ctrl_t *buf_ctrl, const usbd_audio_cfg_t *params, u8 speed)
{
	u8 is_record = (buf_ctrl == &(usbd_uac_dev.uac_isoc_in)) ? 1U : 0U;
	u16 mps;
	u16 slot_count;
	u8 cnt;

	if (usbd_uac_ep_enable(params) != 0) {
		/* The record format is fixed by cfg->in, the playback node fits every declared format */
		mps = (is_record != 0U) ? usbd_uac_get_mps(params, speed) : usbd_uac_get_play_mps(speed);
		if (mps == 0U) {
			return -RTK_ERR_BADARG;
		}
		/* Checked in 16 bits before narrowing: 8 slots per ms at high speed */
		slot_count = usbd_uac_get_ring_buf_cnt(speed, is_record);
		if ((slot_count == 0U) || (slot_count > USBD_UAC_RING_NODE_COUNT_MAX)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Ring cnt %u err\n", slot_count);
			return -RTK_ERR_BADARG;
		}
		cnt = (u8)slot_count;

		/* The ring only depends on the speed: keep it while it fits, so a format switch of the
		   host never reallocates it */
		if ((buf_ctrl->uac_sema_valid != 0U) && (buf_ctrl->isoc_mps == mps) && (buf_ctrl->buf_list_cnt == cnt)) {
			return RTK_SUCCESS;
		}

		usbd_uac_ep_buf_ctrl_deinit(buf_ctrl);
		buf_ctrl->isoc_mps = mps;
		buf_ctrl->buf_list_cnt = cnt;
		buf_ctrl->binterval = (speed == USB_SPEED_HIGH) ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;

		/* Before the ring, so that its failure leaves nothing to release */
		if ((is_record != 0U) && (usbd_uac_rec_pktz_init(params, speed, mps) != RTK_SUCCESS)) {
			return -RTK_ERR_NOMEM;
		}

		if (usb_ringbuf_manager_init(&(buf_ctrl->buf_list), buf_ctrl->buf_list_cnt, buf_ctrl->isoc_mps, 1) != RTK_SUCCESS) {
			if (is_record != 0U) {
				usbd_uac_rec_pktz_deinit();
			}
			return -RTK_ERR_NOMEM;
		}

		if (buf_ctrl->uac_isoc_sema == NULL) {
			if (rtos_sema_create(&(buf_ctrl->uac_isoc_sema), 0U, 1U) != RTK_SUCCESS) {
				usb_ringbuf_manager_deinit(&(buf_ctrl->buf_list));
				if (is_record != 0U) {
					usbd_uac_rec_pktz_deinit();
				}
				return -RTK_ERR_NOMEM;
			}
		}
		buf_ctrl->read_wait_sema = 0;
		/* Every ring resource exists from here on: let usbd_uac_read() / usbd_uac_transmit_data() in.
		   Published under the lock, which orders the ring writes above before the flags for
		   usbd_uac_isoc_in_ep_open() in ISR context on the other core */
		usb_os_enter_critical();
		buf_ctrl->uac_sema_valid = 1;
		buf_ctrl->is_closing = 0U;
		usb_os_exit_critical();
		buf_ctrl->sof_idx = 0;
		buf_ctrl->data_idx = 0;
		buf_ctrl->xfer_cnt = 0;
		buf_ctrl->last_xfer_len = 0;
	} else {
		usbd_uac_ep_buf_ctrl_deinit(buf_ctrl);
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Get USB audio endpoint status
  * @param  ep: PUSB audio endpoint config
  * @retval Status
  */
static inline u8 usbd_uac_ep_enable(const usbd_audio_cfg_t *ep)
{
	if (ep != NULL) {
		return ep->enable;
	}

	return  0;
}

/**
  * @brief  Check whether the microphone AS interface is exposed to the host
  * @note   Ref USB 2.0 9.6.3: a configuration only reports what the device implements, so
  *         the mic AS interface is reported, in standalone and composite mode alike, only
  *         when the app enabled the IN endpoint. Requests targeting an interface or entity
  *         that is not in the reported config descriptor must be stalled.
  * @param  cdev: UAC device instance
  * @retval 1 if the mic AS interface exists, 0 otherwise
  */
static inline u8 usbd_uac_mic_present(const usbd_uac_dev_t *cdev)
{
	return ((cdev->cb != NULL) && (usbd_uac_ep_enable(&(cdev->cfg->in)) != 0U)) ? 1U : 0U;
}

/**
  * @brief  Check whether an interface number, local to this function, belongs to the class
  * @note   Composite mode already rebased wIndex to the local interface number.
  * @param  cdev: UAC device instance
  * @param  interface_number: Interface number from wIndex
  * @retval 1 if the interface is in the reported config descriptor, 0 otherwise
  */
static u8 usbd_uac_is_owned_interface(const usbd_uac_dev_t *cdev, u16 interface_number)
{
	u8 owned;

	if ((interface_number == USB_UAC2_IF_IDX_AC_HEADSET) || (interface_number == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES)) {
		owned = 1U;
	} else if (interface_number == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
		owned = usbd_uac_mic_present(cdev);
	} else {
		owned = 0U;
	}

	return owned;
}

/**
  * @brief  Check whether an entity ID belongs to the microphone path
  * @param  entity_id: Entity ID from the high byte of wIndex
  * @retval 1 for a mic entity, 0 otherwise
  */
static u8 usbd_uac_is_mic_entity(u8 entity_id)
{
	return ((entity_id == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE) ||
			(entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE) ||
			(entity_id == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) ||
			(entity_id == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_HEADSET_MICROPHONE)) ? 1U : 0U;
}

/**
  * @brief  Accept the data stage of an H2D class or vendor request
  * @note   Ref USB 2.0 8.5.3.1 and 9.2.7: the status stage reports whether the request was
  *         carried out. The core only hands a data stage to ep0_data_out() in the Configured
  *         state and ACKs it otherwise, and a parameter block of the wrong size can not be
  *         applied, so both are refused here and the core stalls the request.
  * @param  dev: USB device instance
  * @param  req: USB request
  * @param  expected_length: Size of the parameter block of the addressed control
  * @retval Status
  */
static int usbd_uac_receive_control_data(usb_dev_t *dev, const usb_setup_req_t *req, u16 expected_length)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep0_out = &dev->ep0_out;
	int ret = -RTK_ERR_BADARG;

	if ((dev->dev_state == USBD_STATE_CONFIGURED) && (req->wLength == expected_length) &&
		((u32)req->wLength <= ep0_out->xfer_buf_len)) {
		usb_os_memcpy((void *)&cdev->ctrl_req, (const void *)req, sizeof(usb_setup_req_t));
		cdev->ctrl_req_pending = 1U;
		ep0_out->xfer_len = req->wLength;
		/* Propagate a submit failure so that the core stalls EP0 instead of leaving the host
		   waiting out the data stage, ref USB 2.0 8.5.3.4 */
		ret = usbd_ep_receive(dev, ep0_out);
		if (ret != RTK_SUCCESS) {
			/* The data stage never started, so no EP0 OUT completion will arrive to consume the
			   stashed request. Drop it, else the next unrelated request's data stage would be
			   applied as this one's payload. */
			cdev->ctrl_req_pending = 0U;
		}
	}

	return ret;
}

/**
  * @brief  Handle UAC clock validity request
  * @param  dev: USB device instance
  * @param  flag: Clock validity flag
  * @retval Status of the EP0 IN submit
  */
static int usbd_uac_clk_valid_request(usb_dev_t *dev, u8 flag)
{
	u8 valid = flag;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&valid, sizeof(valid));
	ep0_in->xfer_len = sizeof(valid);
	return usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle UAC current freq request
  * @param  dev: USB device instance
  * @param  frequency: UAC current freq
  * @retval Status of the EP0 IN submit
  */
static int usbd_uac_cur_sampling_freq_request(usb_dev_t *dev, u32 frequency)
{
	u32 dCur = frequency;
	usbd_ep_t *ep0_in = &dev->ep0_in;

	usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&dCur, sizeof(dCur));
	ep0_in->xfer_len = sizeof(dCur);
	return usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Handle freq ctrl requests
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status
  */
static int usbd_uac_sampling_freq_ctrl_range_request(usb_dev_t *dev, u16 max_len)
{
	usb_uac2_freq_t sampling_freq_range;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u16 num_sub_ranges;
	u16 len;
	u16 data_offset = 0;

	if (dev->dev_speed  == USB_SPEED_HIGH) {
		num_sub_ranges = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		num_sub_ranges = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	len = sizeof(num_sub_ranges) + num_sub_ranges * sizeof(usb_uac2_freq_t);

	/* Build the response in place in the EP0 buffer: setup() is ISR context, no
	 * heap alloc; one sub-range is staged on the stack per iteration. */
	usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, sizeof(num_sub_ranges));
	data_offset += sizeof(num_sub_ranges);

	for (u8 i = 0; i < num_sub_ranges; i++) {
		USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, usbd_uac_sampling_rates[i], usbd_uac_sampling_rates[i], 0);
		usb_os_memcpy(ep0_in->xfer_buf + data_offset, &sampling_freq_range, sizeof(sampling_freq_range));
		data_offset += sizeof(sampling_freq_range);
	}

	if (len > max_len) {
		len = max_len;
	}

	ep0_in->xfer_len = len;
	return usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Set UAC class configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval Status
  */
static int usbd_uac_set_config(usb_dev_t *dev, u8 config)
{
	int ret = RTK_SUCCESS;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;

	/* Only the bConfigurationValue advertised in the config descriptor is valid */
	if (config != 1U) {
		return -RTK_ERR_BADARG;
	}

	cdev->dev = dev;

	cdev->alt_setting = 0U;
	cdev->alt_setting_in = 0U;

	/* Invalidate the cached stream format on each SET_CONFIGURATION so the first
	 * SET_INTERFACE(alt=1) of the new session re-fires format_changed(). Without
	 * this, a hotplug re-enumeration leaves cur_ch_cnt/cur_byte_width matching the
	 * host's selection, the "!=" guard drops format_changed(), and apps that gate
	 * playback on it never restart. cur_play_sampling_freq keeps its private_init default. */
	cdev->cur_ch_cnt = 0U;
	cdev->cur_byte_width = 0U;

	/* Ref USB 2.0 9.4.7: the status stage of SET_CONFIGURATION tells the host the device is
	   configured, so a failure of the app is reported and the core stalls the request */
	if ((cb != NULL) && (cb->set_config != NULL)) {
		ret = cb->set_config();
		if (ret != RTK_SUCCESS) {
			ret = -RTK_ERR_IO;
		}
	}

	if (ret == RTK_SUCCESS) {
		/* Published under the lock clear_config() resets it with, refer to usbd_uac_receive_data() */
		usb_os_enter_critical();
		cdev->is_configured = 1U;
		usb_os_exit_critical();
	}

	return ret;
}

/**
  * @brief  Clear UAC configuration
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  config: USB configuration index
  * @retval None
  */
static void usbd_uac_clear_config(usb_dev_t *dev, u8 config)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;
	usb_ep_info_t *info;

	UNUSED(config);

	if (dev == NULL) {
		return;
	}

	/* Ref USB 2.0 9.1.1.3 / 9.4.10: after a bus reset or a de-configuration no alternate setting
	   is selected and the endpoints are gone. The SOF path, the record engine and the task APIs
	   test this state under the same lock, possibly on another core. next_xfer is kept: the
	   stream resumes on its own once the host selects the streaming setting again. */
	usb_os_enter_critical();

	/* Closes the EP arm path of the task APIs before the EPs go, refer to usbd_uac_receive_data() */
	cdev->is_configured = 0U;
	cdev->alt_setting = 0U;
	cdev->alt_setting_in = 0U;
	cdev->uac_isoc_out.last_xfer_len = 0U;
	cdev->uac_isoc_out.sof_idx = 0U;
	cdev->uac_isoc_out.data_idx = 0U;
	/* A control transfer cut by the reset never gets its data stage */
	cdev->ctrl_req_pending = 0U;

	/* DeInit ISOC IN EP */
	info = &ep_isoc_in->info;
	if ((cb != NULL) && (usbd_uac_ep_enable(&(cdev->cfg->in)) != 0U) && (info->addr != 0U)) {
		usbd_ep_deinit(dev, ep_isoc_in);
	}
	/* The aborted IN transfer never completes */
	ep_isoc_in->xfer_state = 0U;

	/* DeInit ISOC OUT EP */
	info = &ep_isoc_out->info;
	if ((cb != NULL) && (usbd_uac_ep_enable(&(cdev->cfg->out)) != 0U) && (info->addr != 0U)) {
		usbd_ep_deinit(dev, ep_isoc_out);
	}

	usb_os_exit_critical();
}

/**
  * @brief  Check freq requests
  * @param  freq: Freq requests from host
  * @param  speed: USB connection speed
  * @retval Status
  */
static bool usbd_uac_is_valid_sample_rate(u32 freq, u8 speed)
{
	u8 count;

	if (speed == USB_SPEED_HIGH) {
		count = USBD_UAC_HS_SAMPLING_FREQ_COUNT;
	} else {
		count = USBD_UAC_FS_SAMPLING_FREQ_COUNT;
	}

	for (u8 i = 0; i < count; ++i) {
		if (freq == usbd_uac_sampling_rates[i]) {
			return true;
		}
	}
	return false;
}

/**
  * @brief  Check a playback format against the ones the descriptors declare at this speed
  * @param  fmt: Playback stream format
  * @param  speed: USB connection speed
  * @retval Status
  */
static int usbd_uac_check_play_fmt(const usbd_audio_cfg_t *fmt, u8 speed)
{
	/* One alt setting per even channel count from 2 up to the max, all with the same width */
	if ((fmt->ch_cnt < USBD_UAC_CH_CNT_2) || (fmt->ch_cnt > USBD_UAC_DEFAULT_CH_CNT) || ((fmt->ch_cnt % 2U) != 0U) ||
		(fmt->byte_width != USBD_UAC_DEFAULT_BYTE_WIDTH) || (!usbd_uac_is_valid_sample_rate(fmt->sampling_freq, speed))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid play fmt %u/%u/%u\n", fmt->sampling_freq, fmt->ch_cnt, fmt->byte_width);
		return -RTK_ERR_BADARG;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Get the microphone stream format reported to the host
  * @note   cfg->in is the single source of the record format: the descriptors, the mic clock
  *         and the ISOC IN EP all follow it, and usbd_uac_config() only accepts the same format.
  * @param  cdev: UAC device instance
  * @retval Microphone stream format
  */
static const usbd_audio_cfg_t *usbd_uac_get_mic_fmt(const usbd_uac_dev_t *cdev)
{
	return (usbd_uac_ep_enable(&(cdev->cfg->in)) != 0U) ? &(cdev->cfg->in) : &usbd_uac_mic_default_fmt;
}

/**
  * @brief  Get the sampling frequency of the microphone clock
  * @note   Ref UAC 2.0 4.7.2.1: the mic has its own clock source, independent of the headphone
  *         one. The record path runs at a single rate, the one the app configured.
  * @param  cdev: UAC device instance
  * @retval Microphone sampling frequency in Hz
  */
static u32 usbd_uac_get_mic_sampling_freq(const usbd_uac_dev_t *cdev)
{
	return usbd_uac_get_mic_fmt(cdev)->sampling_freq;
}

/**
  * @brief  Check the microphone stream format configured by the app
  * @note   Ref UAC 2.0 Format Type I: a subslot is 1 to 4 bytes, 2 to 4 are supported here.
  *         The rate range is the one of the audio HAL capture path. A high-speed capable device
  *         shall also work at full speed, so the format shall fit a full-speed packet, which makes
  *         it fit a high-speed microframe too.
  * @param  fmt: Microphone stream format
  * @retval Status
  */
static int usbd_uac_check_mic_fmt(const usbd_audio_cfg_t *fmt)
{
	if (fmt->enable == 0U) {
		return RTK_SUCCESS;
	}

	if ((fmt->ch_cnt == 0U) || (fmt->ch_cnt > USBD_UAC_IN_MAX_CH_CNT) || (fmt->byte_width < USBD_UAC_BYTE_WIDTH_2) ||
		(fmt->byte_width > USBD_UAC_BYTE_WIDTH_4) || (fmt->sampling_freq < USBD_UAC_SAMPLING_FREQ_8K) ||
		(fmt->sampling_freq > USBD_UAC_SAMPLING_FREQ_192K)) {
		return -RTK_ERR_BADARG;
	}

	if (usbd_uac_get_mps(fmt, USB_SPEED_FULL) == 0U) {
		return -RTK_ERR_BADARG;
	}

	return RTK_SUCCESS;
}

/**
  * @brief  Handle the RANGE request of the microphone clock frequency control
  * @note   Ref UAC 2.0 5.2.1: report the single rate the record path supports, so the host
  *         can only select the rate the app produces data at.
  * @param  dev: USB device instance
  * @param  max_len: Maximum allowed length for response
  * @retval Status of the EP0 IN submit
  */
static int usbd_uac_mic_sampling_freq_range_request(usb_dev_t *dev, u16 max_len)
{
	usb_uac2_freq_t sampling_freq_range;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	u32 freq = usbd_uac_get_mic_sampling_freq(&usbd_uac_dev);
	u16 num_sub_ranges = 1U;
	u16 len = (u16)(sizeof(num_sub_ranges) + sizeof(usb_uac2_freq_t));

	usb_os_memcpy(ep0_in->xfer_buf, (u8 *)&num_sub_ranges, sizeof(num_sub_ranges));
	USBD_UAC_INIT_SUB_RANGE(sampling_freq_range, freq, freq, 0);
	usb_os_memcpy(ep0_in->xfer_buf + sizeof(num_sub_ranges), &sampling_freq_range, sizeof(sampling_freq_range));

	if (len > max_len) {
		len = max_len;
	}

	ep0_in->xfer_len = len;
	return usbd_ep_transmit(dev, ep0_in);
}

/**
  * @brief  Open the ISOC IN endpoint of the microphone AS interface
  * @note   Ref USB 2.0 9.4.10: the endpoints of an alternate setting become active on its
  *         SET_INTERFACE, a host may skip SET_CUR on a clock that offers a single rate.
  *         An IN transfer aborted by the re-init never completes, so xfer_state is cleared
  *         in the same critical section the record engine uses to submit.
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_isoc_in_ep_open(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	u16 mps;

	/* Defensive: SET_INTERFACE never selects an absent mic, refer to usbd_uac_mic_present() */
	if (usbd_uac_ep_enable(&(cdev->cfg->in)) == 0U) {
		return RTK_SUCCESS;
	}

	/* Size the EP from cfg->in, the format the descriptors report, not from the record ring
	   buffer which usbd_uac_config() may be rebuilding at the same time */
	mps = usbd_uac_get_mps(&(cdev->cfg->in), dev->dev_speed);
	if (mps == 0U) {
		return -RTK_ERR_BADARG;
	}

	usb_os_enter_critical();
	usbd_ep_deinit(dev, ep_isoc_in);
	ep_isoc_in->info.mps = mps;
	usbd_ep_init(dev, ep_isoc_in);
	ep_isoc_in->xfer_state = 0U;
	/* Audio queued while the host was not streaming is stale, start the new stream from now.
	   is_closing is raised under this lock before the ring is freed, uac_sema_valid is not */
	if ((cdev->uac_isoc_in.is_closing == 0U) && (cdev->uac_isoc_in.uac_sema_valid != 0U)) {
		usb_ringbuf_reset(&(cdev->uac_isoc_in.buf_list));
	}
	usb_os_exit_critical();

	return RTK_SUCCESS;
}

/**
  * @brief  Calculate the PC's intended volume level based on the UAC driver value
  * @param  x_points: Array of volume levels on the Windows PC
  * @param  y_points: Corresponding array of volume levels used by the UAC driver
  * @param  num_points: Number of data points (at least 2)
  * @param  target_y: The volume level from the UAC driver to be interpolated
  * @retval The intended volume level on the Windows PC corresponding to the UAC driver value
  */
static u8 usbd_uac_volume_linear_interpolation(const u8 x_points[], const s16 y_points[], u8 num_points, s16 target_y)
{
	u8 i;
	u8 x0;
	u8 x1;
	s16 y0;
	s16 y1;
	s32 numerator;
	s32 denominator;
	s32 x;

	if (target_y > 190 || target_y < -190) {
		return UABD_UAC_VOL_ERR_VAL;
	}

	for (i = 0; i < num_points - 1; i++) {
		y0 = y_points[i];
		y1 = y_points[i + 1];
		x0 = x_points[i];
		x1 = x_points[i + 1];

		if ((y0 <= target_y && target_y <= y1) || (y1 <= target_y && target_y <= y0)) {
			numerator = (s32)(target_y - y0) * (x1 - x0);
			denominator = (y1 - y0);
			x = (numerator + denominator / 2) / denominator + x0;
			if (x < 0) {
				x = 0;
			}
			if (x > 100) {
				x = 100;
			}
			return (u8)x;
		}
	}

	return UABD_UAC_VOL_ERR_VAL;
}

/**
  * @brief  Handle UAC specific CTRL requests
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: USB CTRL requests
  * @retval Status
  */
static int usbd_uac_setup(usb_dev_t *dev, usb_setup_req_t *req)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	/* Read once: usbd_uac_deinit() may clear it on another core */
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_ep_t *ep0_in = &dev->ep0_in;
	int ret = RTK_SUCCESS;
	u8 entityId, controlSelector;
	u8 ch_cnt;
	u8 byte_width;
	u8 alt_setting;
	u8 fmt_change = 0;

	switch (req->bmRequestType & USB_REQ_TYPE_MASK) {
	case USB_REQ_TYPE_STANDARD:
		switch (req->bRequest) {
		case USB_REQ_SET_INTERFACE:
			/* Ref USB 2.0 9.4.10: the endpoints of the selected interface return to their
			   default state, not halted and data toggle DATA0. All the endpoints of this
			   class are isochronous, they can neither be halted nor own a data toggle, so
			   nothing has to be reset here. */
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				alt_setting = USB_LOW_BYTE(req->wValue);
				/* Ref USB 2.0 Table 9-10: the whole wIndex is the interface number. Requests for
				   a foreign interface must be rejected so that composite dispatch is not broken
				   and a malformed wIndex cannot touch the streaming state. */
				if (req->wIndex == USB_UAC2_IF_IDX_AC_HEADSET) {
					/* The Audio Control interface owns alternate setting 0 only */
					if (alt_setting != 0U) {
						ret = -RTK_ERR_BADARG;
					}
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES) {
					/* Ref USB 2.0 9.4.5: alt setting 0 is the zero-bandwidth setting, it stops the
					   stream and leaves the negotiated format untouched */
					if (alt_setting == 0U) {
						cdev->alt_setting = 0U;
						/* Ref UAC 2.0 3.16.2: no data is sent on the zero-bandwidth setting. Disarm the
						   SOF underrun filler until the next real packet, else it keeps feeding the
						   reader synthetic nodes after the host stopped. The OUT EP stays armed so
						   that the next alt>0 streams at once, ISOC EPs have no halt or toggle state. */
						cdev->uac_isoc_out.last_xfer_len = 0U;
						cdev->uac_isoc_out.sof_idx = 0U;
						cdev->uac_isoc_out.data_idx = 0U;
					} else if (alt_setting != cdev->alt_setting) {
						switch (alt_setting) {
						case 1:
							byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
							ch_cnt = 2;
							break;
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_4 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
						case 2:
							byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
							ch_cnt = 4;
							break;
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_6 || USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
						case 3:
							byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
							ch_cnt = 6;
							break;
#endif
#if USBD_UAC_DEFAULT_CH_CNT == USBD_UAC_CH_CNT_8
						case 4:
							byte_width = USBD_UAC_DEFAULT_BYTE_WIDTH;
							ch_cnt = 8;
							break;
#endif
						default:
							/* Ref USB 2.0 9.4.5: an alt setting that does not exist is a request error,
							   the streaming state must stay on the previous setting */
							ret = -RTK_ERR_BADARG;
							break;
						}

						if (ret == RTK_SUCCESS) {
							cdev->alt_setting = alt_setting;
							if ((cdev->cur_byte_width != byte_width) && byte_width) {
								cdev->cur_byte_width = byte_width;
								fmt_change = 1;
							}
							if ((cdev->cur_ch_cnt != ch_cnt) && ch_cnt) {
								cdev->cur_ch_cnt = ch_cnt;
								fmt_change = 1;
							}
							if (fmt_change != 0) {
								if ((cb != NULL) && (cb->format_changed != NULL)) {
									cb->format_changed(cdev->cur_play_sampling_freq, cdev->cur_ch_cnt, cdev->cur_byte_width);
								}
							}
						}
					}
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
					/* The mic AS interface owns alt setting 0 and 1 only, and it is not reported
					   at all when the IN endpoint is disabled */
					if ((usbd_uac_mic_present(cdev) == 0U) || (alt_setting > USBD_UAC_AS_MIC_ALT_SETTING_MAX)) {
						ret = -RTK_ERR_BADARG;
					} else {
						/* Open the mic EP on entering the streaming setting only, a repeated
						   alt=1 must not abort the IN transfer in flight */
						if ((alt_setting != 0U) && (cdev->alt_setting_in == 0U)) {
							ret = usbd_uac_isoc_in_ep_open(dev);
						}
						/* Ref USB 2.0 9.4.10: a setting that can not be selected is a request error,
						   the interface stays on the previous one. The record engine tests the
						   setting under the same lock, possibly on another core. */
						if (ret == RTK_SUCCESS) {
							usb_os_enter_critical();
							cdev->alt_setting_in = alt_setting;
							usb_os_exit_critical();
						}
					}
				} else {
					ret = -RTK_ERR_BADARG;
				}
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USB_REQ_GET_INTERFACE:
			if (dev->dev_state == USBD_STATE_CONFIGURED) {
				/* Ref USB 2.0 Table 9-11: the whole wIndex is the interface number, and 9.4.4
				   requires a request error for an interface that does not exist */
				if (req->wIndex == USB_UAC2_IF_IDX_AC_HEADSET) {
					ep0_in->xfer_buf[0] = 0U;
				} else if (req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_HEADPHONES) {
					ep0_in->xfer_buf[0] = cdev->alt_setting;
				} else if ((req->wIndex == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) && (usbd_uac_mic_present(cdev) != 0U)) {
					ep0_in->xfer_buf[0] = cdev->alt_setting_in;
				} else {
					ret = -RTK_ERR_BADARG;
					break;
				}
				ep0_in->xfer_len = 1U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		case USB_REQ_GET_STATUS:
			/* Ref USB 2.0 9.4.5: an interface that does not exist is a request error. The core
			   already checked wValue, wLength and the device state. */
			if ((dev->dev_state == USBD_STATE_CONFIGURED) && (usbd_uac_is_owned_interface(cdev, req->wIndex) != 0U)) {
				ep0_in->xfer_buf[0] = 0U;
				ep0_in->xfer_buf[1] = 0U;
				ep0_in->xfer_len = 2U;
				ret = usbd_ep_transmit(dev, ep0_in);
			} else {
				ret = -RTK_ERR_BADARG;
			}
			break;

		default:
			ret = -RTK_ERR_BADARG;
			break;
		}
		break;/* case USB_REQ_TYPE_STANDARD */

	case USB_REQ_TYPE_CLASS :
		if ((req->bmRequestType & USB_REQ_RECIPIENT_MASK) != USB_REQ_RECIPIENT_INTERFACE) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		/* Ref UAC 2.0 5.2.2: the low byte of wIndex is the interface number and the high byte
		   is the entity ID. All the audio controls of this class live on the AC interface. */
		if (USB_LOW_BYTE(req->wIndex) != USB_UAC2_IF_IDX_AC_HEADSET) {
			ret = -RTK_ERR_BADARG;
			break;
		}
		entityId = USB_HIGH_BYTE(req->wIndex);
		controlSelector = USB_HIGH_BYTE(req->wValue);

		/* Ref UAC 2.0 5.2.2: the low byte of wValue is the channel number. Only the master
		   channel (0) controls are declared, and a Clock Source control shall use 0, so any other
		   channel addresses a control that does not exist (5.2.5.7). The mic entities are stripped
		   from the reported descriptor when the mic is absent, so a request naming one addresses a
		   non-existent entity. Rejecting it here also keeps its data stage away from ep0_data_out(),
		   which only runs for an accepted request. */
		if ((USB_LOW_BYTE(req->wValue) != 0U) ||
			((usbd_uac_is_mic_entity(entityId) != 0U) && (usbd_uac_mic_present(cdev) == 0U))) {
			ret = -RTK_ERR_BADARG;
			break;
		}

		if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_cur_sampling_freq_request(dev, cdev->cur_record_sampling_freq);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						ret = usbd_uac_mic_sampling_freq_range_request(dev, req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 0);
						ret = -RTK_ERR_BADARG;
					}
				} else if (controlSelector == USB_UAC2_CS_CLK_VALID_CONTROL) {
					/* Ref UAC 2.0 4.7.2.1: bmControls declares the validity control read-only */
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_clk_valid_request(dev, cdev->cur_clk_valid);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 31);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 1);
					ret = -RTK_ERR_BADARG;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_cur_sampling_freq_request(dev, cdev->cur_play_sampling_freq);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						ret = usbd_uac_sampling_freq_ctrl_range_request(dev, req->wLength);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 2);
						ret = -RTK_ERR_BADARG;
					}
				} else if (controlSelector == USB_UAC2_CS_CLK_VALID_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ret = usbd_uac_clk_valid_request(dev, cdev->cur_clk_valid);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 3);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 4);
					ret = -RTK_ERR_BADARG;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT:
				if (controlSelector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ep0_in->xfer_buf[0] = cdev->cur_mute;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 9);
						ret = -RTK_ERR_BADARG;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->cur_volume), 2);
						ep0_in->xfer_len = MIN(2U, (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = (cdev->cfg->volume_min != 0U) ? cdev->cfg->volume_min : USBD_UAC_DEFAULT_VOLUME_MIN;
						response.wMAX = (cdev->cfg->volume_max != 0U) ? cdev->cfg->volume_max : USBD_UAC_DEFAULT_VOLUME_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&response, sizeof(response));
						ep0_in->xfer_len = MIN((u32)sizeof(response), (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 10);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 11);
					ret = -RTK_ERR_BADARG;
				}
				break;

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 mic FU
				if (controlSelector == USB_UAC_FU_MUTE) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						ep0_in->xfer_buf[0] = cdev->cur_mute_in;
						ep0_in->xfer_len = 1U;
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 20);
						ret = -RTK_ERR_BADARG;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *) & (cdev->cur_volume_in), 2);
						ep0_in->xfer_len = MIN(2U, (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						usb_uac2_ctrl_range_layout2_struct response;
						response.wNumSubRanges = 1;
						response.wMIN = (cdev->cfg->volume_min != 0U) ? cdev->cfg->volume_min : USBD_UAC_DEFAULT_VOLUME_MIN;
						response.wMAX = (cdev->cfg->volume_max != 0U) ? cdev->cfg->volume_max : USBD_UAC_DEFAULT_VOLUME_MAX;
						response.wRES = 1;

						usb_os_memcpy((void *)ep0_in->xfer_buf, (const void *)&response, sizeof(response));
						ep0_in->xfer_len = MIN((u32)sizeof(response), (u32)req->wLength);
						ret = usbd_ep_transmit(dev, ep0_in);
					} else {
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 21);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 22);
					ret = -RTK_ERR_BADARG;
				}
				break;

			default:
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 12);
				ret = -RTK_ERR_BADARG;
				break;
			}
		} else {
			/* USB_H2D */
			switch (entityId) {
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE:
			case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES:
				if (controlSelector == USB_UAC_CS_SAM_FREQ_CONTROL) {
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.1.1: dCUR of the frequency control, Layout 3 */
						ret = usbd_uac_receive_control_data(dev, req, 4U);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						/* Ref UAC 2.0 5.2.1: RANGE is defined for the D2H direction only */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 26);
						ret = -RTK_ERR_BADARG;
					} else {
						/* Set freq err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 13);
						ret = -RTK_ERR_BADARG;
					}
				} else {
					/* Set freq ctrl err*/
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 14);
					ret = -RTK_ERR_BADARG;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES */

			case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT: //0x05 FU
				if (controlSelector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.1: bCUR of the mute control, Layout 1 */
						ret = usbd_uac_receive_control_data(dev, req, 1U);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						/* Ref UAC 2.0 5.2.1: RANGE is defined for the D2H direction only */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 27);
						ret = -RTK_ERR_BADARG;
					} else {
						/* Set cur mute err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 15);
						ret = -RTK_ERR_BADARG;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.2: wCUR of the volume control, Layout 2 */
						ret = usbd_uac_receive_control_data(dev, req, 2U);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						/* Ref UAC 2.0 5.2.1: RANGE is defined for the D2H direction only */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 28);
						ret = -RTK_ERR_BADARG;
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 16);
						ret = -RTK_ERR_IO;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 17);
					ret = -RTK_ERR_IO;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT */

			case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT: //0x08 mic FU
				if (controlSelector == USB_UAC_FU_MUTE) { //mute
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.1: bCUR of the mute control, Layout 1 */
						ret = usbd_uac_receive_control_data(dev, req, 1U);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						/* Ref UAC 2.0 5.2.1: RANGE is defined for the D2H direction only */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 29);
						ret = -RTK_ERR_BADARG;
					} else {
						/* Set cur mute err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 23);
						ret = -RTK_ERR_BADARG;
					}
				} else if (controlSelector == USB_UAC_FU_VOLUME) { //volume
					if (req->bRequest == USB_UAC_REQ_CUR) {
						/* Ref UAC 2.0 5.2.5.7.2: wCUR of the volume control, Layout 2 */
						ret = usbd_uac_receive_control_data(dev, req, 2U);
					} else if (req->bRequest == USB_UAC_REQ_RANGE) {
						/* Ref UAC 2.0 5.2.1: RANGE is defined for the D2H direction only */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 30);
						ret = -RTK_ERR_BADARG;
					} else {
						/* Set cur volume range err */
						USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 24);
						ret = -RTK_ERR_IO;
					}
				} else {
					/* Set fu err */
					USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 25);
					ret = -RTK_ERR_IO;
				}
				break;/* case USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT */

			default:
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 18);
				ret = -RTK_ERR_BADARG;
				break;
			}
		}
		break;/* case USB_REQ_TYPE_CLASS */

	case USB_REQ_TYPE_VENDOR:
		if ((cb == NULL) || (cb->setup == NULL)) {
			/* No handler for vendor requests, STALL so that the host recovers promptly
			   instead of waiting out the data stage */
			ret = -RTK_ERR_BADARG;
		} else if (req->wLength != 0U) {
			if ((req->bmRequestType & USB_REQ_DIR_MASK) == USB_D2H) {
				u16 rsp_len = req->wLength;

				/* Ref USB 2.0 9.3.4: wLength is the maximum the host accepts, never trust it
				   as a buffer size. Clamp to the EP0 buffer to avoid an over-write. */
				if (rsp_len > ep0_in->xfer_buf_len) {
					rsp_len = (u16)ep0_in->xfer_buf_len;
				}

				/* EP0 buffer is shared with descriptor and OUT traffic, clear the response
				   window so a callback writing fewer bytes cannot leak stale data. */
				usb_os_memset((void *)ep0_in->xfer_buf, 0, rsp_len);
				ret = cb->setup(req, ep0_in->xfer_buf);
				if (ret == RTK_SUCCESS) {
					ep0_in->xfer_len = rsp_len;
					/* Propagate a submit failure so that the core stalls EP0 instead of
					   leaving the host waiting out the data stage, ref USB 2.0 8.5.3.4 */
					ret = usbd_ep_transmit(dev, ep0_in);
				}
			} else {
				/* Ref USB 2.0 8.5.3: an H2D request with wLength > 0 carries the payload in a
				   following data stage, the request is handed to the app with it, refer to
				   usbd_uac_handle_ep0_data_out() */
				ret = usbd_uac_receive_control_data(dev, req, req->wLength);
			}
		} else {
			/* No data stage, the setup packet is self-contained, dispatch it right away */
			ret = cb->setup(req, NULL);
		}
		break;

	default:
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_SETUP, 19);
		ret = -RTK_ERR_IO;
		break;
	}

	return ret;
}

/**
  * @brief  Handles the SOF event for the UAC device
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @retval None
  */
static void usbd_uac_handle_sof(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);

	if ((pdata_ctrl->next_xfer != 0) && (pdata_ctrl->last_xfer_len != 0)) {
		pdata_ctrl->sof_idx ++;

		if ((pdata_ctrl->sof_idx > pdata_ctrl->data_idx) &&
			((pdata_ctrl->sof_idx - pdata_ctrl->data_idx) >= USBD_UAC_ISOC_RX_APPEND_ZLP_DIFF_MAX)) {
			pdata_ctrl->data_idx = pdata_ctrl->sof_idx;
			usbd_uac_append_data(pdata_ctrl);
		}
	}

	if ((cdev->uac_isoc_in.next_xfer != 0) && (cdev->ep_isoc_in.xfer_state == 0)) {
		usbd_uac_isoc_in_process_xfer(dev);
	}

	if ((cb != NULL) && (cb->sof != NULL)) {
		cb->sof();
	}
}

/**
  * @brief  uac_handle_ep0_data_out
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  *         Handle EP0 Rx Ready event
  * @param  dev: USB device instance
  * @retval Status
  */
static int usbd_uac_handle_ep0_data_out(usb_dev_t *dev)
{
	int ret = RTK_SUCCESS;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb = cdev->cb;
	usb_setup_req_t *p_ctrl_req = &(cdev->ctrl_req);
	usbd_ep_t *ep0_out = &dev->ep0_out;
	u32 freq;
	s16 volume_value = 0;
	u8 num_points;
	u8 target_volume = UABD_UAC_VOL_ERR_VAL;

	/* No pending request means this data stage does not belong to UAC, the composite dispatcher
	   already routed it by active_func. cb is released by usbd_uac_deinit(), which may run between
	   the setup and the data stage of an H2D request, so it is checked here as well. */
	if ((cdev->ctrl_req_pending == 0U) || (cb == NULL)) {
		return RTK_SUCCESS;
	}

	/* Consume the pending request: a single data stage belongs to exactly one setup packet, so
	   the saved request must not be replayed by a later EP0 OUT event. */
	cdev->ctrl_req_pending = 0U;

	if ((((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_CLASS) && ((p_ctrl_req->bmRequestType & 0x1FU) == USB_REQ_RECIPIENT_INTERFACE))
		&& (p_ctrl_req->bRequest == USB_UAC_REQ_CUR)) {
		if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_OUTPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_MUTE) && (p_ctrl_req->wLength == 0x01)) {
				if (cb->mute_changed != NULL) {
					cdev->cur_mute = (ep0_out->xfer_buf[0]) ? 1 : 0;
					cb->mute_changed(cdev->cur_mute);
				}
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (p_ctrl_req->wLength == 0x02)) {
				volume_value = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);

				num_points = sizeof(usbd_uac_pc_vol_lvl) / sizeof(usbd_uac_pc_vol_lvl[0]);
				if (num_points != sizeof(usbd_uac_drv_vol) / sizeof(usbd_uac_drv_vol[0])) {
					RTK_LOGS(TAG, RTK_LOG_ERROR, "Volume arrays length err.\n");
					ret = -RTK_ERR_BADARG;
				} else {
					target_volume = usbd_uac_volume_linear_interpolation(usbd_uac_pc_vol_lvl, usbd_uac_drv_vol, num_points, (int)volume_value);
				}

				if ((target_volume != UABD_UAC_VOL_ERR_VAL) && (cb->volume_changed != NULL)) {
					cdev->cur_volume = volume_value;
					cb->volume_changed(target_volume);
				}
			}
		} else if (USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT) {
			if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_MUTE) && (p_ctrl_req->wLength == 0x01)) {
				cdev->cur_mute_in = (ep0_out->xfer_buf[0]) ? 1 : 0;
			} else if ((USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_FU_VOLUME) && (p_ctrl_req->wLength == 0x02)) {
				cdev->cur_volume_in = (u16)ep0_out->xfer_buf[0] + ((u16)(ep0_out->xfer_buf[1]) << 8);
			}
		}

		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_MICROPHONE)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x04)) {
			freq = ((u32)ep0_out->xfer_buf[3] << 24) | ((u32)ep0_out->xfer_buf[2] << 16) | ((u32)ep0_out->xfer_buf[1] << 8) | (u32)ep0_out->xfer_buf[0];
			/* Ref UAC 2.0 4.7.2.1: the mic clock is independent of the headphone one, it only
			   offers the record rate, so the mic EP keeps its packet size and playback is not touched */
			if (freq == usbd_uac_get_mic_sampling_freq(cdev)) {
				cdev->cur_record_sampling_freq = freq;
			} else {
				/* Ref USB 2.0 9.2.7: an unsupported value is a request error, STALL the status stage */
				ret = -RTK_ERR_BADARG;
			}
		}

		if ((USB_HIGH_BYTE(p_ctrl_req->wIndex) == USBD_UAC_CTRL_ENTITYID_CLOCK_HEADSET_HEADPHONES)
			&& (USB_HIGH_BYTE(p_ctrl_req->wValue) == USB_UAC_CS_SAM_FREQ_CONTROL) && (p_ctrl_req->wLength == 0x04)) {
			freq = ((u32)ep0_out->xfer_buf[3] << 24) | ((u32)ep0_out->xfer_buf[2] << 16) | ((u32)ep0_out->xfer_buf[1] << 8) | (u32)ep0_out->xfer_buf[0];
			if (usbd_uac_is_valid_sample_rate(freq, dev->dev_speed)) {
				if (cdev->cur_play_sampling_freq != freq) {
					/* The OUT EP and the ring are sized for the widest declared format, refer to
					   usbd_uac_get_play_mps(), so a packet at the new rate already fits: the stream
					   is left running and the application restarts it with the new format. */
					cdev->cur_play_sampling_freq = freq;

					if (cb->format_changed != NULL) {
						cb->format_changed(cdev->cur_play_sampling_freq, cdev->cur_ch_cnt, cdev->cur_byte_width);
					}
				}
			} else {
				/* Ref USB 2.0 9.2.7: an unsupported value is a request error, STALL the status stage */
				ret = -RTK_ERR_BADARG;
			}
		}

	} else if ((p_ctrl_req->bmRequestType & USB_REQ_TYPE_MASK) == USB_REQ_TYPE_VENDOR) {
		/* Ref USB 2.0 8.5.3.1: the status stage reports the outcome, let the app decide */
		if (cb->setup != NULL) {
			ret = cb->setup(p_ctrl_req, ep0_out->xfer_buf);
		} else {
			ret = -RTK_ERR_BADARG;
		}
	} else {
		/* Other class requests are rejected in the setup stage */
	}

	return ret;
}

/**
  * @brief  Pop one packet from the record TX ring buffer and transmit it on the ISOC IN endpoint.
  * @note   ISR context (SOF trigger / IN completion retrigger) or task context (usbd_uac_transmit_data
  *         kick). Nothing is submitted while the mic streaming setting is not selected, the EP is
  *         then closed. usb_ringbuf_remove_head() returns 0 when the ring buffer is empty, which naturally
  *         results in a ZLP transmit so the endpoint keeps being serviced every interval.
  *         The xfer_state test-and-set, the head pop and the submit run in one critical section, so
  *         the ISR and the record task, possibly on another SMP core, can not both pop the head or
  *         both submit into the busy usbd_uac_tx_buf.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_uac_isoc_in_process_xfer(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	u32 len;

	usb_os_enter_critical();

	/* next_xfer is cleared under this lock before the ring is released, refer to
	   usbd_uac_ep_buf_ctrl_deinit(), so it is tested again here */
	if ((ep_isoc_in->xfer_state == 0U) && (pdata_ctrl->next_xfer != 0U) && (cdev->alt_setting_in != 0U)) {
		len = usb_ringbuf_remove_head(&(pdata_ctrl->buf_list), usbd_uac_tx_buf, pdata_ctrl->isoc_mps, NULL);

		ep_isoc_in->xfer_state = 1U;
		ep_isoc_in->xfer_buf = usbd_uac_tx_buf;
		ep_isoc_in->xfer_len = (u16)len;
		/* On failure no completion IRQ fires to clear xfer_state; clear it here. */
		if (usbd_ep_transmit(dev, ep_isoc_in) != RTK_SUCCESS) {
			ep_isoc_in->xfer_state = 0U;
		}
	}

	usb_os_exit_critical();
}

/**
  * @brief  Handle completion of an ISOC IN transfer for the record engine, then retrigger
  *         if more data is queued.
  * @note   ISR context.
  * @param  dev: USB device instance
  * @retval void
  */
static void usbd_uac_isoc_in_process_complete(usb_dev_t *dev)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	cdev->ep_isoc_in.xfer_state = 0U;

	if (cdev->uac_isoc_in.next_xfer != 0) {
		usbd_uac_isoc_in_process_xfer(dev);
	}
}

/**
  * @brief  Data sent on non-control IN endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
static int usbd_uac_handle_ep_data_in(usb_dev_t *dev, u8 ep_addr, int status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;

	/* Return non-zero if EP is not owned by UAC2, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->isoc_in_addr) {
		return -RTK_ERR_BADARG;
	}

	if (status != RTK_SUCCESS) {
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
	}

	usbd_uac_isoc_in_process_complete(dev);

	return RTK_SUCCESS;
}

/**
  * @brief  Data received on non-control Out endpoint
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  ep_addr: endpoint address
  * @retval Status
  */
/**
  * @brief  Push a node to a ring buffer, dropping the oldest one if it is full.
  * @note   ISR context for the OUT ring, task context for the IN ring. The other side pops the
  *         head, possibly on another SMP core, so the head drop and the tail add are serialized
  *         with it. The ring is released only after next_xfer was cleared under the same lock,
  *         refer to usbd_uac_ep_buf_ctrl_deinit(), so nothing is pushed once it is 0.
  * @param  pdata_ctrl: buffer control structure of the ring
  * @param  buf: node payload, NULL for a filler node
  * @param  len: node length
  * @param  valid: 1 for real data, 0 for a filler node
  * @retval 1 if the oldest node was dropped, 0 otherwise
  */
static u8 usbd_uac_ringbuf_push(usbd_uac_buf_ctrl_t *pdata_ctrl, const u8 *buf, u32 len, u8 valid)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);
	u8 tmp[1];
	u8 dropped = 0U;

	usb_os_enter_critical();
	if (pdata_ctrl->next_xfer != 0U) {
		if (usb_ringbuf_is_full(buf_list) != 0) {
			usb_ringbuf_remove_head(buf_list, tmp, 1, NULL);
			dropped = 1U;
		}
		usb_ringbuf_add_tail(buf_list, buf, len, valid);
	}
	usb_os_exit_critical();

	return dropped;
}

/**
  * @brief  Append a filler node to the OUT ring buffer when the host stops sending data.
  * @note   ISR context (invoked from SOF path).
  */
static void usbd_uac_append_data(usbd_uac_buf_ctrl_t *pdata_ctrl)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);

	if (usbd_uac_ringbuf_push(pdata_ctrl, NULL, pdata_ctrl->last_xfer_len, 0U) != 0U) {
#if USBD_UAC_DEBUG
		pdata_ctrl->append_overwrite_cnt++;
#endif
	}
#if USBD_UAC_DEBUG
	pdata_ctrl->append_zlp_cnt++;
#endif

	if ((pdata_ctrl->read_wait_sema != 0) && (usb_ringbuf_is_empty(buf_list) == 0)) {
		rtos_sema_give(pdata_ctrl->uac_isoc_sema);
	}
}

static int usbd_uac_handle_ep_data_out(usb_dev_t *dev, u8 ep_addr, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	int ret = RTK_SUCCESS;

	UNUSED(dev);

	/* Return non-zero if EP is not owned by UAC2, so composite
	 * dispatcher can continue iterating to the correct sub-function. */
	if (ep_addr != cdev->cfg->isoc_out_addr) {
		return -RTK_ERR_BADARG;
	}

	if (pdata_ctrl->next_xfer != 0) {
		if (len == 0) { //ZLP
			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(dev, ep_isoc_out);
		} else if (len > pdata_ctrl->isoc_mps) {
			/* Longer than a ring node, which is sized for the widest declared format, refer to
			   usbd_uac_get_play_mps(): the host broke its wMaxPacketSize. Drop it rather than
			   queue a truncated packet. */
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(dev, ep_isoc_out);
		} else {
			pdata_ctrl->last_xfer_len = (u16)len;
			pdata_ctrl->xfer_cnt ++;
			pdata_ctrl->data_idx ++;

#if USBD_UAC_DEBUG
			u32 rx_new_tick = usb_os_get_timestamp_us();
			u32 step = (cdev->dev->dev_speed == USB_SPEED_HIGH) ? 185U : 1500U;
			if ((pdata_ctrl->last_xfer_tick > 0) && ((rx_new_tick - pdata_ctrl->last_xfer_tick) > step)) {
				pdata_ctrl->timeout_cnt++;
			}
			pdata_ctrl->last_xfer_tick = rx_new_tick;
#endif

			if (usbd_uac_ringbuf_push(pdata_ctrl, usbd_uac_rx_buf, len, 1U) != 0U) {
#if USBD_UAC_DEBUG
				pdata_ctrl->overwrite_cnt++;
#endif
			}

			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
			ret = usbd_ep_receive(dev, ep_isoc_out);

			if ((pdata_ctrl->read_wait_sema != 0) && (usb_ringbuf_is_empty(buf_list) == 0)) {
				rtos_sema_give(pdata_ctrl->uac_isoc_sema);
			}
		}
	} else {
		ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
		ep_isoc_out->xfer_len = pdata_ctrl->isoc_mps;
		/* Only fails once the device is no longer ready, the OUT EP then stays NAKing until
		   the next start_play() re-arms it: record it, there is nothing to retry in an ISR */
		if (usbd_ep_receive(dev, ep_isoc_out) != RTK_SUCCESS) {
			USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, ep_addr);
		}
	}

	return ret;
}
/**
  * @brief  Patch the runtime-assigned fields in the configuration descriptor
  * @note   Replaces direction-only EP placeholders with the actual addresses from the class
  *         configuration, rewrites the iInterface string index with the index the core
  *         assigned, and rewrites the mic format fields with cfg->in. Fields are located by
  *         descriptor type, subtype and entity / interface, and written byte by byte because
  *         the buffer may be unaligned in composite mode.
  * @param  desc: Pointer to the descriptor buffer (after configuration descriptor header)
  * @param  len: Total length of descriptor data
  * @param  speed: Speed the descriptor describes, the other speed for an Other-Speed descriptor
  * @retval void
  */
static void usbd_uac_patch_desc(u8 *desc, u16 len, u8 speed)
{
	u16 i;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_config_t *cfg = cdev->cfg;
	const usbd_audio_cfg_t *mic = usbd_uac_get_mic_fmt(cdev);
	u8 mic_ch_cfg = usbd_uac_get_ch_config(mic->ch_cnt);
	u16 mic_mps = usbd_uac_get_mps(mic, speed);
	u8 itf_num = 0xFFU;
	u8 itf_subclass = 0U;

	for (i = 0; i < len;) {
		u8 dlen;
		u8 dtype;
		/* bLength and bDescriptorType are read only once both are inside the buffer */
		if (((u32)i + 2U) > len) {
			break;
		}
		dlen = desc[i];
		if ((dlen < 2U) || (((u32)i + dlen) > len)) {
			break;
		}
		dtype = desc[i + 1];
		if ((dtype == USB_DESC_TYPE_ENDPOINT) && (dlen >= USB_LEN_EP_DESC)) {
			u8 addr = desc[i + 2];
			u8 dir = addr & USB_REQ_DIR_MASK;
			u8 type = desc[i + 3] & 0x03;
			if ((dir == USB_D2H) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->isoc_in_addr;
				if (itf_num == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE) {
					desc[i + 4] = USB_LOW_BYTE(mic_mps);      /* wMaxPacketSize */
					desc[i + 5] = USB_HIGH_BYTE(mic_mps);
				}
			} else if ((dir == USB_H2D) && (type == USB_CH_EP_TYPE_ISOC)) {
				desc[i + 2] = cfg->isoc_out_addr;
			}
		} else if ((dtype == USB_DESC_TYPE_INTERFACE) && (dlen >= USB_LEN_IF_DESC)) {
			itf_num = desc[i + 2];
			itf_subclass = desc[i + 6];
			if (itf_subclass == USB_UAC_SUBCLASS_AUDIOCONTROL) {
				/* Only the audio control interface is named, matching the CDC ACM class. */
				desc[i + 8] = cdev->itf_str_idx;     /* iInterface */
			}
		} else if ((dtype == USB_UAC_CS_INTERFACE) && (dlen >= 3U)) {
			if ((itf_subclass == USB_UAC_SUBCLASS_AUDIOCONTROL) && (desc[i + 2] == USB_UAC_AC_INPUT_TERMINAL) &&
				(dlen >= USB_UAC2_LEN_AC_IN_TTY_DESC) && (desc[i + 3] == USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_HEADSET_MICROPHONE)) {
				/* Ref UAC 2.0 4.7.2.4: mic input terminal channel cluster */
				desc[i + 8] = mic->ch_cnt;               /* bNrChannels */
				desc[i + 9] = mic_ch_cfg;                /* bmChannelConfig */
				desc[i + 10] = 0U;
				desc[i + 11] = 0U;
				desc[i + 12] = 0U;
			} else if ((itf_subclass == USB_UAC_SUBCLASS_AUDIOSTREAMING) && (itf_num == USB_UAC2_IF_IDX_AS_HEADSET_MICROPHONE)) {
				if ((desc[i + 2] == USB_UAC_AS_GENERAL) && (dlen >= USB_UAC2_LEN_AS_IF_ALT_SET_DESC)) {
					/* Ref UAC 2.0 4.9.2: mic AS interface channel cluster */
					desc[i + 10] = mic->ch_cnt;          /* bNrChannels */
					desc[i + 11] = mic_ch_cfg;           /* bmChannelConfig */
					desc[i + 12] = 0U;
					desc[i + 13] = 0U;
					desc[i + 14] = 0U;
				} else if ((desc[i + 2] == USB_UAC_AS_FORMAT_TYPE) && (dlen >= USB_UAC2_LEN_AS_FRT_TYPE_ALT_SET_DESC)) {
					/* Ref UAC 2.0 Format Type I: subslot size and effectively used bits */
					desc[i + 4] = mic->byte_width;       /* bSubslotSize */
					desc[i + 5] = (u8)USBD_UAC_BIT_WIDTH(mic->byte_width); /* bBitResolution */
				}
			}
		}
		i += dlen;
	}
}

/**
  * @brief  Build the configuration descriptor to report to the host.
  *         When the mic (USB IN) is present the full duplex descriptor (playback + mic) is
  *         reported, with the mic Feature Unit trimmed to the cfg->in channel count. Otherwise
  *         the mic AC entities and AS interface are stripped so the reported descriptor is
  *         OUT-only.
  * @note   keep_mic is set when the app enabled the IN endpoint (cfg->in), in standalone and
  *         composite mode alike. A playback-only build (IN disabled) reports the OUT-only baseline.
  *         The source is copied around the dropped range, so no overlapping copy is needed.
  * @param  buf: Output buffer
  * @param  buf_len: Capacity of buf in bytes
  * @param  desc: Source static config descriptor (full duplex, mic entities included)
  * @param  full_len: sizeof of desc
  * @param  keep_mic: whether to keep the mic entities/interface in the reported descriptor
  * @retval Actual descriptor length written to buf, or 0 if it does not fit
  */
static u16 usbd_uac_build_config_desc(u8 *buf, u16 buf_len, const u8 *desc, u16 full_len, u8 keep_mic)
{
	u32 ac_mic_off = USB_LEN_CFG_DESC + USB_LEN_IAD_DESC + USBD_UAC_AC_IF_LEN(USBD_UAC_DEFAULT_CH_CNT) - USBD_UAC_AC_MIC_IF_LEN;
	u32 fu_off = ac_mic_off + USB_UAC2_LEN_CLK_SRC_DESC + USB_UAC2_LEN_AC_IN_TTY_DESC;
	u32 ac_off = USB_LEN_CFG_DESC + USB_LEN_IAD_DESC + USB_LEN_IF_DESC + 6U;
	u32 cut_off;
	u32 cut_len;
	u32 tail_len;
	u32 out_len;
	u16 ac_tot;
	u8 ch_cnt = usbd_uac_get_mic_fmt(&usbd_uac_dev)->ch_cnt;

	if (keep_mic) {
		/* The trim below relies on the mic FU layout, check it is where it is expected */
		if ((desc[fu_off] != USBD_UAC_AC_FU_HEAD_DESC_LEN(USBD_UAC_IN_MAX_CH_CNT)) || (desc[fu_off + 2U] != USB_UAC_AC_FEATURE_UNIT) ||
			(desc[fu_off + 3U] != USBD_UAC_CTRL_ENTITYID_INPUTTERMINAL_FEATUREUNIT)) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Mic FU not found\n");
			return 0;
		}
		/* Ref UAC 2.0 4.7.2.8: one bmaControls per channel plus the master one. Drop the
		   entries of the reserved channels cfg->in does not use, they sit right before iFeature. */
		cut_off = fu_off + USBD_UAC_AC_FU_BMA_CONTROLS_OFFSET + (((u32)ch_cnt + 1U) * USBD_UAC_AC_FU_BMA_CONTROLS_SIZE);
		cut_len = ((u32)USBD_UAC_IN_MAX_CH_CNT - ch_cnt) * USBD_UAC_AC_FU_BMA_CONTROLS_SIZE;
		tail_len = 0U;
	} else {
		/* Drop the mic AC entities and the trailing mic AS interface */
		cut_off = ac_mic_off;
		cut_len = USBD_UAC_AC_MIC_IF_LEN;
		tail_len = USBD_UAC_AS_TIF_LEN(USBD_UAC_MIC_AS_ALT_SETTING_NUM);
	}

	out_len = (u32)full_len - cut_len - tail_len;
	/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
	if (out_len > buf_len) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Cfg desc OVSZ %d > %d\n", out_len, buf_len);
		return 0;
	}

	usb_os_memcpy((void *)buf, (const void *)desc, cut_off);
	usb_os_memcpy((void *)(buf + cut_off), (const void *)(desc + cut_off + cut_len), out_len - cut_off);

	if (keep_mic) {
		buf[fu_off] = (u8)USBD_UAC_AC_FU_HEAD_DESC_LEN(ch_cnt); /* mic FU bLength */
	} else {
		buf[4]--;                    /* bNumInterfaces: drop the mic AS interface */
		buf[USB_LEN_CFG_DESC + 3]--; /* IAD bInterfaceCount: drop the mic AS interface */
	}

	/* Shrink the CS AC-Header wTotalLength by the dropped AC bytes */
	ac_tot = (u16)(buf[ac_off] | (buf[ac_off + 1U] << 8));
	ac_tot = (u16)(ac_tot - cut_len);
	buf[ac_off] = USB_LOW_BYTE(ac_tot);
	buf[ac_off + 1U] = USB_HIGH_BYTE(ac_tot);

	return (u16)out_len;
}

/**
  * @brief  Get descriptor callback
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  req: Setup request handle
  * @param  buf: Poniter to Buffer
  * @retval Descriptor length
  */
static u16 usbd_uac_get_descriptor(usb_dev_t *dev, usb_setup_req_t *req, u8 *buf, u16 buf_len)
{
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	usb_speed_type_t speed = dev->dev_speed;
#endif
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const u8 *desc = NULL;
	const u8 *cfg_desc = NULL;
	u16 len = 0;
	u8 type = USB_HIGH_BYTE(req->wValue);
	u8 desc_speed = USB_SPEED_FULL;

	/* Keep the mic entities/interface in the reported descriptor only when the app enabled
	 * the IN endpoint, in standalone and composite mode alike. */
	u8 keep_mic = usbd_uac_mic_present(cdev);

#ifdef CONFIG_SUPPORT_USB_FS_ONLY
	UNUSED(dev);
#endif

	switch (type) {

	case USB_DESC_TYPE_DEVICE:
		desc = usbd_uac_dev_desc;
		len = sizeof(usbd_uac_dev_desc);
		break;

	case USB_DESC_TYPE_CONFIGURATION:
		/* Built from the const template only: isoc_mps is sized once by usbd_uac_config(),
		   rewriting it here would race the ISRs that arm the EPs with it */
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
		if (speed == USB_SPEED_HIGH) {
			cfg_desc = usbd_uac_hs_config_desc;
			len = sizeof(usbd_uac_hs_config_desc);
			desc_speed = USB_SPEED_HIGH;
		} else
#endif
		{
			cfg_desc = usbd_uac_fs_config_desc;
			len = sizeof(usbd_uac_fs_config_desc);
		}
		break;

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	case USB_DESC_TYPE_DEVICE_QUALIFIER:
		desc = usbd_uac_device_qualifier_desc;
		len = sizeof(usbd_uac_device_qualifier_desc);
		break;

	case USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION:
		if (speed == USB_SPEED_HIGH) {
			cfg_desc = usbd_uac_fs_config_desc;
			len = sizeof(usbd_uac_fs_config_desc);
		} else {
			cfg_desc = usbd_uac_hs_config_desc;
			len = sizeof(usbd_uac_hs_config_desc);
			desc_speed = USB_SPEED_HIGH;
		}
		break;
#endif

	case USB_DESC_TYPE_STRING:
		/* Every string this class owns is registered with usbd_add_string() and answered by
		   the core, which only forwards an index it does not know, e.g. the MS OS string */
		USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_GET_DESC, 0);
		break;

	default:
		break;
	}

	if (desc != NULL) {
		/* Truncation is not allowed: a short descriptor is illegal, so stall instead */
		if (len > buf_len) {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "Desc %d OVSZ %d > %d\n", type, len, buf_len);
			return 0;
		}

		usb_os_memcpy((void *)buf, (const void *)desc, len);
	}

	if (cfg_desc != NULL) {
		/* The builder strips the mic entities when needed and bounds buf itself */
		len = usbd_uac_build_config_desc(buf, buf_len, cfg_desc, len, keep_mic);
		if (len == 0U) {
			return 0;
		}

		buf[USB_CFG_DESC_OFFSET_TYPE] = type;
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN] = USB_LOW_BYTE(len);
		buf[USB_CFG_DESC_OFFSET_TOTAL_LEN + 1] = USB_HIGH_BYTE(len);

		usbd_uac_patch_desc(buf + USB_LEN_CFG_DESC, len - USB_LEN_CFG_DESC, desc_speed);
	}

	return len;
}

/**
  * @brief  USB attach status change
  * @note   This function is called within an interrupt service routine (ISR) context;
  *         time-consuming operations (e.g., `malloc`, `rtos_sema_take`) are not permitted.
  * @param  dev: USB device instance
  * @param  old_status: USB old attach status
  * @param  status: USB attach status
  * @retval void
  */
static void usbd_uac_status_changed(usb_dev_t *dev, u8 old_status, u8 status)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	const usbd_uac_cb_t *cb;

	UNUSED(dev);

	if (status == USBD_ATTACH_STATUS_DETACHED) {
		/* Detach: stop tx/rx and wake up the read thread blocked in usbd_uac_read.
		   ISR context here: only rtos_sema_give is allowed (no malloc/sema_take). */
		usbd_uac_stop_play();
		usbd_uac_stop_record();
		if ((cdev->uac_isoc_out.uac_sema_valid != 0) && (cdev->uac_isoc_out.read_wait_sema != 0)) {
			rtos_sema_give(cdev->uac_isoc_out.uac_isoc_sema);
		}
	}

	/* cb is released by usbd_uac_deinit(), which may run concurrently with this ISR: read once */
	cb = cdev->cb;
	if ((cb != NULL) && (cb->status_changed != NULL)) {
		cb->status_changed(old_status, status);
	}
}

#if USBD_UAC_DEBUG
/**
  * @brief  UAC status dump thread
  * @param  param: Pointer to parameters
  * @retval void
  */
static void usbd_uac_status_dump_thread(void *param)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *buf_ctrl = &(cdev->uac_isoc_out);
	u32 loop_idx = 0;
	u32 max_loop = USBD_UAC_DEBUG_LOOP_TIME / USBD_UAC_DEBUG_STEP_TIME;

	UNUSED(param);

	cdev->isoc_dump_thread = 1;
	while (cdev->isoc_dump_thread != 0) {
		loop_idx = 0;
		RTK_LOGS(TAG, RTK_LOG_INFO, "USB Dump RX %d/TO %d/OW %d-%d/zp %d/pkt %d/copylen %d/%d\n",
				 buf_ctrl->xfer_cnt,
				 buf_ctrl->timeout_cnt,
				 buf_ctrl->overwrite_cnt,
				 buf_ctrl->append_overwrite_cnt,
				 buf_ctrl->append_zlp_cnt,
				 usb_ringbuf_get_count(&(buf_ctrl->buf_list)),
				 cdev->copy_data_len,
				 buf_ctrl->next_xfer
				);
		do {
			if (0 == cdev->isoc_dump_thread) {
				break;
			}
			rtos_time_delay_ms(USBD_UAC_DEBUG_STEP_TIME); //split to many times
		} while (loop_idx++ < max_loop); // 100 * 100
	}

	rtos_task_delete(NULL);
}
#endif

/**
  * @brief  Update or manage the count of audio data bytes
  * @param  audio_len: Data len had been wrote to the audiotrack
  * @retval void
  */
static inline void usbd_uac_get_audio_data_cnt(u32 audio_len)
{
#if USBD_UAC_DEBUG
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	cdev->copy_data_len += audio_len;
#else
	UNUSED(audio_len);
#endif
}

/**
  * @brief  Read data from a USB audio ring buffer
  * @param  pdata_ctrl: Pointer to the buffer control structure
  * @param  buffer: Pointer to Data buffer
  * @param  size: Data length
  * @param  copy_len: Copy length
  * @retval Status
  */
static int usbd_uac_read_ring_buf(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size,
								  u32 *copy_len, u16 *pkt_cnt, u32 *zero_pkt_flag)
{
	usb_ringbuf_manager_t *buf_list = &(pdata_ctrl->buf_list);
	u32 read_len;
	u8 valid = 0;

	do {
		/* Exit: 1) enough data obtained; 2) next packet cannot fit */
		if ((*copy_len >= size) || ((*copy_len + pdata_ctrl->isoc_mps) > size)) {
			return RTK_SUCCESS;
		}

		/* The OUT ISR drops the head of a full ring, possibly on another SMP core */
		usb_os_enter_critical();
		read_len = usb_ringbuf_remove_head(buf_list, buffer + *copy_len, size - *copy_len, &valid);
		usb_os_exit_critical();
		if (read_len > 0) {
			if (valid == 0U) {
				/* A filler node carries no payload, the ring hands back the stale bytes of the
				   slot: play it as silence */
				usb_os_memset((void *)(buffer + *copy_len), 0U, read_len);
				/* zero_pkt_flag is a 32-bit bitmap; cap at 32 pkts (shift >= 32 is UB). */
				if ((zero_pkt_flag != NULL) && (*pkt_cnt < 32U)) {
					*zero_pkt_flag |= (1U << (*pkt_cnt));
				}
			}
			*copy_len += read_len;
			(*pkt_cnt)++;
		}
	} while (usb_ringbuf_is_empty(buf_list) == 0);

	if (*copy_len >= size) {
		return RTK_SUCCESS;
	}

	return -RTK_ERR_IO;
}

/**
  * @brief  Initialize UAC device
  * @param  cb: Callback functions for audio events
  * @retval Status
  */
static int usbd_uac_private_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	int ret = RTK_SUCCESS;
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_ep_t *ep_isoc_in = &cdev->ep_isoc_in;
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	usb_ep_info_t *info;

	if (cb == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid user CB\n");
		return -RTK_ERR_BADARG;
	}

	/* No H2D request is waiting for its data stage yet. The device context is a static object,
	   so a re-init after deinit must not inherit a stale pending flag. */
	cdev->ctrl_req_pending = 0U;

	if (cfg == NULL) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid cfg\n");
		return -RTK_ERR_BADARG;
	}

	/* At least one direction has to be enabled, otherwise the class owns no stream at all. */
	if ((cfg->in.enable == 0) && (cfg->out.enable == 0)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Pls cfg UAC EP\n");
		return -RTK_ERR_BADARG;
	}

	/* A direction that is enabled needs its endpoint, with the right direction bit. */
	if (((cfg->in.enable != 0) && (((cfg->isoc_in_addr & USB_REQ_DIR_MASK) != USB_D2H) ||
								   ((cfg->isoc_in_addr & (~USB_REQ_DIR_MASK)) == 0U))) ||
		((cfg->out.enable != 0) && (((cfg->isoc_out_addr & USB_REQ_DIR_MASK) != USB_H2D) ||
									((cfg->isoc_out_addr & (~USB_REQ_DIR_MASK)) == 0U)))) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid EP addr %02x/%02x\n", cfg->isoc_in_addr, cfg->isoc_out_addr);
		return -RTK_ERR_BADARG;
	}

#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	/* The ring slot count is a u8 and high speed needs 8 slots per ms, refer to
	   usbd_uac_get_ring_buf_cnt(): reject a depth that can not be served at either speed */
	if ((((cfg->rx_buf_cnt != 0U) ? (u32)cfg->rx_buf_cnt : (u32)USBD_UAC_DEFAULT_RX_BUF_CNT) > USBD_UAC_BUFFER_COUNT_MAX) ||
		(((cfg->tx_buf_cnt != 0U) ? (u32)cfg->tx_buf_cnt : (u32)USBD_UAC_DEFAULT_TX_BUF_CNT) > USBD_UAC_BUFFER_COUNT_MAX)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Buf cnt %u/%u err\n", cfg->rx_buf_cnt, cfg->tx_buf_cnt);
		return -RTK_ERR_BADARG;
	}
#endif /* Full speed only: one slot per ms, a u8 count always fits USBD_UAC_BUFFER_COUNT_MAX */

	/* The mic descriptors are built on cfg->in, reject a format they can not describe */
	if (usbd_uac_check_mic_fmt(&(cfg->in)) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Invalid mic fmt %u/%u/%u\n", cfg->in.sampling_freq, cfg->in.ch_cnt, cfg->in.byte_width);
		return -RTK_ERR_BADARG;
	}

	/* Published before anything that reads it: the descriptor patch path, the endpoint setup
	   below and the ISOC buffer sizing all resolve their options through cdev->cfg. */
	cdev->cfg = cfg;

	/* The core owns the string table, so the same call works in standalone and composite
	   mode. 0 means no iInterface string. */
	cdev->itf_str_idx = usbd_add_string(cfg->itf_str);

	cdev->cur_volume = (cfg->volume_default != 0U) ? cfg->volume_default : USBD_UAC_DEFAULT_VOLUME;
	cdev->cur_mute = (cfg->mute_default != 0U) ? 1U : 0U;
	cdev->cur_clk_valid = 1;
	/* Non-zero default rate: the first SET_INTERFACE(alt=1) fires format_changed
	 * with all three fields set even if the host skips SET_CUR SAMPLING_FREQ
	 * (single discrete rate), which would otherwise leave playback silent. */
	cdev->cur_play_sampling_freq = USBD_UAC_SAMPLING_FREQ_48K;
	cdev->cur_record_sampling_freq = usbd_uac_get_mic_sampling_freq(cdev);

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	info = &ep_isoc_out->info;
	info->addr = cfg->isoc_out_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH)
					  ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
	ep_isoc_out->xfer_buf_len = (u32)sizeof(usbd_uac_rx_buf);
	info = &ep_isoc_in->info;
	info->addr = cfg->isoc_in_addr;
	info->type = USB_CH_EP_TYPE_ISOC;
	info->binterval = (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH)
					  ? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;

	cdev->cb = cb;

	if (cb->init != NULL) {
		ret = cb->init();
		if (ret != RTK_SUCCESS) {
			return ret;
		}
	}

#if UABD_UAC_DESC_DUMP
#ifndef CONFIG_SUPPORT_USB_FS_ONLY
	usbd_uac_desc_dump(usbd_uac_hs_config_desc, sizeof(usbd_uac_hs_config_desc) / sizeof(u8));
#else
	usbd_uac_desc_dump(usbd_uac_fs_config_desc, sizeof(usbd_uac_fs_config_desc) / sizeof(u8));
#endif
#endif

#if USBD_UAC_DEBUG
	rtos_task_t task_dump;
	if (rtos_task_create(&task_dump, ((const char *)"usbd_uac_status_dump_thread"), usbd_uac_status_dump_thread, NULL, 1024U, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "Create usb status dump task fail\n");
	}
#endif

	return ret;
}

/**
  * @brief  Receive UAC data
  * @param  void
  * @retval Status
  */
static int usbd_uac_receive_data(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl = &(cdev->uac_isoc_out);
	usbd_ep_t *ep_isoc_out = &cdev->ep_isoc_out;
	int ret = RTK_SUCCESS;

	if (dev == NULL) {
		return RTK_SUCCESS;
	}

	if (pbuf_ctrl->isoc_mps == 0) {
		return -RTK_ERR_IO;
	}

	if (usbd_uac_ep_enable(&(cdev->cfg->out)) != 0) {
		/* One critical section with the configured check: a bus reset or SET_CONFIGURATION(0)
		   handled by the ISR on another core runs clear_config(), which must not be undone by an EP
		   armed right after it. dev->is_ready can not tell: the core clears it only after
		   clear_config() returned, and not at all on a bus reset. None of the calls below blocks,
		   they nest the same lock. */
		usb_os_enter_critical();
		if (cdev->is_configured != 0U) {
			usb_ringbuf_reset(&(pbuf_ctrl->buf_list));
			pbuf_ctrl->sof_idx = 0;
			pbuf_ctrl->data_idx = 0;
			pbuf_ctrl->xfer_cnt = 0;
			pbuf_ctrl->next_xfer = 1;
			/* Arm the ISOC OUT EP here, its only ep_init path: the MPS is the ring node size,
			 * which fits every declared format. deinit+init is idempotent. */
			ep_isoc_out->info.mps = pbuf_ctrl->isoc_mps;
			usbd_ep_deinit(dev, ep_isoc_out);
			usbd_ep_init(dev, ep_isoc_out);
			ep_isoc_out->xfer_buf = usbd_uac_rx_buf;
			ep_isoc_out->xfer_len = pbuf_ctrl->isoc_mps;
			ret = usbd_ep_receive(dev, ep_isoc_out);
		} else {
			/* Nothing armed, the stream did not start: let usbd_uac_start_play() fail so that the
			   caller retries instead of reading a stream that never comes */
			ret = -RTK_ERR_IO;
		}
		usb_os_exit_critical();
	}

	return ret;
}

/**
  * @brief  Read the OUT ring buffer, refer to usbd_uac_read()
  * @note   Called by usbd_uac_read() between its gate updates, so the ring and the semaphore
  *         stay valid for the whole call.
  * @param  pdata_ctrl: OUT buffer control structure
  * @param  buffer: Destination buffer
  * @param  size: Destination capacity
  * @param  time_out_ms: Wait time, 0 for no wait
  * @param  zero_pkt_flag: Optional bitmap of filler packets
  * @retval Copied length in bytes
  */
static u32 usbd_uac_read_locked(usbd_uac_buf_ctrl_t *pdata_ctrl, u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	u32 copy_len = 0;
	u16 pkt_cnt = 0;

	if (pdata_ctrl->next_xfer == 0) {
		return copy_len;
	}

	if (time_out_ms == 0) {
		if (usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
			return copy_len;
		}

		usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag);
	} else {
		do {
			if (usb_ringbuf_is_empty(&(pdata_ctrl->buf_list))) {
				//wait sema
				pdata_ctrl->read_wait_sema = 1;
				/* is_closing: the buffer teardown waits for this call to leave */
				if ((pdata_ctrl->uac_sema_valid == 0) || (pdata_ctrl->is_closing != 0U)) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				if (rtos_sema_take(pdata_ctrl->uac_isoc_sema, time_out_ms) != RTK_SUCCESS) {
					pdata_ctrl->read_wait_sema = 0;
					break;
				}
				pdata_ctrl->read_wait_sema = 0;
				if (pdata_ctrl->is_closing != 0U) {
					break;
				}
			} else {
				/* if did not read any pkt, loop to check the wr/rd pos*/
				if ((usbd_uac_read_ring_buf(pdata_ctrl, buffer, size, &copy_len, &pkt_cnt, zero_pkt_flag) == RTK_SUCCESS)
					|| (copy_len > 0)) {
					break;
				}
			}
		} while (pdata_ctrl->next_xfer != 0);
	}

	return copy_len;
}

/* Exported functions --------------------------------------------------------*/

#ifdef CONFIG_USBD_COMPOSITE
int usbd_composite_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		cdev->from_composite = 1;
		ret = usbd_composite_register_driver(&usbd_uac_driver);
		if (ret != RTK_SUCCESS) {
			/* private_init completed, so deinit is its exact reverse. from_composite is
			   already 1, so the unregister inside is a no-op for an unregistered driver. */
			usbd_uac_deinit();
		}
	}
	return ret;
}
#endif

int usbd_uac_init(const usbd_uac_cb_t *cb, const usbd_uac_config_t *cfg)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	ret = usbd_uac_private_init(cb, cfg);
	if (ret == RTK_SUCCESS) {
		cdev->from_composite = 0;
		usbd_register_class(&usbd_uac_driver);
	}
	return ret;
}
/**
  * @brief  DeInitialize UAC device
  * @param  void
  * @retval None
  */
void usbd_uac_deinit(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	int ret;

	/* Leave the core before anything is torn down. Composite: refused until
	   usbd_composite_deinit() has blocked the ISR and cleared the configuration of every
	   function. Standalone: refused in interrupt context. Either way nothing is released when
	   refused, and once accepted no class callback runs any more and every endpoint of this
	   class is deinitialized: ref USB 2.0 9.4.7, the transfers still pending are aborted. */
#ifdef CONFIG_USBD_COMPOSITE
	if (cdev->from_composite != 0U) {
		ret = usbd_composite_unregister_driver(&usbd_uac_driver);
	} else
#endif
	{
		ret = usbd_unregister_class();
	}
	if (ret != RTK_SUCCESS) {
		return;
	}

#if USBD_UAC_DEBUG
	cdev->isoc_dump_thread = 0;
#endif

	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_in));
	usbd_uac_ep_buf_ctrl_deinit(&(cdev->uac_isoc_out));

	/* Unregistered above: no class callback can run afterwards, so dropping the pending
	   control request here cannot race an EP0 OUT completion in ISR context. */
	cdev->ctrl_req_pending = 0U;

	if (cdev->cb != NULL) {
		if (cdev->cb->deinit != NULL) {
			cdev->cb->deinit();
		}
		cdev->cb = NULL;
	}
}
/**
  * @brief  Transmit UAC data
  * @param  buf: Data buffer
  * @param  len: Data length
  * @retval Status
  */
int usbd_uac_transmit_data(const u8 *buf, u32 len)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usb_dev_t *dev = cdev->dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_in);
	usbd_uac_rec_pktz_t *pktz = &usbd_uac_rec_pktz;
	u32 off = 0U;
	u32 chunk;

	if ((buf == NULL) && (len != 0U)) {
		return -RTK_ERR_BADARG;
	}

	if (dev == NULL || dev->is_ready == 0) {
		return RTK_SUCCESS;
	}

	/* Counted under the same lock the buffer teardown closes the gate with, so the teardown
	   either sees this caller or this caller sees is_closing: never both missed */
	usb_os_enter_critical();
	if ((pdata_ctrl->is_closing != 0U) || (pdata_ctrl->uac_sema_valid == 0U) || (pdata_ctrl->next_xfer == 0U)) {
		usb_os_exit_critical();
		return RTK_SUCCESS;
	}
	pdata_ctrl->xfer_users++;
	usb_os_exit_critical();

	/* Defensive: the open gate implies a sized packetizer, refer to usbd_uac_ep_buf_ctrl_init(),
	   but a zero target would never advance the loop below */
	if ((pktz->buf == NULL) || (pktz->target == 0U)) {
		len = 0U;
	}

	/* Cut the stream into packets of the nominal size of each (micro)frame, any length is accepted
	   and nothing is truncated: a tail shorter than a packet waits in the packetizer for the next
	   call. Both ring operations are serialized with the ISR, which pops the head and retriggers
	   the IN EP. A ring full of unsent packets drops its oldest one, which is the expected overrun
	   handling of a live stream. */
	while (off < len) {
		chunk = MIN(len - off, (u32)pktz->target - pktz->fill);
		usb_os_memcpy((void *)(pktz->buf + pktz->fill), (const void *)(buf + off), chunk);
		pktz->fill = (u16)(pktz->fill + chunk);
		off += chunk;

		if (pktz->fill == pktz->target) {
			if (usbd_uac_ringbuf_push(pdata_ctrl, pktz->buf, pktz->fill, 1U) != 0U) {
				USB_DIAG(USB_LAYER_CLASS, USB_EVT_ERR_XFER, cdev->cfg->isoc_in_addr);
			}
			usbd_uac_rec_pktz_next();
		}
	}
	usbd_uac_isoc_in_process_xfer(dev);

	usb_os_enter_critical();
	pdata_ctrl->xfer_users--;
	usb_os_exit_critical();

	return RTK_SUCCESS;
}

/**
  * @brief  Initialize UAC device configuration
  * @param  uac_cfg: USB audio config
  * @param  is_record: 1 for recording, 0 for playback
  * @param  flag: Unused parameter
  * @retval Status
  */
int usbd_uac_config(const usbd_audio_cfg_t *uac_cfg, u8 is_record, u32 flag)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl;
	int ret = RTK_SUCCESS;

	UNUSED(flag);

	/* cdev->dev is assigned in set_config, so it is NULL until the host has enumerated
	   us. The MPS / ring buffer sizing below needs dev_speed, which is only known after
	   the speed negotiation, so reject the call instead of dereferencing NULL: the app
	   is expected to re-issue usbd_uac_config() once attached. */
	if (cdev->dev == NULL) {
		return -RTK_ERR_IO;
	}

	if ((uac_cfg != NULL) && usbd_uac_ep_enable(uac_cfg) != 0) {
		if (is_record != 0) { //
			/* The host learned the mic format from the descriptors built on cfg->in, the
			   record buffers shall use exactly that format */
			if ((usbd_uac_ep_enable(&(cdev->cfg->in)) == 0U) || (uac_cfg->sampling_freq != cdev->cfg->in.sampling_freq) ||
				(uac_cfg->ch_cnt != cdev->cfg->in.ch_cnt) || (uac_cfg->byte_width != cdev->cfg->in.byte_width)) {
				RTK_LOGS(TAG, RTK_LOG_ERROR, "Record fmt != cfg in\n");
				return -RTK_ERR_BADARG;
			}
			pbuf_ctrl = &(cdev->uac_isoc_in);
		} else { //
			if (usbd_uac_check_play_fmt(uac_cfg, cdev->dev->dev_speed) != RTK_SUCCESS) {
				return -RTK_ERR_BADARG;
			}
			pbuf_ctrl = &(cdev->uac_isoc_out);
		}

		/* Stop the stream, the application restarts it with usbd_uac_start_play() / start_record() */
		pbuf_ctrl->next_xfer = 0;

		/* Allocates the ring on the first call only, refer to usbd_uac_ep_buf_ctrl_init() */
		ret = usbd_uac_ep_buf_ctrl_init(pbuf_ctrl, uac_cfg, cdev->dev->dev_speed);
	}

	return ret;
}

/**
  * @brief  Initializes and starts the UAC data rx process
  * @param  void
  * @retval Status
  */
int usbd_uac_start_play(void)
{
	int ret;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play start\n");
#endif

	ret = usbd_uac_receive_data();

	return ret;
}

/**
  * @brief  Stop UAC device play
  * @param  void
  * @retval void
  */
void usbd_uac_stop_play(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC play stop\n");
#endif
	cdev->uac_isoc_out.next_xfer = 0;
}

/**
  * @brief  Initializes and starts the UAC data record (microphone) process
  * @param  void
  * @retval Status
  */
int usbd_uac_start_record(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pbuf_ctrl = &(cdev->uac_isoc_in);
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record start\n");
#endif

	/* Same preconditions as usbd_uac_receive_data(): attached and the ring configured */
	if ((cdev->cfg == NULL) || (usbd_uac_ep_enable(&(cdev->cfg->in)) == 0) || (cdev->dev == NULL) || (pbuf_ctrl->isoc_mps == 0U)) {
		return -RTK_ERR_IO;
	}

	/* usbd_uac_config() did not size the record path, nothing to stream from */
	if (pbuf_ctrl->uac_sema_valid == 0U) {
		return -RTK_ERR_IO;
	}

	/* Published as one unit: the record engine on the other core must not see next_xfer set
	   before the ring reset, nor pop the ring while it is reset */
	usb_os_enter_critical();
	usb_ringbuf_reset(&(pbuf_ctrl->buf_list));
	usbd_uac_rec_pktz_reset();
	pbuf_ctrl->sof_idx = 0;
	pbuf_ctrl->data_idx = 0;
	pbuf_ctrl->xfer_cnt = 0;
	pbuf_ctrl->next_xfer = 1;
	usb_os_exit_critical();

	return RTK_SUCCESS;
}

/**
  * @brief  Stop UAC device record (microphone)
  * @param  void
  * @retval void
  */
void usbd_uac_stop_record(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
#if USBD_UAC_DEBUG
	RTK_LOGS(TAG, RTK_LOG_INFO, "UAC record stop\n");
#endif
	cdev->uac_isoc_in.next_xfer = 0;
}

/**
  * @brief  Read UAC data
  * @param  buf: Pointer to data buffer
  * @param  size: Data length
  * @param  time_out_ms: Time out
  * @retval copy len
  */
u32 usbd_uac_read(u8 *buffer, u32 size, u32 time_out_ms, u32 *zero_pkt_flag)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	usb_dev_t *dev = cdev->dev;
	u32 copy_len;

	if (dev == NULL || dev->is_ready == 0) {
		return 0U;
	}

	/* Counted under the same lock the buffer teardown closes the gate with, so the teardown
	   either sees this caller or this caller sees is_closing: never both missed */
	usb_os_enter_critical();
	if ((pdata_ctrl->is_closing != 0U) || (pdata_ctrl->uac_sema_valid == 0U)) {
		usb_os_exit_critical();
		return 0U;
	}
	pdata_ctrl->xfer_users++;
	usb_os_exit_critical();

	copy_len = usbd_uac_read_locked(pdata_ctrl, buffer, size, time_out_ms, zero_pkt_flag);

	usb_os_enter_critical();
	pdata_ctrl->xfer_users--;
	usb_os_exit_critical();

	usbd_uac_get_audio_data_cnt(copy_len);
	return copy_len;
}

/**
  * @brief  Get UAC read frame cnt
  * @param  void
  * @retval read frame cnt
  */
u32 usbd_uac_get_read_frame_cnt(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);

	return usb_ringbuf_get_count(&(pdata_ctrl->buf_list));
}

/**
  * @brief  Return the current OUT ring buffer occupancy expressed in microseconds
  *         of audio at the negotiated bInterval. Callers use this to pace playback
  *         and detect underruns.
  */
u32 usbd_uac_get_read_frame_time_in_us(void)
{
	usbd_uac_dev_t *cdev = &usbd_uac_dev;
	usbd_uac_buf_ctrl_t *pdata_ctrl = &(cdev->uac_isoc_out);
	u8 binterval = pdata_ctrl->binterval;
	u32 base_us;

	if (binterval == 0U) {
		binterval = (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH)
					? USBD_UAC_HS_DEFAULT_BINTERVAL : USBD_UAC_FS_DEFAULT_BINTERVAL;
	}

	if (cdev->dev != NULL && cdev->dev->dev_speed == USB_SPEED_HIGH) {
		/* HS: 8 microframes per ms, each 125us */
		base_us = USBD_UAC_ONE_KHZ / USBD_UAC_HS_SOF_COUNT_PER_MS;
	} else {
		base_us = USBD_UAC_ONE_KHZ;
	}

	return (u32)usb_ringbuf_get_count(&(pdata_ctrl->buf_list)) * base_us * USBD_UAC_POW2(binterval - 1U);
}
