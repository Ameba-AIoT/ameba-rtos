#include "osif.h"
#include "hci_platform.h"
#include "hci_common.h"
#include "bt_debug.h"
#include "dlist.h"

#define HCI_PATCH_FRAG_SIZE        252
#define HCI_BAUDRATE_SIZE          4

static uint8_t patch_chip_id = 0;
static uint8_t patch_key_id = 0;
static bool hci_is_mp = false;
static uint8_t default_baud[HCI_BAUDRATE_SIZE] = {0x1d, 0x70, 0x00, 0x00}; //115200
static uint8_t work_baud[HCI_BAUDRATE_SIZE] = {0};

extern const unsigned char rtlbt_fw[];
extern unsigned int rtlbt_fw_len;
extern const unsigned char rtlbt_mp_fw[];
extern unsigned int rtlbt_mp_fw_len;

typedef enum {
	OPCODE_PATCH_SNIPPETS = 0x01,
	OPCODE_DUMMY_HEADER = 0x02,
	OPCODE_SECURITY_HEADER = 0x03,
	OPCODE_OTA_FLAG = 0x04,
	OPCODE_CONTROLLER_RSVD = 0x08
} SECTION_OPCODE;

typedef struct {
	struct list_head list;
	uint8_t eco;
	uint8_t priority;
	uint8_t key_id;
	uint8_t reserve;
	uint32_t payload_len;
	uint8_t *payload;
	uint32_t sent_payload_len;
} PATCH_NODE;

typedef struct {
	uint32_t fw_len;
	uint32_t sent_fw_len;
	uint8_t *config_buf;
	uint16_t config_len;
	uint16_t sent_config_len;
	uint16_t cur_index;
	uint16_t end_index;
	uint8_t last_pkt;
	uint8_t *patch_buf;
	uint32_t patch_len;
	PATCH_NODE head_node;
} PATCH_INFO;
static PATCH_INFO *patch_info = NULL;

void hci_patch_set_chipid(uint8_t chipid)
{
	patch_chip_id = hci_platform_record_chipid(chipid);
}

uint8_t hci_downlod_patch_init(void)
{
	patch_info = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(PATCH_INFO));
	if (!patch_info) {
		return HCI_FAIL;
	}

	memset(patch_info, 0, sizeof(PATCH_INFO));
	INIT_LIST_HEAD(&patch_info->head_node.list);

	return HCI_SUCCESS;
}

void hci_downlod_patch_done(void)
{
	struct list_head *pos, *next;
	PATCH_NODE *node;

	list_for_each_safe(pos, next, &patch_info->head_node.list) {
		node = list_entry(pos, PATCH_NODE, list);
		list_del_init(pos);
		osif_mem_free(node);
	}

	if (patch_info) {
		osif_mem_free(patch_info);
	}
	patch_info = NULL;
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
			break;
		} else {
			p_buf -= length;
			opcode = *(--p_buf);
		}
	}

	BT_LOGE("Project ID not found!\r\n");
	return 0xFF;
}

static void _insert_patch_queue(struct list_head *head, PATCH_NODE *p_patch_node)
{
	struct list_head *pos, *next;
	PATCH_NODE *node;

	if (!head || !p_patch_node) {
		return;
	}

	list_for_each_safe(pos, next, head) {
		node = list_entry(pos, PATCH_NODE, list);
		if (node->priority >= p_patch_node->priority) {
			break;
		}
	}

	__list_add(&p_patch_node->list, pos->prev, pos);
}

