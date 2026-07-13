
# the compiler: gcc for C program, define as g++ for C++
CC = g++

#ARM32
#CC=arm-linux-androideabi-g++
#ARM64
#CC=aarch64-linux-android-g++
#x86
#CC=i686-linux-android-g++
#x86_64
#CC=x86_64-linux-android-g++
#mips
#CC=mipsel-linux-android-g++
#mips64
#CC=mips64el-linux-android-g++


INCLUDE = .
RM = rm -f

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#  -fstack-protector-strong  stack canary protection against buffer overflows
#  -D_FORTIFY_SOURCE=2       runtime bounds checking on sprintf/memcpy/strcpy etc.
CFLAGS  = -g -Wall -static -fstack-protector-strong -D_FORTIFY_SOURCE=2 #-fno-exceptions

# NL_DEFAULT: build rtwpriv with Netlink mode enabled by default.
#   make NL_DEFAULT=y
# When set to y/1, the built rtwpriv routes commands through Generic Netlink
# automatically, so "-n" can be omitted:
#   rtwpriv wlan0 <command> [args...]   (same as: rtwpriv -n wlan0 <command>)
# When unset (default), behaviour is unchanged and "-n" is still required to
# explicitly select Netlink mode.
NL_DEFAULT ?= y
ifneq ($(filter y Y 1,$(NL_DEFAULT)),)
CFLAGS += -DRTW_DEFAULT_NETLINK
endif

TARGET = rtwpriv
RTW_API_OBJS = rtw_ioctl.o rtw_parse.o

ifeq ($(CC),arm-linux-androideabi-g++)
TARGET = rtwpriv_arm
endif
ifeq ($(CC),aarch64-linux-android-g++)
TARGET = rtwpriv_arm64
endif
ifeq ($(CC),i686-linux-android-g++)
TARGET = rtwpriv_x86
endif
ifeq ($(CC),x86_64-linux-android-g++)
TARGET = rtwpriv_x86_64
endif
ifeq ($(CC),mipsel-linux-android-g++)
TARGET = rtwpriv_mips
endif
ifeq ($(CC),mips64el-linux-android-g++)
TARGET = rtwpriv_mips64
endif

ifeq ($(CC),g++)
# the build target executable:
TARGET = rtwpriv
endif

all: $(TARGET)

$(TARGET): rtwpriv.o $(RTW_API_OBJS) rtw_hwtx.o rtw_udpsrv.o rtw_nl.o rtw_nl_cmd.o rtw_transport.o
		$(CC) $(CFLAGS) -o $(TARGET) rtwpriv.o $(RTW_API_OBJS) rtw_hwtx.o rtw_udpsrv.o rtw_nl.o rtw_nl_cmd.o rtw_transport.o -lm

rtwpriv.o: rtw_api.h rtwpriv.h rtw_nl_cmd.h rtw_transport.h rtw_hwtx.h
	$(CC) -I$(INCLUDE) $(CFLAGS) -c rtwpriv.cpp

rtw_ioctl.o: rtw_ioctl.h rtwpriv.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_ioctl.cpp

rtw_parse.o: rtw_parse.h rtwpriv.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_parse.cpp



rtw_udpsrv.o: rtw_udpsrv.h rtw_transport.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_udpsrv.cpp

rtw_nl.o: rtw_nl.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_nl.cpp

rtw_nl_cmd.o: rtw_nl_cmd.h rtw_nl.h rtwpriv.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_nl_cmd.cpp

rtw_transport.o: rtw_transport.h rtw_api.h rtw_nl.h rtwpriv.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_transport.cpp

rtw_hwtx.o: rtw_hwtx.h rtw_transport.h rtw_api.h rtwpriv.h
		$(CC) -I$(INCLUDE) $(CFLAGS) -c rtw_hwtx.cpp

clean:
		$(RM) $(TARGET) *.o $(RTW_API_OBJS) rtw_hwtx.o rtw_udpsrv.o rtw_nl.o rtw_nl_cmd.o rtw_transport.o
