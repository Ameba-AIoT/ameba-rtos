/*
 *  GSPI Power-Save Device Example
 *
 *  Device side of the pair with raw_gspi_host_ps. The Host drives the test over
 *  the GSPI bus; this side provides what the wake-up flows need:
 *   - Phase 0 handshake, same as mbed_sdio_device_tp, so the Host's link-up code
 *     works unchanged.
 *   - rpwm_cb: RPWM2 CG suspends this board, RPWM2 ACT resumes it. The mbed
 *     driver only calls rpwm_cb for RPWM2; RPWM1 is logged by the driver itself.
 *   - An ASCII command channel ("PS:xxx" packets) so the Host can sequence each
 *     step instead of relying on both boards' timing.
 *
 *  The six SDIO pads are configured by spdio_init() from SDIO_Pin_Grp in
 *  ameba_intfcfg.c. D2 is unused by GSPI, so it is re-routed to GPIO as an
 *  out-of-band wake line, like whc_sdio_dev.c's WHC_SDIO_USE_GPIO_INT.
 */

#include "spdio_api.h"
#include "os_wrapper.h"
#include "ameba_soc.h"

static const char *const TAG = "GSPI_DEV_PS";

/* ---------- Tunable test parameters ---------- */
/* "PS:RDY0" restores CPU_RDY_IND itself: while not ready the Host cannot use the
 * data path, so a restoring command could not be delivered. */
#define PS_NOT_RDY_MS          3000
#define PS_WAKE_PULSE_MS       100      /* "PS:GPIO" low pulse width */
#define PS_LATE_DELAY_MS       5000     /* "PS:LATE" delay, must exceed the Host's sleep entry */
#define PS_WAKE_PIN            _PA_27   /* out-of-band wake line, SDIO D2 */
#define PS_ENABLE_SLEEP        1        /* 0 = never sleep, ignore RPWM2 CG */
/* Clock gate only. Power gate would cut the SDIO front end, so the Host's RPWM
 * write could not wake this board; UM0505 only promises the host-wake interrupt
 * survives a gated CPU clock. */
#define PS_SLEEP_TYPE          SLEEP_CG
#define PS_SYSACTIVE_MS        2000     /* awake time after a resume */

/* ---------- SPDIO sizing, same as mbed_sdio_device_tp ---------- */
#define EX_SPDIO_STACKSIZE     2048
#define EX_SPDIO_TX_BD_NUM     4
#define EX_SPDIO_RX_BD_NUM     4
#define EX_SPDIO_RX_BUFSZ      (SPDIO_RX_BUFSZ_ALIGN(2048 + 24))

#define HELLO_DEVICE_MSG       "Hello Device"
#define HELLO_HOST_RESP        "Hello Host! I am Here"
#define HELLO_DEVICE_LEN       (sizeof(HELLO_DEVICE_MSG) - 1)
#define HELLO_HOST_RESP_LEN    (sizeof(HELLO_HOST_RESP) - 1)

/* ---------- Host -> Device command channel ---------- */
#define PS_CMD_RDY0_STR        "PS:RDY0"     /* drop CPU_RDY_IND, auto-restore */
#define PS_CMD_CPWM_OFF_STR    "PS:CPWM0"    /* clear CPWM2 ACT bit */
#define PS_CMD_CPWM_ON_STR     "PS:CPWM1"    /* set CPWM2 ACT bit */
#define PS_CMD_CPWM1_TG_STR    "PS:CP1TG"    /* toggle CPWM1 -> HISR CPWM1_INT */
#define PS_CMD_TXDATA_STR      "PS:TXDAT"    /* queue one Device -> Host packet */
#define PS_CMD_GPIO_STR        "PS:GPIO"     /* pulse the wake GPIO low */
#define PS_CMD_LATE_STR        "PS:LATE"     /* queue a packet after a delay */
#define PS_CMD_LATE_CP1_STR    "PS:LTCP1"    /* toggle CPWM1 after a delay */
#define PS_CMD_LATE_CP2_STR    "PS:LTCP2"    /* toggle CPWM2 after a delay */

#define PS_DEV_DATA_MSG        "GSPI-PS device data"

enum ps_cmd_t {
	PS_CMD_NONE = 0,
	PS_CMD_RDY0,
	PS_CMD_CPWM_OFF,
	PS_CMD_CPWM_ON,
	PS_CMD_CPWM1_TG,
	PS_CMD_TXDATA,
	PS_CMD_GPIO,
	PS_CMD_LATE,
	PS_CMD_LATE_CP1,
	PS_CMD_LATE_CP2,
};

