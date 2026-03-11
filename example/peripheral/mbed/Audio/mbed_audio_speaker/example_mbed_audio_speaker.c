#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "ameba_rcc.h"
#include "main.h"
#include "platform_stdlib.h"

#include "ameba_sport.h"
#include "ameba_audio.h"

#include "os_wrapper.h"
#include "basic_types.h"
#include "section_config.h"
#include "example_audio_ext.h"

static const char *const TAG = "TT_AUDIO";

static SP_InitTypeDef SP_InitStruct;
static I2S_InitTypeDef I2S_InitStruct;
static struct SP_GDMA_STRUCT SPGdmaStruct;

u16 sine_16k[64] = {
	0x0000, 0x0000, 0x306c, 0x306c, 0x5979, 0x5979, 0x74e7, 0x74e7, 0x7e88, 0x7e88, 0x74e7, 0x74e7,
	0x5979, 0x5979, 0x306c, 0x306c, 0x0000, 0x0000, 0xcf93, 0xcf93, 0xa686, 0xa686, 0x8b18, 0x8b18,
	0x8177, 0x8177, 0x8b18, 0x8b18, 0xa686, 0xa686, 0xcf93, 0xcf93, 0x0000, 0x0000, 0x306c, 0x306c,
	0x5979, 0x5979, 0x74e7, 0x74e7, 0x7e88, 0x7e88, 0x74e7, 0x74e7, 0x5979, 0x5979, 0x306c, 0x306c,
	0x0000, 0x0000, 0xcf93, 0xcf93, 0xa686, 0xa686, 0x8b18, 0x8b18, 0x8177, 0x8177, 0x8b18, 0x8b18,
	0xa686, 0xa686, 0xcf93, 0xcf93,
};

rtos_sema_t Task_audio_spk_Handler;

u32 AudioHandler(void *Data)
{
	struct SP_GDMA_STRUCT *gs = (struct SP_GDMA_STRUCT *)Data;
	PGDMA_InitTypeDef GDMA_InitStruct;
	GDMA_InitStruct = &(gs->SpTxGdmaInitStruct);

	/* Clear Pending ISR */
	GDMA_ClearINT(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum);
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
	return 0;
}

static void example_audio_init(void)
{
	RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, ENABLE);

#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_AUDIO, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockSource_SPORT(0, CKSL_I2S_XTAL40M);
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_AC_AIP, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockSource_SPORT(CKSL_I2S_XTAL40M);
#endif

	RCC_PeriphClockSource_AUDIOCODEC(CKSL_AC_XTAL);

	/*configure Sport according to the parameters*/
	AUDIO_SP_Reset(0);
	AUDIO_SP_StructInit(&SP_InitStruct);
	SP_InitStruct.SP_SelI2SMonoStereo = SP_CH_STEREO;
	SP_InitStruct.SP_SelWordLen = SP_TXWL_16;
	SP_InitStruct.SP_SR = SP_16K;

#if defined (CONFIG_AMEBALITE)
	SP_InitStruct.SP_SelTDM = I2S_NOTDM;
	// SP_InitStruct.SP_Fix_Bclk = ENABLE;
#endif

	AUDIO_SP_Init(0, SP_DIR_TX, &SP_InitStruct);

	AUDIO_CODEC_SetLDOMode(POWER_ON);
	AUDIO_CODEC_I2S_StructInit(&I2S_InitStruct);
	I2S_InitStruct.CODEC_SelI2STxSR = SR_16K;
	I2S_InitStruct.CODEC_SelI2STxWordLen = SP_TXWL_16;
	AUDIO_CODEC_Playback(I2S0, APP_LINE_OUT, &I2S_InitStruct);
}

void audio_deinit(void)
{
	AUDIO_SP_DmaCmd(0, DISABLE);
	AUDIO_SP_TXStart(0, DISABLE);

	GDMA_ClearINT(SPGdmaStruct.SpTxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpTxGdmaInitStruct.GDMA_ChNum);
	GDMA_Abort(SPGdmaStruct.SpTxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpTxGdmaInitStruct.GDMA_ChNum);
	GDMA_ChnlFree(SPGdmaStruct.SpTxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpTxGdmaInitStruct.GDMA_ChNum);

#if defined (CONFIG_AMEBASMART)
	AUDIO_CODEC_SetDACPowerMode(DAC_L, POWER_DOWN);
	AUDIO_CODEC_SetLineOutPowerMode(CHN_LR, POWER_DOWN);
	AUDIO_CODEC_SetDACPowerMode(DAC_R, POWER_DOWN);
#elif defined (CONFIG_AMEBALITE)
	AUDIO_CODEC_SetDACPowerMode(DAC_L, POWER_DOWN);
	AUDIO_CODEC_SetLineOutPowerMode(DAC_L, POWER_DOWN);
#endif

	AUDIO_CODEC_SetAudioIP(DISABLE);
	AUDIO_CODEC_SetI2SIP(I2S0, DISABLE);

	AUDIO_SP_Unregister(0, SP_DIR_TX);
	AUDIO_SP_Reset(0);

	/* set ldo power down */
	AUDIO_CODEC_SetLDOMode(POWER_DOWN);
	/* disable RCC of sport and codec */
	RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, DISABLE);
#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_AUDIO, APBPeriph_CLOCK_NULL, DISABLE);
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_AC_AIP, APBPeriph_CLOCK_NULL, DISABLE);
#endif

}

#define EXTERNAL_INPUT 1

void example_audio_spk_thread(void)
{
	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio codec play demo begin......\n");
	u16 *tone_data = 0;
	u32 total_len = 0;

	example_audio_init();

#if defined (CONFIG_AMEBALITE)
	/* for 8726e */
#if EXTERNAL_INPUT
	AUDIO_CODEC_SetDACVolume(DAC_L, 0X86);
	/*GPIO À­¸ß */
	GPIO_WriteBit(_PA_12, 0);
	GPIO_InitTypeDef GPIO_InitStruct_Temp;
	GPIO_InitStruct_Temp.GPIO_Pin = _PA_12;
	GPIO_InitStruct_Temp.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Init(&GPIO_InitStruct_Temp);
	GPIO_WriteBit(_PA_12, 1);
#endif

#endif

	tone_data = sine_16k;
	total_len = sizeof(sine_16k);

	/* Enable GMDA and SPORT handshake*/
	AUDIO_SP_DmaCmd(0, ENABLE);
	AUDIO_SP_TXStart(0, ENABLE);
	AUDIO_SP_TXGDMA_Init(0, GDMA_INT, &SPGdmaStruct.SpTxGdmaInitStruct, &SPGdmaStruct, (IRQ_FUN)AudioHandler, (u8 *)tone_data, total_len);

	rtos_task_delete(Task_audio_spk_Handler);
}

int example_mbed_audio_speaker(void)
{
	if (RTK_SUCCESS != rtos_task_create(&Task_audio_spk_Handler, "example_audio_spk_thread", (rtos_task_t)example_audio_spk_thread, NULL, 1024, 2)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\n xTaskCreate(example_audio_spk_thread) failed\n");
	}

	rtos_sched_start();
	return 0;
}
