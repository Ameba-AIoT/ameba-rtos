/**
 * @file ameba_diagnose.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2025-05-27
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef _AMEBA_DIAGNOSE_H_
#define _AMEBA_DIAGNOSE_H_

#include "ameba_soc.h"
#include "ameba_diagnose_types.h"

/*****************************************************************************************************
 *                                   Event User's Definitions
 ****************************************************************************************************/

#define RTK_DIAG_EVENT_DEFAULT_LEVEL RTK_EVENT_LEVEL_INFO

// Only level >= RTK_EVENT_COMPIL_LEVEL will be added to the linked list
#define RTK_DIAG_EVENT_ADD(_level, _type, _info, _len) \
    ((RTK_DIAG_EVENT_DEFAULT_LEVEL >= (_level)) ? \
        rtk_diag_event_add(_level, _type, _info, _len) : \
        RTK_ERR_DIAG_EVT_ADD_FAIL)


#define RTK_EVENT_A(_type, _info, _len)  RTK_DIAG_EVENT_ADD(RTK_EVENT_LEVEL_ALWAYS, _type, _info, _len)
#define RTK_EVENT_E(_type, _info, _len)  RTK_DIAG_EVENT_ADD(RTK_EVENT_LEVEL_ERROR, _type, _info, _len)
#define RTK_EVENT_W(_type, _info, _len)  RTK_DIAG_EVENT_ADD(RTK_EVENT_LEVEL_WARNING, _type, _info, _len)
#define RTK_EVENT_I(_type, _info, _len)  RTK_DIAG_EVENT_ADD(RTK_EVENT_LEVEL_INFO, _type, _info, _len)
#define RTK_EVENT_D(_type, _info, _len)  RTK_DIAG_EVENT_ADD(RTK_EVENT_LEVEL_DEBUG, _type, _info, _len)


/*****************************************************************************************************
 *                                   Event Manager's Definitions
 ****************************************************************************************************/

//NOTE: Porting ipc for each ic
#if defined(CONFIG_AMEBAD)
#define DIAG_IPC_DIR NULL
#define DIAG_IPC_DEV IPCM0_DEV
#define DIAG_IPC_TX_CHANNEL_SHIFT (0)
#elif defined(CONFIG_AMEBADPLUS)
#define DIAG_IPC_DIR IPC_KM4_TO_KM0
#define DIAG_IPC_DEV IPCKM0_DEV
#define DIAG_IPC_TX_CHANNEL_SHIFT IPC_TX_CHANNEL_SHIFT
#elif defined(CONFIG_AMEBAGREEN2) || defined(CONFIG_AMEBAL2) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBASMARTPLUS)
#define DIAG_IPC_DIR IPC_AP_TO_NP
#define DIAG_IPC_DEV IPCNP_DEV
#if defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBASMARTPLUS)
#define DIAG_IPC_TX_CHANNEL_SHIFT (0)
#else
#define DIAG_IPC_TX_CHANNEL_SHIFT IPC_TX_CHANNEL_SHIFT
#endif
#elif defined(CONFIG_AMEBALITE)
#define DIAG_IPC_DIR IPC_KM4_TO_KR4
#define DIAG_IPC_DEV IPCKR4_DEV
#define DIAG_IPC_TX_CHANNEL_SHIFT (0) //FIXME: modify to the right value
#else
#error no ic defined
#endif

#if (defined CONFIG_WHC_DEV || defined CONFIG_WHC_NONE)
int rtk_diag_init(u16 heap_capacity, u16 sender_buffer_size);
#else //CONFIG_WHC_HOST
int rtk_diag_init(void);
#endif
void rtk_diag_deinit(void);
int rtk_diag_event_add(u8 evt_level, u16 evt_type, const u8 *evt_info, u16 evt_len);

//handle atcmd request and response
int rtk_diag_req_timestamp(void); //获取当前系统时间戳
int rtk_diag_req_version(void); //获取版本信息
int rtk_diag_req_set_buf_com_capacity(u16 capacity); //设置通信缓冲容量
int rtk_diag_req_set_buf_evt_capacity(u16 capacity); //设置事件总堆占用上限
// int rtk_diag_req_set_buf_evt_pool_capacity(u16 capacity); //设置小事件内存池容量
int rtk_diag_req_get_event(u32 timestamp, u16 offset); //获取event
int rtk_diag_req_del_event_before(u32 timestamp); //删除指定时间(含)前事件
int rtk_diag_req_del_event_after(u32 timestamp); //删除指定时间后事件
int rtk_diag_req_get_del_events(void); //获取最近删除的事件
int rtk_diag_req_clr_del_events(void); //清空最近删除的事件
int rtk_diag_req_log_enable(u8 state); //设置开启或关闭log


//for debug
#ifndef CONFIG_AMEBA_RLS
int rtk_diag_req_add_event_demo1(u8 evt_level, const char *data);
int rtk_diag_req_dbg_log_enable(u8 state);
#endif
#endif //_AMEBA_DIAGNOSE_H_