/* Re-asserting the CCPWM/CCPWM2 "PS" bit is what raises the interrupt at the
 * Host; SDIO_CPWM1_Set/SDIO_CPWM2_Set always set it. CPWM1 uses a scratch bit,
 * CPWM2 re-asserts ACT so the Host's power-state view stays correct. */
static void ps_cpwm1_toggle(void)
{
	SDIO_CPWM1_Set(SDIO_WIFI, 0x01, ENABLE);
	rtos_time_delay_ms(2);
	SDIO_CPWM1_Set(SDIO_WIFI, 0x01, DISABLE);
}

static void ps_cpwm2_toggle(void)
{
	SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, ENABLE);
}

static struct spdio_t spdio_dev;
static rtos_sema_t tx_sema;
static rtos_queue_t ps_cmd_queue;
static volatile u32 ps_handshake_done;

/* Send one buffer to the Host. Called from the SPDIO RX callback (a task, not
 * an ISR) and from the command task, so blocking on tx_sema is allowed. */
static char device_send_to_host(const u8 *pdata, u16 size)
{
	struct spdio_buf_t *tx_buf;

	if (rtos_sema_take(tx_sema, 5000) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "tx_sema timeout, TX BD exhausted?\n");
		return RTK_FAIL;
	}

	tx_buf = (struct spdio_buf_t *)rtos_mem_malloc(sizeof(struct spdio_buf_t));
	if (!tx_buf) {
		RTK_LOGE(TAG, "malloc tx_buf fail\n");
		rtos_sema_give(tx_sema);
		return RTK_FAIL;
	}
	_memset(tx_buf, 0, sizeof(struct spdio_buf_t));

	tx_buf->buf_allocated = (u32)rtos_mem_malloc(size + SPDIO_DMA_ALIGN_4);
	if (!tx_buf->buf_allocated) {
		RTK_LOGE(TAG, "malloc TX DMA buf fail (size=%u)\n", size);
		rtos_mem_free(tx_buf);
		rtos_sema_give(tx_sema);
		return RTK_FAIL;
	}
	tx_buf->size_allocated = size + SPDIO_DMA_ALIGN_4;
	tx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)tx_buf->buf_allocated, SPDIO_DMA_ALIGN_4);

	_memcpy((void *)tx_buf->buf_addr, pdata, size);
	tx_buf->buf_size = size;
	tx_buf->type = SPDIO_RX_DATA_USER;

	if (spdio_tx(&spdio_dev, tx_buf) == FALSE) {
		RTK_LOGE(TAG, "spdio_tx fail\n");
		rtos_mem_free((void *)tx_buf->buf_allocated);
		rtos_mem_free((void *)tx_buf);
		rtos_sema_give(tx_sema);
		return RTK_FAIL;
	}

	return RTK_SUCCESS;
}

/* Map an incoming payload to a command code, or PS_CMD_NONE. */
static u32 ps_parse_cmd(const u8 *pdata, u16 size)
{
	if (size < 7 || _memcmp(pdata, "PS:", 3) != 0) {
		return PS_CMD_NONE;
	}
	if (_memcmp(pdata, PS_CMD_RDY0_STR, sizeof(PS_CMD_RDY0_STR) - 1) == 0) {
		return PS_CMD_RDY0;
	}
	if (_memcmp(pdata, PS_CMD_CPWM_OFF_STR, sizeof(PS_CMD_CPWM_OFF_STR) - 1) == 0) {
		return PS_CMD_CPWM_OFF;
	}
	if (_memcmp(pdata, PS_CMD_CPWM_ON_STR, sizeof(PS_CMD_CPWM_ON_STR) - 1) == 0) {
		return PS_CMD_CPWM_ON;
	}
	if (_memcmp(pdata, PS_CMD_CPWM1_TG_STR, sizeof(PS_CMD_CPWM1_TG_STR) - 1) == 0) {
		return PS_CMD_CPWM1_TG;
	}
	if (_memcmp(pdata, PS_CMD_TXDATA_STR, sizeof(PS_CMD_TXDATA_STR) - 1) == 0) {
		return PS_CMD_TXDATA;
	}
	if (_memcmp(pdata, PS_CMD_GPIO_STR, sizeof(PS_CMD_GPIO_STR) - 1) == 0) {
		return PS_CMD_GPIO;
	}
	if (_memcmp(pdata, PS_CMD_LATE_CP1_STR, sizeof(PS_CMD_LATE_CP1_STR) - 1) == 0) {
		return PS_CMD_LATE_CP1;
	}
	if (_memcmp(pdata, PS_CMD_LATE_CP2_STR, sizeof(PS_CMD_LATE_CP2_STR) - 1) == 0) {
		return PS_CMD_LATE_CP2;
	}
	if (_memcmp(pdata, PS_CMD_LATE_STR, sizeof(PS_CMD_LATE_STR) - 1) == 0) {
		return PS_CMD_LATE;
	}
	return PS_CMD_NONE;
}

