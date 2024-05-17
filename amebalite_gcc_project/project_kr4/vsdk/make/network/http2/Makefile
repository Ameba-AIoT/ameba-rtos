GLOBAL_CFLAGS += -Wno-implicit-fallthrough

include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/network/http2/nghttp2-1.31.0
MODULE_IFLAGS += -I$(BASEDIR)/component/network/http2/nghttp2-1.31.0/includes
TARGET_LIB = lib_http2
vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC +=	$(DIR)/nghttp2_buf.c \
		$(DIR)/nghttp2_callbacks.c \
		$(DIR)/nghttp2_debug.c \
		$(DIR)/nghttp2_frame.c \
		$(DIR)/nghttp2_hd.c \
		$(DIR)/nghttp2_hd_huffman.c \
		$(DIR)/nghttp2_hd_huffman_data.c \
		$(DIR)/nghttp2_helper.c \
		$(DIR)/nghttp2_http.c \
		$(DIR)/nghttp2_map.c \
		$(DIR)/nghttp2_mem.c \
		$(DIR)/nghttp2_option.c \
		$(DIR)/nghttp2_outbound_item.c \
		$(DIR)/nghttp2_pq.c \
		$(DIR)/nghttp2_priority_spec.c \
		$(DIR)/nghttp2_queue.c \
		$(DIR)/nghttp2_rcbuf.c \
		$(DIR)/nghttp2_session.c \
		$(DIR)/nghttp2_stream.c \
		$(DIR)/nghttp2_submit.c \
		$(DIR)/nghttp2_version.c \

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
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
