#include "platform_autoconf.h"
#include "platform_stdlib.h"

#include "arm_math.h"
#include "arm_const_structs.h"

#include "snr_calculate.h"

static const char *const TAG = "TT_AUDIO";
#define TEST_LENGTH_SAMPLES 2048

/* ------------------------------------------------------------------
* Global variables for FFT Bin Example
* ------------------------------------------------------------------- */
volatile uint32_t ifftFlag = 0;
volatile uint32_t doBitReverse = 1;

/* Reference index at which max energy of bin ocuurs */
uint32_t MaxValueIndex = 0;
uint32_t MaxValueIndexH = 0;

#define ADC_LENGTH_SAMPLES  128

static  float32_t adcOutput[ADC_LENGTH_SAMPLES];
static  float32_t adcOutputH[ADC_LENGTH_SAMPLES];
static float32_t  adcInput_f32_unknownkhz[(ADC_LENGTH_SAMPLES * 2)];
static float32_t  adcInputH_f32_unknownkhz[(ADC_LENGTH_SAMPLES * 2)];

#define ADC_VERI_DATA_LEN       4096

static u32 ADCVeriDatDst[ADC_VERI_DATA_LEN];

void FFTTest(u32 *pSrc, u32 *SNR, u32 *SNRH)
{
	memcpy(ADCVeriDatDst, pSrc, sizeof(ADCVeriDatDst));

	u32 count16 = 0;
	u32 count16H = 0;
	int counttmp = 0;

	float32_t adcMaxValue = 0;
	float32_t adcNoise = 0;
	float32_t adcNoiseSqrt = 0;
	float32_t adcSignal = 0;
	float32_t adcSignalSqrt = 0;
	float32_t adcSNR = 0;

	float32_t adcMaxValueH = 0;
	float32_t adcNoiseH = 0;
	float32_t adcNoiseSqrtH = 0;
	float32_t adcSignalH = 0;
	float32_t adcSignalSqrtH = 0;
	float32_t adcSNRH = 0;

	// TODO: Wait for DAC Sine Wave end
	ifftFlag = 0;
	doBitReverse = 1;
	MaxValueIndex = 0;

	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES * 2); count16++) {
		adcInput_f32_unknownkhz[count16] = 0.0;
		adcInputH_f32_unknownkhz[count16] = 0.0;
	}
	for (count16 = 0; count16 < ADC_LENGTH_SAMPLES; count16++) {
		adcOutput[count16] = 0.0;
		adcOutputH[count16] = 0.0;
	}

	for (count16 = 0, counttmp = 3500; count16 < (ADC_LENGTH_SAMPLES * 2); count16 += 2, counttmp++) {
		adcInput_f32_unknownkhz[count16] = (short)(ADCVeriDatDst[counttmp] & 0xFFFF);
		adcInputH_f32_unknownkhz[count16] = (short)((ADCVeriDatDst[counttmp] & 0xFFFF0000) >> 16);
	}

#if FFT_DBG_INFO
	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES * 2); count16 += 4) {
		RTK_LOGS(TAG, RTK_LOG_INFO, "ADC input (f32):\n");
		printf("0x%04x: %f\t %f\t %f\t %f\n", count16, adcInput_f32_unknownkhz[count16],
			   adcInput_f32_unknownkhz[count16 + 1], adcInput_f32_unknownkhz[count16 + 2],
			   adcInput_f32_unknownkhz[count16 + 3]);
	}
