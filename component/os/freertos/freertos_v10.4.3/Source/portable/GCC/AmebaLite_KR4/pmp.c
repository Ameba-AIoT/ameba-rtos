/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include "pmp.h"

/**
 * @brief get the mxlen from misa register
 *
 */
static __attribute__((naked, unused)) int32_t get_mxlen(void)
{
	__asm__ __volatile__(" csrr a0, misa \n"    /* get misa reg. */
						 " bgt a0, zero, 1f \n" /* check if > 0 */
						 " slli a1 , a0, 1 \n"  /* shift left of 1 bit */
						 " blt a1, zero, 2f \n" /* check if < 0 */
						 " li a0, 64 \n"        /* else mxlen = 32 */
						 " ret \n"
						 "1: \n"
						 " li a0, 32 \n"
						 " ret \n"
						 "2: \n"
						 " li a0, 128 \n"
						 " ret \n" ::
						 : "a0", "a1");
}

static int32_t _get_detected_granularity(size_t address)
{
	if (0 == address) {
		return PMP_INVALID_PARAM;
	}

	/* Get the index of the least significant set bit */
	int index = 0;
	while (((address >> index) & 0x1) == 0) {
		index += 1;
	}

	/* The granularity is equal to 2^(index + 2) bytes */
	return (1 << (index + 2));
}

/**
 * @brief initialize pmp, basically compute pmp granularity and number of region
 *
 * @param[out]  pmp_info    info on pmp
 * @return int32_t
 */
int32_t init_pmp(pmp_info_t *pmp_info)
{
	int32_t result = PMP_DEFAULT_ERROR;
	size_t i = 0;
	uint8_t pmp_config = 0;
	size_t address = 0;

	if (NULL == pmp_info) {
		return (PMP_INVALID_POINTER);
	}

	pmp_info->nb_pmp = MAX_PMP_REGION;

	for (i = 0; i < MAX_PMP_REGION; i++) {

		pmp_config = 0;
		address = 0;
		result = write_pmp_config(pmp_info, i, pmp_config, address);
		if (PMP_SUCCESS > result) {
			return (result);
		}

		address = -1;
		result = write_pmp_config(pmp_info, i, pmp_config, address);
		if (PMP_SUCCESS > result) {
			return (result);
		}

		result = read_pmp_config(pmp_info, i, &pmp_config, &address);
		if (PMP_SUCCESS > result) {
			return (result);
		}

		/* Compute granularity */
		if (0 == i) {
			result = _get_detected_granularity(address);
			if (PMP_SUCCESS > result) {
				return (result);
			}
			pmp_info->granularity = result;
		}

		/* Check number of active region */
		if (0 == address) {
			pmp_info->nb_pmp = i;
			break;
		}
	}

	return (PMP_SUCCESS);
}

int32_t set_pmp_config(pmp_cfg_t *config, uint8_t *register_val)
{
	uint8_t reg_temp = 0;

	if ((NULL == config) || (NULL == register_val)) {
		return (PMP_INVALID_POINTER);
	}

	if (true == config->R) {
		reg_temp |= 1 << PMP_READ_RIGHT_OFFSET;
	}

	if (true == config->W) {
		reg_temp |= 1 << PMP_WRITE_RIGHT_OFFSET;
	}

	if (true == config->X) {
		reg_temp |= 1 << PMP_EXECUTE_RIGHT_OFFSET;
	}

	if (true == config->L) {
		reg_temp |= 1 << PMP_LOCK_RIGHT_OFFSET;
	}

	switch (config->A) {
	case PMP_OFF:
	case PMP_TOR:
	case PMP_NA4:
	case PMP_NAPOT:
		reg_temp |= config->A << PMP_ADDRESS_RIGHT_OFFSET;
		break;
	default:
		return (PMP_INVALID_PARAM);
	}

	*register_val = reg_temp;

	return (PMP_SUCCESS);
}

