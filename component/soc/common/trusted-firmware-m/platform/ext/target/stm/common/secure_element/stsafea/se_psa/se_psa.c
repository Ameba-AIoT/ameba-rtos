/**
  ******************************************************************************
  * @file    se_psa.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @brief   Implementation file of psa_drv_se interface.
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

#if defined(PSA_USE_SE_ST)

/* Includes ------------------------------------------------------------------*/
#include "tfm_mbedcrypto_include.h"
#include "se_psa.h"
#include "stsafea_service_stub.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "psa_manifest/pid.h"
#include "tfm_nspm.h"
#include "tfm_spm_log.h"
#include "tfm_plat_defs.h"
#include "tfm_plat_crypto_keys.h"

/* Private typedef -----------------------------------------------------------*/
typedef struct
{
  uint32_t map_size;
} SE_psa_drv_se_persistent_t;


#define SE_ST_MAX_INPUT_SIZE 488 /* data to be shared by user is */

/* Transient context command status */
#define SE_ST_CMD_NOT_INIT 0
#define SE_ST_CMD_READY    1
#define SE_ST_CMD_EXECUTED 2

typedef struct
{
    uint8_t cmd;
    uint16_t size;
    uint8_t buffer[SE_ST_MAX_INPUT_SIZE];
    uint8_t status;
}       psa_se_st_transient_ctx_t;

psa_se_st_transient_ctx_t se_st_t_ctx;

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define SE_ST_PSA_PRINT debug_print
/* Private struct ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static psa_status_t register_se_keys(void);
//#define TOP_CHECK_KEY
int32_t debug_print(char const *fmt, ...);
psa_status_t se_st_to_psa_error(int32_t  ret);
/**
  * @brief   debug_print
  *          psa trace function for SE PSA traces.
  *
  * @param   fmt : char string to format output
  * @retval  return value from tfm_hal_output_spm_log
  */
int32_t debug_print(char const *fmt, ...)
{
  int32_t len;
  va_list args;
  va_start(args, fmt);
  char trace_buf[500];

  len = vsnprintf(trace_buf, sizeof(trace_buf), fmt, args);
  SPMLOG_DBGMSG(trace_buf);
  return 0;
}

/**
  * @brief   se_st_to_psa_error
  *          translate error code to psa_status_t
  *
  * @param   ret[in] : int32_t error code
  * @retval  psa_status_t translation
  */
psa_status_t se_st_to_psa_error(int32_t  ret)
{
  switch (ret)
  {
    case 0:
      return PSA_SUCCESS;
    default:
      return PSA_ERROR_HARDWARE_FAILURE;
  }
}


/**
  * @brief   register a key in its operational domain(s)
  *
  * @param   key[in]  : Identifier of the key to use for the operation.
  * @param   attr[in] : The attributes for the key.
  *
  * @retval  #PSA_SUCCESS
  *          A key slot is created.
  */
static psa_status_t register_key_in_all_domain(mbedtls_svc_key_id_t key,
                                               psa_key_attributes_t *attr) {
  psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;
  
  
  psa_key_handle_t key_handle;
    
    
 /* check if keys are present */
  SE_ST_PSA_PRINT("checking key existance %x\n", key.MBEDTLS_PRIVATE(key_id));
  psa_status = psa_open_key(key, &key_handle);
  if (psa_status != PSA_SUCCESS)
  {
    psa_set_key_id(attr, key);
    SE_ST_PSA_PRINT("Registering key : ");
    psa_status = mbedtls_psa_register_se_key(attr);
    if (psa_status != PSA_SUCCESS) {
        SE_ST_PSA_PRINT("KO %d\n", psa_status);
  goto exit;
    }
    SE_ST_PSA_PRINT("OK\n");
  }
  else
  {
    psa_close_key(key_handle);
    psa_status = PSA_SUCCESS;
  }

exit:
  
  return psa_status;
}

/**
  * @brief   register a function key with its corresponding attributes.
  *          This type of key is not physically in the Secure Element but
  *          represents the structure of physical element(s).
  *
  * @param   key[in]  : Identifier of the key to use for the operation.
  * @param   attr[in] : The attributes for the key.
  *
  * @retval  #PSA_SUCCESS
  *          Key is imported and usable.
  *          Its usage can be from all applications but also extended to
  *          specific partition(s).
  */
