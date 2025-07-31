CC=gcc

CFLAGS = -I. -I.. -I../.. -I../../.. -I../../../common

LDFLAGS = -lpthread

SRC_FILES = rtw_ctrl_api.c rtw_ctrl_cli.c

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