/* Command worker. Commands run here, not in the RX callback, because they
 * block (delays) and the RX callback must return promptly. */
static void ps_cmd_task(void *param)
{
	UNUSED(param);
	u32 cmd;

	for (;;) {
		if (rtos_queue_receive(ps_cmd_queue, &cmd, RTOS_MAX_TIMEOUT) != RTK_SUCCESS) {
			continue;
		}

		switch (cmd) {
		case PS_CMD_RDY0:
			SDIO_SetReady(SDIO_WIFI, DISABLE);
			rtos_time_delay_ms(PS_NOT_RDY_MS);
			SDIO_SetReady(SDIO_WIFI, ENABLE);
			RTK_LOGI(TAG, "RDY0: CPU_RDY_IND 0 for %d ms, restored\n", PS_NOT_RDY_MS);
			break;

		case PS_CMD_CPWM_OFF:
			SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, DISABLE);
			break;

		case PS_CMD_CPWM_ON:
			SDIO_CPWM2_Set(SDIO_WIFI, CPWM2_ACT_BIT, ENABLE);
			break;

		case PS_CMD_CPWM1_TG:
			/* The documented alternative to queueing data for interrupting the
			 * Host (Resume Flow method 2). */
			ps_cpwm1_toggle();
			break;

		case PS_CMD_TXDATA:
			if (device_send_to_host((const u8 *)PS_DEV_DATA_MSG,
									sizeof(PS_DEV_DATA_MSG) - 1) != RTK_SUCCESS) {
				RTK_LOGE(TAG, "TXDAT: queue failed\n");
			}
			break;

		/* The three "wake a sleeping Host" triggers. All wait first, so the Host
		 * is really asleep when the interrupt is raised. */
		case PS_CMD_LATE:
			rtos_time_delay_ms(PS_LATE_DELAY_MS);
			if (device_send_to_host((const u8 *)PS_DEV_DATA_MSG,
									sizeof(PS_DEV_DATA_MSG) - 1) != RTK_SUCCESS) {
				RTK_LOGE(TAG, "LATE: queue failed\n");
			}
			break;

		case PS_CMD_LATE_CP1:
			rtos_time_delay_ms(PS_LATE_DELAY_MS);
			ps_cpwm1_toggle();
			break;

		case PS_CMD_LATE_CP2:
			rtos_time_delay_ms(PS_LATE_DELAY_MS);
			ps_cpwm2_toggle();
			break;

		case PS_CMD_GPIO:
			GPIO_WriteBit(PS_WAKE_PIN, 0);
			rtos_time_delay_ms(PS_WAKE_PULSE_MS);
			GPIO_WriteBit(PS_WAKE_PIN, 1);
			break;

		default:
			break;
		}
	}
}

#if PS_ENABLE_SLEEP
/* CG keeps the SDIO front end powered, so nothing needs tearing down here. The
 * logs only prove when the board went down and what woke it. */
static u32 ps_peripheral_suspend(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	/* Keep short: the UART stops mid-line once the clocks gate. */
	RTK_LOGS(TAG, RTK_LOG_INFO, "suspend\n");
	return TRUE;
}

static u32 ps_peripheral_resume(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	/* WAK_STATUS1 bit1 = WAKE_SRC_SDIO_WIFI. */
	RTK_LOGS(TAG, RTK_LOG_INFO, "resume: wake status %08x %08x\n",
			 HAL_READ32(PMC_BASE, WAK_STATUS0), HAL_READ32(PMC_BASE, WAK_STATUS1));
	pmu_set_sysactive_time(PS_SYSACTIVE_MS);
	return TRUE;
}
#endif

/* The Host's suspend/resume doorbell, same handling as whc_sdio_dev.c. The mbed
 * driver only calls this for RPWM2 and does not null-check the pointer, so it
 * must be installed before the Host writes any RPWM register. */
