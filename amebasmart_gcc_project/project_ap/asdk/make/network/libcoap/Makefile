
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS += -I$(BASEDIR)/component/network/libcoap/include

.PHONY: all clean
TARGET_LIB = lib_coap

#*****************************************************************************#
#                               VARIABLES                                     #
#*****************************************************************************#
DIR = $(BASEDIR)/component/network/libcoap

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC +=	$(DIR)/async.c \
		$(DIR)/block.c \
		$(DIR)/coap_debug.c \
		$(DIR)/coap_list.c \
		$(DIR)/coap_net.c \
		$(DIR)/encode.c \
		$(DIR)/hashkey.c \
		$(DIR)/option.c \
		$(DIR)/pdu.c \
		$(DIR)/resource.c \
		$(DIR)/str.c \
		$(DIR)/subscribe.c \
		$(DIR)/uri.c \


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(CSRC:.c=.o)

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CORE_TARGETS COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))

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
