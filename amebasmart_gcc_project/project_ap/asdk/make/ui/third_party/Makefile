include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                                  #
#*****************************************************************************#

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
lib:
ifeq ($(CONFIG_GUI_LIB_PNG),y)
	make -C libz lib
	make -C libpng lib
endif
ifeq ($(CONFIG_GUI_LIB_JPG),y)
	make -C libjpeg lib
endif

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C libz clean
	make -C libpng clean
	make -C libjpeg clean
-include $(DEPS)
