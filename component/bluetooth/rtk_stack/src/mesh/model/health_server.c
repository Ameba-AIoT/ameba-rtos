/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
* @file     health_server.c
* @brief    Source file for health server model.
* @details  Data types and external functions declaration.
* @author   hector_huang
* @date     2018-7-5
* @version  v1.0
* *************************************************************************************
*/

/* Add Includes here */
#include <string.h>
#include "mesh_api.h"
#include "health.h"
#if MODEL_ENABLE_DELAY_MSG_RSP
#include "delay_msg_rsp.h"
#endif


#define HEALTH_FAULT_MAX_NUM       256
#define HEALTH_FAULT_BLOCK_SIZE    32
#define HEALTH_FAULT_BLOCK_COUNT   ((HEALTH_FAULT_MAX_NUM + HEALTH_FAULT_BLOCK_SIZE - 1) / HEALTH_FAULT_BLOCK_SIZE)

typedef struct
{
    uint16_t company_id;
    uint8_t recently_test_id;
    uint8_t fast_period_divisor;
    const health_server_test_t *ptests;
    uint8_t num_tests;
    uint32_t registered_faults[HEALTH_FAULT_BLOCK_COUNT];
    uint32_t current_faults[HEALTH_FAULT_BLOCK_COUNT];
} health_info_t, *health_info_p;

static uint8_t health_server_fault_count_internal(const uint32_t *faults);
static void health_server_fill_fault(uint8_t *dst, const uint32_t *fault_bits)
{
    uint32_t temp_fault = 0;
    uint32_t bit = 0;
    uint8_t offset = 0;
    for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i)
    {
        temp_fault = fault_bits[i];
        while (0 != temp_fault)
        {
            bit = (temp_fault & ~(temp_fault - 1));
            offset = 0;
            while (0 != bit)
            {
                offset ++;
                bit >>= 1;
            }
            *dst ++ = i * 32 + offset - 1;
            temp_fault &= (temp_fault - 1);
        }
    }
}

static uint8_t hamming_weight(uint32_t data)
{
    uint32_t weight = data;
    weight = (weight & 0x55555555) + ((weight >> 1) & 0x55555555);
    weight = (weight & 0x33333333) + ((weight >> 2) & 0x33333333);
    weight = (weight & 0x0f0f0f0f) + ((weight >> 4) & 0x0f0f0f0f);
    weight = (weight & 0x00ff00ff) + ((weight >> 8) & 0x00ff00ff);
    weight = (weight & 0x0000ffff) + ((weight >> 16) & 0x0000ffff);

    return weight;
}

static uint8_t health_server_fault_count_internal(const uint32_t *faults)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i)
    {
        count += hamming_weight(faults[i]);
    }

    return count;
}

static mesh_msg_send_cause_t health_server_send(mesh_msg_p pmesh_msg, uint8_t *pmsg, uint16_t len,
                                                uint32_t delay_time)
{
    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmesh_msg->pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = pmsg;
    mesh_msg.msg_len = len;
    mesh_msg.dst = pmesh_msg->src;
    mesh_msg.app_key_index = pmesh_msg->app_key_index;
    mesh_msg.delay_time = delay_time;
    return access_send(&mesh_msg);
}

static mesh_msg_send_cause_t health_curt_stat(mesh_model_info_p pmodel_info, uint8_t test_id,
                                              uint16_t company_id, const uint32_t *fault_array, uint32_t delay_time)
{
    uint8_t fault_array_len = health_server_fault_count_internal(fault_array);
    uint16_t msg_len = MEMBER_OFFSET(health_curt_stat_t, fault_array) + fault_array_len;
    health_curt_stat_p phealth_curt_stat = (health_curt_stat_p) plt_malloc(msg_len, RAM_TYPE_DATA_OFF);
    if (phealth_curt_stat == NULL)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(phealth_curt_stat->opcode, MESH_MSG_HEALTH_CURT_STAT);
    phealth_curt_stat->test_id = test_id;
    phealth_curt_stat->company_id = company_id;
    health_server_fill_fault(phealth_curt_stat->fault_array, fault_array);

    mesh_msg_t mesh_msg;
    mesh_msg.pmodel_info = pmodel_info;
    access_cfg(&mesh_msg);
    mesh_msg.pbuffer = (uint8_t *)phealth_curt_stat;
    mesh_msg.msg_len = msg_len;
    mesh_msg.delay_time = delay_time;
    mesh_msg_send_cause_t ret = access_send(&mesh_msg);
    plt_free(phealth_curt_stat, RAM_TYPE_DATA_OFF);
    return ret;
}

