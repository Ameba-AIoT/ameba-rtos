/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_PWMTIMER_H_
#define _AMEBA_PWMTIMER_H_

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup PWMTIMER
  * @brief PWMTIMER driver modules
  * @{
  */

/* AUTO_GEN_START */
// Do NOT modify any AUTO_GEN code below

/* Registers Definitions --------------------------------------------------------*/

/** @defgroup PWMTIMER_Register_Definitions PWMTIMER Register Definitions
  * @{
  */

/** @defgroup EN
  * @brief TIM enable register
  * @{
  */
#define TIM_BIT_CNT_EN    ((u32)0x00000001 << 16) /*!< R 0x0  counter working status * 0: counter is stoppped * 1: counter is working*/
#define TIM_BIT_CEN       ((u32)0x00000001 << 8)  /*!< R 0x0  counter run status * 0: counter is disabled * 1: counter is enabled*/
#define TIM_BIT_CNT_STOP  ((u32)0x00000001 << 1)  /*!< WA0 0x0  disable the counter. Poll CNT_RUN to see the counter status. If CNT_RUN is 0, it means that the counter has been disabled internally.*/
#define TIM_BIT_CNT_START ((u32)0x00000001 << 0)  /*!< WA0 0x0  enable the counter.  Poll CNT_RUN to see the counter status. If CNT_RUN is 1, it means that the counter has been enabled internally.*/
/** @} */

/** @defgroup CR
  * @brief TIM control register
  * @{
  */
#define TIM_MASK_ETP   ((u32)0x00000003 << 8)           /*!< R/W 0x0  external trigger polarity(TRGI) * 0: positive edge is active * 1: negative edge is active * 2 or 3: both edge is active Note: these bits are only valid in one-pulse mode.*/
#define TIM_ETP(x)     (((u32)((x) & 0x00000003) << 8))
#define TIM_GET_ETP(x) ((u32)(((x >> 8) & 0x00000003)))
#define TIM_BIT_ARPE   ((u32)0x00000001 << 4)           /*!< R/W 0x0  enable auto-reload preloading * 0: TIM_ARR register is not buffered * 1: TIM_ARR register is buffered*/
#define TIM_BIT_OPM    ((u32)0x00000001 << 3)           /*!< R/W 0x0  one-pulse mode * 0: counter is not stopped at UEV * 1: counter stops counting at next UEV*/
#define TIM_BIT_URS    ((u32)0x00000001 << 2)           /*!< R/W 0x0  update request source * 0: counter overflow and setting the UG bit * 1: counter overflow*/
#define TIM_BIT_UDIS   ((u32)0x00000001 << 1)           /*!< R/W 0x0  disable UEV. Shadow registers keep their values. Otherwise, buffered registers are then loaded with their preload values when UEV happen.*/
/** @} */

/** @defgroup DIER
  * @brief TIM interupt enable register
  * @{
  */
#define TIM_BIT_UIE5  ((u32)0x00000001 << 21) /*!< R/W 0x0  enable channel 5 update interrupt*/
#define TIM_BIT_UIE4  ((u32)0x00000001 << 20) /*!< R/W 0x0  enable channel 4 update interrupt*/
#define TIM_BIT_UIE3  ((u32)0x00000001 << 19) /*!< R/W 0x0  enable channel 3 update interrupt*/
#define TIM_BIT_UIE2  ((u32)0x00000001 << 18) /*!< R/W 0x0  enable channel 2 update interrupt*/
#define TIM_BIT_UIE1  ((u32)0x00000001 << 17) /*!< R/W 0x0  enable channel 1 update interrupt*/
#define TIM_BIT_UIE0  ((u32)0x00000001 << 16) /*!< R/W 0x0  enable channel 0 update interrupt*/
#define TIM_BIT_CC5IE ((u32)0x00000001 << 6)  /*!< R/W 0x0  enable Capture/Compare 5 interrupt*/
#define TIM_BIT_CC4IE ((u32)0x00000001 << 5)  /*!< R/W 0x0  enable Capture/Compare 4 interrupt*/
#define TIM_BIT_CC3IE ((u32)0x00000001 << 4)  /*!< R/W 0x0  enable Capture/Compare 3 interrupt*/
#define TIM_BIT_CC2IE ((u32)0x00000001 << 3)  /*!< R/W 0x0  enable Capture/Compare 2 interrupt*/
#define TIM_BIT_CC1IE ((u32)0x00000001 << 2)  /*!< R/W 0x0  enable Capture/Compare 1 interrupt*/
#define TIM_BIT_CC0IE ((u32)0x00000001 << 1)  /*!< R/W 0x0  enable Capture/Compare 0 interrupt*/
#define TIM_BIT_UIE   ((u32)0x00000001 << 0)  /*!< R/W 0x0  enable counter update interrupt*/
/** @} */

/** @defgroup SR
  * @brief TIM status register
  * @{
  */
#define TIM_BIT_UG_DONE ((u32)0x00000001 << 31) /*!< R 0x1  UG operation status This bit is cleared by hardware when the UG bit in the TIMx_EGR register is set. When the UG operation is done, hardware set this bit to '1'. So, software can poll this bit to see the UG operation status.*/
#define TIM_BIT_UIF5    ((u32)0x00000001 << 21) /*!< RW1C 0x0  refer to UIF0 description*/
#define TIM_BIT_UIF4    ((u32)0x00000001 << 20) /*!< RW1C 0x0  refer to UIF0 description*/
#define TIM_BIT_UIF3    ((u32)0x00000001 << 19) /*!< RW1C 0x0  refer to UIF0 description*/
#define TIM_BIT_UIF2    ((u32)0x00000001 << 18) /*!< RW1C 0x0  refer to UIF0 description*/
#define TIM_BIT_UIF1    ((u32)0x00000001 << 17) /*!< RW1C 0x0  refer to UIF0 description*/
#define TIM_BIT_UIF0    ((u32)0x00000001 << 16) /*!< RW1C 0x0  update interrupt flag of channel0 This flag is set by hardware when the UIE0 bit in the TIMx_EGR register is set, and update interrupt of channel0 happens.*/
#define TIM_BIT_CC5IF   ((u32)0x00000001 << 6)  /*!< RW1C 0x0  refer to CC0IF description*/
#define TIM_BIT_CC4IF   ((u32)0x00000001 << 5)  /*!< RW1C 0x0  refer to CC0IF description*/
#define TIM_BIT_CC3IF   ((u32)0x00000001 << 4)  /*!< RW1C 0x0  refer to CC0IF description*/
#define TIM_BIT_CC2IF   ((u32)0x00000001 << 3)  /*!< RW1C 0x0  refer to CC0IF description*/
#define TIM_BIT_CC1IF   ((u32)0x00000001 << 2)  /*!< RW1C 0x0  refer to CC0IF description*/
#define TIM_BIT_CC0IF   ((u32)0x00000001 << 1)  /*!< RW1C 0x0  Capture/Compare 0 interrupt flag channel CC0 configured as output: This flag is set by hardware when the counter TIM_CNT value matches the value of TIM_CCR0 register. It is cleared by software. Channel CC0 configured as input: This flag is set by hardware and the counter value has been captured in CCR0 field of TIM_CCR0 register when an active edge has been  detected. It is cleared by software.*/
#define TIM_BIT_UIF     ((u32)0x00000001 << 0)  /*!< RW1C 0x0  update interrupt flag*/
/** @} */

/** @defgroup EGR
  * @brief TIM event generation register
  * @{
  */
#define TIM_BIT_CC5G ((u32)0x00000001 << 6) /*!< WA0 0x0  refer to CC0G description*/
#define TIM_BIT_CC4G ((u32)0x00000001 << 5) /*!< WA0 0x0  refer to CC0G description*/
#define TIM_BIT_CC3G ((u32)0x00000001 << 4) /*!< WA0 0x0  refer to CC0G description*/
#define TIM_BIT_CC2G ((u32)0x00000001 << 3) /*!< WA0 0x0  refer to CC0G description*/
#define TIM_BIT_CC1G ((u32)0x00000001 << 2) /*!< WA0 0x0  refer to CC0G description*/
#define TIM_BIT_CC0G ((u32)0x00000001 << 1) /*!< WA0 0x0  Capture/Compare 0 generation This bit is set by software and is automatically cleared by hardware. channel CC0 configured as output: The CC0IF flag is set, the corresponding interrupt or  request is sent if enabled.  Channel CC0 configured as input: The current value of counter is capture in CCR0 field of TIM_CCR0 register. The CC0IF flag is set, the corresponding interrupt or  request is sent if enabled.*/
#define TIM_BIT_UG   ((u32)0x00000001 << 0) /*!< WA0 0x0  update generation. Re-initialize the counter and generate an update of registers. Note that the prescaler counter is cleared too.(anyway the prescaler ratio is not affected)*/
/** @} */

/** @defgroup CNT
  * @brief TIM counter
  * @{
  */
#define TIM_MASK_CNT   ((u32)0x0000FFFF << 0)           /*!< R 0x0  counter value*/
#define TIM_CNT(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_CNT(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PSC
  * @brief TIM prescaler register
  * @{
  */
#define TIM_MASK_PSC   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  prescaler value. PSC contains the value to be loaded in the actual prescaler register at each UEV(including when the counter is cleared through UG bit of TIM_EGR register).*/
#define TIM_PSC(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_PSC(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup ARR
  * @brief TIM auto-reload register
  * @{
  */
#define TIM_MASK_ARR   ((u32)0x0000FFFF << 0)           /*!< R/W 0xFFFF  the value to be loaded in the actual auto-reload register. It can be preloaded by setting ARPE bit in TIM_CR.*/
#define TIM_ARR(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_ARR(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup CCRx
  * @brief TIM Capture/Compare register x
  * @{
  */
#define TIM_BIT_OPM_DLx ((u32)0x00000001 << 29)          /*!< R/W 0x0  One Pulse Mode default level 0: default level is 0 1: default level is 1 this bit takes effect only in PWM output mode's One-Pulse-Mode*/
#define TIM_BIT_CCxM    ((u32)0x00000001 << 27)          /*!< R/W 0x0  CCx working mode * 0: PWM mode * 1: Input capture mode*/
#define TIM_BIT_CCxP    ((u32)0x00000001 << 26)          /*!< R/W 0x0  CCx channel configured as output: * 0: OCx active high * 1: OCx ative low CCx channel configured as input: * 0: positive edge of TRGI is active for capture * 1: negative edge of TRGI is active for capture*/
#define TIM_BIT_OCxPE   ((u32)0x00000001 << 25)          /*!< R/W 0x0  enable output capture x preload * 0: disable preload register on CCRx. CCRx can be written at any time, the new value is taken into account immediately. * 1: enable preload register on CCRx. Read/Write operations access the preload register. CCRx preload value is loaded in the active register at each UEV.*/
#define TIM_BIT_CCxE    ((u32)0x00000001 << 24)          /*!< R/W 0x0  enable CCx*/
#define TIM_MASK_CCRx   ((u32)0x0000FFFF << 0)           /*!< R/W/ES 0x0  Capture/Compare x value CCx channel configured as output: CCRx is the value to be loaded in the actual capture/compare x register(preload value). It is loaded permanently if the preload feature is not selected in the OCxPE bit.Else the preload value is copied in the active capture/compare x register when an UEV occurs.The active capture/compare x register contains the value to be compared to the counter TIM_CNT and signalled on OCx output. CCx channel configured as pulse mode 1: CCRx is the counter value transferred by the last input capture event(TRGI)*/
#define TIM_CCRx(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_CCRx(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PSYNCx
  * @brief TIM Phase Synchronization register x
  * @{
  */
#define TIM_BIT_SYNCENx       ((u32)0x00000001 << 27)          /*!< R/W 0x0  enable phase sync function in channel x. Phase sync takes effect only in output PWM mode.*/
#define TIM_BIT_SYNCDIRx      ((u32)0x00000001 << 26)          /*!< R/W 0x0  phase sync direction to the reference counter: 0:delay to the reference counter 1:ahead to the reference counter*/
#define TIM_BIT_SYNCPEx       ((u32)0x00000001 << 25)          /*!< R/W 0x0  enable sync phase parameter preload * 0: disable preload register on PSYNCx. PSYNCx can be written at any time, the new value is taken into account immediately. * 1: enable preload register on PSYNCx. Read/Write operations access the preload register. PSYNCx preload value is loaded in the active register at each UEV.*/
#define TIM_MASK_SYNCPHASEx   ((u32)0x0000FFFF << 0)           /*!< R/W 0x0  the deviation value of channelx to the reference counter*/
#define TIM_SYNCPHASEx(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_SYNCPHASEx(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup PHASECNTx
  * @brief TIM phase counter register x
  * @{
  */
#define TIM_MASK_CNTx   ((u32)0x0000FFFF << 0)           /*!< R 0x0  phase counter value of channelx*/
#define TIM_CNTx(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_CNTx(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @defgroup AUDx
  * @brief
  * @{
  */
#define TIM_MASK_CHx_DEADZONE_TIM       ((u32)0x000007FF << 9)           /*!< R/W 11'd0  deadzone time.  The count cycle is same as the ARR count clock.*/
#define TIM_CHx_DEADZONE_TIM(x)         (((u32)((x) & 0x000007FF) << 9))
#define TIM_GET_CHx_DEADZONE_TIM(x)     ((u32)(((x >> 9) & 0x000007FF)))
#define TIM_BIT_CHx_FIFO_EMPTY_MODE_SEL ((u32)0x00000001 << 8)           /*!< R/W 0x0  When the counter is working and FIFO is empty, two working states can be selected 0: main last data 1: change to break state*/
#define TIM_BIT_CHx_DEADZONE_EN         ((u32)0x00000001 << 7)           /*!< R/W 0x0  Enable dead zone control function*/
#define TIM_BIT_CHx_PAD_OUT_MODE_SEL    ((u32)0x00000001 << 6)           /*!< R/W 0x0  Used to select the correspondence between ch and output pad. 0:Two chns control two output pad 1:One chn controls two output pad*/
#define TIM_BIT_CHx_PHASE_SEL           ((u32)0x00000001 << 5)           /*!< R/W 0x0  Select output phase in audio/motor scenario 0:opposite phase 1:Same phase*/
#define TIM_BIT_CHx_CCR_DATA_SEL        ((u32)0x00000001 << 4)           /*!< R/W 0x0  Select CCR data source 0: cpu 1: fifo*/
#define TIM_BIT_CHx_EMG_BRK_EN          ((u32)0x00000001 << 3)           /*!< R/W 0x0  Enable ch emergency braking function*/
#define TIM_BIT_CHx_EMG_BRK_ST          ((u32)0x00000001 << 2)           /*!< R/W 0x0  Emergency braking state*/
#define TIM_BIT_CHx_BRK_ST              ((u32)0x00000001 << 0)           /*!< R/W 0x0  Ini and braking  state*/
/** @} */

/** @defgroup ISR
  * @brief
  * @{
  */
#define TIM_BIT_IMR_EMG_BRK_RELEASE ((u32)0x00000001 << 3) /*!< R/W 0x0  0: mask isr 1:    enable isr*/
#define TIM_BIT_IMR_WORK_FIFO_EMPTY ((u32)0x00000001 << 2) /*!< R/W 0x0  0: mask isr 1:    enable isr*/
#define TIM_BIT_ISR_EMG_BRK_RELEASE ((u32)0x00000001 << 1) /*!< RW1C 0x0  Emergency brake release event triggers the int this interrupt.*/
#define TIM_BIT_ISR_WORK_FIFO_EMPTY ((u32)0x00000001 << 0) /*!< RW1C 0x0  When the counter is working and encounters an empty FIFO, it will trigger this interrupt*/
/** @} */

/** @defgroup CFG
  * @brief
  * @{
  */
#define TIM_BIT_CH_BRK_UP             ((u32)0x00000001 << 31)           /*!< R/W/ES 0x0  After configuring the braking status of all CHs, the software needs to raise this signal, and the hardware update will lower this signal.*/
#define TIM_MASK_CH_ID_DEADZONE_MAX   ((u32)0x00000007 << 28)           /*!< R/W 0x0  reserved*/
#define TIM_CH_ID_DEADZONE_MAX(x)     (((u32)((x) & 0x00000007) << 28))
#define TIM_GET_CH_ID_DEADZONE_MAX(x) ((u32)(((x >> 28) & 0x00000007)))
#define TIM_BIT_EMG_BRK_TRIG_POLARITY ((u32)0x00000001 << 27)           /*!< R/W 0x0  0: high level to trigger emergency braking 1:low level to trigger emergency braking*/
#define TIM_MASK_EMG_BRK_DBC_CNT      ((u32)0x0000FFFF << 1)            /*!< R/W 0x0  Debounce count for UPS funciton*/
#define TIM_EMG_BRK_DBC_CNT(x)        (((u32)((x) & 0x0000FFFF) << 1))
#define TIM_GET_EMG_BRK_DBC_CNT(x)    ((u32)(((x >> 1) & 0x0000FFFF)))
#define TIM_BIT_EMG_BRK_DBC_FEN       ((u32)0x00000001 << 0)            /*!< R/W 0x0  Debounce count for UPS funciton*/
/** @} */

/** @defgroup DMA
  * @brief
  * @{
  */
#define TIM_MASK_TXDMA_MSIZE   ((u32)0x00000007 << 4)           /*!< R/W 0x0  txdma burst size. When fifo reach certain entry depth , controller will req dma operation. 0: 1 entry 1: 4 2: 8 N: 2**(N+1) Note: this setting must be same with GDMA burst len , or  fifo will underflow or overflow.*/
#define TIM_TXDMA_MSIZE(x)     (((u32)((x) & 0x00000007) << 4))
#define TIM_GET_TXDMA_MSIZE(x) ((u32)(((x >> 4) & 0x00000007)))
#define TIM_BIT_TXDMA_EN       ((u32)0x00000001 << 0)           /*!< R/W 0x0  1:enable txdma Note: dma interface must used GDMA as flow control , IP flow control is not support .*/
/** @} */

/** @defgroup FIFO
  * @brief
  * @{
  */
#define TIM_MASK_FIFO_AF_LEVEL   ((u32)0x0000003F << 3)           /*!< R/W 6'd1  When writing a FIFO for the first time, you can configure this reg to allow the FIFO to reach a certain number before starting to read. Generally greater than the number of ch working in FIFO. The calculation method  is 32-fifo_af_level.*/
#define TIM_FIFO_AF_LEVEL(x)     (((u32)((x) & 0x0000003F) << 3))
#define TIM_GET_FIFO_AF_LEVEL(x) ((u32)(((x >> 3) & 0x0000003F)))
#define TIM_BIT_FIFO_WFULL       ((u32)0x00000001 << 2)           /*!< R 0x0  fifo full*/
#define TIM_BIT_FIFO_EMPTY       ((u32)0x00000001 << 1)           /*!< R 0x0  fifo empty*/
#define TIM_BIT_FIFO_CLR         ((u32)0x00000001 << 0)           /*!< WA0 0x0  clean fifo*/
/** @} */

/** @defgroup FIFO_DATA
  * @brief
  * @{
  */
#define TIM_MASK_FIFO_DATA   ((u32)0x0000FFFF << 0)           /*!< R/WPD 0x0  fifo wdata*/
#define TIM_FIFO_DATA(x)     (((u32)((x) & 0x0000FFFF) << 0))
#define TIM_GET_FIFO_DATA(x) ((u32)(((x >> 0) & 0x0000FFFF)))
/** @} */

/** @} */
/* Exported Types --------------------------------------------------------*/

/** @defgroup PWMTIMER_Exported_Types PWMTIMER Exported Types
  * @{
  */

/** @brief PWMTIMER Register Declaration
  */

typedef struct {
	__IO uint32_t EN          ;  /*!< TIM ENABLE REGISTER,  Address offset:0x000 */
	__IO uint32_t CR          ;  /*!< TIM CONTROL REGISTER,  Address offset:0x004 */
	__IO uint32_t DIER        ;  /*!< TIM INTERUPT ENABLE REGISTER,  Address offset:0x008 */
	__IO uint32_t SR          ;  /*!< TIM STATUS REGISTER,  Address offset:0x00C */
	__O  uint32_t EGR         ;  /*!< TIM EVENT GENERATION REGISTER,  Address offset:0x010 */
	__I  uint32_t CNT         ;  /*!< TIM COUNTER Register,  Address offset:0x014 */
	__IO uint32_t PSC         ;  /*!< TIM PRESCALER REGISTER,  Address offset:0x018 */
	__IO uint32_t ARR         ;  /*!< TIM AUTO-RELOAD REGISTER,  Address offset:0x01C */
	__IO uint32_t RSVD0       ;  /*!< Reserved,  Address offset:0x020-0x023 */
	__IO uint32_t CCRx[6]     ;  /*!< TIM CAPTURE/COMPARE REGISTER x Register,  Address offset:0x024-0x038 */
	__IO uint32_t PSYNCx[6]   ;  /*!< TIM PHASE SYNCHRONIZATION REGISTER x Register,  Address offset:0x03C-0x050 */
	__I  uint32_t PHASECNTx[6];  /*!< TIM PHASE COUNTER REGISTER x Register,  Address offset:0x054-0x068 */
	__IO uint32_t AUDx[6]     ;  /*!< Register,  Address offset:0x06C-0x080 */
	__IO uint32_t ISR         ;  /*!< Register,  Address offset:0x084 */
	__IO uint32_t CFG         ;  /*!< Register,  Address offset:0x088 */
	__IO uint32_t DMA         ;  /*!< Register,  Address offset:0x08C */
	__IO uint32_t FIFO        ;  /*!< Register,  Address offset:0x090 */
	__IO uint32_t FIFO_DATA   ;  /*!< Register,  Address offset:0x094 */
	__IO uint32_t DUMMY       ;  /*!< Register,  Address offset:0x098 */
} RTIM_TypeDef;

/** @} */


// Do NOT modify any AUTO_GEN code above
/* AUTO_GEN_END */

/* MANUAL_GEN_START */
#ifdef __cplusplus
extern "C" {
#endif


//Please add your defination here

/* Exported types ------------------------------------------------------------*/
/** @addtogroup PWMTIMER_Exported_Types PWMTIMER Exported Types
  * @{
  */

/**
  * @brief  TIM Basic Init Structure Definition
  */
typedef struct {
	u32 TIM_Prescaler;		/*!< Specifies the prescaler value used to divide the TIM clock.
							This parameter can be a number between 0x0 and 0xFFFF, basic timer dont care */
	u32 TIM_Period;			/*!< Specifies the period value to be loaded into the active
								Auto-Reload Register at the next update event.
								This parameter is 16bits for TIM8-9, and 32bits for TIM0-TIM7
								you can get it from SourceClock & TIM_Prescaler */
	u32 TIM_UpdateEvent;	/*!< Specifies whether or not to enable update event(UEV).
								This parameter can be ENABLE or DISABLE. ENABLE means UEV Enable, DISABLE means UEV Disable*/
	u32 TIM_UpdateSource;	/*!< Specifies the update request source. This parameter can be
								TIM_UpdateSource_Overflow or TIM_UpdateSource_Global.
								TIM_UpdateSource_Overflow means counter overflow generates an update event(UEV).
								TIM_UpdateSource_Global means both counter overflow and setting the UG bit can generate UEV.*/
	u32 TIM_ARRProtection;	/*!< DISABLE or ENABLE, when ENABLE: period will update when cnt = 0(counter overflow, an UEV happens),
								or  period will update immediatly */

	u8	TIM_Idx;			/*!< 0~9 */

	u32	TIM_SecureTimer;	/*!< Specifies the secure attribute of this timer.
								This parameter is used in secure world of trustzone.*/
} RTIM_TimeBaseInitTypeDef;

/**
  * @brief  TIM Output Compare Init Structure Definition
  */
typedef struct {
	u32 TIM_CCMode;			/*!< Specifies the TIM8 mode. This parameter can be a value of TIM_CCMode_PWM or TIM_CCMode_Inputcapture */
	u32 TIM_CCPolarity;		/*!< Specifies the polarity. This parameter can be TIM_CCPolarity_High/TIM_CCPolarity_Low.
								If CCx channel is configured as output:
									TIM_CCPolarity_High means OCx active high.
									TIM_CCPolarity_Low means OCx active low.
								If CCx channel is configured as input:
									TIM_CCPolarity_High means positive edge of TRGI is active for capture.
									TIM_CCPolarity_Low means negative edge of TRGI is active for capture. */

	u32 TIM_OCProtection;	/*!< Output Compare value update protection. TIM_OCPreload_Enable/TIM_OCPreload_Disable.
								TIM_OCPreload_Enable means duty cycle will update when UEV happens if write to CCRx field in TIMx_CCRX.
								TIM_OCPreload_Disable means duty cycle will update immediately if write to CCRx field in TIMx_CCRX.*/

	u32 TIM_OCPulse;		/*!< Specifies the output pulse value to be loaded into the CCRx Register, which decides the duty cycle.
								This parameter can be a number between 0x0000 and 0xFFFF */

	u32 TIM_ICPulseMode;	/*!< Specifies the TIM6 mode, TIM_CCMode_PulseWidth or TIM_CCMode_PulseNumber */
} TIM_CCInitTypeDef;

/**
  * @}
  */

/**
  * @brief  PWM dev Table Definition
  */
typedef struct {
	RTIM_TypeDef *TIMx;
	u32 Tx_HandshakeInterface;
} PWM1_DevTable;
/**
  * @}
  */

/**
  * @brief  TIM5 audio and motor Output mode Init Structure Definition
  */
typedef struct {
	u32 TIM_IdleLevel;  /*!< Specifies the PWM init output level.
								This parameter can be PWM_OUTPUT_LOW/PWM_OUTPUT_HIGH. */

	u32 TIM_AudBrkLevel;/*!< Specifies the PWM brake output level.
								This parameter can be PWM_OUTPUT_LOW/PWM_OUTPUT_HIGH. */

	u32 TIM_AudFIFOEmptyLevel;/*!< Specifies when fifo is empty PWM Output Level.
                This parameter can be PWM_OUTPUT_LOW/PWM_OUTPUT_HIGH. */

	u32 TIM_AudEmgBrk;/*!< Specifies ENABLE/DISABLE emergency brake fuction.
								This parameter can be ENABLE/DISABLE. */

	u32 TIM_AudEmgBrkLevel;/*!< Specifies after emergency brake PWM output level.
                This parameter can be PWM_EMGBRK_OUTPUT_LOW/PWM_EMGBRK_OUTPUT_HIGH. */

	u32 TIM_AudEmgBrkPol;/*!< Specifies after emergency brake trig polarity.
                This parameter can be PWM_EMGBRK_TRI_POL_HIHG/PWM_EMGBRK_TRI_POL_LOW. */

	u32 TIM_AudCCRSourceSel;/*!< Specifies the CCR data source select.
								This parameter can be PWM_CCR_SOURCE_CPU/PWM_CCR_SOURCE_FIFO. */

	u32 TIM_AudPolSel;/*!< Specifies channel1/3/5 polarity, for channel0/2/4 this parameter is invalid.
                This parameter can be PWM_POLARITY_OPPOSITE/PWM_POLARITY_SAME. */

	u32 TIM_AudChnNum;/*!< Specifies PWM channel 0/2/4 control 1 or 2 output pad, for channel1/3/5 this parameter is invalid.
                This parameter can be PWM_CHANNEL_OUTPUT_ONEPAD/PWM_CHANNEL_OUTPUT_TWOPAD. */

	u32 TIM_AudDeadZoneEn;/*!< Specifies deadzone statue.
                This parameter can be DISABLE/ENABLE*/

	u32 TIM_AudDeadZoneTim;/*!< Specifies deadzone time.
                This parameter can be a number between 0x000 to 0x7FF.*/

	u32 TIM_AudFIFOemptyMode;/*!< Specifies when fifo is empty PWM Output mode.
                This parameter can be PWM_FIFO_EMPTY_MAINTAIN/PWM_FIFO_EMPTY_BRAKE. */
} TIM_AudInitTypeDef;
/**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup PWMTIMER_Exported_Constants PWMTIMER Exported Constants
  * @{
  */

/** @defgroup SEC_Control_Enable_Definition
  * @{
  */
#define TIM_BIT_SEC        ((u32)0x00000001 << 0)          /*!<R/W 0x0  Secure timer enable * 0: non-security timer * 1: security timer */
/**
  * @}
  */

/** @defgroup TIM_Type
  * @{
  */

#define IS_TIM_BASIC_TIM(PERIPH)		(((PERIPH) == TIM0) || \
										((PERIPH) == TIM1) || \
										((PERIPH) == TIM2) || \
										((PERIPH) == TIM3) || \
										((PERIPH) == TIM0_S) || \
										((PERIPH) == TIM1_S) || \
										((PERIPH) == TIM2_S) || \
										((PERIPH) == TIM3_S))

#define IS_TIM_ONE_PULSE_TIM(PERIPH)	(((PERIPH) == TIM4) || \
										((PERIPH) == TIM5) || \
                    ((PERIPH) == TIM6) || \
										((PERIPH) == TIM4_S) || \
										((PERIPH) == TIM5_S) || \
                    ((PERIPH) == TIM6_S))

#define IS_TIM_INPULSE_TIM(PERIPH)	(((PERIPH) == TIM6) || \
									((PERIPH) == TIM6_S))

#define IS_TIM_CCM_TIM(PERIPH)	(((PERIPH) == TIM4) || \
										((PERIPH) == TIM5) || \
										((PERIPH) == TIM6) || \
										((PERIPH) == TIM4_S) || \
										((PERIPH) == TIM5_S) || \
										((PERIPH) == TIM6_S))

#define IS_TIM_PWM_TIM(PERIPH)	(((PERIPH) == TIM4) || \
										((PERIPH) == TIM5) || \
										((PERIPH) == TIM4_S) || \
										((PERIPH) == TIM5_S))

#define IS_TIM_40M_TIM(PERIPH)	(((PERIPH) == TIM4) || \
										((PERIPH) == TIM5) || \
										((PERIPH) == TIM6) || \
										((PERIPH) == TIM4_S) || \
										((PERIPH) == TIM5_S) || \
										((PERIPH) == TIM6_S))

#define IS_PWM_TIM5(PERIPH)	(((PERIPH) == TIM5) || \
									((PERIPH) == TIM5_S))
/**
  * @}
  */

/** @defgroup TIM_Channel
  * @note TIMx: 4~8
  * @{
  */
#define TIM_Channel_0			((u16)0x0000)
#define TIM_Channel_1			((u16)0x0001)
#define TIM_Channel_2			((u16)0x0002)
#define TIM_Channel_3			((u16)0x0003)
#define TIM_Channel_4			((u16)0x0004)
#define TIM_Channel_5			((u16)0x0005)

#define IS_TIM_CHANNEL(CHANNEL)	(((CHANNEL) == TIM_Channel_0) || \
								((CHANNEL) == TIM_Channel_1) || \
								((CHANNEL) == TIM_Channel_2) || \
								((CHANNEL) == TIM_Channel_3) || \
                ((CHANNEL) == TIM_Channel_4) || \
								((CHANNEL) == TIM_Channel_5))
