/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/
#include <bt_api_config.h>
#if defined(RTK_BREDR_SUPPORT) && RTK_BREDR_SUPPORT
#include <rtk_bt_def.h>
#include <rtk_bt_common.h>
#include <stdio.h>
#include <string.h>
#include <dlist.h>
#include <osif.h>
#include <rtk_bt_br_gap.h>
#include <rtk_bt_sdp.h>
#include <gap_br.h>
#include <trace_app.h>
#include <bt_sdp.h>
#include <remote.h>
#include <btm.h>

/* sdp record list */
static struct list_head rtk_sdp_record_list;
static uint8_t rtk_sdp_record_init_flag = 0;
extern T_APP_BR_LINK *app_find_br_link(uint8_t *bd_addr);

static uint16_t rtk_sdp_record_list_enqueue(struct list_head *plist)
{
	/* initialize first */
	if (!rtk_sdp_record_init_flag) {
		INIT_LIST_HEAD(&rtk_sdp_record_list);
		rtk_sdp_record_init_flag = 1;
	}
	/* insert list */
	list_add_tail(plist, &rtk_sdp_record_list);

	return RTK_BT_OK;
}

static struct list_head *rtk_sdp_record_list_dequeue(void)
{
	struct list_head *plist = NULL;

	if (list_empty(&rtk_sdp_record_list)) {
		BT_LOGE("%s(): rtk_sdp_record_list is empty !\r\n", __func__);
		return NULL;
	} else {
		plist = rtk_sdp_record_list.next;
		list_del_init(plist);
		return plist;
	}
}

static void app_sdp_bt_cback(T_BT_EVENT event_type, void *event_buf, uint16_t buf_len)
{
	(void)buf_len;
	T_BT_EVENT_PARAM *param = event_buf;
	T_APP_BR_LINK *p_link;
	bool handle = true;
	rtk_bt_evt_t *p_evt = NULL;

	switch (event_type) {

	/* source role */
	case BT_EVENT_DID_ATTR_INFO: {
		rtk_sdp_did_attr_info *p_info = NULL;

		p_link = app_find_br_link(param->did_attr_info.bd_addr);
		if (p_link != NULL) {
			APP_PRINT_INFO0("BT_EVENT_DID_ATTR_INFO");
			BT_LOGE("app_sdp_bt_cback: BT_EVENT_DID_ATTR_INFO \r\n");
			{
				p_evt = rtk_bt_event_create(RTK_BT_BR_GP_SDP, RTK_BT_SDP_EVT_DID_ATTR_INFO, sizeof(rtk_sdp_did_attr_info));
				if (!p_evt) {
					BT_LOGE("app_sdp_bt_cback: evt_t allocate fail \r\n");
					handle = false;
					break;
				}
				p_info = (rtk_sdp_did_attr_info *)p_evt->data;
				memcpy((void *)p_info->bd_addr, (void *)p_link->bd_addr, 6);
				p_info->specific_id = param->did_attr_info.specific_id;
				p_info->vendor_id = param->did_attr_info.vendor_id;
				p_info->vendor_src = param->did_attr_info.vendor_src;
				p_info->product_id = param->did_attr_info.product_id;
				p_info->version = param->did_attr_info.version;
				/* Send event */
				if (RTK_BT_OK != rtk_bt_evt_indicate(p_evt, NULL)) {
					handle = false;
					break;
				}
			}
		} else {
			APP_PRINT_INFO0("SDP p_link is NULL");
			BT_LOGE("app_sdp_bt_cback: SDP p_link is NULL \r\n");
		}
	}
	break;

	default: {
		APP_PRINT_INFO1("app_sdp_bt_cback: default event_type 0x%04x", event_type);
		// BT_LOGE("app_sdp_bt_cback: default event_type 0x%04x \r\n", event_type);
		handle = false;
	}
	break;
	}

	if (handle == true) {
		// APP_PRINT_INFO1("app_sdp_bt_cback: event_type 0x%04x", event_type);
		// BT_LOGA("app_sdp_bt_cback: event_type 0x%04x \r\n", event_type);
	}
}

uint16_t bt_stack_sdp_record_add(void *record, uint32_t length)
{
	rtk_sdp_record_t *p_temp_record_t = NULL;

	/* allocate record mem */
	p_temp_record_t = (rtk_sdp_record_t *)osif_mem_alloc(RAM_TYPE_DATA_ON, sizeof(rtk_sdp_record_t));
	if (!p_temp_record_t) {
		BT_LOGE("%s(): p_temp_record_t allocate fail !\r\n", __func__);
		return RTK_BT_FAIL;
	}
	/* memcpy record data */
	memcpy((void *)p_temp_record_t->record, record, length);
	/* construct sdp record struct */
	INIT_LIST_HEAD(&p_temp_record_t->list);
	p_temp_record_t->length = length;
	/* add to record list, waiting for rtk_sdp_init() invoking */
	rtk_sdp_record_list_enqueue(&p_temp_record_t->list);

	return RTK_BT_OK;
}

static uint16_t bt_stack_sdp_discov_start(void *param)
{
	uint8_t *bd_addr = (uint8_t *)param;

	if (bt_did_discov_start(bd_addr)) {
		return RTK_BT_OK;
	}

	return RTK_BT_FAIL;
}

uint16_t bt_stack_sdp_act_handle(rtk_bt_cmd_t *p_cmd)
{
	int16_t ret = 0;
	BT_LOGD("bt_stack_sdp_act_handle: act = %d \r\n", p_cmd->act);
	switch (p_cmd->act) {

	case RTK_BT_SDP_ACT_DISCOV_START:
		ret = bt_stack_sdp_discov_start(p_cmd->param);
		break;

	default:
		BT_LOGE("bt_stack_sdp_act_handle: unknown act: %d \r\n", p_cmd->act);
		ret = 0;
		break;
	}

	p_cmd->ret = ret;
	osif_sem_give(p_cmd->psem);

	return ret;
}

uint16_t bt_stack_sdp_init(void)
{
	BT_LOGA("[SDP]app_sdp_init\n");

	struct list_head *plist = NULL;
	struct list_head *plist_next = NULL;
	rtk_sdp_record_t *precord = NULL;

	if (list_empty(&rtk_sdp_record_list)) {
		BT_LOGE("%s(): rtk_sdp_record_list is empty !\r\n", __func__);
		return RTK_BT_FAIL;
	}
	plist = rtk_sdp_record_list.next;
	while (1) {
		if (&rtk_sdp_record_list == plist) {
			break;
		} else {
			plist_next = plist->next;
			precord = (rtk_sdp_record_t *)plist;
			bt_sdp_record_add((void *)precord->record);
			plist = plist_next;
		}
	}
	bt_mgr_cback_register(app_sdp_bt_cback);

	return RTK_BT_OK;
}

extern void sdp_deinit(void);

void bt_stack_sdp_deinit(void)
{
	struct list_head *plist = NULL;
	rtk_sdp_record_t *precord = NULL;

	BT_LOGA("[SDP]app_sdp_deinit\n");
	rtk_sdp_record_init_flag = 0;
	while (1) {
		if (list_empty(&rtk_sdp_record_list)) {
			BT_LOGE("%s(): rtk_sdp_record_list is empty !\r\n", __func__);
			break;
		}
		plist = rtk_sdp_record_list_dequeue();
		precord = (rtk_sdp_record_t *)plist;
		bt_sdp_record_delete((void *)precord->record);
		osif_mem_free((void *)precord);
	}
	sdp_deinit();
}

#endif