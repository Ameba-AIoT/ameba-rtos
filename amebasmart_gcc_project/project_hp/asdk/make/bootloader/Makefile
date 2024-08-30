
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/bootloader
CFGDIR = $(TARGETDIR)/usrcfg
DRVDIR = $(TARGETDIR)/fwlib/ram_hp
DRVCOM = $(TARGETDIR)/fwlib/ram_common
LOGDIR = $(TARGETDIR)/swlib
LZMA_DIR = $(TARGETDIR)/../common/lzma/

MODULE_IFLAGS += -I$(LOGDIR)
MODULE_IFLAGS += -I$(FREERTOSDIR)/include/
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/non_secure
MODULE_IFLAGS += -I$(FREERTOSDIR)/portable/GCC/AmebaSmart_HP/secure
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/common/crashdump/include
MODULE_IFLAGS += -I$(LZMA_DIR)

vpath %.c $(DIR) $(CFGDIR) $(DRVDIR) $(DRVCOM) $(LOGDIR) $(LZMA_DIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = \
	$(DIR)/bootloader_hp.c \
	$(DIR)/boot_trustzone_hp.c \
	$(DIR)/boot_security_hp.c \
	$(DIR)/boot_ota_hp.c \
	$(CFGDIR)/amebahp_boot_trustzonecfg.c \
	$(CFGDIR)/ameba_bootcfg.c \
	$(CFGDIR)/ameba_flashcfg.c \
	$(DRVDIR)/ameba_flashclk.c \
	$(DRVDIR)/ameba_nandflash.c \
	$(DRVDIR)/ameba_ddrphy.c \
	$(DRVDIR)/ameba_rxi316_dram_init.c\
	$(DRVDIR)/ameba_psram.c \
	$(DRVCOM)/ameba_clk.c \
	$(DRVCOM)/ameba_swr.c \
	$(DRVCOM)/ameba_otp.c \
	$(DRVCOM)/ameba_swr_calib.c \
	$(DRVCOM)/ameba_pll.c \
	$(DRVCOM)/ameba_reset.c \
	$(DRVCOM)/ameba_ipc_api.c \
	$(DRVCOM)/ameba_fault_handle.c \

CSRC += $(LOGDIR)/log.c
#CSRC += $(DRVDIR)/ameba_ddrphy_scanpara.c

ifeq ($(CONFIG_COMPRESS_OTA_IMG), y)
CSRC += $(LZMA_DIR)/ameba_boot_lzma.c \
 		$(LZMA_DIR)/LzmaDec.c
endif
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS
RENAME_LOADER_OBJS: CORE_TARGETS
COPY_RAM_OBJS: RENAME_LOADER_OBJS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