/**
  * @}
  */

/** @defgroup TIM5_Channel
  * @note TIMx: 5
  * @{
  */
#define TIM5_Channel_0			((u16)0x0000)
#define TIM5_Channel_1			((u16)0x0001)
#define TIM5_Channel_2			((u16)0x0002)
#define TIM5_Channel_3			((u16)0x0003)
#define TIM5_Channel_4			((u16)0x0004)
#define TIM5_Channel_5			((u16)0x0005)

#define IS_TIM5_CHANNEL(CHANNEL)	(((CHANNEL) == TIM5_Channel_0) || \
								((CHANNEL) == TIM5_Channel_1) || \
								((CHANNEL) == TIM5_Channel_2) || \
								((CHANNEL) == TIM5_Channel_3) || \
								((CHANNEL) == TIM5_Channel_4) || \
								((CHANNEL) == TIM5_Channel_5))
/**
  * @}
  */

/** @defgroup TIM_Prescaler
  * @{
  */
#define IS_TIM_PSC(VAL) (VAL <= 0xFFFF)
/**
  * @}
  */

/** @defgroup TIM_Control_Bit
  * @{
  */
#define TIM_OPMode_ETP_positive		((u32)0x00000000)
#define TIM_OPMode_ETP_negative		((u32)0x00000100)
#define TIM_OPMode_ETP_bothedge		((u32)0x00000200)
#define IS_TIM_OPM_ETP_MODE(MODE)	(((MODE) == TIM_OPMode_ETP_positive) || \
									((MODE) == TIM_OPMode_ETP_negative) || \
									((MODE) == TIM_OPMode_ETP_bothedge))

