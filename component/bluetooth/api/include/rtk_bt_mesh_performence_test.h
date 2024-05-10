/**
 * @file      rtk_bt_mesh_performence_test.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH datatrans model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_API_PERFORMENCE_TEST_H__
#define __RTK_BT_MESH_API_PERFORMENCE_TEST_H__

#include <stdint.h>

/******************Opcode define***********************/
/**
 * @typedef   rtk_bt_mesh_performence_test_act_t
 * @brief     BLE MESH performence test act definition.
 */
typedef enum {
	RTK_BT_MESH_PERFORMENCE_TEST_PARAM_SET = 1,
	RTK_BT_MESH_PERFORMENCE_TEST_START,
	RTK_BT_MESH_PERFORMENCE_TEST_SENDING,
	RTK_BT_MESH_PERFORMENCE_TEST_END,
	RTK_BT_MESH_PERFORMENCE_TEST_REQUEST,
	RTK_BT_MESH_PERFORMENCE_TEST_RESPONSE,
	RTK_BT_MESH_PERFORMENCE_TEST_PRINT,
	RTK_BT_MESH_PERFORMENCE_TEST_OPCODE_MAX,
} rtk_bt_mesh_performence_test_opcode_t;

/**
 * @typedef   rtk_bt_mesh_performence_test_act_t
 * @brief     BLE MESH performence test act definition.
 */
typedef enum {
	RTK_BT_MESH_PERFORMENCE_TEST_ACT_PARAM_SET = 1,
	RTK_BT_MESH_PERFORMENCE_TEST_ACT_MSG_SEND,
	RTK_BT_MESH_PERFORMENCE_TEST_ACT_RESPONSE_GET,
	RTK_BT_MESH_PERFORMENCE_TEST_ACT_MAX,
} rtk_bt_mesh_performence_test_act_t;

/**
 * @typedef   Bt_Mesh_Test_Node
 * @brief     BLE MESH performence test data store structure.
 */
typedef struct Node {
	uint16_t src;
	uint16_t count_ack_num;
	uint16_t response_num;
	uint16_t real_total_num;
	struct Node *next;
} Bt_Mesh_Test_Node, *Bt_Mesh_Test_Node_Point;

#endif