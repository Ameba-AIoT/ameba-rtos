/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2014 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */
#include "ameba_soc.h"
#include "ameba_gspi.h"
#include "example_gspi_ps_ext.h"
#include "os_wrapper.h"

#define TAG "GSPI_HOST_PS"

/* ---------- Tunable test parameters ---------- */
#define GSPI_BUS_FREQ_HZ        10000000   /* bus clock after activation, max 25MHz */
#define GSPI_FREQ_ACTIVATE      1000000
#define PS_RUN_PROBE            1          /* probe every PM register and pin once */
#define PS_RUN_SUSPEND_RESUME   1          /* phase A: Host suspends/resumes Device */
#define PS_CYCLES               3
#define PS_SLEEP_MS             3000       /* how long the Device is left asleep */
#define PS_RPWM_TIMEOUT_MS      1000       /* CPU_RDY_IND handshake timeout */
#define PS_RUN_HOST_SLEEP       1          /* phase B: Device wakes a sleeping Host */
#define PS_HOST_SLEEP_CYCLES    2
#define PS_HOST_WAKE_TIMEOUT_MS 10000      /* must exceed Device PS_LATE_DELAY_MS */
#define PS_HOST_SYSACTIVE_MS    2000
#define PS_RUN_LONGRUN          1          /* phase C: alternate A and B; needs both */
#define PS_LONGRUN_ROUNDS       10         /* 0 = forever */
#define PS_WATCH_MS             500        /* probe observation window */
#define PS_WATCH_STEP_MS        10
#define PS_RDY_WATCH_MS         6000

/* ---------- Fixed sizing ---------- */
#define PS_MAX_PAYLOAD          64
#define TP_PKT_TYPE_USER        0x41       /* enum spdio_rx_data_t USER */

#define HELLO_DEVICE_MSG        "Hello Device"
#define HELLO_HOST_RESP         "Hello Host! I am Here"

/* Commands understood by mbed_gspi_device_ps. */
#define PS_CMD_RDY0             "PS:RDY0"
#define PS_CMD_CPWM_OFF         "PS:CPWM0"
#define PS_CMD_CPWM_ON          "PS:CPWM1"
#define PS_CMD_CPWM1_TG         "PS:CP1TG"
#define PS_CMD_TXDATA           "PS:TXDAT"
#define PS_CMD_GPIO             "PS:GPIO"
#define PS_CMD_LATE             "PS:LATE"
#define PS_CMD_LATE_CP1         "PS:LTCP1"
#define PS_CMD_LATE_CP2         "PS:LTCP2"

/* 16-byte TX descriptor, matching ameba_inic.h's INIC_TX_DESC. */
typedef struct {
	u16 pktsize;    /* payload bytes only, excluding this header */
	u8  offset;     /* = sizeof(this struct) */
	u8  byte3;      /* bus_agg_num */
	u8  type;
	u8  byte5;
	u16 word3;
	u32 start_addr;
	u16 data_len;
	u16 rsv_last;
} __attribute__((packed)) gspi_desc_t;

static GSPI_HandleTypeDef gspi;

/* Frame buffers: [CMD 4B][data, 4-byte aligned][Status 8B]. */
static u8 gspi_txbuf[GSPI_FIFO_BUF_SIZE(GSPI_TX_DESC_SIZE + PS_MAX_PAYLOAD)] ALIGNMTO(32);
static u8 gspi_rxbuf[GSPI_FIFO_BUF_SIZE(PS_MAX_PAYLOAD)] ALIGNMTO(32);

/* Send one packet to the Device: descriptor at buf[4], payload after it. */
static int ps_send_cmd(const char *cmd)
{
	gspi_desc_t *desc = (gspi_desc_t *)(gspi_txbuf + GSPI_CMD_LEN);
	u16 len = (u16)_strlen((char *)cmd);
	int ret;

	_memset(desc, 0, sizeof(gspi_desc_t));
	desc->pktsize = len;
	desc->offset = GSPI_TX_DESC_SIZE;
	desc->byte3 = 1;
	desc->type = TP_PKT_TYPE_USER;
	desc->data_len = len;
	_memcpy(gspi_txbuf + GSPI_CMD_LEN + GSPI_TX_DESC_SIZE, cmd, len);

	ret = GSPI_WriteTxFifo(&gspi, gspi_txbuf, GSPI_TX_DESC_SIZE + len, NULL);
	if (ret != GSPI_OK) {
		RTK_LOGE(TAG, "send \"%s\" failed (%d)\n", cmd, ret);
	}
	return ret;
}

