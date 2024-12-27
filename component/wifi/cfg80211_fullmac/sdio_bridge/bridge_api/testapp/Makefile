CC=gcc

CFLAGS = -I. -I.. -I../..

LDFLAGS = -lpthread

SRC_FILES = ../rtw_sdio_bridge_api.c main.c

OBJ_FILES = $(SRC_FILES:.c=.o)

TARGET = bridge

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

../%.o: ../%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(TARGET) *.o