static void _parse_patch_section(uint8_t *p_buf, uint32_t *p_fw_len, SECTION_OPCODE opcode, bool *p_found_security_header,
								 PATCH_NODE *p_patch_node_head)
{
	PATCH_NODE *patch_node;
	uint16_t number, reserve;
	uint8_t *position;
	uint8_t eco;
	uint32_t payload_len;

	LE_TO_UINT16(number, p_buf);

	position = p_buf + sizeof(number) + sizeof(reserve);
	for (uint16_t i = 0; i < number; i++) {
		eco = *(position);
		LE_TO_UINT32(payload_len, position + sizeof(patch_node->eco) + sizeof(patch_node->priority) + \
					 sizeof(patch_node->key_id) + sizeof(patch_node->reserve));

		if (eco == patch_chip_id) {
			patch_node = (PATCH_NODE *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(PATCH_NODE));
			if (patch_node == NULL) {
				BT_LOGE("patch_node allocate fail!\r\n");
				return;
			}
			memset(patch_node, 0, sizeof(PATCH_NODE));

			patch_node->eco = eco;
			patch_node->priority = *(position + sizeof(patch_node->eco));
			if (opcode == OPCODE_SECURITY_HEADER) {
				patch_node->key_id = *(position + sizeof(patch_node->eco) + sizeof(patch_node->priority));
			}
			patch_node->payload_len = payload_len;
			patch_node->payload = position + sizeof(patch_node->eco) + sizeof(patch_node->priority) + \
								  sizeof(patch_node->key_id) + sizeof(patch_node->reserve) + sizeof(patch_node->payload_len);

			if (opcode == OPCODE_PATCH_SNIPPETS || opcode == OPCODE_DUMMY_HEADER) {
				_insert_patch_queue(&p_patch_node_head->list, patch_node);
				*p_fw_len += payload_len;
			} else if (opcode == OPCODE_SECURITY_HEADER) {
				if (patch_node->key_id == patch_key_id) {
					_insert_patch_queue(&p_patch_node_head->list, patch_node);
					*p_fw_len += payload_len;
					*p_found_security_header = true;
				} else {
					BT_LOGE("patch_node->key_id = 0x%x mismatch patch_key_id = 0x%x\r\n", patch_node->key_id, patch_key_id);
					osif_mem_free(patch_node);
				}
			}
		}

		position += sizeof(patch_node->eco) + sizeof(patch_node->priority) + \
					sizeof(patch_node->key_id) + sizeof(patch_node->reserve) + sizeof(patch_node->payload_len) + payload_len;
	}
}

static uint32_t _parse_patch(uint8_t *p_buf, PATCH_NODE *p_patch_node_head)
{
	uint32_t i;
	uint32_t section_num;
	uint8_t *p_section;
	uint32_t opcode, length;
	uint32_t fw_len = 0;
	bool found_security_header = false;

	LE_TO_UINT32(section_num, p_buf);

	if (section_num == 0) {
		BT_LOGE("Section num error!\r\n");
		return 0;
	} else {
		p_section = p_buf + sizeof(section_num);
		for (i = 0; i < section_num; i++) {
			LE_TO_UINT32(opcode, p_section);
			LE_TO_UINT32(length, p_section + sizeof(opcode));

			switch (opcode) {
			case OPCODE_PATCH_SNIPPETS:
				_parse_patch_section(p_section + sizeof(opcode) + sizeof(length), &fw_len, OPCODE_PATCH_SNIPPETS, NULL, p_patch_node_head);
				break;
			case OPCODE_DUMMY_HEADER:
				if (patch_key_id != 0) {
					BT_LOGE("patch_key_id = 0x%x, ignore\r\n", patch_key_id);
					break;
				}
				_parse_patch_section(p_section + sizeof(opcode) + sizeof(length), &fw_len, OPCODE_DUMMY_HEADER, NULL, p_patch_node_head);
				break;
			case OPCODE_SECURITY_HEADER:
				if (patch_key_id == 0) {
					BT_LOGE("patch_key_id = 0x%x, ignore\r\n", patch_key_id);
					break;
				}
				_parse_patch_section(p_section + sizeof(opcode) + sizeof(length), &fw_len, OPCODE_SECURITY_HEADER, &found_security_header, p_patch_node_head);
				break;
			case OPCODE_OTA_FLAG:
				BT_LOGE("OTA flag not support\r\n");
				break;
			case OPCODE_CONTROLLER_RSVD:
				break;
			default:
				BT_LOGE("Unknown opcode 0x%x\r\n", opcode);
				break;
			}
			p_section += sizeof(opcode) + sizeof(length) + length;
		}

		//if has key id but not found security header, parse dummy header again
		if (patch_key_id != 0 && found_security_header == false) {
			p_section = p_buf + sizeof(section_num);
			for (i = 0; i < section_num; i++) {
				LE_TO_UINT32(opcode, p_section);
				LE_TO_UINT32(length, p_section + sizeof(opcode));

				if (opcode == OPCODE_DUMMY_HEADER) {
					_parse_patch_section(p_section + sizeof(opcode) + sizeof(length), &fw_len, OPCODE_DUMMY_HEADER, NULL, p_patch_node_head);
				}
				p_section += sizeof(opcode) + sizeof(length) + length;
			}
		}
	}

	return fw_len;
}

