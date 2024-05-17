
include $(MAKE_INCLUDE_GEN)

GLOBAL_CFLAGS += -Wno-address-of-packed-member


MODULE_IFLAGS               += -I$(BASEDIR)/component/range/include \
			-I$(BASEDIR)/component/range/dw3000 \
			-I$(BASEDIR)/component/range/mcps802154 \
			-I$(BASEDIR)/component/range/port \
			-I$(BASEDIR)/component/range/mcps802154/ccc \
			-I$(BASEDIR)/component/range/mcps802154/fira \
			-I$(BASEDIR)/component/range/mcps802154/pctt \
			-I$(BASEDIR)/component/range/mcps802154/nfcc \

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/range

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#


CSRC +=	\
	dw3000_pctt_mcps.o \
	dw3000_calib.o \
	dw3000_chip_c0.o \
	dw3000_chip_d0.o \
	dw3000_chip_e0.o \
	dw3000_core.o \
	dw3000_mcps.o \
	dw3000_spi.o \
	dw3000_stm.o \
	dw3000_trc.o \
	dw3000_txpower_adjustment.o \
	dw3000_nfcc_coex_core.o \
	dw3000_nfcc_coex_buffer.o \
	dw3000_nfcc_coex_msg.o \
	dw3000_nfcc_coex_mcps.o

CSRC += ca.o \
	default_region.o \
	endless_scheduler.o \
	on_demand_scheduler.o \
	fproc.o \
	fproc_broken.o \
	fproc_multi.o \
	fproc_vendor.o \
	fproc_nothing.o \
	fproc_rx.o \
	fproc_stopped.o \
	fproc_tx.o \
	frame.o \
	ie.o \
	mcps_main.o \
	ops.o \
	regions.o \
	schedule.o \
	schedulers.o
	
CSRC += fira_access.o \
	fira_aead.o \
	fira_cmac.o \
	fira_crypto.o \
	fira_round_hopping_sequence.o \
	fira_round_hopping_crypto.o \
	fira_frame.o \
	fira_region.o \
	fira_session.o

CSRC += pctt_access.o \
	pctt_region.o \
	pctt_region_call.o \
	pctt_session.o

CSRC += spi.c \
	skbuff.c \
	range.c

CSRC += ccc_kdf.c \
	ccc_round_hopping_sequence.c \
	ccc_aead.c \
	ccc_frame.c \
	ccc_region.c \
	ccc_session.c \
	ccc_access.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
