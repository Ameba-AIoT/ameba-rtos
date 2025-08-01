#ifndef _AUDIO_DMIC_SNR_THD_H_
#define _AUDIO_DMIC_SNR_THD_H_

#include "ameba_soc.h"

#if defined (CONFIG_AMEBASMART)

#define SPORT_RX_INDEX     1

#elif defined (CONFIG_AMEBALITE)

#define SPORT_RX_INDEX     0

#elif defined (CONFIG_AMEBADPLUS)

#define SPORT_RX_INDEX     0

#endif

#if defined (CONFIG_AMEBASMART)

#define MBED_AUDIO_DMIC_CLK     _PB_22
#define MBED_AUDIO_DMIC_DATA    _PB_21

#elif defined (CONFIG_AMEBALITE)

#define MBED_AUDIO_DMIC_CLK    _PA_23
#define MBED_AUDIO_DMIC_DATA   _PA_24

#elif defined (CONFIG_AMEBADPLUS)

#define MBED_AUDIO_DMIC_CLK    _PB_0
#define MBED_AUDIO_DMIC_DATA   _PB_1

#endif

void FFTTest(u32 *pSrc, u32 *SNR, u32 *SNRH);
void THDTest(u32 *pSrc, u32 *THD, u32 *THDH);

extern int example_mbed_audio_dmic_snr(void);

#endif