/* Wait up to timeout_ms for the Device to have data pending. */
static u32 ps_wait_rx(u32 timeout_ms)
{
	u32 elapsed = 0;
	u32 pending;

	for (;;) {
		pending = GSPI_GetRxLen(&gspi, NULL);
		if (pending) {
			return pending;
		}
		if (elapsed >= timeout_ms) {
			return 0;
		}
		rtos_time_delay_ms(PS_WATCH_STEP_MS);
		elapsed += PS_WATCH_STEP_MS;
	}
}

/* Read out everything the Device has queued, so SPI_INT returns high. */
static void ps_drain_rx(void)
{
	u32 pending = GSPI_GetRxLen(&gspi, NULL);

	while (pending && (pending <= PS_MAX_PAYLOAD)) {
		if (GSPI_ReadRxFifo(&gspi, gspi_rxbuf, pending, NULL) != GSPI_OK) {
			break;
		}
		pending = GSPI_GetRxLen(&gspi, NULL);
	}
}

/* SPI_INT and the D2 wake line are plain inputs until phase B re-arms SPI_INT
 * as an edge interrupt. */
static void ps_pin_input(u32 pin)
{
	GPIO_InitTypeDef gpio;

	Pinmux_Config(pin, PINMUX_FUNCTION_GPIO);
	gpio.GPIO_Pin = pin;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Mode = GPIO_Mode_IN;
	GPIO_Init(&gpio);
}

/* Activate GSPI mode, unmask the interrupts the test needs and complete the
 * Device's Phase 0 handshake. Returns 0 on success. */
static int gspi_host_link_up(void)
{
	GSPI_InitTypeDef init;
	u32 pending;
	u8 cpu_ind;

	GSPI_StructInit(&init);
	init.GSPI_Index = 0;
	init.GSPI_ClkPin = GSPI_SCLK;
	init.GSPI_MosiPin = GSPI_MOSI;
	init.GSPI_MisoPin = GSPI_MISO;
	init.GSPI_CsPin = GSPI_CS;
	init.GSPI_ClkFreq = GSPI_BUS_FREQ_HZ;
	init.GSPI_ClkFreqInit = GSPI_FREQ_ACTIVATE;
	if (GSPI_Init(&gspi, &init) != GSPI_OK) {
		RTK_LOGE(TAG, "GSPI_Init failed\n");
		return -1;
	}

	/* CS is already latched high by GSPI_Init; let the Device bring up its
	 * SPDIO backend before the first bus transaction. */
	rtos_time_delay_ms(3000);

	ps_pin_input(GSPI_INT);
	ps_pin_input(GSPI_WAKE);

	if (GSPI_Configuration(&gspi, GSPI_BIG_ENDIAN_32) != GSPI_OK) {
		return -1;
	}
	cpu_ind = GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL);

	/* CPWM1/CPWM2 are unmasked because SPI_INT only follows masked HISR bits,
	 * and phase B uses their toggles as wake triggers. */
	GSPI_INTClear(&gspi, 0xFFFFFFFF);
	GSPI_WriteReg32(&gspi, GSPI_REG_RX_AGG, 0, NULL);
	GSPI_INTConfig(&gspi, GSPI_BIT_RX_REQUEST | GSPI_BIT_AVAL_INT |
				   GSPI_BIT_CPWM1_INT | GSPI_BIT_CPWM2_INT, ENABLE);

	if (ps_send_cmd(HELLO_DEVICE_MSG) != GSPI_OK) {
		RTK_LOGE(TAG, "handshake failed: TX write\n");
		return -1;
	}
	pending = ps_wait_rx(3000);
	if ((pending == 0) || (GSPI_ReadRxFifo(&gspi, gspi_rxbuf, pending, NULL) != GSPI_OK)) {
		RTK_LOGE(TAG, "handshake failed: no reply, HISR=0x%08x\n", GSPI_INTStatus(&gspi, NULL));
		return -1;
	}
	if (_memcmp(&gspi_rxbuf[GSPI_CMD_LEN], HELLO_HOST_RESP, sizeof(HELLO_HOST_RESP) - 1) != 0) {
		RTK_LOGE(TAG, "handshake failed: unexpected reply (%d bytes)\n", (int)pending);
		return -1;
	}

	RTK_LOGI(TAG, "link up: CPU_RDY=%d HIMR=0x%08x, handshake OK\n",
			 cpu_ind & GSPI_BIT_CPU_RDY, GSPI_GetINTMask(&gspi));
	return 0;
}

