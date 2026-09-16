/*
*******************************************************************************
* Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#include <string.h>

#include <rtk_bt_common.h>
#include <rtk_bt_def.h>
#include <rtk_bt_mesh_def.h>
#include <rtk_bt_mesh_device_firmware_update_model.h>
#include <rtk_bt_device_firmware_update_interface.h>

#include <zephyr/bluetooth/mesh/dfu_cli.h>
#include <dfu_slot.h>

#if defined(BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE) && BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

static struct {
	struct bt_mesh_dfu_target targets[32];
	struct bt_mesh_blob_target_pull pull[32];
	size_t target_cnt;
	struct bt_mesh_blob_cli_inputs inputs;
} dfu_tx = {0};

// After send all packets, and receive the Firmware Update Status msg, call enter this callback function
static void stand_dfu_cli_ended(struct bt_mesh_dfu_cli *cli, enum bt_mesh_dfu_status reason)
{
	int ret;
	BT_LOGD("[%s] DFU ended: %u.\r\n", __func__, reason);
	if (BT_MESH_DFU_SUCCESS == reason) {
		BT_LOGD("[%s] Start send verify msg.\r\n", __func__);
		// Start send Firmware Update Apply msg to remote device
		ret = bt_mesh_dfu_cli_apply(cli);
		if (ret) {
			BT_LOGE("[%s] Call apply api return fail, ret:%d.\r\n", __func__, ret);
		} else {
			rtk_bt_evt_t *p_evt = NULL;
			rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *trans = NULL;
			uint32_t num_of_addr = dfu_tx.target_cnt;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_VERIFY,
										sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_other_t) + num_of_addr * sizeof(uint16_t));
			trans = (rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *)p_evt->data;
			trans->dist_phase = RTK_BT_MESH_DFU_DIST_PHASE_APPLY;
			trans->addr_num = num_of_addr;
			trans->paddr = (uint16_t *)((uint8_t *)p_evt->data + sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_other_t));
			for (uint32_t i = 0; i < num_of_addr; i++) {
				trans->paddr[i] = dfu_tx.targets[i].blob.addr;
			}
			rtk_bt_evt_indicate(p_evt, NULL);
		}
	} else {
		BT_LOGE("[%s] DFU client end fail, reason:%u.\r\n", __func__, reason);
	}
}

static void stand_dfu_cli_applied(struct bt_mesh_dfu_cli *cli)
{
	(void)cli;
	BT_LOGA("[%s] DFU applied.\r\n", __func__);
	// maybe report a event with type RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_COMPLETE
}

static void stand_dfu_cli_confirmed(struct bt_mesh_dfu_cli *cli)
{
	(void)cli;
	BT_LOGD("[%s] DFU confirmed.\r\n", __func__);
}

// If not receive ack msg after retry many times, call this callback function
static void stand_dfu_cli_lost_target(struct bt_mesh_dfu_cli *cli, struct bt_mesh_dfu_target *target)
{
	(void)cli;
	BT_LOGD("[%s] DFU target lost: 0x%04x.\r\n", __func__, target->blob.addr);

	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t *node_fail = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_NODE_FAIL,
								sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t));
	node_fail = (rtk_bt_mesh_dfu_evt_distributor_transfer_node_fail_t *)p_evt->data;
	node_fail->dist_phase = RTK_BT_MESH_DFU_DIST_PHASE_UNKNOWN;
	node_fail->addr = target->blob.addr;
	rtk_bt_evt_indicate(p_evt, NULL);
}

const struct bt_mesh_dfu_cli_cb stand_dfu_cli_cb = {
	.ended = stand_dfu_cli_ended,
	.applied = stand_dfu_cli_applied,
	.confirmed = stand_dfu_cli_confirmed,
	.lost_target = stand_dfu_cli_lost_target,
};

struct bt_mesh_dfu_cli dfu_client = BT_MESH_DFU_CLI_INIT(&stand_dfu_cli_cb);

static struct bt_mesh_dfu_slot *p_local_slot = NULL;
static uint16_t zephyr_set_param_to_mesh_dfu_slot(rtk_bt_mesh_dfu_standalone_updater_set_param_t *param)
{
	int err;

	p_local_slot = bt_mesh_dfu_slot_reserve();
	if (!p_local_slot) {
		BT_LOGE("[%s][%d] Slot point is NULL.\r\n", __func__, __LINE__);
		return 2;
	}

	err = bt_mesh_dfu_slot_info_set(p_local_slot, param->image_size, param->meta_data, param->meta_data_len);
	if (err) {
		BT_LOGE("[%s][%d] Set slot info err:%d.\r\n", __func__, __LINE__, err);
		return 3;
	}

	err = bt_mesh_dfu_slot_fwid_set(p_local_slot, param->fw_id, param->fw_id_len);
	if (err) {
		BT_LOGE("[%s][%d] Set firmware id err:%d.\r\n", __func__, __LINE__, err);
		return 4;
	}

	err = bt_mesh_dfu_slot_commit(p_local_slot);
	if (err) {
		BT_LOGE("[%s][%d] Commit for slot fail, err:%d.\r\n", __func__, __LINE__, err);
		return 5;
	}

	return 0;
}

static uint16_t zephyr_stack_dfu_set_param(rtk_bt_mesh_dfu_standalone_updater_set_param_t *param)
{
	BT_LOGD("[%s][%d] enter.\r\n", __func__, __LINE__);

	uint16_t ret = 0;
	int err = 0;
	static bool is_first_set = true;

	if (!param) {
		BT_LOGE("[%s] Wrong input param is NULL.\r\n", __func__);
		return 1;
	}

	if (is_first_set) {
		is_first_set = false;
		BT_LOGD("[%s][%d] Set mesh dfu slot first.\r\n", __func__, __LINE__);
		ret = zephyr_set_param_to_mesh_dfu_slot(param);
	} else {
		BT_LOGD("[%s][%d] Second set param, delete first one and restart set param.\r\n", __func__, __LINE__);
		err = bt_mesh_dfu_slot_del(p_local_slot);
		if (err) {
			BT_LOGE("[%s] Delete dfu slot fail, ret:%d.\r\n", __func__, err);
			return 6;
		}
		ret = zephyr_set_param_to_mesh_dfu_slot(param);
	}

	return ret;
}

static uint16_t zephyr_stack_dfu_add_receiver(rtk_bt_mesh_dfu_standalone_updater_add_receiver_param_t *rec)
{
	BT_LOGD("[%s] Enter.\r\n", __func__);

	if (!rec) {
		BT_LOGE("[%s] Wrong input param is NULL.\r\n", __func__);
		return 1;
	}

	uint16_t dst_addr = rec->addr;

	if (dfu_tx.target_cnt == ARRAY_SIZE(dfu_tx.targets)) {
		BT_LOGE("[%s] There is no memory for save new reicever.\r\n", __func__);
		return 2;
	}

	// Whether the wanted add receiver is already in list
	for (size_t i = 0; i < dfu_tx.target_cnt; i++) {
		if (dfu_tx.targets[i].blob.addr == dst_addr) {
			BT_LOGE("[%s] Target 0x%04x already exists fail.\r\n", __func__, dst_addr);
			return 0;
		}
	}

	dfu_tx.targets[dfu_tx.target_cnt].blob.addr = dst_addr;
	dfu_tx.targets[dfu_tx.target_cnt].img_idx = rec->update_image_index;
	sys_slist_append(&dfu_tx.inputs.targets, &dfu_tx.targets[dfu_tx.target_cnt].blob.n);
	dfu_tx.target_cnt++;

	return 0;
}

extern const struct bt_mesh_comp *bt_mesh_comp_get(void);
static bool bt_mesh_zephyr_model_first_get(uint16_t id, const struct bt_mesh_model **mod)
{
	const struct bt_mesh_comp *comp = bt_mesh_comp_get();

	for (size_t i = 0; i < comp->elem_count; i++) {
		*mod = bt_mesh_model_find(&comp->elem[i], id);
		if (*mod) {
			return true;
		}
	}

	return false;
}

// After receive Firmware Update Status msg, enter this callback function
static int stand_blob_io_open(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer, enum bt_mesh_blob_io_mode mode)
{
	(void)io;
	(void)mode;
	rtk_bt_evt_t *p_evt = NULL;
	uint32_t block_size = 0x1 << xfer->block_size_log;
	BT_LOGD("[%s] xfer->block_size_log:%d, block size:%d.\r\n", __func__, xfer->block_size_log, block_size);
	rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *blob;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_PARAM,
								sizeof(rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t));
	blob = (rtk_bt_mesh_dfu_evt_distributor_or_initiator_blob_param_t *)p_evt->data;
	blob->blob_size = xfer->size;
	blob->block_size = block_size;
	blob->total_blocks = xfer->size / block_size + (xfer->size % block_size ? 1 : 0);
	blob->chunk_size = xfer->chunk_size;
	rtk_bt_evt_indicate(p_evt, NULL);

	rtk_bt_dfu_standalone_updater_resource_init();

	return 0;
}

static void stand_blob_io_close(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer)
{
	(void)io;
	(void)xfer;
	BT_LOGD("[%s] Enter.\r\n", __func__);

	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *trans = NULL;
	uint32_t num_of_addr = dfu_tx.target_cnt;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_SUCCESS,
								sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_other_t) + num_of_addr * sizeof(uint16_t));
	trans = (rtk_bt_mesh_dfu_evt_distributor_transfer_other_t *)p_evt->data;
	trans->dist_phase = RTK_BT_MESH_DFU_DIST_PHASE_VERIFY;
	trans->addr_num = num_of_addr;
	trans->paddr = (uint16_t *)((uint8_t *)p_evt->data + sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_other_t));
	for (uint32_t i = 0; i < num_of_addr; i++) {
		trans->paddr[i] = dfu_tx.targets[i].blob.addr;
	}
	rtk_bt_evt_indicate(p_evt, NULL);
}

static void stand_blob_block_end(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer, const struct bt_mesh_blob_block *block)
{
	(void)io;
	BT_LOGD("[%s] Enter.\r\n", __func__);

	rtk_bt_evt_t *p_evt = NULL;
	rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t *trans_progress = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_STANDALONE_UPDATER_MODEL, RTK_BT_MESH_DFU_EVT_DISTRIBUTOR_BLOB_TRANSFER_PROGRESS,
								sizeof(rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t));
	trans_progress = (rtk_bt_mesh_dfu_evt_distributor_transfer_progress_t *)p_evt->data;
	trans_progress->dist_phase = RTK_BT_MESH_DFU_DIST_PHASE_BLOB_TRANSFER;
	trans_progress->progress = (block->offset + block->size) * 100 / xfer->size;
	rtk_bt_evt_indicate(p_evt, NULL);
}

static int stand_blob_chunk_wr(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer,
							   const struct bt_mesh_blob_block *block, const struct bt_mesh_blob_chunk *chunk)
{
	(void)io;
	(void)xfer;
	(void)block;
	(void)chunk;
	BT_LOGA("[%s] Enter.\r\n", __func__);
	return 0;
}

static int stand_blob_chunk_rd(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer,
							   const struct bt_mesh_blob_block *block, const struct bt_mesh_blob_chunk *chunk)
{
	(void)io;
	static uint8_t *p_block_data = NULL;
	static uint32_t max_block_len = 0;

	BT_LOGD("[%s][%d] CHUNK:offset(0x%x), size(%d); BLOCK:offset(%d), size(%d), num(%d), chunk count(%d), BLOB:size(%d).\r\n", __func__, __LINE__, \
			chunk->offset, chunk->size, block->offset, block->size, block->number, block->chunk_count, xfer->size);

	// Start read the first chunk packet of BLOB, malloc memory for save a block data
	if (0 == block->offset && 0 == chunk->offset) {
		BT_LOGD("[%s][%d] The first chunk packet of BLOB, malloc len:%d.\r\n", __func__, __LINE__, block->size);
		p_block_data = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, block->size);
		if (p_block_data) {
			max_block_len = block->size;
		} else {
			BT_LOGE("[%s] Malloc fail.\r\n", __func__);
			return -1;
		}
	} else if (!p_block_data) {
		max_block_len = 0;
		BT_LOGE("[%s] Memory pointer is NULL, fail.\r\n", __func__);
		return -2;
	}

	// Start read a new block data from flash.
	if (0 == chunk->offset) {
		if (block->size > max_block_len) {
			BT_LOGE("[%s] block len(%d) > max block len(%d), do not cover this situation fail.\r\n", __func__, block->size, max_block_len);
			return -3;
		}
		memset(p_block_data, 0xff, max_block_len);
		// Read a block packet form flash to memory buffer
		if (!rtk_bt_dfu_load_data_from_flash(block->offset, block->size, p_block_data)) {
			BT_LOGE("[%s] Load data from flash fail.\r\n", __func__);
			if (p_block_data) {
				osif_mem_free(p_block_data);
				p_block_data = NULL;
				max_block_len = 0;
			}
			return -4;
		}
	}

	// Copy a chunk data from memory to zephyr stack
	if (chunk->offset + chunk->size > max_block_len) {
		BT_LOGE("[%s] Try to write len(%d) > max block len(%d), do not cover this situation fail.\r\n", __func__, chunk->offset + chunk->size, max_block_len);
		return -5;
	} else {
		// Read a chunk packet from memory buffer
		memcpy(chunk->data, p_block_data + chunk->offset, chunk->size);
	}

	// Read the whole BLOB data complete, free the memory.
	if (xfer->size == block->offset + chunk->offset + chunk->size) {
		BT_LOGD("[%s][%d] The last chunk packet of BLOB, free the memory.\r\n", __func__, __LINE__);
		if (p_block_data) {
			osif_mem_free(p_block_data);
			p_block_data = NULL;
			max_block_len = 0;
		}
	}

	return 0;
}

static const struct bt_mesh_blob_io dfu_stand_dummy_blob_io = {
	.open = stand_blob_io_open,
	.close = stand_blob_io_close,
	.block_end = stand_blob_block_end,
	.wr = stand_blob_chunk_wr,
	.rd = stand_blob_chunk_rd,
};

#define BLOB_SIZE_LOG_FOR_BLOB_TRANSFER_MODEL  12
#define CHUNK_SIZE_FOR_BLOB_TRANSFER_MODEL  256
static uint16_t zephyr_stack_dfu_start(rtk_bt_mesh_dfu_standalone_update_start_t *start)
{
	struct bt_mesh_dfu_cli_xfer_blob_params blob_params = {0};
	struct bt_mesh_dfu_cli_xfer xfer = { 0 };
	const struct bt_mesh_model *mod_cli = NULL;
	uint16_t dst_addr = BT_MESH_ADDR_UNASSIGNED;
	int err = 0;

	BT_LOGD("[%s] Enter.\r\n", __func__);

	if (!start) {
		BT_LOGE("[%s] Input param is NULL fail.\r\n", __func__);
		return 1;
	}

	if (!bt_mesh_zephyr_model_first_get(BT_MESH_MODEL_ID_DFU_CLI, &mod_cli)) {
		BT_LOGE("[%s] Get relate model info fail.\r\n", __func__);
		return 2;
	}

	dst_addr = start->dst;
	xfer.mode = BT_MESH_BLOB_XFER_MODE_PUSH;

#if 1
	blob_params.block_size_log = BLOB_SIZE_LOG_FOR_BLOB_TRANSFER_MODEL;
	blob_params.chunk_size = CHUNK_SIZE_FOR_BLOB_TRANSFER_MODEL;
	xfer.blob_params = &blob_params;
#else
	xfer.blob_params = NULL;
#endif

	if (!dfu_tx.target_cnt) {
		BT_LOGE("[%s] There is no target nodes.\r\n", __func__);
		return 3;
	}

	if (p_local_slot) {
		xfer.slot = p_local_slot;
	} else {
		BT_LOGE("[%s] Get local slot point fail.\r\n", __func__);
		return 4;
	}

	BT_LOGD("[%s] Starting DFU with %u targets.\r\n", __func__, dfu_tx.target_cnt);

	dfu_tx.inputs.group = dst_addr;
	dfu_tx.inputs.app_idx = start->app_key_index;
	dfu_tx.inputs.ttl = BT_MESH_TTL_DEFAULT;
	dfu_tx.inputs.timeout_base = start->update_timeout_base;

	err = bt_mesh_dfu_cli_send((struct bt_mesh_dfu_cli *)mod_cli->rt->user_data, &dfu_tx.inputs, &dfu_stand_dummy_blob_io, &xfer);
	if (err) {
		BT_LOGE("[%s] Failed (err: %d).\r\n", __func__, err);
		return 5;
	}
	return 0;
}

extern bool bt_stack_profile_check(rtk_bt_profile_t profile);
uint16_t bt_mesh_device_firmware_update_standlone_model_act_handle(rtk_bt_cmd_t *p_cmd)
{
	uint16_t ret = RTK_BT_MESH_MSG_SEND_CAUSE_FAIL;
	if (true != bt_stack_profile_check(RTK_BT_PROFILE_MESH)) {
		BT_LOGE("[%s] Error: BLE MESH profile is not initiated\r\n", __func__);
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	} else {
		BT_LOGE("[%s] Error:This BLE MESH feature is not supported now\r\n", __func__);
		ret = RTK_BT_ERR_UNSUPPORTED;
		goto end;
	}
	switch (p_cmd->act) {
	case RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_SET_PARAM: {
		ret = zephyr_stack_dfu_set_param(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_ADD_RECEIVER: {
		ret = zephyr_stack_dfu_add_receiver(p_cmd->param);
		break;
	}
	case RTK_BT_MESH_DFU_ACT_STANDALONE_UPDATER_START: {
		ret = zephyr_stack_dfu_start(p_cmd->param);
		break;
	}
	default:
		BT_LOGE("[%s] Unknown p_cmd->act:%d\r\n", __func__, p_cmd->act);
		break;
	}
end:
	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);
	return ret;
}
#endif  // BT_MESH_ENABLE_DFU_STANDALONE_UPDATER_ROLE

#if defined(BT_MESH_ENABLE_DFU_TARGET_ROLE) && BT_MESH_ENABLE_DFU_TARGET_ROLE

static int target_metadata_check(struct bt_mesh_dfu_srv *srv, const struct bt_mesh_dfu_img *img,
								 struct net_buf_simple *metadata_raw, enum bt_mesh_dfu_effect *effect)
{
	(void)srv;
	(void)img;
	(void)metadata_raw;
	(void)effect;
	BT_LOGD("[%s] Enter.\r\n", __func__);
	return 0;
}

static int target_blob_chunk_rd(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer,
								const struct bt_mesh_blob_block *block, const struct bt_mesh_blob_chunk *chunk)
{
	(void)io;
	(void)xfer;
	(void)block;
	(void)chunk;
	BT_LOGD("[%s] Enter.\r\n", __func__);

	return 0;
}

#define INVALID_BLOCK_NUM  0xFFFF
struct block_buffer_t {
	uint8_t *p_buff;
	uint32_t buff_size;
	uint32_t target_data_len;
	uint32_t actual_data_len;
	uint16_t block_num;  // Block number the data belone to
};
static struct block_buffer_t buffer_block = {0};

static bool block_buffer_point_is_null(void)
{
	if (buffer_block.p_buff) {
		return false;
	} else {
		return true;
	}
}

static bool clear_buffer_for_save_block_data(void)
{
	if (block_buffer_point_is_null()) {
		BT_LOGE("[%s] The buffer point for save block data is null, fail.\r\n", __func__);
		return false;
	} else if (buffer_block.buff_size) {
		memset(buffer_block.p_buff, 0xff, buffer_block.buff_size);
		buffer_block.target_data_len = 0;
		buffer_block.actual_data_len = 0;
		buffer_block.block_num = INVALID_BLOCK_NUM;
		return true;
	} else {
		BT_LOGE("[%s] Block buffer len is 0, fail.\r\n", __func__);
		return false;
	}
}

static bool malloc_buffer_for_save_block_data(uint32_t len)
{
	if (block_buffer_point_is_null()) {
		buffer_block.p_buff = (uint8_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, len);
		if (buffer_block.p_buff) {
			buffer_block.buff_size = len;
			if (clear_buffer_for_save_block_data()) {
				return true;
			} else {
				return false;
			}
		} else {
			BT_LOGE("[%s] Malloc buffer for save block data fail.\r\n", __func__);
			return false;
		}
	} else {
		BT_LOGE("[%s] The buffer point is not NULL, malloc fail.\r\n", __func__);
		return false;
	}
}

// Save one chunk data to ram
static bool save_data_to_block_buffer(uint32_t chunk_offset, uint8_t *pdata, uint32_t len, uint16_t block_number, uint32_t block_size)
{
	if (chunk_offset + len > buffer_block.buff_size) {
		BT_LOGE("[%s] Save data len to buffer extend the max value, fail; chunk_offset:%d, len:%d, max len:%d.\r\n", __func__,
				chunk_offset, len, buffer_block.buff_size);
		return false;
	}

	// Receive the first chunk of block
	if (0 == chunk_offset) {
		// Make sure the block num is invalid value
		if (INVALID_BLOCK_NUM == buffer_block.block_num) {
			buffer_block.block_num = block_number;
			buffer_block.target_data_len = block_size;
		} else {
			BT_LOGE("[%s][%d] Error situation, fail.\r\n", __func__, __LINE__);
			return false;
		}
	}

	if (buffer_block.block_num != block_number) {
		BT_LOGE("[%s] Unexpect block num, input:%d, actual process:%d.\r\n", __func__, block_number, buffer_block.block_num);
		return false;
	}

	memcpy(buffer_block.p_buff + chunk_offset, pdata, len);

	buffer_block.actual_data_len += len;

	return true;
}

static bool data_in_buffer_of_block_is_full(void)
{
	return buffer_block.actual_data_len == buffer_block.target_data_len;
}

static bool save_whole_block_buffer_to_flash(uint32_t block_number)
{
	if (block_buffer_point_is_null()) {
		BT_LOGE("[%s] The buffer save block data is null, fail.\r\n", __func__);
		return false;
	}

	if (rtk_bt_dfu_updater_server_block_data(block_number, buffer_block.p_buff, buffer_block.actual_data_len)) {
		return true;
	} else {
		BT_LOGE("[%s] Save buffer block data to flash fail.\r\n", __func__);
		return false;
	}
}

static bool free_the_block_data_buffer(void)
{
	if (block_buffer_point_is_null()) {
		BT_LOGE("[%s] The block memory buffer point is NULL, fail.\r\n", __func__);
		return false;
	} else {
		osif_mem_free(buffer_block.p_buff);
		memset(&buffer_block, 0, sizeof(struct block_buffer_t));
		return true;
	}
}

static int target_blob_chunk_wr(const struct bt_mesh_blob_io *io, const struct bt_mesh_blob_xfer *xfer,
								const struct bt_mesh_blob_block *block, const struct bt_mesh_blob_chunk *chunk)
{
	(void)io;

	BT_LOGD("[%s][%d] BLOB size:%d; BLOCK num:%d, offset:0x%x; CHUNK offset:0x%x, size:%d.\r\n",
			__func__, __LINE__, xfer->size, block->number, block->offset, chunk->offset, chunk->size);

	if (0 == block->offset && 0 == chunk->offset) {
		// Receive the first chunk data of first block of BLOB, malloc memory of block size for save the block data
		if (malloc_buffer_for_save_block_data(block->size)) {
			BT_LOGD("[%s][%d] Receive the first chunk of BLOB, malloc memory of len(%d) for save block data.\r\n", __func__, __LINE__, block->size);
		} else {
			BT_LOGE("[%s] Malloc buffer for save block data fail.\r\n", __func__);
			return -1;
		}
	} else if (block_buffer_point_is_null()) {
		BT_LOGE("[%s] The memory point for save data is NULL, fail.\r\n", __func__);
		return -2;
	}

	// Save the chunk packet to memory temp
	save_data_to_block_buffer(chunk->offset, chunk->data, chunk->size, block->number, block->size);

	// Receive a complete block packet
	if (data_in_buffer_of_block_is_full()) {
		// Start write data to flash.
		if (save_whole_block_buffer_to_flash(block->number)) {
			// Have save a whole block data to flash succeed, clean the block buffer
			clear_buffer_for_save_block_data();

			// Report a event to app
			rtk_bt_evt_t *p_evt = NULL;
			p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_BLOCK_DATA, sizeof(uint16_t));
			*(uint16_t *)(p_evt->data) = block->number;
			rtk_bt_evt_indicate(p_evt, NULL);

			// Receive a whole BLOB data, free the memory
			if (xfer->size == block->offset + block->size) {
				BT_LOGD("[%s] Receive all data of BLOB, free the memory.\r\n", __func__);
				free_the_block_data_buffer();
			}
		} else {
			BT_LOGE("[%s] Save a block data to platform fail.\r\n", __func__);
			free_the_block_data_buffer();
			return -3;
		}
	}

	return 0;
}

static const struct bt_mesh_blob_io dfu_target_dummy_blob_io = {
	.rd = target_blob_chunk_rd,
	.wr = target_blob_chunk_wr,
};

// When receive Firmware Update Start msg, call this callback function
static int target_dfu_start(struct bt_mesh_dfu_srv *srv, const struct bt_mesh_dfu_img *img,
							struct net_buf_simple *metadata, const struct bt_mesh_blob_io **io)
{
	(void)srv;
	(void)img;
	(void)metadata;
	BT_LOGD("[%s][%d] Enter.\r\n", __func__, __LINE__);
	*io = &dfu_target_dummy_blob_io;
	rtk_bt_dfu_target_resource_init();

	return 0;
}

// When receive the whole BLOB data, call this callback function
static void target_dfu_transfer_end(struct bt_mesh_dfu_srv *srv, const struct bt_mesh_dfu_img *img, bool success)
{
	(void)img;
	(void)success;
	BT_LOGD("[%s] Start verify new image.\r\n", __func__);
	if (rtk_bt_dfu_updater_server_verify()) {
		bt_mesh_dfu_srv_verified(srv);
	} else {
		BT_LOGE("[%s] Verify new firmware fail.\r\n", __func__);
	}
}

// When receive Firmware Update Apply msg and send Firmware Update Status, call this callback function
static int target_dfu_apply(struct bt_mesh_dfu_srv *srv, const struct bt_mesh_dfu_img *img)
{
	(void)srv;
	(void)img;
	BT_LOGD("[%s] Start apply new image.\r\n", __func__);
	rtk_bt_evt_t *p_evt = NULL;
	p_evt = rtk_bt_event_create(RTK_BT_LE_GP_MESH_DFU_TARGET_MODEL, RTK_BT_MESH_DFU_EVT_TARGET_APPLY, 0);
	rtk_bt_evt_indicate(p_evt, NULL);

	if (!rtk_bt_dfu_updater_server_apply()) {
		BT_LOGE("[%s] Apply for new firmware fail.\r\n", __func__);
	}
	return 0;
}

static int target_dfu_recover(struct bt_mesh_dfu_srv *srv, const struct bt_mesh_dfu_img *img, const struct bt_mesh_blob_io **io)
{
	(void)srv;
	(void)img;
	(void)io;
	BT_LOGD("[%s] Enter.\r\n", __func__);
	return 0;
}

static const struct bt_mesh_dfu_srv_cb dfu_handlers = {
	.check = target_metadata_check,
	.start = target_dfu_start,
	.end = target_dfu_transfer_end,
	.apply = target_dfu_apply,
	.recover = target_dfu_recover,
};

static uint32_t target_fw_ver_curr = 0x0100;
static struct bt_mesh_dfu_img dfu_imgs[] = { {
		.fwid = &target_fw_ver_curr,
		.fwid_len = sizeof(target_fw_ver_curr),
	}
};

struct bt_mesh_dfu_srv dfu_server = BT_MESH_DFU_SRV_INIT(&dfu_handlers, dfu_imgs, ARRAY_SIZE(dfu_imgs));

#endif  // BT_MESH_ENABLE_DFU_TARGET_ROLE
