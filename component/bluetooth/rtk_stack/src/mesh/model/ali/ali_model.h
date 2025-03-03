/**
*****************************************************************************************
*     Copyright(c) 2015, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
  * @file     ali_model.h
  * @brief    Head file for ali model.
  * @details  Data types and external functions declaration.
  * @author   bill
  * @date     2019-3-25
  * @version  v1.0
  * *************************************************************************************
  */

/* Define to prevent recursive inclusion */
#ifndef _ALI_MODEL_H
#define _ALI_MODEL_H

/* Add Includes here */
#include "mesh_api.h"

BEGIN_DECLS

/**
 * @addtogroup ALI
 * @{
 */

/**
 * @defgroup ALI_ACCESS_OPCODE Access Opcode
 * @brief Mesh message access opcode
 * @{
 */
#define MESH_MSG_ALI_TRANSPARENT_MSG                    0xCFA801
#define MESH_MSG_ALI_ATTR_GET                           0xD0A801
#define MESH_MSG_ALI_ATTR_SET                           0xD1A801
#define MESH_MSG_ALI_ATTR_SET_UNACK                     0xD2A801
#define MESH_MSG_ALI_ATTR_STAT                          0xD3A801
#define MESH_MSG_ALI_ATTR_IND                           0xD4A801
#define MESH_MSG_ALI_ATTR_CONF                          0xD5A801
#define MESH_MSG_ALI_ATTR_REQ                           0xDEA801
#define MESH_MSG_ALI_ATTR_RESP                          0xDFA801
/** @} */

/**
 * @defgroup ALI_MODEL_ID Model ID
 * @brief Mesh model id
 * @{
 */
#define MESH_MODEL_ALI_VENDOR_SERVER                    0x000001A8
#define MESH_MODEL_ALI_VENDOR_CLIENT                    0x000101A8
/** @} */

/**
 * @defgroup ALI_MESH_MSG Mesh Msg
 * @brief Mesh message types used by models
 * @{
 */
