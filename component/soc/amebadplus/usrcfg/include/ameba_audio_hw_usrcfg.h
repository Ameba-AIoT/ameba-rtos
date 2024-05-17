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
#define AUDIO_HW_AMPLIFIER_PIN          _PA_31

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
#define AUDIO_HW_DMIC_CLK_PIN           _PB_0
#define AUDIO_HW_DMIC_DATA0_PIN         _PB_1
// #define AUDIO_HW_DMIC_DATA1_PIN         _PB_18
// #define AUDIO_HW_DMIC_DATA2_PIN         _PB_17
// #define AUDIO_HW_DMIC_DATA3_PIN         _PB_16

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
 * if using PLL, please make sure to change soc/amebadplus/usrcfg/ameba_bootcfg.c's clock
 * as 98.304->5, 45.1584->4
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
 *Define I2S out sport index, for example, if I2S in using sport0, set 0 as value here.
 *For I2S out, soc always works as master.
 */
#define AUDIO_I2S_OUT_SPORT_INDEX       0

/*
 *Define I2S out pins.
 */
#define AUDIO_I2S_OUT_MCLK_PIN           _PA_19
#define AUDIO_I2S_OUT_BCLK_PIN           _PA_23
#define AUDIO_I2S_OUT_LRCLK_PIN          _PA_20
#define AUDIO_I2S_OUT_DATA0_PIN          _PA_22
#define AUDIO_I2S_OUT_DATA1_PIN          _PB_17
#define AUDIO_I2S_OUT_DATA2_PIN          _PB_18
#define AUDIO_I2S_OUT_DATA3_PIN          _PB_19

#define AUDIO_I2S_OUT_MULTIIO_EN         1

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
#define AUDIO_I2S_IN_SPORT_INDEX         1

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
#define AUDIO_I2S_IN_MCLK_PIN           _PB_10
#define AUDIO_I2S_IN_BCLK_PIN           _PA_23
#define AUDIO_I2S_IN_LRCLK_PIN          _PA_20
#define AUDIO_I2S_IN_DATA0_PIN          _PA_21
#define AUDIO_I2S_IN_DATA1_PIN          _PA_22
#define AUDIO_I2S_IN_DATA2_PIN          _PB_22
#define AUDIO_I2S_IN_DATA3_PIN          _PB_3

/*
 *Define I2S in sport index, for example, if I2S in using sport2, set 2 as value here.
 */
#define AUDIO_I2S_IN_EXTRA_SPORT_INDEX   1

/*
 *Define I2S extra pins.
 */
#define AUDIO_I2S_IN_EXTRA_MCLK_PIN           -1
#define AUDIO_I2S_IN_EXTRA_BCLK_PIN           _PA_26
#define AUDIO_I2S_IN_EXTRA_LRCLK_PIN          _PA_27
#define AUDIO_I2S_IN_EXTRA_DATA0_PIN          _PB_21
#define AUDIO_I2S_IN_EXTRA_DATA1_PIN          -1
#define AUDIO_I2S_IN_EXTRA_DATA2_PIN          -1
#define AUDIO_I2S_IN_EXTRA_DATA3_PIN          -1

/*
 *Define whether the external audio codec need MCLK from soc.
 */
#define AUDIO_I2S_IN_EXTRA_NEED_MCLK_OUT       0


#define AUDIO_I2S_IN_MULTIIO_EN                1

#define AUDIO_I2S_IN_EXTRA_MULTIIO_EN          1

/*
 *Set 1 to enable debug for buffer level.
 *Set 0 to disable debug for buffer level.
 */
#define AUDIO_OUT_DEBUG_BUFFER_LEVEL           0

#endif
