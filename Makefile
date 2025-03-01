FLAGS = -Wno-incompatible-library-redeclaration -nostartfiles
SIZE_FLAGS = -Wl,--undefined=main -flto -Oz -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all -finline-hint-functions -fno-builtin -Wl,--print-gc-sections -fno-unwind-tables -fomit-frame-pointer -fno-asynchronous-unwind-tables -fno-ident -fvisibility=hidden -march=native -Wl,--build-id=none

all: m4 c test

m4:
	m4 moonshine.m4 > moonshine.m4.h

c:
	clang main.c -o out.bin $(FLAGS) -O3

test:
	./out.bin

profile:
	clang main.c -o out.bin.profile $(FLAGS) -O3
	perf record ./out.bin.profile
	perf report > ./out.profile.txt

# useless. it don't work.
# profile:
# 	clang -D__MOONSHINE_PROFILER main.c -o out.bin.profile $(FLAGS) $(SIZE_FLAGS) -fprofile-generate
# 	echo NOTE: Let the program run completely and provide it with typical inputs.
# 	./out.bin.profile
# 	llvm-profdata merge -output=default.profdata profile.profraw
# 	rm -rf profile.profraw
# 	clang main.c -o out.bin.release $(FLAGS) $(SIZE_FLAGS) -fprofile-use
#   rm -rf profile.profdata

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
