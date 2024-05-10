#include "ameba_soc.h"
#include "os_wrapper/mutex.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "os_wrapper/common.h"
#ifdef TEST_FRAMEWORK_NS
#include "os_wrapper/thread.h"
#include "os_wrapper/semaphore.h"
#endif

static SemaphoreHandle_t xTfmWrapperSemaphore;

void *os_wrapper_mutex_create(void)
{
	// xTfmWrapperSemaphore = xSemaphoreCreateMutex();
	return (void *)xTfmWrapperSemaphore;
}

uint32_t os_wrapper_mutex_acquire(void *handle, uint32_t timeout)
{
	UNUSED(handle);
	UNUSED(timeout);
	xTfmWrapperSemaphore = xSemaphoreCreateMutex();
	xSemaphoreTake(xTfmWrapperSemaphore, portMAX_DELAY);
	return OS_WRAPPER_SUCCESS;
}


uint32_t os_wrapper_mutex_release(void *handle)
{
	UNUSED(handle);
	xSemaphoreGive(xTfmWrapperSemaphore);
	vSemaphoreDelete(xTfmWrapperSemaphore);
	return OS_WRAPPER_SUCCESS;
}

int tfm_log_printf(const char *fmt, ...)
{
	int count = 0;
	va_list ap;

	if (ConfigDebugClose == 1) {
		return 0;
	}

	va_start(ap, fmt);
	count = DiagVSprintf(NULL, fmt, ap);
	va_end(ap);

	return count;
}

int stdio_output_string(const unsigned char *str, uint32_t len)
{
	int nChars = 0;

	for (/*Empty */; len > 0; --len) {
		LOGUART_PutChar(*str++);
		++nChars;
	}
	return nChars;
}

#ifdef TEST_FRAMEWORK_NS
void *os_wrapper_semaphore_create(uint32_t max_count, uint32_t initial_count,
								  const char *name)
{
	UNUSED(max_count);
	UNUSED(initial_count);
	UNUSED(name);

	SemaphoreHandle_t	tfm_sema = NULL;
	vSemaphoreCreateBinary(tfm_sema);
	xSemaphoreTake(tfm_sema, 1 / portTICK_RATE_MS);

	return (void *)tfm_sema;
}

uint32_t os_wrapper_semaphore_acquire(void *handle, uint32_t timeout)
{
	xSemaphoreTake(handle, timeout);
	return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_semaphore_release(void *handle)
{
	xSemaphoreGive(handle);
	return OS_WRAPPER_SUCCESS;
}

uint32_t os_wrapper_semaphore_delete(void *handle)
{
	vSemaphoreDelete(handle);
	return OS_WRAPPER_SUCCESS;
}

void *os_wrapper_thread_new(const char *name, int32_t stack_size,
							os_wrapper_thread_func func, void *arg,
							uint32_t priority)
{
	BaseType_t xReturned;
	TaskHandle_t xHandle = NULL;

	xReturned = xTaskCreate(
					func,				/* Function that implements the task. */
					name,				/* Text name for the task. */
					stack_size,			/* Stack size in words, not bytes. */
					arg,				/* Parameter passed into the task. */
					priority,			/* Priority at which the task is created. */
					&xHandle);			/* Used to pass out the created task's handle. */


	if (pdTRUE != xReturned) {
		return NULL;
	}

	return (void *)xHandle;
}

uint32_t os_wrapper_thread_get_priority(void *handle, uint32_t *priority)
{
	UBaseType_t uxTaskPriority = uxTaskPriorityGet(handle);

	*priority = (uint32_t)uxTaskPriority;

	return OS_WRAPPER_SUCCESS;
}

void *os_wrapper_thread_get_handle(void)
{
	return (void *)xTaskGetCurrentTaskHandle();
}

void os_wrapper_thread_exit(void)
{
	vTaskDelete(NULL); /*If null is passed in here then it is the calling task that is being deleted. */
}
#endif