#endif

	//Process the data through the CFFT/CIFFT module
	arm_cfft_f32(&arm_cfft_sR_f32_len128, adcInput_f32_unknownkhz, ifftFlag, doBitReverse);
	arm_cfft_f32(&arm_cfft_sR_f32_len128, adcInputH_f32_unknownkhz, ifftFlag, doBitReverse);

	//Process the data through the Complex Magnitude Module for calculating the magnitude at each bin
	arm_cmplx_mag_f32(adcInput_f32_unknownkhz, adcOutput, ADC_LENGTH_SAMPLES);
	arm_cmplx_mag_f32(adcInputH_f32_unknownkhz, adcOutputH, ADC_LENGTH_SAMPLES);

	//remove some data according to DAC wave quality
	adcOutput[0] = 0.0;
	adcOutput[1] = 0.0;
	adcOutput[2] = 0.0;

	adcOutputH[0] = 0.0;
	adcOutputH[1] = 0.0;
	adcOutputH[2] = 0.0;

	// Calculates maxValue and returns corresponding BIN value
	arm_max_f32(adcOutput, (ADC_LENGTH_SAMPLES / 2), &adcMaxValue, &MaxValueIndex);
	arm_max_f32(adcOutputH, (ADC_LENGTH_SAMPLES / 2), &adcMaxValueH, &MaxValueIndexH);

#if FFT_DBG_INFO
	printf("ADC input after FFT:\n");
	printf(">Index------------------------------------------\n");
	for (count16 = 0; count16 < ADC_LENGTH_SAMPLES; count16 += 4) {
		printf("ADC input (f32):\n");
		printf("0x%04x: %f\t %f\t %f\t %f\n", count16, adcInput_f32_unknownkhz[count16],
			   adcInput_f32_unknownkhz[count16 + 1], adcInput_f32_unknownkhz[count16 + 2],
			   adcInput_f32_unknownkhz[count16 + 3]);
	}

	printf("\nADC out after FFT:\n");
	printf(">Index------------------------------------------\n");
	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES); count16 += 4) {
		printf("0x%04x: %d\t %d\t %d\t %d\n", count16, adcOutput[count16],
			   adcOutput[count16 + 1], adcOutput[count16 + 2], adcOutput[count16 + 3]);
	}

	printf("\nADC out after FFT one col:\n");
	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES); count16 += 1) {
		printf("%f\n", adcOutput[count16]);
	}

	printf("Max value(variable):%f\n", adcMaxValue);
	printf("Max value index:%d, value:%f\n", MaxValueIndex, adcOutput[MaxValueIndex]);

	printf("\nADC remove the DC value by dicarding the first 3 value\n");
#endif

	// RTK_LOGI(TAG, "MaxValueIndex:%d,MaxValueIndexH:%d\n", MaxValueIndex, MaxValueIndexH);

#if FFT_REDUCE_HARMONIC

	for (count16 = 2; count16 <= FFT_HARMONIC_ORDER; count16++) {
		//printf("before adcOutput[%02d]: %f\n", (count16*MaxValueIndex), adcOutput[count16*MaxValueIndex]);//rtl_printf
		if ((count16 * MaxValueIndex < ADC_LENGTH_SAMPLES) && (count16 * MaxValueIndexH < ADC_LENGTH_SAMPLES)) {
			adcOutput[count16 * MaxValueIndex] = 0.0;
			adcOutputH[count16 * MaxValueIndexH] = 0.0;
		} else {
			RTK_LOGS(TAG, RTK_LOG_ERROR, "error index!!!\n");
			break;
		}
		//printf("adcOutput[%02d]: %f\n", (count16*MaxValueIndex), adcOutput[count16*MaxValueIndex]);//rtl_printf
	}
#endif

#if FFT_DBG_INFO
	printf("ADC SNR calculate:\n");
	printf("ADC square the output value\n");
#endif
	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES / 2); count16 += 1) {
		adcOutput[count16] = (adcOutput[count16] * adcOutput[count16]);
		adcOutputH[count16] = (adcOutputH[count16] * adcOutputH[count16]);
	}

#if FFT_DBG_INFO
	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES / 2); count16 += 2) {
		printf("%f\t%f\n", adcOutput[count16], adcOutput[count16 + 1]);
	}

	printf("ADC calculate noise:\n");
