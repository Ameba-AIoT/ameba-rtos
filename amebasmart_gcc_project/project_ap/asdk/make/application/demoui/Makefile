
include $(MAKE_INCLUDE_GEN)

.PHONY: all lib clean


#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS
ifeq ($(CONFIG_DEMO_UI_EN),y)
	make -C app all
	#make -C entry all
endif

lib: CORE_TARGETS
#	make -C utils lib
#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)




#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C app clean
	make -C utils clean
	make -C entry clean
-include $(DEPS)
