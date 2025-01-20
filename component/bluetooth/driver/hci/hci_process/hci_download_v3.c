#include "osif.h"
#include "hci_platform.h"
#include "hci_common.h"
#include "bt_debug.h"
#include "dlist.h"

#define USE_HCI_H4 1
#if defined(USE_HCI_H4) && USE_HCI_H4
#define RESERVE_LEN 1
#elif defined(USE_HCI_H5) && USE_HCI_H5
#define RESERVE_LEN 4
#else
#define RESERVE_LEN 0
#endif

#define HCI_PATCH_FRAG_SIZE         252
#define HCI_PATCH_IMAGE_INFO_LEN    1024

#define HCI_BT_REG_KEY_ID           0xb000ada4

#define REG_ADDR   0
#define REG_VALUE  1

#define TRIM_INDEX(num, index)    {    \
        num = (index >= 0x80) ? ((index - 0x80) % 0x7f + 1) : (index % 0x80); \
    }

typedef enum {
	INFO_REG_PATCH_START_ADDRESS = 0x00,
	INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE = 0x01,
	INFO_REG_NUM,
} PATCH_IMAGE_INFO_REG_NAME;

typedef enum {
	OPCODE_PATCH_IMAGE = 0x01,
} SECTION_OPCODE;

struct SECTION_NODE {
	struct list_head list;
	bool new_section_check;
	uint8_t priority;
	uint16_t cur_index;
	uint16_t end_index;
	uint8_t last_pkt;
	uint32_t reg_addr[INFO_REG_NUM];
	uint32_t reg_value[INFO_REG_NUM];
	uint32_t image_len;
	uint8_t *image_payload;
	uint32_t sent_image_len;
};

struct PATCH_INFO {
	uint32_t fw_len;
	uint32_t sent_fw_len;
	uint8_t section_cnt;
	uint8_t *config_buf;
	uint16_t config_len;
	uint16_t sent_config_len;
	uint8_t *patch_buf;
	uint32_t patch_len;
	struct SECTION_NODE head_node;
	struct SECTION_NODE *security_node;
};

static struct PATCH_INFO *patch_info = NULL;
static uint8_t patch_key_id = 0;

static uint8_t hci_vendor_read(uint32_t addr, uint32_t *value)
{
	/* OpCode: 0xFC61, Data Len: Cmd(8), Event(10) */
	uint16_t opcode = 0xFC61;
	uint8_t buf_raw[RESERVE_LEN + 10];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(5);
	buf[3] = (uint8_t)(0x21);
	for (uint8_t i = 0; i < 4; i++) {
		buf[4 + i] = (uint8_t)(addr >> (8 * i));
	}

	BT_DUMPD("hci_vendor_read HCI CMD: ", buf, buf[2] + 3);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 8, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	LE_TO_UINT32(*value, &buf[6]);

	return HCI_SUCCESS;
}

static uint8_t hci_vendor_write(uint32_t addr, uint32_t value)
{
	/* OpCode: 0xFC62, Data Len: Cmd(12), Event(5) */
	uint16_t opcode = 0xFC62;
	uint8_t buf_raw[RESERVE_LEN + 12];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[0] = (uint8_t)(opcode >> 0);
	buf[1] = (uint8_t)(opcode >> 8);
	buf[2] = (uint8_t)(9);
	buf[3] = (uint8_t)(0x21);
	for (uint8_t i = 0; i < 4; i++) {
		buf[4 + i] = (uint8_t)(addr >> (8 * i));
		buf[8 + i] = (uint8_t)(value >> (8 * i));
	}

	BT_DUMPD("hci_vendor_write HCI CMD: ", buf, buf[2] + 3);

	if (HCI_SUCCESS != hci_sa_send(HCI_CMD, buf, 12, true)) {
		return HCI_FAIL;
	}

	/* Check Resp: OpCode and Status */
	if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
		return HCI_FAIL;
	}

	return HCI_SUCCESS;
}

static void _insert_patch_queue(struct list_head *head, struct SECTION_NODE *p_node)
{
	struct list_head *pos, *next;
	struct SECTION_NODE *node;

	if (!head || !p_node) {
		return;
	}

	list_for_each_safe(pos, next, head) {
		node = list_entry(pos, struct SECTION_NODE, list);
		if (node->priority >= p_node->priority) {
			break;
		}
	}

	__list_add(&p_node->list, pos->prev, pos);
}

