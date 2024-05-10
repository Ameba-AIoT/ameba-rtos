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
	make -C aidl all
	#make -C src all

lib: CORE_TARGETS
	make -C libs lib
	make -C xip lib

#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)




#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)
	make -C aidl clean
	make -C src clean
	make -C xip clean
	make -C libs clean

-include $(DEPS)
