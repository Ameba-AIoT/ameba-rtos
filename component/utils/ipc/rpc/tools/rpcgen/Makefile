-include ../../CommonConfig

RM = /bin/rm
GCC = /usr/bin/gcc


RPCGEN_DEFINES += -DDO_CODEGEN_FOR_ENABLE_FW2SYSTEM_RPC_HOOK_FUNCTION_IN_COMMON

all: linux
#	${GCC} $(DEFINES) -g -o rpcgen.linux rpc*.c

linux:
	make clean ;
	${GCC} -g $(RPCGEN_DEFINES) -o rpcgen rpc*.c
clean:
	${RM} -f sample_clnt.c
	${RM} -f sample.h
	${RM} -f sample_svc.c
	${RM} -f sample_xdr.c
	$(RM) -f rpcgen
	$(RM) -f core*
	$(RM) -f *~

cleanall: clean
	$(RM) -f rpcgen