static void _select_security_section_node(struct SECTION_NODE **dest_node, struct SECTION_NODE *p_node)
{
	if (!p_node) {
		return;
	}

	if ((p_node->reg_value[INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE] != 0) &&
		(p_node->reg_addr[INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE] != 0) &&
		(p_node->reg_addr[INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE] != 0xffffffff) &&
		((*dest_node == NULL) || (p_node->priority > (*dest_node)->priority))) {
		*dest_node = p_node;
	}
}

static uint32_t _parse_patch_image(uint8_t *p_payload)
{
	struct PATCH_INFO *info = patch_info;
	struct SECTION_NODE *node;
	uint8_t *position;
	uint16_t chip_id;
	uint8_t ic_cut, key_id, ota_en;
	uint64_t temp_reg_addr, temp_reg_value, temp_image_len;
	uint32_t total_len;

	position = p_payload;
	LE_TO_UINT16(chip_id, position);
	ic_cut = *(position + sizeof(chip_id));
	key_id = *(position + sizeof(chip_id) + sizeof(ic_cut));
	ota_en = *(position + sizeof(chip_id) + sizeof(ic_cut) + sizeof(key_id));

	if (chip_id != HCI_PATCH_PROJECT_ID || key_id != patch_key_id || ota_en != 0) {
		BT_LOGE("Mismatch chip id / key id / ota en!\r\n");
		return 0;
	}

	if (ic_cut != hci_patch_get_chipid()) {
		/* Drop section whose ic cut mismatch */
		BT_LOGD("Mismatch ic cut!\r\n");
		return 0;
	}

	if (info->section_cnt >= 64) {
		BT_LOGE("Section number exceed 64!\r\n");
		return 0;
	}

	position += HCI_PATCH_IMAGE_INFO_LEN;
	LE_TO_UINT64(temp_image_len, position);
	if (temp_image_len == 0) {
		/* Drop section whose image_len is 0 */
		BT_LOGD("Section data length is 0.\r\n");
		return 0;
	}

	node = (struct SECTION_NODE *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct SECTION_NODE));
	if (node == NULL) {
		BT_LOGE("Section node allocate fail!\r\n");
		return 0;
	}
	memset(node, 0, sizeof(struct SECTION_NODE));
	info->section_cnt++;

	node->image_len = (uint32_t)(temp_image_len);
	node->image_payload = position + sizeof(temp_image_len);

	position -= sizeof(node->priority);
	node->priority = *(position);
	for (int8_t i = (INFO_REG_NUM - 1); i >= 0; i--) {
		position -= sizeof(temp_reg_value);
		LE_TO_UINT64(temp_reg_value, position);
		node->reg_value[i] = (uint32_t)(temp_reg_value);

		position -= sizeof(temp_reg_addr);
		LE_TO_UINT64(temp_reg_addr, position);
		node->reg_addr[i] = (uint32_t)(temp_reg_addr);
	}

	total_len = (info->section_cnt == 1) ? node->image_len + info->config_len : node->image_len;
	node->cur_index = 1;
	node->end_index = (total_len - 1) / HCI_PATCH_FRAG_SIZE + 1;
	node->last_pkt = total_len % HCI_PATCH_FRAG_SIZE;
	if (node->last_pkt == 0) {
		node->last_pkt = HCI_PATCH_FRAG_SIZE;
	}

	_insert_patch_queue(&info->head_node.list, node);

	_select_security_section_node(&info->security_node, node);

	return node->image_len;
}

static uint32_t _parse_sections(uint8_t *p_buf)
{
	uint8_t *p_section;
	uint32_t section_num, opcode, i;
	uint32_t fw_len = 0;
	uint64_t payload_len;

	LE_TO_UINT32(section_num, p_buf);

	if (section_num == 0) {
		BT_LOGE("Section num error!\r\n");
		return 0;
	}

	p_section = p_buf + sizeof(section_num);
	for (i = 0; i < section_num; i++) {
		LE_TO_UINT32(opcode, p_section);
		LE_TO_UINT64(payload_len, p_section + sizeof(opcode));

		switch (opcode) {
		case OPCODE_PATCH_IMAGE:
			fw_len += _parse_patch_image(p_section + sizeof(opcode) + sizeof(payload_len));
			break;
		default:
			BT_LOGE("Unknown opcode 0x%x\r\n", opcode);
			break;
		}
		p_section += sizeof(opcode) + sizeof(payload_len) + payload_len;
	}

	return fw_len;
}

