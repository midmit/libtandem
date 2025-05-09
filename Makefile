CC_FLAGS = -ggdb
VERSION = 0.1.0

all: libtandem.so libtandem.a remove_o

libtandem.so: tandem.o
	cd build && cc $(CC_FLAGS) tandem.o -shared -o libtandem.so.$(VERSION)
	cd build && ln -sf libtandem.so.$(VERSION) libtandem.so

libtandem.a: tandem.o
	cd build && ar rcs libtandem.a tandem.o

remove_o: tandem.o
	rm build/tandem.o

tandem.o: include/coro_vec.h include/tandem/coro.h src/coro.c src/coro_vec.c
	mkdir -p build/ build/include

	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/coro.c -o coro.o
	cd build && cc $(CC_FLAGS) -I../include -c ../src/coro_vec.c -o coro_vec.o
	cc $(CC_FLAGS) -c src/arch/x86_64.s -o build/x86_64.o
	cd build && cc $(CC_FLAGS) -shared coro.o coro_vec.o x86_64.o -o tandem.o

	cd build && rm -f coro.o coro_vec.o x86_64.o

	cp -r include/tandem/ build/include/tandem

examples: libtandem.so examples/*
	mkdir -p build/examples
	cc $(CC_FLAGS) -Wall -Iinclude -Lbuild -ltandem \
	-Wl,-rpath build/ \
	examples/basic.c -o build/examples/basic

clean:
	rm -r build/

.PHONY: libtandem.so libtandem.a
