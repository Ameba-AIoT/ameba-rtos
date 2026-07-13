ifeq ($(TARGET_ARCH),arm)

LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_SRC_FILES := rtwpriv.cpp rtw_ioctl.cpp rtw_parse.cpp rtw_hwtx.cpp rtw_udpsrv.cpp rtw_nl.cpp rtw_nl_cmd.cpp rtw_transport.cpp

LOCAL_CFLAGS := -DANDROID_CHANGES

# NL_DEFAULT: build rtwpriv with Netlink mode enabled by default.
#   make NL_DEFAULT=y
# When set to y/1, the built rtwpriv routes commands through Generic Netlink
# automatically, so "-n" can be omitted:
#   rtwpriv wlan0 <command> [args...]   (same as: rtwpriv -n wlan0 <command>)
# When unset (default), behaviour is unchanged and "-n" is still required to
# explicitly select Netlink mode.
NL_DEFAULT ?= n
ifneq ($(filter y Y 1,$(NL_DEFAULT)),)
LOCAL_CFLAGS += -DRTW_DEFAULT_NETLINK
endif

LOCAL_MODULE := rtwpriv

LOCAL_MODULE_TAGS := debug

LOCAL_CERTIFICATE := platform

include $(BUILD_EXECUTABLE)

endif
