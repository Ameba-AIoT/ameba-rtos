
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/ssl/mbedtls-2.16.9
DIR_RAMMAP = $(BASEDIR)/component/ssl/ssl_ram_map

MODULE_IFLAGS += -DCONFIG_BUILD_SECURE=1

vpath %.c $(DIR) $(shell find $(DIR) -type d) $(DIR_RAMMAP) $(shell find $(DIR_RAMMAP) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/library/aes.c			\
       $(DIR)/library/base64.c			\
       $(DIR)/library/oid.c			\
       $(DIR)/library/pem.c			\
       $(DIR)/library/pk.c			\
       $(DIR)/library/pk_wrap.c			\
       $(DIR)/library/platform.c		\
       $(DIR)/library/rsa.c			\
	   $(DIR)/library/rsa_internal.c			\
       $(DIR)/library/ssl_tls.c			\
       $(DIR_RAMMAP)/ssl_ram_map.c		\
       $(DIR)/library/pkparse.c

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

%.o:%.c
	$(CC) -ffunction-sections $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@
#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
