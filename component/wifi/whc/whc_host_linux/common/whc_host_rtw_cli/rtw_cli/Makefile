CC=gcc

PKG_CONFIG ?= pkg-config
NL_CFLAGS := $(shell $(PKG_CONFIG) --cflags libnl-3.0 libnl-genl-3.0)
NL_LDFLAGS := $(shell $(PKG_CONFIG) --libs libnl-3.0 libnl-genl-3.0)

CFLAGS = -I. -I.. -I../.. -I../../.. -I../../../common $(NL_CFLAGS)

LDFLAGS = -lpthread $(NL_LDFLAGS)

SRC_FILES = rtw_cli_api.c rtw_cli.c rtw_cli_netlink.c

OBJ_FILES = $(SRC_FILES:.c=.o)

TARGET = rtw_cli

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

../%.o: ../%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(TARGET) *.o