int32_t get_pmp_config(uint8_t *register_val, pmp_cfg_t *config)
{
	if ((NULL == config) || (NULL == register_val)) {
		return (PMP_INVALID_POINTER);
	}

	config->R = *register_val & PMP_CFG_1_BIT_MASK;
	config->W = (*register_val >> PMP_WRITE_RIGHT_OFFSET) & PMP_CFG_1_BIT_MASK;
	config->X =
		(*register_val >> PMP_EXECUTE_RIGHT_OFFSET) & PMP_CFG_1_BIT_MASK;
	config->A =
		(*register_val >> PMP_ADDRESS_RIGHT_OFFSET) & PMP_CFG_2_BIT_MASK;
	config->L = (*register_val >> PMP_LOCK_RIGHT_OFFSET) & PMP_CFG_1_BIT_MASK;

	return (PMP_SUCCESS);
}

int32_t write_pmp_config(pmp_info_t *pmp_info, uint32_t region,
						 uint8_t pmp_config, size_t address)
{
	uint32_t bit_shift = 0;

	/* check pointer not null */
	if (NULL == pmp_info) {
		return (PMP_INVALID_POINTER);
	}

	/* check region */
	if (region >= pmp_info->nb_pmp) {
		return (PMP_INVALID_PARAM);
	}

	switch (region) {
	case 0:
		__asm__ __volatile__("csrw pmpaddr0, %[addr]" ::[addr] "r"(address) :);
		break;
	case 1:
		__asm__ __volatile__("csrw pmpaddr1, %[addr]" ::[addr] "r"(address) :);
		break;
	case 2:
		__asm__ __volatile__("csrw pmpaddr2, %[addr]" ::[addr] "r"(address) :);
		break;
	case 3:
		__asm__ __volatile__("csrw pmpaddr3, %[addr]" ::[addr] "r"(address) :);
		break;
	case 4:
		__asm__ __volatile__("csrw pmpaddr4, %[addr]" ::[addr] "r"(address) :);
		break;
	case 5:
		__asm__ __volatile__("csrw pmpaddr5, %[addr]" ::[addr] "r"(address) :);
		break;
	case 6:
		__asm__ __volatile__("csrw pmpaddr6, %[addr]" ::[addr] "r"(address) :);
		break;
	case 7:
		__asm__ __volatile__("csrw pmpaddr7, %[addr]" ::[addr] "r"(address) :);
		break;
	case 8:
		__asm__ __volatile__("csrw pmpaddr8, %[addr]" ::[addr] "r"(address) :);
		break;
	case 9:
		__asm__ __volatile__("csrw pmpaddr9, %[addr]" ::[addr] "r"(address) :);
		break;
	case 10:
		__asm__ __volatile__("csrw pmpaddr10, %[addr]" ::[addr] "r"(address) :);
		break;
	case 11:
		__asm__ __volatile__("csrw pmpaddr11, %[addr]" ::[addr] "r"(address) :);
		break;
	case 12:
		__asm__ __volatile__("csrw pmpaddr12, %[addr]" ::[addr] "r"(address) :);
		break;
	case 13:
		__asm__ __volatile__("csrw pmpaddr13, %[addr]" ::[addr] "r"(address) :);
		break;
	case 14:
		__asm__ __volatile__("csrw pmpaddr14, %[addr]" ::[addr] "r"(address) :);
		break;
	case 15:
		__asm__ __volatile__("csrw pmpaddr15, %[addr]" ::[addr] "r"(address) :);
		break;
	}

#if __riscv_xlen == 32
	bit_shift = (region % 4) << 3;
	switch (region / 4) {
	case 0:
		__asm__ __volatile__("li t1, 0xFF\n"
							 "sll t1, t1, %[bit_shift] \n"
							 "sll t2, %[cfg], %[bit_shift] \n"
							 "csrc pmpcfg0, t1 \n"
							 "csrs pmpcfg0, t2 \n"
							 :
							 : [ bit_shift ] "r"(bit_shift),
							 [ cfg ] "r"(pmp_config)
							 : "t1", "t2");
		break;
	case 1:
		__asm__ __volatile__("li t1, 0xFF\n"
							 "sll t1, t1, %[bit_shift] \n"
							 "sll t2, %[cfg], %[bit_shift] \n"
							 "csrc pmpcfg1, t1 \n"
							 "csrs pmpcfg1, t2 \n"
							 :
							 : [ bit_shift ] "r"(bit_shift),
							 [ cfg ] "r"(pmp_config)
							 : "t1", "t2");
		break;
	case 2:
		__asm__ __volatile__("li t1, 0xFF\n"
							 "sll t1, t1, %[bit_shift] \n"
							 "sll t2, %[cfg], %[bit_shift] \n"
							 "csrc pmpcfg2, t1 \n"
							 "csrs pmpcfg2, t2 \n"
							 :
							 : [ bit_shift ] "r"(bit_shift),
							 [ cfg ] "r"(pmp_config)
							 : "t1", "t2");
		break;
	case 3:
		__asm__ __volatile__("li t1, 0xFF\n"
							 "sll t1, t1, %[bit_shift] \n"
							 "sll t2, %[cfg], %[bit_shift] \n"
							 "csrc pmpcfg3, t1 \n"
							 "csrs pmpcfg3, t2 \n"
							 :
							 : [ bit_shift ] "r"(bit_shift),
							 [ cfg ] "r"(pmp_config)
							 : "t1", "t2");
		break;
	}
#elif __riscv_xlen == 64
	bit_shift = (region % 8) << 3;
	switch (region / 8) {
	case 0:
		__asm__ __volatile__("li t1, 0xFF\n"
							 "sll t1, t1, %[bit_shift] \n"
							 "sll t2, %[cfg], %[bit_shift] \n"
							 "csrc pmpcfg0, t1 \n"
							 "csrs pmpcfg0, t2 \n"
							 :
							 : [ bit_shift ] "r"(bit_shift),
							 [ cfg ] "r"(pmp_config)
							 : "t1", "t2");
		break;
	case 1:
		__asm__ __volatile__("li t1, 0xFF\n"
							 "sll t1, t1, %[bit_shift] \n"
							 "sll t2, %[cfg], %[bit_shift] \n"
							 "csrc pmpcfg2, t1 \n"
							 "csrs pmpcfg2, t2 \n"
							 :
							 : [ bit_shift ] "r"(bit_shift),
							 [ cfg ] "r"(pmp_config)
							 : "t1", "t2");
		break;
	}
#else
	return (PMP_ERR_UNSUPPORTED);
#endif

	return (PMP_SUCCESS);
}

