#include "osif.h"
#include "hci_platform.h"
#include "hci_common.h"
#include "hci_transport.h"
#include "bt_debug.h"
#include "dlist.h"

#define RESERVE_LEN 1

#define HCI_PATCH_FRAG_SIZE         252
#define HCI_PATCH_IMAGE_INFO_LEN    1024

/* section fomart: ... [regaddr(8bytes) regvalue(8bytes)] index(1byte) image_len(8bytes) payload */
#define REG_VALUE_POSITION(__payload, __idx)   ((__payload) - 9 - ((__idx) * 2 + 1) * 8)
#define REG_ADDR_POSITION(__payload, __idx)    ((__payload) - 9 - ((__idx) * 2 + 2) * 8)

typedef enum {
	/* enum uses reverse order */
	INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE = 0x00,
	INFO_REG_PATCH_START_ADDRESS = 0x01,
	INFO_REG_NUM,
} PATCH_IMAGE_INFO_REG_NAME;

typedef enum {
	OPCODE_PATCH_IMAGE = 0x01,
} SECTION_OPCODE;

struct SECTION_NODE {
	struct list_head list;
	uint8_t index;
	uint32_t length;
	uint8_t *payload;
};

struct SECTION_NODE section_head;

static uint8_t hci_vendor_write(uint32_t addr, uint32_t value)
{
	/* OpCode: 0xFC62, Data Len: Cmd(12), Event(5) */
	uint8_t buf_raw[RESERVE_LEN + 12];
	uint8_t *buf = buf_raw + RESERVE_LEN;

	buf[2] = (uint8_t)(9);
	buf[3] = (uint8_t)(0x21);
	for (uint8_t i = 0; i < 4; i++) {
		buf[4 + i] = (uint8_t)(addr >> (8 * i));
		buf[8 + i] = (uint8_t)(value >> (8 * i));
	}

	BT_DUMPD("hci_vendor_write HCI CMD: ", buf, buf[2] + 3);

	return hci_sa_send_cmd_sync(0xFC62, buf, 12);
}

