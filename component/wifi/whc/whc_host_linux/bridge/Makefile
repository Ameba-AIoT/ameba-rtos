CC=gcc

CFLAGS = -I. -I.. -I../..

LDFLAGS = -lpthread

SRC_FILES = whc_bridge_host_api.c whc_bridge_host_demo.c

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
