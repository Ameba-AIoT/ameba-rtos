#include "tsf_sync_dev.h"

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "whc_dev_app.h"
#include "whc_dev_api.h"

/* Calibration pulse output, wired to host GPIO18 */
#define TSF_SYNC_PULSE_PIN          _PA_0

/* port0 TSF register offset (amebagreen2: REG_P0_TSFTR_L, high 32 bits at +4) */
#define TSF_SYNC_REG_P0_TSFTR_L     0x560

#define TSF_PERIOD_MS               500
#define TSF_SYNC_PULSE_WIDTH_US     50
#define TSF_SYNC_TASK_STACK         512
#define TSF_SYNC_TASK_PRIO          3

static volatile u8  g_tsf_sync_enabled = 0;

static rtos_sema_t  g_ack_sema = NULL;
static rtos_task_t  g_tsf_task = NULL;

static volatile u8  g_ack_valid = 0;
static volatile u32 g_ack_seq   = 0;

/* low may wrap past 2^32 between the two reads; retry until high is stable */
static inline u64 tsf_read_port0_atomic(void)
{
	u32 hi1, lo, hi2;
	do {
		hi1 = HAL_READ32(WIFI_REG_BASE, TSF_SYNC_REG_P0_TSFTR_L + 4);
		lo  = HAL_READ32(WIFI_REG_BASE, TSF_SYNC_REG_P0_TSFTR_L);
		hi2 = HAL_READ32(WIFI_REG_BASE, TSF_SYNC_REG_P0_TSFTR_L + 4);
	} while (hi1 != hi2);
	return ((u64)hi2 << 32) | lo;
}

/* Reading wifi registers while wifi is in IPS (powered down) hangs the chip. */
static inline int tsf_pon_ready(void)
{
	return (HAL_READ32(WIFI_REG_BASE, 0xA4) & 0x7F00) == (1u << 13);
}

/* Edge and TSF read glued together with interrupts off so preemption cannot add
 * phase error. Caller must check tsf_pon_ready(). */
static u64 tsf_sync_emit_and_read(void)
{
	u64 tsf;

	rtos_critical_enter(RTOS_CRITICAL_DEFAULT);
	GPIO_WriteBit(TSF_SYNC_PULSE_PIN, 1);
	tsf = tsf_read_port0_atomic();
	rtos_critical_exit(RTOS_CRITICAL_DEFAULT);

	DelayUs(TSF_SYNC_PULSE_WIDTH_US);
	GPIO_WriteBit(TSF_SYNC_PULSE_PIN, 0);
	return tsf;
}

static void tsf_sync_send_sample(u32 seq, u64 tsf)
{
	u8 buf[TSF_HDR_LEN + sizeof(struct tsf_sync_sample)];
	struct tsf_sync_sample s;

	s.seq = seq;
	s.tsf = tsf;
	*(u32 *)buf = WHC_WIFI_TEST;
	buf[sizeof(u32)] = WHC_WIFI_TEST_TSF_SAMPLE;
	/* memcpy: the payload sits at a 5-byte offset, so the u64 is unaligned */
	memcpy(buf + TSF_HDR_LEN, &s, sizeof(s));
	whc_dev_api_send_to_host(buf, sizeof(buf));
}

static void tsf_sync_task(void *param)
{
	u32 seq     = 1;
	u64 cur_tsf = 0;
	u8  emitted = 0;   /* this seq already produced a pulse + sample */
	(void)param;

	while (g_tsf_sync_enabled) {
		if (!emitted) {
			if (!tsf_pon_ready()) {
				rtos_time_delay_ms(TSF_PERIOD_MS);
				continue;
			}
			cur_tsf = tsf_sync_emit_and_read();
			emitted = 1;
		}

		g_ack_valid = 0;
		rtos_sema_take(g_ack_sema, 0);   /* drain a give left over from last round */
		tsf_sync_send_sample(seq, cur_tsf);
		rtos_sema_take(g_ack_sema, TSF_PERIOD_MS);

		if (g_ack_valid && g_ack_seq == seq) {
			g_ack_valid = 0;
			seq++;
			emitted = 0;
			rtos_time_delay_ms(TSF_PERIOD_MS);
		}
		/* else: timeout / wrong seq -> resend the same (seq,tsf), no new pulse */
	}

	g_tsf_task = NULL;
	rtos_task_delete(NULL);
}

static void tsf_sync_gpio_init(void)
{
	GPIO_InitTypeDef gpio;

	Pinmux_Config(TSF_SYNC_PULSE_PIN, PINMUX_FUNCTION_GPIO);

	gpio.GPIO_Pin  = TSF_SYNC_PULSE_PIN;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(&gpio);

	GPIO_WriteBit(TSF_SYNC_PULSE_PIN, 0);
}

/* Overrides the __weak stub in whc_dev_app.c. Holds the only cmd path rx slot
 * while it runs, so just latch and wake. */
void whc_dev_tsf_ack_hdl(u8 *buf, u32 len)
{
	struct tsf_ack ack;

	if (len < sizeof(ack)) {
		return;
	}
	if (!g_tsf_sync_enabled || g_ack_sema == NULL) {
		return;
	}
	memcpy(&ack, buf, sizeof(ack));
	g_ack_seq   = ack.seq;
	g_ack_valid = 1;
	rtos_sema_give(g_ack_sema);
}

void tsf_sync_dev_start(void)
{
	if (g_tsf_sync_enabled) {
		return;
	}

	if (g_ack_sema == NULL) {
		rtos_sema_create(&g_ack_sema, 0, 1);
	}
	g_ack_valid = 0;

	tsf_sync_gpio_init();

	g_tsf_sync_enabled = 1;

	if (g_tsf_task == NULL) {
		rtos_task_create(&g_tsf_task, "tsf_sync", tsf_sync_task,
						 NULL, TSF_SYNC_TASK_STACK, TSF_SYNC_TASK_PRIO);
	}
}

void tsf_sync_dev_stop(void)
{
	if (!g_tsf_sync_enabled) {
		return;
	}

	g_tsf_sync_enabled = 0;

	GPIO_WriteBit(TSF_SYNC_PULSE_PIN, 0);

	/* wake the task if it is blocked in sema_take so it exits promptly */
	if (g_ack_sema) {
		rtos_sema_give(g_ack_sema);
	}
}
