include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/hal/src

vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += $(DIR)/flash_api.c \
		$(DIR)/pinmap_common.c \
		$(DIR)/sys_api.c \
		$(DIR)/us_ticker.c \
		$(DIR)/timer_api.c \
		$(DIR)/dma_api.c \
		$(DIR)/gpio_api.c \
		$(DIR)/gpio_irq_api.c \
		$(DIR)/pinmap.c \
		$(DIR)/port_api.c \
		$(DIR)/spi_api.c \
		$(DIR)/serial_api.c \
		$(DIR)/i2c_api.c \
		$(DIR)/pwmout_api.c \
		$(DIR)/analogin_api.c \
		$(DIR)/efuse_api.c \
		$(DIR)/wdt_api.c \
		$(DIR)/us_ticker_api.c \
		$(DIR)/wait_api.c \
		$(DIR)/rtc_api.c \
		$(DIR)/captouch_api.c \

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
CORE_TARGETS:	$(OBJS)


#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