static uint8_t _get_patch_info(void)
{
	const uint8_t patch_sig_v1[]    = {0x52, 0x65, 0x61, 0x6C, 0x74, 0x65, 0x63, 0x68}; //V1 signature: Realtech
	const uint8_t patch_sig_v2[]    = {0x52, 0x54, 0x42, 0x54, 0x43, 0x6F, 0x72, 0x65}; //V2 signature: RTBTCore
	const uint8_t ext_section_sig[] = {0x51, 0x04, 0xFD, 0x77};                         //Extension section signature
	PATCH_INFO *info = patch_info;
	bool ext_section_check;
	uint8_t project_id;
	uint32_t version_date, version_time;
	uint32_t fw_len;

	if (CHECK_CFG_SW(CFG_SW_USE_FLASH_PATCH)) {
		if (hci_is_mp_mode()) {
			info->patch_buf = (uint8_t *)(void *)rtlbt_mp_fw;
			info->patch_len = rtlbt_mp_fw_len;
		} else {
			info->patch_buf = (uint8_t *)(void *)rtlbt_fw;
			info->patch_len = rtlbt_fw_len;
		}
		ext_section_check = true;
	} else {
		info->patch_buf = (uint8_t *)HCI_PATCH_FLASH_ADDRESS;
		ext_section_check = false;
	}

	if (ext_section_check) {
		if ((!memcmp(info->patch_buf, patch_sig_v1, sizeof(patch_sig_v1))) && \
			(!memcmp(info->patch_buf + info->patch_len - sizeof(ext_section_sig), ext_section_sig, sizeof(ext_section_sig)))) {
			BT_LOGE("Signature check success: Merge patch v1 not support\r\n");
			return HCI_IGNORE;
		} else if ((!memcmp(info->patch_buf, patch_sig_v2, sizeof(patch_sig_v2))) && \
				   (!memcmp(info->patch_buf + info->patch_len - sizeof(ext_section_sig), ext_section_sig, sizeof(ext_section_sig)))) {
			BT_LOGA("Signature check success: Merge patch v2\r\n");

			project_id = _get_patch_project_id(info->patch_buf + info->patch_len - sizeof(ext_section_sig));
			if (project_id != HCI_PATCH_PROJECT_ID) {
				BT_LOGE("Project ID 0x%02x check fail, No available patch!\r\n", project_id);
				return HCI_IGNORE;
			}
		} else {
			BT_LOGE("Signature check fail, No available patch!\r\n");
			return HCI_IGNORE;
		}
	} else {
		if (!memcmp(info->patch_buf, patch_sig_v1, sizeof(patch_sig_v1))) {
			BT_LOGE("Signature check success: Merge patch v1 not support\r\n");
			return HCI_IGNORE;
		} else if (!memcmp(info->patch_buf, patch_sig_v2, sizeof(patch_sig_v2))) {
			BT_LOGA("Signature check success: Merge patch v2\r\n");
		} else {
			BT_LOGE("Signature check fail, No available patch!\r\n");
			return HCI_IGNORE;
		}
	}

	LE_TO_UINT32(version_date, patch_info->patch_buf + sizeof(patch_sig_v2));
	LE_TO_UINT32(version_time, patch_info->patch_buf + sizeof(patch_sig_v2) + sizeof(version_date));
	BT_LOGA("FW Version: %d%d\r\n", version_date, version_time);

	fw_len = _parse_patch(info->patch_buf + sizeof(patch_sig_v2) + sizeof(version_date) + sizeof(version_time), &info->head_node);
	if (fw_len == 0) {
		BT_LOGE("Available patch not found!\r\n");
		return HCI_IGNORE;
	}
	BT_LOGA("FW Length: %d\r\n", fw_len);

	info->fw_len = fw_len;

	hci_platform_get_config(&info->config_buf, &info->config_len);

	/* Calculate patch info */
	info->end_index = (info->fw_len + info->config_len - 1) / HCI_PATCH_FRAG_SIZE;
	info->last_pkt = (info->fw_len + info->config_len) % HCI_PATCH_FRAG_SIZE;
	if (info->last_pkt == 0) {
		info->last_pkt = HCI_PATCH_FRAG_SIZE;
	}

	return HCI_SUCCESS;
}