#define TIM_OPMode_Single			((u32)0x00000008)
#define TIM_OPMode_Repetitive		((u32)0x00000000) /* repeative is PWM mode */
#define IS_TIM_OPM_MODE(MODE)		(((MODE) == TIM_OPMode_Single) || \
									((MODE) == TIM_OPMode_Repetitive))

#define TIM_UpdateSource_Global			((u32)0x00000000) /*!< Source of update is the counter overflow or the setting of UG bit. */
#define TIM_UpdateSource_Overflow		((u32)0x00000004) /*!< Source of update is counter overflow. */
#define IS_TIM_UPDATE_SOURCE(SOURCE)	(((SOURCE) == TIM_UpdateSource_Global) || \
										((SOURCE) == TIM_UpdateSource_Overflow))
/**
  * @}
  */

/** @defgroup TIM_Interrupt_Control
  * @{
  */
#define TIM_IT_Update			((u32)0x00000001)
#define TIM_IT_CC0				((u32)0x00000002)
#define TIM_IT_CC1				((u32)0x00000004)
#define TIM_IT_CC2				((u32)0x00000008)
#define TIM_IT_CC3				((u32)0x00000010)
#define TIM_IT_CC4				((u32)0x00000020)
#define TIM_IT_CC5				((u32)0x00000040)
#define TIM_IT_UIE0				((u32)0x00010000)
#define TIM_IT_UIE1				((u32)0x00020000)
#define TIM_IT_UIE2				((u32)0x00040000)
#define TIM_IT_UIE3				((u32)0x00080000)
#define TIM_IT_UIE4				((u32)0x00100000)
#define TIM_IT_UIE5				((u32)0x00200000)
#define IS_TIM_IT(IT)	((((IT) & (u32)0xFFC0FF80) == 0x0000) && (((IT) & (u32)0x3F007F) != 0x0000))

