#ifndef _TSF_SYNC_DEV_H_
#define _TSF_SYNC_DEV_H_

#include "basic_types.h"

/* Cmd path frame: [desc][u32 WHC_WIFI_TEST][u8 subtype][payload]. The payload is
 * at a 5-byte offset, so memcpy the structs below instead of casting. */
#define TSF_HDR_LEN       (sizeof(u32) + 1)

#define TSF_ACK_DONE      0u
#define TSF_ACK_NO_PULSE  1u

/* Must be byte-identical with the host side definitions. */
struct tsf_sync_sample {
	u32 seq;
	u64 tsf;   /* port0 TSF, in us */
} __attribute__((packed));

struct tsf_ack {
	u32 seq;
	u32 result;
} __attribute__((packed));

void tsf_sync_dev_start(void);
void tsf_sync_dev_stop(void);

#endif /* _TSF_SYNC_DEV_H_ */
