#include "lwip_netconf.h"
#include "MQTTClient.h"

#define MQTT_SELECT_TIMEOUT 1

static void messageArrived(MessageData *data, void *discard)
{
	(void)discard;
	mqtt_printf(MQTT_INFO, "Message arrived on topic %s: %s\n", data->topicName->lenstring.data, (char *)data->message->payload);
}

//This example is original and cannot restart if failed. To use this example, define WAIT_FOR_ACK and not define MQTT_TASK in MQTTClient.h
void prvMQTTEchoTask(void *pvParameters)
{
	/* To avoid gcc warnings */
	(void) pvParameters;

	/* connect to gpssensor.ddns.net, subscribe to a topic, send and receive messages regularly every 5 sec */
	MQTTClient client;
	Network network;
	unsigned char sendbuf[512], readbuf[80];
	int rc = 0, count = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	const char *address = "broker.emqx.io";
	const char *sub_topic = "LASS/Test/Pm25Ameba/#";
	const char *pub_topic = "LASS/Test/Pm25Ameba/FT1_018";

	memset(readbuf, 0x00, sizeof(readbuf));

	NetworkInit(&network);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

	mqtt_printf(MQTT_INFO, "Wait Wi-Fi to be connected.");
	// Delay to check successful WiFi connection and obtain of an IP address
	while (LwIP_Check_Connectivity(NETIF_WLAN_STA_INDEX) != CONNECTION_VALID) {
		rtos_time_delay_ms(2000);
	}
	mqtt_printf(MQTT_INFO, "Wi-Fi connected.");

	mqtt_printf(MQTT_INFO, "Connect Network \"%s\"", address);
	while ((rc = NetworkConnect(&network, (char *)address, 1883)) != 0) {
		mqtt_printf(MQTT_INFO, "Return code from network connect is %d\n", rc);
		rtos_time_delay_ms(1000);
	}
	mqtt_printf(MQTT_INFO, "\"%s\" Connected", address);

	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = (char *)"FT1_018";

	mqtt_printf(MQTT_INFO, "Start MQTT connection");
	while ((rc = MQTTConnect(&client, &connectData)) != 0) {
		mqtt_printf(MQTT_INFO, "Return code from MQTT connect is %d\n", rc);
		rtos_time_delay_ms(1000);
	}
	mqtt_printf(MQTT_INFO, "MQTT Connected");

	mqtt_printf(MQTT_INFO, "Subscribe to Topic: %s", sub_topic);
	if ((rc = MQTTSubscribe(&client, sub_topic, QOS2, messageArrived)) != 0) {
		mqtt_printf(MQTT_INFO, "Return code from MQTT subscribe is %d\n", rc);
	}

	mqtt_printf(MQTT_INFO, "Publish Topics: %s", pub_topic);
	while (1) {
		MQTTMessage message;
		char payload[300];

		if (++count == 0) {
			count = 1;
		}

		message.qos = QOS1;
		message.retained = 0;
		message.payload = payload;
		sprintf(payload, "hello from AMEBA %d", count);
		message.payloadlen = strlen(payload);

		if ((rc = MQTTPublish(&client, pub_topic, &message)) != 0) {
			mqtt_printf(MQTT_INFO, "Return code from MQTT publish is %d\n", rc);
		}
		if ((rc = MQTTYield(&client, 1000)) != 0) {
			mqtt_printf(MQTT_INFO, "Return code from yield is %d\n", rc);
		}
		rtos_time_delay_ms(5000);
	}
	/* do not return */
}

#if defined(MQTT_TASK)
void MQTTPublishMessage(MQTTClient *c, char *topic)
{
	int rc = 0;
	static int count = 0;
	MQTTMessage message;
	char payload[300];
	message.qos = QOS1;
	message.retained = 0;
	message.payload = payload;

	if (c->mqttstatus == MQTT_RUNNING) {
		count++;
		sprintf(payload, "hello from AMEBA %d", count);
		message.payloadlen = strlen(payload);
		mqtt_printf(MQTT_INFO, "Publish Topic %s : %d", topic, count);
		if ((rc = MQTTPublish(c, topic, &message)) != 0) {
			mqtt_printf(MQTT_INFO, "Return code from MQTT publish is %d\n", rc);
			MQTTSetStatus(c, MQTT_START);
			c->ipstack->disconnect(c->ipstack);
		}
	}

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

	MQTTClient client;
	Network network;
	static unsigned char sendbuf[MQTT_SENDBUF_LEN], readbuf[MQTT_READBUF_LEN];
	int rc = 0, mqtt_pub_count = 0;
	MQTTPacket_connectData connectData = MQTTPacket_connectData_initializer;
	connectData.MQTTVersion = 3;
	connectData.clientID.cstring = (char *)"FT1_018";
	const char *address = "broker.emqx.io";
	const char *sub_topic = "LASS/Test/Pm25Ameba/#";
	const char *pub_topic = "LASS/Test/Pm25Ameba/FT1_018";

	NetworkInit(&network);
	MQTTClientInit(&client, &network, 30000, sendbuf, sizeof(sendbuf), readbuf, sizeof(readbuf));

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
			rc = FreeRTOS_Select(network.my_socket + 1, &read_fds, NULL, &except_fds, &timeout);

			if (FD_ISSET(network.my_socket, &except_fds)) {
				mqtt_printf(MQTT_INFO, "except_fds is set");
				MQTTSetStatus(&client, MQTT_START); //my_socket will be close and reopen in MQTTDataHandle if STATUS set to MQTT_START
			} else if (rc == 0) { //select timeout
				if (++mqtt_pub_count == 5) { //Send MQTT publish message every 5 seconds
					MQTTPublishMessage(&client, (char *)pub_topic);
					mqtt_pub_count = 0;
				}
			}
		}
		MQTTDataHandle(&client, &read_fds, &connectData, messageArrived, (char *)address, (char *)sub_topic);
	}
}
#endif

void vStartMQTTTasks(uint16_t usTaskStackSize, uint32_t uxTaskPriority)
{
#if defined(MQTT_TASK)
	if (rtos_task_create(NULL, "MQTTTask", prvMQTTTask, NULL, usTaskStackSize * 2, uxTaskPriority) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(prvMQTTTask) failed", __FUNCTION__);
	}
#else
	if (rtos_task_create(NULL, "MQTTEcho0", prvMQTTEchoTask, NULL, usTaskStackSize * 2, uxTaskPriority) != RTK_SUCCESS) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "\n\r%s rtos_task_create(prvMQTTEchoTask) failed", __FUNCTION__);
	}
#endif

}

void example_mqtt(void)
{
	vStartMQTTTasks(4096, 4);
}
/*-----------------------------------------------------------*/


