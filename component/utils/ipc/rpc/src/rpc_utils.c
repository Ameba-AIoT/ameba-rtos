#define LOG_TAG "RPCCommon"
#include "rpc_utils.h"

#include "ameba_soc.h"

// Maximum value of a semaphore
#define SEM_VALUE_MAX 0x7FFFU

extern int rtos_critical_is_in_interrupt(void);

void RPC_MutexLock(RPC_Mutex *mutex)
{
	if (rtos_sema_take(mutex->sema, RTOS_MAX_DELAY) != SUCCESS) {
		RPC_LOGE("%s: %d rtos_sema_take failed", __func__, __LINE__);
	}
}

void RPC_MutexUnlock(RPC_Mutex *mutex)
{
	if (rtos_sema_give(mutex->sema) != SUCCESS) {
		RPC_LOGE("%s: %d rtos_sema_give failed", __func__, __LINE__);
	}
}

void RPC_MutexInit(RPC_Mutex *mutex)
{
	rtos_sema_create_binary(&(mutex->sema));
	RPC_MutexUnlock(mutex);
}

void RPC_MutexDestroy(RPC_Mutex *mutex)
{
	rtos_sema_delete(mutex->sema);
}

void RPC_SemInit(RPC_Sem *sem, uint32_t value)
{
	sem->value = value;
	rtos_sema_create(&sem->semaphore, 0, SEM_VALUE_MAX);
	if (!sem->semaphore) {
		RPC_LOGE("%s: no memory.", __func__);
	}
}

int32_t RPC_SemWait(RPC_Sem *sem, uint32_t ms)
{
	int old_value = __atomic_fetch_sub((uint32_t *)&sem->value, 1, __ATOMIC_SEQ_CST);

	if (old_value > 0) {
		return 0;
	}

	int status = FAIL;
	if (rtos_critical_is_in_interrupt()) {
		status = rtos_sema_take(sem->semaphore, NULL);
	} else {
		status = rtos_sema_take(sem->semaphore, ms);
	}
	if (status != SUCCESS) {
		RPC_LOGE("%s: rtos_sema_take fail.", __func__);
		return -1;
	}
	return 0;
}

void RPC_SemPost(RPC_Sem *sem)
{
	int32_t old_value = __atomic_fetch_add((uint32_t *)&sem->value, 1, __ATOMIC_SEQ_CST);
	if (old_value < 0) {
		rtos_sema_give(sem->semaphore);
	}
}

void RPC_SemDestroy(RPC_Sem *sem)
{
	if (sem->semaphore) {
		rtos_sema_delete(sem->semaphore);
	}

}
