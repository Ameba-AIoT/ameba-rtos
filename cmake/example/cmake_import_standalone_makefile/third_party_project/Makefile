# Makefile for foo

ifndef CROSS_COMPILE
    $(error error: CROSS_COMPILE is not defined. Please specify it by running 'make CROSS_COMPILE=<prefix>')
endif

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

ARFLAGS = rcs # r: replace, c: create, s: create index
RAW_CFLAGS = $(GLOBAL_MCU_COMPILE_C_OPTIONS)
CFLAGS := $(subst ;, ,$(RAW_CFLAGS)) -Iinclude

SRCS = $(wildcard *.c)
DEPS = $(wildcard include/*.h)
OBJS = $(SRCS:.c=.o)

TARGET = lib_foo_makefile.a

ifeq ($(OS), Windows_NT)
    RM = del /Q
else
    RM = rm -f
endif

.PHONY: all
all: $(TARGET)

$(TARGET): $(OBJS)
	@echo "--- archiving $(TARGET) ---"
	@$(AR) $(ARFLAGS) $@ $^

%.o: %.c $(DEPS)
	@echo "--- compiling $<---"
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "--- cleanup $(OBJS) $(TARGET) ---"
	-$(RM) $(OBJS) $(TARGET)