/* Ask the Device for one packet and wait for it: re-checks the Device -> Host
 * direction after a power-management operation. Returns 1 if it arrived. */
static int ps_check_dev_to_host(const char *stage)
{
	u32 pending;

	if (ps_send_cmd(PS_CMD_TXDATA) != GSPI_OK) {
		return 0;
	}

	pending = ps_wait_rx(1000);
	if (pending == 0) {
		RTK_LOGE(TAG, "  D2H(%s): timeout, HISR=0x%08x\n", stage, GSPI_INTStatus(&gspi, NULL));
		return 0;
	}
	GSPI_ReadRxFifo(&gspi, gspi_rxbuf, pending, NULL);
	RTK_LOGI(TAG, "  D2H(%s): OK, %d bytes\n", stage, (int)pending);
	return 1;
}

#if PS_RUN_PROBE
/* Watch both interrupt-capable pins. bit0 = INT went low, bit1 = WAKE went low. */
static u32 ps_watch_pins(u32 watch_ms)
{
	u32 elapsed = 0;
	u32 seen = 0;

	while (elapsed < watch_ms) {
		if (GPIO_ReadDataBit(GSPI_INT) == 0) {
			seen |= 1u;
		}
		if (GPIO_ReadDataBit(GSPI_WAKE) == 0) {
			seen |= 2u;
		}
		rtos_time_delay_ms(PS_WATCH_STEP_MS);
		elapsed += PS_WATCH_STEP_MS;
	}
	return seen;
}

/* P1: HRPWM doorbell, confirmed on the Device side ("RPWM INT triggered").
 * The TOGGLING bit does not read back, only its edge matters. */
static void ps_probe_hrpwm(void)
{
	u8 next = (u8)(GSPI_ReadReg8(&gspi, GSPI_REG_HRPWM, NULL) ^ GSPI_BIT_HRPWM_TOGGLING);

	GSPI_WriteReg8(&gspi, GSPI_REG_HRPWM, next, NULL);
	rtos_time_delay_ms(PS_WATCH_MS);
	RTK_LOGI(TAG, "P1 HRPWM: wrote 0x%02x, readback 0x%02x (Device logs RPWM INT)\n",
			 next, GSPI_ReadReg8(&gspi, GSPI_REG_HRPWM, NULL));
}

/* P2: does HRPWM2 exist in the SPI front end? Only RPWM2 reaches the Device's
 * rpwm_cb, so this decides whether the SDK's ACT/CG protocol can be reused. */
static void ps_probe_hrpwm2(void)
{
	u16 val = (u16)(GSPI_BIT_HRPWM2_TOGGLE | GSPI_BIT_HRPWM2_ACT);

	GSPI_WriteReg16(&gspi, GSPI_REG_HRPWM2, val, NULL);
	rtos_time_delay_ms(PS_WATCH_MS);
	/* The TOGGLE bit self-clears, so the readback shows ACT only. */
	RTK_LOGI(TAG, "P2 HRPWM2: wrote 0x%04x, readback 0x%04x (Device logs rpwm_cb)\n",
			 val, GSPI_ReadReg16(&gspi, GSPI_REG_HRPWM2, NULL));
}

/* P3: CPU_IND bit0 follows the Device's SDIO_SetReady(). */
static void ps_probe_cpu_ind(void)
{
	u32 elapsed = 0;
	u32 t_low = 0;
	u8 last = GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL) & GSPI_BIT_CPU_RDY;
	u8 cur;
	u32 transitions = 0;

	if (ps_send_cmd(PS_CMD_RDY0) != GSPI_OK) {
		return;
	}

	while ((elapsed < PS_RDY_WATCH_MS) && (transitions < 2)) {
		cur = GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL) & GSPI_BIT_CPU_RDY;
		if (cur != last) {
			last = cur;
			transitions++;
			if (transitions == 1) {
				t_low = elapsed;
			}
		}
		rtos_time_delay_ms(PS_WATCH_STEP_MS);
		elapsed += PS_WATCH_STEP_MS;
	}

	RTK_LOGI(TAG, "P3 CPU_RDY_IND: cleared @%dms, restored @%dms (%d transitions)\n",
			 t_low, elapsed, transitions);
}

