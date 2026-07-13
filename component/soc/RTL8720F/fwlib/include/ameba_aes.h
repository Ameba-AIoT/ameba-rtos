/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_AES_H_
#define _AMEBA_AES_H_


/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup AES AES
  * @brief AES driver modules
  * @{
  */

/// @cond
/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup AES_Register_Definitions AES Register Definitions
  * @{
  */

/** @defgroup AES_CPU_DATAIN_x
  * @brief CPU Data Input register x
  * @{
  */
#define AES_MASK_CPU_DATAIN_x   ((u32)0xFFFFFFFF << 0)           /*!< RW 0x0  Input register bit[32*x+31:32*x] of the AES engine in the slave mode, only used in Slave_mode. As input to the engine, such as plaintext or ciphertext. The input order depends on chnx_aes_reg.config.byte_swap_disable bit. If byte_swap_disable = 0, data = {cpu_datain_3[7:0], cpu_datain_3[15:8], cpu_datain_3[23:16], cpu_datain_3[31:24], cpu_datain_2[7:0], cpu_datain_2[15:8], cpu_datain_2[23:16], cpu_datain_2[31:24], cpu_datain_1[7:0], cpu_datain_1[15:8], cpu_datain_1[23:16], cpu_datain_1[31:24], cpu_datain_0[7:0], cpu_datain_0[15:8], cpu_datain_0[23:16], cpu_datain_0[31:24]} If byte_swap_disable = 1, data = {cpu_datain_3, cpu_datain_2, cpu_datain_1, cpu_datain_0} Write 4 words to cpu_datain would trigger aes engine to start computing, and user can keep writing cpu_datain as long as input fifo is not full (see config.cpu_fifo_status). Note that when user starts to write input data, 128bits (4 words) should be written together, that is, user is not allowed to read-write interleave. In other word, no partial write (3 words/2 words/1 words) is allowed, and no read can be inserted between writes.*/
#define AES_CPU_DATAIN_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_CPU_DATAIN_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_CPU_DATAOUT_x
  * @brief CPU Data Output register x
  * @{
  */
#define AES_MASK_CPU_DATAOUT_x   ((u32)0xFFFFFFFF << 0)           /*!< RO 0x0  Output register bit[32*x+31:32*x] of the AES engine in the slave mode, only used in Slave_mode. User can wait for the interrupt or poll the aes_slave_done signal to know whether the AES operation has completed. The result of AES will be stored in this register. The data order depends on chnx_aes_reg.config.byte_swap_disable bit. If byte_swap_disable = 0, rdata = {cpu_dataout_3[7:0], cpu_dataout_3[15:8], cpu_dataout_3[23:16], cpu_dataout_3[31:24],   cpu_dataout_2[7:0], cpu_dataout_2[15:8], cpu_dataout_2[23:16], cpu_dataout_2[31:24],   cpu_dataout_1[7:0], cpu_dataout_1[15:8], cpu_dataout_1[23:16], cpu_dataout_1[31:24], cpu_dataout_0[7:0], cpu_dataout_0[15:8], cpu_dataout_0[23:16], cpu_dataout_0[31:24]}; If byte_swap_disable = 1,r data = {cpu_dataout_3, cpu_dataout_2, cpu_dataout_1, cpu_dataout_0} User can keep reading cpu_dataout as long as output fifo is not empty (see config.cpu_fifo_status). *Note that when user starts to read output data, 128bits (4 words) should be read together, that is, user is not allowed to read-write interleave. In other word, no partial read (3 words/2 words/1 words) is allowed, and no write can be inserted between reads.*/
#define AES_CPU_DATAOUT_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_CPU_DATAOUT_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_KEY_DE_OUT_x
  * @brief Decryption key register x
  * @{
  */
#define AES_MASK_KEY_DE_OUT_x   ((u32)0xFFFFFFFF << 0)           /*!< RO 0x0  Decryption key register bit[32*x+31:32*x] If using ECB or CBC mode to decrypt, the decryption key will be derived first. The register is used to store this key. User can read for verification. The key order is depend on chnx_aes_reg.config.byte_swap_disable bit. If byte_swap_disable = 0, rdata = {key_de_out_7[7:0], key_de_out_7[15:8], key_de_out_7[23:16], key_de_out_7[31:24], key_de_out_6[7:0], key_de_out_6[15:8], key_de_out_6[23:16], key_de_out_6[31:24], key_de_out_5[7:0], key_de_out_5[15:8], key_de_out_5[23:16], key_de_out_5[31:24], key_de_out_4[7:0], key_de_out_4[15:8], key_de_out_4[23:16], key_de_out_4[31:24], key_de_out_3[7:0], key_de_out_3[15:8], key_de_out_3[23:16], key_de_out_3[31:24], key_de_out_2[7:0], key_de_out_2[15:8], key_de_out_2[23:16], key_de_out_2[31:24], key_de_out_1[7:0], key_de_out_1[15:8], key_de_out_1[23:16], key_de_out_1[31:24], key_de_out_0[7:0], key_de_out_0[15:8], key_de_out_0[23:16], key_de_out_0[31:24]}; If byte_swap_disable = 1,r data = {key_de_out_7, key_de_out_6, key_de_out_5, key_de_out_4, key_de_out_3, key_de_out_2, key_de_out_1, key_de_out_0}*/
#define AES_KEY_DE_OUT_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_KEY_DE_OUT_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_IV_x
  * @brief IV register x
  * @{
  */
#define AES_MASK_IV_x   ((u32)0xFFFFFFFF << 0)           /*!< RW 0x0  IV register bit[32*x+31:32*x] Except for ECB mode, other modes require IV as input.  The user should set the IV before using the AES engine.  The IV order depends on chnx_aes_reg.config.byte_swap_disable bit. If byte_swap_disable = 0, iv = {iv_3[7:0], iv_3[15:8], iv_3[23:16], iv_3[31:24], iv_2[7:0], iv_2[15:8], iv_2[23:16], iv_2[31:24], iv_1[7:0], iv_1[15:8], iv_1[23:16], iv_1[31:24], iv_0[7:0], iv_0[15:8], iv_0[23:16], iv_0[31:24]}; If byte_swap_disable = 1, iv = {iv_3, iv_2, iv_1, iv_0}*/
#define AES_IV_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_IV_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_CONFIG
  * @brief Configuration register
  * @{
  */
#define AES_BIT_IDLE                                  ((u32)0x00000001 << 31)           /*!< RO 0x1  When using slave mode to compute gmac, cmac, cbc-mac modes, since the result is stored in tag register, there would be no data to be read from output fifo. Therefore, we use this bit to inform that gmac/cmac/cbc-mac computation has finished. (finished means: no input data in input fifo waited to be processed and aes engine has finished all computations) user can poll this bit to check whether aes engine is not running when using GMAC/CMAC/CBC-MAC.*/
#define AES_BIT_IV_ADD_1                              ((u32)0x00000001 << 27)           /*!< R/W1T 0x0  write 1 to trigger a pulse in engine to add current iv inside iv register by 1. will return 0 automatically.*/
#define AES_BIT_GCM_INIT                              ((u32)0x00000001 << 26)           /*!< R/W1T 0x0  write 1 to inform the engine to perform additional computation in gcm's dma mode. will return to 0 automatically when in gcm's dma mode.*/
#define AES_BIT_ICG_DISABLE                           ((u32)0x00000001 << 25)           /*!< RW 0x0  In case of any unexpected malfunction, user can use icg_disable to let engine run in free run clk. Default: icg enabled.*/
#define AES_BIT_SOFT_RESET_TRIGGER_OR_SOFT_RESET_BUSY ((u32)0x00000001 << 24)           /*!< R/W1T 0x0  Writting 1 to trigger a soft reset.  All registers except the mutex register will be reset. Read the bit until 0 to determine if the soft reset is complete.*/
#define AES_MASK_CPU_FIFO_STATUS                      ((u32)0x0000003F << 18)           /*!< RO 0x10  CPU FIFO status (slave mode FIFO status)  {input fifo full (1b), output fifo empty (1b), input data write cnt (2b), output data read cnt (2b)} By adding fifos, user can keep writing input data to cpu_datain as long as input fifo is not full, and keep reading output data as long as output fifo is not empty. For user to check input/output fifo status. When aes_intr asserted (error_cpu_fifo), user can check fifo status to debug.*/
#define AES_CPU_FIFO_STATUS(x)                        (((u32)((x) & 0x0000003F) << 18))
#define AES_GET_CPU_FIFO_STATUS(x)                    ((u32)(((x >> 18) & 0x0000003F)))
#define AES_BIT_MAC_LAST                              ((u32)0x00000001 << 17)           /*!< RW 0x0  Only used in CMAC/CBC-MAC mode. This bit controls whether to compute the the last step for the last block in the transaction. In CMAC mode, if set to 1, it means the transaction is the last time, and the last block will xor the subkey. In CBC-MAC mode, if set to 1, it means the transaction is the last time, and the engine will do auto-padding to the last block when operating in DMA mode. (slave mode still needs user to do zero-padding manually)*/
#define AES_BIT_AEAD_LAST                             ((u32)0x00000001 << 16)           /*!< RW 0x0  Only used in GCM/CCM mode. This bit controls whether to compute the last step in GCM/CCM algorithm. If set to 1, it means the transaction is the last time, the length of data can be any multiple of 8 bits, and the engine will compute the last step. If set to 0, data must be a multiple of 128 bits, otherwise the calculated result will be wrong. In CCM mode, if operates in DMA mode, engine will do auto-padding to the last block if this bit is set to 1. (if user mode, user still needs to do zero-padding manually) In some cases, user would need to use slave mode to compute gcm/ccm last while all other computations (GMAC/CBC-MAC) are computed in DMA mode (please refer to pages for GCM and CCM modes). In such cases, user should "not" set cpu_datain, user should set engine mode to slave mode first and then set aead_last to trigger AES engine. *Note that user should clear this be to zero after calculation.*/
#define AES_BIT_RANDOM_OP_MODE                        ((u32)0x00000001 << 15)           /*!< RW 0x0  Random operation insertion mode. 1: Dynamically insert random cycle of dummy operation in each AES round. 0: Insert fixed cycle (specify by random_op_cycle) of dummy operation in each AES round.*/
#define AES_MASK_RANDOM_OP_CYCLE                      ((u32)0x00000003 << 13)           /*!< RW 0x0  Random operation cycle. Range from 0~3. 0: Not insert any dummy operation. 1~3: Insert specific cycle of dummy operation.*/
#define AES_RANDOM_OP_CYCLE(x)                        (((u32)((x) & 0x00000003) << 13))
#define AES_GET_RANDOM_OP_CYCLE(x)                    ((u32)(((x >> 13) & 0x00000003)))
#define AES_BIT_CORE_SELECT                           ((u32)0x00000001 << 12)           /*!< RW 0x0  Cipher core select. 1: Select SM4 as cipher core 0: Select AES as cipher core*/
#define AES_BIT_SCA_MASK_EN                           ((u32)0x00000001 << 11)           /*!< RW 0x0  SCA mask enable. SW should also set the chn0_sca_mask registers when system initalization. 1: enable SCA protection 0: disable SCA protection*/
#define AES_BIT_WAIT_FOR_SLAVE_DIS                    ((u32)0x00000001 << 10)           /*!< RW 0x0  pready in APB protocol can be low or always high (only for writing cpu_datain and reading cpu_dataout in slave mode ) set to 1: pready will always be 1 set to 0: pready could be 0 (wait states) default: 0 The difference is that, if set to 1, whenever cpu writes data to input fifo when input fifo is full/read output data from output fifo when output fifo is empty, pready will stay 1, and respond an error (pslverr asserted) immediately. if set to 0, when cpu writes data to input fifo when input fifo is full, however, if there is still space in output fifo, pready will stay 0 untill aes consumes input data from input fifo, that is, there are spaces for incoming input data now. Then, pready is asserted, and there will be no error response. When cpu reads data from output fifo when output fifo is empty, however, if there is still available data in input fifo, pready will stay 0 untill aes push output data to output fifo, that is, there is output data waiting to be read now. then, pready is asserted, and there will be no error response. if cpu attempts to write data to input fifo when input is full and output fifo is also full/read data from output fifo when output data is empty and input fifo is also empty, pready will be asserted and respond an error.*/
#define AES_BIT_DE_KEY_GEN                            ((u32)0x00000001 << 9)            /*!< RW 0x0  In general, the decryption key will be derived when decrypting in ECB or CBC mode. If the user has prepared the decryption key, can set this register to 0 to skip the cycles of derived. If 1, it will generate a key when decrypting. If 0, it will not generate the key, but instead uses the input to start decrypting.*/
#define AES_BIT_FIRST_BLOCK                           ((u32)0x00000001 << 8)            /*!< RW 0x0  Most modes of AES have additional operations when processing the first block. Set to 1, indicating that the task starts from the first block In DMA mode, the last step before starting to use AES is to set this bit to 1. When the engine is not idling, registers such as keys and IV cannot be set. When using slave mode, the last step after setting all registers is to set cpu datain. After setting cpu datain (128 bits in total), AES engine will start computing automatically. (recommend to set this bit to 0 if using slave mode)*/
#define AES_BIT_EN_DE                                 ((u32)0x00000001 << 7)            /*!< RW 0x0  1'b0: decrypt 1'b1: encrypt*/
#define AES_MASK_KEY_SIZE                             ((u32)0x00000003 << 5)            /*!< RW 0x0  2'b00: 128 bits 2'b01: 192 bits 2'b10: 256 bits*/
#define AES_KEY_SIZE(x)                               (((u32)((x) & 0x00000003) << 5))
#define AES_GET_KEY_SIZE(x)                           ((u32)(((x >> 5) & 0x00000003)))
#define AES_MASK_CIPHER_MODE                          ((u32)0x0000000F << 1)            /*!< RW 0x0  4'b0000: ECB mode 4'b0001: CBC mode 4'b0010: CFB mode 4'b0011: OFB mode 4'b0100: CTR mode 4'b0101: GMAC mode (only available when SUPPORT_GCM=1) 4'b0110: GCM mode (only available when SUPPORT_GCM=1) 4'b0111: CMAC mode 4'b1000: XTS mode (only available when SUPPORT_XTS=1) 4'b1001: CBC-MAC mode 4'b1010: CCM mode*/
#define AES_CIPHER_MODE(x)                            (((u32)((x) & 0x0000000F) << 1))
#define AES_GET_CIPHER_MODE(x)                        ((u32)(((x >> 1) & 0x0000000F)))
#define AES_BIT_ENGINE_MODE                           ((u32)0x00000001 << 0)            /*!< RW 0x0  0: Slave mode 1: DMA mode*/
/** @} */

/** @defgroup AES_CIPHER_LEN
  * @brief Input length register
  * @{
  */
#define AES_MASK_CIPHER_LEN   ((u32)0xFFFFFFFF << 0)           /*!< RW 0x0  Text bit length. In the CFB, OFB and CTR mode, whether the last block is 16-byte aligned affects the length of the output. In the XTS and CMAC mode, the last block requires additional computation. The user needs to provide the total length for reference. In the GCM mode, the user needs to inform the length of the plaintext or ciphertext. The plaintext or ciphertext does not need to be given at once. But every time add data before the end of the calculation, the user need to update the data length. Will be used in the last GMAC calculation of GCM In CCM/CBC-MAC mode, if using DMA mode, engine will do auto-padding according to cipher_len if (mac_last/aead_last is set to 1). Note that in CBC-MAC mode, the cipher length should be the length of formatted(nonce+len of aad, aad), please refer to CCM algorithm.*/
#define AES_CIPHER_LEN(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_CIPHER_LEN(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_GMAC_LEN_x
  * @brief AAD length register x
  * @{
  */
#define AES_MASK_GMAC_LEN_x   ((u32)0xFFFFFFFF << 0)           /*!< RW 0x0  AAD length register bit[32*x+31:32*x] in GMAC and GCM mode In the GCM or GMAC mode, the user needs to inform the length of the Additional Authenticated Data (AAD). The length of the one AAD needs to be accumulated.  Will be used in the last GMAC calculation of GCM.*/
#define AES_GMAC_LEN_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_GMAC_LEN_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_TAG_x
  * @brief Tag register x
  * @{
  */
#define AES_MASK_TAG_x   ((u32)0xFFFFFFFF << 0)           /*!< RW 0x0  Tag register bit[32*x+31:32*x] After a complete GCM/CCM operation, a tag will be generated. The tag will be stored in this register. In addition, because GCM/CCM can be done in segments, tag can be used as input when GCM/CCM is started again. The tag order depends on chnx_aes_reg.config.byte_swap_disable bit. If byte_swap_disable = 0, tag = {tag_3[7:0], tag_3[15:8], tag_3[23:16], tag_3[31:24], tag_2[7:0], tag_2[15:8], tag_2[23:16], tag_2[31:24], tag_1[7:0], tag_1[15:8], tag_1[23:16], tag_1[31:24], tag_0[7:0], tag_0[15:8], tag_0[23:16], tag_0[31:24]}; If byte_swap_disable = 1, tag = {tag_3, tag_2, tag_1, tag_0}*/
#define AES_TAG_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_TAG_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_GHASH_KEY_x
  * @brief GHASH key register x
  * @{
  */
#define AES_MASK_GHASH_KEY_x   ((u32)0xFFFFFFFF << 0)           /*!< RW 0x0  GHASH key register bit[32*x+31:32*x] If using GCM mode without AAD, need to first calculate the ghash key and write it to the register. To generate the ghash key, apply the AES block cipher to the "zero" block with actual key. The ghash_keyorder depends on chnx_aes_reg.config.byte_swap_disable bit. If byte_swap_disable = 0, ghash_key = {ghash_key_3[7:0], ghash_key_3[15:8], ghash_key_3[23:16], ghash_key_3[31:24], ghash_key_2[7:0], ghash_key_2[15:8], ghash_key_2[23:16], ghash_key_2[31:24], ghash_key_1[7:0], ghash_key_1[15:8], ghash_key_1[23:16], ghash_key_1[31:24], ghash_key_0[7:0], ghash_key_0[15:8], ghash_key_0[23:16], ghash_key_0[31:24]}; If byte_swap_disable = 1, ghash_key = {ghash_key_3, ghash_key_2, ghash_key_1, ghash_key_0}*/
#define AES_GHASH_KEY_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_GHASH_KEY_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_BYTE_SWAP_DISABLE
  * @brief Byte swap control register
  * @{
  */
#define AES_BIT_GHASH_KEY_SWAP_DISABLE ((u32)0x00000001 << 5) /*!< RW 0x0  1: disable swap ghash_key_x*/
#define AES_BIT_TAG_SWAP_DISABLE       ((u32)0x00000001 << 4) /*!< RW 0x0  1: disable swap tag_x*/
#define AES_BIT_IV_SWAP_DISABLE        ((u32)0x00000001 << 3) /*!< RW 0x0  1: disable swap iv_x*/
#define AES_BIT_CPU_DATA_SWAP_DISABLE  ((u32)0x00000001 << 2) /*!< RW 0x0  1: disable swap cpu_datain_x and cpu_dataout_x*/
#define AES_BIT_KEY_SWAP_DISABLE       ((u32)0x00000001 << 1) /*!< RW 0x0  1: disable swap key_x or key_management input key*/
#define AES_BIT_PAYLOAD_SWAP_DISABLE   ((u32)0x00000001 << 0) /*!< RW 0x0  1: disable swap the data from mem*/
/** @} */

/** @defgroup AES_MUTEX_STATUS
  * @brief Engine mutex status register
  * @{
  */
#define AES_MASK_ENGINE_MUTEX_STATUS   ((u32)0x00000003 << 0)           /*!< RS/W0C 0x0  If read 0, lock mutex. If not 0, the engine is being used. 2'b01: secure domain lock mutex 2'b10: non-secure domain lock mutex Write 0 to unlock mutex*/
#define AES_ENGINE_MUTEX_STATUS(x)     (((u32)((x) & 0x00000003) << 0))
#define AES_GET_ENGINE_MUTEX_STATUS(x) ((u32)(((x >> 0) & 0x00000003)))
/** @} */

/** @defgroup AES_MUTEX_OVERWRITE
  * @brief Engine mutex overwrite register
  * @{
  */
#define AES_BIT_ENGINE_MUTEX_OVERWRITE ((u32)0x00000001 << 0) /*!< RW 0x0  Secure only. Allows secure domain overwrite mutex. 1: enable 0: disable*/
/** @} */

/** @defgroup AES_CHN0_SCA_MASK_x
  * @brief SCA mask reigster x
  * @{
  */
#define AES_MASK_CHN0_SCA_MASK_x   ((u32)0xFFFFFFFF << 0)           /*!< WO 0x0  Set a SCA mask seed when system initialization. SW should set a random number generated by TRNG to this register every boot.*/
#define AES_CHN0_SCA_MASK_x(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_CHN0_SCA_MASK_x(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_INTR_STATUS
  * @brief Interrupt status register
  * @{
  */
#define AES_BIT_CHN15_AES_INT_STATUS ((u32)0x00000001 << 15) /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 15.*/
#define AES_BIT_CHN14_AES_INT_STATUS ((u32)0x00000001 << 14) /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 14.*/
#define AES_BIT_CHN13_AES_INT_STATUS ((u32)0x00000001 << 13) /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 13.*/
#define AES_BIT_CHN12_AES_INT_STATUS ((u32)0x00000001 << 12) /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 12.*/
#define AES_BIT_CHN11_AES_INT_STATUS ((u32)0x00000001 << 11) /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 11.*/
#define AES_BIT_CHN10_AES_INT_STATUS ((u32)0x00000001 << 10) /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 10.*/
#define AES_BIT_CHN9_AES_INT_STATUS  ((u32)0x00000001 << 9)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 9.*/
#define AES_BIT_CHN8_AES_INT_STATUS  ((u32)0x00000001 << 8)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 8.*/
#define AES_BIT_CHN7_AES_INT_STATUS  ((u32)0x00000001 << 7)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 7.*/
#define AES_BIT_CHN6_AES_INT_STATUS  ((u32)0x00000001 << 6)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 6.*/
#define AES_BIT_CHN5_AES_INT_STATUS  ((u32)0x00000001 << 5)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 5.*/
#define AES_BIT_CHN4_AES_INT_STATUS  ((u32)0x00000001 << 4)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 4.*/
#define AES_BIT_CHN3_AES_INT_STATUS  ((u32)0x00000001 << 3)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 3.*/
#define AES_BIT_CHN2_AES_INT_STATUS  ((u32)0x00000001 << 2)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 2.*/
#define AES_BIT_CHN1_AES_INT_STATUS  ((u32)0x00000001 << 1)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 1.*/
#define AES_BIT_CHN0_AES_INT_STATUS  ((u32)0x00000001 << 0)  /*!< RO 0x0  Set to 1 when any AES interrupt occurs on channel 0.*/
/** @} */

/** @defgroup AES_CHN0_AES_INT_RAW
  * @brief Raw interrupt register
  * @{
  */
#define AES_BIT_ERROR_CPU_FIFO_RAW      ((u32)0x00000001 << 4) /*!< RO 0x0  Set to 1 when the user uses slave mode and writes data when input data fifo is full or reads data when output data fifo is empty. (error condition is discussed  discussed in config.wait_for_slave_dis)*/
#define AES_BIT_ERROR_ENDE_CIRCUIT_RAW  ((u32)0x00000001 << 3) /*!< RO 0x0  Set to 1 when the user uses the corresponding operation when closing the encryption or decryption circuit.*/
#define AES_BIT_ERROR_DECODE_CIPHER_RAW ((u32)0x00000001 << 2) /*!< RO 0x0  Set to 1 when the user sets a non-existent cipher mode.*/
#define AES_BIT_ERROR_BLOCK_SIZE_RAW    ((u32)0x00000001 << 1) /*!< RO 0x0  Set to 1 when the input size is not a multiple of 128 when using ECB or CBC mode.*/
#define AES_BIT_SLAVE_DONE_RAW          ((u32)0x00000001 << 0) /*!< RO 0x0  Set to 1 when aes has output data waiting for user to read  (tag reg or output data in output fifo)*/
/** @} */

/** @defgroup AES_CHN0_AES_INT_MASK
  * @brief Enable interrupt register
  * @{
  */
#define AES_BIT_ERROR_CPU_FIFO_MASK      ((u32)0x00000001 << 4) /*!< RW 0x1  Interrupt error_cpu_fifo enable.*/
#define AES_BIT_ERROR_ENDE_CIRCUIT_MASK  ((u32)0x00000001 << 3) /*!< RW 0x1  Interrupt error_ende_circuit enable.*/
#define AES_BIT_ERROR_DECODE_CIPHER_MASK ((u32)0x00000001 << 2) /*!< RW 0x1  Interrupt error_decode_cipher enable.*/
#define AES_BIT_ERROR_BLOCK_SIZE_MASK    ((u32)0x00000001 << 1) /*!< RW 0x1  Interrupt error_block_size enable.*/
#define AES_BIT_SLAVE_DONE_MASK          ((u32)0x00000001 << 0) /*!< RW 0x1  Interrupt aes_slave_done enable.*/
/** @} */

/** @defgroup AES_CHN0_AES_INT_STATUS
  * @brief Masked interrupt register
  * @{
  */
#define AES_BIT_ERROR_CPU_FIFO_STATUS      ((u32)0x00000001 << 4) /*!< RO 0x0  Masked error_cpu_fifo*/
#define AES_BIT_ERROR_ENDE_CIRCUIT_STATUS  ((u32)0x00000001 << 3) /*!< RO 0x0  Masked error_ende_circuit.*/
#define AES_BIT_ERROR_DECODE_CIPHER_STATUS ((u32)0x00000001 << 2) /*!< RO 0x0  Masked error_decode_cipher.*/
#define AES_BIT_ERROR_BLOCK_SIZE_STATUS    ((u32)0x00000001 << 1) /*!< RO 0x0  Masked error_block_size.*/
#define AES_BIT_SLAVE_DONE_STATUS          ((u32)0x00000001 << 0) /*!< RO 0x0  Masked aes_slave_done.*/
/** @} */

/** @defgroup AES_CHN0_AES_INT_CLEAR
  * @brief Clear interrupt register
  * @{
  */
#define AES_BIT_INT_CLEAR ((u32)0x00000001 << 0) /*!< WO 0x0  Interrupt clear. Writing 1'b1 to the register will trigger a 1-cycle pulse interrupt clear.*/
/** @} */

/** @defgroup AES_CHN_AES_HW_CONFIG_STATUS
  * @brief AES engine hw config status register
  * @{
  */
#define AES_BIT_SUPPORT_GCM       ((u32)0x00000001 << 19) /*!< RO 0x1  Indicate whether support gcm*/
#define AES_BIT_SUPPORT_XTS       ((u32)0x00000001 << 18) /*!< RO 0x1  Indicate whether support xts*/
#define AES_BIT_PRESENT           ((u32)0x00000001 << 17) /*!< RO 0x1  Indicate whether aes core is configured*/
#define AES_BIT_SM4_PRESENT       ((u32)0x00000001 << 16) /*!< RO 0x1  Indicate whether sm4 core is configured*/
#define AES_BIT_DMA_EN            ((u32)0x00000001 << 4)  /*!< RO 0x1  Indicate whether DMA is configured*/
#define AES_BIT_CORE_SELECT_LOCK  ((u32)0x00000001 << 3)  /*!< RO 0x0  Indicate whether the core_select was locked by hardware pin.*/
#define AES_BIT_KEY_MANAGEMENT_EN ((u32)0x00000001 << 2)  /*!< RO 0x0  Indicate whether to use key_management.*/
#define AES_BIT_TRUSTZONE_EN      ((u32)0x00000001 << 1)  /*!< RO 0x0  Indicate whether to use TrustZone.*/
#define AES_BIT_GF_MASK_EN        ((u32)0x00000001 << 0)  /*!< RO 0x1  Indicate whether to use SCA countermeasure.*/
/** @} */

/** @defgroup AES_CHN_AES_EN_DE_CIRCUIT
  * @brief Circuit status register
  * @{
  */
#define AES_MASK_EN_CIRCUIT   ((u32)0x0000FFFF << 16)           /*!< RO 0x0  Indicate which AES engine has the encryption circuit.*/
#define AES_EN_CIRCUIT(x)     (((u32)((x) & 0x0000FFFF) << 16))
#define AES_GET_EN_CIRCUIT(x) ((u32)(((x >> 16) & 0x0000FFFF)))
#define AES_MASK_DE_CIRCUIT   ((u32)0x0000FFFF << 0)            /*!< RO 0x0  Indicate which AES engine has the decryption circuit.*/
#define AES_DE_CIRCUIT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define AES_GET_DE_CIRCUIT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup AES_CHN_AES_VERSION
  * @brief Version of lalu_aes register
  * @{
  */
#define AES_MASK_VERSION   ((u32)0xFFFFFFFF << 0)           /*!< RO 0x0  indicate the verion of lalu_aes. For 25.0.0, should be 32'h0025_0000.*/
#define AES_VERSION(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_VERSION(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @defgroup AES_DATE
  * @brief Date register
  * @{
  */
#define AES_MASK_DATE   ((u32)0xFFFFFFFF << 0)           /*!< RO 0x0  The date that design was generated.*/
#define AES_DATE(x)     (((u32)((x) & 0xFFFFFFFF) << 0))
#define AES_GET_DATE(x) ((u32)(((x >> 0) & 0xFFFFFFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup AES_Exported_Types AES Exported Types
  * @{
  */

/** @brief AES Register Declaration
  */

typedef struct {
	__IO uint32_t AES_CPU_DATAIN_x[4]         ;  /*!< CPU DATA INPUT REGISTER x Register,  Address offset:0x000-0x00C */
	__I  uint32_t AES_CPU_DATAOUT_x[4]        ;  /*!< CPU DATA OUTPUT REGISTER x Register,  Address offset:0x010-0x01C */
	__I  uint32_t AES_KEY_DE_OUT_x[8]         ;  /*!< DECRYPTION KEY REGISTER x Register,  Address offset:0x020-0x03C */
	__IO uint32_t RSVD0[8]                    ;  /*!< Reserved,  Address offset:0x040-0x05F */
	__IO uint32_t AES_IV_x[4]                 ;  /*!< IV REGISTER x Register,  Address offset:0x060-0x06C */
	__IO uint32_t AES_CONFIG                  ;  /*!< CONFIGURATION REGISTER,  Address offset:0x070 */
	__IO uint32_t AES_CIPHER_LEN              ;  /*!< INPUT LENGTH REGISTER,  Address offset:0x074 */
	__IO uint32_t AES_GMAC_LEN_x[2]           ;  /*!< AAD LENGTH REGISTER x Register,  Address offset:0x078-0x07C */
	__IO uint32_t AES_TAG_x[4]                ;  /*!< TAG REGISTER x Register,  Address offset:0x080-0x08C */
	__IO uint32_t AES_GHASH_KEY_x[4]          ;  /*!< GHASH KEY REGISTER x Register,  Address offset:0x090-0x09C */
	__IO uint32_t AES_BYTE_SWAP_DISABLE       ;  /*!< BYTE SWAP CONTROL REGISTER,  Address offset:0x0A0 */
	__IO uint32_t RSVD1[791]                  ;  /*!< Reserved,  Address offset:0x0A4-0xCFF */
	__IO uint32_t AES_MUTEX_STATUS            ;  /*!< ENGINE MUTEX STATUS REGISTER,  Address offset:0xD00 */
	__IO uint32_t AES_MUTEX_OVERWRITE         ;  /*!< ENGINE MUTEX OVERWRITE REGISTER,  Address offset:0xD04 */
	__IO uint32_t RSVD2[62]                   ;  /*!< Reserved,  Address offset:0xD08-0xDFF */
	__O  uint32_t AES_CHN0_SCA_MASK_x[4]      ;  /*!< SCA MASK REIGSTER x Register,  Address offset:0xE00-0xE0C */
	__IO uint32_t RSVD3[52]                   ;  /*!< Reserved,  Address offset:0xE10-0xEDF */
	__I  uint32_t AES_INTR_STATUS             ;  /*!< INTERRUPT STATUS REGISTER,  Address offset:0xEE0 */
	__IO uint32_t RSVD4[3]                    ;  /*!< Reserved,  Address offset:0xEE4-0xEEF */
	__I  uint32_t AES_CHN0_AES_INT_RAW        ;  /*!< RAW INTERRUPT REGISTER,  Address offset:0xEF0 */
	__IO uint32_t AES_CHN0_AES_INT_MASK       ;  /*!< ENABLE INTERRUPT REGISTER,  Address offset:0xEF4 */
	__I  uint32_t AES_CHN0_AES_INT_STATUS     ;  /*!< MASKED INTERRUPT REGISTER,  Address offset:0xEF8 */
	__O  uint32_t AES_CHN0_AES_INT_CLEAR      ;  /*!< CLEAR INTERRUPT REGISTER,  Address offset:0xEFC */
	__IO uint32_t RSVD5[60]                   ;  /*!< Reserved,  Address offset:0xF00-0xFEF */
	__I  uint32_t AES_CHN_AES_HW_CONFIG_STATUS;  /*!< AES ENGINE HW CONFIG STATUS REGISTER,  Address offset:0xFF0 */
	__I  uint32_t AES_CHN_AES_EN_DE_CIRCUIT   ;  /*!< CIRCUIT STATUS REGISTER,  Address offset:0xFF4 */
	__I  uint32_t AES_CHN_AES_VERSION         ;  /*!< VERSION OF LALU_AES REGISTER,  Address offset:0xFF8 */
	__I  uint32_t AES_DATE                    ;  /*!< DATE REGISTER,  Address offset:0xFFC */
} AES_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */
/// @endcond

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your definition here


/* Exported Types --------------------------------------------------------*/
/** @addtogroup AES_Exported_Types AES Exported Types
  * @{
  */

/**
  * @brief  AES Init Structure Definition
  */
typedef struct {
	u32	CipherMode;		/*!< Specifies the AES cipher mode.
										This parameter can be a value of @ref AES_Cipher_Mode. */

	u32	EngineMode;		/*!< Specifies the AES engine mode.
										This parameter can be a value of @ref AES_Engine_Mode. */

	u32	IsEnc;			/*!< Specifies AES encrypt or decrypt.
										This parameter can be TRUE or FALSE. */

	u32	CipherLen;		/*!< Specifies the AES cipher length in bytes. */

	u32 key_len;	/*!< AES key length in bits. This parameter can be a value of @ref AES_Key_Size. */
} AES_InitTypeDef;
/** @} */

/* Exported constants --------------------------------------------------------*/
/** @defgroup AES_Exported_Constants AES Exported Constants
  * @{
  */

/** @defgroup Crypto_Block_Size Crypto Block Size
  * @{
  */
#define CIPHER_BLOCK_SIZE			16	/*!< AES/cipher block size in bytes */
/** @} */

/** @defgroup Crypto_Timeout Crypto Timeout
  * @{
  */
#define CRYPTO_DMA_TIMEOUT			((u32) 1000000)		/*!< DMA mode operation timeout count */
#define CRYPTO_MUTEX_TIMEOUT			((u32) 1000000)		/*!< Engine mutex acquisition timeout count */
#define CRYPTO_SLAVE_TIMEOUT			((u32) 10000)		/*!< Slave mode operation timeout count */
/** @} */

/** @defgroup AES_Key_Size AES Key Size
  * @{
  */
#define AES_KEY_SIZE_128				((u8)0x00)	/*!< Key size 128 bits */
#define AES_KEY_SIZE_192				((u8)0x01)	/*!< Key size 192 bits */
#define AES_KEY_SIZE_256				((u8)0x02)	/*!< Key size 256 bits */
/** @} */

/** @defgroup AES_Cipher_Mode AES Cipher Mode
  * @{
  */
#define CIPHER_ENCRYPTION_MODE  ((u8)0x01)	/*!< AES encryption direction */
#define CIPHER_DECRYPTION_MODE  ((u8)0x00)	/*!< AES decryption direction */

#define CIPHER_MODE_ECB			((u8)0x00)	/*!< Electronic Codebook mode */
#define CIPHER_MODE_CBC			((u8)0x01)	/*!< Cipher Block Chaining mode */
#define CIPHER_MODE_CFB			((u8)0x02)	/*!< Cipher Feedback mode */
#define CIPHER_MODE_OFB			((u8)0x03)	/*!< Output Feedback mode */
#define CIPHER_MODE_CTR			((u8)0x04)	/*!< Counter mode */
#define CIPHER_MODE_GMAC		((u8)0x05)	/*!< Galois Message Authentication Code mode */
#define CIPHER_MODE_GCM			((u8)0x06)	/*!< Galois/Counter Mode (authenticated encryption) */
#define CIPHER_MODE_CMAC		((u8)0x07)	/*!< Cipher-based Message Authentication Code mode */
#define CIPHER_MODE_CBC_MAC	((u8)0x09)	/*!< CBC-MAC mode */
#define CIPHER_MODE_CCM		  ((u8)0x0A)	/*!< Counter with CBC-MAC mode (authenticated encryption) */

/** @brief Check whether MODE is a valid AES cipher mode. */
#define IS_AES_CIPHER_MODE(MODE) (((MODE) == CIPHER_MODE_ECB) || \
								((MODE) == CIPHER_MODE_CBC) || \
								((MODE) == CIPHER_MODE_CFB) || \
								((MODE) == CIPHER_MODE_OFB) || \
								((MODE) == CIPHER_MODE_CTR) || \
								((MODE) == CIPHER_MODE_GMAC) || \
								((MODE) == CIPHER_MODE_GCM) || \
								((MODE) == CIPHER_MODE_CMAC))

/** @} */

/** @defgroup AES_DMA_Settings AES DMA Settings
  * @{
  */
#define AES_DMA_CH_NUM                          0       /*!< DMA channel number for AES transfers */
#define AES_DMA_CH_MASK                         ((u32)0x00000001 << AES_DMA_CH_NUM)  /*!< DMA channel enable mask bit for AES */
/** @} */

/** @defgroup AES_Byte_Swap_Defaults AES Byte Swap Defaults
  * @{
  */
#define AES_BYTE_SWAP_DEFAULTS AES_BIT_KEY_SWAP_DISABLE	/*!< Default byte swap: key in native order, no swap */
#define AES_BYTE_SWAP_KEY_SWAP ((u32) 0x00)				/*!< Byte swap setting: key swap disabled (both key and payload in native order) */
/** @} */

/** @defgroup AES_Engine_Mode AES Engine Mode
  * @{
  */
#define AES_SLAVE_MODE			((u8)0x00)	/*!< Slave mode: plaintext written directly to hardware registers */
#define AES_DMA_MODE			((u8)0x01)	/*!< DMA mode: plaintext transferred via DMA */

/** @brief Check whether MODE is a valid AES engine mode. */
#define IS_AES_ENGINE_MODE(MODE) (((MODE) == AES_SLAVE_MODE) || \
								((MODE) == AES_DMA_MODE))
/** @} */

/** @defgroup AES_TIMEOUT AES Timeout
  * @{
  */
#define AES_DMA_TIMEOUT			((u32) 10000000)	/*!< DMA mode operation timeout count */
#define AES_MUTEX_TIMEOUT		((u32) 1000000)		/*!< Engine mutex acquisition timeout count */
#define AES_SLAVE_TIMEOUT		((u32) 10000)		/*!< Slave mode operation timeout count */
/** @} */

/** @defgroup AES_DMA_SIZE AES DMA Size
  * @{
  */
#define AES_BLOCK_SIZE            16			/*!< AES block size in bytes */
#define AES_BLOCK_SIZE_BIT        128			/*!< AES block size in bits */
#define AES_DMA_MAX_BLOCK_TS      536870910		/*!< Maximum DMA block transfer size in transfer units */
#define AES_DMA_MAX_BLOCK_BYTE    (AES_DMA_MAX_BLOCK_TS * 1)	/*!< Maximum DMA transfer size in bytes */
/** @} */

/// @cond
/** @brief Poll for AES slave-mode cipher completion; set ret and jump to exit on error or timeout. */
#define WAIT_FOR_AES_SLAVE_CIPHER_COMPLETION() \
do { \
    while (1) { \
        if ((AES)->AES_CHN0_AES_INT_RAW & AES_BIT_SLAVE_DONE_RAW) { \
			ret = RTK_SUCCESS; \
            break; \
        } \
        \
        if ((AES)->AES_CHN0_AES_INT_RAW & \
            (AES_BIT_ERROR_DECODE_CIPHER_RAW | AES_BIT_ERROR_BLOCK_SIZE_RAW | AES_BIT_ERROR_ENDE_CIRCUIT_RAW)) { \
            ret = (AES)->AES_CHN0_AES_INT_RAW; \
            goto exit; \
        } \
        \
        (loopWait)--; \
        if ((loopWait) == 0) { \
            ret = _ERRNO_CRYPTO_SLAVE_TIME_OUT; \
            goto exit; \
        } \
    } \
} while(0)

/** @brief Poll for AES slave-mode MAC completion; set ret and jump to exit on error or timeout. */
#define WAIT_FOR_AES_SLAVE_MAC_COMPLETION() \
do { \
    while (1) { \
        if ((AES)->AES_CONFIG & AES_BIT_IDLE) { \
			ret = RTK_SUCCESS; \
            break; \
        } \
        \
        if ((AES)->AES_CHN0_AES_INT_RAW & \
            (AES_BIT_ERROR_DECODE_CIPHER_RAW | AES_BIT_ERROR_BLOCK_SIZE_RAW | AES_BIT_ERROR_ENDE_CIRCUIT_RAW)) { \
            ret = (AES)->AES_CHN0_AES_INT_RAW; \
            goto exit; \
        } \
        \
        (loopWait)--; \
        if ((loopWait) == 0) { \
            ret = _ERRNO_CRYPTO_SLAVE_TIME_OUT; \
            goto exit; \
        } \
    } \
} while(0)

/** @brief Set AES_CONFIG key length bits based on key_len_bits value; jump to exit on invalid length. */
#define SET_AES_CONFIG_KEY_LEN(key_len_bits, Temp) \
do { \
    switch (key_len_bits) { \
        case KEY_BIT_128: \
            (Temp) |= AES_KEY_SIZE(AES_KEY_SIZE_128); \
            break; \
        case KEY_BIT_192: \
            (Temp) |= AES_KEY_SIZE(AES_KEY_SIZE_192); \
            break; \
        case KEY_BIT_256: \
            (Temp) |= AES_KEY_SIZE(AES_KEY_SIZE_256); \
            break; \
        default: \
            ret = _ERRNO_CRYPTO_KEY_LENGTH_ERR; \
            goto exit; \
    } \
} while(0)
/// @endcond

/** @} */

/* Exported functions --------------------------------------------------------*/
/** @defgroup AES_Exported_Functions AES Exported Functions
  * @{
  */

/**
 * @brief Perform AES-ECB mode encryption and decryption in slave mode.
 *        In order to avoid the impact of 32-byte cache line alignment,
 *        an additional slave mode interface is provided.
 *        Input and output are limited to 16 bytes.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Address to software key array
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ecb_slave(u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 output[16]);

/**
 * @brief Perform AES-CTR mode encryption and decryption in slave mode.
 *        In order to avoid the impact of 32-byte cache line alignment,
 *        an additional slave mode interface is provided.
 *        Input and output are limited to 16 bytes.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Address to software key array
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  iv IV array address. IV length must be 16 bytes.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ctr_slave(u8 key_id, u32 key_len_bits, u8 *key_addr, u8 is_encryption, const u8 input[16], u8 iv[16], u8 output[16]);

/**
 * @brief Perform AES-ECB encryption and decryption in DMA mode.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ecb(u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 *output);

/**
 * @brief Perform AES-CBC encryption and decryption in DMA mode.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16 bytes.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_cbc(u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Perform AES-CFB encryption and decryption in DMA mode.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16 bytes.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_cfb(u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Perform AES-OFB encryption and decryption in DMA mode.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16 bytes.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ofb(u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Perform AES-CTR encryption and decryption in DMA mode.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param is_encryption
 * 		 @arg 1: Encryption
 * 		 @arg 0: Decryption
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  iv IV array address. IV length must be 16 bytes.
 * @param  output The address of the output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ctr(u8 key_id, u32 key_len_bits, u8 is_encryption, const u8 *input, u32 len, u8 iv[16], u8 *output);

/**
 * @brief Compute AES-CMAC authentication tag.
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  input The address of the input data.
 * @param  len The length of input data.
 * @param  tag Pointer to the output tag buffer
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of encrypted and decrypted data must be 32-byte (cache line size) aligned.
 *           At the same time, the destination address is also 32-byte aligned. Otherwise, an error will be reported.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_cmac(u8 key_id, u32 key_len_bits, const u8 *input, u32 len, u8 *tag);

/**
 * @brief Perform AES-GCM mode encryption and tag generation (DMA mode).
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  len The length of the plaintext.
 * @param  iv_addr IV array address.
 * @param  iv_len IV length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the plaintext data.
 * @param  output The address of the ciphertext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_gcm_encrypt_and_tag(u8 key_id, u32 key_len_bits, u32 len, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Perform AES-GCM mode authenticated decryption (DMA mode).
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  len The length of the ciphertext.
 * @param  iv_addr IV array address.
 * @param  iv_len IV length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the ciphertext data.
 * @param  output The address of the plaintext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag input data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_gcm_authenticated_decrypt(u8 key_id, u32 key_len_bits, u32 len, u8 *iv_addr, u32 iv_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Perform AES-CCM mode encryption and tag generation (DMA mode).
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  msg_len The length of the plaintext.
 * @param  nonce_addr Nonce array address.
 * @param  nonce_len Nonce length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the plaintext data.
 * @param  output The address of the ciphertext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag output data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ccm_encrypt_and_tag(u8 key_id, u32 key_len_bits, u8 *key_addr, u32 msg_len, u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/**
 * @brief Perform AES-CCM mode authenticated decryption (DMA mode).
 * @param  key_id Value of @ref KM_KEY
 * @param  key_len_bits
 *		 @arg KEY_BIT_128
 *		 @arg KEY_BIT_192
 *		 @arg KEY_BIT_256
 * @param  key_addr Software key address, set to NULL if use hardware key
 * @param  msg_len The length of the ciphertext.
 * @param  nonce_addr Nonce array address.
 * @param  nonce_len Nonce length.
 * @param  aad_addr The address of the AAD data.
 * @param  aad_len The length of the AAD data.
 * @param  input The address of the ciphertext data.
 * @param  output The address of the plaintext data.
 * @param  tag_len The length of the tag.
 * @param  tag The address of the tag input data.
 * @note
 *         - The engine will be locked during the execution of this function,
 *           and will be unlocked after execution.
 *           Avoid multiple threads/CPUs using the AES engine at the same time.
 *         - The length of input data, output data and aad data must be 32-byte (cache line size) aligned.
 * @return Process status:
 *         - 0: success
 *         - Other: error code. Refer to @ref CRYPTO_Process_Status
 */
_LONG_CALL_ int crypto_aes_ccm_auth_decrypt(u8 key_id, u32 key_len_bits, u8 *key_addr, u32 msg_len, u8 *nonce_addr, u32 nonce_len, u8 *aad_addr, u32 aad_len,
		const u8 *input, u8 *output, u32 tag_len, u8 *tag);

/** @} */
/** @} */

/* AES hardware functions */
/**
 * @brief Return different AES crypto engine register addresses according to the security state
 * @return AES_REG_BASE or AES_REG_BASE_S
 */
_LONG_CALL_ AES_TypeDef *AES_Get_AES_Addr(void);

/**
 * @brief Return different crypto engine DMA register addresses according to the security state
 * @return AES_SHA_DMA_REG_BASE or AES_SHA_DMA_REG_BASE_S
 */
_LONG_CALL_ GDMA_TypeDef *AES_Get_DMA_Addr(void);

/**
 * @brief Lock the mutex by reading the mutex register before accessing the crypto engine.
 *        Without locking, engine registers cannot be accessed.
 *        This driver calls this API internally. Do not call this API from application code.
 */
_LONG_CALL_ void AES_lock_mutex(void);

/**
 * @brief Release the engine mutex after the crypto engine has been used.
 *        This driver calls this API internally. Do not call this API from application code.
 */
_LONG_CALL_ void AES_unlock_mutex(void);

/**
 * @brief Initialize the DMA engine. Configures DMA channel 0 to: SRC/DST TrWidthOneByte and MsizeFour.
 */
_LONG_CALL_ void AES_DMA_Init(void);

/**
 * @brief Set the value of the initialization vector.
 * @param  iv IV array address. IV length must be 16 bytes.
 * @note  For AES-GCM mode, use AES_GCM_Set_Iv() to set the IV; this function is for all other modes.
 */
_LONG_CALL_ void AES_SetIv(u8 iv[16]);

/**
 * @brief Get the AES IV value.
 * @param  iv Pointer to read out iv array. IV length must be 16 bytes.
 */
_LONG_CALL_ void AES_GetIv(u8 iv[16]);

/**
 * @brief Output the TAG value in CMAC and GCM mode.
 *        The full tag length is 16 bytes. If tag_len is less than 16, only the first tag_len bytes of the 16-byte tag are returned.
 * @param  tag Pointer to the output tag address
 * @param  tag_len Max output length is 16 bytes
 */
_LONG_CALL_ void AES_GetTag(u8 *tag, u32 tag_len);

/**
 * @brief Clear AES TAG registers to zero.
 */
_LONG_CALL_ void AES_InitTag(void);

/**
 * @brief  Set or clear the FIRST_BLOCK bit to indicate that the AES task starts from the first block.
 * @param  NewState
 *  @arg ENABLE: Set FIRST_BLOCK to 1. In DMA mode, set this as the last step before starting AES.
 *  @arg DISABLE: Clear FIRST_BLOCK to 0.
 * @note In DMA mode, when the engine is not idling, registers such as keys and IV cannot be modified.
 */
_LONG_CALL_ void AES_Cmd(u32 NewState);

/**
 * @brief Perform DMA mode data input and output.
 * @param  input The address of the input data.
 * @param  len The length of the data.
 * @param  output The address of the output data. Note that cache line alignment is required
 * @return RTK_SUCCESS or CRYPTO_Process_Status
 */
_LONG_CALL_ int AES_ProcessDma(const u8 *input, u32 len, u8 *output);

/**
 * @brief Enable SCA mask and fill the SCA mask registers with random values.
 */
_LONG_CALL_ void AES_Set_SCA_Mask(void);

/**
 * @brief Set the IV for GCM mode. Supports standard 12-byte IV and arbitrary-length IV.
 * @param  iv_addr The address of the IV data.
 * @param  iv_len The length of the IV in bytes.
 * @return RTK_SUCCESS or CRYPTO_Process_Status
 */
_LONG_CALL_ int AES_GCM_Set_Iv(u8 *iv_addr, u32 iv_len);

/** @} */

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */
#endif