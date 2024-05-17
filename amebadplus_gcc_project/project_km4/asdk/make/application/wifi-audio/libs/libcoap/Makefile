include $(MAKE_INCLUDE_GEN)
include ../config.mk

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/coap/libcoap

vpath %.c $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += \
    -Wno-format \
    -Wno-undef \
    -Wno-unused-variable \
    -Wno-unused-parameter \
    -Wno-implicit-fallthrough \
    -Wno-type-limits

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
MODULE_IFLAGS += \
    -I$(BASEDIR)/component/application/wifi-audio/coap/libcoap \
    -I$(BASEDIR)/component/application/wifi-audio/coap/libcoap/include

CSRC +=	\
    $(DIR)/src/address.c \
    $(DIR)/src/async.c \
    $(DIR)/src/block.c \
    $(DIR)/src/coap_asn1.c \
    $(DIR)/src/coap_cache.c \
    $(DIR)/src/coap_debug.c \
    $(DIR)/src/coap_event.c \
    $(DIR)/src/coap_hashkey.c \
    $(DIR)/src/coap_gnutls.c \
    $(DIR)/src/coap_io.c \
    $(DIR)/src/coap_mbedtls.c \
    $(DIR)/src/coap_notls.c \
    $(DIR)/src/coap_openssl.c \
    $(DIR)/src/coap_prng.c \
    $(DIR)/src/coap_session.c \
    $(DIR)/src/coap_tcp.c \
    $(DIR)/src/coap_time.c \
    $(DIR)/src/coap_tinydtls.c \
    $(DIR)/src/encode.c \
    $(DIR)/src/mem.c \
    $(DIR)/src/net.c \
    $(DIR)/src/option.c \
    $(DIR)/src/pdu.c \
    $(DIR)/src/resource.c \
    $(DIR)/src/str.c \
    $(DIR)/src/subscribe.c \
    $(DIR)/src/uri.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
lib: COPY_RAM_OBJS
	cp *.o  ../../xip

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
