target=rtw_aware
INCLUDES+=-I/usr/include
INCLUDES+=-I./
LIBS+=-L./ -lnanapi -lglib-2.0 -lgio-2.0 -ljson-c -lrt
SRC=rtw_aware.c

$(target): $(SRC)
	$(CC) -o $@ $^ $(INCLUDES) $(LIBS)
####################################################################
all:  $(target)
clean:
	rm -rf $(target)