enum
{
    /* system attr */
    ALI_ATTR_TYPE_ERROR = 0x0000,
    ALI_ATTR_TYPE_VERSION = 0xFF01,
    ALI_ATTR_TYPE_DEVICE_FEATURE = 0xFF02,
    ALI_ATTR_TYPE_FLASH_TOTAL_SIZE = 0xFF03,
    ALI_ATTR_TYPE_FLASH_USED_SIZE = 0xFF04,
    ALI_ATTR_TYPE_FLASH_UNUSED_SIZE = 0xFF05,
    ALI_ATTR_TYPE_ENGINEER_MODE = 0xFF06,
    /* generic attr */
    ALI_ATTR_TYPE_WORK_STAT = 0xF001,
    ALI_ATTR_TYPE_USER_ID = 0xF002,
    ALI_ATTR_TYPE_DEVICE_NAME = 0xF003,
    ALI_ATTR_TYPE_SCENE_MODE = 0xF004,
    ALI_ATTR_TYPE_ON_OFF_SCHED = 0xF008,
    ALI_ATTR_TYPE_EVENT = 0xF009,
    ALI_ATTR_TYPE_SIGNAL_STRENGTH = 0xF00A,
    ALI_ATTR_TYPE_STEP_MOVE = 0xF00B,
    ALI_ATTR_TYPE_ELEMENT_NUM = 0xF00C,
    ALI_ATTR_TYPE_ATTR_SWITCH = 0xF00D,
    ALI_ATTR_TYPE_RMT_ADDR = 0xF00E,
    ALI_ATTR_TYPE_NEIGHBOR_DEVICE_SIGNAL_STRENGTH = 0xF00F,
    ALI_ATTR_TYPE_TIMING_SETTING = 0xF010,
    ALI_ATTR_TYPE_PERIODIC_SETTING = 0xF011,
    ALI_ATTR_TYPE_REMOVE_TIMING = 0xF012,
    ALI_ATTR_TYPE_REQ_UPD_TIMING = 0xF013,
    ALI_ATTR_TYPE_TIME_SYNC = 0xF01D,
    ALI_ATTR_TYPE_TIMEZONE_SETTING = 0xF01E,
    ALI_ATTR_TYPE_UNIX_TIME = 0xF01F,
    ALI_ATTR_TYPE_POWER_OFF_STORE = 0xF021,
    /* generic physical */
    ALI_ATTR_TYPE_ON_OFF = 0x0100,
    ALI_ATTR_TYPE_DATE = 0x0101,
    ALI_ATTR_TYPE_TIME = 0x0102,
    ALI_ATTR_TYPE_UNIX_TIMESTAMP = 0x0103,
    ALI_ATTR_TYPE_BATTERY_PERCENTAGE = 0x0104,
    ALI_ATTR_TYPE_VOLTAGE = 0x0105,
    ALI_ATTR_TYPE_CURRENT = 0x0106,
    ALI_ATTR_TYPE_POWER = 0x0107,
    ALI_ATTR_TYPE_POWER_CONSUMPTION = 0x0108,
    ALI_ATTR_TYPE_VOLUME = 0x0109,
    ALI_ATTR_TYPE_LEVEL = 0x010A,
    ALI_ATTR_TYPE_UNIT = 0x010B,
    ALI_ATTR_TYPE_TARGET_TEMPERATURE = 0x010C,
    ALI_ATTR_TYPE_PRESENT_TEMPERATURE = 0x010D,
    ALI_ATTR_TYPE_TARGET_HUMIDITY = 0x010E,
    ALI_ATTR_TYPE_PRESENT_HUMIDITY = 0x010F,
    ALI_ATTR_TYPE_FB_POSITION = 0x0110,
    ALI_ATTR_TYPE_LR_POSITION = 0x0111,
    ALI_ATTR_TYPE_UD_POSITION = 0x0112,
    ALI_ATTR_TYPE_FL_POSITION = 0x0113,
    ALI_ATTR_TYPE_BL_POSITION = 0x0114,
    ALI_ATTR_TYPE_LL_POSITION = 0x0115,
    ALI_ATTR_TYPE_RL_POSITION = 0x0116,
    ALI_ATTR_TYPE_UL_POSITION = 0x0117,
    ALI_ATTR_TYPE_DL_POSITION = 0x0118,
    ALI_ATTR_TYPE_UD_ANGLE = 0x0119,
    ALI_ATTR_TYPE_LR_ANGLE = 0x011A,
    ALI_ATTR_TYPE_UL_ANGLE = 0x011B,
    ALI_ATTR_TYPE_DL_ANGLE = 0x011C,
    ALI_ATTR_TYPE_LL_ANGLE = 0x011D,
    ALI_ATTR_TYPE_RL_ANGLE = 0x011E,
    ALI_ATTR_TYPE_POSITION_CALIBRATION = 0x011F,
    ALI_ATTR_TYPE_WATER_LEVEL = 0x0120,
    ALI_ATTR_TYPE_LIGHTNESS_LEVEL = 0x0121,
    ALI_ATTR_TYPE_COLOR_TEMPERATURE = 0x0122,
    ALI_ATTR_TYPE_HSL_COLOR = 0x0123,
    ALI_ATTR_TYPE_SECONDARY_ON_OFF = 0x0124,
    ALI_ATTR_TYPE_TERTIARY_ON_OFF = 0x0125,
    ALI_ATTR_TYPE_SECONDARY_LEVEL = 0x0128,
    ALI_ATTR_TYPE_TERTIARY_LEVEL = 0x0129,
    /* sport & health */
    ALI_ATTR_TYPE_WEIGHT = 0x0200,
    ALI_ATTR_TYPE_IMPEDANCE = 0x0201,
    ALI_ATTR_TYPE_BODY_FAT_PERCENTAGE = 0x0202,
    ALI_ATTR_TYPE_BMI = 0x0203,
    ALI_ATTR_TYPE_MUSCLE_MASS = 0x0204,
    ALI_ATTR_TYPE_MUSCLE_PERCENTAGE = 0x0205,
    ALI_ATTR_TYPE_FATFREE_MASS = 0x0206,
    ALI_ATTR_TYPE_HEIGHT = 0x0207,
    ALI_ATTR_TYPE_BODY_WATER_MASS = 0x0208,
    ALI_ATTR_TYPE_BASAL_METABOLISM = 0x0209,
    ALI_ATTR_TYPE_SPO2 = 0x020A,
    ALI_ATTR_TYPE_SYSTOLIC_PRESSURE = 0x020B,
    ALI_ATTR_TYPE_DIASTOLIC_PRESSURE = 0x020C,
    ALI_ATTR_TYPE_BLOOD_GLUCOSE = 0x020D,
    ALI_ATTR_TYPE_SPEED = 0x020E,
    ALI_ATTR_TYPE_PACE = 0x020F,
    ALI_ATTR_TYPE_SLOPE = 0x0210,
    ALI_ATTR_TYPE_MOTION_RESISTANCE_LEVEL = 0x0211,
    ALI_ATTR_TYPE_SPORT_COUNT = 0x0212,
    ALI_ATTR_TYPE_SPORT_START_TIME = 0x0213,
    ALI_ATTR_TYPE_SPORTING_TIME = 0x0214,
    ALI_ATTR_TYPE_SPORT_TARGET_TIME = 0x0215,
    ALI_ATTR_TYPE_DISTANCE = 0x0216,
    ALI_ATTR_TYPE_TARGET_DISTANCE = 0x0217,
    ALI_ATTR_TYPE_HEARTRATE = 0x0218,
    ALI_ATTR_TYPE_TACHOGRAPHS = 0x0219,
    ALI_ATTR_TYPE_TOTAL_STEP = 0x021A,
    ALI_ATTR_TYPE_SITTING_TIME = 0x021B,
    ALI_ATTR_TYPE_SPORT_TIME = 0x021C,
    ALI_ATTR_TYPE_SLEEP_TIME = 0x021D,
    ALI_ATTR_TYPE_LIGHT_SLEEP_TIME = 0x021E,
    ALI_ATTR_TYPE_DEEP_SLEEP_TIME = 0x021F,
    ALI_ATTR_TYPE_TURN_OVER = 0x0220,
    ALI_ATTR_TYPE_TOTAL_CALORIES = 0x0221,
    ALI_ATTR_TYPE_CURRENT_STEP = 0x0222,
    /* air quality */
    ALI_ATTR_TYPE_PM2_5 = 0x0300,
    ALI_ATTR_TYPE_PM10 = 0x0301,
    ALI_ATTR_TYPE_SO2 = 0x0302,
    ALI_ATTR_TYPE_NO2 = 0x0303,
    ALI_ATTR_TYPE_CO = 0x0304,
    ALI_ATTR_TYPE_CO2 = 0x0305,
    ALI_ATTR_TYPE_O3 = 0x0306,
    ALI_ATTR_TYPE_HCHO = 0x0307,
    ALI_ATTR_TYPE_TVOC = 0x0308,
    ALI_ATTR_TYPE_AQI = 0x0309,
    /* water quality */
    ALI_ATTR_TYPE_PH = 0x030A,
    ALI_ATTR_TYPE_DISSOLVED_OXYGEN = 0x030B,
    ALI_ATTR_TYPE_TDS = 0x030C,
    /* device state */
    ALI_ATTR_TYPE_DOOR_STATE = 0x0400,
    ALI_ATTR_TYPE_HUMAN_ACTIVITY = 0x0401,
    /* device function */
    ALI_ATTR_TYPE_ANGLE_AUTO_LR_ONOFF = 0x0500,
    ALI_ATTR_TYPE_ANGLE_AUTO_UD_ONOFF = 0x0501,
    ALI_ATTR_TYPE_POSITION_AUTO_LR_ONOFF = 0x0502,
    ALI_ATTR_TYPE_POSITION_AUTO_FB_ONOFF = 0x0503,
    ALI_ATTR_TYPE_POSITION_AUTO_UD_ONOFF = 0x0504,
    ALI_ATTR_TYPE_ANION_ONOFF = 0x0505,
    ALI_ATTR_TYPE_CONSTANT_HUMIDITY_ONOFF = 0x0506,
    ALI_ATTR_TYPE_SLEEP_ONOFF = 0x0507,
    ALI_ATTR_TYPE_ECO_ONOFF = 0x0508,
    ALI_ATTR_TYPE_STRONG_ONOFF = 0x0509,
    ALI_ATTR_TYPE_AUTO_ONOFF = 0x050A,
    ALI_ATTR_TYPE_CHILD_LOCK_ONOFF = 0x050C,
    ALI_ATTR_TYPE_DISPLAY_ONOFF = 0x050D,
    ALI_ATTR_TYPE_GREEN_ONOFF = 0x050E,
    ALI_ATTR_TYPE_NO_STRAIGHT_BLOWING_ONOFF = 0x050F,
    ALI_ATTR_TYPE_HEATING = 0x0510,
    ALI_ATTR_TYPE_COOLING_ONOFF = 0x0511,
    ALI_ATTR_TYPE_MASSAGE_ONOFF = 0x0512,
    ALI_ATTR_TYPE_BUBBLE_ONOFF = 0x0513,
    ALI_ATTR_TYPE_AIR_PRESSURE_ONOFF = 0x0514,
    ALI_ATTR_TYPE_VIBRATING_ONOFF = 0x0515,
    ALI_ATTR_TYPE_UP_TUBE_BAKING_ONOFF = 0x0516,
    ALI_ATTR_TYPE_BOTTOM_TUBE_BAKING_ONOFF = 0x0517,
    ALI_ATTR_TYPE_BOTH_TUBE_BAKING_ONOFF = 0x0518,
    ALI_ATTR_TYPE_HEAT_POWER = 0x0519,
    ALI_ATTR_TYPE_PRESERVE_HEAT_ONOFF = 0x051A,
    ALI_ATTR_TYPE_TASTE = 0x051B,
    ALI_ATTR_TYPE_RICE_TYPE = 0x051C,
    ALI_ATTR_TYPE_SWEET_TASTE = 0x051D,
    ALI_ATTR_TYPE_MICRO_WAVE = 0x051E,
    ALI_ATTR_TYPE_LIGHT_WAVE = 0x051F,
    ALI_ATTR_TYPE_DEODORIZATION_ONOFF = 0x0520,
    ALI_ATTR_TYPE_FAN_DIRECTION = 0x0521,
    ALI_ATTR_TYPE_STEP_ACTION_ONOFF = 0x0522,
    ALI_ATTR_TYPE_HUMAN_DETECT_ONOFF = 0x0523,
    ALI_ATTR_TYPE_IR_ONOFF = 0x0524,
    ALI_ATTR_TYPE_QUICK_FROZEN_ONOFF = 0x0525,
    ALI_ATTR_TYPE_DEFROST_ONOFF = 0x0526,
    ALI_ATTR_TYPE_ANTIBIOSIS_ONOFF = 0x0527,
    ALI_ATTR_TYPE_SILVER_ION_ONOFF = 0x0528,
    ALI_ATTR_TYPE_STRONG_WASHING_ONOFF = 0x0529,
    ALI_ATTR_TYPE_QUICK_WASHING_ONOFF = 0x052A,
    ALI_ATTR_TYPE_OFF_PEAK_ONOFF = 0x052B,
    ALI_ATTR_TYPE_LOW_TEMPERATURE_ONOFF = 0x052C,
    ALI_ATTR_TYPE_IRON_FREE_ONOFF = 0x052D,
    ALI_ATTR_TYPE_ULTRAVIOLET_ONOFF = 0x052E,
    ALI_ATTR_TYPE_AIR_DRY_ONOFF = 0x052F,
    ALI_ATTR_TYPE_CONSTANT_TEMPERATURE_ONOFF = 0x0530,
    ALI_ATTR_TYPE_QUICK_STEW_ONOFF = 0x0531,
    ALI_ATTR_TYPE_SHOCK_ABSORPTION_ONOFF = 0x0532,
    ALI_ATTR_TYPE_BACKGROUND_LIGHT_ONOFF = 0x0533,
    ALI_ATTR_TYPE_MAIN_LIGHT_ONOFF = 0x0534,
    ALI_ATTR_TYPE_MUTE_ONOFF = 0x0535,
    ALI_ATTR_TYPE_HUMIDIFICATION_ONOFF = 0x0536,
    ALI_ATTR_TYPE_CLEAN_ONOFF = 0x0537,
    ALI_ATTR_TYPE_COOL_WIND_ONOFF = 0x0538,
    ALI_ATTR_TYPE_HOT_WIND_ONOFF = 0x0539,
    ALI_ATTR_TYPE_SMART_WIND = 0x053A,
    ALI_ATTR_TYPE_EIGHT_DEGREE_HEATING = 0x053B,
    ALI_ATTR_TYPE_NOBODY_SAVE = 0x053C,
    ALI_ATTR_TYPE_LOW_TEM_DRY = 0x053D,
    ALI_ATTR_TYPE_MILDEW_PROOF = 0x053E,
    ALI_ATTR_TYPE_ELECTRIC_HEATING = 0x053F,
    ALI_ATTR_TYPE_CURRENT_POWER = 0x0540,
    ALI_ATTR_TYPE_HEALTH_FUNCTION = 0x0541,
    ALI_ATTR_TYPE_ANGLE = 0x0542,
    ALI_ATTR_TYPE_WIND_SPEED = 0x0543,
    ALI_ATTR_TYPE_TEMP_SWITCH = 0x0544,
    ALI_ATTR_TYPE_MUSIC_SWITCH = 0x0545,
    ALI_ATTR_TYPE_MASSAGE_AIR_STRENGTH = 0x0546
} _SHORT_ENUM_;
typedef uint16_t ali_attr_type_t;

