/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RV32_CRASHDUMP_H
#define RV32_CRASHDUMP_H

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

#endif
