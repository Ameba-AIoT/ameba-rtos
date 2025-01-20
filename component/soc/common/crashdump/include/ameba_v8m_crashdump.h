/*
 * Copyright(c) 2020, Realtek Semiconductor Corp. All rights reserved.
 *
 * Author: CTC PSP Software
 *
 */

#ifndef _AMEBA_V8M_CRASHDUMP_H_
#define _AMEBA_V8M_CRASHDUMP_H_

extern struct cmb_hard_fault_regs regs;

typedef uint32_t (*crash_on_task)(uint32_t *reg, uint32_t src);

extern crash_on_task crash_task_info;

/**
 * @brief    Dump crash informtion
 *
 * @details  Dump registers exception handler stored and trigger stack backtrace
 *
 * @param    [in] pc: The program counter while crash
 *           [in] sp: The stack pointer while crash
 *           [in] reg: The pointer to memory which exception handler store the registers
 *
 * @return   NULL
 */
extern int crash_dump(uint32_t *pc, uint32_t *sp, uint32_t *reg);

extern uint32_t *vTaskStackAddr(void);

extern volatile uint32_t *vTaskStackTOPAddr(void);

extern char *vTaskName(void);

extern uint32_t vTaskStatus(uint32_t src);

extern uint32_t vTaskCrashCallback(uint32_t *reg, uint32_t src);

extern void fault_diagnosis(struct cmb_hard_fault_regs *regs);

#endif