#endif

	for (count16 = 0, adcNoise = 0, adcNoiseH = 0; count16 < (ADC_LENGTH_SAMPLES / 2); count16 += 1) {
		if ((count16 != MaxValueIndex) && (count16 != (MaxValueIndex - 1)) && (count16 != (MaxValueIndex + 1))) {
			adcNoise += adcOutput[count16];
		}

		if ((count16 != MaxValueIndexH) && (count16 != (MaxValueIndexH - 1)) && (count16 != (MaxValueIndexH + 1))) {
			adcNoiseH += adcOutputH[count16];
		}
	}

	adcNoiseSqrt = 0.0;
	adcNoiseSqrtH = 0.0;
	arm_sqrt_f32(adcNoise, &adcNoiseSqrt);
	arm_sqrt_f32(adcNoiseH, &adcNoiseSqrtH);
	//printf("ADC noise sum:%f\t, noise sqrt:%f\n", adcNoise, adcNoiseSqrt);//rtl_printf
	//printf("ADC calculate signal:\n");

	adcSignal = adcOutput[MaxValueIndex - 1] + adcOutput[MaxValueIndex] + adcOutput[MaxValueIndex + 1];
	adcSignalH = adcOutputH[MaxValueIndexH - 1] + adcOutputH[MaxValueIndexH] + adcOutputH[MaxValueIndexH + 1];
	adcSignalSqrt = 0.0;
	adcSignalSqrtH = 0.0;
	arm_sqrt_f32(adcSignal, &adcSignalSqrt);
	arm_sqrt_f32(adcSignalH, &adcSignalSqrtH);
	//printf("ADC signal sum:%f\t, signal sqrt:%f\n", adcSignal, adcSignalSqrt);//rtl_printf
	//printf("ADC signal sum:%f\t, signal sqrt:%f\n", adcSignalH, adcSignalSqrtH);//rtl_printf

	//printf("ADC SNR:\n");
	adcSNR = log10(adcSignalSqrt / adcNoiseSqrt);
	adcSNR = 20 * adcSNR;
	//printf("SNR result: %f\n", adcSNR);//rtl_printf
	adcSNRH = log10(adcSignalSqrtH / adcNoiseSqrtH);
	adcSNRH = 20 * adcSNRH;
	//printf("SNRH result: %f\n", adcSNRH);//rtl_printf

	count16 = (int)(adcSNR);
	*SNR = count16;
	count16H = (int)(adcSNRH);
	*SNRH = count16H;

#if FFT_DBG_INFO
	RTK_LOGW(TAG, "SNR: %d\n", count16);//printf("SNR: %d\n", count16);
	RTK_LOGW(TAG, "SNRH: %d\n", count16H);//printf("SNRH: %d\n", count16H);
#endif

}