static psa_status_t register_function_key(psa_key_id_t key_id,
                                          psa_key_attributes_t *attr)
{
    mbedtls_svc_key_id_t mbedtls_id = {0,tfm_nspm_get_current_client_id()};
    /* attribute for function keys */
    psa_set_key_bits(attr, 8); /* logical asset */
    psa_set_key_lifetime(attr, PSA_SE_ST_LIFETIME_READ_ONLY);
    psa_set_key_usage_flags(attr, PSA_KEY_USAGE_EXPORT);
    psa_set_key_type(attr, PSA_KEY_TYPE_RAW_DATA);

    mbedtls_id.MBEDTLS_PRIVATE(key_id) = key_id,

    psa_set_key_slot_number(attr, PSA_ID_TO_SE_ST_ID(key_id));

    return register_key_in_all_domain(mbedtls_id, attr);
}

/**
  * @brief   register a function key to deal with the unique serial number
  *          identifier of in the Secure Element.
  *
  * @retval  #PSA_SUCCESS
  *          Key is created.
  */
static psa_status_t register_se_serial_number_key(void)
{
    psa_key_attributes_t attributes = psa_key_attributes_init();

    return register_function_key( SE_ST_ID_TO_PSA_ID(SE_ST_SERIAL_NUMBER),
                                  &attributes);
}

/**
  * @brief   register a key set of the Secure Element
  *
  * @retval  #PSA_SUCCESS
  *          Key set is created and subcribed in the TFM.
  */
static psa_status_t register_se_keys(void)
{
 psa_status_t psa_status = PSA_ERROR_GENERIC_ERROR;

    /* check if keys are present */
#ifdef TOP_CHECK_KEY
    mbedtls_svc_key_id_t key_id = {0,tfm_nspm_get_current_client_id()};
    psa_key_handle_t key_handle;

    key_id.MBEDTLS_PRIVATE(key_id) = SE_ST_ID_TO_PSA_ID(SE_ST_SERIAL_NUMBER);
    psa_status = psa_open_key(key_id, &key_handle);
    if (psa_status != PSA_SUCCESS)
    {
#endif
      if ((psa_status = register_se_serial_number_key()) != PSA_SUCCESS)
            return psa_status;
#ifdef TOP_CHECK_KEY
    }
#endif
    return psa_status;
}

/**
  * @brief   psa_se_st_init
  *          Init of PSA drv SE
  * @note    Start Low Level HW for SE
  *
  * @param   drv_context : psa_drv_se_context_t structure to keep persistent data and transient data
  * @param   persistent_data : init data provided by psa drv se mechanism.
  * @param   location : location of the init call
  * @retval  PSA_SUCCESS if success, an error code otherwise
  */

static psa_status_t psa_se_st_init(psa_drv_se_context_t *drv_context,
                                   void *persistent_data,
                                   psa_key_location_t location)
{
  /* Not used by SE_API */
  if (drv_context == NULL)
  {
    
    return PSA_ERROR_INVALID_ARGUMENT;
  }
  memset(&se_st_t_ctx, 0, sizeof(se_st_t_ctx));
  drv_context->MBEDTLS_PRIVATE(transient_data) = (uintptr_t)&se_st_t_ctx;

  BSP_I2C_Init();
  HW_IO_Init();
  /* check PSA key location */
  if (location != PSA_SE_ST_LOCATION)
  {
    SE_ST_PSA_PRINT("Invalid driver\n");
    
    return PSA_ERROR_GENERIC_ERROR;
  }

  SE_ST_PSA_PRINT("Secure Element driver initialized\n");
  /* register keys */
  return register_se_keys();
}

