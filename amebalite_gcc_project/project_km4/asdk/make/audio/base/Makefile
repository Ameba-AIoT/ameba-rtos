
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
lib: CORE_TARGETS
	make -C libs lib
	make -C log lib
	make -C osal lib
	make -C cutils lib
	make -C audio_utils lib
	make -C xip lib

all: CORE_TARGETS
	make -C cutils all
#*****************************************************************************#
#              GENERATE OBJECT FILE                                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)




#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C libs clean
	make -C log clean
	make -C osal clean
	make -C cutils clean
	make -C audio_utils clean
	make -C xip clean
-include $(DEPS)