/* P4: HCPWM2 mirrors the Device's CCPWM2, so the Host can read its power state. */
static void ps_probe_hcpwm(void)
{
	u16 v0 = GSPI_ReadReg16(&gspi, GSPI_REG_HCPWM2, NULL);
	u16 v1, v2;

	ps_send_cmd(PS_CMD_CPWM_OFF);
	rtos_time_delay_ms(PS_WATCH_MS);
	v1 = GSPI_ReadReg16(&gspi, GSPI_REG_HCPWM2, NULL);

	ps_send_cmd(PS_CMD_CPWM_ON);
	rtos_time_delay_ms(PS_WATCH_MS);
	v2 = GSPI_ReadReg16(&gspi, GSPI_REG_HCPWM2, NULL);

	RTK_LOGI(TAG, "P4 HCPWM2(ACT bit0): 0x%04x -> CPWM0 0x%04x -> CPWM1 0x%04x\n", v0, v1, v2);
}

/* P5: which trigger drives the interrupt line? Queued data (method 1) and a
 * CPWM1 toggle (method 2), sampled without draining the FIFO. */
static void ps_probe_int_pin(void)
{
	u32 seen;

	/* Clear the latched CPWM bits first, otherwise the pin is already low from
	 * the CPWM changes made by P4. */
	GSPI_INTClear(&gspi, GSPI_BIT_CPWM1_INT | GSPI_BIT_CPWM2_INT);

	ps_send_cmd(PS_CMD_TXDATA);
	seen = ps_watch_pins(PS_WATCH_MS);
	RTK_LOGI(TAG, "P5a queued data: INT low=%d WAKE low=%d, HISR=0x%08x pending=%d\n",
			 (seen & 1u) ? 1 : 0, (seen & 2u) ? 1 : 0,
			 GSPI_INTStatus(&gspi, NULL), (int)GSPI_GetRxLen(&gspi, NULL));
	ps_drain_rx();
	GSPI_INTClear(&gspi, GSPI_BIT_CPWM1_INT | GSPI_BIT_CPWM2_INT);

	ps_send_cmd(PS_CMD_CPWM1_TG);
	seen = ps_watch_pins(PS_WATCH_MS);
	RTK_LOGI(TAG, "P5b CPWM1 toggle: INT low=%d, CPWM1_INT in HISR=%d\n",
			 (seen & 1u) ? 1 : 0,
			 (GSPI_INTStatus(&gspi, NULL) & GSPI_BIT_CPWM1_INT) ? 1 : 0);
	GSPI_INTClear(&gspi, GSPI_BIT_CPWM1_INT | GSPI_BIT_CPWM2_INT);
}

/* P5': out-of-band wake line on D2, the fallback if SPI_INT cannot be used. */
static void ps_probe_wake_pin(void)
{
	u32 seen;

	ps_send_cmd(PS_CMD_GPIO);
	seen = ps_watch_pins(PS_WATCH_MS);
	RTK_LOGI(TAG, "P5' D2 pulse: WAKE low=%d INT low=%d\n",
			 (seen & 2u) ? 1 : 0, (seen & 1u) ? 1 : 0);
}

static void ps_run_probes(void)
{
	RTK_LOGI(TAG, "==== probe start ====\n");
	/* Re-check Device -> Host after each operation, so a regression can be
	 * attributed to a specific one. */
	ps_check_dev_to_host("handshake");
	ps_probe_hrpwm();
	ps_check_dev_to_host("HRPWM");
	ps_probe_hrpwm2();
	ps_check_dev_to_host("HRPWM2 ACT");
	ps_probe_cpu_ind();
	ps_check_dev_to_host("RDY0");
	ps_probe_hcpwm();
	ps_check_dev_to_host("CPWM");
	ps_probe_int_pin();
	ps_probe_wake_pin();
	ps_drain_rx();
	RTK_LOGI(TAG, "==== probe done ====\n");
}
#endif /* PS_RUN_PROBE */

#if PS_RUN_SUSPEND_RESUME
/* Suspend or resume the Device the way whc_sdio_host_rpwm_notify() does over
 * SDIO: write HRPWM2 TOGGLE|CG or TOGGLE|ACT, then wait for CPU_RDY_IND.
 * Returns the handshake time in ms, or -1 on timeout. */
