
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/cmsis-dsp/Source
TARGET_LIB = lib_cmsis_dsp

vpath %.c $(DIR) $(shell find $(DIR) -type d)
vpath %.S $(DIR) $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/BasicMathFunctions/BasicMathFunctions.c \
	$(DIR)/BayesFunctions/BayesFunctions.c \
	$(DIR)/CommonTables/CommonTables.c \
	$(DIR)/ComplexMathFunctions/ComplexMathFunctions.c \
	$(DIR)/ControllerFunctions/ControllerFunctions.c \
	$(DIR)/DistanceFunctions/DistanceFunctions.c \
	$(DIR)/FastMathFunctions/FastMathFunctions.c \
	$(DIR)/FilteringFunctions/FilteringFunctions.c \
	$(DIR)/MatrixFunctions/MatrixFunctions.c \
	$(DIR)/StatisticsFunctions/StatisticsFunctions.c \
	$(DIR)/SupportFunctions/SupportFunctions.c \
	$(DIR)/SVMFunctions/SVMFunctions.c \
	$(DIR)/TransformFunctions/TransformFunctions.c \
	
ASMSRC = $(DIR)/TransformFunctions/arm_bitreversal2.S

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))
ASM_OBJS = $(notdir $(ASMSRC:.S=.o))
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS
	$(call GEN_AND_MOVE_TO_LIB_FOLDER, $(TARGET_LIB), $(TARGET_LIBAPP_FOLDER))
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS) $(ASM_OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(call CLEAN_FUNC)

-include $(DEPS)
