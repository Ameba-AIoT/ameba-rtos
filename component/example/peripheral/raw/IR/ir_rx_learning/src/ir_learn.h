/*
 *  Routines to access hardware
 *
 *  Copyright (c) 2013 Realtek Semiconductor Corp.
 *
 *  This module is a confidential and proprietary property of RealTek and
 *  possession or use of this module requires written permission of RealTek.
 */


/** @addtogroup  DUALMODE_RMC Dualmode RMC
    * @{
    */

/** @defgroup  IR_LEARN Ir Learn
    * @brief  This file handles ir learn function.
    * @{
    */

/*============================================================================*
  *                                 Macros
  *============================================================================*/
/** @defgroup IR_LEARN_Exported_Macros Ir Learn Exported Macros
    * @{
    */
#define IR_FREQ                 10000000 /**<10M rx sample frequency*/
#define IR_RX_FIFO_THR_LEVEL    10  /**<trigger interrupt if rx has IR_RX_FIFO_THR_LEVEL num data*/
#define QUEUE_CAPABILITY        0xFFF
#define LOOP_QUEUE_MAX_SIZE     (QUEUE_CAPABILITY + 1)

/* Enable calculate frequency when decode packet or not */
#define CALCULATE_FREQUENCY         TRUE
/* Enable correcting frequency by software */
#define CORRECT_FREQ_BY_SOFTWARE    TRUE

#define RMC_IR_LEARN_TIMEOUT_MS    (10 * RTOS_TICK_RATE_HZ) /**<10s*/

/*--------------Configure by user to confirm IR learn parameter start --------*/
/* Filter threshold value. If 100< time interval< 2000, treat it as a part of a carrier time */
#define TIME_LOWEST_VALUE          50  /**<5us when learn frequency is 10M*/
#define TIME_HIGHEST_VALUE         400 /**<40us when learn frequency is 10M*/
/* If low level time>0.25ms,we treat it as a time which have no carrier */
#define TIME_LOW_STOP_VALUE        2500 /**<250us when learn frequency is 10M*/

#define LEARN_WAVE_MIN_SIZE        (10)    /*min size of learn data of per key in uint32_t*/
#define LEARN_WAVE_MAX_SIZE        (100)   /*max size of learn data of per key in uint32_t*/

/*--------------Configure by user to confirm IR learn parameter end --------*/

#define ABS_FREQUENCY(a,b)         ((a>b) ? (a-b):(b-a))

/** End of IR_LEARN_Exported_Macros
    * @}
    */

/*============================================================================*
  *                                     Types
  *============================================================================*/
/** @defgroup IR_LEARN_Exported_Types Ir Learn Exported Types
    * @{
    */
typedef enum t_ir_ret_code {
	IR_RET_OK,          /**< ir return ok*/
	IR_LEARN_ERR,       /**< ir learn error*/
} T_IR_RET_CODE;

/* Data struct for IR learn, to-do: 4 bytes align*/
/*need to find if all needed ###*/
typedef struct t_ir_packet_typedef {
	/* Manage loop queue */
	volatile uint16_t   read_index;          /**< index of read queue */
	volatile uint16_t   write_index;         /**< index of write queue */
	uint32_t *queue;    /**< pointer to the queue */

	/* Record IR learned data */
	uint32_t            detect_time[LEARN_WAVE_MAX_SIZE];/**< record total time*/
	uint32_t            square_wave_num;    /**< record square wave number*/
	uint16_t            detect_time_num;    /**< record IR data number */
	bool                is_high_level;  /**< record decode voltage level */
	volatile bool       over_flow;  /**< record loop queue over folw or not */

#if CALCULATE_FREQUENCY
	/* Record learned IR frequency */
	uint32_t            wave_frequency[LEARN_WAVE_MAX_SIZE / 2 + 1];    /**< record IR frequency*/
#else
	/* Record IR high waveform time parameters */
	uint32_t            high_wave_time[LEARN_WAVE_MAX_SIZE / 2 + 1];
	uint16_t            high_wave_count[LEARN_WAVE_MAX_SIZE / 2 + 1];
#endif
	uint16_t            frequency_num;       /**< Record number of calculated frequency */
	volatile bool       data_receive_completed; /**< Record IR learn data receive completed flag */

} T_IR_PACKET_TYPEDEF;
/** End of IR_LEARN_Exported_Types
    * @}
    */

/*============================================================================*
  *                                 Variables
  *============================================================================*/
/** @defgroup IR_LEARN_Exported_Variables Ir Learn Exported Variables
    * @{
    */

/*ir learn data packet*/
extern T_IR_PACKET_TYPEDEF g_ir_learn_packet;

/** End of IR_LEARN_Exported_Variables
    * @}
    */

/** @} */ /* End of group IR_LEARN */
/** @} */ /* End of group DUALMODE_RMC */


