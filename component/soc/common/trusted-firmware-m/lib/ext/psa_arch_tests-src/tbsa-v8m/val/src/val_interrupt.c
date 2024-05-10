/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#include "val_interrupt.h"
#include "val_interfaces.h"
#include "val_infra.h"
#include "val_test.h"
#include "pal_interfaces.h"

extern void tbsa_entry (void);

uint32_t        stack_top_dummy      = 0x0;
tbsa_isr_vector restore_handler_addr = NULL;

/**
    @brief   - This is the structure for secure vector table, containing default handlers
               in case an unexpected secure exception occurs.
**/
__attribute__((aligned(128))) tbsa_isr_vector g_tbsa_s_isr_vector[MAX_EXCP_NUM] =
{
    (tbsa_isr_vector)&stack_top_dummy,             /* Top of stack */
    /* System IRQs */
    (tbsa_isr_vector)&tbsa_entry,                  /* 01 */
    TBSA_S_NMI_Handler,                            /* 02 */
    TBSA_S_HardFault_Handler,                      /* 03 */
    TBSA_S_MemManage_Handler,                      /* 04 */
    TBSA_S_BusFault_Handler,                       /* 05 */
    TBSA_S_UsageFault_Handler,                     /* 06 */
    TBSA_S_SecureFault_Handler,                    /* 07 */
    TBSA_S_default_sys_handler,                    /* 08 */
    TBSA_S_default_sys_handler,                    /* 09 */
    TBSA_S_default_sys_handler,                    /* 10 */
    TBSA_S_SVC_Handler,                            /* 11 */
    TBSA_S_DebugMon_Handler,                       /* 12 */
    TBSA_S_default_sys_handler,                    /* 13 */
    TBSA_S_PendSV_Handler,                         /* 14 */
    TBSA_S_SysTick_Handler,                        /* 15 */

    /* NVIC IRQs : External Interrupts */
    [EXCP_NUM_EXT_INT(0) ... (MAX_EXCP_NUM - 1)] = TBSA_S_default_ext_int_handler
};

/**
    @brief   - This is the structure for non-secure vector table, containing default handlers
               in case an unexpected non-secure exception occurs.
**/
__attribute__((section(".tbsa_ns_data")))
__attribute__((aligned(128)))
tbsa_isr_vector g_tbsa_ns_isr_vector[MAX_EXCP_NUM] =
{
    NULL,
    /* System IRQs */
    NULL, /* 01 */
    TBSA_NS_default_sys_handler, /* 02 */
    TBSA_NS_default_sys_handler, /* 03 */
    TBSA_NS_default_sys_handler, /* 04 */
    TBSA_NS_default_sys_handler, /* 05 */
    TBSA_NS_default_sys_handler, /* 06 */
    TBSA_NS_default_sys_handler, /* 07 */
    TBSA_NS_default_sys_handler, /* 08 */
    TBSA_NS_default_sys_handler, /* 09 */
    TBSA_NS_default_sys_handler, /* 10 */
    TBSA_NS_default_sys_handler, /* 11 */
    TBSA_NS_default_sys_handler, /* 12 */
    TBSA_NS_default_sys_handler, /* 13 */
    TBSA_NS_default_sys_handler, /* 14 */
    TBSA_NS_default_sys_handler, /* 15 */

    /* NVIC IRQs : External Interrupts */
    [EXCP_NUM_EXT_INT(0) ... (MAX_EXCP_NUM - 1)] = TBSA_NS_default_sys_handler
};