static mesh_msg_send_cause_t health_fault_stat(mesh_msg_p pmesh_msg, uint8_t test_id,
                                               uint16_t company_id, const uint32_t *fault_array,
                                               uint32_t delay_time)
{
    mesh_msg_send_cause_t ret;
    uint8_t fault_array_len = health_server_fault_count_internal(fault_array);
    uint16_t msg_len = MEMBER_OFFSET(health_curt_stat_t, fault_array) + fault_array_len;
    health_curt_stat_p pmsg = (health_curt_stat_p) plt_malloc(msg_len, RAM_TYPE_DATA_OFF);
    if (pmsg == NULL)
    {
        return MESH_MSG_SEND_CAUSE_NO_MEMORY;
    }

    ACCESS_OPCODE_BYTE(pmsg->opcode, MESH_MSG_HEALTH_FAULT_STAT);
    pmsg->test_id = test_id;
    pmsg->company_id = company_id;
    health_server_fill_fault(pmsg->fault_array, fault_array);
    ret = health_server_send(pmesh_msg, (uint8_t *)pmsg, msg_len, delay_time);
    plt_free(pmsg, RAM_TYPE_DATA_OFF);
    return ret;
}

static bool is_health_server_has_fault(const mesh_model_info_p pmodel_info)
{
    health_info_p phealth_info = pmodel_info->pargs;
    bool has = FALSE;
    for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i)
    {
        if (0 != phealth_info->current_faults[i])
        {
            has = TRUE;
        }
    }

    return has;
}

/**
 * @brief health model publish callback
 * @param pmodel_info - health model
 * @param retrans - retransmit times
 * @retval 0: nothing to do
 * @retval >0: new timer interval
 * @retval <0: reserved
 */
static int32_t health_server_publish(mesh_model_info_p pmodel_info, bool retrans)
{
    UNUSED(retrans);
    health_info_p phealth_info = pmodel_info->pargs;

    health_curt_stat(pmodel_info, phealth_info->recently_test_id, phealth_info->company_id,
                     phealth_info->current_faults, 0);

    if (is_health_server_has_fault(pmodel_info))
    {
        /* need to fast timer interval */
        uint32_t divisor = (1 << phealth_info->fast_period_divisor);
        return mesh_model_pub_period_get(pmodel_info->pmodel) / divisor;
    }

    return 0;
}

static mesh_msg_send_cause_t health_period_stat(mesh_msg_p pmesh_msg, uint8_t fast_period_divisor,
                                                uint32_t delay_time)
{
    health_period_stat_t msg;
    msg.fast_period_divisor = fast_period_divisor;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_PERIOD_STAT);
    return health_server_send(pmesh_msg, (uint8_t *)&msg, sizeof(health_period_stat_t), delay_time);
}

static mesh_msg_send_cause_t health_attn_stat(mesh_msg_p pmesh_msg, uint8_t attn,
                                              uint32_t delay_time)
{
    health_attn_stat_t msg;
    msg.attn = attn;
    ACCESS_OPCODE_BYTE(msg.opcode, MESH_MSG_HEALTH_ATTN_STAT);
    return health_server_send(pmesh_msg, (uint8_t *)&msg, sizeof(health_attn_stat_t), delay_time);
}

uint8_t health_server_fault_count(const mesh_model_info_p pmodel_info)
{
    health_info_p phealth_info = pmodel_info->pargs;
    return health_server_fault_count_internal(phealth_info->current_faults);
}

void health_server_set_tests(mesh_model_info_p pmodel_info, const health_server_test_t *ptests,
                             uint8_t num_tests)
{
    health_info_p phealth_info = pmodel_info->pargs;
    phealth_info->ptests = ptests;
    phealth_info->num_tests = num_tests;
}

