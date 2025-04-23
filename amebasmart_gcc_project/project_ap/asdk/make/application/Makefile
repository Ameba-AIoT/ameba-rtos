include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = 


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
ifeq ($(CONFIG_CLINTWOOD), y)
	make -C clintwood all
endif
	make -C demoui all
ifeq ($(CONFIG_WIFI_AUDIO),y)
	make -C wifi-audio all
	#make -C wifi-audio test
endif
ifeq ($(CONFIG_MATTER_EN), y)
	@make -C $(MATTER_BUILDDIR)/make all
endif

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C clintwood clean
	make -C demoui clean
	make -C wifi-audio clean
ifeq ($(CONFIG_MATTER_EN), y)
	@make -C $(MATTER_BUILDDIR)/make clean
endif

-include $(DEPS)
