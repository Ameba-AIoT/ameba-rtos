include $(MAKE_INCLUDE_GEN)

.PHONY: all lib wfa clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets

all:lib

lib:
	make -C haac all
	make -C fdk-aac all
	make -C flac all
	make -C tremolo all
	make -C opus all

wfa:
	make -C flac all

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C haac clean
	make -C fdk-aac clean
	make -C flac clean
	make -C tremolo clean
	make -C opus clean

-include $(DEPS)
