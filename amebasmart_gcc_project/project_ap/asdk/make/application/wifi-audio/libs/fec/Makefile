include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/audio_algorithm
MODULE_IFLAGS               += -I$(BASEDIR)/component/application/wifi-audio/base/cutils/include
MODULE_IFLAGS               += -I$(BASEDIR)/component/application/wifi-audio/base/utils/include
.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/fec

MODULE_IFLAGS               += -I$(DIR)

vpath %.c $(shell find $(DIR)/ -type d)
vpath %.cc $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror
CPPFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing -Werror -std=c++14 -std=gnu++14

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC += $(DIR)/fec/fec_private_tables_bursty.cc
CPPSRC += $(DIR)/fec/fec_private_tables_random.cc
CPPSRC += $(DIR)/fec/flexfec_header_reader_writer.cc
CPPSRC += $(DIR)/fec/forward_error_correction.cc
CPPSRC += $(DIR)/fec/forward_error_correction_internal.cc
CPPSRC += $(DIR)/fec/ulpfec_header_reader_writer.cc
CPPSRC += $(DIR)/fec/copy_on_write_buffer.cc
CPPSRC += $(DIR)/fec/rtp_header_extension_map.cc
CPPSRC += $(DIR)/fec/flexfec_sender.cc
CPPSRC += $(DIR)/fec/flexfec_receiver.cc
CPPSRC += $(DIR)/fec/rtp_packet.cc
CPPSRC += $(DIR)/fec/rtp_packet_received.cc
CPPSRC += $(DIR)/fec/rtp_packet_to_send.cc
CPPSRC += $(DIR)/fec/ulpfec_receiver.cc
CPPSRC += $(DIR)/fec/ulpfec_generator.cc
CPPSRC += $(DIR)/fec/rtp_headers.cc
CPPSRC += $(DIR)/fec/zero_memory.cc
CPPSRC += $(DIR)/fec/random.cc
CPPSRC += $(DIR)/fec/checks.cc
CPPSRC += $(DIR)/fec/logging.cc

CPPSRC += $(DIR)/absl/base/internal/raw_logging.cc
CPPSRC += $(DIR)/absl/strings/internal/string_view.cc

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
COBJS = $(notdir $(CSRC:.c=.o))
CPPOBJS = $(notdir $(CPPSRC:.cc=.o))

OBJS = $(COBJS)
OBJS += $(CPPOBJS)
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
	$(COPY) $(OBJS) ../../xip/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.cc
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@
%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