#define IS_TIM_GET_IT(IT)	(((IT) == TIM_IT_Update) || \
							((IT) == TIM_IT_CC0) || \
							((IT) == TIM_IT_CC1) || \
							((IT) == TIM_IT_CC2) || \
							((IT) == TIM_IT_CC3) || \
              ((IT) == TIM_IT_CC4) || \
							((IT) == TIM_IT_CC5) || \
							((IT) == TIM_IT_UIE0) || \
							((IT) == TIM_IT_UIE1) || \
							((IT) == TIM_IT_UIE2) || \
              ((IT) == TIM_IT_UIE3) || \
							((IT) == TIM_IT_UIE4) || \
							((IT) == TIM_IT_UIE5))
/**
  * @}
  */

/** @defgroup TIM_PSC_Reload_Mode
  * @{
  */
#define TIM_PSCReloadMode_Update		((u32)0x00000000)
#define TIM_PSCReloadMode_Immediate		((u32)0x00000001)
#define IS_TIM_PRESCALER_RELOAD(RELOAD)	(((RELOAD) == TIM_PSCReloadMode_Update) || \
										((RELOAD) == TIM_PSCReloadMode_Immediate))
/**
  * @}
  */

/** @defgroup TIM_Event_Source
  * @{
  */
#define TIM_EventSource_Update		((u32)0x00000001)
#define TIM_EventSource_CC0			((u32)0x00000002)
#define TIM_EventSource_CC1			((u32)0x00000004)
#define TIM_EventSource_CC2			((u32)0x00000008)
#define TIM_EventSource_CC3			((u32)0x00000010)
#define TIM_EventSource_CC4			((u32)0x00000020)
#define TIM_EventSource_CC5			((u32)0x00000040)
#define TIM_EventSource_CC6			((u32)0x00000080)
#define TIM_EventSource_CC7			((u32)0x00000100)
#define IS_LP_TIM_EVENT_SOURCE(SOURCE)	((((SOURCE) & 0xFFFFFFFE) == 0x0000) && \
										(((SOURCE) & 0x1) != 0x0000))