static char ex_spdio_rpwm_cb(void *priv, u16 value)
{
	UNUSED(priv);

	if (value & RPWM2_ACT_BIT) {
#if PS_ENABLE_SLEEP
		pmu_acquire_wakelock(PMU_DEV_USER_BASE);
#endif
		SDIO_SetReady(SDIO_WIFI, ENABLE);
		RTK_LOGI(TAG, "rpwm_cb ACT: CPU_RDY_IND 1 (RPWM2=0x%08x)\n", value);
	}

	if (value & RPWM2_CG_BIT) {
#if PS_ENABLE_SLEEP
		SDIO_SetReady(SDIO_WIFI, DISABLE);
		RTK_LOGI(TAG, "rpwm_cb CG: CPU_RDY_IND 0, wakelock released\n");
		pmu_release_wakelock(PMU_DEV_USER_BASE);
#else
		RTK_LOGI(TAG, "rpwm_cb CG: ignored, sleep disabled\n");
#endif
	}

	return RTK_SUCCESS;
}

/* SPDIO RX done callback (Host -> Device). */
static char ex_spdio_rx_done_cb(void *priv, void *pbuf, u8 *pdata, u16 size, u8 type)
{
	struct spdio_t *obj = (struct spdio_t *)priv;
	struct spdio_buf_t *rx_buf = (struct spdio_buf_t *)pbuf;
	u32 cmd;
	u32 new_buf;

	UNUSED(type);

	if (size >= HELLO_DEVICE_LEN && _memcmp(pdata, HELLO_DEVICE_MSG, HELLO_DEVICE_LEN) == 0) {
		/* Always answer, so the Host can be restarted on its own. */
		RTK_LOGI(TAG, "handshake #%d, replying\n", ++ps_handshake_done);
		device_send_to_host((const u8 *)HELLO_HOST_RESP, HELLO_HOST_RESP_LEN);
		goto reassign_buf;
	}

	cmd = ps_parse_cmd(pdata, size);
	if (cmd != PS_CMD_NONE) {
		if (rtos_queue_send(ps_cmd_queue, &cmd, 0) != RTK_SUCCESS) {
			RTK_LOGE(TAG, "cmd queue full, dropped cmd %d\n", cmd);
		}
	} else {
		RTK_LOGW(TAG, "RX %d bytes, not a PS command\n", size);
	}

reassign_buf:
	/* Re-arm the RX buffer for the next DMA transfer. */
	new_buf = (u32)rtos_mem_malloc(obj->device_rx_bufsz + SPDIO_DMA_ALIGN_4);
	if (new_buf) {
		DCache_CleanInvalidate(new_buf, obj->device_rx_bufsz + SPDIO_DMA_ALIGN_4);
		rtos_mem_free((void *)rx_buf->buf_allocated);
		rx_buf->buf_allocated = new_buf;
		rx_buf->size_allocated = obj->device_rx_bufsz + SPDIO_DMA_ALIGN_4;
		rx_buf->buf_addr = (u32)N_BYTE_ALIGMENT((u32)rx_buf->buf_allocated, SPDIO_DMA_ALIGN_4);
	} else {
		RTK_LOGE(TAG, "RX buf realloc fail, reusing old buffer\n");
	}

	return RTK_SUCCESS;
}

/* Fires once the Host has read the data out, which is also what returns the TX
 * token. */
static char ex_spdio_tx_done_cb(void *priv, void *pbuf)
{
	UNUSED(priv);
	struct spdio_buf_t *tx_buf = (struct spdio_buf_t *)pbuf;

	rtos_mem_free((void *)tx_buf->buf_allocated);
	rtos_mem_free((void *)tx_buf);
	rtos_sema_give(tx_sema);
	return RTK_SUCCESS;
}

/* Wake line to the Host: idle high, pulsed low on request. Must run after
 * spdio_init(), which configures this pad as SDIO D2. */
static void ps_wake_pin_init(void)
{
	GPIO_InitTypeDef gpio;

	Pinmux_Config(PS_WAKE_PIN, PINMUX_FUNCTION_GPIO);
	gpio.GPIO_Pin = PS_WAKE_PIN;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&gpio);
	GPIO_WriteBit(PS_WAKE_PIN, 1);
}

