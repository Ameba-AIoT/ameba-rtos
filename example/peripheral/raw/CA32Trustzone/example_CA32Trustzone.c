/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */

/**
 * @file example_CA32Trustzone.c
 * @brief CA32 Trustzone secure service invocation example
 *
 * This example demonstrates how to invoke secure world services from the
 * Non-Secure World via SMC (Secure Monitor Call).
 *
 * Secure service implementation:
 * component/soc/amebasmart/atf/plat/realtek/sheipa/service/bsec_svc.c
 */

#include "ameba_soc.h"

/* ARM SMCCC call result structure */
struct arm_smccc_res {
	unsigned long a0;
	unsigned long a1;
	unsigned long a2;
	unsigned long a3;
};

struct arm_smccc_quirk {
	int id;
	union {
		unsigned long a6;
	} state;
};

extern void __arm_smccc_smc(unsigned long a0, unsigned long a1,
							unsigned long a2, unsigned long a3, unsigned long a4,
							unsigned long a5, unsigned long a6, unsigned long a7,
							struct arm_smccc_res *res, struct arm_smccc_quirk *quirk);

#define arm_smccc_smc(...) __arm_smccc_smc(__VA_ARGS__, NULL)

/* Invoke secure world test service via SMC call */
static unsigned long invoke_securetest(unsigned long function_id, unsigned long arg0, unsigned long arg1, unsigned long arg2)
{
	struct arm_smccc_res res;

	arm_smccc_smc(function_id, arg0, arg1, arg2, 0, 0, 0, 0, &res);

	printf("value: %lu 0x%lx %lu\n", res.a0, res.a1, res.a2);

	return res.a0;
}

int example_raw_ap_tz(void)
{
	/* Invoke secure world test service with Function ID 0x82000001 */
	invoke_securetest(0x82000001, 0, 0, 0);

	return 0;
}
