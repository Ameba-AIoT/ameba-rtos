include $(MAKE_INCLUDE_GEN)
include ../config.mk

.PHONY: all lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
WFA_DIR = $(BASEDIR)/component/application/wifi-audio
DIR = $(WFA_DIR)/pal

MODULE_IFLAGS += \
    -I$(DIR)/include

MODULE_IFLAGS += \
    -I$(WFA_DIR)/base/log/include \
    -I$(WFA_DIR)/base/cutils/include \
    -I$(WFA_DIR)/base/utils/include \
    -I$(WFA_DIR)/foundation/include

vpath %.c $(DIR) $(shell find $(DIR) -type d)
vpath %.cpp $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC += \
    $(DIR)/wifi_manager.c \
    $(DIR)/system_time.c

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))
COBJS = $(notdir $(CSRC:.c=.o))
OBJS = $(CPPOBJS)
OBJS += $(COBJS)
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
lib: COPY_RAM_OBJS
	$(COPY) $(OBJS) ../../xip/

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

%.o:%.cpp
	$(Q)$(CC) $(CPPFLAGS) $(MODULE_IFLAGS) $<  -o $@

%.o:%.c
	$(Q)$(CC) $(GLOBAL_CFLAGS) $(MODULE_IFLAGS) $<  -o $@

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean:
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
