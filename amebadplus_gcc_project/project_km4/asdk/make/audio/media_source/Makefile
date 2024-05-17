include $(MAKE_INCLUDE_GEN)

.PHONY: lib clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR = $(BASEDIR)/component/audio/media_source

MODULE_IFLAGS += -I$(BASEDIR)/component/ssl/mbedtls-2.16.9/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/libs/src/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/log/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_c/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/base/osal/osal_cxx/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/interfaces
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/media_utils/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/media_common/include
MODULE_IFLAGS += -I$(BASEDIR)/component/audio/configs/include
MODULE_IFLAGS += -I$(DIR)/include

vpath %.cpp $(shell find $(DIR) -type d)
#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#
GLOBAL_CFLAGS += -D__RTOS__ -Wno-multichar -Wno-unused-parameter  -DCONFIG_MEDIA_DATASOURCE_BUFFER -DCONFIG_MEDIA_DATASOURCE_HTTP -Werror

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
CPPSRC  = $(DIR)/src/data_source_base.cpp
CPPSRC += $(DIR)/src/data_source_factory.cpp
CPPSRC += $(DIR)/src/file_source.cpp
CPPSRC += $(DIR)/src/buffer_source.cpp
CPPSRC += $(DIR)/src/cdata_source.cpp
CPPSRC += $(DIR)/src/http/http_source.cpp
CPPSRC += $(DIR)/src/http/http_tls.cpp
#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CPPSRC:.cpp=.o))

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#

# Define the Rules to build the core targets
COPY_RAM_OBJS: CORE_TARGETS
lib: COPY_RAM_OBJS
	cp *.o  ../media_xip

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
