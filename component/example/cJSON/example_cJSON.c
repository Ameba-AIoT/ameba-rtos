/* FreeRTOS includes. */
#include "cJSON.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"

/*  The data structure for this example

{
	"Motion_Sensor":	5,
	"Light":	{
		"Red":	253,
		"Green":	123,
		"Blue":	43
	}
}

*/
char *generate_json_data(int i, int r, int g, int b)
{


	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);


	cJSON *IOTJSObject = NULL, *colorJSObject = NULL;
	char *iot_json = NULL;

	if ((IOTJSObject = cJSON_CreateObject()) != NULL) {

		cJSON_AddItemToObject(IOTJSObject, "Motion_Sensor", cJSON_CreateNumber(i));
		cJSON_AddItemToObject(IOTJSObject, "Light", colorJSObject = cJSON_CreateObject());

		cJSON_AddItemToObject(colorJSObject, "Red", cJSON_CreateNumber(r));
		cJSON_AddItemToObject(colorJSObject, "Green", cJSON_CreateNumber(g));
		cJSON_AddItemToObject(colorJSObject, "Blue", cJSON_CreateNumber(b));

		iot_json = cJSON_Print(IOTJSObject);
		cJSON_Delete(IOTJSObject);
	}
	return iot_json;

}

static void handle_json_data(char *iot_json)
{
	cJSON_Hooks memoryHook;

	memoryHook.malloc_fn = malloc;
	memoryHook.free_fn = free;
	cJSON_InitHooks(&memoryHook);


	cJSON *IOTJSObject, *sensorJSObject, *lightJSObject, *redJSObject, *greenJSObject, *blueJSObject;
	int sensor_data = 0, red = 0, green = 0, blue = 0;

	if ((IOTJSObject = cJSON_Parse(iot_json)) != NULL) {
		sensorJSObject = cJSON_GetObjectItem(IOTJSObject, "Motion_Sensor");
		if (sensorJSObject) {
			sensor_data = sensorJSObject->valueint;
			printf("\r\nThe sensor data is %d\r\n", sensor_data);
		}

		lightJSObject = cJSON_GetObjectItem(IOTJSObject, "Light");

		if (lightJSObject) {
			redJSObject = cJSON_GetObjectItem(lightJSObject, "Red");
			greenJSObject = cJSON_GetObjectItem(lightJSObject, "Green");
			blueJSObject = cJSON_GetObjectItem(lightJSObject, "Blue");

			if (redJSObject) {
				red = redJSObject->valueint;
			}
			if (greenJSObject) {
				green = greenJSObject->valueint;
			}
			if (blueJSObject) {
				blue = blueJSObject->valueint;
			}
			printf("\r\nThe RGB value is %d/%d/%d\r\n", red, green, blue);
		}

		cJSON_Delete(IOTJSObject);
	}
}

static void example_cJSON_thread(void *param)
{
	(void)param;
	rtos_time_delay_ms(3000);
	char *iot_json;
	iot_json = generate_json_data(5, 253, 123, 43);
	if (iot_json) {
		printf("\r\nThe IoT json is: \r\n%s", iot_json);
		handle_json_data(iot_json);
		free(iot_json);
	} else {
		printf("\r\nGen JSON failed!\r\n");
	}
	rtos_task_delete(NULL);
}

void example_cJSON(void)
{
	if (rtos_task_create(NULL, "example_cJSON_thread", example_cJSON_thread, NULL, 2048 * 4, 1) != SUCCESS) {
		printf("\n\r%s rtos_task_create example_cJSON_thread failed", __FUNCTION__);
	}
}