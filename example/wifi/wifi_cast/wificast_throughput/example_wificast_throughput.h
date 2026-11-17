#ifndef _EXAMPLE_WIFICAST_THROUGHPUT_H_
#define _EXAMPLE_WIFICAST_THROUGHPUT_H_

#include "../wificast_defs.h"
#include "os_wrapper.h"

#define TP_MAGIC            0x54505431  /* "TPT1" */
#define TP_FLAG_END         BIT(0)      /* last packet of a test, seq field carries total packets sent */

#define TP_DEFAULT_SECONDS  10
#define TP_MAX_SECONDS      86400       /* one day, keeps seconds * 1000 within u32 */
#define TP_LAT_BUCKETS      6           /* ack latency histogram buckets, see tp_lat_bound */
#define TP_REPORT_MS        1000
#define TP_RETRY_DEFAULT    0xFF        /* retry_limit not set, keep the driver default */
#define TP_WAIT_MS_DEFAULT  100         /* ACK wait budget when ack = 1, the driver default (3000) stalls 3 s on every lost packet */

/**
 * @brief Throughput test packet header, followed by padding up to the requested length.
 */
struct tp_hdr {
	uint32_t magic;   /* TP_MAGIC */
	uint32_t session; /* test session id, changes on every `wificast tx` */
	uint32_t seq;     /* packet sequence number, total packet count when TP_FLAG_END is set */
	uint32_t flags;   /* TP_FLAG_xxx */
} __attribute__((packed));

#endif /* _EXAMPLE_WIFICAST_THROUGHPUT_H_ */
