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
ifeq ($(CONFIG_PNG_EN),y)
	make -C libz all
	make -C libpng all
endif
ifeq ($(CONFIG_JPEG_EN),y)
	make -C libjpeg all
endif
#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean:
	make -C libz clean
	make -C libpng clean
	make -C libjpeg clean
-include $(DEPS)