/**
  * @brief   psa_se_st_validate_slot_number_fct
  *          Validate slot number for psa drv se according to key attributes
  *
  * @param   drv_context : psa_drv_se_context_t structure to keep persistent data and transient data
  * @param   persistent_data : init data provided by psa drv se mechanism.
  * @param   attributes : key attributes to map on SE ressources
  * @param   method : key creation context supported values
  *                   PSA_KEY_CREATION_REGISTER : persistent key declaration
  * @param   key_slot : internal key index
  * @retval  PSA_SUCCESS if success, an error code otherwise
  */
static psa_status_t psa_se_st_validate_slot_number_fct(
  psa_drv_se_context_t *drv_context,
  void *persistent_data,
  const psa_key_attributes_t *attributes,
  psa_key_creation_method_t method,
  psa_key_slot_number_t key_slot)
{
  psa_key_type_t type = psa_get_key_type(attributes);
  
  /* support registration and import only for memory region */
  if (method != PSA_KEY_CREATION_REGISTER)
  {
    SE_ST_PSA_PRINT("Key slot validation cannot operate with method : %x\n", method);
    
    return PSA_ERROR_NOT_SUPPORTED;
  }
  /* if type = PSA_KEY_TYPE_RAW_DATA check if SE_ST_SERIAL_NUMBER */
  if (type == PSA_KEY_TYPE_RAW_DATA)
  {
    if (key_slot != SE_ST_SERIAL_NUMBER)
    {
      return PSA_ERROR_INVALID_ARGUMENT;
    }
  }
  else
  {
    return PSA_ERROR_INVALID_ARGUMENT;
  }

  /* if already exists do not return error */
  
  return PSA_SUCCESS;
}

/**
  * @brief   psa_se_st_export_public_fct
  *          
  * @note    Not implemented
  *
  * @param   drv_context 
  * @param   key
  * @param   p_data
  * @param   data_size
  * @param   p_data_length
  * @retval  PSA_ERROR_NOT_SUPPORTED
  */
static psa_status_t psa_se_st_export_public_fct(psa_drv_se_context_t *drv_context,
                                                psa_key_slot_number_t key,
                                                uint8_t *p_data,
                                                size_t data_size,
                                                size_t *p_data_length)
{
  return PSA_ERROR_NOT_SUPPORTED;
}


/**
  * @brief   psa_se_st_gen_key_fct
  *          
  * @note    Not implemented
  *
  * @param   drv_context 
  * @param   key_slot 
  * @param   attributes 
  * @param   pubkey 
  * @param   pubkey_size 
  * @param   pubkey_length 
  * @retval  PSA_ERROR_NOT_SUPPORTED
  */
static psa_status_t psa_se_st_gen_key_fct(
  psa_drv_se_context_t *drv_context,
  psa_key_slot_number_t key_slot,
  const psa_key_attributes_t *attributes,
  uint8_t *pubkey, size_t pubkey_size, size_t *pubkey_length)
{
  return PSA_ERROR_NOT_SUPPORTED;
}

/**
  * @brief   psa_se_st_import_fct
  *         
  * @note    PSA_ERROR_NOT_SUPPORTED
  *
  * @param   drv_context
  * @param   key_slot
  * @param   attributes
  * @param   data
  * @param   data_length
  * @param   bits
  * @retval  PSA_ERROR_NOT_SUPPORTED
  */
static psa_status_t psa_se_st_import(
  psa_drv_se_context_t *drv_context,
  psa_key_slot_number_t key_slot,
  const psa_key_attributes_t *attributes,
  const uint8_t *data,
  size_t data_length,
  size_t *bits)
{
  return PSA_ERROR_NOT_SUPPORTED;
}

static void byte2char(uint8_t b, uint8_t *c)
{
  uint8_t d = b >> 4;
  if (d < 10)
    *c = '0' + d;
  else
    *c = 'A' + d - 10;
  c++;
  d = b & 0xF;
  if (d < 10)
    *c = '0' + d;
  else
    *c = 'A' + d - 10;
}

