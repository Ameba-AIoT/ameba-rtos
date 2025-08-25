CC=gcc

CFLAGS = -I. -I.. -I../.. -I../common

LDFLAGS = -lpthread

SRC_FILES = whc_host_app_api.c whc_host_app.c

OBJ_FILES = $(SRC_FILES:.c=.o)

TARGET = whc_ctrl_app

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

../%.o: ../%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(TARGET) *.o
