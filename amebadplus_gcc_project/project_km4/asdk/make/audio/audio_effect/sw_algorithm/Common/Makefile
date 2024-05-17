include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_effect/sw_algorithm/Common/include
.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/audio_effect/sw_algorithm/Common/src

vpath %.c $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__
GLOBAL_CFLAGS += -DNDEBUG -UDEBUG -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/InstAlloc.c
CSRC += $(DIR)/DC_2I_D16_TRC_WRA_01.c
CSRC += $(DIR)/DC_2I_D16_TRC_WRA_01_Init.c
CSRC += $(DIR)/FO_2I_D16F32C15_LShx_TRC_WRA_01.c
CSRC += $(DIR)/FO_2I_D16F32Css_LShx_TRC_WRA_01_Init.c
CSRC += $(DIR)/FO_1I_D16F16C15_TRC_WRA_01.c
CSRC += $(DIR)/FO_1I_D16F16Css_TRC_WRA_01_Init.c
CSRC += $(DIR)/BP_1I_D16F32C30_TRC_WRA_01.c
CSRC += $(DIR)/BP_1I_D16F16C14_TRC_WRA_01.c
CSRC += $(DIR)/BP_1I_D32F32C30_TRC_WRA_02.c
CSRC += $(DIR)/BP_1I_D16F16Css_TRC_WRA_01_Init.c
CSRC += $(DIR)/BP_1I_D16F32Cll_TRC_WRA_01_Init.c
CSRC += $(DIR)/BP_1I_D32F32Cll_TRC_WRA_02_Init.c
CSRC += $(DIR)/BQ_2I_D32F32Cll_TRC_WRA_01_Init.c
CSRC += $(DIR)/BQ_2I_D32F32C30_TRC_WRA_01.c
CSRC += $(DIR)/BQ_2I_D16F32C15_TRC_WRA_01.c
CSRC += $(DIR)/BQ_2I_D16F32C14_TRC_WRA_01.c
CSRC += $(DIR)/BQ_2I_D16F32C13_TRC_WRA_01.c
CSRC += $(DIR)/BQ_2I_D16F32Css_TRC_WRA_01_init.c
CSRC += $(DIR)/BQ_2I_D16F16C15_TRC_WRA_01.c
CSRC += $(DIR)/BQ_2I_D16F16C14_TRC_WRA_01.c
CSRC += $(DIR)/BQ_2I_D16F16Css_TRC_WRA_01_Init.c
CSRC += $(DIR)/BQ_1I_D16F16C15_TRC_WRA_01.c
CSRC += $(DIR)/BQ_1I_D16F16Css_TRC_WRA_01_Init.c
CSRC += $(DIR)/BQ_1I_D16F32C14_TRC_WRA_01.c
CSRC += $(DIR)/BQ_1I_D16F32Css_TRC_WRA_01_init.c
CSRC += $(DIR)/PK_2I_D32F32C30G11_TRC_WRA_01.c
CSRC += $(DIR)/PK_2I_D32F32C14G11_TRC_WRA_01.c
CSRC += $(DIR)/PK_2I_D32F32CssGss_TRC_WRA_01_Init.c
CSRC += $(DIR)/PK_2I_D32F32CllGss_TRC_WRA_01_Init.c
CSRC += $(DIR)/Int16LShiftToInt32_16x32.c
CSRC += $(DIR)/From2iToMono_16.c
CSRC += $(DIR)/Copy_16.c
CSRC += $(DIR)/MonoTo2I_16.c
CSRC += $(DIR)/LoadConst_16.c
CSRC += $(DIR)/dB_to_Lin32.c
CSRC += $(DIR)/Shift_Sat_v16xv16.c
CSRC += $(DIR)/Abs_32.c
CSRC += $(DIR)/Int32RShiftToInt16_Sat_32x16.c
CSRC += $(DIR)/From2iToMono_32.c
CSRC += $(DIR)/mult3s_16x16.c
CSRC += $(DIR)/NonLinComp_D16.c
CSRC += $(DIR)/DelayMix_16x16.c
CSRC += $(DIR)/MSTo2i_Sat_16x16.c
CSRC += $(DIR)/From2iToMS_16x16.c
CSRC += $(DIR)/Mac3s_Sat_16x16.c
CSRC += $(DIR)/Add2_Sat_16x16.c
CSRC += $(DIR)/LVC_MixSoft_1St_2i_D16C31_SAT.c
CSRC += $(DIR)/LVC_MixSoft_1St_D16C31_SAT.c
CSRC += $(DIR)/LVC_Mixer_VarSlope_SetTimeConstant.c
CSRC += $(DIR)/LVC_Mixer_SetTimeConstant.c
CSRC += $(DIR)/LVC_Mixer_SetTarget.c
CSRC += $(DIR)/LVC_Mixer_GetTarget.c
CSRC += $(DIR)/LVC_Mixer_Init.c
CSRC += $(DIR)/LVC_Core_MixHard_1St_2i_D16C31_SAT.c
CSRC += $(DIR)/LVC_Core_MixSoft_1St_2i_D16C31_WRA.c
CSRC += $(DIR)/LVC_Core_MixInSoft_D16C31_SAT.c
CSRC += $(DIR)/LVC_Mixer_GetCurrent.c
CSRC += $(DIR)/LVC_MixSoft_2St_D16C31_SAT.c
CSRC += $(DIR)/LVC_Core_MixSoft_1St_D16C31_WRA.c
CSRC += $(DIR)/LVC_Core_MixHard_2St_D16C31_SAT.c
CSRC += $(DIR)/LVC_MixInSoft_D16C31_SAT.c
CSRC += $(DIR)/AGC_MIX_VOL_2St1Mon_D32_WRA.c
CSRC += $(DIR)/LVM_Timer.c
CSRC += $(DIR)/LVM_Timer_Init.c
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Dependency	                                  #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS

lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../../../audio_xip/local/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