void health_server_set_company_id(mesh_model_info_p pmodel_info, uint16_t company_id)
{
    health_info_p phealth_info = pmodel_info->pargs;
    phealth_info->company_id = company_id;
}

void health_server_fault_register(mesh_model_info_p pmodel_info, uint8_t fault)
{
    health_info_p phealth_info = pmodel_info->pargs;
    uint8_t pos = fault / HEALTH_FAULT_BLOCK_SIZE;
    uint8_t bit = fault % HEALTH_FAULT_BLOCK_SIZE;
    phealth_info->registered_faults[pos] |= (1 << bit);
    phealth_info->current_faults[pos] |= (1 << bit);

    /* TODO: only need to fast from 0 to 1 */
    health_curt_stat(pmodel_info, phealth_info->recently_test_id, phealth_info->company_id,
                     phealth_info->current_faults, 0);
    /* need to fast timer now */
    mesh_model_p pmodel = pmodel_info->pmodel;
    if (NULL != pmodel->pub_timer)
    {
        uint32_t divisor = (1 << phealth_info->fast_period_divisor);
        uint32_t pub_period = mesh_model_pub_period_get(pmodel);
        if (pub_period > 0)
        {
            plt_timer_change_period(pmodel->pub_timer, pub_period / divisor, 0);
        }
    }
}

void health_server_fault_clear(mesh_model_info_p pmodel_info, uint8_t fault)
{
    health_info_p phealth_info = pmodel_info->pargs;
    uint8_t pos = fault / HEALTH_FAULT_BLOCK_SIZE;
    uint8_t bit = fault % HEALTH_FAULT_BLOCK_SIZE;
    phealth_info->current_faults[pos] &= ~bit;
}

void health_server_fault_clear_all(mesh_model_info_p pmodel_info)
{
    health_info_p phealth_info = pmodel_info->pargs;
    for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i)
    {
        phealth_info->current_faults[i] = 0;
    }
}

static void health_server_registered_fault_clear_all(mesh_model_info_p pmodel_info)
{
    health_info_p phealth_info = pmodel_info->pargs;
    for (uint8_t i = 0; i < HEALTH_FAULT_BLOCK_COUNT; ++i)
    {
        phealth_info->registered_faults[i] = 0;
    }
}

bool health_server_fault_is_set(const mesh_model_info_p pmodel_info, uint8_t fault)
{
    health_info_p phealth_info = pmodel_info->pargs;
    uint8_t pos = fault / HEALTH_FAULT_BLOCK_SIZE;
    uint8_t bit = fault % HEALTH_FAULT_BLOCK_SIZE;

    return (0x01 == (phealth_info->current_faults[pos] & (1 << bit)));
}


/**
 * @brief health model receive callback
 * @param pmesh_msg - received mesh message
 */
