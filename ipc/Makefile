SUBDIRS = pipe queue sema shm

MAKE    = make
all:
	@for i in $(SUBDIRS); do (cd ./$$i && echo $$i: && $(MAKE) $@) done || exit 1

install:
	@for i in $(SUBDIRS); do (cd ./$$i && echo $$i: && $(MAKE) $@) done || exit 1

clean:
	@for i in $(SUBDIRS); do (cd ./$$i && echo $$i: && $(MAKE) $@) done || exit 1

