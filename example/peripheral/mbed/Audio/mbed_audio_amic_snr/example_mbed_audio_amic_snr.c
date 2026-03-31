#include "platform_autoconf.h"
#include "ameba_soc.h"
#include "main.h"

#include "platform_stdlib.h"
#include "basic_types.h"
#include "ameba_rcc.h"

#include "ameba_sport.h"
#include "ameba_audio.h"

#include "os_wrapper.h"
#include "section_config.h"

#include "audio_rx.h"
#include "example_audio_ext.h"
#include "snr_calculate.h"

static const char *const TAG = "TT_AUDIO";

static SP_InitTypeDef SP_InitStruct;
static I2S_InitTypeDef I2S_InitStruct;
static struct SP_GDMA_STRUCT SPGdmaStruct;

#if defined (CONFIG_AMEBASMART)

volatile u8 sp_rx_buf[SP_RX_DMA_PAGE_SIZE * SP_RX_DMA_PAGE_NUM];
volatile u8 sp_rx_buf_ext[SP_RX_DMA_PAGE_SIZE_EXT * SP_RX_DMA_PAGE_NUM_EXT];
volatile u8 sp_full_buf[SP_FULL_BUF_SIZE];
volatile u8 sp_full_buf_ext[SP_FULL_BUF_SIZE_EXT];

u8 sp_rx_buf_1[SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2)];
u8 sp_rx_buf_2[SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2)];
u8 sp_rx_buf_3[SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2)];

#elif defined (CONFIG_AMEBALITE)

volatile u8 sp_rx_buf[SP_RX_DMA_PAGE_SIZE * SP_RX_DMA_PAGE_NUM];
volatile u8 sp_full_buf[SP_FULL_BUF_SIZE];
u8 sp_rx_buf_1[SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2)];
u8 sp_rx_buf_2[SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2)];

#endif

volatile u8 record_flag = 1;
extern volatile u32 pagecnt;

#if defined (CONFIG_AMEBASMART)
volatile u8 record_flag_ext = 1;
extern volatile u32 pagecnt_ext;
#endif

#define IS_SNR_NOT_90DB(x)	((x) < 90)
#define IS_SNR_NOT_70DB(x)	((x) < 70)

static void example_audio_init(void)
{
	/* Enable SPORT/AUDIO CODEC CLOCK and Function */
#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_SPORT1, APBPeriph_SPORT1_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_AUDIO, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockSource_SPORT(SPORT_RX_INDEX, CKSL_I2S_XTAL40M);
#elif defined (CONFIG_AMEBALITE)
	RCC_PeriphClockCmd(APBPeriph_SPORT0, APBPeriph_SPORT0_CLOCK, ENABLE);
	RCC_PeriphClockCmd(APBPeriph_AC_AIP, APBPeriph_CLOCK_NULL, ENABLE);
	RCC_PeriphClockSource_SPORT(CKSL_I2S_XTAL40M);
#endif
	RCC_PeriphClockSource_AUDIOCODEC(CKSL_AC_XTAL);
	RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, ENABLE);

	/*configure Sport according to the parameters*/
	AUDIO_SP_Reset(SPORT_RX_INDEX);
	AUDIO_SP_StructInit(&SP_InitStruct);
	SP_InitStruct.SP_SR = SP_16K;

#if defined (CONFIG_AMEBASMART)
	SP_InitStruct.SP_SelTDM = SP_RX_TDM6;
	SP_InitStruct.SP_SelFIFO = SP_RX_FIFO6;
#elif defined (CONFIG_AMEBALITE)
	SP_InitStruct.SP_SelWordLen = SP_RXWL_16;
	SP_InitStruct.SP_SelI2SMonoStereo = SP_CH_STEREO;
	SP_InitStruct.SP_SelTDM = I2S_TDM4;
	SP_InitStruct.SP_SelFIFO = SP_RX_FIFO4;
	// SP_InitStruct.SP_Fix_Bclk = ENABLE;
#endif

	AUDIO_SP_Init(SPORT_RX_INDEX, SP_DIR_RX, &SP_InitStruct);

	AUDIO_CODEC_SetLDOMode(POWER_ON);
	AUDIO_CODEC_I2S_StructInit(&I2S_InitStruct);

#if defined (CONFIG_AMEBASMART)
	I2S_InitStruct.CODEC_SelRxI2STdm = I2S_TDM6;
#elif defined (CONFIG_AMEBALITE)
	I2S_InitStruct.CODEC_SelRxI2STdm = I2S_TDM4;
