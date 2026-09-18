# TSF sync - application side library (kernel side: ../../common/tsf_sync/)

CC      ?= gcc
CFLAGS  ?= -O2 -Wall -Wextra
LDLIBS  += -lrt -lpthread

# Not in CFLAGS: CFLAGS uses ?= and would be wiped by `make CFLAGS=...`
LATCH_DIR := ../../common/tsf_sync
TSF_INC   := -I$(LATCH_DIR)

all: tsf_time.o

tsf_time.o: tsf_time.c tsf_time.h $(LATCH_DIR)/tsf_latch.h
	$(CC) $(CFLAGS) $(TSF_INC) -c -o $@ tsf_time.c

clean:
	rm -f tsf_time.o

.PHONY: all clean
