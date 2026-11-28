/**
 * @file      rtk_bt_mesh_performance_test.h
 * @author    pico_zhai@realsil.com.cn
 * @brief     Bluetooth LE MESH datatrans model data struct and function definition
 * @copyright Copyright (c) 2022. Realtek Semiconductor Corporation. All rights reserved.
 */

#ifndef __RTK_BT_MESH_API_PERFORMANCE_TEST_H__
#define __RTK_BT_MESH_API_PERFORMANCE_TEST_H__

#include <stdint.h>

/******************Opcode define***********************/

/** @addtogroup BT_Types BT Types
 * @{
 */
/** @addtogroup BT_Mesh_Types BT Mesh Types
 * @{
 */
/** @addtogroup BT_Mesh_Performance_Test_Types BT Mesh Performance Test Types
 * @{
 */
/**
 * @typedef   rtk_bt_mesh_performance_test_opcode_t
 * @brief     BLE MESH performance test opcode definition.
 */
typedef enum {
	RTK_BT_MESH_PERFORMANCE_TEST_PARAM_SET = 1,
	RTK_BT_MESH_PERFORMANCE_TEST_START,
	RTK_BT_MESH_PERFORMANCE_TEST_SENDING,
	RTK_BT_MESH_PERFORMANCE_TEST_END,
	RTK_BT_MESH_PERFORMANCE_TEST_REQUEST,
	RTK_BT_MESH_PERFORMANCE_TEST_RESPONSE,
	RTK_BT_MESH_PERFORMANCE_TEST_PRINT,
	RTK_BT_MESH_PERFORMANCE_TEST_OPCODE_MAX,
} rtk_bt_mesh_performance_test_opcode_t;

/**
 * @typedef   rtk_bt_mesh_performance_test_act_t
 * @brief     BLE MESH performance test act definition.
 */
typedef enum {
	RTK_BT_MESH_PERFORMANCE_TEST_ACT_PARAM_SET = 1,
	RTK_BT_MESH_PERFORMANCE_TEST_ACT_MSG_SEND,
	RTK_BT_MESH_PERFORMANCE_TEST_ACT_RESPONSE_GET,
	RTK_BT_MESH_PERFORMANCE_TEST_ACT_MAX,
} rtk_bt_mesh_performance_test_act_t;

/**
 * @typedef   Bt_Mesh_Test_Node
 * @brief     BLE MESH performance test data store structure.
 */
typedef struct TestNode {
	uint16_t src;
	uint16_t count_ack_num;
	uint16_t response_num;
	uint16_t real_total_num;
	struct TestNode *next;
} Bt_Mesh_Test_Node, *Bt_Mesh_Test_Node_Point;

/**
 * @}
 */
/**
 * @}
 */
/**
 * @}
 */
#endif
