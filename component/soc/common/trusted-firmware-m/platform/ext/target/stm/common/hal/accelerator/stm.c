/*
 * Copyright (c) 2019-2020, Arm Limited. All rights reserved.
 * Copyright (c) 2021 STMicroelectronics. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "crypto_hw.h"
#if defined(GENERATOR_HW_CRYPTO_DPA_SUPPORTED)
#include "stm32hal.h"
#endif
#ifdef PSA_USE_SE_ST
extern int se_st_engine_init(void);/* fixme include se api.h */
#endif

/*
 * \brief Initialize the stm crypto accelerator
 */

int crypto_hw_accelerator_init(void)
{
#if defined(GENERATOR_HW_CRYPTO_DPA_SUPPORTED)
    __HAL_RCC_SHSI_ENABLE();
#endif
#ifdef PSA_USE_SE_ST
    return se_st_engine_init();
#else
    return 0;
#endif
}

/*
 * \brief Deallocate the stm crypto accelerator
 */
int crypto_hw_accelerator_finish(void)
{
    return 0;
}


