#include "osif.h"
#include "hci_platform.h"
#include "hci_common.h"
#include "bt_debug.h"
#include "dlist.h"

#define HCI_PATCH_FRAG_SIZE        252
#define HCI_BAUDRATE_SIZE          4

static uint8_t patch_chip_id = 0;
static bool hci_is_mp = false;
static uint8_t default_baud[HCI_BAUDRATE_SIZE] = {0x1d, 0x70, 0x00, 0x00}; /* 115200 */
static uint8_t work_baud[HCI_BAUDRATE_SIZE] = {0};

extern const unsigned char rtlbt_fw[];
extern unsigned int rtlbt_fw_len;
extern const unsigned char rtlbt_mp_fw[];
extern unsigned int rtlbt_mp_fw_len;

void hci_patch_set_chipid(uint8_t chipid)
{
	patch_chip_id = hci_platform_record_chipid(chipid);
}

uint8_t hci_patch_get_chipid(void)
{
	return patch_chip_id;
}

static uint8_t _get_patch_project_id(uint8_t *p_buf)
{
	uint8_t opcode;
	uint8_t length;
	uint8_t data;

	opcode = *(--p_buf);

	while (opcode != 0xFF) {
		length = *(--p_buf);
		if (opcode == 0x00) {
			if (length != 1) {
				BT_LOGE("Project ID length error!\r\n");
				return 0xFF;
			} else {
				data = *(--p_buf);
				return data;
			}
		} else {
			p_buf -= length;
			opcode = *(--p_buf);
		}
	}

	BT_LOGE("Project ID not found!\r\n");
	return 0xFF;
}

uint8_t hci_patch_get_patch_version(uint8_t **pp_patch_buf, uint32_t *p_patch_len)
{
	const uint8_t patch_sig_v1[] = {0x52, 0x65, 0x61, 0x6C, 0x74, 0x65, 0x63, 0x68}; /* V1 signature: Realtech */
	const uint8_t patch_sig_v2[] = {0x52, 0x54, 0x42, 0x54, 0x43, 0x6F, 0x72, 0x65}; /* V2 signature: RTBTCore */
	const uint8_t patch_sig_v3[] = {0x42, 0x54, 0x4E, 0x49, 0x43, 0x30, 0x30, 0x33}; /* V3 signature: BTNIC003 */
	const uint8_t ext_section_sig[] = {0x51, 0x04, 0xFD, 0x77};                      /* Extension section signature */
	bool ext_section_check;
	uint8_t project_id;
	uint8_t *p_patch = NULL;
	uint32_t patch_len;
	uint8_t patch_version = PATCH_VERSION_INVALID;

	if (CHECK_CFG_SW(CFG_SW_USE_FLASH_PATCH)) {
		if (hci_is_mp_mode()) {
			p_patch = (uint8_t *)(void *)rtlbt_mp_fw;
			patch_len = rtlbt_mp_fw_len;
		} else {
			p_patch = (uint8_t *)(void *)rtlbt_fw;
			patch_len = rtlbt_fw_len;
		}
		ext_section_check = true;
	} else {
		p_patch = (uint8_t *)HCI_PATCH_FLASH_ADDRESS;
		ext_section_check = false;
	}

	if (!memcmp(p_patch, patch_sig_v3, sizeof(patch_sig_v3))) {
		patch_version = PATCH_VERSION_V3;
	} else {
		if (ext_section_check) {
			if ((!memcmp(p_patch, patch_sig_v1, sizeof(patch_sig_v1))) &&
				(!memcmp(p_patch + patch_len - sizeof(ext_section_sig), ext_section_sig, sizeof(ext_section_sig)))) {
				patch_version = PATCH_VERSION_V1;
			} else if ((!memcmp(p_patch, patch_sig_v2, sizeof(patch_sig_v2))) &&
					   (!memcmp(p_patch + patch_len - sizeof(ext_section_sig), ext_section_sig, sizeof(ext_section_sig)))) {
				patch_version = PATCH_VERSION_V2;
				project_id = _get_patch_project_id(p_patch + patch_len - sizeof(ext_section_sig));
				if (project_id != HCI_PATCH_PROJECT_ID) {
					BT_LOGE("Project ID 0x%02x check fail, No available patch!\r\n", project_id);
					return PATCH_VERSION_INVALID;
				}
			} else {
				return PATCH_VERSION_INVALID;
			}
		} else {
			if (!memcmp(p_patch, patch_sig_v1, sizeof(patch_sig_v1))) {
				patch_version = PATCH_VERSION_V1;
			} else if (!memcmp(p_patch, patch_sig_v2, sizeof(patch_sig_v2))) {
				patch_version = PATCH_VERSION_V2;
			} else {
				return PATCH_VERSION_INVALID;
			}
		}
	}

	if (pp_patch_buf) {
		*pp_patch_buf = p_patch;
	}

	if (p_patch_len) {
		*p_patch_len = patch_len;
	}

	return patch_version;
}

void hci_set_mp(bool is_mp)
{
	hci_is_mp = is_mp;
}

bool hci_check_mp(void)
{
	return hci_is_mp;
}

