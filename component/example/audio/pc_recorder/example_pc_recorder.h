#ifndef _EXAMPLE_PC_RECORDER_H_
#define _EXAMPLE_PC_RECORDER_H_

#include "serial_api.h"
#include "serial_ex_api.h"
#include "PinNames.h"
#include "ameba_soc.h"
#include "os_wrapper.h"
#include "cJSON/cJSON.h"
#include "audio/audio_record.h"
#include "audio/audio_track.h"
#include "audio/audio_control.h"
#include "platform_stdlib.h"
#include "basic_types.h"


#define PR_MSG_ACK		0x00
#define PR_MSG_ERROR	0x01
#define PR_MSG_CONFIG	0x02
#define PR_MSG_START	0x03
#define PR_MSG_STOP		0x04
#define PR_MSG_DATA		0x05
#define PR_MSG_QUERY	0x06
#define PR_MSG_VOLUME	0x07

#define PR_VERSION		"1.0"
#define RECORD_IDLE		0x0
#define RECORD_BUSY		0x1


#define PR_AMIC1		BIT(0)
#define PR_AMIC1_REF	BIT(1)
#define PR_AMIC2		BIT(2)
#define PR_AMIC2_REF	BIT(3)
#define PR_AMIC3		BIT(4)
#define PR_AMIC3_REF	BIT(5)
#define PR_AMIC4		BIT(6)
#define PR_AMIC4_REF	BIT(7)
#define PR_DMIC1		BIT(8)
#define PR_DMIC1_REF	BIT(9)
#define PR_DMIC2		BIT(10)
#define PR_DMIC2_REF	BIT(11)
#define PR_DMIC3		BIT(12)
#define PR_DMIC3_REF	BIT(13)
#define PR_DMIC4		BIT(14)
#define PR_DMIC4_REF	BIT(15)


#define PR_AMIC_NONE			0xFF
#define PR_DMIC_NONE			0xFF
#define PR_MIC_REF				BIT(16)
#define MAX_CHANNEL_COUNT		8


struct pr_msg {
	int item;
	char *item_str;
};

typedef struct msg_attrib_s {
	int type;
	int samplerate;
	int device;
	int format;
	int chnum;
	u8 chmap[256];
	int offset;
	int refch;
	int adcindex;
	int mode;
	int ch_src[MAX_CHANNEL_COUNT];
	u8 *url;
} msg_attrib_t;

typedef struct pr_adapter_s {
	int rx_cnt;
	int tx_cnt;
	u8 record_stop;
	u8 record_status;
} pr_adapter_t;

void example_pc_recorder(void);


#endif

