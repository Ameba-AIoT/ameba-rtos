#ifndef _SPEECH_MIND_PC_RECORDER
#define _SPEECH_MIND_PC_RECORDER

#define RECORD_PAGE_SIZE        4096

int32_t PCRecorder_dump(void *buffer);
void PCRecorder_init(void);
void PCRecorder_deInit(void);

#endif

