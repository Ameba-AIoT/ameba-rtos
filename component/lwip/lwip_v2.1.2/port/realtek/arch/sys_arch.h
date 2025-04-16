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
#ifndef __SYS_RTXC_H__
#define __SYS_RTXC_H__

#include "os_wrapper.h"

#define SYS_MBOX_NULL (rtos_queue_t)0
#define SYS_SEM_NULL  (rtos_sema_t)0

typedef rtos_sema_t sys_sem_t;
typedef rtos_mutex_t sys_mutex_t;
typedef rtos_queue_t sys_mbox_t;
typedef rtos_task_t sys_thread_t;

typedef struct _sys_arch_state_t
{
    // Task creation data.
    char cTaskName[24];
    unsigned short nStackDepth;
    unsigned short nTaskCount;
} sys_arch_state_t;



//extern sys_arch_state_t s_sys_arch_state;

//void sys_set_default_state();
//void sys_set_state(signed char *pTaskName, unsigned short nStackSize);

/* Message queue constants. */
#define archMESG_QUEUE_LENGTH    ( 6 )

#define LWIP_INDEX               ( 0 )
sys_sem_t *sys_thread_sem_get(void);
sys_sem_t *sys_thread_sem_init(void);
void sys_thread_sem_deinit(void);

#endif /* __SYS_RTXC_H__ */

