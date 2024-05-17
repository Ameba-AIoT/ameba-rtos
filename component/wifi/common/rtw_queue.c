/**
  ******************************************************************************
  * @file    rtw_queue.c
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#include <os_wrapper.h>
#include "basic_types.h"
#include "rtw_queue.h"

void rtw_init_listhead(_list *list)
{
	INIT_LIST_HEAD(list);
}

/*
For the following list_xxx operations,
caller must guarantee the atomic context.
Otherwise, there will be racing condition.
*/
u32 rtw_is_list_empty(_list *phead)
{
	if (list_empty(phead)) {
		return _TRUE;
	}

	return _FALSE;
}

void rtw_list_insert_head(_list *plist, _list *phead)
{
	list_add(plist, phead);
}

void rtw_list_insert_tail(_list *plist, _list *phead)
{
	list_add_tail(plist, phead);
}

/*

Caller must check if the list is empty before calling rtw_list_delete

*/
void rtw_list_delete(_list *plist)
{
	list_del_init(plist);
}

void rtw_init_queue(_queue	*pqueue)
{
	rtw_init_listhead(&(pqueue->queue));
	rtos_mutex_create_static(&(pqueue->lock));
}

u32 rtw_queue_empty(_queue	*pqueue)
{
	return (rtw_is_list_empty(&(pqueue->queue)));
}

u32 rtw_end_of_queue_search(_list *head, _list *plist)
{
	if (head == plist) {
		return _TRUE;
	} else {
		return _FALSE;
	}
}

