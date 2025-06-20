#ifndef _AIVOICE_SDK_CONFIG_H
#define _AIVOICE_SDK_CONFIG_H

typedef enum {
	AIVOICE_MEMORY_ALLOC_MODE_DEFAULT = 0,  /* only use sdk default heap (PSRAM) */
	AIVOICE_MEMORY_ALLOC_MODE_SRAM = 1,     /* use sdk default heap,
                                               and allocate space from SRAM for memory critical data.
                                               ONLY available on Hifi5DSP platform.
                                             */
} aivoice_memory_alloc_mode_e;

struct aivoice_sdk_config {
	int no_cmd_timeout;     /* In full flow, ASR exits when no command word detected during this duration.
                               In AFE+KWS+VAD flow, VAD works only within this duration after a keyword detected.
                               Unit: second. Recommend range [5, 60] */

	aivoice_memory_alloc_mode_e memory_alloc_mode;
};

/************ default configurations **********/
#define AIVOICE_SDK_CONFIG_DEFAULT() {\
    .no_cmd_timeout=10,\
    .memory_alloc_mode=AIVOICE_MEMORY_ALLOC_MODE_DEFAULT,\
};

#endif
