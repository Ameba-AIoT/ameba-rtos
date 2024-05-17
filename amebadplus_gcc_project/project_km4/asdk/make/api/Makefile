
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/lwip/api

#COMPONENT := wifi
#ifneq ($(CONFIG_INIC_IPC), y)
#COMPONENT += network
#endif

#COMPONENT_CLEAN += network
#COMPONENT_CLEAN += wifi

vpath %.c $(DIR)

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
ifeq ($(CONFIG_AS_INIC_NP), y)
CSRC +=		$(DIR)/lwip_netconf.c
endif
ifeq ($(CONFIG_SINGLE_CORE_WIFI), y)
CSRC +=		$(DIR)/lwip_netconf.c
endif
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
#$(foreach n, $(COMPONENT), $(eval $(call GenerateTargets, $(n),all)))

#COPY_RAM_OBJS: CORE_TARGETS
#all: COPY_RAM_OBJS $(addsuffix -all, $(COMPONENT))
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
#$(foreach n, $(COMPONENT_CLEAN), $(eval $(call GenerateTargets,$(n),clean)))

#clean: CLEAN_OBJS $(addsuffix -clean, $(COMPONENT_CLEAN))
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
	make -C network clean

-include $(DEPS)

