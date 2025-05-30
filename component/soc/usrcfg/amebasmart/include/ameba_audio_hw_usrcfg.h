/*
* Copyright (c) 2024 Realtek Semiconductor Corp.
*
* SPDX-License-Identifier: Apache-2.0
*/

#ifndef AMEBA_FWK_AUDIO_CONFIGS_HARDWARE_AUDIO_AUDIO_HW_CONFIG_H
#define AMEBA_FWK_AUDIO_CONFIGS_HARDWARE_AUDIO_AUDIO_HW_CONFIG_H

/*
 *Audio amplifier pin, if your board has no audio amplifier pin , please set it as -1.
 */
#define AUDIO_HW_AMPLIFIER_PIN          _PB_19 /*_PB_31 for old board*/

/*
 * See amplifier types defined in: component/audio/audio_driver/amp_manager/interfaces/amp_manager.h
 * enum AMP_TYPE. for example: dummy = 0.
 */
#define AUDIO_HW_AMPLIFIER_TYPE         0

/*
 *After enable amplifer, it need some time to be steady. For d2 demo board, it's 90ms.
 */
#define AUDIO_HW_AMPLIFIER_ENABLE_TIME  90

/*
 *After disable amplifer, it need some time to be steady. For d2 demo board, it's 1ms.
 */
#define AUDIO_HW_AMPLIFIER_DISABLE_TIME 1

/*
 * Set 1 to let hal control amplifier's enable and disable automatically.
 * Set 0 to let user control amplifier's enable and disable.
 */
#define AUDIO_HW_AMPLIFIER_CONTROL_ENABLE  1

/*
 *When no sound playing, should amplifier mute? Set 1 if user wants amplifier mute, set
 *0 if user wants amplifier stay unmute.
 *For d2 and lite demo board, our hardware design makes sure aec noise acceptable, if
 *amplifier stays unmute, so it can be set 0.
 */
#define AUDIO_HW_AMPLIFIER_MUTE_ENABLE  0

/*
 *DMIC clock and data pins.
 */
#define AUDIO_HW_DMIC_CLK_PIN           _PB_22
#define AUDIO_HW_DMIC_DATA0_PIN         _PB_21
#define AUDIO_HW_DMIC_DATA1_PIN         _PB_18
#define AUDIO_HW_DMIC_DATA2_PIN         _PB_17
#define AUDIO_HW_DMIC_DATA3_PIN         _PB_16

/*
 *After dmic clock enable, it need some time to be steady. For d2 demo board, it's 100ms.
 *For customer's board, need to check with customer.
 */
#define AUDIO_HW_DMIC_STEADY_TIME       100

/*
 *Sport counter max irq X, when sport delivered X frames, there will be one interrupt.
 */
#define AUDIO_HW_MAX_SPORT_IRQ_X        134217727

/*
 *0: using xtal 40M.
 *1: using PLL, mclk is mulitiplier of fs.
 *2: using PLL, mclk is fixed value.
 */
#define AUDIO_HW_OUT_SPORT_CLK_TYPE     0

/*
 *Define whether the external audio codec need MCLK from soc.
 */
#define AUDIO_I2S_OUT_NEED_MCLK_OUT     1

/*
 *If using pll, mclk is mulitiplier of fs. Set mulitiplier here.
 */
#define AUDIO_HW_OUT_MCLK_MULITIPLIER   256

/*
 *If using pll, mclk is fixed value. Set mclk value here.
 */
#define AUDIO_HW_OUT_FIXED_MCLK         20000000

/*
 *Define I2S out sport index, for example, if I2S in using sport3, set 3 as value here.
 *For I2S out, soc always works as master.
 */
#define AUDIO_I2S_OUT_SPORT_INDEX       0

/*
 *Define I2S out's data format
 *0 for I2S
 *1 for Left Justified
 *2 for SP_DF_PCM_A
 *3 for SP_DF_PCM_B
 */
#define AUDIO_I2S_OUT_DATA_FORMAT        0

/*
 *Define I2S out pins.
 */
#define AUDIO_I2S_OUT_MCLK_PIN           _PB_7
#define AUDIO_I2S_OUT_BCLK_PIN           _PB_8
#define AUDIO_I2S_OUT_LRCLK_PIN          _PB_9
#define AUDIO_I2S_OUT_DATA0_PIN          _PB_10
#define AUDIO_I2S_OUT_DATA1_PIN          _PB_11
#define AUDIO_I2S_OUT_DATA2_PIN          _PA_4
#define AUDIO_I2S_OUT_DATA3_PIN          _PB_18

#define AUDIO_I2S_OUT_MULTIIO_EN         0

/*
 *0: using xtal 40M.
 *1: using PLL, mclk is mulitiplier of fs.
 *2: using PLL, mclk is fixed value.
 *3: using PLL, ignore mclk.
 */
#define AUDIO_HW_IN_SPORT_CLK_TYPE       0

/*
 *Define whether the external audio codec need MCLK from soc.
 */
#define AUDIO_I2S_IN_NEED_MCLK_OUT       0

/*
 *If using pll, mclk is mulitiplier of fs. Set mulitiplier here.
 */
#define AUDIO_HW_IN_MCLK_MULITIPLIER     256

/*
 *If using pll, mclk is fixed value. Set mclk value here.
 */
#define AUDIO_HW_IN_FIXED_MCLK           20000000

/*
 *Define I2S in sport index, for example, if I2S in using sport2, set 2 as value here.
 */
#define AUDIO_I2S_IN_SPORT_INDEX         2

#define AUDIO_I2S_MASTER                 0
#define AUDIO_I2S_SLAVE                  1

/*
 *Define I2S in role.
 *0: master.
 *1: slave.
 */
#define AUDIO_I2S_IN_ROLE                1

/*
 *Define I2S in pins.
 */
#define AUDIO_I2S_IN_MCLK_PIN           _PB_14
#define AUDIO_I2S_IN_BCLK_PIN           _PB_15
#define AUDIO_I2S_IN_LRCLK_PIN          _PB_16
#define AUDIO_I2S_IN_DATA0_PIN          _PB_17
#define AUDIO_I2S_IN_DATA1_PIN          _PB_18
#define AUDIO_I2S_IN_DATA2_PIN          _PB_31  /*_PB_19 for old board*/
#define AUDIO_I2S_IN_DATA3_PIN          _PB_20

#define AUDIO_I2S_IN_MULTIIO_EN         0

/*
 *Define I2S in's data format
 *0 for I2S
 *1 for Left Justified
 *2 for SP_DF_PCM_A
 *3 for SP_DF_PCM_B
 */
#define AUDIO_I2S_IN_DATA_FORMAT        0

/*
 *Set 1 to enable debug for buffer level.
 *Set 0 to disable debug for buffer level.
 */
#define AUDIO_OUT_DEBUG_BUFFER_LEVEL    0


#define AUDIO_TSTAMP_BY_COUNTER         0
#define AUDIO_TSTAMP_BY_RATIO           1
#define AUDIO_TSTAMP_BY_DMA             2

/*
 *Set audio out tstamp type can be AUDIO_TSTAMP_BY_COUNTER, AUDIO_TSTAMP_BY_RATIO, AUDIO_TSTAMP_BY_DMA.
 */
#define AUDIO_OUT_TSTAMP_TYPE           AUDIO_TSTAMP_BY_COUNTER

/*
 *Set 1 to use native system time for timestamp.
 *Set 0 to use other time for timestamp.
 *Please always use 1.
 */
#define AUDIO_USING_NATIVE_TIME                 1

#endif