static psa_status_t psa_se_st_get_serial(uint8_t *data, size_t data_size, size_t *p_data_length)
{
  uint8_t sn_data[] = { 0x14, 0x11, 0xFC, 0xBE };  
  uint8_t r_data[100];
  uint8_t *sn = &r_data[10];
  int32_t ret = 0;
  if (data_size < 9)
  {
    SE_ST_PSA_PRINT("Serial Number cannot be set in a buffer of size : %d\n", data_size);
    
    return PSA_ERROR_BUFFER_TOO_SMALL;
  }

  ret = BSP_I2C_Send(0x40, sn_data, 4);
  if (ret == 0)
  {
    BSP_TimeDelay(5);
    ret = BSP_I2C_Recv(0x40, r_data, 69);
  }
  if (ret != 0)
  {
    return PSA_ERROR_HARDWARE_FAILURE;
  }
  
  if (data_size >= 19)
  {
    for (int i = 0; i < 9; i++)
    {
      byte2char(sn[i], &data[2*i]);
    }
    data[18] = '\0';
    if (p_data_length != NULL)
    {
      *p_data_length = 19;
    }
  }
  else
  {
    for (int i = 0; i < 9; i++)
      data[i] = sn[i];
    if (p_data_length != NULL)
    {
      *p_data_length = 9;
    }
  }
  if (ret != 0)
  {
    SE_ST_PSA_PRINT("Serial Number cannot be recovered\n");
    
    return PSA_ERROR_HARDWARE_FAILURE;
  }
  
  return PSA_SUCCESS;
}

/**
  * @brief   psa_se_st_export_fct
  *          export serial number from SE
  * @note    
  *
  * @param   drv_context : psa_drv_se_context_t structure to keep persistent data and transient data
  * @param   key_slot : internal key index
  * @param   attributes : key attributes for the memory region
  * @param   data : data buffer to write SE Serial Number
  * @param   data_size : requested size of data to export
  * @param   p_data_length : actual size of exported data
  * @retval  PSA_SUCCESS if success, an error code otherwise
  */
static psa_status_t psa_se_st_export(psa_drv_se_context_t *drv_context,
                              psa_key_slot_number_t key,
                              uint8_t *data,
                              size_t data_size,
                              size_t *p_data_length)
{
  const uint16_t slot = key;
  
  switch (slot)
  {
    case SE_ST_SERIAL_NUMBER :
    {
      return psa_se_st_get_serial(data, data_size, p_data_length);      
    }
    default:
      return PSA_ERROR_NOT_SUPPORTED;
  }

}

/**
  * @brief   psa_se_st_allocate_key
  *          
  *
  * @param   drv_context
  * @param   persistent_data
  * @param   attributes
  * @param   method
  * @param   key_slot
  * @retval  PSA_ERROR_NOT_SUPPORTED
  */
static psa_status_t psa_se_st_allocate_key(
  psa_drv_se_context_t *drv_context,
  void *persistent_data,
  const psa_key_attributes_t *attributes,
  psa_key_creation_method_t method,
  psa_key_slot_number_t *key_slot)
{
  return PSA_ERROR_NOT_SUPPORTED;
}


/**
  * @brief   psa_se_st_destroy_key
  *          key deallocation of SE ressources
  *
  * @param   drv_context : psa_drv_se_context_t structure to keep persistent data and transient data
  * @param   persistent_data : updatable persistent data for psa drv se
  * @param   key_slot : internal key slot to free
  * @retval  PSA_SUCCESS if success, an error code otherwise
  */
static psa_status_t psa_se_st_destroy_key(
  psa_drv_se_context_t *drv_context,
  void *persistent_data,
  psa_key_slot_number_t key_slot)
{
  return PSA_SUCCESS;
}



/**
  * @brief   psa_se_st_sign_fct
  *
  * @param   drv_context
  * @param   key_slot
  * @param   alg
  * @param   p_hash
  * @param   hash_length
  * @param   p_signature
  * @param   signature_size
  * @param   p_signature_length
  * @retval  PSA_ERROR_NOT_SUPPORTED
  */
static psa_status_t psa_se_st_sign_fct(psa_drv_se_context_t *drv_context,
                                       psa_key_slot_number_t key_slot,
                                       psa_algorithm_t alg,
                                       const uint8_t *p_hash,
                                       size_t hash_length,
                                       uint8_t *p_signature,
                                       size_t signature_size,
                                       size_t *p_signature_length)
{
  return PSA_ERROR_NOT_SUPPORTED;
}

