
all: bin/iw bin/nan_vendor_wrapper

script: bin/iw bin/nan_vendor_wrapper

aware: bin/iw bin/nan_vendor_wrapper bin/rtw_aware

bin/nan_vendor_wrapper:
	gcc src/vendor_wrapper/nan_vendor_wrapper.c -o /usr/bin/nan_vendor_wrapper

bin/iw:src/iw-5.4
	$(MAKE) -C src/iw-5.4/
	cp src/iw-5.4/iw /usr/bin/

bin/rtw_aware:src/rtw_nan_api
	$(MAKE) -C src/rtw_nan_api/
	$(MAKE) -C rtw_aware/

src/iw-5.4:src/iw-5.4.tar.gz
	tar -zxvf src/iw-5.4.tar.gz -C src/

.PHONY : clean
clean:
	-rm -rf bin/*
	-rm -rf src/iw-5.4
	#-$(MAKE) -C src/iw-5.4/ clean
	-$(MAKE) -C src/rtw_nan_api/ clean
	-$(MAKE) -C rtw_aware/ clean