#endif

	I2S_InitStruct.CODEC_SelI2SRxSR = SR_16K;
	AUDIO_CODEC_Record(SPORT_RX_INDEX, APP_AMIC_RECORD, &I2S_InitStruct);
}

void audio_deinit(void)
{
	AUDIO_SP_DmaCmd(SPORT_RX_INDEX, DISABLE);
	AUDIO_SP_RXStart(SPORT_RX_INDEX, DISABLE);

	GDMA_ClearINT(SPGdmaStruct.SpRxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStruct.GDMA_ChNum);
	GDMA_Abort(SPGdmaStruct.SpRxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStruct.GDMA_ChNum);
	GDMA_ChnlFree(SPGdmaStruct.SpRxGdmaInitStruct.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStruct.GDMA_ChNum);

#if defined (CONFIG_AMEBASMART)
	GDMA_ClearINT(SPGdmaStruct.SpRxGdmaInitStructExt.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStructExt.GDMA_ChNum);
	GDMA_Abort(SPGdmaStruct.SpRxGdmaInitStructExt.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStructExt.GDMA_ChNum);
	GDMA_ChnlFree(SPGdmaStruct.SpRxGdmaInitStructExt.GDMA_Index, SPGdmaStruct.SpRxGdmaInitStructExt.GDMA_ChNum);
#endif

	AUDIO_CODEC_SetMicBstPowerMode(AMIC1, SHUTDOWN);
	AUDIO_CODEC_SetMicBstPowerMode(AMIC2, SHUTDOWN);
	AUDIO_CODEC_SetMicBstPowerMode(AMIC3, SHUTDOWN);
#if defined (CONFIG_AMEBASMART)
	AUDIO_CODEC_SetMicBstPowerMode(AMIC4, SHUTDOWN);
	AUDIO_CODEC_SetMicBstPowerMode(AMIC5, SHUTDOWN);
#endif
	AUDIO_CODEC_SetMicBiasPowerMode(POWER_DOWN);

	AUDIO_CODEC_SetAudioIP(DISABLE);
	AUDIO_CODEC_SetI2SIP(I2S1, DISABLE);

	AUDIO_SP_Unregister(SPORT_RX_INDEX, SP_DIR_RX);
	AUDIO_SP_Reset(SPORT_RX_INDEX);

	/* set ldo power down */
	AUDIO_CODEC_SetLDOMode(POWER_DOWN);
	/* disable RCC of sport and codec */
#if defined (CONFIG_AMEBASMART)
	RCC_PeriphClockCmd(APBPeriph_AUDIO, APBPeriph_CLOCK_NULL, DISABLE);
#else
	RCC_PeriphClockCmd(APBPeriph_AC_AIP, APBPeriph_CLOCK_NULL, DISABLE);
#endif
	RCC_PeriphClockCmd(APBPeriph_SPORT1, APBPeriph_SPORT1_CLOCK, DISABLE);
	RCC_PeriphClockCmd(APBPeriph_AC, APBPeriph_AC_CLOCK, DISABLE);
}

void example_audio_amic_recorder_thread(void)
{
	u32 rx_addr;
	u32 rx_length;

#if defined (CONFIG_AMEBASMART)
	u32 rx_addr_ext;
	u32 rx_length_ext;
#endif

	u32 ErrFlag_SNR = 0;
	u32 ErrFlag_THD = 0;

	u32 adcSNR1, adcSNR2, adcSNR3, adcSNR4;
	u32 adcTHD1, adcTHD2, adcTHD3, adcTHD4;
#if defined (CONFIG_AMEBASMART)
	u32 adcSNR5, adcSNR6;
	u32 adcTHD5, adcTHD6;
#endif

	RTK_LOGS(TAG, RTK_LOG_INFO, "Audio amic recorder demo begin......\n");

	example_audio_init();
	sp_init_rx_variables((u8 *)sp_rx_buf, (u8 *)sp_full_buf);
#if defined (CONFIG_AMEBASMART)
	sp_init_rx_variables_ext((u8 *)sp_rx_buf_ext, (u8 *)sp_full_buf_ext);
#endif

	while (1) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "enter record process\n");
		AUDIO_SP_DmaCmd(SPORT_RX_INDEX, ENABLE);
		rx_addr = (u32)sp_get_free_rx_page();
		rx_length = sp_get_free_rx_length();
		AUDIO_SP_RXGDMA_Init(SPORT_RX_INDEX, GDMA_INT, &SPGdmaStruct.SpRxGdmaInitStruct, &SPGdmaStruct, (IRQ_FUN)sp_rx_complete, (u8 *)rx_addr, rx_length);