#define IS_HP_TIM_EVENT_SOURCE(SOURCE)	((((SOURCE) & 0xFFFFFE00) == 0x0000) && \
										(((SOURCE) & 0x1FF) != 0x0000))
/**
  * @}
  */

/** @defgroup TIM_Capture_Compare_Control
  * @{
  */
#define TIM_CCx_Enable		((u32)0x01000000)
#define TIM_CCx_Disable		((u32)0x00000000)
#define IS_TIM_CCX(CCX)		(((CCX) == TIM_CCx_Enable) || ((CCX) == TIM_CCx_Disable))

#define TIM_OCPreload_Enable			((u32)0x02000000)
#define TIM_OCPreload_Disable			((u32)0x00000000)
#define IS_TIM_OCPRELOAD_STATE(STATE)	(((STATE) == TIM_OCPreload_Enable) || \
										((STATE) == TIM_OCPreload_Disable))

#define TIM_CCPolarity_High				((u32)0x00000000) /*!< if input is set :  Positive edge of TRGI is active for capture */
#define TIM_CCPolarity_Low				((u32)0x04000000) /*!< if input is set :  negative edge of TRGI is active for capture */
#define IS_TIM_CC_POLARITY(POLARITY)	(((POLARITY) == TIM_CCPolarity_High) || \
										((POLARITY) == TIM_CCPolarity_Low))

