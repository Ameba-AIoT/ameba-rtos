#ifndef rtw_nan_cmd_api_h__
#define rtw_nan_cmd_api_h__

#include "stack_api.h"
#include "rtw_nan_struct.h"

#define DEBUG_LEVEL 1
#if DEBUG_LEVEL > 1
#define INFO_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#define DEBUG_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#elif DEBUG_LEVEL > 0
#define INFO_PRINT(...) do{ fprintf( stderr, __VA_ARGS__ ); } while(0)
#define DEBUG_PRINT(...) do{ } while (0)
#else
#define INFO_PRINT(...) do{ } while(0)
#define DEBUG_PRINT(...) do{ } while (0)
#endif

/* event relared api*/
extern void rtw_nan_api_register_evt_cb(rtw_nan_evt_cb cb);

/* manual-trigger nan api */
extern RTW_RET_STATUS rtw_nan_api_get_capability(char *intf, uint16_t *nan_cap);
extern RTW_RET_STATUS rtw_nan_api_init(uint16_t phy_num, char *intf);
extern RTW_RET_STATUS rtw_nan_api_deinit(uint16_t phy_num);
extern RTW_RET_STATUS rtw_nan_api_start_nan(char *intf);
extern RTW_RET_STATUS rtw_nan_api_stop_nan(char *intf);
extern RTW_RET_STATUS rtw_nan_api_config_nan();

/* ble-trigger nan api */
extern RTW_RET_STATUS rtw_nan_api_ble_init(enum ble_role role);
extern RTW_RET_STATUS rtw_nan_api_ble_deinit();
//extern RTW_RET_STATUS rtw_nan_api_ble_start_tds_srvc(char *service);
extern RTW_RET_STATUS rtw_nan_api_ble_start_ble_trigger(uint8_t trigger_nan);
extern RTW_RET_STATUS rtw_nan_api_ble_stop();

/* nan service api */
extern RTW_RET_STATUS rtw_nan_api_publish(char *intf, struct srvc_info *info);
extern RTW_RET_STATUS rtw_nan_api_publish_update(struct srvc_info *info);
extern RTW_RET_STATUS rtw_nan_api_subscribe(char *intf, struct srvc_info *info);
extern RTW_RET_STATUS rtw_nan_api_cancel_publish(char *intf, uint8_t id);
extern RTW_RET_STATUS rtw_nan_api_cancel_subscribe(char *intf, uint8_t id);

/* nan datapath api */
extern RTW_RET_STATUS rtw_nan_api_send_datapath_req(struct datapath_info *info);
extern RTW_RET_STATUS rtw_nan_api_send_datapath_rsp(struct datapath_info *info);
extern RTW_RET_STATUS rtw_nan_api_send_datapath_confirm(uint8_t datapath_id);
extern RTW_RET_STATUS rtw_nan_api_send_datapath_end(uint8_t datapath_id);

/* nan pairing api */
extern RTW_RET_STATUS rtw_nan_api_send_follow_up(struct tx_followup_info *tx_flwup_data);
extern RTW_RET_STATUS rtw_nan_api_pasn_setpw(uint8_t *pairing_peer_nmi, char *pw, uint8_t pw_len);
extern RTW_RET_STATUS rtw_nan_api_pasn_start(struct pairing_info *pairing_info);

#endif
