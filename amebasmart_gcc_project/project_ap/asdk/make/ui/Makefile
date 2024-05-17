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
all: CORE_TARGETS COPY_RAM_OBJS
ifeq ($(CONFIG_GUI_EN),y)
ifeq ($(CONFIG_LVGL_EN),y)
	make -C LVGL all
endif
	make -C third_party all
endif
	#make -C test all

lib: CORE_TARGETS
	make -C drivers lib
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C LVGL clean
	make -C emWin clean
	make -C drivers clean
	make -C third_party clean
	#make -C test clean
-include $(DEPS)
