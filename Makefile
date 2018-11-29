EXTRAS = Makefile LICENSE README.md

HEADERS = include/bitvector.h

SOURCES = src/bitvector.c

OBJECTS = $(SOURCES:src/%.c=obj/%.o)

BITVECTORLIB = bitvector
CC = gcc
DBG = -g -Wall -fstack-protector-all -pedantic
OPT = #-march=native -O3 -DNDEBUG -ffast-math -fomit-frame-pointer
INCLUDES = -Iinclude
LIBS = -l$(BITVECTORLIB)
LDFLAGS = -Llib
CFLAGS = -std=gnu99 $(DBG) $(OPT) $(INCLUDES)
AR = ar r
RANLIB = ranlib

all: depend lib/lib$(BITVECTORLIB).a

depend: .depend
.depend: $(SOURCES)
	@echo "Building dependencies" 
ifneq ($(wildcard ./.depend),)
	@rm -f "./.depend"
endif
	@$(CC) $(CFLAGS) -MM $^ > .depend
# Make .depend use the 'obj' directory
	@sed -i.bak -e :a -e '/\\$$/N; s/\\\n//; ta' .depend
	@sed -i.bak 's/^/obj\//' .depend
	@rm -f .depend.bak
-include .depend

$(OBJECTS): obj/%.o : src/%.c Makefile
	@echo "Compiling "$<""
	@[ -d obj ] || mkdir -p obj
	@$(CC) $(CFLAGS) -c $< -o $@

lib/lib$(BITVECTORLIB).a: $(OBJECTS) Makefile
	@echo "Creating "$@""
	@[ -d lib ] || mkdir -p lib
	@rm -f $@
	@$(AR) $@ $(OBJECTS)
	@$(RANLIB) $@

test/test: test/test.c lib/lib$(BITVECTORLIB).a
	$(CC) $(CFLAGS) $(LDFLAGS) test/test.c -o test/test $(LIBS)

clean:
	rm -rf *~ */*~ $(OBJECTS) ./.depend test/test *.dSYM test/test.dSYM

edit:
	emacs -nw $(EXTRAS) $(HEADERS) $(SOURCES)
