#ifndef _AUDIO_AMIC_SNR_THD_H_
#define _AUDIO_AMIC_SNR_THD_H_

#include "ameba_soc.h"

#if defined (CONFIG_AMEBASMART)

#define SPORT_RX_INDEX     1

#elif defined (CONFIG_AMEBALITE)

#define SPORT_RX_INDEX     0

#endif

void FFTTest(u32 *pSrc, u32 *SNR, u32 *SNRH);
void THDTest(u32 *pSrc, u32 *THD, u32 *THDH);

extern int example_mbed_audio_amic_snr(void);

#endif