/**
  * @brief   psa_se_st_verify_fct
  *          Asymmetric signature verification
  * @note    not implemented yet
  *
  * @param   drv_context : psa_drv_se_context_t structure to keep persistent data and transient data
  * @param   key_slot : internal key slot
  * @param   alg : expected signature scheme (only PSA_ALG_ECDSA() supported)
  * @param   p_hash : hash buffer
  * @param   hash_length : size of hash buffer
  * @param   p_signature : signature buffer for RAW R and S output
  * @param   signature_size : size of signature buffer
  * @param   p_signature_length : actual size of outputed signature
  * @retval  PSA_SUCCESS if success, an error code otherwise
  */
static psa_status_t psa_se_st_verify_fct(psa_drv_se_context_t *drv_context,
                                         psa_key_slot_number_t key_slot,
                                         psa_algorithm_t alg,
                                         const uint8_t *p_hash,
                                         size_t hash_length,
                                         const uint8_t *p_signature,
                                         size_t signature_length)
{
  return PSA_SUCCESS;
}



psa_drv_se_key_management_t psa_se_st_key_management_fct =
{
  /* first step to allocate a new private key */
  .MBEDTLS_PRIVATE(p_allocate) = psa_se_st_allocate_key,
  .MBEDTLS_PRIVATE(p_destroy) = psa_se_st_destroy_key,
  /* use to address memory region read */
  .MBEDTLS_PRIVATE(p_export) = psa_se_st_export,
  /* export public key for the 3 first sectors */
  .MBEDTLS_PRIVATE(p_export_public) = psa_se_st_export_public_fct,
  /* not use for ECDHE as not implemented yet by MbedCrypto */
  .MBEDTLS_PRIVATE(p_generate) = psa_se_st_gen_key_fct,
  /* use to address memory region update */
  .MBEDTLS_PRIVATE(p_import) = psa_se_st_import,
  /* use to validate the pre provisionned key */
  .MBEDTLS_PRIVATE(p_validate_slot_number) = psa_se_st_validate_slot_number_fct,
};


psa_drv_se_key_derivation_t psa_se_st_derivation_fct =
{
  .MBEDTLS_PRIVATE(p_derive) = NULL,
  .MBEDTLS_PRIVATE(context_size) = 0,
  .MBEDTLS_PRIVATE(p_collateral) = NULL,
  .MBEDTLS_PRIVATE(p_export) = NULL,
  .MBEDTLS_PRIVATE(p_setup) = NULL,
};

psa_drv_se_asymmetric_t psa_se_st_asymmetric_fct =
{
  .MBEDTLS_PRIVATE(p_decrypt) = NULL,
  .MBEDTLS_PRIVATE(p_encrypt) = NULL,
  .MBEDTLS_PRIVATE(p_sign) = psa_se_st_sign_fct,
  .MBEDTLS_PRIVATE(p_verify) = psa_se_st_verify_fct,
};

const psa_drv_se_t psa_se_st =
{
  .MBEDTLS_PRIVATE(hal_version) = PSA_DRV_SE_HAL_VERSION,
  .MBEDTLS_PRIVATE(aead) = NULL,
  .MBEDTLS_PRIVATE(asymmetric) = &psa_se_st_asymmetric_fct,
  .MBEDTLS_PRIVATE(cipher) = NULL,
  .MBEDTLS_PRIVATE(derivation) = &psa_se_st_derivation_fct,
  .MBEDTLS_PRIVATE(key_management) = &psa_se_st_key_management_fct,
  .MBEDTLS_PRIVATE(mac) = NULL,
  .MBEDTLS_PRIVATE(persistent_data_size) = sizeof(SE_psa_drv_se_persistent_t),
  .MBEDTLS_PRIVATE(p_init) = psa_se_st_init,
};
int se_st_engine_init(void)
{
    if (psa_register_se_driver(PSA_SE_ST_LOCATION, &psa_se_st) !=PSA_SUCCESS)
    {
        return -1;
    }
    return 0;
}
#endif /* PSA_USE_SE_ST */