enum
{
    ALI_EVENT_ID_LOW_POWER = 0x01,
    ALI_EVENT_ID_DEVICE_CRASH,
    ALI_EVENT_ID_DEVICE_POWER_ON,
    ALI_EVENT_ID_KEY_CLICK = 0x05,
    ALI_EVENT_ID_KEY_DOUBLE_CLICK,
    ALI_EVENT_ID_KEY_LONG_PRESS,
    ALI_EVENT_ID_WATER_LEVEL_TOO_HIGH,
    ALI_EVENT_ID_WATER_LEVEL_TOO_LOW,
    ALI_EVENT_ID_TEMPERATURE_TOO_HIGH,
    ALI_EVENT_ID_TEMPERATURE_TOO_LOW,
    ALI_EVENT_ID_COMPONENT_MISS,
    ALI_EVENT_ID_GAS_ALERT,
    ALI_EVENT_ID_SMOG_ALERT,
    ALI_EVENT_ID_FIRE_ALERT,
    ALI_EVENT_ID_BREAK_IN_ALERT,
    ALI_EVENT_ID_TIMING_COMPLETE,
    ALI_EVENT_ID_DEVICE_NETWORKING,
    ALI_EVENT_ID_CHARGING_START,
    ALI_EVENT_ID_CHARGING_COMPLETE
} _SHORT_ENUM_;
typedef uint8_t ali_event_id_t;

