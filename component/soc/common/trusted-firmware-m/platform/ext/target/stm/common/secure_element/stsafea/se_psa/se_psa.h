/**
  ******************************************************************************
  * @file    se_psa.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @brief   SE_PSA interface include file.
  *          Specifies device specific defines, macros, types.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SE_PSA_H__
#define __SE_PSA_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "psa/crypto_se_driver.h"
#include "psa/crypto.h"
#include "psa/crypto_extra.h"
#include "se_psa_id.h"


/** This macro checks if the result of an `expression` is equal to an
  *  `expected` value and sets a `status` variable of type `psa_status_t` to
  *  `PSA_SUCCESS`. If they are not equal, the `status` is set to
  *  `psa_error instead`, the error details are printed, and the code jumps
  *  to the `exit` label. */
#define ASSERT_STATUS(expression, expected, psa_error)              \
  do                                                              \
  {                                                               \
    SE_API_Status_t ASSERT_result = (expression);                   \
    SE_API_Status_t ASSERT_expected = (expected);                   \
    if ((ASSERT_result) != (ASSERT_expected))                   \
    {                                                           \
      printf("assertion failed at %s:%d "                     \
             "(actual=%d expected=%d)\n", __FILE__, __LINE__, \
             ASSERT_result, ASSERT_expected);                 \
      status = (psa_error);                                   \
      goto exit;                                              \
    }                                                           \
    status = PSA_SUCCESS;                                       \
  } while(0)

/** Check if an ATCA operation is successful, translate the error otherwise. */
#define ASSERT_SUCCESS(expression) ASSERT_STATUS(expression, SE_OK, \
                                                 se_st_to_psa_error(ASSERT_result))

/** Does the same as the macro above, but without the error translation and for
  *  the PSA return code - PSA_SUCCESS.*/
#define ASSERT_SUCCESS_PSA(expression) ASSERT_STATUS(expression, PSA_SUCCESS, \
                                                     ASSERT_result)


extern const psa_drv_se_t psa_se_st;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __SE_PSA_H__ */


