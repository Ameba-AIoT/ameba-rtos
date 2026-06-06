CC := $(CROSS_COMPILE)gcc
O := $(shell pwd)

CFLAGS += -I. -I../../driver -I../../driver/include

ccflags-y += -DCONFIG_FULLMAC
CFLAGS += $(ccflags-y)

LDFLAGS += -lpthread

SRC_FILES := whc_host_app_api.c whc_host_app_ota.c  whc_host_app.c

OBJ_FILES := $(addprefix $(O)/, $(SRC_FILES:.c=.o))

TARGET := whc_cust_path_demo

all: $(O)/$(TARGET)

$(O)/$(TARGET): $(OBJ_FILES)
	$(CC) $(OBJ_FILES) -o $@ $(LDFLAGS) $(CFLAGS)

$(O)/%.o: %.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

clean:
	rm -fr $(O)/$(TARGET)
	rm -fr $(O)/*.o $(O)/*.a