/* TIM8 PWM or Inputcapture mode */
#define TIM_CCMode_PWM					((u32)0x00000000)
#define TIM_CCMode_Inputcapture			((u32)0x08000000)
#define IS_TIM_CC_MODE(MODE)			(((MODE) == TIM_CCMode_PWM) || \
										((MODE) == TIM_CCMode_Inputcapture))

/* TIM6 pulse mode */
#define TIM_CCMode_PulseWidth			((u32)0x00000000)
#define TIM_CCMode_PulseNumber			((u32)0x10000000)
#define IS_TIM_CC_PULSEMODE(MODE)		(((MODE) == TIM_CCMode_PulseWidth) || \
										((MODE) == TIM_CCMode_PulseNumber))

#define TIM_CCMode_CCR					((u32)0x0000FFFF)
#define IS_TIM_CC_PULSEWIDTH(Compare)	((Compare) <= TIM_CCMode_CCR)
/**
  * @}
  */

/** @defgroup TIM6_CC0_Pulse_Mode
  * @{
  */
#define TIM6_GET_CC0PM(x)		((u32)(((x) & 0x10000000) >> 28)) /*!<R/W 0x0  CC0 pulse mode * 0: pulse mode 0 * 1: pulse mode 1 */
#define TIM6_CC0PM(x)			((u32)(((x) & 0x00000001) << 28))
/**
  * @}
  */

/** @defgroup TIMBasic_ARR
  * @{
  */
#define TIMBasic_GET_ARR(x)		((u32)(((x) & 0xFFFFFFFF) >> 0)) /*!<R/W 0xFFFFFFFF  the value to be loaded in the actual auto-reload register. It can be preloaded by setting ARPE bit in TIM_CR. */
#define TIMBasic_ARR(x)			((u32)(((x) & 0xFFFFFFFF) << 0))
/**
  * @}
  */

/** @defgroup TIMBasic_CNT
  * @{
  */
#define TIMBasic_GET_CNT(x)		((u32)(((x) & 0xFFFFFFFF) >> 0)) /*!<R 0x0  counter value */
#define TIMBasic_CNT(x)			((u32)(((x) & 0xFFFFFFFF) << 0))
/**
  * @}
  */

