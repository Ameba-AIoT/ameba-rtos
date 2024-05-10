
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/lwip/lwip_v2.1.2/src/core/ipv4/ip_nat
MODULE_IFLAGS += -I$(BASEDIR)/component/lwip/lwip_v2.1.2/src/core/ipv4/ip_nat
all:GLOBAL_CFLAGS += -DIP_FORWARD -DIP_NAT
TARGET_LIB = lib_ipnat

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =  $(DIR)/lwip_ip4nat.c \
		$(TARGET_LIB)_git_version.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
#all: CORE_TARGETS COPY_RAM_OBJS
$(CSRC): GEN_GIT_VERSION_FILE
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	$(AR) rvs lib_ipnat.a $(OBJS)
	$(FROMELF) -g -D lib_ipnat.a
	$(MOVE) -f lib_ipnat.a $(ROOTDIR)/lib/application
	$(REMOVE) $(TARGET_LIB)_git_version.*
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

#*****************************************************************************#
#              GENERATE GIT VERSION FILE                                      #
#*****************************************************************************#
GEN_GIT_VERSION_FILE:
	$(call GEN_GIT_VER, $(TARGET_LIB))

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
	
-include $(DEPS)