static uint8_t _get_patch_info(uint8_t *p_patch, uint32_t patch_len)
{
	struct PATCH_INFO *info = patch_info;
	uint32_t version_date, version_time, reserved;
	uint32_t fw_len;
	uint8_t sig_len = 8;

	info->patch_buf = p_patch;
	info->patch_len = patch_len;

	LE_TO_UINT32(version_date, info->patch_buf + sig_len);
	LE_TO_UINT32(version_time, info->patch_buf + sig_len + sizeof(version_date));
	BT_LOGA("FW Version: %08d%06d\r\n", version_date, version_time);

	hci_platform_get_config(&info->config_buf, &info->config_len);

	fw_len = _parse_sections(info->patch_buf + sig_len + sizeof(version_date) + sizeof(version_time) + sizeof(reserved));
	if (fw_len == 0) {
		BT_LOGE("Available patch not found!\r\n");
		return HCI_IGNORE;
	}
	BT_LOGA("FW Length: %d\r\n", fw_len);

	info->fw_len = fw_len;

	return HCI_SUCCESS;
}

static uint8_t hci_patch_download_init_parse(uint8_t *p_patch, uint32_t patch_len)
{
	uint8_t ret;

	patch_info = osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct PATCH_INFO));
	if (!patch_info) {
		return HCI_FAIL;
	}

	memset(patch_info, 0, sizeof(struct PATCH_INFO));
	INIT_LIST_HEAD(&patch_info->head_node.list);

	/* Parse patch and get info */
	ret = _get_patch_info(p_patch, patch_len);
	if (HCI_SUCCESS != ret) {
		return ret;
	}

	return HCI_SUCCESS;
}

static void hci_patch_download_done(void)
{
	struct list_head *pos, *next;
	struct SECTION_NODE *node;

	list_for_each_safe(pos, next, &patch_info->head_node.list) {
		node = list_entry(pos, struct SECTION_NODE, list);
		list_del_init(pos);
		osif_mem_free(node);
	}

	if (patch_info) {
		osif_mem_free(patch_info);
	}
	patch_info = NULL;
}

static uint8_t hci_patch_get_cmd_buf(uint32_t (*reg_arr)[2], bool *new_section, uint8_t *cmd_buf)
{
	struct PATCH_INFO *info = patch_info;
	struct SECTION_NODE *node;
	struct list_head *pos, *next;
	uint8_t *data_buf = &cmd_buf[2];  /* cmd_buf[0]: len, cmd_buf[1]: index */
	uint8_t remain_len, sending_len;

	/* Send each section image frag */
	list_for_each_safe(pos, next, &info->head_node.list) {
		node = list_entry(pos, struct SECTION_NODE, list);
		if (node->new_section_check == false) {
			for (uint8_t i = 0; i < INFO_REG_NUM; i++) {
				reg_arr[i][REG_ADDR] = node->reg_addr[i];
				reg_arr[i][REG_VALUE] = node->reg_value[i];
			}
			node->new_section_check = true;
			*new_section = true;
		}

		if (node->cur_index > node->end_index) {
			/* Section sent completed when cur_index greater than end_index */
			continue;
		} else if (node->cur_index == node->end_index) {
			cmd_buf[0] = node->last_pkt + 1;
		} else {
			cmd_buf[0] = HCI_PATCH_FRAG_SIZE + 1;
		}
		TRIM_INDEX(cmd_buf[1], node->cur_index);

		remain_len = cmd_buf[0] - 1;
		if (node->sent_image_len + remain_len <= node->image_len) {
			sending_len = remain_len;
			memcpy(data_buf, node->image_payload + node->sent_image_len, sending_len);
			info->sent_fw_len += sending_len;
			node->sent_image_len += sending_len;
			remain_len -= sending_len;
		} else {
			/* For first section last pkt, need add config data */
			sending_len = node->image_len - node->sent_image_len;
			memcpy(data_buf, node->image_payload + node->sent_image_len, sending_len);
			info->sent_fw_len += sending_len;
			node->sent_image_len += sending_len;
			remain_len -= sending_len;

			memcpy(data_buf + sending_len, info->config_buf + info->sent_config_len, remain_len);
			info->sent_config_len += remain_len;

			if (node->image_len != node->sent_image_len) {
				BT_LOGE("Section node has not been sent completely! image_len = %d, sent_image_len = %d\r\n", node->image_len, node->sent_image_len);
				return HCI_FAIL;
			}
		}
		break;
	}

	if (node->cur_index > node->end_index) {
		/* Each section sent completed when exit the loop, last pkt need be sent */
		cmd_buf[0] = 1;
		cmd_buf[1] = 0x80;

		if (info->sent_fw_len != info->fw_len) {
			BT_LOGE("Firmware has not been sent completely! fw_len = %d, sent_fw_len = %d\r\n", info->fw_len, info->sent_fw_len);
			return HCI_FAIL;
		}

		if (info->sent_config_len != info->config_len) {
			BT_LOGE("Config data has not been sent completely! config_len = %d, sent_config_len = %d\r\n", info->config_len, info->sent_config_len);
			return HCI_FAIL;
		}
	} else {
		node->cur_index++;
	}

	return HCI_SUCCESS;
}

