target=rtw_aware
INCLUDES+=-I/usr/include
INCLUDES+=-I./
LIBS+=-L./ -lnanapi -lnanimpl -lglib-2.0 -lgio-2.0 -lbluetooth-internal -ljson-c -lrt
SRC=rtw_aware.c /usr/lib/libnanimpl.so /usr/include/stack_api.h

$(target): $(SRC)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)
####################################################################
all:  $(target)
clean:
	rm -rf $(target)
