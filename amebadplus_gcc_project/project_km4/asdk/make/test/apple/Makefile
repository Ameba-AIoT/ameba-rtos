
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/verification/swlib/apple

MODULE_IFLAGS = -I$(BASEDIR)/component/application/apple/homekit/crypto/chacha \
	-I$(BASEDIR)/component/application/apple/homekit/crypto/ed25519 \
	-I$(BASEDIR)/component/application/apple/homekit/crypto/ed25519/core \
	-I$(BASEDIR)/component/application/apple/homekit/crypto/poly1305 \
	-I$(BASEDIR)/component/application/apple/homekit/crypto/sha512 \
	-I$(BASEDIR)/component/application/apple/WACServer/External/Curve25519 \
	-I$(BASEDIR)/component/application/apple/WACServer/External/GladmanAES \

vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =  \
       $(DIR)/curve25519-donna-test.c \
       $(DIR)/ed25519_test.c \
       $(DIR)/sha512_hkdf_test.c

#CSRC += $(DIR)/aes_test.c

CSRC += $(DIR)/chachatest.c \
       $(DIR)/poly1305test.c \

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
	$(call CLEAN_FUNC)

