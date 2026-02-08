
include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#COMPONENT := lwip
#COMPONENT += httpc
#COMPONENT += httpd
#COMPONENT += websocket

#COMPONENT_CLEAN := lwip
#COMPONENT_CLEAN += httpc
#COMPONENT_CLEAN += httpd
#COMPONENT_CLEAN += websocket


#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

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
	make -C lwip all
	make -C mqtt all
	make -C http2 all
	make -C httpc all
	make -C httpd all
	make -C websocket all
	#make -C eap all
	#make -C libcoap all
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
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d
	make -C lwip clean
	make -C httpc clean
	make -C httpd clean
	make -C websocket clean
	make -C mqtt clean
	make -C tftp clean
	make -C http2 clean
	make -C eap clean
	make -C libcoap clean

-include $(DEPS)
