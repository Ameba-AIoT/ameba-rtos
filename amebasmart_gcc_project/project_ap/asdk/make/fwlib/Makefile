
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR_CORE = $(TARGETDIR)/fwlib/ap_core
DIR_PERI = $(TARGETDIR)/fwlib/ap_peripheral
DIR_COM = $(TARGETDIR)/fwlib/ram_common
CFGDIR = $(TARGETDIR)/usrcfg
DIR_HP = $(TARGETDIR)/fwlib/ram_hp
LOGDIR = $(TARGETDIR)/swlib

MODULE_IFLAGS = -I$(DIR)/../include/
MODULE_IFLAGS += -I$(LOGDIR)
MODULE_IFLAGS += -I$(DIR_CORE)

vpath %.c $(DIR_CORE) $(DIR_PERI) $(DIR_COM) $(CFGDIR) $(DIR_HP)
vpath %.c $(LOGDIR)
vpath %.S $(DIR_CORE) $(DIR_COM) $(CFGDIR)
vpath ameba_pmu.c $(TARGETDIR)/misc
vpath ameba_ota.c $(TARGETDIR)/misc

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =	$(DIR_CORE)/arm_gic.c \
	$(DIR_CORE)/cstub.c \
	$(DIR_CORE)/system_sheipa.c \
	$(DIR_CORE)/amebaap_vector.c \
	$(DIR_CORE)/ameba_app_start.c \
	$(DIR_CORE)/traps.c \
	$(DIR_PERI)/ameba_loguart.c \
	$(DIR_CORE)/psci.c \
	$(DIR_CORE)/smp.c \
	$(DIR_PERI)/ameba_pmc_ram.c \
	$(DIR_CORE)/ameba_delay.c \
	$(DIR_CORE)/ameba_pmc.c \

CSRC += $(TARGETDIR)/misc/ameba_pmu.c \

CSRC +=	$(DIR_COM)/ameba_ipc_api.c \
	$(DIR_COM)/ameba_ipc_ram.c \
	$(DIR_COM)/ameba_gdma_memcpy.c \
	$(DIR_COM)/ameba_gdma_ram.c \
	$(DIR_COM)/ameba_adc.c \
	$(DIR_COM)/ameba_captouch.c \
	$(DIR_COM)/ameba_comparator.c \
	$(DIR_COM)/ameba_i2c.c \
	$(DIR_COM)/ameba_ir.c \
	$(DIR_COM)/ameba_ledc.c \
	$(DIR_COM)/ameba_lcdc.c \
	$(DIR_COM)/ameba_mipi.c \
	$(DIR_COM)/ameba_rtc.c \
	$(DIR_COM)/ameba_spi.c \
	$(DIR_COM)/ameba_thermal.c \
	$(DIR_COM)/ameba_tim.c \
	$(DIR_COM)/ameba_uart.c \
	$(DIR_COM)/ameba_flash_ram.c \
	$(DIR_COM)/ameba_pll.c \
	$(DIR_COM)/ameba_otp.c \
	$(DIR_COM)/ameba_reset.c \
	$(DIR_COM)/ameba_aontimer.c \
	$(DIR_COM)/ameba_wakepin.c \
	$(DIR_COM)/ameba_bor.c \


CSRC +=	$(CFGDIR)/ameba_flashcfg.c \
		$(CFGDIR)/ameba_sleepcfg.c \

CSRC +=	$(DIR_HP)/ameba_sd.c \
	$(DIR_HP)/ameba_sdio_host.c \
	$(DIR_HP)/ameba_codec.c \
	$(DIR_HP)/ameba_sport.c \
	$(DIR_HP)/ameba_audio_clock.c \
	$(DIR_HP)/ameba_vad.c \
	$(DIR_HP)/ameba_vad_bt.c \
	$(DIR_HP)/ameba_system.c \

CSRC += $(LOGDIR)/log.c
CSRC += $(LOGDIR)/sscanf_minimal.c


ifeq ($(CONFIG_AS_INIC_AP),y)
CSRC += $(TARGETDIR)/misc/ameba_ota.c
endif

ASM_CSRC = $(DIR_CORE)/startup.S \
		$(DIR_CORE)/vectors_table.S \
		$(DIR_CORE)/smccc_call.S \
		$(DIR_CORE)/cache-v7.S \
		#$(DIR_CORE)/translation_table.S \


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
ASM_OBJS = $(notdir $(ASM_CSRC:.S=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS) $(ASM_OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	$(REMOVE) *.su

-include $(DEPS)
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/ARM_CA7/
