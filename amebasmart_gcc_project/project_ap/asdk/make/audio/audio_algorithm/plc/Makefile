include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                                  VARIABLES                                  #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/audio_algorithm/plc

vpath %.c $(shell find $(DIR)/ -type d)
vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Werror
CPPFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter -Wno-narrowing -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/vox_mem.c
CSRC += $(DIR)/vox_math.c
CPPSRC += $(DIR)/rt_plc.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
COBJS = $(notdir $(CSRC:.c=.o))
CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))

OBJS = $(COBJS)
OBJS += $(CPPOBJS)
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
	$(COPY) $(OBJS) ../../audio_xip/local/
	$(COPY) $(OBJS) ../../audio_xip/passthrough/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)

%.o:%.cpp
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@
%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