/** @defgroup TIM_Phase_Sync_Control
  * @{
  */

#define TIMPWM_PSync_Delay				((u32)0x00000000)
#define TIMPWM_PSync_Ahead				((u32)0x00000001)
#define IS_TIMPWM_PSync_Dir(DIR)		(((DIR) == TIMPWM_PSync_Delay) || ((DIR) == TIMPWM_PSync_Ahead))

#define TIMPWM_PSyncPreload_Enable		((u32)0x00000000)
#define TIMPWM_PSyncPreload_Disable		((u32)0x00000001)
#define IS_TIMPWM_PPRELOAD_STATE(STATE)	(((STATE) == TIMPWM_PSyncPreload_Enable) || \
										((STATE) == TIMPWM_PSyncPreload_Disable))
/**
  * @}
  */

/** @defgroup TIM_Default_Level
  * @{
  */

#define TIMPWM_DefaultLevel_High		((u32)0x00000000)
#define TIMPWM_DefaultLevel_Low			((u32)0x00000001)
#define IS_TIMPWM_DefaultLevel(LEVEL)	(((LEVEL) == TIMPWM_DefaultLevel_High) || ((LEVEL) == TIMPWM_DefaultLevel_Low))
/**
  * @}
  */
/** @defgroup PWM_AudandMotor_Control
  * @{
  */
#define PWM_OUTPUT_LOW			((u32)0x00000000)
#define PWM_OUTPUT_HIGH		  ((u32)0x00000001)

#define PWM_EMGBRK_OUTPUT_LOW			((u32)0x00000000)
#define PWM_EMGBRK_OUTPUT_HIGH		((u32)0x00000004)

#define PWM_EMGBRK_TRI_POL_HIHG   ((u32)0x00000000)
#define PWM_EMGBRK_TRI_POL_LOW    ((u32)0x04000000)

#define PWM_CCR_SOURCE_CPU			((u32)0x00000000)
#define PWM_CCR_SOURCE_FIFO		  ((u32)0x00000010)

#define PWM_EMGBRK_DISABLE		((u32)0x00000000)
#define PWM_EMGBRK_ENABLE	    ((u32)0x00000008)

#define PWM_POLARITY_OPPOSITE			((u32)0x00000000)
#define PWM_POLARITY_SAME		      ((u32)0x00000020)

#define PWM_CHANNEL_OUTPUT_ONEPAD			((u32)0x00000000)
#define PWM_CHANNEL_OUTPUT_TWOPAD		  ((u32)0x00000040)

#define PWM_DeadzoneEn			      ((u32)0x00000080)
#define PWM_DeadzoneDis		        ((u32)0x00000000)

#define PWM_FIFO_EMPTY_MAINTAIN			((u32)0x00000000)
#define PWM_FIFO_EMPTY_BRAKE		    ((u32)0x00000100)
/**
  * @}
  */

/** @defgroup PWM_AudandMotor_Intr_Control
  * @{
  */
