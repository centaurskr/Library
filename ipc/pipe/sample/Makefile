#
#
#
LD_HOME  = $(RUN_HOME)
INCPATH  = $(LD_HOME)/include
LIBPATH  = $(LD_HOME)/lib
.c.o:
	cc -g -c -I$(INCPATH) $*.c

all :: npipesvr npipecil npevent

npipesvr : npipesvr.o
	cc -o $@ $@.o -L$(LIBPATH) -lnhcommon

npipecil : npipecil.o
	cc -o $@ $@.o -L$(LIBPATH) -lnhcommon
npevent : npevent.o
	cc -o $@ $@.o -L$(LIBPATH) -lnhcommon
clean :
	rm -f *.o core npipesvr npipecil npevent TEST.PIPE
