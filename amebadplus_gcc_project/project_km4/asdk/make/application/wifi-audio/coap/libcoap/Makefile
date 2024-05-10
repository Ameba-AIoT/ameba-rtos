
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean
TARGET_LIB = lib_coap

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/application/wifi-audio/coap/libcoap

MODULE_IFLAGS += \
	-I$(BASEDIR)/component/application/wifi-audio/coap/libcoap \
	-I$(BASEDIR)/component/application/wifi-audio/coap/libcoap/include

vpath %.c $(DIR) $(shell find $(DIR) -type d)

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
STATIC_LIB = lib_wfa_coap.a

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS) $(STATIC_LIB)
$(STATIC_LIB):$(OBJS)
	$(AR) rvs $@ $^
	$(FROMELF) -g -D $@
	$(MOVE) -f $@ $(ROOTDIR)/lib/application

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
