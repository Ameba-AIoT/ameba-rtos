/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_ROM_PATCH_H_
#define _AMEBA_ROM_PATCH_H_

/* Exported constants --------------------------------------------------------*/
/** @defgroup ROM_PATCH_Exported_Constants ROM_PATCH Exported Constants
  * @{
  */

// add your constants here

/**
  * @}
  */

/* Exported functions -------------------------------------------------------- */
/** @defgroup ROM_PATCH_Exported_Functions ROM_PATCH Exported Functions
  * @{
  */

// add your function statements here
_LONG_CALL_ void GPIO_ODInit(u8 PinName, u32 PUInternal);
_LONG_CALL_ void GPIO_ODWriteBit(u8 PinName, u32 PinState);
_LONG_CALL_ void Pinmux_Config(u8 PinName, u32 PinFunc);
_LONG_CALL_ void LOGUART_PutChar_RAM(u8 c);
_LONG_CALL_ void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct);
_LONG_CALL_ u32 UART_Writable(UART_TypeDef *UARTx);
_LONG_CALL_ void UART_SendData(UART_TypeDef *UARTx, u8 *InBuf, u32 Count);
_LONG_CALL_ u32 UART_SendDataTO(UART_TypeDef *UARTx, u8 *InBuf, u32 Count, u32 Times);
_LONG_CALL_ int rtl_crypto_aes_cbc_encrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_cbc_decrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_ecb_encrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_ecb_decrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_ctr_encrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_ctr_decrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_cfb_encrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_cfb_decrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_ofb_encrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);
_LONG_CALL_ int rtl_crypto_aes_ofb_decrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u32 ivlen, OUT u8 *pResult);

_LONG_CALL_ int rtl_crypto_aes_gcm_encrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u8 *aad, IN const u32 aadlen, OUT u8 *pResult,
		OUT u8 *pTag);
_LONG_CALL_ int rtl_crypto_aes_gcm_decrypt(IN const u8 *message, IN const u32 msglen, IN const u8 *iv, IN const u8 *aad, IN const u32 aadlen, OUT u8 *pResult,
		OUT u8 *pTag);
_LONG_CALL_ int TRNG_get_random_bytes(void *dst, u32 size);
_LONG_CALL_ int TRNG_get_random_bytes_f_rng(void *p_rng, unsigned char *output, size_t output_size);

/**
  * @}
  */

#endif /* _AMEBA_ROM_PATCH_H_ */