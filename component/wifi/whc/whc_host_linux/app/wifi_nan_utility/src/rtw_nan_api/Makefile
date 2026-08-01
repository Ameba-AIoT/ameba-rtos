APP_PATH=../../rtw_aware
LIB_NAME=libnanapi.so
EXT_HDR=rtw_nan_struct.h rtw_nan_cmd_api.h rtw_nan_scenario_api.h

all:
	@gcc -c -fpic *.c
	@gcc -shared -o $(LIB_NAME) *.o
	@rm -rf *.o
	cp $(LIB_NAME) $(APP_PATH)
	cp $(EXT_HDR) $(APP_PATH)
	@echo "libnanapi success!"
clean:
	rm -rf *.o $(LIB_NAME)