static void ps_init_thread(void *param)
{
	UNUSED(param);
	u32 i;

	rtos_sema_create(&tx_sema, EX_SPDIO_TX_BD_NUM - 1, EX_SPDIO_TX_BD_NUM - 1);
	if (rtos_queue_create(&ps_cmd_queue, 8, sizeof(u32)) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "create cmd queue fail\n");
		rtos_task_delete(NULL);
		return;
	}

	spdio_dev.priv = NULL;
	spdio_dev.host_tx_bd_num = EX_SPDIO_RX_BD_NUM;
	spdio_dev.host_rx_bd_num = EX_SPDIO_TX_BD_NUM;
	spdio_dev.device_rx_bufsz = EX_SPDIO_RX_BUFSZ;
	spdio_dev.pSDIO = SDIO_WIFI;

	spdio_dev.rx_buf = (struct spdio_buf_t *)rtos_mem_malloc(spdio_dev.host_tx_bd_num * sizeof(struct spdio_buf_t));
	if (!spdio_dev.rx_buf) {
		RTK_LOGE(TAG, "malloc rx_buf array fail\n");
		rtos_task_delete(NULL);
		return;
	}
	_memset(spdio_dev.rx_buf, 0, spdio_dev.host_tx_bd_num * sizeof(struct spdio_buf_t));

	for (i = 0; i < spdio_dev.host_tx_bd_num; i++) {
		spdio_dev.rx_buf[i].buf_allocated = (u32)rtos_mem_malloc(spdio_dev.device_rx_bufsz + SPDIO_DMA_ALIGN_4);
		if (!spdio_dev.rx_buf[i].buf_allocated) {
			RTK_LOGE(TAG, "malloc rx_buf[%d] fail\n", i);
			while (i-- > 0) {
				rtos_mem_free((void *)spdio_dev.rx_buf[i].buf_allocated);
			}
			rtos_mem_free(spdio_dev.rx_buf);
			rtos_task_delete(NULL);
			return;
		}
		spdio_dev.rx_buf[i].size_allocated = spdio_dev.device_rx_bufsz + SPDIO_DMA_ALIGN_4;
		spdio_dev.rx_buf[i].buf_addr = (u32)N_BYTE_ALIGMENT((u32)spdio_dev.rx_buf[i].buf_allocated, SPDIO_DMA_ALIGN_4);
	}

	spdio_dev.device_rx_done_cb = ex_spdio_rx_done_cb;
	spdio_dev.device_tx_done_cb = ex_spdio_tx_done_cb;
	spdio_dev.rpwm_cb = ex_spdio_rpwm_cb;

	spdio_init(&spdio_dev);
	ps_wake_pin_init();

	if (rtos_task_create(NULL, ((const char *)"gspi_ps_cmd"), ps_cmd_task, NULL,
						 EX_SPDIO_STACKSIZE, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "create cmd task fail\n");
	}

#if PS_ENABLE_SLEEP
	/* SDIO is not a wake source by default: ameba_sleepcfg.c only enables it for
	 * CONFIG_WHC_INTF_SDIO builds. The AP core is where this example is linked.
	 * Also override the default sleep type, which is PG for non-WHC builds. */
	SOCPS_SetAPWakeEvent(WAKE_SRC_SDIO_WIFI, ENABLE);
	pmu_register_sleep_callback(PMU_DEV_USER_BASE, (PSM_HOOK_FUN)ps_peripheral_suspend, NULL,
								(PSM_HOOK_FUN)ps_peripheral_resume, NULL);
	pmu_set_sleep_type(PS_SLEEP_TYPE);
	/* Hold our own lock so the board stays up until the Host asks for CG, then
	 * release the OS lock that otherwise blocks tickless entirely. */
	pmu_acquire_wakelock(PMU_DEV_USER_BASE);
	pmu_release_wakelock(PMU_OS);
#endif

	RTK_LOGI(TAG, "ready: sleep=%d type=%s wake_pin=PA%d lockbit=0x%08x\n",
			 PS_ENABLE_SLEEP, (PS_SLEEP_TYPE == SLEEP_CG) ? "CG" : "PG",
			 PIN_NUM(PS_WAKE_PIN), pmu_get_wakelock_status());

	rtos_task_delete(NULL);
}

int example_mbed_gspi_device_ps(void)
{
	if (rtos_task_create(NULL, ((const char *)"gspi_dev_ps"), ps_init_thread, NULL,
						 EX_SPDIO_STACKSIZE * 4, 5) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "create init task fail\n");
	}

	return 0;
}
