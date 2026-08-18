/*
 * Continuous-rotation servo control via MCP (Model Context Protocol).
 * Target: Tower Pro SSG90 (continuous rotation variant of SG90).
 *
 * PWM parameters (RTL8721F / amebagreen2):
 *   Timer   : TIM4, channel 0
 *   GPIO    : PA_6
 *   Tick    : 40 MHz / (39 + 1) = 1 MHz
 *   Period  : 20000 ticks = 20 ms  (50 Hz)
 *   Pulse   : 1000 µs = full CW / 1500 µs = stop / 2000 µs = full CCW
 *
 * Wiring:
 *   Servo signal -> PA_6
 *   Servo VCC    -> 5 V (external supply recommended)
 *   Servo GND    -> GND (shared with board)
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "mcp_server.h"
#include "PinNames.h"
#include "example_mcp_servo.h"
#include <stdlib.h>

#define TAG  "MCP_SERVO"

/* ── PWM hardware config ─────────────────────────────────────────────────── */
#define SERVO_TIMER       4
#define SERVO_CHANNEL     0
#define SERVO_GPIO        _PA_6

/* Prescaler 39: 40 MHz / 40 = 1 MHz tick */
#define SERVO_PRESCALER   39
#define SERVO_PERIOD      20000   /* 20 ms @ 1 MHz → 50 Hz */

/* Continuous rotation: 1000=full CW, 1500=stop, 2000=full CCW */
#define SERVO_PULSE_CW     1000   /* 1.0 ms → full clockwise        */
#define SERVO_PULSE_STOP   1500   /* 1.5 ms → stop                  */
#define SERVO_PULSE_CCW    2000   /* 2.0 ms → full counter-clockwise */
#define SERVO_SPEED_MAX    100

static int g_servo_speed = 0;    /* tracks last commanded speed */

/* ── Helpers ─────────────────────────────────────────────────────────────── */

/* speed: -100 (full CW) .. 0 (stop) .. +100 (full CCW) */
static u32 speed_to_pulse(int speed)
{
	return (u32)(SERVO_PULSE_STOP + speed * (SERVO_PULSE_CCW - SERVO_PULSE_STOP) / SERVO_SPEED_MAX);
}

static void servo_pwm_init(void)
{
	RTIM_TimeBaseInitTypeDef base;
	TIM_CCInitTypeDef cc;

	RCC_PeriphClockCmd(APBPeriph_TIMx[SERVO_TIMER],
					   APBPeriph_TIMx_CLOCK[SERVO_TIMER], ENABLE);

	RTIM_TimeBaseStructInit(&base);
	base.TIM_Idx       = SERVO_TIMER;
	base.TIM_Prescaler = SERVO_PRESCALER;
	base.TIM_Period    = SERVO_PERIOD - 1;
	RTIM_TimeBaseInit(TIMx[SERVO_TIMER], &base,
					  TIMx_irq[SERVO_TIMER], NULL, NULL);

	RTIM_CCStructInit(&cc);
	cc.TIM_OCPulse = speed_to_pulse(g_servo_speed);
	RTIM_CCxInit(TIMx[SERVO_TIMER], &cc, SERVO_CHANNEL);
	RTIM_CCxCmd(TIMx[SERVO_TIMER], SERVO_CHANNEL, TIM_CCx_Enable);
	Pinmux_Config(SERVO_GPIO, PINMUX_FUNCTION_TIM4_PWM0 + SERVO_CHANNEL);

	RTIM_Cmd(TIMx[SERVO_TIMER], ENABLE);
}

/* ── MCP tool handlers ───────────────────────────────────────────────────── */

static char *tool_servo_set_speed(const cJSON *params, bool *is_error, void *user_data)
{
	cJSON *j_speed = cJSON_GetObjectItem(params, "speed");
	char  *out     = (char *)malloc(64);
	int    speed;
	u32    pulse;

	(void)user_data;
	if (!out) {
		return NULL;
	}

	if (!j_speed || !cJSON_IsNumber(j_speed)) {
		*is_error = true;
		DiagSnPrintf(out, 64, "missing or invalid parameter: speed");
		return out;
	}

	speed = j_speed->valueint;
	if (speed < -SERVO_SPEED_MAX || speed > SERVO_SPEED_MAX) {
		*is_error = true;
		DiagSnPrintf(out, 64, "speed out of range [-%d, %d]",
					 SERVO_SPEED_MAX, SERVO_SPEED_MAX);
		return out;
	}

	pulse = speed_to_pulse(speed);
	g_servo_speed = speed;
	RTIM_CCRxSet(TIMx[SERVO_TIMER], pulse, SERVO_CHANNEL);

	*is_error = false;
	DiagSnPrintf(out, 64, "speed=%d pulse=%u us", speed, (unsigned)pulse);
	RTK_LOGS(TAG, RTK_LOG_INFO, "servo -> speed=%d (%u us)\r\n", speed, (unsigned)pulse);
	return out;
}

static char *tool_servo_get_speed(const cJSON *params, bool *is_error, void *user_data)
{
	char *out = (char *)malloc(16);

	(void)params;
	(void)user_data;
	if (!out) {
		return NULL;
	}

	*is_error = false;
	DiagSnPrintf(out, 16, "%d", g_servo_speed);
	return out;
}

/* ── Parameter descriptors ───────────────────────────────────────────────── */

static const mcp_param_desc_t set_speed_params[] = {
	{
		.name        = "speed",
		.description = "Speed -100 (full CW) to +100 (full CCW), 0 = stop",
		.type        = MCP_PARAM_INTEGER,
		.required    = true,
	}
};

/* ── Entry point ─────────────────────────────────────────────────────────── */

static void mcp_servo_thread(void *param)
{
	mcp_server_config_t cfg = {0};
	uint8_t *ip;
	char ip_str[16];

	(void)param;

	while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	servo_pwm_init();

	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "\n====================Example: MCP Servo Control====================\n");

	mcp_server_add_tool("servo_set_speed",
						"Set continuous-rotation servo speed."
						" 'speed' is -100 (full CW) to +100 (full CCW), 0 = stop.",
						set_speed_params, 1, tool_servo_set_speed, NULL);

	mcp_server_add_tool("servo_get_speed",
						"Get the current servo speed (-100 to +100).",
						NULL, 0, tool_servo_get_speed, NULL);

	cfg.port        = EXAMPLE_MCP_PORT;
	cfg.endpoint    = EXAMPLE_MCP_ENDPOINT;
	cfg.server_name = EXAMPLE_MCP_NAME;

	if (mcp_server_start(&cfg) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "mcp_server_start failed\r\n");
		rtos_task_delete(NULL);
		return;
	}

	ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
	DiagSnPrintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
				 ip[0], ip[1], ip[2], ip[3]);
	RTK_LOGS(TAG, RTK_LOG_INFO, "MCP server ready at http://%s:%d%s\r\n",
			 ip_str, EXAMPLE_MCP_PORT, EXAMPLE_MCP_ENDPOINT);

	rtos_task_delete(NULL);
}

void example_mcp_servo(void)
{
	if (rtos_task_create(NULL, "mcp_servo",
						 mcp_servo_thread, NULL,
						 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR,
				 "%s rtos_task_create failed\r\n", __FUNCTION__);
	}
}