enum
{
    ALI_SCENE_NUM_DEFAULT = 0x0001,
    ALI_SCENE_NUM_AUTO,
    ALI_SCENE_NUM_READ,
    ALI_SCENE_NUM_CINEMA,
    ALI_SCENE_NUM_WARMTH,
    ALI_SCENE_NUM_NIGHT_LIGHT,
    ALI_SCENE_NUM_HELP_SLEEP,
    ALI_SCENE_NUM_GET_UP,
    ALI_SCENE_NUM_REFRIGERATION,
    ALI_SCENE_NUM_HEATING,
    ALI_SCENE_NUM_VENTILATE,
    ALI_SCENE_NUM_BLOWING_IN,
    ALI_SCENE_NUM_DEHUMIDIFICATION,
    ALI_SCENE_NUM_SLEEP,
    ALI_SCENE_NUM_LIVING,
    ALI_SCENE_NUM_MANUAL = 0x0010,
    ALI_SCENE_NUM_MUTE,
    ALI_SCENE_NUM_POWER_SAVING,
    ALI_SCENE_NUM_WIND_NORMAL,
    ALI_SCENE_NUM_WIND_NATURAL,
    ALI_SCENE_NUM_WIND_SLEEP,
    ALI_SCENE_NUM_WIND_MUTE,
    ALI_SCENE_NUM_WIND_COMFORT,
    ALI_SCENE_NUM_WIND_BABY,
    ALI_SCENE_NUM_COTTON,
    ALI_SCENE_NUM_CHEMICAL_FIBER,
    ALI_SCENE_NUM_WOOL,
    ALI_SCENE_NUM_DEGERMING,
    ALI_SCENE_NUM_CUBE_CLEAN,
    ALI_SCENE_NUM_SILK,
    ALI_SCENE_NUM_HOLIDAY,
    ALI_SCENE_NUM_SMART = 0x0020,
    ALI_SCENE_NUM_MUSIC,
    ALI_SCENE_NUM_ZERO_G,
    ALI_SCENE_NUM_ANTI_SNORE,
    ALI_SCENE_NUM_MULTI_PLAYER,
    ALI_SCENE_NUM_WOBBLING,
    ALI_SCENE_NUM_STRONG,
    ALI_SCENE_NUM_NORMAL,
    ALI_SCENE_NUM_WORK,
    ALI_SCENE_NUM_QUICK_COOL,
    ALI_SCENE_NUM_QUICK_FREEZE,
    ALI_SCENE_NUM_MICRO_DRY,
    ALI_SCENE_NUM_FULL_DRY,
    ALI_SCENE_NUM_SUPER_DRY,
    ALI_SCENE_NUM_SUMMER,
    ALI_SCENE_NUM_WINTER,
    ALI_SCENE_NUM_STANDARD = 0x0030,
    ALI_SCENE_NUM_QUICK_WASH,
    ALI_SCENE_NUM_BABY_WASH,
    ALI_SCENE_NUM_ONLY_DEHYDRATION,
    ALI_SCENE_NUM_POWER_SAVING_WASH,
    ALI_SCENE_NUM_INTELLIGENT_LIGHT,
    ALI_SCENE_NUM_LEARN,
    ALI_SCENE_NUM_BEFORE_SLEEP,
    ALI_SCENE_NUM_EYE_PROTECT_INTELLIGENT,
    ALI_SCENE_NUM_EYE_PROTECT,
    ALI_SCENE_NUM_SMASH,
    ALI_SCENE_NUM_VACUUM,
    ALI_SCENE_NUM_STIR,
    ALI_SCENE_NUM_WARM,
    ALI_SCENE_NUM_UNFREEZE,
    ALI_SCENE_NUM_FERMENTATION,
    ALI_SCENE_NUM_OPEN_COOK = 0x0040,
    ALI_SCENE_NUM_BAKE,
    ALI_SCENE_NUM_STEAM,
    ALI_SCENE_NUM_STEW,
    ALI_SCENE_NUM_STEAM_STEW,
    ALI_SCENE_NUM_NO_WATER_BAKE,
    ALI_SCENE_NUM_FRYING,
    ALI_SCENE_NUM_HOT_POT,
    ALI_SCENE_NUM_FRIED_DISH,
    ALI_SCENE_NUM_HEAT_UP_WATER,
    ALI_SCENE_NUM_SWEETMEATS,
    ALI_SCENE_NUM_EGG_CUSTARD,
    ALI_SCENE_NUM_POTATO_STEW,
    ALI_SCENE_NUM_MEAT,
    ALI_SCENE_NUM_CHICKEN_DUCK,
    ALI_SCENE_NUM_COW_SHEEP,
    ALI_SCENE_NUM_FISH = 0x0050,
    ALI_SCENE_NUM_BIG_BONES,
    ALI_SCENE_NUM_RIBS,
    ALI_SCENE_NUM_TENDON_BEAN,
    ALI_SCENE_NUM_SEASONAL_VEGETABLE,
    ALI_SCENE_NUM_FROZEN_FOODS,
    ALI_SCENE_NUM_PASTRY,
    ALI_SCENE_NUM_PIZZA,
    ALI_SCENE_NUM_ROAST_PORK,
} _SHORT_ENUM_;
typedef uint16_t ali_scene_num_t;

