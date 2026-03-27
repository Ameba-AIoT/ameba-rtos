#include "lwip_netconf.h"
#include "MQTTClient.h"
#include "ameba_diagnose.h"

#define MQTT_SELECT_TIMEOUT 1
extern int atcmd_service(char *line_buf);
static MQTTClient client;
static const char *address = "192.168.1.106";
static const char *sub_topic = "request_diag_events";
static const char *pub_topic = "response_diag_events";
u8  diag_send_buf[1024] = {0};
int rtk_diag_mqtt_send(const u8 *data, u16 len);
static const char *atcmd_header = "AT+";
static void messageArrived(MessageData *data, void *discard)
{
	(void)discard;
	mqtt_printf(MQTT_INFO, "Message arrived on topic %s: %s\n", data->topicName->lenstring.data, (char *)data->message->payload);

	// Check if it's a direct AT command
	if (memcmp(data->message->payload, atcmd_header, strlen(atcmd_header)) == 0) {
		mqtt_printf(MQTT_INFO, "Processing direct atcmd: %s\n", (char *)data->message->payload);
		atcmd_service((char *)data->message->payload);
	} else { // Try to parse JSON format

		mqtt_printf(MQTT_INFO, "Message is not a valid AT command\n");
	}
	// clear the payload buffer
	memset(data->message->payload, 0, data->message->payloadlen);
	data->message->payloadlen = 0;
}

int rtk_diag_mqtt_send(const u8 *data, u16 len)
{
	int rc = 0;
	MQTTMessage message;

	if (client.mqttstatus == MQTT_RUNNING) {
		message.qos = QOS1;
		message.retained = 0;
		message.payload = (char *)data;
		message.payloadlen = len;

		mqtt_printf(MQTT_INFO, "Publish Topic %s with data length %d", pub_topic, len);

		if ((rc = MQTTPublish(&client, pub_topic, &message)) != 0) {
			mqtt_printf(MQTT_INFO, "Return code from MQTT publish is %d\n", rc);
			MQTTSetStatus(&client, MQTT_START);
			client.ipstack->disconnect(client.ipstack);
		}
	}

	return rc;
}

static void prvMQTTTask(void *pvParameters)
{
	/* To avoid gcc warnings */
	(void) pvParameters;

	// Delay to check successful WiFi connection and obtain of an IP address
	while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}

	printf("\r\n====================Example: mqtt====================\r\n");
	Network network;
	static unsigned char sendbuf[MQTT_SENDBUF_LEN], readbuf[MQTT_READBUF_LEN];
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = (char *)"FT1_018";

	NetworkInit(&network);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));
	rtk_diag_config_transform(rtk_diag_mqtt_send, diag_send_buf, 1024);
	while (1) {
		fd_set read_fds;
		fd_set except_fds;
		struct timeval timeout;

		FD_ZERO(&read_fds);
		FD_ZERO(&except_fds);
		timeout.tv_sec = MQTT_SELECT_TIMEOUT;
		timeout.tv_usec = 0;

		if (network.my_socket >= 0) {
			FD_SET(network.my_socket, &read_fds);
			FD_SET(network.my_socket, &except_fds);
			FreeRTOS_Select(network.my_socket + 1, &read_fds, NULL, &except_fds, &timeout);

			if (FD_ISSET(network.my_socket, &except_fds)) {
				mqtt_printf(MQTT_INFO, "except_fds is set");
				MQTTSetStatus(&client, MQTT_START); //my_socket will be close and reopen in MQTTDataHandle if STATUS set to MQTT_START
			}
		}  else {
			rtos_time_delay_ms(2000);
		}
		MQTTDataHandle(&client, &read_fds, &connectData, messageArrived, (char *)address, (char *)sub_topic);
	}
}

void vStartMQTTTasks(uint16_t usTaskStackSize, uint32_t uxTaskPriority)
{
	if (rtos_task_create(NULL, "MQTTTask", prvMQTTTask, NULL, usTaskStackSize * 2, uxTaskPriority) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(prvMQTTTask) failed", __FUNCTION__);
	}
}

void example_mqtt(void)
{
	vStartMQTTTasks(4096, 4);
}
/*-----------------------------------------------------------*/


