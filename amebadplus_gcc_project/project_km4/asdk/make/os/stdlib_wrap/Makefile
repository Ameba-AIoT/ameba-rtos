
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

DIR = $(BASEDIR)/component/soc/amebadplus/swlib
vpath %.c $(DIR)

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/gcc_wrap.c

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
CORE_TARGETS: $(OBJS)


%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@	
#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

	