static int ps_rpwm_notify(u8 suspend)
{
	u16 val = (u16)(GSPI_BIT_HRPWM2_TOGGLE |
					(suspend ? GSPI_BIT_HRPWM2_CG : GSPI_BIT_HRPWM2_ACT));
	u8 target = suspend ? 0 : GSPI_BIT_CPU_RDY;
	u32 elapsed = 0;

	GSPI_WriteReg16(&gspi, GSPI_REG_HRPWM2, val, NULL);

	while (elapsed < PS_RPWM_TIMEOUT_MS) {
		if ((GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL) & GSPI_BIT_CPU_RDY) == target) {
			return (int)elapsed;
		}
		rtos_time_delay_ms(1);
		elapsed++;
	}
	return -1;
}

/* One suspend / sleep / resume / verify cycle. Returns 0 on success. */
static int ps_suspend_resume_once(u32 cycle, u8 verbose)
{
	int ack_cg, ack_act;
	u8 ind_asleep;
	u16 hcpwm2_asleep;

	ack_cg = ps_rpwm_notify(1);
	if (ack_cg < 0) {
		RTK_LOGE(TAG, "cycle %d: suspend not acked, CPU_IND=0x%02x\n",
				 cycle, GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL));
		return -1;
	}

	/* Registers stay reachable while the Device sleeps: the GSPI front end is in
	 * the always-on card domain. */
	rtos_time_delay_ms(PS_SLEEP_MS / 2);
	ind_asleep = GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL);
	hcpwm2_asleep = GSPI_ReadReg16(&gspi, GSPI_REG_HCPWM2, NULL);
	rtos_time_delay_ms(PS_SLEEP_MS / 2);

	ack_act = ps_rpwm_notify(0);
	if (ack_act < 0) {
		RTK_LOGE(TAG, "cycle %d: resume not acked, CPU_IND=0x%02x\n",
				 cycle, GSPI_ReadReg8(&gspi, GSPI_REG_CPU_IND, NULL));
		return -1;
	}

	if (verbose) {
		RTK_LOGI(TAG, "cycle %d: CG acked %dms, asleep CPU_IND=0x%02x HCPWM2=0x%04x, ACT acked %dms\n",
				 cycle, ack_cg, ind_asleep, hcpwm2_asleep, ack_act);
	}

	return ps_check_dev_to_host("resume") ? 0 : -1;
}

/* Phase A: Host suspends and resumes the Device. */
static void ps_run_suspend_resume(void)
{
	u32 cycle;
	u32 fails = 0;

	RTK_LOGI(TAG, "==== phase A: Host suspends Device x%d ====\n", PS_CYCLES);
	for (cycle = 1; cycle <= PS_CYCLES; cycle++) {
		if (ps_suspend_resume_once(cycle, 1) != 0) {
			fails++;
		}
	}
	RTK_LOGI(TAG, "==== phase A done: %d/%d failed ====\n", fails, PS_CYCLES);
}
#endif /* PS_RUN_SUSPEND_RESUME */

#if PS_RUN_HOST_SLEEP
static rtos_sema_t ps_int_sema;
static volatile u32 ps_int_hits;

/* SPI_INT went low. Take the wakelock back first so the system cannot fall
 * asleep again mid-handling. */
static void ps_int_irq_handler(void *data, u32 id)
{
	UNUSED(data);
	UNUSED(id);

	/* The SDK's GPIO_INTHandler already cleared the edge status. */
	GPIO_INTConfig(GSPI_INT, DISABLE);
	pmu_acquire_wakelock(PMU_DEV_USER_BASE);
	ps_int_hits++;
	rtos_sema_give(ps_int_sema);
	GPIO_INTConfig(GSPI_INT, ENABLE);
}

static u32 ps_host_suspend(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	RTK_LOGS(TAG, RTK_LOG_INFO, "host suspend\n");
	return TRUE;
}

static u32 ps_host_resume(u32 expected_idle_time, void *param)
{
	UNUSED(expected_idle_time);
	UNUSED(param);

	/* WAK_STATUS0 bit17 = WAKE_SRC_GPIOA. */
	RTK_LOGS(TAG, RTK_LOG_INFO, "host resume: wake status %08x %08x\n",
			 HAL_READ32(PMC_BASE, WAK_STATUS0), HAL_READ32(PMC_BASE, WAK_STATUS1));
	pmu_set_sysactive_time(PS_HOST_SYSACTIVE_MS);
	return TRUE;
}

