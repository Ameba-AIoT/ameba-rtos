#include "ameba_soc.h"
#include "os_wrapper.h"
#include "lwip_netconf.h"
#include "mcp_server.h"
#include "PinNames.h"
#include "gpio_api.h"
#include "example_mcp_gpio.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define TAG "MCP_GPIO"

/* Parse "PA_0".."PC_8" → PinName; returns NC on invalid input. */
static PinName parse_pin(const char *s)
{
	int port, num;

	if (!s || s[0] != 'P' || s[2] != '_') {
		return NC;
	}
	switch (s[1]) {
	case 'A':
		port = 0;
		break;
	case 'B':
		port = 1;
		break;
	case 'C':
		port = 2;
		break;
	default:
		return NC;
	}
	num = atoi(s + 3);
	if (num < 0 || num > 31) {
		return NC;
	}
	if (port == 2 && num > 8) {
		return NC;
	}
	return (PinName)((port << 5) | num);
}

/* ── Tool handler ─────────────────────────────────────────────────────── */

static char *tool_gpio_control(const cJSON *params, bool *is_error, void *user_data)
{
	cJSON      *j_pin   = cJSON_GetObjectItem(params, "pin");
	cJSON      *j_state = cJSON_GetObjectItem(params, "state");
	const char *pin_str = (j_pin   && cJSON_IsString(j_pin))   ? j_pin->valuestring   : "";
	const char *state   = (j_state && cJSON_IsString(j_state)) ? j_state->valuestring : "";
	char       *out     = (char *)malloc(64);
	int         value;
	PinName     pin;
	gpio_t      gpio;

	(void)user_data;
	if (!out) {
		return NULL;
	}

	pin = parse_pin(pin_str);
	if (pin == NC) {
		*is_error = true;
		DiagSnPrintf(out, 64, "invalid pin: %s", pin_str);
		return out;
	}

	if (strcmp(state, "high") == 0) {
		value = 1;
	} else if (strcmp(state, "low") == 0) {
		value = 0;
	} else {
		*is_error = true;
		DiagSnPrintf(out, 64, "invalid state: must be high or low");
		return out;
	}

	gpio_init(&gpio, pin);
	gpio_mode(&gpio, PullNone);
	gpio_dir(&gpio, PIN_OUTPUT);
	gpio_write(&gpio, value);

	*is_error = false;
	DiagSnPrintf(out, 64, "%s set to %s", pin_str, state);
	RTK_LOGS(TAG, RTK_LOG_INFO, "[MCP] GPIO %s -> %s\r\n", pin_str, state);
	return out;
}

/* ── Tool parameter descriptors ───────────────────────────────────────── */

static const mcp_param_desc_t gpio_params[] = {
	{
		.name = "pin",
		.description = "GPIO pin name, e.g. \"PA_0\", \"PB_3\", \"PC_2\"",
		.type = MCP_PARAM_STRING,
		.required = true
	},
	{
		.name = "state",
		.description = "\"high\" or \"low\"",
		.type = MCP_PARAM_STRING,
		.required = true
	}
};

/* ── Entry point ──────────────────────────────────────────────────────── */

static void example_mcp_gpio_thread(void *param)
{
	mcp_server_config_t cfg = {0};

	(void)param;

	while (lwip_check_connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	RTK_LOGS(TAG, RTK_LOG_INFO,
			 "\n====================Example: MCP GPIO Control====================\n");

	mcp_server_add_tool("gpio_control",
						"Control a GPIO pin. 'pin' is the pin name (e.g. \"PA_0\"),"
						" 'state' is \"high\" or \"low\".",
						gpio_params, 2, tool_gpio_control, NULL);

	cfg.port        = EXAMPLE_MCP_PORT;
	cfg.endpoint    = EXAMPLE_MCP_ENDPOINT;
	cfg.server_name = EXAMPLE_MCP_NAME;

	if (mcp_server_start(&cfg) != 0) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "ERROR: mcp_server_start failed\r\n");
		rtos_task_delete(NULL);
		return;
	}

	uint8_t *ip = lwip_get_ip(NETIF_WLAN_STA_INDEX);
	char ip_str[16];
	DiagSnPrintf(ip_str, sizeof(ip_str), "%d.%d.%d.%d",
				 ip[0], ip[1], ip[2], ip[3]);
	RTK_LOGS(TAG, RTK_LOG_INFO, "MCP server ready at http://%s:%d%s\r\n",
			 ip_str, EXAMPLE_MCP_PORT, EXAMPLE_MCP_ENDPOINT);

	rtos_task_delete(NULL);
}

void example_mcp_gpio(void)
{
	if (rtos_task_create(NULL, "mcp_gpio_ctrl",
						 example_mcp_gpio_thread, NULL,
						 2048 * 4, 1) != RTK_SUCCESS) {
		RTK_LOGS(TAG, RTK_LOG_ERROR,
				 "%s rtos_task_create failed\r\n", __FUNCTION__);
	}
}