enum
{
    ALI_ERROR_CODE_NOT_READY = 0x80, //!< @ref HEALTH_FAULT_VENDOR_START
    ALI_ERROR_CODE_NOT_SUPPORTED_ATTR,
    ALI_ERROR_CODE_NOT_SUPPORTED_OPERATION,
    ALI_ERROR_CODE_INVALID_PARAMETER,
    ALI_ERROR_CODE_INVALID_STATE,
    ALI_ERROR_CODE_NOT_FOUND_INDEX,
    ALI_ERROR_CODE_FULL_STORAGE,
    ALI_ERROR_CODE_WRONG_FORMAT
} _SHORT_ENUM_;
typedef uint8_t ali_error_code_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_ALI_ATTR_GET)];
    uint8_t tid;
    ali_attr_type_t attr_type[1]; //!< variable number
} _PACKED4_ ali_attr_get_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_ALI_ATTR_SET)];
    uint8_t tid;
    ali_attr_type_t attr_type[0]; //!< variable number
    uint8_t attr_param[0]; //!< variable length
} _PACKED4_ ali_attr_set_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_ALI_ATTR_STAT)];
    uint8_t tid;
    ali_attr_type_t attr_type[1]; //!< variable number
    uint8_t attr_param[1]; //!< variable length
} _PACKED4_ ali_attr_stat_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_ALI_ATTR_IND)];
    uint8_t tid;
    ali_attr_type_t attr_type[1]; //!< variable number
    uint8_t attr_param[1]; //!< variable length
} _PACKED4_ ali_attr_ind_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_ALI_ATTR_CONF)];
    uint8_t tid;
} _PACKED4_ ali_attr_conf_t;