static void ps_host_sleep_init(void)
{
	GPIO_InitTypeDef gpio;
	static u8 done;

	if (done) {
		return;
	}
	done = 1;

	rtos_sema_create(&ps_int_sema, 0, 0xFFFFFFFF);

	/* Re-arm the INT pin, set up as a plain input by the link-up code, as an
	 * edge interrupt. GPIOA is already WAKEUP_AP in ameba_sleepcfg.c; enable it
	 * here too so the example does not depend on that file. */
	gpio.GPIO_Pin = GSPI_INT;
	gpio.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio.GPIO_Mode = GPIO_Mode_INT;
	gpio.GPIO_ITTrigger = GPIO_INT_Trigger_EDGE;
	gpio.GPIO_ITPolarity = GPIO_INT_POLARITY_ACTIVE_LOW;   /* falling edge */
	gpio.GPIO_ITDebounce = GPIO_INT_DEBOUNCE_DISABLE;
	GPIO_Init(&gpio);
	GPIO_UserRegIrq(GSPI_INT, (GPIO_IRQ_FUN)ps_int_irq_handler, NULL);
	GPIO_INTConfig(GSPI_INT, ENABLE);
	SOCPS_SetAPWakeEvent(WAKE_SRC_GPIOA, ENABLE);

	pmu_register_sleep_callback(PMU_DEV_USER_BASE, (PSM_HOOK_FUN)ps_host_suspend, NULL,
								(PSM_HOOK_FUN)ps_host_resume, NULL);
	pmu_set_sleep_type(SLEEP_CG);
	/* Take our own lock first, then release the OS lock that blocks tickless. */
	pmu_acquire_wakelock(PMU_DEV_USER_BASE);
	pmu_release_wakelock(PMU_OS);
}

/* The ways the Device can raise an interrupt at the Host: queue a packet
 * (RX_REQUEST) or toggle CPWM1 / CPWM2. All three end up on SPI_INT. */
enum ps_wake_trigger {
	PS_TRIG_DATA = 0,
	PS_TRIG_CPWM1,
	PS_TRIG_CPWM2,
	PS_TRIG_NUM,
};

static const char *const ps_trig_cmd[PS_TRIG_NUM] = {
	PS_CMD_LATE, PS_CMD_LATE_CP1, PS_CMD_LATE_CP2
};
static const char *const ps_trig_name[PS_TRIG_NUM] = { "data", "cpwm1", "cpwm2" };
static const u32 ps_trig_hisr[PS_TRIG_NUM] = {
	GSPI_BIT_RX_REQUEST, GSPI_BIT_CPWM1_INT, GSPI_BIT_CPWM2_INT
};

/* One "Device wakes Host" cycle: arm the Device with the given trigger,
 * clock-gate, wait for the SPI_INT edge, then confirm the cause. */
static int ps_host_sleep_once(u32 cycle, u8 trig, u8 verbose)
{
	u32 hisr;
	u32 pending = 0;

	/* Start from a quiet interrupt line: any pending cause holds SPI_INT low so
	 * no new falling edge could arrive, and a stale edge would end the wait
	 * without the Host ever sleeping. */
	ps_drain_rx();
	GSPI_INTClear(&gspi, GSPI_BIT_CPWM1_INT | GSPI_BIT_CPWM2_INT);
	while (rtos_sema_take(ps_int_sema, 0) == RTK_SUCCESS) {
	}
	if (GPIO_ReadDataBit(GSPI_INT) == 0) {
		RTK_LOGW(TAG, "cycle %d: INT still low before sleep, HISR=0x%08x\n",
				 cycle, GSPI_INTStatus(&gspi, NULL));
	}

	if (ps_send_cmd(ps_trig_cmd[trig]) != GSPI_OK) {
		return -1;
	}
	pmu_release_wakelock(PMU_DEV_USER_BASE);

	if (rtos_sema_take(ps_int_sema, PS_HOST_WAKE_TIMEOUT_MS) != RTK_SUCCESS) {
		pmu_acquire_wakelock(PMU_DEV_USER_BASE);
		RTK_LOGE(TAG, "cycle %d(%s): not woken in %dms, INT=%d HISR=0x%08x\n",
				 cycle, ps_trig_name[trig], PS_HOST_WAKE_TIMEOUT_MS,
				 GPIO_ReadDataBit(GSPI_INT), GSPI_INTStatus(&gspi, NULL));
		ps_drain_rx();
		return -1;
	}

	hisr = GSPI_INTStatus(&gspi, NULL);
	if (trig == PS_TRIG_DATA) {
		pending = ps_wait_rx(100);
		if (pending == 0) {
			RTK_LOGE(TAG, "cycle %d(data): woken but nothing pending, HISR=0x%08x\n", cycle, hisr);
			return -1;
		}
		GSPI_ReadRxFifo(&gspi, gspi_rxbuf, pending, NULL);
	} else if ((hisr & ps_trig_hisr[trig]) == 0) {
		RTK_LOGE(TAG, "cycle %d(%s): woken but HISR=0x%08x lacks the cause\n",
				 cycle, ps_trig_name[trig], hisr);
		return -1;
	}
	GSPI_INTClear(&gspi, GSPI_BIT_CPWM1_INT | GSPI_BIT_CPWM2_INT);

	if (verbose) {
		RTK_LOGI(TAG, "cycle %d(%s): woken by SPI_INT, HISR=0x%08x, %d bytes read, INT now %d\n",
				 cycle, ps_trig_name[trig], hisr, (int)pending, GPIO_ReadDataBit(GSPI_INT));
	}
	return 0;
}

