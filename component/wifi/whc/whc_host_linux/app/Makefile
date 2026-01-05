CC=gcc

CFLAGS = -I. -I.. -I../.. -I../common -I../common/include

ccflags-y += -DCONFIG_FULLMAC
CFLAGS += $(ccflags-y)

LDFLAGS = -lpthread

SRC_FILES = whc_host_app_api.c whc_host_app.c

OBJ_FILES = $(SRC_FILES:.c=.o)

TARGET = whc_cust_path_demo

all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS)

../%.o: ../%.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
	
clean:
	rm -f $(TARGET) *.o