typedef struct
{
    uint8_t opcode[ACCESS_OPCODE_SIZE(MESH_MSG_ALI_TRANSPARENT_MSG)];
    uint8_t transparent_msg[1]; //!< variable length
} _PACKED4_ ali_transparent_msg_t;
/** @} */

/**
 * @defgroup ALI_ATTR_DATA Attr Definition
 * @brief attribute data struct
 * @{
 */
typedef struct
{
    ali_attr_type_t attr_type;
    ali_error_code_t error_code;
} _PACKED4_ ali_attr_error_t;

typedef struct
{
    ali_attr_type_t attr_type;
    uint16_t param_len;
    uint8_t *attr_param;
} ali_attr_t;
/** @} */

/**
 * @defgroup ALI_SERVER_DATA Server Data
 * @brief Data types and structure used by data process callback
 * @{
 */
typedef enum
{
    ALI_SERVER_DATA_TYPE_ATTR_GET, //!< @ref
    ALI_SERVER_DATA_TYPE_ATTR_SET, //!< @ref
    ALI_SERVER_DATA_TYPE_ATTR_INDICATION_RESULT, //!< @ref
    ALI_SERVER_DATA_TYPE_TRANSPARENT //!< @ref
} ali_server_data_type_t;
/** @} */

/**
 * @defgroup ALI_MODEL_API Model API
 * @brief Functions declaration
 * @{
 */