static bool health_server_receive(mesh_msg_p pmesh_msg)
{
    bool ret = TRUE;
    uint8_t *pbuffer = pmesh_msg->pbuffer + pmesh_msg->msg_offset;
	mesh_model_info_p pmodel_info = pmesh_msg->pmodel_info;
	switch (pmesh_msg->access_opcode) {
    case MESH_MSG_HEALTH_ATTN_GET:
        if (pmesh_msg->msg_len == sizeof(health_attn_get_t))
        {
            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
			/**
			 * Add a method to get data from app layer
			*/
			health_server_attn_t attn_value;
			if (NULL != pmodel_info->model_data_cb) {
				pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_GET_ATTENTION, &attn_value);
			}
			// BT_LOGA("Attenetion get from app: %d \r\n",attn_value.attn);
            health_attn_stat(pmesh_msg, attn_timer_get(pmesh_msg->pmodel_info->element_index), delay_rsp_time);
        }
        break;
    case MESH_MSG_HEALTH_ATTN_SET:
    case MESH_MSG_HEALTH_ATTN_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(health_attn_set_t))
        {
            health_attn_set_p pmsg = (health_attn_set_p)pbuffer;
            attn_timer_start(pmesh_msg->pmodel_info->element_index, pmsg->attn);
            if (pmesh_msg->access_opcode == MESH_MSG_HEALTH_ATTN_SET)
            {
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
				/**
				 * Add a method to indicate app layer
				*/
				health_server_attn_t attn_value;
				attn_value.attn = pmsg->attn;
				if (NULL != pmodel_info->model_data_cb) {
					pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_SET_ATTENTION, &attn_value);
				}
                health_attn_stat(pmesh_msg, attn_timer_get(pmesh_msg->pmodel_info->element_index), delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_HEALTH_PERIOD_GET:
        if (pmesh_msg->msg_len == sizeof(health_period_get_t))
        {
            health_info_p phealth_info = pmesh_msg->pmodel_info->pargs;

            uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
            delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
			/**
			 * Add a method to get data from app layer
			*/
			health_server_period_t period_value;
			if (NULL != pmodel_info->model_data_cb) {
				pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_GET_PERIOD, &period_value);
			}
			// BT_LOGA("Period get from app: %d \r\n",period_value.fast_period_divisor);
            health_period_stat(pmesh_msg, phealth_info->fast_period_divisor, delay_rsp_time);
        }
        break;
    case MESH_MSG_HEALTH_PERIOD_SET:
    case MESH_MSG_HEALTH_PERIOD_SET_UNACK:
        if (pmesh_msg->msg_len == sizeof(health_period_set_t))
        {
            health_period_set_p pmsg = (health_period_set_p)pbuffer;
            health_info_p phealth_info = pmesh_msg->pmodel_info->pargs;
            if (IS_FAST_PERIOD_DIVISOR_VALID(pmsg->fast_period_divisor))
            {
                phealth_info->fast_period_divisor = (pmsg->fast_period_divisor & 0x0f);
                if (MESH_MSG_HEALTH_PERIOD_SET == pmesh_msg->access_opcode)
                {
                    uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
				/**
				 * Add a method to indicate app layer
				*/
				health_server_period_t period_value;
				period_value.fast_period_divisor = pmsg->fast_period_divisor;
				if (NULL != pmodel_info->model_data_cb) {
					pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_SET_PERIOD, &period_value);
				}
                    health_period_stat(pmesh_msg, phealth_info->fast_period_divisor, delay_rsp_time);
                }
            }
        }
        break;
    case MESH_MSG_HEALTH_FAULT_GET:
        if (pmesh_msg->msg_len == sizeof(health_fault_get_t))
        {
            health_fault_get_p pmsg = (health_fault_get_p)pbuffer;
            health_info_p phealth_info = pmesh_msg->pmodel_info->pargs;
            if (pmsg->company_id == phealth_info->company_id)
            {
                uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
				/**
				 * Add a method to get data from and indicate app layer
				*/
				health_server_fault_get_t fault_status;
				fault_status.company_id = pmsg->company_id;
				if (NULL != pmodel_info->model_data_cb) {
					pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_GET_FAULT, &fault_status);
				}
				// BT_LOGA("fault array:");
				// BT_DUMPA("", &fault_status.fault_array[1], fault_status.fault_array[0]);
                health_fault_stat(pmesh_msg, phealth_info->recently_test_id, phealth_info->company_id,
                                  phealth_info->registered_faults, delay_rsp_time);
            }
        }
        break;
    case MESH_MSG_HEALTH_FAULT_CLEAR:
    case MESH_MSG_HEALTH_FAULT_CLEAR_UNACK:
        if (pmesh_msg->msg_len == sizeof(health_fault_clear_t))
        {
            health_fault_clear_p pmsg = (health_fault_clear_p)pbuffer;
            health_info_p phealth_info = pmesh_msg->pmodel_info->pargs;
            if (pmsg->company_id == phealth_info->company_id)
            {
                health_server_registered_fault_clear_all(pmesh_msg->pmodel_info);
                if (MESH_MSG_HEALTH_FAULT_CLEAR == pmesh_msg->access_opcode)
                {
                    uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                    delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
					/**
					 * Add a method to get data from and indicate app layer
					*/
					health_server_fault_clear_t fault_status;
					fault_status.company_id = pmsg->company_id;
					if (NULL != pmodel_info->model_data_cb) {
						pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_CLEAR_FAULT, &fault_status);
					}
                    // BT_LOGA("fault array:");
                    // BT_DUMPA("", &fault_status.fault_array[1], fault_status.fault_array[0]);
                    health_fault_stat(pmesh_msg, phealth_info->recently_test_id, phealth_info->company_id,
                                      phealth_info->registered_faults, delay_rsp_time);
                }
            }
        }
        break;
    case MESH_MSG_HEALTH_FAULT_TEST:
    case MESH_MSG_HEALTH_FAULT_TEST_UNACK:
        if (pmesh_msg->msg_len == sizeof(health_fault_test_t))
        {
            health_fault_test_p pmsg = (health_fault_test_p)pbuffer;
            health_info_p phealth_info = pmesh_msg->pmodel_info->pargs;
            if (pmsg->company_id == phealth_info->company_id)
            {
                uint8_t i = 0;
                for (; i < phealth_info->num_tests; ++i)
                {
                    if (phealth_info->ptests[i].test_id == pmsg->test_id)
                    {
                        phealth_info->recently_test_id = pmsg->test_id;
                        if (NULL != phealth_info->ptests[i].test_cb)
                        {
                            phealth_info->ptests[i].test_cb(pmesh_msg->pmodel_info, phealth_info->company_id, pmsg->test_id);
                        }
                        break;
                    }
                }
                if (i < phealth_info->num_tests)
                {
                    if (MESH_MSG_HEALTH_FAULT_TEST == pmesh_msg->access_opcode)
                    {
                        uint32_t delay_rsp_time = 0;
#if MODEL_ENABLE_DELAY_MSG_RSP
                        delay_rsp_time = delay_msg_get_rsp_delay(pmesh_msg->dst);
#endif
                        health_fault_stat(pmesh_msg, phealth_info->recently_test_id, phealth_info->company_id,
                                          phealth_info->registered_faults, delay_rsp_time);
                    }
                }
				/**
				 * Add a method to get data from and indicate app layer
				*/
				health_server_fault_test_t fault_status;
				fault_status.company_id = pmsg->company_id;
				fault_status.test_id = pmsg->test_id;
				if (NULL != pmodel_info->model_data_cb) {
					pmodel_info->model_data_cb(pmodel_info, HEALTH_SERVER_TEST_FAULT, &fault_status);
            }
				// BT_LOGA("fault array:");
				// BT_DUMPA("", &fault_status.fault_array[1], fault_status.fault_array[0]);
        }
		}
        break;
    default:
        ret = FALSE;
        break;
    }
    return ret;
}

