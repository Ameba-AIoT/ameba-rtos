/*
 * Copyright (c) 2001-2003 Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 */

/* lwIP includes. */
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/timeouts.h"

struct timeoutlist
{
    struct sys_timeouts timeouts;
    rtos_task_t pid;
};

/* This is the number of threads that can be started with sys_thread_new() */
#define SYS_THREAD_MAX 6

static struct timeoutlist s_timeoutlist[SYS_THREAD_MAX];
static u16_t s_nextthread = 0;


/*-----------------------------------------------------------------------------------*/
//  Creates an empty mailbox.
err_t sys_mbox_new(sys_mbox_t *mbox, int size)
{
    (void ) size;

    rtos_queue_create(mbox, size, sizeof( void * ) );

#if SYS_STATS
      ++lwip_stats.sys.mbox.used;
      if (lwip_stats.sys.mbox.max < lwip_stats.sys.mbox.used) {
         lwip_stats.sys.mbox.max = lwip_stats.sys.mbox.used;
      }
#endif /* SYS_STATS */
 if (*mbox == NULL)
  return ERR_MEM;

 return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Deallocates a mailbox. If there are messages still present in the
  mailbox when the mailbox is deallocated, it is an indication of a
  programming error in lwIP and the developer should be notified.
*/
void sys_mbox_free(sys_mbox_t *mbox)
{
    if( rtos_queue_message_waiting( *mbox ) )
    {
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
        // TODO notify the user of failure.
    }

    rtos_queue_delete( *mbox );

#if SYS_STATS
     --lwip_stats.sys.mbox.used;
#endif /* SYS_STATS */
}

/*-----------------------------------------------------------------------------------*/
//   Posts the "msg" to the mailbox.
void sys_mbox_post(sys_mbox_t *mbox, void *data)
{
    while ( rtos_queue_send(*mbox, &data, RTOS_MAX_TIMEOUT ) != RTK_SUCCESS ){}
}


/*-----------------------------------------------------------------------------------*/
//   Try to post the "msg" to the mailbox.
err_t sys_mbox_trypost(sys_mbox_t *mbox, void *msg)
{
    err_t result;

    if ( rtos_queue_send( *mbox, &msg, 0 ) == RTK_SUCCESS ){
        result = ERR_OK;
    }
    else {
        // could not post, queue must be full
        result = ERR_MEM;
#if SYS_STATS
        lwip_stats.sys.mbox.err++;
#endif /* SYS_STATS */
    }

    return result;
}

/*-----------------------------------------------------------------------------------*/
//   Try to post the "msg" to the mailbox from ISR.
err_t sys_mbox_trypost_fromisr(sys_mbox_t *q, void *msg)
{
    return sys_mbox_trypost(q, msg);
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread until a message arrives in the mailbox, but does
  not block the thread longer than "timeout" milliseconds (similar to
  the sys_arch_sem_wait() function). The "msg" argument is a result
  parameter that is set by the function (i.e., by doing "*msg =
  ptr"). The "msg" parameter maybe NULL to indicate that the message
  should be dropped.

  The return values are the same as for the sys_arch_sem_wait() function:
  Number of milliseconds spent waiting or SYS_ARCH_TIMEOUT if there was a
  timeout.

  Note that a function with a similar name, sys_mbox_fetch(), is
  implemented by lwIP.
*/
u32_t sys_arch_mbox_fetch(sys_mbox_t *mbox, void **msg, u32_t timeout)
{
    void *dummyptr;
    uint32_t StartTime, EndTime, Elapsed;

    StartTime = rtos_time_get_current_system_time_ms();

    if ( msg == NULL )
    {
        msg = &dummyptr;
    }

    if ( timeout != 0 )
    {
        if ( RTK_SUCCESS == rtos_queue_receive( *mbox, &(*msg), timeout) )
        {
            EndTime = rtos_time_get_current_system_time_ms();
            Elapsed = EndTime - StartTime;

            return ( Elapsed );
        }
        else // timed out blocking for message
        {
            *msg = NULL;

            return SYS_ARCH_TIMEOUT;
        }
    }
    else // block forever for a message.
    {
        while( RTK_SUCCESS != rtos_queue_receive( *mbox, &(*msg), RTOS_MAX_TIMEOUT ) ){} // time is arbitrary
        EndTime = rtos_time_get_current_system_time_ms();
        Elapsed = EndTime - StartTime;

        return ( Elapsed ); // return time blocked TODO test
    }
}

/*-----------------------------------------------------------------------------------*/
/*
  Similar to sys_arch_mbox_fetch, but if message is not ready immediately, we'll
  return with SYS_MBOX_EMPTY.  On success, 0 is returned.
*/
u32_t sys_arch_mbox_tryfetch(sys_mbox_t *mbox, void **msg)
{
void *dummyptr;

    if ( msg == NULL )
    {
        msg = &dummyptr;
    }

   if ( RTK_SUCCESS == rtos_queue_receive( *mbox, &(*msg), 0 ) )
   {
      return ERR_OK;
   }
   else
   {
      return SYS_MBOX_EMPTY;
   }
}
/*----------------------------------------------------------------------------------*/
int sys_mbox_valid(sys_mbox_t *mbox)
{
    if (*mbox == SYS_MBOX_NULL)
        return 0;
    else
        return 1;
}
/*-----------------------------------------------------------------------------------*/
void sys_mbox_set_invalid(sys_mbox_t *mbox)
{
  *mbox = SYS_MBOX_NULL;
}

/*-----------------------------------------------------------------------------------*/
//  Creates a new semaphore. The "count" argument specifies
//  the initial state of the semaphore.
err_t sys_sem_new(sys_sem_t *sem, u8_t count)
{
    rtos_sema_create_binary(sem);
    rtos_sema_give(*sem);
    if(*sem == NULL)
    {
#if SYS_STATS
      ++lwip_stats.sys.sem.err;
#endif /* SYS_STATS */
        return ERR_MEM;
    }

    if(count == 0)    // Means it can't be taken
    {
        rtos_sema_take(*sem, 1);
    }
#if SYS_STATS
    ++lwip_stats.sys.sem.used;
     if (lwip_stats.sys.sem.max < lwip_stats.sys.sem.used) {
        lwip_stats.sys.sem.max = lwip_stats.sys.sem.used;
    }
#endif /* SYS_STATS */

    return ERR_OK;
}

/*-----------------------------------------------------------------------------------*/
/*
  Blocks the thread while waiting for the semaphore to be
  signaled. If the "timeout" argument is non-zero, the thread should
  only be blocked for the specified time (measured in
  milliseconds).

  If the timeout argument is non-zero, the return value is the number of
  milliseconds spent waiting for the semaphore to be signaled. If the
  semaphore wasn't signaled within the specified time, the return value is
  SYS_ARCH_TIMEOUT. If the thread didn't have to wait for the semaphore
  (i.e., it was already signaled), the function may return zero.

  Notice that lwIP implements a function with a similar name,
  sys_sem_wait(), that uses the sys_arch_sem_wait() function.
*/
u32_t sys_arch_sem_wait(sys_sem_t *sem, u32_t timeout)
{
    uint32_t StartTime, EndTime, Elapsed;

    StartTime = rtos_time_get_current_system_time_ms();

    if(timeout != 0)
    {
        if(rtos_sema_take(*sem, timeout) == RTK_SUCCESS)
        {
            EndTime = rtos_time_get_current_system_time_ms();
            Elapsed = EndTime - StartTime;

            return (Elapsed); // return time blocked TODO test
        }
        else
        {
            return SYS_ARCH_TIMEOUT;
        }
    }
    else // must block without a timeout
    {
        while(rtos_sema_take(*sem, RTOS_MAX_TIMEOUT) != RTK_SUCCESS){}
        EndTime = rtos_time_get_current_system_time_ms();
        Elapsed = EndTime - StartTime;

        return ( Elapsed ); // return time blocked
    }
}

/*-----------------------------------------------------------------------------------*/
// Signals a semaphore
void sys_sem_signal(sys_sem_t *sem)
{
    rtos_sema_give(*sem);
}

/*-----------------------------------------------------------------------------------*/
// Deallocates a semaphore
void sys_sem_free(sys_sem_t *sem)
{
#if SYS_STATS
    --lwip_stats.sys.sem.used;
#endif /* SYS_STATS */
    rtos_sema_delete(*sem);
}
/*-----------------------------------------------------------------------------------*/
int sys_sem_valid(sys_sem_t *sem)
{
    if (*sem == SYS_SEM_NULL)
        return 0;
    else
        return 1;
}

/*-----------------------------------------------------------------------------------*/
void sys_sem_set_invalid(sys_sem_t *sem)
{
    *sem = SYS_SEM_NULL;
}

/*-----------------------------------------------------------------------------------*/
// Initialize sys arch
void sys_init(void)
{
    int i;

    // Initialize the the per-thread sys_timeouts structures
    // make sure there are no valid pids in the list
    for(i = 0; i < SYS_THREAD_MAX; i++)
    {
        s_timeoutlist[i].pid = 0;
        s_timeoutlist[i].timeouts.next = NULL;
    }

    // keep track of how many threads have been created
    s_nextthread = 0;
}

/*-----------------------------------------------------------------------------------*/
/*
  Returns a pointer to the per-thread sys_timeouts structure. In lwIP,
  each thread has a list of timeouts which is represented as a linked
  list of sys_timeout structures. The sys_timeouts structure holds a
  pointer to a linked list of timeouts. This function is called by
  the lwIP timeout scheduler and must not return a NULL value.

  In a single threaded sys_arch implementation, this function will
  simply return a pointer to a global sys_timeouts variable stored in
  the sys_arch module.
*/
struct sys_timeouts *sys_arch_timeouts(void)
{
    int i;
    rtos_task_t pid;
    struct timeoutlist *tl;

    pid =  rtos_task_handle_get();

    for(i = 0; i < s_nextthread; i++)
    {
        tl = &(s_timeoutlist[i]);
        if(tl->pid == pid)
        {
            return &(tl->timeouts);
        }
    }
    // Error
    return NULL;
}
/*-----------------------------------------------------------------------------------*/
                                      /* Mutexes*/
/*-----------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------------*/
#if LWIP_COMPAT_MUTEX == 0
/* Create a new mutex*/
err_t sys_mutex_new(sys_mutex_t *mutex) 
{
    rtos_mutex_create(mutex);
    if(*mutex == NULL)
    {
#if SYS_STATS
        ++lwip_stats.sys.mutex.err;
#endif /* SYS_STATS */
        return ERR_MEM;
    }

#if SYS_STATS
    ++lwip_stats.sys.mutex.used;
    if (lwip_stats.sys.mutex.max < lwip_stats.sys.mutex.used) {
        lwip_stats.sys.mutex.max = lwip_stats.sys.mutex.used;
    }
#endif /* SYS_STATS */
    return ERR_OK;
}
/*-----------------------------------------------------------------------------------*/
/* Deallocate a mutex*/
void sys_mutex_free(sys_mutex_t *mutex)
{
#if SYS_STATS
      --lwip_stats.sys.mutex.used;
#endif /* SYS_STATS */

    rtos_mutex_delete(*mutex);
}
/*-----------------------------------------------------------------------------------*/
/* Lock a mutex*/
void sys_mutex_lock(sys_mutex_t *mutex)
{
    rtos_mutex_take(*mutex, MUTEX_WAIT_TIMEOUT);
}

/*-----------------------------------------------------------------------------------*/
/* Unlock a mutex*/
void sys_mutex_unlock(sys_mutex_t *mutex)
{
    rtos_mutex_give(*mutex);
}
#endif /*LWIP_COMPAT_MUTEX*/

/*-----------------------------------------------------------------------------------*/
// TODO
/*-----------------------------------------------------------------------------------*/
/*
  Starts a new thread with priority "prio" that will begin its execution in the
  function "thread()". The "arg" argument will be passed as an argument to the
  thread() function. The id of the new thread is returned. Both the id and
  the priority are system dependent.
*/
sys_thread_t sys_thread_new(const char *name, lwip_thread_fn thread , void *arg, int stacksize, int prio)
{
    rtos_task_t CreatedTask;
    int result;

   if ( s_nextthread < SYS_THREAD_MAX )
   {
       rtos_critical_enter(RTOS_CRITICAL_LWIP);
       result = rtos_task_create(&CreatedTask, (const char *) name, thread, arg, stacksize*4, prio);

       // For each task created, store the task handle (pid) in the timers array.
       // This scheme doesn't allow for threads to be deleted
       s_timeoutlist[s_nextthread++].pid = CreatedTask;
       rtos_critical_exit(RTOS_CRITICAL_LWIP);

       if(result == RTK_SUCCESS)
       {
           return CreatedTask;
       }
       else
       {
           return NULL;
       }
   }
   else
   {
      return NULL;
   }
}

int sys_thread_delete(rtos_task_t pid)
{
    int i, isFind = 0;
    struct timeoutlist *tl, *tend = NULL;

    pid = (( pid == NULL)?(rtos_task_t) rtos_task_handle_get() : pid);

    if (s_nextthread)
    {
        rtos_critical_enter(RTOS_CRITICAL_LWIP);

        tend = &(s_timeoutlist[s_nextthread-1]);//the last one
        for(i = 0; i < s_nextthread; i++)
        {
            tl = &(s_timeoutlist[i]);
            if(tl->pid == pid)
            {//find the task, exchange with the last one
                memcpy(tl, tend, sizeof(struct timeoutlist));
                memset(tend, 0, sizeof(struct timeoutlist));
                s_nextthread --;
                isFind = 1;
                break;
            }
        }

        if (isFind) {
            rtos_task_delete(pid);
        }

        rtos_critical_exit(RTOS_CRITICAL_LWIP);

        if (isFind)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}

/*
  This optional function does a "fast" critical region protection and returns
  the previous protection level. This function is only called during very short
  critical regions. An embedded system which supports ISR-based drivers might
  want to implement this function by disabling interrupts. Task-based systems
  might want to implement this by using a mutex or disabling tasking. This
  function should support recursive calls from the same task or interrupt. In
  other words, sys_arch_protect() could be called while already protected. In
  that case the return value indicates that it is already protected.

  sys_arch_protect() is only required if your port is supporting an operating
  system.
*/
sys_prot_t sys_arch_protect(void)
{
    rtos_critical_enter(RTOS_CRITICAL_LWIP);
    return 1;
}

/*
  This optional function does a "fast" set of critical region protection to the
  value specified by pval. See the documentation for sys_arch_protect() for
  more information. This function is only required if your port is supporting
  an operating system.
*/
void sys_arch_unprotect(sys_prot_t pval)
{
    ( void ) pval;
    rtos_critical_exit(RTOS_CRITICAL_LWIP);
}

/*
 * Prints an assertion messages and aborts execution.
 */
void sys_assert( const char *msg )
{
    ( void ) msg;
    /*FSL:only needed for debugging
    printf(msg);
    printf("\n\r");
    */
    rtos_critical_enter(RTOS_CRITICAL_LWIP);
    for(;;)
    ;
}

u32_t sys_now(void)
{
    return rtos_time_get_current_system_time_ms();
}

u32_t sys_jiffies(void)
{
    return rtos_time_get_current_system_time_ms();
}

#if LWIP_NETCONN_SEM_PER_THREAD
sys_sem_t *sys_thread_sem_init(void)
{
    sys_sem_t *sem = (sys_sem_t *)rtos_mem_malloc(sizeof(sys_sem_t));
    if (!sem) {
        return 0;
    }
    err_t err = sys_sem_new(sem, 0);
    if (err != ERR_OK) {
        rtos_mem_free(sem);
        return 0;
    }
    rtos_task_set_thread_local_storage_pointer(NULL, RTOS_LOCAL_STORAGE_LWIP_INDEX, (void *)sem);
    return sem;
}

sys_sem_t* sys_thread_sem_get(void)
{
    sys_sem_t *sem = (sys_sem_t *)rtos_task_get_thread_local_storage_pointer(NULL, RTOS_LOCAL_STORAGE_LWIP_INDEX);
    if (!sem) {
        sem = sys_thread_sem_init();
    }
    return sem;
}

void sys_thread_sem_deinit(void)
{
    sys_sem_t *sem = (sys_sem_t *)rtos_task_get_thread_local_storage_pointer(NULL, RTOS_LOCAL_STORAGE_LWIP_INDEX);
    if (sem != NULL) {
        sys_sem_free(sem);
        rtos_mem_free(sem);
        rtos_task_set_thread_local_storage_pointer(NULL, RTOS_LOCAL_STORAGE_LWIP_INDEX, NULL);
    }
}

void sys_thread_sem_deinit_tcb(uint32_t *pxTCB)
{
    sys_sem_t *sem = (sys_sem_t *)rtos_task_get_thread_local_storage_pointer((rtos_task_t)pxTCB, RTOS_LOCAL_STORAGE_LWIP_INDEX);
    if (sem != NULL) {
        sys_sem_free(sem);
        rtos_mem_free(sem);
        rtos_task_set_thread_local_storage_pointer((rtos_task_t)pxTCB, RTOS_LOCAL_STORAGE_LWIP_INDEX, NULL);
    }
}
#else
void sys_thread_sem_deinit_tcb(uint32_t *pxTCB)
{
    ( void ) pxTCB;
    return;
}
#endif