/**
 * @brief ali attribute get
 *
 * @param[in] pmodel_info: pointer to model info
 * @param[in] dst: destination
 * @param[in] app_key_index: AppKey index
 * @param[in] tid: tid
 * @param[in] attr_type: attribute type
 * @param[in] type_num: type num
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t ali_attr_get(mesh_model_info_t *pmodel_info, uint16_t dst,
                                   uint16_t app_key_index, uint8_t tid, ali_attr_type_t attr_type[], uint8_t type_num);

/**
 * @brief ali attribute confirm
 *
 * @param[in] pmodel_info: pointer to model info
 * @param[in] pmesh_msg: pointer to mesh message
 * @param[in] tid: tid
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t ali_attr_conf(mesh_model_info_t *pmodel_info, const mesh_msg_t *pmesh_msg,
                                    uint8_t tid);

/**
 * @brief ali attribute send raw data message
 *
 * @param[in] pmodel_info: pointer to model info
 * @param[in] dst: destination
 * @param[in] app_key_index: AppKey index
 * @param[in] ali_opcode: ali opcode
 * @param[in] tid: tid
 * @param[in] raw_data: pointer to raw data
 * @param[in] data_len: length of data
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t ali_attr_msg_raw(mesh_model_info_t *pmodel_info, uint16_t dst,
                                       uint16_t app_key_index, uint32_t ali_opcode, uint8_t tid, uint8_t raw_data[], uint16_t data_len);

/**
 * @brief ali attribute send message
 *
 * @param[in] pmodel_info: pointer to model info
 * @param[in] dst: destination
 * @param[in] app_key_index: AppKey index
 * @param[in] ali_opcode: ali opcode
 * @param[in] tid: tid
 * @param[in] attr: attribute
 * @param[in] attr_num: attribute num
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t ali_attr_msg(mesh_model_info_t *pmodel_info, uint16_t dst,
                                   uint16_t app_key_index, uint32_t ali_opcode, uint8_t tid, ali_attr_t attr[], uint16_t attr_num);

/**
 * @brief ali model transparent message
 *
 * @param[in] pmodel_info: pointer to model info
 * @param[in] dst: destination
 * @param[in] app_key_index: AppKey index
 * @param[in] transparent_msg: transparent message
 * @param[in] msg_len: message length
 * @return mesh_msg_send_cause_t
 */
mesh_msg_send_cause_t ali_transparent_msg(mesh_model_info_t *pmodel_info, uint16_t dst,
                                          uint16_t app_key_index, uint8_t transparent_msg[1], uint16_t msg_len);

/**
 * @brief register ali model
 *
 * @param[in] element_index: element index
 * @param[in] pmodel_info: pointer to model info
 * @param[in] server: server or client
 * @return true
 * @return false
 */
bool ali_model_reg(uint8_t element_index, mesh_model_info_p pmodel_info, bool server);
/** @} */
/** @} */

END_DECLS

#endif /* _ALI_MODEL_H */
