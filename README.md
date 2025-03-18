# moonshine
This library aims to be a drop-in libc replacement for most C projects, whilst also providing helpful utilies such as variable arrays, bounds checking, better memory allocation, algebraic data types, garbage collection, and more.

It already supports the system calls and library functions which were necessary for its functionality, but more stdlib support should come soon.
This library relies upon nothing but the C language itself, not even the standard library headers that C often uses, such as stdlib.h, unistd.h, etc.

**Note**: Moonshine currently focuses only on supporting x86_64 Linux.
Support for more operating systems and architectures is not part of the goals of this project, but it is a consideration to be kept in mind.
That said, any contribution to expand support to other configurations are absolutely welcome.

## Usage guide
First, build the builder (yes, the build system and macro generator are written in no-libc C as well):
```sh
$ clang build.c -nostdlib -nostartfiles -std=c23 -o builder
```
Run it for a guide:
```sh
$ ./builder
```
Build the macros:
```sh
$ ./builder macro
```
Verify the test cases:
```sh
$ ./builder run
```
Install moonshine:
```sh
$ ./builder install
```
All-in-one quick install:
```sh
$ ./builder all
```

## Current features
- Mark & Sweep garbage collector paired with a unique checkpoint arena system for safe and responsible memory management
- Easy array creation:
    `new(int)`, `new(char, 4)`, `fill(2, 4, 5)`, `fill("hello", "world")`, `single("string")`, `single(22.4)`
- Global print statement, takes up to 512 arguments of any type, and figures out how to print them out by itself.
  (Currently does not support custom structs). Example:
    `print("hello, ", "world!")`, `print("The sum is: ", 2 + 5)`, `print("I have a pointer with size ", pointer_size, " and address ", pointer)`
- Algebraic data types using the `datatype` macro, eg. 
    `datatype(binarytree, ((leaf, int), (node, binarytree *, int, binarytree *) ));`
- Self-defined types such as `var` for `void*`, `string` for `char*`, `u8` for `uint64_t`, and const-correct types such as `car` for `const void*` and `ctring` for `const char*`
- Basic type coercion:
    `auto x = 4; auto y = 9.0; auto z = "Hello!";`
- Custom float type for more accuracy and ease-of-use
- Convert file sizes to a human readable format
- Array processing macros such as `map` and `reduce` which can be used such as 
    `map((output_type, array), item_name, ({ expression; }))`, `reduce(array, (initial, item_name), ({ expression; }))` or
    `filter(array, item_name, ({ boolean_expression; }))`
- Easy system calls: 
    `syscall2(60, 0); // 60 = SYS_exit`
- Const-correct stdlib functions: `exit`, `fork`, `getchar`, `putchar`, `strlen`, `memset`, `memmove`, etc
- Made-from-scratch stdout buffer management: `write`, `flush`, etc
- Deprecation warnings for functions such as `malloc`, `realloc` and `free`
- Intelligent and thread-safe memory management using `alloc`, `remap` and `release` which automatically switches allocators to balance performance and load-tolerance.
- Performant hash table implementation
- Check pointer ownership using `mine(var ptr)`
- Easy array manipulation with `push(array, item)`
- Array bounds checking using `get(array, index)`, `set(array, index, value)`, `count(array)`, `last(array)` and metadata using `element_size(array)`
- Foreach loops using
    `foreach(item in array) { print(item); }` or `foreach(item in entries(hashtable)) { print(item); }` to loop over a hashtable.
- Revamped stdlib functions such as `char* strdup(const char* str) -> t(char) strtdup(ctring str)`
- Array manipulation using `reverse_array(array)` which returns the reversed array and also reverses it in-place.
- Custom `_start` function that initializes moonshine functionality whilst still keeping the ease of use of the libc `main` function.
- Pattern matching using algebraic data types.
- Easy thread creation as simple as `thread({ /* thread code */ })` which returns a thread management object.

## Roadmap
- Achieving more libc compatibility
- Moving common operations to macros for reduced code size and increased conciseness

## Layout
At the core of moonshine lies its linux systemcalls, which communicate directly with the kernel to exchange information.
Atop that layer are the allocators, which handle fast dynamic memory allocation and deallocation,
which are the supporting pillars of the tricky memory manipulations that power the dynamic memory allocations, the general-purpose macros, and more.