extern const unsigned char rtlbt_fw[];
static uint32_t _parse_patch_image(uint8_t *p_payload)
{
	struct SECTION_NODE *node, *listnode;
	uint8_t *position;
	uint8_t ic_cut, key_id, ota_en;
	uint64_t length;
	struct list_head *pos, *next;

	position = p_payload;
	ic_cut = *(position + 2);
	key_id = *(position + 2 + sizeof(ic_cut));
	ota_en = *(position + 2 + sizeof(ic_cut) + sizeof(key_id));

	if (key_id != 0 || ota_en != 0) {
		BT_LOGE("Mismatch key id / ota en!\r\n");
		return 0;
	}

	if (ic_cut != hci_patch_get_chipid()) {
		/* Drop section whose ic cut mismatch */
		BT_LOGD("Mismatch ic cut!\r\n");
		return 0;
	}

	position += HCI_PATCH_IMAGE_INFO_LEN;
	LE_TO_UINT64(length, position);
	if (length == 0) {
		/* Drop section whose length is 0 */
		BT_LOGD("Section data length is 0.\r\n");
		return 0;
	}

	node = (struct SECTION_NODE *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(struct SECTION_NODE));
	if (node == NULL) {
		BT_LOGE("Section node allocate fail!\r\n");
		return 0;
	}
	memset(node, 0, sizeof(struct SECTION_NODE));

	node->length = (uint32_t)(length);
	node->payload = position + sizeof(length);
	node->index = *(position - 1);

	list_for_each_safe(pos, next, &section_head.list) {
		listnode = list_entry(pos, struct SECTION_NODE, list);
		if (listnode->index >= node->index) {
			break;
		}
	}

	/* Add node before pos */
	list_add_tail(&node->list, pos);

	return node->length;
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

static uint8_t hci_patch_parse(uint8_t *p_patch)
{
	uint32_t version_date, version_time, reserved;
	uint32_t fw_len;
	uint8_t sig_len = 8;

	INIT_LIST_HEAD(&section_head.list);

	/* Parse patch and get info */

	LE_TO_UINT32(version_date, p_patch + sig_len);
	LE_TO_UINT32(version_time, p_patch + sig_len + sizeof(version_date));
	BT_LOGA("FW Version: %08d%06d\r\n", version_date, version_time);

	fw_len = _parse_sections(p_patch + sig_len + sizeof(version_date) + sizeof(version_time) + sizeof(reserved));
	if (fw_len == 0) {
		BT_LOGE("Available patch not found!\r\n");
		return HCI_IGNORE;
	}
	BT_LOGA("FW Length: %d\r\n", fw_len);

	return HCI_SUCCESS;
}

static void hci_patch_download_done(void)
{
	struct list_head *pos, *next;
	struct SECTION_NODE *node;

	list_for_each_safe(pos, next, &section_head.list) {
		node = list_entry(pos, struct SECTION_NODE, list);
		list_del_init(pos);
		osif_mem_free(node);
	}
}

static uint8_t _send_payload_to_controller(uint8_t *data, uint32_t len, bool *add_config)
{
	uint8_t buf_raw[RESERVE_LEN + 256];
	uint8_t *cmd = buf_raw + RESERVE_LEN;
	uint8_t send, idx = 0;
	uint32_t remain;

download:
	remain = len;
	while (remain) {
		idx++;
		idx = (idx & 0x80) ? 1 : idx; /* valid range 1~0x7F */
		send = (remain < HCI_PATCH_FRAG_SIZE) ? remain : HCI_PATCH_FRAG_SIZE;
		memcpy(&cmd[4], data + len - remain, send);
		remain -= send;
		cmd[2] = send + 1;
		cmd[3] = idx;
		if (HCI_SUCCESS != hci_sa_send_cmd_sync(0xFC20, cmd, send + 4)) {
			return HCI_FAIL;
		}
	}

	if (*add_config) {
		*add_config = false;
		len = 0;
		hci_platform_get_config(&data, (uint16_t *)&len);
		goto download;
	}

	return HCI_SUCCESS;
}

uint8_t hci_patch_download_v3(uint8_t *p_patch)
{
	uint8_t ret = HCI_SUCCESS;
	uint32_t reg_addr, reg_value;
	struct SECTION_NODE *node, *security_node = NULL;
	struct list_head *pos, *next;
	int i;
	uint8_t cmd[16];
	bool add_config = true;

	ret = hci_patch_parse(p_patch);
	if (HCI_SUCCESS != ret) {
		goto dl_patch_done;
	}

	/* Send each section image frag */
	list_for_each_safe(pos, next, &section_head.list) {
		node = list_entry(pos, struct SECTION_NODE, list);
		for (i = INFO_REG_NUM - 1; i >= 0; i--) {
			/* write registers if address is valid */
			LE_TO_UINT32(reg_addr, REG_ADDR_POSITION(node->payload, i));
			if (reg_addr != 0 && reg_addr != 0xffffffff) {
				LE_TO_UINT32(reg_value, REG_VALUE_POSITION(node->payload, i));
				ret = hci_vendor_write(reg_addr, reg_value);
				if (HCI_SUCCESS != ret) {
					goto dl_patch_done;
				}

				/* select security node, which is needed after all sections are downloaded */
				if (i == INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE && reg_value != 0) {
					if ((security_node == NULL) || (node->index > security_node->index)) {
						security_node = node;
					}
				}
			}
		}

		/* send section content to controller, config data should be sent following first section */
		ret = _send_payload_to_controller(node->payload, node->length, &add_config);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}
	}

	/* this register value is used when controller does image content verifiction */
	if (security_node) {
		uint32_t value;
		LE_TO_UINT32(value, REG_VALUE_POSITION(security_node->payload, INFO_REG_PATCH_START_ADDRESS));
		LE_TO_UINT32(reg_addr, REG_ADDR_POSITION(security_node->payload, INFO_REG_PATCH_START_ADDRESS));
		reg_value = value + security_node->length;
		LE_TO_UINT32(value, REG_VALUE_POSITION(security_node->payload, INFO_REG_SECURITY_HEADER_BUF_REMAIN_SIZE));
		reg_value -= value;

		ret = hci_vendor_write(reg_addr, reg_value);
		if (HCI_SUCCESS != ret) {
			goto dl_patch_done;
		}
	}

	/* send last 0xfc20 to trigger controller to verify image content */
	cmd[3] = 1;
	cmd[4] = 0x80;
	ret = hci_sa_send_cmd_sync(0xFC20, cmd + 1, 4);

dl_patch_done:
	hci_patch_download_done();

	return ret;
}