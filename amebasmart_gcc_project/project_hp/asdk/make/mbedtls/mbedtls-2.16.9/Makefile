
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ssl/mbedtls-2.16.9

MODULE_IFLAGS += -DCONFIG_BUILD_NONSECURE=1

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifneq ($(CONFIG_MP_INCLUDED), y)
ifeq ($(CONFIG_WLAN),y)
# AP
ifeq ($(CONFIG_SINGLE_CORE_WIFI), y)
CSRC = $(DIR)/library/net_sockets.c
endif
endif
endif

CSRC += $(DIR)/library/aes.c			\
       $(DIR)/library/aesni.c			\
       $(DIR)/library/blowfish.c		\
       $(DIR)/library/camellia.c		\
       $(DIR)/library/ccm.c			\
	   $(DIR)/library/certs.c			\
       $(DIR)/library/cipher.c			\
       $(DIR)/library/cipher_wrap.c		\
       $(DIR)/library/cmac.c			\
       $(DIR)/library/debug.c			\
       $(DIR)/library/error.c			\
	   $(DIR)/library/entropy.c			\
       $(DIR)/library/gcm.c			\
       $(DIR)/library/havege.c			\
       $(DIR)/library/memory_buffer_alloc.c	\
       $(DIR)/library/padlock.c		\
       $(DIR)/library/pkcs5.c			\
       $(DIR)/library/pkcs11.c			\
       $(DIR)/library/pkcs12.c			\
       $(DIR)/library/pkparse.c		\
       $(DIR)/library/platform.c		\
       $(DIR)/library/ssl_cache.c		\
       $(DIR)/library/ssl_cli.c		\
       $(DIR)/library/ssl_srv.c   		\
       $(DIR)/library/ssl_ciphersuites.c	\
       $(DIR)/library/ssl_ticket.c		\
       $(DIR)/library/ssl_tls.c		\
       $(DIR)/library/threading.c		\
       $(DIR)/library/timing.c			\
       $(DIR)/library/version.c		\
       $(DIR)/library/version_features.c	\
       $(DIR)/library/x509.c			\
       $(DIR)/library/x509_create.c		\
       $(DIR)/library/x509_crl.c		\
       $(DIR)/library/x509_crt.c		\
       $(DIR)/library/x509_csr.c		\
       $(DIR)/library/x509write_crt.c		\
       $(DIR)/library/x509write_csr.c		\
       $(DIR)/library/rsa.c	\
       $(DIR)/library/arc4.c \
       $(DIR)/library/ctr_drbg.c	\
       $(DIR)/library/rsa_internal.c	\
       $(DIR)/library/ctr_drbg.c	\

ifeq ($(CONFIG_RLE1080_A_CUT),y)
CSRC += $(DIR)/library/platform_util.c
endif


CSRC +=   $(DIR)/library/dhm.c	\
	$(DIR)/library/hmac_drbg.c	\
	$(DIR)/library/pem.c	\
	$(DIR)/library/pk.c		\
	$(DIR)/library/pk_wrap.c	\
	$(DIR)/library/pkwrite.c	\
	$(DIR)/library/rsa.c	\
	$(DIR)/library/oid.c \
	$(DIR)/library/base64.c \
	$(DIR)/library/ecjpake.c \
	$(DIR)/library/chacha20.c	\
	$(DIR)/library/chachapoly.c	\
	$(DIR)/library/poly1305.c	\
	$(DIR)/library/ssl_cookie.c	\
	#$(DIR)/library/ecdh.c	\
	#$(DIR)/library/ecdsa.c	\

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
# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

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