static bool hci_patch_found_security_section(uint32_t *reg_addr, uint32_t *reg_value)
{
	struct SECTION_NODE *node = patch_info->security_node;

	if (node == NULL) {
		return false;
	}

	*reg_addr = (uint32_t)(node->reg_addr[INFO_REG_PATCH_START_ADDRESS]);
	*reg_value = (uint32_t)(node->reg_value[INFO_REG_PATCH_START_ADDRESS] + node->image_len - node->reg_value[INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE]);

	return true;
}

uint8_t hci_patch_download_v3(uint16_t opcode, uint8_t *p_patch, uint32_t patch_len)
{
	/* OpCode: 0xFC20, Data Len: Cmd(256), Event(7) */
	uint8_t ret = HCI_SUCCESS;
	uint8_t buf_raw[RESERVE_LEN + 256];
	uint8_t *buf = buf_raw + RESERVE_LEN;
	bool new_section;
	uint32_t reg_arr[INFO_REG_NUM][2];
	uint32_t sec_reg_addr, sec_reg_value;
	uint32_t key_id_addr = HCI_BT_REG_KEY_ID, key_id_value;

	ret = hci_vendor_read(key_id_addr, &key_id_value);
	if ((HCI_SUCCESS != ret) || (key_id_value != 0)) {
		return HCI_FAIL;
	}
	patch_key_id = key_id_value;

	ret = hci_patch_download_init_parse(p_patch, patch_len);
	if (HCI_SUCCESS != ret) {
		goto dl_patch_done;
	}

	while (1) {
		buf[0] = (uint8_t)(opcode >> 0);
		buf[1] = (uint8_t)(opcode >> 8);
		new_section = false;
		memset(reg_arr, 0, sizeof(reg_arr));

		ret = hci_patch_get_cmd_buf(reg_arr, &new_section, &buf[2]);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}

		if (new_section) {
			for (uint8_t i = 0; i < INFO_REG_NUM; i++) {
				if (reg_arr[i][REG_ADDR] != 0 && reg_arr[i][REG_ADDR] != 0xffffffff) {
					ret = hci_vendor_write(reg_arr[i][REG_ADDR], reg_arr[i][REG_VALUE]);
					if (HCI_SUCCESS != ret) {
						goto dl_patch_done;
					}
				}
			}
		}

		if (buf[2] == 1 && buf[3] & 0x80) {
			if (hci_patch_found_security_section(&sec_reg_addr, &sec_reg_value)) {
				ret = hci_vendor_write(sec_reg_addr, sec_reg_value);
				if (HCI_SUCCESS != ret) {
					goto dl_patch_done;
				}
			}
		}

		ret = hci_sa_send(HCI_CMD, buf, buf[2] + 3, true);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}

		/* Check Resp: OpCode and Status */
		if (buf[3] != (uint8_t)(opcode >> 0) || buf[4] != (uint8_t)(opcode >> 8) || buf[5] != 0x00) {
			goto dl_patch_done;
		}

		/* Check the last download finished info */
		if (buf[6] == 0x80) {
			break;
		}
	}

dl_patch_done:
	hci_patch_download_done();

	return ret;
}