#if defined (CONFIG_AMEBASMART)
		rx_addr_ext = (u32)sp_get_free_rx_page_ext();
		rx_length_ext = sp_get_free_rx_length_ext();
		AUDIO_SP_RXGDMA_Init(SPORT_RX_INDEX, GDMA_EXT, &SPGdmaStruct.SpRxGdmaInitStructExt, &SPGdmaStruct, (IRQ_FUN)sp_rx_complete_ext, (u8 *)rx_addr_ext,
							 rx_length_ext);
#endif

		AUDIO_SP_RXStart(SPORT_RX_INDEX, ENABLE);

#if defined (CONFIG_AMEBASMART)
		while (!((record_flag == 0) && (record_flag_ext == 0)));

		AUDIO_SP_DmaCmd(SPORT_RX_INDEX, DISABLE);
		AUDIO_SP_RXStart(SPORT_RX_INDEX, DISABLE);

		RTK_LOGS(TAG, RTK_LOG_INFO, "enter record end\n");

		record_flag = 1;
		record_flag_ext = 1;
		pagecnt = 0;
		pagecnt_ext = 0;

		sp_init_rx_variables((u8 *)sp_rx_buf, (u8 *)sp_full_buf);
		sp_init_rx_variables_ext((u8 *)sp_rx_buf_ext, (u8 *)sp_full_buf_ext);

#elif defined (CONFIG_AMEBALITE)
		while (record_flag);

		AUDIO_SP_DmaCmd(SPORT_RX_INDEX, DISABLE);
		AUDIO_SP_RXStart(SPORT_RX_INDEX, DISABLE);

		record_flag = 1;
		pagecnt = 0;

		sp_init_rx_variables((u8 *)sp_rx_buf, (u8 *)sp_full_buf);

#endif
		DCache_CleanInvalidate((u32)sp_rx_buf, SP_RX_DMA_PAGE_SIZE * SP_RX_DMA_PAGE_NUM);
#if defined (CONFIG_AMEBASMART)
		DCache_CleanInvalidate((u32)sp_rx_buf_ext, SP_RX_DMA_PAGE_SIZE_EXT * SP_RX_DMA_PAGE_NUM_EXT);
		DCache_CleanInvalidate((u32)sp_rx_buf_1, SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2));
		DCache_CleanInvalidate((u32)sp_rx_buf_2, SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2));
		DCache_CleanInvalidate((u32)sp_rx_buf_3, SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2));

		u32 i, j;
		for (i = 0, j = 0; j < (sizeof(sp_rx_buf)) / 4; i++, j += 2) {
			((u32 *)sp_rx_buf_1)[i] = ((u32 *)sp_rx_buf)[j];
			((u32 *)sp_rx_buf_2)[i] = ((u32 *)sp_rx_buf)[j + 1];
		}
		for (i = 0; i < (sizeof(sp_rx_buf_ext)) / 4; i++) {
			((u32 *)sp_rx_buf_3)[i] = ((u32 *)sp_rx_buf_ext)[i];
		}

		/*for SNR test*/
		FFTTest((u32 *)sp_rx_buf_1, &adcSNR1, &adcSNR2);
		FFTTest((u32 *)sp_rx_buf_2, &adcSNR3, &adcSNR4);
		FFTTest((u32 *)sp_rx_buf_3, &adcSNR5, &adcSNR6);

		if (IS_SNR_NOT_90DB(adcSNR1) || IS_SNR_NOT_90DB(adcSNR2) || \
			IS_SNR_NOT_90DB(adcSNR3) || IS_SNR_NOT_90DB(adcSNR5) || \
			IS_SNR_NOT_90DB(adcSNR4)) {
			ErrFlag_SNR++;
		}

		RTK_LOGI(TAG, "AMIC1 SNR: %d\n", adcSNR1);
		RTK_LOGI(TAG, "AMIC2 SNR: %d\n", adcSNR2);
		RTK_LOGI(TAG, "AMIC3 SNR: %d\n", adcSNR3);
		RTK_LOGI(TAG, "AMIC4 SNR: %d\n", adcSNR4);
		RTK_LOGI(TAG, "AMIC5 SNR: %d\n", adcSNR5);
		/*for THD test*/
		THDTest((u32 *)sp_rx_buf_1, &adcTHD1, &adcTHD2);
		THDTest((u32 *)sp_rx_buf_2, &adcTHD3, &adcTHD4);
		THDTest((u32 *)sp_rx_buf_3, &adcTHD5, &adcTHD6);

		if (IS_SNR_NOT_70DB(adcTHD1) || IS_SNR_NOT_70DB(adcTHD2) || \
			IS_SNR_NOT_70DB(adcTHD3) || IS_SNR_NOT_70DB(adcTHD4) || \
			IS_SNR_NOT_70DB(adcTHD5)) {
			ErrFlag_THD++;
		}

		RTK_LOGI(TAG, "AMIC1 THD: %d\n", adcTHD1);
		RTK_LOGI(TAG, "AMIC2 THD: %d\n", adcTHD2);
		RTK_LOGI(TAG, "AMIC3 THD: %d\n", adcTHD3);
		RTK_LOGI(TAG, "AMIC4 THD: %d\n", adcTHD4);
		RTK_LOGI(TAG, "AMIC5 THD: %d\n", adcTHD5);

		if (ErrFlag_SNR > 0) {
			ErrFlag_SNR = 0;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "<== AUDIO SNR TEST fAIL!\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "<== AUDIO SNR TEST PASS!\n");
		}

		if (ErrFlag_THD > 0) {
			ErrFlag_THD = 0;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "<== AUDIO THD TEST fAIL!\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "<== AUDIO THD TEST PASS!\n");
		}

		rtos_time_delay_ms(100);