uint8_t hci_get_patch_cmd_len(uint8_t *cmd_len)
{
	uint8_t ret;
	PATCH_INFO *info = patch_info;

	/* Download FW partial patch first time, get patch and info */
	if (0 == info->cur_index) {
		ret = _get_patch_info();
		if (HCI_SUCCESS != ret) {
			return ret;
		}
	}

	if (info->cur_index == info->end_index) {
		*cmd_len = info->last_pkt + 1;
		return HCI_SUCCESS;
	}

	*cmd_len = HCI_PATCH_FRAG_SIZE + 1;

	return HCI_SUCCESS;
}

uint8_t hci_get_patch_cmd_buf(uint8_t *cmd_buf, uint8_t cmd_len)
{
	PATCH_INFO *info = patch_info;
	uint8_t        *data_buf   = &cmd_buf[1];
	uint8_t         data_len   = cmd_len - 1;
	uint8_t         remain_len = data_len;
	uint8_t         sending_len = 0;
	uint32_t        total_node_len = 0;
	struct list_head *pos, *next;
	PATCH_NODE *node;

	/* first byte is index */
	if (info->cur_index >= 0x80) {
		cmd_buf[0] = (info->cur_index - 0x80) % 0x7f + 1;
	} else {
		cmd_buf[0] = info->cur_index % 0x80;
	}
	if (info->cur_index == info->end_index) {
		cmd_buf[0] |= 0x80;
	}

	list_for_each_safe(pos, next, &info->head_node.list) {
		node = list_entry(pos, PATCH_NODE, list);
		total_node_len += node->payload_len;

		// Find the patch node need to be send
		if (info->sent_fw_len < total_node_len) {
			if (info->sent_fw_len + remain_len < total_node_len) {
				sending_len = remain_len;
				memcpy(data_buf + data_len - remain_len, node->payload + node->sent_payload_len, sending_len);
				info->sent_fw_len += sending_len;
				node->sent_payload_len += sending_len;
				remain_len -= sending_len;
				// data_buf is already full, break the loop
				break;
			} else {
				sending_len = node->payload_len - node->sent_payload_len;
				memcpy(data_buf + data_len - remain_len, node->payload + node->sent_payload_len, sending_len);
				info->sent_fw_len += sending_len;
				node->sent_payload_len += sending_len;
				remain_len -= sending_len;
				if (node->payload_len != node->sent_payload_len) {
					BT_LOGE("Patch node has not been sent completely! payload_len = %d, sent_payload_len = %d\r\n", node->payload_len, node->sent_payload_len);
					return HCI_FAIL;
				}
				// data_buf is not full, jump to the next patch node
			}
		}
	}

	if (remain_len > 0) {
		if (info->fw_len != info->sent_fw_len) {
			BT_LOGE("Firmware has not been sent completely! fw_len = %d, sent_fw_len = %d\r\n", info->fw_len, info->sent_fw_len);
			return HCI_FAIL;
		}

		// Add config data after firmware
		memcpy(data_buf + data_len - remain_len, info->config_buf + info->sent_config_len, remain_len);
		info->sent_config_len += remain_len;
	}

	if (info->cur_index == info->end_index) {
		if (info->config_len != info->sent_config_len) {
			BT_LOGE("Config data has not been sent completely! config_len = %d, sent_config_len = %d\r\n", info->config_len, info->sent_config_len);
			return HCI_FAIL;
		}
	} else {
		info->cur_index++;
	}

	return HCI_SUCCESS;
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
		//Init baudrate
		memcpy(baudrate, default_baud, HCI_BAUDRATE_SIZE);
	} else {
		//Work baudrate
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