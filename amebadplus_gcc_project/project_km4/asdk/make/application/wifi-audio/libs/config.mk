###########################################################
## Common settings for building wifi-audio
###########################################################

# a flag to diffientiate platform
GLOBAL_CFLAGS += -D__RTOS__

# fix conflict of freertos posix and GNU tool's posix
GLOBAL_CFLAGS += -D_POSIX_C_SOURCE=199309

# treat build warnings as error
GLOBAL_CFLAGS += -Werror

# add these default path for posix headers
MODULE_IFLAGS += \
    -I$(FREERTOSDIR)/include \
    -I$(FREERTOSDIR)/portable/GCC/AmebaDplus_KM4/non_secure \
    -I$(OSDIR)/freertos/freertos_posix/include \
    -I$(OSDIR)/freertos/freertos_posix/FreeRTOS-Plus-POSIX/include \
    -I$(OSDIR)/freertos/freertos_posix/FreeRTOS-Plus-POSIX/include/portable/realtek