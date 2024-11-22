include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES                                     #
#*****************************************************************************#
DIR = $(BASEDIR)/component/bluetooth

vpath %.c $(DIR)/driver/bt_inic \
		  $(DIR)/driver/hci

CSRC += $(DIR)/driver/hci/hci_if_inic.c

ifeq ($(CONFIG_BT_INIC_H4), y)
CSRC += $(DIR)/driver/bt_inic/bt_inic_h4.c
else ifeq ($(CONFIG_BT_INIC_H5), y)
CSRC += $(DIR)/driver/bt_inic/bt_inic_h5.c
else ifeq ($(CONFIG_BT_INIC_SPI), y)
CSRC += $(DIR)/driver/bt_inic/bt_inic_spi_dev.c
endif 

OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# LIBBLUETOOTH_OBJS_PATH = $(shell pwd)/objs
# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
	# mkdir -p $(LIBBLUETOOTH_OBJS_PATH)
	# for i in $(OBJS); do \
	# 	 $(COPY)  $$i $(LIBBLUETOOTH_OBJS_PATH) -f;\
	# 	done
	# $(REMOVE) *.i
	# $(REMOVE) *.s
	# $(REMOVE) *.d

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