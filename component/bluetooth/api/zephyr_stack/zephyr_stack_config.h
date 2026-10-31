/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_STACK_CONFIG_H
#define ZEPHYR_STACK_CONFIG_H

#define GAP_MAX_LE_COC_SERVER_REG_NUM         10
#define GAP_MAX_LE_COC_CHANN_NUM              10
#define GAP_MAX_LE_COC_CREDITS                2

#define API_TASK_STACK_SIZE         4096
#define API_TASK_PRIORITY           4
#define API_TASK_MSG_QUEUE_SIZE     0x20

#endif /* ZEPHYR_STACK_CONFIG_H */