int32_t read_pmp_config(pmp_info_t *pmp_info, uint32_t region,
						uint8_t *pmp_config, size_t *address)
{
	uint8_t pmp_config_temp = 0;

	if ((NULL == pmp_info) || (NULL == pmp_config) || (NULL == address)) {
		return (PMP_INVALID_POINTER);
	}

	/* check region */
	if (region >= pmp_info->nb_pmp) {
		return (PMP_INVALID_PARAM);
	}

#if __riscv_xlen == 32
	switch (region / 4) {
	case 0:
		__asm__ __volatile__("csrr %[cfg], pmpcfg0"
							 : [ cfg ] "=r"(pmp_config_temp)::);
		break;
	case 1:
		__asm__ __volatile__("csrr %[cfg], pmpcfg1"
							 : [ cfg ] "=r"(pmp_config_temp)::);
		break;
	case 2:
		__asm__ __volatile__("csrr %[cfg], pmpcfg2"
							 : [ cfg ] "=r"(pmp_config_temp)::);
		break;
	case 3:
		__asm__ __volatile__("csrr %[cfg], pmpcfg3"
							 : [ cfg ] "=r"(pmp_config_temp)::);
		break;
	}

	*pmp_config = (0xFF & (pmp_config_temp >> (8 * (region % 4))));
#elif __riscv_xlen == 64
	switch (region / 8) {
	case 0:
		__asm__ __volatile__("csrr %[cfg], pmpcfg0"
							 : [ cfg ] "=r"(pmp_config_temp)::);
		break;
	case 1:
		__asm__ __volatile__("csrr %[cfg], pmpcfg2"
							 : [ cfg ] "=r"(pmp_config_temp)::);
		break;
	}

	*pmp_config = (0xFF & (pmp_config_temp >> (8 * (region % 8))));
#else
	return (PMP_ERR_UNSUPPORTED);
#endif

	switch (region) {
	case 0:
		__asm__ __volatile__("csrr %[addr], pmpaddr0"
							 : [ addr ] "=r"(*address)::);
		break;
	case 1:
		__asm__ __volatile__("csrr %[addr], pmpaddr1"
							 : [ addr ] "=r"(*address)::);
		break;
	case 2:
		__asm__ __volatile__("csrr %[addr], pmpaddr2"
							 : [ addr ] "=r"(*address)::);
		break;
	case 3:
		__asm__ __volatile__("csrr %[addr], pmpaddr3"
							 : [ addr ] "=r"(*address)::);
		break;
	case 4:
		__asm__ __volatile__("csrr %[addr], pmpaddr4"
							 : [ addr ] "=r"(*address)::);
		break;
	case 5:
		__asm__ __volatile__("csrr %[addr], pmpaddr5"
							 : [ addr ] "=r"(*address)::);
		break;
	case 6:
		__asm__ __volatile__("csrr %[addr], pmpaddr6"
							 : [ addr ] "=r"(*address)::);
		break;
	case 7:
		__asm__ __volatile__("csrr %[addr], pmpaddr7"
							 : [ addr ] "=r"(*address)::);
		break;
	case 8:
		__asm__ __volatile__("csrr %[addr], pmpaddr8"
							 : [ addr ] "=r"(*address)::);
		break;
	case 9:
		__asm__ __volatile__("csrr %[addr], pmpaddr9"
							 : [ addr ] "=r"(*address)::);
		break;
	case 10:
		__asm__ __volatile__("csrr %[addr], pmpaddr10"
							 : [ addr ] "=r"(*address)::);
		break;
	case 11:
		__asm__ __volatile__("csrr %[addr], pmpaddr11"
							 : [ addr ] "=r"(*address)::);
		break;
	case 12:
		__asm__ __volatile__("csrr %[addr], pmpaddr12"
							 : [ addr ] "=r"(*address)::);
		break;
	case 13:
		__asm__ __volatile__("csrr %[addr], pmpaddr13"
							 : [ addr ] "=r"(*address)::);
		break;
	case 14:
		__asm__ __volatile__("csrr %[addr], pmpaddr14"
							 : [ addr ] "=r"(*address)::);
		break;
	case 15:
		__asm__ __volatile__("csrr %[addr], pmpaddr15"
							 : [ addr ] "=r"(*address)::);
		break;
	}

	return (PMP_SUCCESS);
}

