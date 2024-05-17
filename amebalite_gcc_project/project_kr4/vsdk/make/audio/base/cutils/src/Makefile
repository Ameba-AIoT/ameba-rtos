include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/cutils/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS               += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces

.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/base/cutils/src

vpath %.c $(shell find $(DIR)/ -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
CFLAGS += -D__RTOS__  -Werror -Wno-multichar

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/parcel.c
CSRC += $(DIR)/property.c
CSRC += $(DIR)/rtrbtree.c
CSRC += $(DIR)/rtintmap.c
CSRC += $(DIR)/rtlist.c

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

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

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
