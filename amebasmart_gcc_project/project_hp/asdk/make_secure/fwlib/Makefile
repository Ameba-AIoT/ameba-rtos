
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ram_hp
DIR_COM = $(TARGETDIR)/fwlib/ram_common
LOGDIR = $(TARGETDIR)/swlib

MODULE_IFLAGS = -I$(DIR)/../include/
MODULE_IFLAGS += -I$(LOGDIR)

vpath %.c $(DIR) $(DIR_COM) $(LOGDIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =	$(DIR)/ameba_trustzone.c \
	$(DIR)/ameba_codec.c \
	$(DIR)/ameba_sport.c \
	$(DIR)/ameba_vad.c \
	$(DIR)/ameba_vad_bt.c \
	$(DIR_COM)/ameba_tim.c \
	$(DIR_COM)/ameba_otp.c \
	$(DIR_COM)/ameba_uart.c \
	$(DIR_COM)/ameba_spi.c \
	$(DIR_COM)/ameba_i2c.c \
	$(DIR_COM)/ameba_gdma_ram.c \
	$(DIR_COM)/ameba_ir.c \
	$(DIR_COM)/ameba_ledc.c \

CSRC += $(LOGDIR)/log.c \

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
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