#elif defined (CONFIG_AMEBALITE)

		DCache_CleanInvalidate((u32)sp_rx_buf_1, SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2));
		DCache_CleanInvalidate((u32)sp_rx_buf_2, SP_RX_DMA_PAGE_SIZE * (SP_RX_DMA_PAGE_NUM / 2));

		u32 i, j;
		for (i = 0, j = 0; j < (sizeof(sp_rx_buf)) / 4; i++, j += 2) {
			((u32 *)sp_rx_buf_1)[i] = ((u32 *)sp_rx_buf)[j];
			((u32 *)sp_rx_buf_2)[i] = ((u32 *)sp_rx_buf)[j + 1];
		}

		//RTK_LOGI(TAG, "Debug %x %x\n", sp_rx_buf_1, sp_rx_buf_2);

		FFTTest((u32 *)sp_rx_buf_1, &adcSNR1, &adcSNR2);
		FFTTest((u32 *)sp_rx_buf_2, &adcSNR3, &adcSNR4);

		if (IS_SNR_NOT_90DB(adcSNR1) || IS_SNR_NOT_90DB(adcSNR2) || \
			IS_SNR_NOT_90DB(adcSNR3)) {
			ErrFlag_SNR++;
		}

		RTK_LOGI(TAG, "AMIC1 SNR: %d\n", adcSNR1);
		RTK_LOGI(TAG, "AMIC2 SNR: %d\n", adcSNR2);
		RTK_LOGI(TAG, "AMIC3 SNR: %d\n", adcSNR3);

		/*for THD test*/
		THDTest((u32 *)sp_rx_buf_1, &adcTHD1, &adcTHD2);
		THDTest((u32 *)sp_rx_buf_2, &adcTHD3, &adcTHD4);

		if (IS_SNR_NOT_70DB(adcTHD1) || IS_SNR_NOT_70DB(adcTHD2) || IS_SNR_NOT_70DB(adcTHD3)) {
			ErrFlag_THD++;
		}

		RTK_LOGI(TAG, "AMIC1 THD: %d\n", adcTHD1);
		RTK_LOGI(TAG, "AMIC2 THD: %d\n", adcTHD2);
		RTK_LOGI(TAG, "AMIC3 THD: %d\n", adcTHD3);

		if (ErrFlag_SNR > 0) {
			ErrFlag_SNR = 0;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "<== AUDIO SNR TEST fAIL!\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "<== AUDIO SNR TEST PASS!\n");
		}

		if (ErrFlag_THD > 0) {
			ErrFlag_THD = 0;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "<== AUDIO THD TEST fAIL!\n");
		} else {
			RTK_LOGS(TAG, RTK_LOG_INFO, "<== AUDIO THD TEST PASS!\n");
		}

		rtos_time_delay_ms(50);
#endif
	}

	audio_deinit();
	rtos_task_delete(NULL);
}

int example_mbed_audio_amic_snr(void)
{
	if (RTK_SUCCESS != rtos_task_create(NULL, "example_audio_amic_recorder_thread", (rtos_task_t)example_audio_amic_recorder_thread, NULL, 2048, 1)) {
		RTK_LOGS(TAG, RTK_LOG_ERROR, "\n xTaskCreate(example_audio_amic_recorder_thread) failed\n");
	}

	rtos_sched_start();
	return 0;
}