#define TIM_IT_FIFO_EMPTY_FLAG        ((u32)0x00000001)
#define TIM_IT_EMG_BRK_RELEASE_FLAG   ((u32)0x00000002)
#define TIM_IT_FIFO_EMPTY             ((u32)0x00000004)
#define TIM_IT_EMG_BRK_RELEASE        ((u32)0x00000008)
#define TIM_IT_AUD_ALL_FLAG    (TIM_IT_FIFO_EMPTY_FLAG | TIM_IT_EMG_BRK_RELEASE_FLAG)
#define IS_TIM_AUD_IT_FLAG(IT)   ((IT) == TIM_IT_FIFO_EMPTY_FLAG) || ((IT) == TIM_IT_EMG_BRK_RELEASE_FLAG)
#define TIM_IT_AUD_ALL    (TIM_IT_FIFO_EMPTY | TIM_IT_EMG_BRK_RELEASE)
#define IS_TIM_AUD_IT(IT)    ((IT) == TIM_IT_FIFO_EMPTY) || ((IT) == TIM_IT_EMG_BRK_RELEASE)
/**
  * @}
  */

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @defgroup PWMTIMER_Exported_Functions PWMTIMER Exported Functions
  * @{
  */

/** @defgroup TimeBase_Management_Functions TimeBase Management Functions
* @{
*/
_LONG_CALL_ void RTIM_TimeBaseStructInit(RTIM_TimeBaseInitTypeDef *TIM_InitStruct);
_LONG_CALL_ void RTIM_TimeBaseInit(RTIM_TypeDef *TIMx, RTIM_TimeBaseInitTypeDef *TIM_InitStruct, IRQn_Type   IrqNum,
								   IRQ_FUN UserCB, u32 UserCBData);
_LONG_CALL_ void RTIM_Cmd(RTIM_TypeDef *TIMx, u32 NewState);
_LONG_CALL_ void RTIM_DeInit(RTIM_TypeDef *TIMx);
_LONG_CALL_ u32 RTIM_GetCount(RTIM_TypeDef *TIMx);
_LONG_CALL_ void RTIM_UpdateDisableConfig(RTIM_TypeDef *TIMx, u32 NewState);
_LONG_CALL_ void RTIM_ARRPreloadConfig(RTIM_TypeDef *TIMx, u32 NewState);
_LONG_CALL_ void RTIM_UpdateRequestConfig(RTIM_TypeDef *TIMx, u32 TIM_UpdateSource);
_LONG_CALL_ void RTIM_PrescalerConfig(RTIM_TypeDef *TIMx, u32 Prescaler, u32 TIM_PSCReloadMode);
_LONG_CALL_ void RTIM_GenerateEvent(RTIM_TypeDef *TIMx, u32 TIM_EventSource);
_LONG_CALL_ void RTIM_ChangePeriod(RTIM_TypeDef *TIMx, u32 Autoreload);
_LONG_CALL_ void RTIM_ChangePeriodImmediate(RTIM_TypeDef *TIMx, u32 Autoreload);
_LONG_CALL_ void RTIM_ChangePeriod_us(RTIM_TypeDef *TIMx, u8 tim_idx, u32 duration_us);
_LONG_CALL_ void RTIM_ChangePeriodImmediate_us(RTIM_TypeDef *TIMx, u8 tim_idx, u32 duration_us);
_LONG_CALL_ void RTIM_Reset(RTIM_TypeDef *TIMx);
/**
  * @}
  */

/** @defgroup Capture_Compare_Management_Functions Capture Compare Management Functions
* @{
*/
_LONG_CALL_ void RTIM_CCStructInit(TIM_CCInitTypeDef *TIM_CCInitStruct);
_LONG_CALL_ void RTIM_CCxInit(RTIM_TypeDef *TIMx, TIM_CCInitTypeDef *TIM_CCInitStruct, u16 TIM_Channel);
_LONG_CALL_ void RTIM_CCRxMode(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_CCMode);
_LONG_CALL_ void RTIM_CCRxSet(RTIM_TypeDef *TIMx, u32 Compare, u16 TIM_Channel);
_LONG_CALL_ u32 RTIM_CCRxGet(RTIM_TypeDef *TIMx, u16 TIM_Channel);
_LONG_CALL_ void RTIM_OCxPreloadConfig(RTIM_TypeDef *TIMx, u32 TIM_OCProtection, u16 TIM_Channel);
_LONG_CALL_ void RTIM_CCxPolarityConfig(RTIM_TypeDef *TIMx, u32 TIM_OCPolarity, u16 TIM_Channel);
_LONG_CALL_ void RTIM_CCxCmd(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_CCx);
_LONG_CALL_ void RTIM_SetOnePulseOutputMode(RTIM_TypeDef *TIMx, u32 TIM_OPMode, u32 TrigerPolarity);
_LONG_CALL_ void RTIM_SetOnePulseDefaultLevel(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 DefaultLevel);
/**
  * @}
  */

/** @defgroup Interrupt_Management_Functions Interrupt Management Functions
* @{
*/
_LONG_CALL_ void RTIM_INTConfig(RTIM_TypeDef *TIMx, u32 TIM_IT, u32 NewState);
_LONG_CALL_ void RTIM_INTClear(RTIM_TypeDef *TIMx);
_LONG_CALL_ void RTIM_INTClearPendingBit(RTIM_TypeDef *TIMx, u32 TIM_IT);
_LONG_CALL_ u32 RTIM_GetINTStatus(RTIM_TypeDef *TIMx, u32 TIM_IT);
/**
  * @}
  */

/** @defgroup Phase_Sync_Functions Phase Sync Functions
* @{
*/
_LONG_CALL_ void RTIM_PSyncxSet(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_SyncPhase);
_LONG_CALL_ u32 RTIM_PSyncxGet(RTIM_TypeDef *TIMx, u16 TIM_Channel);
_LONG_CALL_ void RTIM_PSyncxDir(RTIM_TypeDef *TIMx, u16 TIM_Channel, u8 TIM_SyncDir);
_LONG_CALL_ void RTIM_PSyncxPreloadConfig(RTIM_TypeDef *TIMx, u16 TIM_Channel, u32 TIM_PSyncProtection);
_LONG_CALL_ void RTIM_PSyncxCmd(RTIM_TypeDef *TIMx, u16 TIM_Channel, u8 NewState);
_LONG_CALL_ u32 RTIM_GetChannelCountx(RTIM_TypeDef *TIMx, u16 TIM_Channel);
/**
  * @}
  */

/** @defgroup Audio and motor Functions
* @{
*/
_LONG_CALL_ void RTIM_AudStructInit(TIM_AudInitTypeDef *TIM_AudInitStruct);
_LONG_CALL_ void RTIM_AudxInit(RTIM_TypeDef *TIMx, TIM_AudInitTypeDef *TIM_AudInitStruct, u16 TIM_Channel);
_LONG_CALL_ void RTIM_AudioandMotorINTConfig(RTIM_TypeDef *TIMx, u32 TIM_IT, u32 NewState);
_LONG_CALL_ void RTIM_AudioandMotorINTClear(RTIM_TypeDef *TIMx);
_LONG_CALL_ void RTIM_AudioandMotorSetBrakeLevel(RTIM_TypeDef *TIMx, u32 BRAKE_LEVEL, u16 TIM_Channel);
_LONG_CALL_ void RTIM_AudioandMotorSetBrakeLevelUp(RTIM_TypeDef *TIMx);
_LONG_CALL_ void RTIM_AudioandMotorSetFIFOEmptyMode(RTIM_TypeDef *TIMx, u32 FIFO_EMPTY_MODE, u16 TIM_Channel);
_LONG_CALL_ u32 RTIM_GetAudioandMotorINTStatus(RTIM_TypeDef *TIMx, u32 TIM_IT_AUD_FLAG);
_LONG_CALL_ void RTIM_SetDeadzoneTim(RTIM_TypeDef *TIMx, u32 deadzone_tim, u16 TIM_Channel);
_LONG_CALL_ void RTIM_SetDeadzoneMAXChnID(RTIM_TypeDef *TIMx);
_LONG_CALL_ void RTIM_EmgBrkDbcCmd(RTIM_TypeDef *TIMx, u32 NewState);
_LONG_CALL_ void RTIM_SetEmgBrkLevel(RTIM_TypeDef *TIMx, u32 EMG_BRK_LEVEL, u16 TIM_Channel);
_LONG_CALL_ void RTIM_SetEmgBrkDbcCnt(RTIM_TypeDef *TIMx, u16 Dbc_Cnt);
_LONG_CALL_ void RTIM_SetFIFOFirstLevel(RTIM_TypeDef *TIMx, u16 FIFO_Level);
_LONG_CALL_ void RTIM_ResetFIFO(RTIM_TypeDef *TIMx);
_LONG_CALL_ void PWM_DmaCmd(RTIM_TypeDef *TIMx, u32 NewState);
_LONG_CALL_ void PWM_DmaBurstSize(RTIM_TypeDef *TIMx, u32 msize);
_LONG_CALL_ bool PWM_TXGDMA_Init(u32 Index, GDMA_InitTypeDef *GDMA_InitStruct, void *CallbackData, IRQ_FUN CallbackFunc, u8 *pTXData, u32 Length);
/**
  * @}
  */

/**
  * @}
  */

/** @} */

/** @} */

/* Other Definitions -------------------------------------------------------------------*/

typedef struct {
	u32 PWM_CHANNEL;
	u32 NP_CHAN_STATUS;
	u32 AP_CHAN_STATUS;
} PWMCHANCFG_TypeDef;

#define TimerNum	7
#define PWM_TIMER_NUM 2

extern PWMCHANCFG_TypeDef  pwmchannel_config[];
extern int TIMx_irq[TimerNum];
extern RTIM_TypeDef *TIMx[TimerNum];
extern RTIM_TypeDef *TIMx_S[TimerNum];
extern u32 APBPeriph_TIMx[TimerNum];
extern u32 APBPeriph_TIMx_CLOCK[TimerNum];

#define PWM_CHAN_MAX 6
#define CAPTURE_CHANNEL_MAX		1

extern u32 TIM_IT_CCx[PWM_CHAN_MAX];

#define TIMER_TICK_US			31
#define TIMER_TICK_US_X4		(4*1000000/32000) //32k clock, 31.25us every timer_tick

#define TIMER_POLLING_TIMES		4000		//Polling Timer Status Register Timeout

#ifdef __cplusplus
}
#endif

/* MANUAL_GEN_END */

#endif