void THDTest(u32 *pSrc, u32 *THD, u32 *THDH)
{
	memcpy(ADCVeriDatDst, pSrc, sizeof(ADCVeriDatDst));

	u32 count16 = 0;
	u32 count16H = 0;
	int counttmp = 0;

	float32_t adcMaxValue = 0;
	float32_t adcNoise = 0;
	float32_t adcNoiseSqrt = 0;
	float32_t adcSignal = 0;
	float32_t adcSignalSqrt = 0;
	float32_t adcSNR = 0;

	float32_t adcMaxValueH = 0;
	float32_t adcNoiseH = 0;
	float32_t adcNoiseSqrtH = 0;
	float32_t adcSignalH = 0;
	float32_t adcSignalSqrtH = 0;
	float32_t adcSNRH = 0;

	// TODO: Wait for DAC Sine Wave end
	ifftFlag = 0;
	doBitReverse = 1;
	MaxValueIndex = 0;

	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES * 2); count16++) {
		adcInput_f32_unknownkhz[count16] = 0.0;
		adcInputH_f32_unknownkhz[count16] = 0.0;
	}
	for (count16 = 0; count16 < ADC_LENGTH_SAMPLES; count16++) {
		adcOutput[count16] = 0.0;
		adcOutputH[count16] = 0.0;
	}

	for (count16 = 0, counttmp = 3500; count16 < (ADC_LENGTH_SAMPLES * 2); count16 += 2, counttmp++) {
		adcInput_f32_unknownkhz[count16] = (short)(ADCVeriDatDst[counttmp] & 0xFFFF);
		adcInputH_f32_unknownkhz[count16] = (short)((ADCVeriDatDst[counttmp] & 0xFFFF0000) >> 16);
	}

	//Process the data through the CFFT/CIFFT module
	arm_cfft_f32(&arm_cfft_sR_f32_len128, adcInput_f32_unknownkhz, ifftFlag, doBitReverse);
	arm_cfft_f32(&arm_cfft_sR_f32_len128, adcInputH_f32_unknownkhz, ifftFlag, doBitReverse);

	//Process the data through the Complex Magnitude Module for calculating the magnitude at each bin
	arm_cmplx_mag_f32(adcInput_f32_unknownkhz, adcOutput, ADC_LENGTH_SAMPLES);
	arm_cmplx_mag_f32(adcInputH_f32_unknownkhz, adcOutputH, ADC_LENGTH_SAMPLES);

	//remove some data according to DAC wave quality
	adcOutput[0] = 0.0;
	adcOutput[1] = 0.0;
	adcOutput[2] = 0.0;

	adcOutputH[0] = 0.0;
	adcOutputH[1] = 0.0;
	adcOutputH[2] = 0.0;

	// Calculates maxValue and returns corresponding BIN value
	arm_max_f32(adcOutput, (ADC_LENGTH_SAMPLES / 2), &adcMaxValue, &MaxValueIndex);
	arm_max_f32(adcOutputH, (ADC_LENGTH_SAMPLES / 2), &adcMaxValueH, &MaxValueIndexH);

	for (count16 = 0; count16 < (ADC_LENGTH_SAMPLES / 2); count16 += 1) {
		adcOutput[count16] = (adcOutput[count16] * adcOutput[count16]);
		adcOutputH[count16] = (adcOutputH[count16] * adcOutputH[count16]);
	}

	for (count16 = 0, adcNoise = 0, adcNoiseH = 0; count16 < (ADC_LENGTH_SAMPLES / 2); count16 += 1) {
		if ((count16 != MaxValueIndex) && (count16 != (MaxValueIndex - 1)) && (count16 != (MaxValueIndex + 1))) {
			adcNoise += adcOutput[count16];
		}

		if ((count16 != MaxValueIndexH) && (count16 != (MaxValueIndexH - 1)) && (count16 != (MaxValueIndexH + 1))) {
			adcNoiseH += adcOutputH[count16];
		}
	}

	adcNoiseSqrt = 0.0;
	adcNoiseSqrtH = 0.0;
	arm_sqrt_f32(adcNoise, &adcNoiseSqrt);
	arm_sqrt_f32(adcNoiseH, &adcNoiseSqrtH);
	//printf("ADC noise sum:%f\t, noise sqrt:%f\n", adcNoise, adcNoiseSqrt);//rtl_printf

	//printf("ADC calculate signal:\n");
	adcSignal = adcOutput[MaxValueIndex - 1] + adcOutput[MaxValueIndex] + adcOutput[MaxValueIndex + 1];
	adcSignalH = adcOutputH[MaxValueIndexH - 1] + adcOutputH[MaxValueIndexH] + adcOutputH[MaxValueIndexH + 1];
	adcSignalSqrt = 0.0;
	adcSignalSqrtH = 0.0;
	arm_sqrt_f32(adcSignal, &adcSignalSqrt);
	arm_sqrt_f32(adcSignalH, &adcSignalSqrtH);

	adcSNR = log10(adcSignalSqrt / adcNoiseSqrt);
	adcSNR = 20 * adcSNR;
	adcSNRH = log10(adcSignalSqrtH / adcNoiseSqrtH);
	adcSNRH = 20 * adcSNRH;

	count16 = (int)(adcSNR);
	*THD = count16;
	count16H = (int)(adcSNRH);
	*THDH = count16H;
}
