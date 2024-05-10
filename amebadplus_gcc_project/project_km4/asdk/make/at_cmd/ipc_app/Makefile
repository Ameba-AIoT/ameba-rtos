
include $(MAKE_INCLUDE_GEN)
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/app/ipc_app
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/app/ipc_app/include
MODULE_IFLAGS += -I$(BASEDIR)/component/soc/amebalite/app/ipc_app/src

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
IMQDIR = $(BASEDIR)/component/soc/amebalite/app/ipc_app/src

vpath %.c $(IMQDIR) $(shell find $(IMQDIR) -type d)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += $(IMQDIR)/imq/ipc_message_queue_api.c
CSRC += $(IMQDIR)/usrcfg/ipc_cores_cfg.c
CSRC += $(IMQDIR)/share_memory/share_memory_api.c

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

-include $(OBJS:.o=.d)
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: 
	$(call CLEAN_FUNC)

-include $(DEPS)