uint32_t _convert_baudrate(uint8_t *baudrate)
{
	int i, len;

	struct {
		uint8_t bt_baudrate[HCI_BAUDRATE_SIZE];
		uint32_t uart_baudrate;
	} br_map[] = {
		{{0x1d, 0x70, 0x00, 0x00}, 115200},
		{{0x0A, 0xC0, 0x52, 0x02}, 230400},
		{{0x04, 0x50, 0xF7, 0x03}, 921600},
		{{0x04, 0x50, 0xF7, 0x05}, 921600},
		{{0x04, 0x50, 0x00, 0x00}, 1000000},
		{{0x02, 0x80, 0x92, 0x04}, 1500000},
		{{0x02, 0x50, 0x00, 0x00}, 2000000},
		{{0x01, 0xB0, 0x00, 0x00}, 2500000},
		{{0x01, 0x80, 0x92, 0x04}, 3000000},
		{{0x01, 0x60, 0x2A, 0x05}, 3500000},
		{{0x01, 0x50, 0x00, 0x00}, 4000000},
	};

	len = sizeof(br_map) / sizeof(br_map[0]);

	for (i = 0; i < len; i++) {
		if (memcmp(baudrate, br_map[i].bt_baudrate, HCI_BAUDRATE_SIZE) == 0) {
			return br_map[i].uart_baudrate;
		}
	}

	return 0;
}

void hci_set_work_baudrate(uint8_t *baudrate)
{
	memcpy(work_baud, baudrate, HCI_BAUDRATE_SIZE);
}

void hci_get_baudrate(uint8_t *baudrate, bool use_default_rate)
{
	if (use_default_rate) {
		/* Init baudrate */
		memcpy(baudrate, default_baud, HCI_BAUDRATE_SIZE);
	} else {
		/* Work baudrate */
		memcpy(baudrate, work_baud, HCI_BAUDRATE_SIZE);
	}
}

uint8_t hci_update_uart_baudrate(bool use_default_rate)
{
	uint8_t *baud = work_baud;

	if (use_default_rate) {
		baud = default_baud;
	}

	hci_uart_set_bdrate(_convert_baudrate(baud));
	osif_delay(10);

	return HCI_SUCCESS;
}

void set_reg_value(uint32_t reg_address, uint32_t Mask, uint32_t val)
{
	uint32_t shift = 0;
	uint32_t data = 0;

	for (shift = 0; shift < 31; shift++) {
		if (((Mask >> shift) & 0x1) == 1) {
			break;
		}
	}

	data = HAL_READ32(reg_address, 0);
	data = ((data & (~Mask)) | (val << shift));
	HAL_WRITE32(reg_address, 0, data);
	data = HAL_READ32(reg_address, 0);
}

uint8_t hci_get_hdr_len(uint8_t type)
{
	if (type == HCI_CMD) {
		return sizeof(struct hci_cmd_hdr);
	} else if (type == HCI_EVT) {
		return sizeof(struct hci_evt_hdr);
	} else if (type == HCI_ACL) {
		return sizeof(struct hci_acl_hdr);
	} else if (type == HCI_ISO) {
		return sizeof(struct hci_iso_hdr);
	} else if (type == HCI_SCO) {
		return sizeof(struct hci_sco_hdr);
	}

	return 0;
}

uint16_t hci_get_body_len(const void *hdr, uint8_t type)
{
	uint16_t len = 0;
	if (type == HCI_CMD) {
		len = ((const struct hci_cmd_hdr *)hdr)->param_len;
	} else if (type == HCI_EVT) {
		len = ((const struct hci_evt_hdr *)hdr)->len;
	} else if (type == HCI_ISO) {
		LE_TO_UINT16(len, &(((const struct hci_iso_hdr *)hdr)->len));
		len &= 0x3FFF;
	} else if (type == HCI_ACL) {
		LE_TO_UINT16(len, &(((const struct hci_acl_hdr *)hdr)->len));
	} else if (type == HCI_SCO) {
		len = ((const struct hci_sco_hdr *)hdr)->len;
	}

	return len;
}

static bool _controller_is_enabled = false;
uint8_t hci_process(void);
bool hci_controller_enable(void)
{
	if (_controller_is_enabled) {
		BT_LOGE("Controller Already enabled!\r\n");
		return false;
	}

	/* Set WiFi leave powersave mode */
	if (rtk_bt_pre_enable() == false) {
		BT_LOGE("rtk_bt_pre_enable fail!\r\n");
		return HCI_FAIL;
	}

	/* BT Board Init */
	if (HCI_FAIL == hci_platform_init()) {
		BT_LOGE("hci_platform_init fail!\r\n");
		goto fail;
	}

	/* HCI Transport */
	if (HCI_FAIL == hci_transport_open()) {
		BT_LOGE("hci_transport_open fail!\r\n");
		goto fail;
	}

	/* HCI Transport Bridge to StandAlone */
	hci_transport_register(&hci_sa_cb);

	if (HCI_FAIL == hci_process()) {
		BT_LOGE("hci_process fail!\r\n");
		goto fail;
	}

	/* Recover WiFi powersave mode */
	rtk_bt_post_enable();
	_controller_is_enabled = true;
	return true;

fail:
	hci_controller_disable();
	return false;
}

void hci_controller_disable(void)
{
	hci_platform_deinit();  /* Platform Deinit First */
	hci_transport_close();  /* HCI Transport Close */

	_controller_is_enabled = false;
}

void hci_controller_free(void)
{
	hci_uart_free();        /* UART Free */
	hci_transport_free();   /* HCI Transport Free */
}

bool hci_controller_is_enabled(void)
{
	return _controller_is_enabled;
}