int32_t read_pmp_configs(size_t *configs, uint32_t pmp_region)
{
	size_t jump_idx = 0;

	if (NULL == configs) {
		return (PMP_INVALID_POINTER);
	}

	jump_idx = (NB_PMP_CFG_REG - (pmp_region / SIZE_PMP_CFG_REG)) << 3;

#if __riscv_xlen == 32
	__asm__ __volatile__("la t0, 1f \n"
						 "add t0, t0, %[jump_idx] \n"
						 "jr t0 \n"
						 "1: \n"
						 "csrr t1, pmpcfg3 \n"
						 "sw t1, 12(%[configs]) \n"
						 "csrr t0, pmpcfg2 \n"
						 "sw t0, 8(%[configs]) \n"
						 "csrr t1, pmpcfg1 \n"
						 "sw t1, 4(%[configs]) \n"
						 "csrr t0, pmpcfg0 \n"
						 "sw t0, 0(%[configs]) \n"
						 :
						 : [ configs ] "r"(configs), [ jump_idx ] "r"(jump_idx)
						 : "t0", "t1");
#elif __riscv_xlen == 64
	__asm__ __volatile__("la t0, 1f \n"
						 "add t0, t0, %[jump_idx] \n"
						 "jr t0 \n"
						 "1: \n"
						 "csrr t2, pmpcfg2 \n"
						 "sd t2, 8(%[configs]) \n"
						 "csrr t1, pmpcfg0 \n"
						 "sd t1, 0(%[configs]) \n"
						 :
						 : [ configs ] "r"(configs), [ jump_idx ] "r"(jump_idx)
						 : "t1", "t2");
#else
	return (PMP_ERR_UNSUPPORTED);
#endif

	return (PMP_SUCCESS);
}

