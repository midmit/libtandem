CC_FLAGS = -ggdb
VERSION = 0.1.0

all: libtandem.so libtandem.a examples clean

libtandem.a: libtandem.so
	cd build && ar rc libtandem.a coro.o async.o coro_vec.o x86_64.o

libtandem.so: include/coro_vec.h include/tandem/coro.h src/coro.c src/async.c src/coro_vec.c
	mkdir -p build/ build/include

	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/coro.c -o coro.o
	cd build && cc -fPIC $(CC_FLAGS) -I../include -c ../src/async.c -o async.o

	cd build && cc $(CC_FLAGS) -I../include -c ../src/coro_vec.c -o coro_vec.o
	cc $(CC_FLAGS) -c src/arch/x86_64.s -o build/x86_64.o
	cd build && cc $(CC_FLAGS) -shared -luring coro.o async.o coro_vec.o x86_64.o -o libtandem.so

	cp -r include/ build/

examples: libtandem.a
	mkdir -p build/examples

	cc $(CC_FLAGS) -Wall \
	examples/basic.c -o build/examples/basic \
	-Ibuild/include/ -Lbuild -l:libtandem.a -Wl,-rpath=build

	cc $(CC_FLAGS) -Wall \
	examples/async.c -o build/examples/async \
	-Ibuild/include/ -Lbuild -l:libtandem.a -luring -Wl,-rpath=build

clean:
	cd build && rm -f coro.o async.o coro_vec.o x86_64.o

cleanall:
	rm -r build/
