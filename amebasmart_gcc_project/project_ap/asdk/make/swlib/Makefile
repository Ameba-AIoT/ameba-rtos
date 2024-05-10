
include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(TARGETDIR)/swlib

vpath %.c $(DIR)
#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CSRC = $(DIR)/diag.c \
	$(DIR)/memset.c \
	$(DIR)/rand.c \
	$(DIR)/memcpy.c \
	$(DIR)/memcmp.c \
	$(DIR)/memchr.c \
	$(DIR)/memmove.c \
	$(DIR)/strcmp.c \
	$(DIR)/strlen.c \
	$(DIR)/stricmp.c \
	$(DIR)/strupr.c \
	$(DIR)/strtoul.c \
	$(DIR)/char2num.c \
	$(DIR)/strwrap.c \

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                           ROM Object FILE LIST                              #
#*****************************************************************************#
SWLIB_ROM_OBJS = diag.o \
	memcmp.o \
	memcpy.o \
	memset.o \
	rand.o \
	memchr.o \
	memmove.o \
	strcmp.o \
	strlen.o \
	stricmp.o \
	strupr.o \
	strtoul.o \
	strwrap.o \
#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
all: CORE_TARGETS COPY_RAM_OBJS
	
#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS:	$(OBJS)



#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