/**
    @brief    - This is default handler for all non-secure exceptions
    @return   - none
**/
__attribute__((section(".tbsa_ns_text")))
void TBSA_NS_default_sys_handler (void)
{
    val_print_nsc(PRINT_ALWAYS, "\n\r\t---- Default NS Handler ----", 0);
    val_print_nsc(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure NMI
    @return   - none
**/
void TBSA_S_NMI_Handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- NMI Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure HardFault
    @return   - none
**/
void TBSA_S_HardFault_Handler (void)
{
    uint32_t hfsr;

    val_mem_reg_read(HFSR, &hfsr);
    val_print(PRINT_ALWAYS, "\n\r\t---- Hard Fault Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tHFSR\t\t: 0x%x", hfsr);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure MemManageFault
    @return   - none
**/
void TBSA_S_MemManage_Handler (void)
{
    uint32_t mmfsr, mmfar;
    val_print(PRINT_ALWAYS, "\n\r\t---- MemManage Fault Handler ----", 0);
    val_mem_reg_read(CFSR, &mmfsr);
    val_print(PRINT_ALWAYS, "\n\r\tMMFSR = 0x%x", mmfsr & 0xFF);
    val_mem_reg_read(MMFAR, &mmfar);
    val_print(PRINT_ALWAYS, "\n\r\tMMFAR = 0x%x", mmfar);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);

    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure BusFault
    @return   - none
**/
void TBSA_S_BusFault_Handler (void)
{
    uint32_t bfsr, bfar;
    val_print(PRINT_ALWAYS, "\n\r\t---- Bus Fault Handler ----", 0);
    val_mem_reg_read(CFSR, &bfsr);
    val_print(PRINT_ALWAYS, "\n\r\tBFSR = 0x%x", (bfsr >> 8) & 0xFF);
    val_mem_reg_read(BFAR, &bfar);
    val_print(PRINT_ALWAYS, "\n\r\tBFAR = 0x%x", bfar);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);

    while(1)
    {
    }
}


/**
    @brief    - This is default handler for secure UsageFault
    @return   - none
**/
void TBSA_S_UsageFault_Handler (void)
{
    uint32_t ufsr;
    val_print(PRINT_ALWAYS, "\n\r\t---- Usage Fault Handler ----", 0);
    val_mem_reg_read(CFSR, &ufsr);
    val_print(PRINT_ALWAYS, "\n\r\tUFSR = 0x%x", ufsr >> 16);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);

    while(1)
    {
    }
}


/**
    @brief    - This is default handler for SecureFault
    @return   - none
**/
void TBSA_S_SecureFault_Handler (void)
{
    uint32_t sf_dump;

    val_print(PRINT_ALWAYS, "\n\r\t---- Secure Fault Handler ----", 0);
    val_mem_reg_read(SFSR, &sf_dump);
    val_print(PRINT_ALWAYS, "\n\r\tSFSR\t\t: 0x%x", sf_dump);
    val_mem_reg_read(SFAR, &sf_dump);
    val_print(PRINT_ALWAYS, "\n\r\tSFAR\t\t: 0x%x", sf_dump);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}


/**
    @brief    - This is default handler for secure SVC exception
    @return   - none
**/
void TBSA_S_SVC_Handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- SVC Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure DebugMonitor exception
    @return   - none
**/
void TBSA_S_DebugMon_Handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- DebugMonitor Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure PendSV exception
    @return   - none
**/
void TBSA_S_PendSV_Handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- PendSV Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure SysTick exception
    @return   - none
**/
void TBSA_S_SysTick_Handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- SysTick Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure exception, not-defined in v8M architecture
    @return   - none
**/
void TBSA_S_default_sys_handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- Reserved Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - This is default handler for secure External Interrupt
    @return   - none
**/
void TBSA_S_default_ext_int_handler (void)
{
    val_print(PRINT_ALWAYS, "\n\r\t---- External Interrupt Handler ----", 0);
    val_print(PRINT_ALWAYS, "\n\r\tEntering standby\n", 0);
    while(1)
    {
    }
}

/**
    @brief    - Installs given IRQ handler into the vector table.
    @param    - excep_num : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
                handler   : handler to be installed for the excep_num
    @return   - error status
**/
tbsa_status_t val_interrupt_install_handler(uint32_t excep_num, tbsa_isr_vector handler)
{
    uint32_t vector_table_addr, tt_output, vtor_base;

    if (handler == NULL) {
        return TBSA_STATUS_INVALID_IRQ_HANDLER;
    }

    if ((excep_num <= EXCP_NUM_NMI) || (excep_num >= MAX_EXCP_NUM)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    /* Check whether called from S or NS test */
    tt_output = tt_asm((uint32_t *)handler);

    if (!(tt_output & BIT22)) {
        /* Target address is non-secure */
        vtor_base = VTOR_NS;
    } else {
        /* Target address is secure */
        vtor_base = VTOR;
    }

    /* Have a copy of default handler */
    val_mem_reg_read(vtor_base, &vector_table_addr);
    val_mem_read(((uint32_t *)((uint32_t *)vector_table_addr + excep_num)), WORD, (uint32_t *)&restore_handler_addr);

    val_mem_write(((uint32_t *)((uint32_t *)vector_table_addr + excep_num)), WORD, (uint32_t)handler);

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Restores the default handler
    @param    - excep_num : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
    @return   - error status
**/
tbsa_status_t val_interrupt_restore_handler(uint32_t excep_num)
{
    uint32_t vector_table_addr, tt_output, vtor_base;
    tbsa_status_t status;

    if ((excep_num <= EXCP_NUM_NMI) || (excep_num >= MAX_EXCP_NUM)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    /* Check whether called from S or NS test */
    tt_output = tt_asm((uint32_t *)restore_handler_addr);

    if (!(tt_output & BIT22)) {
        /* Target address is non-secure */
        vtor_base = VTOR_NS;
    } else {
        /* Target address is secure */
        vtor_base = VTOR;
    }

    /* Restore the default handler in vector table */
    val_mem_reg_read(vtor_base, &vector_table_addr);
    val_mem_write(((uint32_t *)((uint32_t *)vector_table_addr + excep_num)), WORD, (uint32_t)restore_handler_addr);

    if (excep_num >= EXCP_NUM_EXT_INT(0)) {
        status = val_interrupt_disable(excep_num);
        if(TBSA_STATUS(status))
            return status;
    }

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This API will enable a specific interrupt in NVIC
    @param    - intr_num   : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
    @return   - error status
**/
tbsa_status_t val_interrupt_enable(uint32_t intr_num)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    pal_NVIC_EnableIRQ(intr_num);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Disable a given interrupt in NVIC
    @param    - intr_num   : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
    @return   - error status
**/
tbsa_status_t val_interrupt_disable(uint32_t intr_num)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    pal_NVIC_DisableIRQ(intr_num);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Route the interrupt to given security target
    @param    - intr_num  : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
                tgt_state : security state to target the interrupt to
    @return   - error status
**/
tbsa_status_t val_interrupt_route(uint32_t intr_num, cpu_state_t tgt_state)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);

    if(tgt_state == PE_SECURE) {
        pal_NVIC_ClearTargetState(intr_num);
    } else {
        pal_NVIC_SetTargetState(intr_num);
    }
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Sets the priority for a given interrupt
    @param    - intr_num  : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
                priority  : priority which interrupt must be programmed to
    @return   - error status
**/
tbsa_status_t val_interrupt_set_priority(uint32_t intr_num, uint32_t priority)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    pal_NVIC_SetPriority(intr_num, priority);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Updates the pending bit status for a given interrupt
    @param    - intr_num  : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
                status    : 1 - if interrupt is pending
                            0 - if interrupt is not pending
    @return   - error status
**/
tbsa_status_t val_interrupt_get_pending_status(uint32_t intr_num, uint32_t *status)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    *status = pal_NVIC_GetPendingIRQ(intr_num);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Updates the active bit status for a given interrupt
    @param    - intr_num  : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
                status    : 1 - if interrupt is active
                            0 - if interrupt is not active
    @return   - error status
**/
tbsa_status_t val_interrupt_get_active_status(uint32_t intr_num, uint32_t *status)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    *status = pal_NVIC_GetActive(intr_num);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Sets the pending bit for a given interrupt
    @param    - intr_num  : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
    @return   - error status
**/
tbsa_status_t val_interrupt_set_pending(uint32_t intr_num)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    pal_NVIC_SetPendingIRQ(intr_num);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Clear the pending bit for a given interrupt
    @param    - intr_num  : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
    @return   - error status
**/
tbsa_status_t val_interrupt_clear_pending(uint32_t intr_num)
{
    if(!IS_INTR_NUM_VALID(intr_num)) {
        return TBSA_STATUS_INVALID_IRQ_NUMBER;
    }

    intr_num -= EXCP_NUM_EXT_INT(0);
    pal_NVIC_ClearPendingIRQ(intr_num);
    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - Sets the priority and the handler installation for a given interrupt
    @param    - excep_num : Exception number of the interrupt, it is
                            {int_id + EXCP_NUM_EXT_INT(0)} for v8M architecture
                priority  : priority which interrupt must be programmed to
                handler   : handler to be installed for the excep_num
    @return   - error status
**/
tbsa_status_t val_interrupt_setup_handler(uint32_t excep_num, uint32_t priority, tbsa_isr_vector handler)
{
    tbsa_status_t status;
    status = val_interrupt_install_handler(excep_num, handler);
    if(TBSA_STATUS(status))
        return status;

    if (excep_num >= EXCP_NUM_EXT_INT(0)) {
        status = val_interrupt_set_priority(excep_num, priority);
        if(TBSA_STATUS(status))
            return status;

        status = val_interrupt_enable(excep_num);
        if(TBSA_STATUS(status))
            return status;

    }
    return TBSA_STATUS_SUCCESS;
}
