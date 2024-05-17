
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/network/dnrd/src
MODULE_IFLAGS += -I$(BASEDIR)/component/network/dnrd/src
TARGET_LIB = lib_ipdnrd

vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC =  $(DIR)/common.c \
		$(DIR)/dnrd.c \
		$(DIR)/dns.c \
		$(DIR)/lib.c \
		$(DIR)/master.c \
		$(DIR)/query.c \
		$(DIR)/relay.c \
		$(DIR)/udp.c \
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
	$(AR) rvs lib_ipdnrd.a $(OBJS)
	$(FROMELF) -g -D lib_ipdnrd.a
	$(MOVE) -f lib_ipdnrd.a $(ROOTDIR)/lib/application
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
