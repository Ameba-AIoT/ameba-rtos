#ifndef _AUDIO_SPEAKER_TEST_H_
#define _AUDIO_SPEAKER_TEST_H_

#include "ameba_soc.h"

struct SP_GDMA_STRUCT {
	GDMA_InitTypeDef       	SpTxGdmaInitStruct;              //Pointer to GDMA_InitTypeDef
};

extern int example_mbed_audio_speaker(void);

#endif
