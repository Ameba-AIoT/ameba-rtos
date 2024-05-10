
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/fwlib/ram_km4
DIR_COM = $(TARGETDIR)/fwlib/ram_common
LOGDIR = $(TARGETDIR)/swlib

MODULE_IFLAGS = -I$(DIR)/../include/
MODULE_IFLAGS += -I$(LOGDIR)
vpath %.c $(DIR) $(DIR_COM) $(LOGDIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =	$(DIR)/ameba_trustzone.c \
		$(DIR_COM)/ameba_gdma_ram.c \
		$(DIR)/ameba_codec.c \
		$(DIR)/ameba_sport.c \
		$(DIR_COM)/ameba_ledc.c \
		$(DIR_COM)/ameba_uart.c \
		$(DIR_COM)/ameba_spi.c \
		$(DIR_COM)/ameba_i2c.c \
		$(DIR_COM)/ameba_tim.c \
		$(DIR_COM)/ameba_rtc.c \
		$(DIR_COM)/ameba_adc.c \
		$(DIR_COM)/ameba_comparator.c \
		$(DIR_COM)/ameba_keyscan.c \
		$(LOGDIR)/log.c
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
