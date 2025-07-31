#ifndef _AUDIO_DMIC_SNR_THD_H_
#define _AUDIO_DMIC_SNR_THD_H_

#include "ameba_soc.h"

// To reduce harmonic for better SNR result
#define FFT_REDUCE_HARMONIC 1
#define FFT_HARMONIC_ORDER  6

// Enable FFT debug msg
#define FFT_DBG_INFO        0

#endif
