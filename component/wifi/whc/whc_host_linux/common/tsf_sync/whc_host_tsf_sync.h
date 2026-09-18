#ifndef __WHC_HOST_TSF_SYNC_H__
#define __WHC_HOST_TSF_SYNC_H__

#include <whc_host_linux.h>
#include <whc_host_netlink.h>
#include <whc_host_cmd_path_api.h>
#include "tsf_latch.h"

#define TSF_HDR_LEN        (sizeof(u32) + 1)

/* Must be byte-identical with the device side definitions. */
struct tsf_sync_sample {
	u32 seq;
	u64 tsf;   /* port0 TSF, in us */
} __attribute__((packed));

struct tsf_ack {
	u32 seq;
	u32 result;   /* TSF_ACK_DONE / TSF_ACK_NO_PULSE */
} __attribute__((packed));

#define TSF_ACK_DONE       0u
#define TSF_ACK_NO_PULSE   1u

#if (KERNEL_VERSION(6, 12, 0) <= LINUX_VERSION_CODE)

#define TSF_PULSE_PIN			585

#elif (KERNEL_VERSION(6, 6, 0) <= LINUX_VERSION_CODE)

/* For kernel version higher than 6.6, gpio pins of raspberry pi 5 are numbered differently.
check the correct number of the required GPIO pin to use command " cat /sys/kernel/debug/gpio" */
#define TSF_PULSE_PIN			587

#else

#define TSF_PULSE_PIN			18

#endif

#define TSF_PULSE_IRQ			gpio_to_irq(TSF_PULSE_PIN)

int  whc_host_tsf_sync_register(void);

void whc_host_tsf_sync_unregister(void);

void whc_host_tsf_sync_push(const u8 *buf, u32 len);

#endif /* __WHC_HOST_TSF_SYNC_H__ */