int32_t write_pmp_configs(size_t *configs, uint32_t pmp_region)
{
	size_t jump_idx = 0;

	if (NULL == configs) {
		return (PMP_INVALID_POINTER);
	}

	jump_idx = (NB_PMP_CFG_REG - (pmp_region / SIZE_PMP_CFG_REG)) << 3;

#if __riscv_xlen == 32
	__asm__ __volatile__("la t0, 1f \n"
						 "add t0, t0, %[jump_idx] \n"
						 "jr t0 \n"
						 "1: \n"
						 "lw t1, 12(%[configs]) \n"
						 "csrw pmpcfg3, t1 \n"
						 "lw t0, 8(%[configs]) \n"
						 "csrw pmpcfg2, t0 \n"
						 "lw t1, 4(%[configs]) \n"
						 "csrw pmpcfg1, t1 \n"
						 "lw t0, 0(%[configs]) \n"
						 "csrw pmpcfg0, t0 \n"
						 :
						 : [ configs ] "r"(configs), [ jump_idx ] "r"(jump_idx)
						 : "t0", "t1");
#elif __riscv_xlen == 64
	__asm__ __volatile__("la t0, 1f \n"
						 "add t0, t0, %[jump_idx] \n"
						 "jr t0 \n"
						 "1: \n"
						 "ld t2, 8(%[configs]) \n"
						 "csrw pmpcfg2, t2 \n"
						 "ld t1, 0(%[configs]) \n"
						 "csrw pmpcfg0, t1 \n"
						 :
						 : [ configs ] "r"(configs), [ jump_idx ] "r"(jump_idx)
						 : "t0", "t1", "t2");
#else
	return (PMP_ERR_UNSUPPORTED);
#endif

	return (PMP_SUCCESS);
}

int32_t write_pmp_addrs(size_t *address, uint32_t pmp_region)
{
	size_t inst_offset = 0;

	if (NULL == address) {
		return (PMP_INVALID_POINTER);
	}

	if (MAX_PMP_REGION < pmp_region) {
		return (PMP_INVALID_PARAM);
	}

#if __riscv_xlen == 32
	inst_offset = (MAX_PMP_REGION - pmp_region) << 3;
	__asm__ __volatile__("la t1, 1f \n"
						 "add t2, t1, %[inst_offset] \n"
						 "jr t2 \n"
						 "1: \n"
						 "lw t0, 60(%[address]) \n"
						 "csrw pmpaddr15, t0 \n"
						 "lw t1, 56(%[address]) \n"
						 "csrw pmpaddr14, t1 \n"
						 "lw t2, 52(%[address]) \n"
						 "csrw pmpaddr13, t2 \n"
						 "lw t0, 48(%[address]) \n"
						 "csrw pmpaddr12, t0 \n"
						 "lw t1, 44(%[address]) \n"
						 "csrw pmpaddr11, t1 \n"
						 "lw t2, 40(%[address]) \n"
						 "csrw pmpaddr10, t2 \n"
						 "lw t0, 36(%[address]) \n"
						 "csrw pmpaddr9, t0 \n"
						 "lw t1, 32(%[address]) \n"
						 "csrw pmpaddr8, t1 \n"
						 "lw t2, 28(%[address]) \n"
						 "csrw pmpaddr7, t2 \n"
						 "lw t0, 24(%[address]) \n"
						 "csrw pmpaddr6, t0 \n"
						 "lw t1, 20(%[address]) \n"
						 "csrw pmpaddr5, t1 \n"
						 "lw t2, 16(%[address]) \n"
						 "csrw pmpaddr4, t2 \n"
						 "lw t0, 12(%[address]) \n"
						 "csrw pmpaddr3, t0 \n"
						 "lw t1, 8(%[address]) \n"
						 "csrw pmpaddr2, t1 \n"
						 "lw t2, 4(%[address]) \n"
						 "csrw pmpaddr1, t2 \n"
						 "lw t0, 0(%[address]) \n"
						 "csrw pmpaddr0, t0 \n"
						 :
						 : [ address ] "r"(address),
						 [ inst_offset ] "r"(inst_offset)
						 : "t0", "t1", "t2");
#elif __riscv_xlen == 64
	inst_offset = (MAX_PMP_REGION - pmp_region) << 3;

	__asm__ __volatile__("la t1, 1f \n"
						 "add t2, t1, %[inst_offset] \n"
						 "jr t2 \n"
						 "1: \n"
						 "ld t4, 120(%[address]) \n"
						 "csrw pmpaddr15, t4 \n"
						 "ld t3, 112(%[address]) \n"
						 "csrw pmpaddr14, t3 \n"
						 "ld t2, 104(%[address]) \n"
						 "csrw pmpaddr13, t2 \n"
						 "ld t1, 96(%[address]) \n"
						 "csrw pmpaddr12, t1 \n"
						 "ld t4, 88(%[address]) \n"
						 "csrw pmpaddr11, t4 \n"
						 "ld t3, 80(%[address]) \n"
						 "csrw pmpaddr10, t3 \n"
						 "ld t2, 72(%[address]) \n"
						 "csrw pmpaddr9, t2 \n"
						 "ld t1, 64(%[address]) \n"
						 "csrw pmpaddr8, t1 \n"
						 "ld t4, 56(%[address]) \n"
						 "csrw pmpaddr7, t4 \n"
						 "ld t3, 48(%[address]) \n"
						 "csrw pmpaddr6, t3 \n"
						 "ld t2, 40(%[address]) \n"
						 "csrw pmpaddr5, t2 \n"
						 "ld t1, 32(%[address]) \n"
						 "csrw pmpaddr4, t1 \n"
						 "ld t4, 24(%[address]) \n"
						 "csrw pmpaddr3, t4 \n"
						 "ld t3, 16(%[address]) \n"
						 "csrw pmpaddr2, t3 \n"
						 "ld t2, 8(%[address]) \n"
						 "csrw pmpaddr1, t2 \n"
						 "ld t1, 0(%[address]) \n"
						 "csrw pmpaddr0, t1 \n"
						 :
						 : [ address ] "r"(address),
						 [ inst_offset ] "r"(inst_offset)
						 : "t1", "t2", "t3", "t4");
#else
	return (PMP_ERR_UNSUPPORTED);
#endif

	return (PMP_SUCCESS);
}