#if MESH_MODEL_ENABLE_DEINIT
static void health_server_deinit(mesh_model_info_t *pmodel_info)
{
    if (pmodel_info->model_receive == health_server_receive)
    {
        if (NULL != pmodel_info->pargs)
        {
            plt_free(pmodel_info->pargs, RAM_TYPE_DATA_ON);
            pmodel_info->pargs = NULL;
        }
        pmodel_info->model_receive = NULL;
    }
}
#endif

bool health_server_reg(uint16_t element_index, mesh_model_info_p pmodel_info)
{
    if (NULL == pmodel_info)
    {
        return FALSE;
    }

    pmodel_info->model_id = MESH_MODEL_HEALTH_SERVER;
    if (NULL == pmodel_info->model_receive)
    {
        pmodel_info->pargs = plt_malloc(sizeof(health_info_t), RAM_TYPE_DATA_ON);
        if (NULL == pmodel_info->pargs)
        {
            printe("health_server_reg: fail to allocate memory for the new model extension data!");
            return FALSE;
        }
        memset(pmodel_info->pargs, 0, sizeof(health_info_t));

        pmodel_info->model_receive = health_server_receive;

#if MESH_MODEL_ENABLE_DEINIT
        pmodel_info->model_deinit = health_server_deinit;
#endif
    }

    if (NULL == pmodel_info->model_pub_cb)
    {
        pmodel_info->model_pub_cb = health_server_publish;
    }

    return mesh_model_reg(element_index, pmodel_info);
}

