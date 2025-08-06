#ifndef _RTK_AIVOICE_INTERFACE_H_
#define _RTK_AIVOICE_INTERFACE_H_

// user configure structures
#include "aivoice_afe_config.h"
#include "aivoice_vad_config.h"
#include "aivoice_kws_config.h"
#include "aivoice_asr_config.h"
#include "aivoice_sdk_config.h"

struct aivoice_config {
	struct afe_config *afe;
	struct vad_config *vad;
	struct kws_config *kws;
	struct asr_config *asr;
	struct aivoice_sdk_config *common;
	const char *resource;   /* memory start address of aivoice binary resources,
                                which MUST be 16 bytes aligned.
                                Since aivoice does not do any resource copying work internally,
                                users need to load resource from flash to memory first. */
};

enum aivoice_out_event_type {
	AIVOICE_EVOUT_VAD = 0,          // callback message type: struct aivoice_evout_vad
	AIVOICE_EVOUT_WAKEUP,           // callcack message type: char *json
	AIVOICE_EVOUT_ASR_RESULT,       // callcack message type: char *json
	AIVOICE_EVOUT_AFE,              // callback message type: struct aivoice_evout_afe
	AIVOICE_EVOUT_ASR_REC_TIMEOUT,  // callback message is NULL
	AIVOICE_EVOUT_AGE_GENDER_RESULT,// callback message type: char *json
};

struct aivoice_evout_vad {
	int status;                     /*  0: vad is changed from speech to silence,
                                           indicating the end point of a speech segment
                                        1: vad is changed from silence to speech,
                                           indicating the start point of a speech segment */
	unsigned int offset_ms;         /* time offset relative to reset point. */
};

struct aivoice_evout_afe {
	int     ch_num;                       /* channel number of output audio signal, default: 1 */
	short  *data;                         /* enhanced audio signal samples */
	char   *out_others_json;              /* reserved for other output data, like flags, key: value */
};

////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

/** @brief The callback function called by aivoice.
 *
 * @param[in] user_data     data provided by user
 * @param[in] event_type    event type.
 * @param[in] msg           detail message about the event_type;
 *                          it may be a string, a structure, raw audio data... or NULL,
 *                          depending on the event_type.
 *                          refer to "enum aivoice_out_event_type" comment.
 * @param[in] len           length of msg
 *
 * @retval  0: success;  others: error;
 * */
typedef int (*aivoice_callback_handler)(void *user_data,
										enum aivoice_out_event_type event_type,
										const void *msg, int len);

/**
 * @brief Create an aivoice instance.
 *
 * @param[in] config    aivoice configuration.
 *                      1. you MUST set a configuration to config->afe.
 *                      2. for config->vad/kws/asr/common, it can be NULL,
 *                          then the default configures will be used.
 *
 * @retval    aivoice instance handle, or `nullptr` to indicate an error.
 */
typedef void *(*prtk_aivoice_create)(struct aivoice_config *config);

/**
 * @brief Destroy the aivoice instance
 *
 * @param handle        aivoice instance
 */
typedef void (*prtk_aivoice_destroy)(void *handle);

/**
 * @brief Reset the aivoice instance
 *
 * @param handle        aivoice instance
 */
typedef void (*prtk_aivoice_reset)(void *handle);

/**
 * @brief Feed audio to aivoice.
 *
 * @param[in] handle        aivoice instance create by prtk_aivoice_create.
 *       [in] input_data    audio;
 *                          channels interleaved when use multi-channels;
 *       [in]  length       bytes in one frame of input_data.
 *                          for flows with afe, one frame is (config->afe->frame_size*1000/config->afe->sample_rate) ms.
 *                          for flows without afe, one frame is 16 ms.
 * @retval  0: success;  others: error code.
 */
typedef int (*prtk_aivoice_feed)(void *handle, char *input_data, int length);

/**
 * @brief Register callback function
 *
 * @param[in] handle                aiovice instance
 * @param[in] user_callback_func    callback handler
 * @param[in] user_data             user data
 */
void rtk_aivoice_register_callback(void *handle,
								   aivoice_callback_handler user_callback_func,
								   void *user_data);

/**
 * @brief Trigger SSL to get the voice angle.
 *        Only available when afe module is used.
 *
 *        You can call this function at the moment of wakeup or recognition,
 *        then next time after feed, you'll get the output angle
 *        in callback message AIVOICE_EVOUT_AFE:aivoice_evout_afe->out_others_json.
 *
 * @param[in] handle      aiovice instance
 * @param[in] duration    length (number of sampleling points in one channel) of valid voice.
 */
void rtk_aivoice_trigger_ssl(void *handle, int duration);

////////////////////////////////////////////////////////////////////////////////
/**
 * This structure contains the functions used to do operations on aivoice.
 */
struct rtk_aivoice_iface {
	prtk_aivoice_create    create;
	prtk_aivoice_destroy   destroy;
	prtk_aivoice_reset     reset;
	prtk_aivoice_feed      feed;
};

#ifdef __cplusplus
}
#endif

////////////////////////////////////////////////////////////////////////////////
extern const struct rtk_aivoice_iface aivoice_iface_full_flow_v1;
extern const struct rtk_aivoice_iface aivoice_iface_afe_kws_v1;
extern const struct rtk_aivoice_iface aivoice_iface_afe_kws_vad_v1;
extern const struct rtk_aivoice_iface aivoice_iface_afe_v1;
extern const struct rtk_aivoice_iface aivoice_iface_vad_v1;
extern const struct rtk_aivoice_iface aivoice_iface_kws_v1;
extern const struct rtk_aivoice_iface aivoice_iface_asr_v1;


#endif // _RTK_AIVOICE_INTERFACE_H_
