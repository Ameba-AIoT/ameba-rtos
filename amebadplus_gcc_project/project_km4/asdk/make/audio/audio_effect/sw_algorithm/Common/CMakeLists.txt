set(
    MODULE_IFLAGS 
    ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/include
)

set(DIR ${BASEDIR}/component/audio/audio_effect/sw_algorithm/Common/src)

set(MODULE_C_DEFINES __RTOS__ NDEBUG)
set(MODULE_C_OPTIONS -UDEBUG -Werror)

set(
    CSRC
    ${DIR}/InstAlloc.c
    ${DIR}/DC_2I_D16_TRC_WRA_01.c
    ${DIR}/DC_2I_D16_TRC_WRA_01_Init.c
    ${DIR}/FO_2I_D16F32C15_LShx_TRC_WRA_01.c
    ${DIR}/FO_2I_D16F32Css_LShx_TRC_WRA_01_Init.c
    ${DIR}/FO_1I_D16F16C15_TRC_WRA_01.c
    ${DIR}/FO_1I_D16F16Css_TRC_WRA_01_Init.c
    ${DIR}/BP_1I_D16F32C30_TRC_WRA_01.c
    ${DIR}/BP_1I_D16F16C14_TRC_WRA_01.c
    ${DIR}/BP_1I_D32F32C30_TRC_WRA_02.c
    ${DIR}/BP_1I_D16F16Css_TRC_WRA_01_Init.c
    ${DIR}/BP_1I_D16F32Cll_TRC_WRA_01_Init.c
    ${DIR}/BP_1I_D32F32Cll_TRC_WRA_02_Init.c
    ${DIR}/BQ_2I_D32F32Cll_TRC_WRA_01_Init.c
    ${DIR}/BQ_2I_D32F32C30_TRC_WRA_01.c
    ${DIR}/BQ_2I_D16F32C15_TRC_WRA_01.c
    ${DIR}/BQ_2I_D16F32C14_TRC_WRA_01.c
    ${DIR}/BQ_2I_D16F32C13_TRC_WRA_01.c
    ${DIR}/BQ_2I_D16F32Css_TRC_WRA_01_init.c
    ${DIR}/BQ_2I_D16F16C15_TRC_WRA_01.c
    ${DIR}/BQ_2I_D16F16C14_TRC_WRA_01.c
    ${DIR}/BQ_2I_D16F16Css_TRC_WRA_01_Init.c
    ${DIR}/BQ_1I_D16F16C15_TRC_WRA_01.c
    ${DIR}/BQ_1I_D16F16Css_TRC_WRA_01_Init.c
    ${DIR}/BQ_1I_D16F32C14_TRC_WRA_01.c
    ${DIR}/BQ_1I_D16F32Css_TRC_WRA_01_init.c
    ${DIR}/PK_2I_D32F32C30G11_TRC_WRA_01.c
    ${DIR}/PK_2I_D32F32C14G11_TRC_WRA_01.c
    ${DIR}/PK_2I_D32F32CssGss_TRC_WRA_01_Init.c
    ${DIR}/PK_2I_D32F32CllGss_TRC_WRA_01_Init.c
    ${DIR}/Int16LShiftToInt32_16x32.c
    ${DIR}/From2iToMono_16.c
    ${DIR}/Copy_16.c
    ${DIR}/MonoTo2I_16.c
    ${DIR}/LoadConst_16.c
    ${DIR}/dB_to_Lin32.c
    ${DIR}/Shift_Sat_v16xv16.c
    ${DIR}/Abs_32.c
    ${DIR}/Int32RShiftToInt16_Sat_32x16.c
    ${DIR}/From2iToMono_32.c
    ${DIR}/mult3s_16x16.c
    ${DIR}/NonLinComp_D16.c
    ${DIR}/DelayMix_16x16.c
    ${DIR}/MSTo2i_Sat_16x16.c
    ${DIR}/From2iToMS_16x16.c
    ${DIR}/Mac3s_Sat_16x16.c
    ${DIR}/Add2_Sat_16x16.c
    ${DIR}/LVC_MixSoft_1St_2i_D16C31_SAT.c
    ${DIR}/LVC_MixSoft_1St_D16C31_SAT.c
    ${DIR}/LVC_Mixer_VarSlope_SetTimeConstant.c
    ${DIR}/LVC_Mixer_SetTimeConstant.c
    ${DIR}/LVC_Mixer_SetTarget.c
    ${DIR}/LVC_Mixer_GetTarget.c
    ${DIR}/LVC_Mixer_Init.c
    ${DIR}/LVC_Core_MixHard_1St_2i_D16C31_SAT.c
    ${DIR}/LVC_Core_MixSoft_1St_2i_D16C31_WRA.c
    ${DIR}/LVC_Core_MixInSoft_D16C31_SAT.c
    ${DIR}/LVC_Mixer_GetCurrent.c
    ${DIR}/LVC_MixSoft_2St_D16C31_SAT.c
    ${DIR}/LVC_Core_MixSoft_1St_D16C31_WRA.c
    ${DIR}/LVC_Core_MixHard_2St_D16C31_SAT.c
    ${DIR}/LVC_MixInSoft_D16C31_SAT.c
    ${DIR}/AGC_MIX_VOL_2St1Mon_D32_WRA.c
    ${DIR}/LVM_Timer.c
    ${DIR}/LVM_Timer_Init.c
)

set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_DEFINITIONS "${MODULE_C_DEFINES}")
set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES COMPILE_OPTIONS "${MODULE_C_OPTIONS}")
set_source_files_properties(${CSRC} DIRECTORY ../../.. PROPERTIES INCLUDE_DIRECTORIES "${MODULE_IFLAGS}")
target_sources(audio_mixer_${PROJECT_NAME} PRIVATE ${CSRC})