/* Phase B: each trigger drives SPI_INT low, which is a GPIOA interrupt and
 * therefore a wake source for the clock-gated Host. */
static void ps_run_host_sleep(void)
{
	u32 cycle;
	u8 trig;
	u32 fails = 0;

	RTK_LOGI(TAG, "==== phase B: Device wakes Host, %d triggers x%d ====\n",
			 PS_TRIG_NUM, PS_HOST_SLEEP_CYCLES);
	ps_host_sleep_init();

	for (trig = 0; trig < PS_TRIG_NUM; trig++) {
		for (cycle = 1; cycle <= PS_HOST_SLEEP_CYCLES; cycle++) {
			if (ps_host_sleep_once(cycle, trig, 1) != 0) {
				fails++;
			}
		}
	}
	RTK_LOGI(TAG, "==== phase B done: %d/%d failed ====\n",
			 fails, PS_TRIG_NUM * PS_HOST_SLEEP_CYCLES);
}
#endif /* PS_RUN_HOST_SLEEP */

#if PS_RUN_LONGRUN
/* Phase C: alternate both directions, one line per round. */
static void ps_run_longrun(void)
{
	u32 round = 0;
	u32 fails_a = 0;
	u32 fails_b = 0;

	RTK_LOGI(TAG, "==== phase C: long run, %d rounds (0 = forever) ====\n", PS_LONGRUN_ROUNDS);
	ps_host_sleep_init();

	for (;;) {
		round++;
#if (PS_LONGRUN_ROUNDS != 0)
		if (round > PS_LONGRUN_ROUNDS) {
			break;
		}
#endif
		if (ps_suspend_resume_once(round, 0) != 0) {
			fails_a++;
		}
		/* Rotate the wake trigger so a long run covers all three. */
		if (ps_host_sleep_once(round, (u8)((round - 1) % PS_TRIG_NUM), 0) != 0) {
			fails_b++;
		}
		RTK_LOGI(TAG, "round %d(%s): suspend fails=%d, wake fails=%d\n", round,
				 ps_trig_name[(round - 1) % PS_TRIG_NUM], fails_a, fails_b);
	}

	RTK_LOGI(TAG, "==== phase C done: %d suspend fails, %d wake fails ====\n", fails_a, fails_b);
}
#endif /* PS_RUN_LONGRUN */

static void gspi_host_ps_task(void)
{
	if (gspi_host_link_up() != 0) {
		RTK_LOGE(TAG, "GSPI PS Demo: fail\n");
		rtos_task_delete(NULL);
		return;
	}

#if PS_RUN_PROBE
	ps_run_probes();
#endif
#if PS_RUN_SUSPEND_RESUME
	ps_run_suspend_resume();
#endif
#if PS_RUN_HOST_SLEEP
	ps_run_host_sleep();
#endif
#if PS_RUN_LONGRUN
	ps_run_longrun();
#endif

	RTK_LOGI(TAG, "GSPI PS Demo: done\n");
	rtos_task_delete(NULL);
}

int example_raw_gspi_host_ps(void)
{
	if (rtos_task_create(NULL, ((const char *)"gspi_host_ps"), (rtos_task_t)gspi_host_ps_task,
						 NULL, 1024 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "\n\r%s Create gspi_host_ps_task failed", __FUNCTION__);
	}

	return 0;
}