int32_t read_pmp_addrs(size_t *address, uint32_t pmp_region)
{
	uint32_t inst_offset = 0;

	if (NULL == address) {
		return (PMP_INVALID_POINTER);
	}

	if (MAX_PMP_REGION < pmp_region) {
		return (PMP_INVALID_PARAM);
	}

#if __riscv_xlen == 32
	inst_offset = (MAX_PMP_REGION - pmp_region) << 3;
	__asm__ __volatile__("la t1, 1f \n"
						 "add t2, t1, %[inst_offset] \n"
						 "jr t2 \n"
						 "1: \n"
						 "csrr t0, pmpaddr15 \n"
						 "sw t0, 60(%[address]) \n"
						 "csrr t1, pmpaddr14 \n"
						 "sw t1, 56(%[address]) \n"
						 "csrr t2, pmpaddr13 \n"
						 "sw t2, 52(%[address]) \n"
						 "csrr t0, pmpaddr12 \n"
						 "sw t0, 48(%[address]) \n"
						 "csrr t1, pmpaddr11 \n"
						 "sw t1, 44(%[address]) \n"
						 "csrr t2, pmpaddr10 \n"
						 "sw t2, 40(%[address]) \n"
						 "csrr t0, pmpaddr9 \n"
						 "sw t0, 36(%[address]) \n"
						 "csrr t1, pmpaddr8 \n"
						 "sw t1, 32(%[address]) \n"
						 "csrr t2, pmpaddr7 \n"
						 "sw t2, 28(%[address]) \n"
						 "csrr t0, pmpaddr6 \n"
						 "sw t0, 24(%[address]) \n"
						 "csrr t1, pmpaddr5 \n"
						 "sw t1, 20(%[address]) \n"
						 "csrr t2, pmpaddr4 \n"
						 "sw t2, 16(%[address]) \n"
						 "csrr t0, pmpaddr3 \n"
						 "sw t0, 12(%[address]) \n"
						 "csrr t1, pmpaddr2 \n"
						 "sw t1, 8(%[address]) \n"
						 "csrr t0, pmpaddr1 \n"
						 "sw t0, 4(%[address]) \n"
						 "csrr t1, pmpaddr0 \n"
						 "sw t1, 0(%[address]) \n"
						 :
						 : [ address ] "r"(address),
						 [ inst_offset ] "r"(inst_offset)
						 : "t0", "t1", "t2");
#elif __riscv_xlen == 64
	inst_offset = (MAX_PMP_REGION - pmp_region) << 3;

	__asm__ __volatile__("la t1, 1f \n"
						 "add t2, t1, %[inst_offset] \n"
						 "jr t2 \n"
						 "1: \n"
						 "csrr t4, pmpaddr15 \n"
						 "sd t4, 120(%[address]) \n"
						 "csrr t3, pmpaddr14 \n"
						 "sd t3, 112(%[address]) \n"
						 "csrr t2, pmpaddr13 \n"
						 "sd t2, 104(%[address]) \n"
						 "csrr t1, pmpaddr12 \n"
						 "sd t1, 96(%[address]) \n"
						 "csrr t4, pmpaddr11 \n"
						 "sd t4, 88(%[address]) \n"
						 "csrr t3, pmpaddr10 \n"
						 "sd t3, 80(%[address]) \n"
						 "csrr t2, pmpaddr9 \n"
						 "sd t2, 72(%[address]) \n"
						 "csrr t1, pmpaddr8 \n"
						 "sd t1, 64(%[address]) \n"
						 "csrr t4, pmpaddr7 \n"
						 "sd t4, 56(%[address]) \n"
						 "csrr t3, pmpaddr6 \n"
						 "sd t3, 48(%[address]) \n"
						 "csrr t2, pmpaddr5 \n"
						 "sd t2, 40(%[address]) \n"
						 "csrr t1, pmpaddr4 \n"
						 "sd t1, 32(%[address]) \n"
						 "csrr t4, pmpaddr3 \n"
						 "sd t4, 24(%[address]) \n"
						 "csrr t3, pmpaddr2 \n"
						 "sd t3, 16(%[address]) \n"
						 "csrr t2, pmpaddr1 \n"
						 "sd t2, 8(%[address]) \n"
						 "csrr t1, pmpaddr0 \n"
						 "sd t1, 0(%[address]) \n"
						 :
						 : [ address ] "r"(address),
						 [ inst_offset ] "r"(inst_offset)
						 : "t1", "t2", "t3", "t4");
#else
	return (PMP_ERR_UNSUPPORTED);
#endif

	return (PMP_SUCCESS);
}

