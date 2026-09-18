#
# Makefile
# All right reserved by Sagittarius(Young_Jin Yoon).
# 1996/03/19-14:23:14
#

#include makefile.def

LD_HOME    = $(HOME)
CC         = cc
AR         = ar
PROC       =
SUBDIRS    =
LIBPATH    = $(LD_HOME)/lib
INCLUDE    = $(LD_HOME)/include
CFLAGS     = -g -c -I$(INCLUDE)
ARFLAGS    = ruv
PROCFLAGS  =

.SUFFIXES:.c .o .pc
.c.o:
	$(CC) $(CFLAGS) $*.c
.c.pc:
	$(PROC) $(PROCFLAGS) iname=$*.pc include=$(INCLUDE)

LIBOBJS    =
LIBNAME    =

MOTIFLIBS  = -lXm -lXt -lX11
LD_LIBS    = -L$(LIBPATH) -lsaipc

EXECS      = msgqsvr msgqcli

all::$(EXECS)
# User define
#
# sample : sample.o
# 	$(CC) -o $@ $@.o  $(LD_LIBS)
#

msgqsvr : msgqsvr.o
	$(CC) -o $@ $@.o  $(LD_LIBS)

msgqcli : msgqcli.o
	$(CC) -o $@ $@.o  $(LD_LIBS)

clean :
	rm -f *.o core a.out Makefile.old

lib : $(LIBOBJS)
	$(AR) $(ARFLAGS) $(LIBPATH)/$(LIBNAME) $(LIBOBSJS)

compress :
	rm -f  *.o core Makefile.old $(EXECS)
# Pro-C dependency
