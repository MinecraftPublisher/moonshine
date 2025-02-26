FLAGS = -Wno-incompatible-library-redeclaration -nostartfiles
SIZE_FLAGS = -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -s -fno-unwind-tables -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-ident -fvisibility=hidden -march=native -Wl,--build-id=none

all: m4 c test

m4:
	m4 moonshine.m4 > moonshine.m4.h

c:
	clang main.c -o out.bin $(FLAGS) -O3

test:
	./out.bin

release:
	clang main.c -o out.bin.release $(FLAGS) $(SIZE_FLAGS)

debug: m4
	clang main.c -o out.bin $(FLAGS) -g
	lldb ./out.bin -o run

install:
	# make sure you have your CPATH configured to recognize `~/include/`!
	cp moonshine.h ~/include/moonshine.h
	cp moonshine.m4.h ~/include/moonshine.m4.h

full: m4 c test install