int32_t addr_modifier(size_t granularity, size_t address_in,
					  size_t *address_out)
{

	if (NULL == address_out) {
		return (PMP_INVALID_POINTER);
	}

	if (0 != address_in % granularity) {
		return (PMP_UNALIGNED_ADDRESS);
	}

	*address_out = address_in >> 2;

	return (PMP_SUCCESS);
}

int32_t napot_addr_modifier(size_t granularity, size_t address_in,
							size_t *address_out, size_t size)
{
	size_t index = 0;

	if (NULL == address_out) {
		return (PMP_INVALID_POINTER);
	}

	// check size is power of 2 and superior or equal to 4
	if (granularity > size) {
		return (PMP_ERR_GRANULARITY);
	}

	if (0 != address_in % granularity) {
		return (PMP_UNALIGNED_ADDRESS);
	}

	// in case the whole adressable space is requested
	if ((size_t) -1 == size) {
		size = size >> 1;
		size += 1;
		size = size >> 1;
	} else {
		size = size >> 2;
	}

	address_in = address_in >> 2;

	/* check if size is a power of 2 */
	for (index = 0; index < sizeof(size_t); index++) {
		if (0 != ((size >> index) & 1)) {
			if (index + 1 == sizeof(size_t)) {
				break;
			} else if (0 == (size >> (index + 1))) {
				break;
			} else {
				return (PMP_INVALID_NAPOT_SIZE);
			}
		}
	}

	/* check alignment between size and address */
	if (0 != address_in % size) {
		return (PMP_UNALIGNED_ADDRESS);
	}

	*address_out = address_in | ((size - 1) >> 1);

	return (PMP_SUCCESS);
}
