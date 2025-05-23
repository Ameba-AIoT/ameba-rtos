/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_ROM_PATCH_H_
#define _AMEBA_ROM_PATCH_H_

/* Exported constants --------------------------------------------------------*/
/** @defgroup ROM_PATCH_Exported_Exported_Types
  * @{
  */

/** @defgroup ROM_PATCH_Structure_Type Structure Type
  * @{
  */

struct Rcc_ClkDiv {
	u32 CkdGroupOfs;
	u32 BitMask;
	u32 DivShift;
	u32 DivVal; /* Note in some IP, the real work divider equals (regval + 1) */
};
/** @} */
/** @} */

/* Exported functions -------------------------------------------------------- */
/** @defgroup ROM_PATCH_Exported_Functions ROM_PATCH Exported Functions
  * @{
  */

// add your function statements here
_LONG_CALL_ void RCC_PeriphClockDivSet(const struct Rcc_ClkDiv *pdiv);
_LONG_CALL_ u8 RCC_PeriphClockEnableChk(u32 APBPeriph_Clock_in);
_LONG_CALL_ void RCC_PeriphClockSource_UART(UART_TypeDef *UARTx, u32 Source);
_LONG_CALL_ void RCC_PeriphClockSource_LOGUART(u32 Source);
_LONG_CALL_ void UART_Init(UART_TypeDef *UARTx, UART_InitTypeDef *UART_InitStruct);
_LONG_CALL_ u32 UART_Writable(UART_TypeDef *UARTx);
_LONG_CALL_ void UART_SendData(UART_TypeDef *UARTx, u8 *InBuf, u32 Count);
_LONG_CALL_ u32 UART_SendDataTO(UART_TypeDef *UARTx, u8 *InBuf, u32 Count, u32 Times);
_LONG_CALL_ void UART_WaitBusy(UART_TypeDef *UARTx, u32 PollTimes);
_LONG_CALL_ void GPIO_Init(GPIO_InitTypeDef  *GPIO_InitStruct);
_LONG_CALL_ void GPIO_WriteBit_Critical(u32 GPIO_Pin, u32 Pin_State);
_LONG_CALL_ int OTP_LogicalMap_Read(u8 *pbuf, u32 addr, u32 len);
_LONG_CALL_ int OTP_LogicalMap_Write(u32 addr, u32 cnts, u8 *data);
_LONG_CALL_ void LOGUART_INT_NP2AP(void);
_LONG_CALL_ void LOGUART_INT_AP2NP(void);
_LONG_CALL_ u8 LOGUART_Writable(void);
_LONG_CALL_ void LOGUART_WaitBusy(void);
_LONG_CALL_ void LOGUART_WaitTxComplete(void);
_LONG_CALL_ int OTP_LogicalMap_CHK(void);
_LONG_CALL_ int TRNG_get_random_bytes(void *dst, u32 size);
_LONG_CALL_ int TRNG_get_random_bytes_f_rng(void *p_rng, unsigned char *output, size_t output_size);
char *__real_strncpy(char *dst0, const char *src0, int count);
char *__real_strcpy(char *s1, const char *s2);
long __real_strtol(const char *nptr, char **endptr, int base);
_LONG_CALL_ void FLASH_RxBasic(u8 cmd, u32 Address, u32 read_len, u8 *read_data);
_LONG_CALL_ void FLASH_TxBasic(u8 cmd, u32 Address, u32 DataLen, u8 *TransmitData);
/** @} */

#endif /* _AMEBA_ROM_PATCH_H_ */
