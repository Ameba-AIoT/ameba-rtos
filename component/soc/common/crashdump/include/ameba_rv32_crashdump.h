/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RV32_CRASHDUMP_H
#define RV32_CRASHDUMP_H

#ifdef __cplusplus
extern "C" {
#endif

// Maximum number of instructions check while backtrace
#define MAX_INSTR_CHECK (1024)

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
void crash_dump(uint32_t *pc,
				uint32_t *sp,
				uint32_t *reg);

extern uint32_t vTaskCrashCallback(uint32_t *reg, uint32_t src);

#ifdef __cplusplus
}
#endif

#endif
