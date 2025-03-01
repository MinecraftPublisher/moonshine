// Compile any program that uses this library with `-nostartfiles -nostdlib` :)
// This library probably only works with clang/llvm, so don't try gcc...

#ifndef moonshine_header
#define moonshine_header

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

//

#include "moonshine.m4.h"

#define overload __attribute__((overloadable))
#define line     overload flatten
#define unused   __attribute__((unused))

// algebraic data type macros

#define expand(...)                         __VA_ARGS__
#define tail(x, ...)                        __VA_ARGS__
#define head(x, ...)                        x
#define second(a, b, ...)                   b
#define algebra_format(extra, time, x, ...) cat3(extra, _, time) cat4(object_, extra, _, time);
#define algebra_enum(extra, time, x)        cat(head x, _type),
#define algebra_argument(extra, time, body)                                                                                    \
    struct head body { EXPAND_general(algebra_format, cat(type_, head body), tail body) } head body;
#define algebra_type_size_inner(extra, time, ...) typedef __VA_ARGS__ cat3(type_, extra, time);
#define algebra_type_size(extra, time, ...)       EXPAND_general(algebra_type_size_inner, cat(head __VA_ARGS__, _), tail __VA_ARGS__)
// TODO: Add datatype initializations such as binarytree_leaf(int v), etc
#define datatype(node_name, body)                                                                                              \
    typedef struct node_name node_name;                                                                                        \
    EXPAND_general2(algebra_type_size, node_name, expand body) struct node_name {                                              \
        enum cat(node_name, _type) { EXPAND_general2(algebra_enum, node_name, expand body) } type;                             \
        union {                                                                                                                \
            EXPAND_general2(algebra_argument, node_name, expand body)                                                          \
        } data;                                                                                                                \
    }
#define instance(__type, kind, ...) ((struct __type) { .type = cat(kind, _type), .data.kind = { __VA_ARGS__ } })
#define scope(name, ...)                                                                                                       \
    _Pragma("unroll") for (auto name unused = __VA_ARGS__, datatype_break = (typeof(name)) 0;                                  \
                           datatype_break == (typeof(name)) 0;                                                                 \
                           datatype_break = (typeof(name)) 1)
#define match(value)                          scope(parent_value, &value) switch (parent_value->type)
#define algebra_field_sum(extra, time, field) +sizeof(cat(extra, field))
#define algebra_of_field(extra, time, field)  scope(field, ({ parent_value->data.extra.cat4(object_type_, extra, _, time); }))
#define of(type, ...)                                                                                                          \
    break;                                                                                                                     \
    case cat(type, _type): EXPAND_general(algebra_of_field, type, __VA_ARGS__)
#define other()                                                                                                                \
    break;                                                                                                                     \
    default:

// array macros

#define obj(type) (type *) alloc(sizeof(type))

#define new(a, ...)         ((a **) new_x(a __VA_OPT__(, ) __VA_ARGS__, new2, new1)(a __VA_OPT__(, ) __VA_ARGS__))
#define new_x(a, b, c, ...) c
#define new1(type)          (__new_array(#type, sizeof(type), 0))
#define new2(type, count)   (__new_array(#type, sizeof(type), count))

#define fill(type, ...)                                                                                                        \
    ({                                                                                                                         \
        type __fill_values[] = { __VA_ARGS__ };                                                                                \
        u4   __fill_count    = sizeof(__fill_values) / sizeof(type);                                                           \
        auto __fill_arr      = new (type, __fill_count);                                                                       \
        for (u4 __i = 0; __i < __fill_count; __i++) { de(__fill_arr)[ __i ] = __fill_values[ __i ]; }                          \
        __fill_arr;                                                                                                            \
    })

#define single(...)                                                                                                            \
    ({                                                                                                                         \
        auto value = __VA_ARGS__;                                                                                              \
        fill(typeof(value), value);                                                                                            \
    })

int errno = 0;

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define NULL ((void *) 0)

#if defined __x86_64__ && !defined __ILP32__
    #define __WORDSIZE 64
#else
    #define __WORDSIZE                32
    #define __WORDSIZE32_SIZE_ULONG   0
    #define __WORDSIZE32_PTediFF_LONG 0
#endif

#define __WORDSIZE_TIME64_COMPAT32 1

#ifdef __x86_64__
    #define __syscall_WORDSIZE 64
#endif

typedef signed char        int8_t;
typedef unsigned char      uint8_t;
typedef signed short int   int16_t;
typedef unsigned short int uint16_t;
typedef signed int         int32_t;
typedef unsigned int       uint32_t;
#if __WORDSIZE == 64
typedef signed long int   int64_t;
typedef unsigned long int uint64_t;
#else
__extension__ typedef signed long long int   int64_t;
__extension__ typedef unsigned long long int uint64_t;
#endif

typedef unsigned char byte;
typedef char         *string;
typedef const char   *ctring;
typedef void         *ptr;
typedef void         *var;
typedef const void   *car;
typedef uint64_t      u8;
typedef int64_t       i8;
typedef int32_t       i4;
typedef uint32_t      u4;
typedef uint8_t bool;

const bool true  = 1;
const bool false = 0;

#define auto __auto_type

#define switch_item(value, ...)                                                                                                \
    case value: {                                                                                                              \
        __VA_ARGS__;                                                                                                           \
    }; break
#define switch_none(...)                                                                                                       \
    default: {                                                                                                                 \
        __VA_ARGS__;                                                                                                           \
    } break

#define decast(type) (*((type *) value))

#define syscall1(num)                                                                                                          \
    ({                                                                                                                         \
        long result;                                                                                                           \
        __asm__ volatile("syscall" : "=a"(result) : "a"(num) : "rcx", "r11", "memory", "cc");                                  \
        result;                                                                                                                \
    })

#define syscall2(num, arg1)                                                                                                    \
    ({                                                                                                                         \
        long result;                                                                                                           \
        __asm__ volatile("syscall" : "=a"(result) : "a"(num), "D"(arg1) : "rcx", "r11", "memory", "cc");                       \
        result;                                                                                                                \
    })

#define syscall3(num, arg1, arg2)                                                                                              \
    ({                                                                                                                         \
        long result;                                                                                                           \
        __asm__ volatile("syscall" : "=a"(result) : "a"(num), "D"(arg1), "S"(arg2) : "rcx", "r11", "memory", "cc");            \
        result;                                                                                                                \
    })

#define syscall4(num, arg1, arg2, arg3)                                                                                        \
    ({                                                                                                                         \
        long result;                                                                                                           \
        __asm__ volatile("syscall" : "=a"(result) : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3) : "rcx", "r11", "memory", "cc"); \
        result;                                                                                                                \
    })

#define syscall5(num, arg1, arg2, arg3, arg4)                                                                                  \
    ({                                                                                                                         \
        long          result;                                                                                                  \
        register long __arg4 __asm__("r10") = (long) (arg4);                                                                   \
        __asm__ volatile("syscall"                                                                                             \
                         : "=a"(result)                                                                                        \
                         : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(__arg4)                                              \
                         : "rcx", "r11", "memory", "cc");                                                                      \
        result;                                                                                                                \
    })

#define syscall6(num, arg1, arg2, arg3, arg4, arg5)                                                                            \
    ({                                                                                                                         \
        long          result;                                                                                                  \
        register long __arg4 __asm__("r10") = (long) (arg4);                                                                   \
        register long __arg5 __asm__("r8")  = (long) (arg5);                                                                   \
        __asm__ volatile("syscall"                                                                                             \
                         : "=a"(result)                                                                                        \
                         : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(__arg4), "r"(__arg5)                                 \
                         : "rcx", "r11", "memory", "cc");                                                                      \
        result;                                                                                                                \
    })

#define syscall7(num, arg1, arg2, arg3, arg4, arg5, arg6)                                                                      \
    ({                                                                                                                         \
        long          result;                                                                                                  \
        register long __arg4 __asm__("r10") = (long) (arg4);                                                                   \
        register long __arg5 __asm__("r8")  = (long) (arg5);                                                                   \
        register long __arg6 __asm__("r9")  = (long) (arg6);                                                                   \
        __asm__ volatile("syscall"                                                                                             \
                         : "=a"(result)                                                                                        \
                         : "a"(num), "D"(arg1), "S"(arg2), "d"(arg3), "r"(__arg4), "r"(__arg5), "r"(__arg6)                    \
                         : "rcx", "r11", "memory", "cc");                                                                      \
        result;                                                                                                                \
    })

#define putchar(x)                                                                                                             \
    ({                                                                                                                         \
        byte y = x;                                                                                                            \
        write((char *) &y, 1, stdout);                                                                                         \
    })

#define stdin  0
#define stdout 1
#define stderr 2

#define BUFFER_SIZE 4096

static char buffer[ BUFFER_SIZE ];
static int  buffer_index = 0;

int flush(int fd);

int flush(int fd) {
    if (buffer_index == 0) { return 0; }

    int result   = syscall4(1, fd, buffer, buffer_index);
    buffer_index = 0;
    return result;
}

int write(const char *ptr, const int size, const int fd) {
    int total_written = 0;
    for (int i = 0; i < size; i++) {
        if (ptr[ i ] == 0) continue;

        if (buffer_index >= BUFFER_SIZE || ptr[ i ] == '\n') {
            int result = flush(fd);
            if (result < 0) { return -1; }
            total_written += result;
        }

        buffer[ buffer_index++ ] = ptr[ i ];

        if (ptr[ i ] == '\n') {
            int result = flush(fd);
            if (result < 0) { return -1; }
            total_written += result;
        }
    }

    if (buffer_index > 0) {
        int result = flush(fd);
        if (result < 0) { return -1; }
        total_written += result;
    }

    return total_written;
}

#define puts_size(value, size) write(value, size, stdout)
#define puts(value)                                                                                                            \
    ({                                                                                                                         \
        auto v = value;                                                                                                        \
        puts_size(v, strlen(v));                                                                                               \
    })
#define puts_static(value) puts_size(value, sizeof(value))

void puts_static_ptr(car value, u8 size) { puts_size(*(string *) value, size); }

#define print_local(extra, time, value)                                                                                        \
    ({                                                                                                                         \
        if (#value[ 0 ] == '"') {                                                                                              \
            auto obj = value;                                                                                                  \
            puts_static_ptr((car) (u8) & obj, sizeof(value));                                                                  \
        } else {                                                                                                               \
            printer(value, #value);                                                                                            \
        }                                                                                                                      \
    });

#define print(...)                                                                                                             \
    ({                                                                                                                         \
        EXPAND_general(print_local, "broken print", __VA_ARGS__);                                                              \
        putchar('\n');                                                                                                         \
    })

void puts_number(const int64_t number, const bool is_signed) {
    uint64_t n;
    if (is_signed && number < 0) {
        putchar('-');
        n = (uint64_t) (~number) + 1;
    } else {
        n = (uint64_t) number;
    }

    char buffer[ 20 ];
    int  pos = 20;

    do {
        buffer[ --pos ] = '0' + (n % 10);
        n /= 10;
    } while (n != 0);

    for (int i = pos; i < 20; ++i) { putchar(buffer[ i ]); }
}

void puts_float(const float number) {
    int   int_part  = (int) number;
    float frac_part = number - int_part;

    if (int_part < 0) {
        putchar('-');
        int_part = -int_part;
    }

    char int_buffer[ 12 ];
    int  i = 0;
    do {
        int_buffer[ i++ ] = (int_part % 10) + '0';
        int_part /= 10;
    } while (int_part > 0);

    while (i > 0) { putchar(int_buffer[ --i ]); }

    putchar('.');

    for (int j = 0; j < 6; j++) {
        frac_part *= 10;
        int frac_digit = (int) frac_part;
        putchar(frac_digit + '0');
        frac_part -= frac_digit;
    }
}

#define flatten __attribute__((flatten))

flatten void puts_hex(const unsigned char hex) {
    const char hex_chars[] = "0123456789abcdef";
    putchar(hex_chars[ hex >> 4 ]);
    putchar(hex_chars[ hex & 0x0F ]);
}

flatten void puts_pointer(const void *ptr) {
    puts_static("0x");

    unsigned long address = (unsigned long) ptr;

    char hex_buffer[ 16 ];
    int  i = 0;

    do {
        int digit = address % 16;
        if (digit < 10) {
            hex_buffer[ i++ ] = digit + '0';
        } else {
            hex_buffer[ i++ ] = (digit - 10) + 'a';
        }
        address /= 16;
    } while (address > 0);

    while (i > 0) { putchar(hex_buffer[ --i ]); }
}

__attribute__((diagnose_as_builtin(__builtin_strlen, 1))) unsigned long strlen(ctring str) {
    unsigned long len = 0;
    while (str[ len ] != '\0') len++;
    return len;
}

#define cast_index(arr, type, index) ((type *) arr)[ index ]
#define cast_ptr(arr, type, index)   (&cast_index(arr, type, index))

#define align_value(value) ((u8) ((((value) + 15) & ~15ULL)))
const int   magic_number = 0xB00E;
typedef int magic_type;
const int   __addon_size = 2 * sizeof(u4) + sizeof(string) + sizeof(magic_type);
const int   addon_size   = align_value(__addon_size);
const int   spacer_size  = addon_size - __addon_size;

#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define PROT_READ     0x1
#define PROT_WRITE    0x2
#define PROT_EXEC     0x4
#define MAP_FIXED     0x10
#define MAP_ANONYMOUS 0x20

var __bare_mmap(const var addr, const u8 len, const int prot, const int flags, const int fd, const int64_t offset)
    [[clang::allocating]] {
    const u8 syscall_number = 9;

    const var result = (var) syscall7(syscall_number, addr, len, prot, flags, fd, offset);

    if ((int64_t) result == -1) {
        errno = -(int64_t) result;
        return (void *) -1;
    }

    return result;
}

int __bare_munmap(const var addr, const uint64_t len) {
    const uint64_t syscall_number = 11;
    const int      result         = syscall3(syscall_number, addr, len);

    return result;
}

var __bare_alloc(const uint64_t size) [[clang::allocating]] {
    return __bare_mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
}

#define __demon(code, ID)                                                                                                      \
    ({                                                                                                                         \
        flush(stdout);                                                                                                         \
        auto cat(fork_, ID) = fork();                                                                                          \
        if (cat(fork_, ID) < 0) {                                                                                              \
            puts_static("Failed to fork!\n");                                                                                  \
            exit(1);                                                                                                           \
        } else if (cat(fork_, ID) == 0) {                                                                                      \
            code;                                                                                                              \
            exit(0);                                                                                                           \
            byte *osjdoijsoij = (byte *) (0 + 129387 - 129387);                                                                \
            (void) *osjdoijsoij;                                                                                               \
        };                                                                                                                     \
        cat(fork_, ID);                                                                                                        \
    })
#define demon(code) __demon(code, expand(__COUNTER__))

int fork() {
    const uint64_t syscall_number = 57;
    const int      pid            = syscall1(syscall_number);

    return pid;
}

void exit(const int exit_code) {
    const uint64_t syscall_number = 60;

    (void) syscall2(syscall_number, exit_code);

    __builtin_unreachable();
}

int wait(const int *status) {
    const uint64_t syscall_number = 9;
    const int      result         = syscall3(syscall_number, -1, status);

    return result;
}

// Check if the current process tree owns a pointer. Expensive, do not use
// sparingly.
bool mine(const ptr pointer) {
    bool *glob_var = __bare_alloc(sizeof(bool));

    (void) demon({
        *glob_var = 0;
        byte *x   = pointer;
        byte  y   = *x;
        (void) y;
        __asm__ volatile("mov %[ptr], %%edi" : : [ptr] "m"(*(void **) pointer) : "edi");
        *glob_var = 1;
    });

    wait(NULL);
    bool output = *glob_var;

    __bare_munmap(glob_var, sizeof(bool));

    return output;
}

line void printer(const char x, ctring unused text) { putchar(x); }
line void printer(ctring x, ctring unused text) { puts(x); }
line void printer(const int x, ctring text) {
    if (text[ 0 ] == '\'') putchar(x);
    else
        puts_number(x, 1);
}
line void printer(const long x, ctring unused text) { puts_number(x, true); }
line void printer(const unsigned long x, ctring unused text) { puts_number(x, false); }
line void printer(const u4 x, ctring unused text) { puts_number(x, false); }
line void printer(const short x, ctring unused text) { puts_number(x, false); }
line void printer(const unsigned short x, ctring unused text) { puts_number(x, false); }
line void printer(const float x, ctring unused text) { puts_float(x); }
line void printer(const double x, ctring unused text) { puts_float(x); }
line void printer(const bool x, ctring unused text) {
    if (x) puts_static("true");
    else if (x == 0) {
        puts_static("false");
    } else {
        putchar(x);
    }
}
line void printer(var x, ctring unused text) {
    if (mine(x)) {
        if (mine(*(var *) x)) {
            // handle array
            var array = *(var *) x;

            array = cast_ptr(array, byte, -addon_size);

            if (cast_index(array, magic_type, 0) != magic_number) goto UNKNOWN;
            array = cast_ptr(array, magic_type, 1);

            string array_type = cast_index(array, string, 0);

            array = cast_ptr(array, string, 1);

            u4 element_count = cast_index(array, u4, 1);

            puts_static("<(");
            puts(array_type);
            puts_static(")_array[");
            puts_number(element_count, false);
            puts_static("]>");

            return;
        }
    }

UNKNOWN:;
    puts_static("<pointer ");
    puts_pointer(x);
    puts_static(" (");
    puts_number((u8) x, false);
    puts_static(")>");
}
line void printer(car x, ctring unused text) { printer((var) x, text); }

#define SYS_read 0

char getchar() {
    char c;
    u8   result = syscall4(SYS_read, stdin, &c, 1);

    if (c == 0) return -1;
    if (result <= 0) return -1;

    return c;
}

#define __MOONSHINE_MALLOC_WARNINGS
#ifdef __MOONSHINE_MALLOC_WARNINGS
    #define MOONSHINE_LEGACY_LIBC_WARNING                                                                                      \
        "You are using moonshine, this library does not allow the use of legacy libc functions. Please use the "               \
        "moonshine-provided alternative instead."
void malloc() __attribute__((deprecated(MOONSHINE_LEGACY_LIBC_WARNING, "alloc")));
void realloc() __attribute__((deprecated(MOONSHINE_LEGACY_LIBC_WARNING, "remap")));
void free() __attribute__((deprecated(MOONSHINE_LEGACY_LIBC_WARNING, "release")));
#endif

__attribute__((diagnose_as_builtin(__builtin_strcmp, 1, 2))) byte strcmp(ctring left, ctring right) {
    while (*left && (*left == *right) && left++ && right++);
    return (byte) (*left) - (byte) (*right);
}

__attribute__((diagnose_as_builtin(__builtin_strncmp, 1, 2, 3))) byte strncmp(ctring left, ctring right, u8 n) {
    while (n && *left && (*left == *right)) {
        left++;
        right++;
        n--;
    }
    return (n == 0) ? 0 : ((byte) (*left) - (byte) (*right));
}

#define eq(a, b) (strcmp(a, b) == 0)

u8 strnlen(ctring txt, u8 len) {
    u8 s = 0;
    while (s < len && txt[ s ] != '\0') s++;
    return s;
}

__attribute__((diagnose_as_builtin(__builtin_memset, 1, 2, 3))) void memset(var _ptr, int n, u8 size) {
    string ptr = _ptr;
    for (u8 i = 0; i < size; i++) ptr[ i ] = n;
}

__attribute__((diagnose_as_builtin(__builtin_memmove, 1, 2, 3))) void *memmove(void *dest, const void *src, unsigned long n) {
    unsigned char       *d = (unsigned char *) dest;
    const unsigned char *s = (const unsigned char *) src;
    if (d == s || n == 0) return dest;
    if (d < s || d >= s + n)
        for (unsigned long i = 0; i < n; i++) d[ i ] = s[ i ];
    else
        for (unsigned long i = n; i > 0; i--) d[ i - 1 ] = s[ i - 1 ];

    return dest;
}

__attribute__((diagnose_as_builtin(__builtin_memcpy, 1, 2, 3))) void memcpy(var _dest, car _src, const unsigned long n) {
    string dest = _dest;
    ctring src  = _src;

    for (u8 i = 0; i < n; i++) dest[ i ] = src[ i ];
}

#define SYS_getpid 39
#define SYS_kill   62

#define SIGTRAP 5
#define SIGILL  4

#define getpid()       syscall1(SYS_getpid)
#define kill(pid, sig) syscall3(SYS_kill, pid, sig)
#define raise(sig)     kill(getpid(), sig)

// TODO: Setting a breakpoint multiple times can fuck with the stdout buffer, somehow
#define breakpoint(str)                                                                                                        \
    ({                                                                                                                         \
        const string wowie = "" str;                                                                                           \
        (void) wowie;                                                                                                          \
        (void) raise(SIGTRAP);                                                                                                 \
    })

#define fg_ansi(r, g, b) "\e[38;2;" #r ";" #g ";" #b "m"
#define bg_ansi(r, g, b) "\e[48;2;" #r ";" #g ";" #b "m"
#define no_ansi()        "\e[0m"

#define throw(...)                                                                                                             \
    ({                                                                                                                         \
        print(fg_ansi(255, 90, 90) "Error: ", __VA_ARGS__, no_ansi());                                                         \
        puts_static(fg_ansi(235, 235, 50) "The following is an exception-related crash: " no_ansi());                          \
        (void) raise(SIGILL);                                                                                                  \
        __builtin_unreachable();                                                                                               \
        0;                                                                                                                     \
    })

var __bare_mremap(const var ptr, const uint64_t old, const uint64_t new) [[clang::allocating]] {
    const var new_ptr = __bare_alloc(new);
    memcpy(new_ptr, ptr, old);
    __bare_munmap(ptr, old);
    return new_ptr;
}

#define BASE_ARRAY_CAPACITY      32
#define BASE_PAGE_TABLE_CAPACITY 16

struct AllocatedPointer {
    u8   size;
    u8   location;
    byte allocated;
};

struct FreeBlock {
    u8 start;
    u8 size;
};

struct Page {
    bool             dirty;
    struct FreeBlock last_result;
    u8               size;
    string           start;
    struct PageArray {
        u8                       size;
        u8                       capacity;
        struct AllocatedPointer *data;
    } pointers;
};

struct PageTable {
    u8           size;
    u8           capacity;
    struct Page *pages;
};

#define AllocatedPointer(size, location)                                                                                       \
    ((struct AllocatedPointer) { .size = (size), .location = (location), .allocated = true })
#define PageArray()                                                                                                            \
    ((struct PageArray) { .capacity = BASE_ARRAY_CAPACITY,                                                                     \
                          .data     = __bare_alloc(sizeof(struct AllocatedPointer) * BASE_ARRAY_CAPACITY) })
#define Page(page_size)                                                                                                        \
    ((struct Page) { .dirty       = true,                                                                                      \
                     .last_result = {},                                                                                        \
                     .size        = page_size,                                                                                 \
                     .start       = __bare_alloc(sizeof(byte) * page_size),                                                    \
                     .pointers    = PageArray() })

#define PageTable()                                                                                                            \
    ((struct PageTable) { 0, BASE_PAGE_TABLE_CAPACITY, __bare_alloc(sizeof(struct Page) * BASE_PAGE_TABLE_CAPACITY) })

struct PageTable global_page_table;

typedef unsigned int size_t;

static void swap(char *a, char *b, size_t size) {
    while (size--) {
        char tmp = *a;
        *a++     = *b;
        *b++     = tmp;
    }
}

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void *)) {
    if (nitems < 2) return;

    char  *array      = (char *) base;
    size_t pivotIndex = nitems / 2;

    swap(array, array + pivotIndex * size, size);

    size_t last = 0;
    for (size_t i = 1; i < nitems; i++) {
        if (compar(array + i * size, array) < 0) {
            last++;
            swap(array + i * size, array + last * size, size);
        }
    }
    swap(array, array + last * size, size);

    qsort(array, last, size, compar);
    qsort(array + (last + 1) * size, nitems - last - 1, size, compar);
}

static int compare_locations(const void *a, const void *b) {
    const struct AllocatedPointer *ap = a;
    const struct AllocatedPointer *bp = b;
    if (ap->location < bp->location) return -1;
    if (ap->location > bp->location) return 1;
    return 0;
}

struct FreeBlock find_free_space(struct Page *page) [[clang::nonallocating]] {
    if (!page->dirty) return page->last_result;

    struct FreeBlock max_free = { 0, 0 };
    if (page == NULL) return max_free;

    uint64_t allocated_count = 0;
    for (uint64_t i = 0; i < page->pointers.size; ++i) {
        if (page->pointers.data[ i ].allocated) allocated_count++;
    }

    if (allocated_count == 0) {
        max_free.start    = 0;
        max_free.size     = page->size;
        page->dirty       = false;
        page->last_result = max_free;
        return max_free;
    }

    struct AllocatedPointer allocated_blocks[ allocated_count ];
    uint64_t                idx = 0;
    for (uint64_t i = 0; i < page->pointers.size; ++i) {
        if (page->pointers.data[ i ].allocated) { allocated_blocks[ idx++ ] = page->pointers.data[ i ]; }
    }

    qsort(allocated_blocks, allocated_count, sizeof(struct AllocatedPointer), compare_locations);

    uint64_t prev_end = 0;
    max_free.size     = 0;

    for (uint64_t i = 0; i < allocated_count; ++i) {
        struct AllocatedPointer *block       = &allocated_blocks[ i ];
        uint64_t                 block_end   = block->location + block->size;
        uint64_t                 current_end = block_end > page->size ? page->size : block_end;

        if (block->location > prev_end) {
            uint64_t gap_size = block->location - prev_end;
            if (gap_size > max_free.size) {
                max_free.start = prev_end;
                max_free.size  = gap_size;
            }
        }

        if (current_end > prev_end) { prev_end = current_end; }
    }

    if (prev_end < page->size) {
        uint64_t gap_size = page->size - prev_end;
        if (gap_size > max_free.size) {
            max_free.start = prev_end;
            max_free.size  = gap_size;
        }
    }

    if (max_free.start % 16 != 0) {
        const u8 change = align_value(max_free.start) - max_free.start;
        max_free.start += change;
        max_free.size -= change;
    }

    page->dirty       = false;
    page->last_result = max_free;

    return max_free;
}

#define max(a, b) ((a) > (b) ? (a) : (b))

void clean_pages() {
    for (u8 i = 0; i < global_page_table.size; i++) {
        auto page = &global_page_table.pages[ i ];

        if (page->start == NULL) continue;

        for (u8 ptr = 0; ptr < page->pointers.size; ptr++) {
            if (page->pointers.data[ ptr ].allocated) goto LEAVE_PAGE;
        }

        __bare_munmap(page->start, page->size);
        page->start = 0;
        page->dirty = true;
        __bare_munmap(page->pointers.data, sizeof(struct AllocatedPointer) * page->pointers.capacity);

    LEAVE_PAGE:;
    }
}

// TODO: Place page pointer and pointer size behind the allocated pointer

__attribute__((diagnose_as_builtin(__builtin_malloc, 1))) __attribute__((malloc)) var alloc(const u8 len)
    [[clang::allocating]] {
    static int alloc_count = 0;
    if (unlikely(alloc_count++ == 1024)) {
        alloc_count = 0;
        clean_pages();
    }

    i8               biggest_free_page = -1;
    struct FreeBlock max_biggest_free  = { 0, 0 };
    struct Page     *page;

    for (u8 i = 0; i < global_page_table.size; i++) {
        if (global_page_table.pages[ i ].start == NULL) continue;
        struct FreeBlock biggest_free = find_free_space(&global_page_table.pages[ i ]);

        if (biggest_free.size <= max_biggest_free.size) continue;
        max_biggest_free  = biggest_free;
        biggest_free_page = i;
    }

    u8 place = biggest_free_page;
    page     = &global_page_table.pages[ place ];

    if (max_biggest_free.size < len) {
        // add a page

        // re-use page space
        for (u8 i = 0; i < global_page_table.size; i++) {
            if (global_page_table.pages[ i ].start == 0) {
                place = i;
                goto END;
            }
        }

        place = global_page_table.size;
        global_page_table.size++;
        if (global_page_table.size == global_page_table.capacity) {
            global_page_table.pages = __bare_mremap(
                global_page_table.pages,
                global_page_table.size * sizeof(struct Page),
                (global_page_table.capacity *= 2) * sizeof(struct Page));
        }

    END:

        global_page_table.pages[ place ] = Page(max(len, 512));
        page                             = &global_page_table.pages[ place ];
        biggest_free_page                = place;
        max_biggest_free.size            = max(len, 512);
        max_biggest_free.start           = 0;
    }

    page->dirty = true;

    u8 pointer_place = 0;

    // re-use space
    for (u8 i = 0; i < page->pointers.size; i++) {
        if (page->pointers.data[ i ].allocated) continue;
        pointer_place = i;
        goto ABYSS;
    }

    pointer_place = page->pointers.size++;

    if (page->pointers.size == page->pointers.capacity) {
        page->pointers.data = __bare_mremap(
            page->pointers.data,
            page->pointers.size * sizeof(struct AllocatedPointer),
            (page->pointers.capacity *= 2) * sizeof(struct AllocatedPointer));
    }

ABYSS:;

    const string new_location = page->start + max_biggest_free.start;

    page->pointers.data[ pointer_place ]
        = (struct AllocatedPointer) { .size = len, .location = max_biggest_free.start, .allocated = true };

    return (var) new_location;
}

struct FoundPointer {
    u8                       page_index;
    u8                       allocation_size;
    struct AllocatedPointer *pointer_ref;
    struct Page             *page_ref;
};

struct FoundPointer find_pointer(const var addr) {
    for (u8 i = 0; i < global_page_table.size; i++) {
        struct Page *page     = &global_page_table.pages[ i ];
        const u8     location = (u8) addr - (u8) page->start;

        if (addr >= (var) page->start && location < page->size) {
            struct AllocatedPointer *best_ptr = NULL;

            for (u8 j = 0; j < page->pointers.size; j++) {
                struct AllocatedPointer *ptr = &page->pointers.data[ j ];

                if (ptr->location == location) {
                    if (!ptr->allocated && best_ptr != NULL) continue;
                    best_ptr = ptr;
                }
            }

            if (!best_ptr->allocated) throw("Unallocated pointer? ");
            if (best_ptr == NULL) throw("Pointer for location does not exist in page");

            return (struct FoundPointer) {
                .allocation_size = best_ptr->size, .pointer_ref = best_ptr, .page_ref = page, .page_index = i
            };
        }
    }

    throw("Invalid pointer!");
}

void unmap(const struct FoundPointer ptr, const var addr) {
    if (!ptr.pointer_ref->allocated) throw("unmap double-free for pointer ", addr);

    ptr.pointer_ref->allocated = false;
    ptr.page_ref->dirty        = true;
}

__attribute__((diagnose_as_builtin(__builtin_free, 1))) void release(const var addr) {
    if (addr == 0) return;
    unmap(find_pointer(addr), addr);
}

__attribute__((diagnose_as_builtin(__builtin_realloc, 1, 2))) var remap(const var ptr, const u8 new_size) {
    const var new_ptr = alloc(new_size);
    if (ptr == 0) return new_ptr;
    const auto found = find_pointer(ptr);
    memcpy(new_ptr, ptr, found.allocation_size);
    unmap(found, ptr);
    return new_ptr;
}

#define t(type) type **
#define de(obj) (*(obj))

const var *__new_array(const string type_name, const u4 type_size, const u4 count) [[clang::allocating]] {
    // string name, and one place for the special code
    const u8 size = type_size * count + addon_size;

    const var base = alloc(size);
    var       ptr  = base;

    cast_index(ptr, magic_type, 0) = magic_number; // magic number for arrays
    ptr                            = cast_ptr(ptr, magic_type, 1);

    cast_index(ptr, ctring, 0) = type_name;
    ptr                        = cast_ptr(ptr, string, 1);

    cast_index(ptr, u4, 0) = type_size;
    cast_index(ptr, u4, 1) = count;

    var *holder = alloc(sizeof(var));

    *holder = cast_ptr(base, byte, addon_size);

    return holder;
}

// TODO:
// 1. Add a hidden capacity checker
// 2. Based on the capacity, detect whether we should or shouldn't reallocate the memory
// 3. eg. if an item is popped, we may keep that memory intact so that further allocations and memory copies will be unnecessary

flatten void unsafe_extend(
    var      *array_ref,
    const var array_from_u4,
    const var array_base,
    const u4  element_count,
    const u4  element_size,
    const u4  offset) [[clang::allocating]] {
    cast_index(array_from_u4, u4, 1) += offset;
    const var    new_base    = remap(array_base, addon_size + element_size * (element_count + offset));
    const string array_place = &((string) new_base)[ addon_size ];
    *array_ref               = (var) array_place;
}

void extend(var *array, u8 count) [[clang::allocating]] {
    const var main_wrap  = cast_ptr(*array, byte, -addon_size);
    var       array_wrap = main_wrap;

    if (cast_index(array_wrap, magic_type, 0) != magic_number) {
        throw("Invalid array magic number: ", cast_index(array_wrap, magic_type, 0), ", expected ", magic_number);
    }

    array_wrap = cast_ptr(array_wrap, magic_type, 1);
    array_wrap = cast_ptr(array_wrap, string, 1);

    const u4 element_count = cast_index(array_wrap, u4, 1);
    const u4 element_size  = cast_index(array_wrap, u4, 0);

    unsafe_extend(array, array_wrap, main_wrap, element_count, element_size, count);
}

void unsafe_push(var *array, const var ptr, const u4 ptr_size) [[clang::allocating]] {
    const var main_wrap  = cast_ptr(*array, byte, -addon_size);
    var       array_wrap = main_wrap;

    if (cast_index(array_wrap, magic_type, 0) != magic_number) {
        throw("Invalid array magic number: ", cast_index(array_wrap, magic_type, 0), ", expected ", magic_number);
    }

    array_wrap       = cast_ptr(array_wrap, magic_type, 1);
    string type_name = array_wrap;
    array_wrap       = cast_ptr(array_wrap, string, 1);

    const u4 element_count = cast_index(array_wrap, u4, 1);
    const u4 element_size  = cast_index(array_wrap, u4, 0);

    if (unlikely(element_size != ptr_size && ptr_size != 0)) {
        throw(
            "Mismatching array size: Item of size ",
            ptr_size,
            " cannot be placed in an array with elements of size ",
            element_size,
            " and of type ",
            type_name,
            '.');
    }

    unsafe_extend(array, array_wrap, main_wrap, element_count, element_size, 1);
    memcpy(&(((char *) array[ 0 ])[ element_count * element_size ]), ptr, ptr_size);
}

#define count(array) ((u4) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u4, 1))
#define element_size(array)                                                                                                    \
    (u4) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u4, 0)
#define last(array) ((array)[ 0 ][ count(array) ])
#define safe_push(type, array, ptr, ...)                                                                                       \
    ({                                                                                                                         \
        type **_array = array;                                                                                                 \
        type _ptr     = ptr __VA_OPT__(, ) __VA_ARGS__;                                                                        \
        unsafe_push((var *) _array, (var) & (_ptr), sizeof(type));                                                             \
    })
#define push(array, ptr, ...) safe_push(typeof((array)[ 0 ][ 0 ]), array, ptr __VA_OPT__(, ) __VA_ARGS__)

var return_item(var **arr, u8 index) {
    if (unlikely(index >= count(arr))) {
        throw("Requested index in array (", index, ") exceeds the size of the array! (", count(arr), ")");
    }
    if (unlikely(index < 0)) throw("Cannot access negative offset in array!");

    return &arr[ 0 ][ index * element_size(arr) ];
}

void set_item(var **arr, u8 index, var value) {
    if (unlikely(index >= count(arr))) {
        throw("Target index in array (", index, ") exceeds the size of the array! (", count(arr), ")");
    }
    if (unlikely(index < 0)) throw("Cannot write to negative offset in array!");

    memcpy(&arr[ 0 ][ index * element_size(arr) ], value, element_size(arr));
}

#define address_item(__array, __index) (typeof(__array[0])) return_item((var **) __array, __index)
#define get(__array, __index)          (*(typeof(__array[0])) return_item((var **) __array, __index))
#define set(__array, __index, __value) ({ typeof(__array[0][0]) v = __value; set_item((var **) __array, __index, &v); })

#define in               ,
#define foreach(...)     foreach_xp(foreach_inner, (__VA_ARGS__))
#define foreach_xp(a, b) a b
#define foreach_inner(item, array)                                                                                             \
    scope(cat(item, _index), (var) 0) for (auto item = get(array, (u8) 0); (u8) cat(item, _index) < count(array);              \
                                           item      = get(array, (u8) ++cat(item, _index)))

#define LIBC_WARNINGS_TEXT(name)                                                                                               \
    "[[>>> The " name                                                                                                          \
    " function is part of the moonshine revamp, which means it behaves a bit differently than the libc " name                  \
    ". If you acknowledge this fact and you've read on how this function behaves, you may disable the revamp warnings by "     \
    "defining the __MOONSHINE_LIBC_WARNINGS flag before including the moonshine library: #define "                             \
    "__MOONSHINE_LIBC_WARNINGS "                                                                                               \
    "<<<]]"

#ifndef __MOONSHINE_LIBC_WARNINGS
__attribute__((deprecated(LIBC_WARNINGS_TEXT("strdup"))))
#endif

t(char) strdup(ctring str) {
    const u8 len = strlen(str);
    t(char) obj  = new (char, len + 1);

    for (u8 i = 0; i < len; i++) obj[ 0 ][ i ] = str[ i ];

    obj[ 0 ][ len ] = 0;

    return obj;
}

var **__reverse_array(var **array) {
    const u4 element_size = element_size(array);
    const u4 count_val    = count(array);
    if (count_val <= 1) return array;
    byte temp[ element_size ];

    for (u4 i = 0; i < count_val / 2; i++) {
        u4 left_idx  = i;
        u4 right_idx = count_val - 1 - i;

        memcpy(temp, address_item(array, left_idx), element_size);
        memcpy(address_item(array, left_idx), address_item(array, right_idx), element_size);
        memcpy(address_item(array, right_idx), temp, element_size);
    }

    return array;
}

#define reverse_array(...) (typeof(__VA_ARGS__)) __reverse_array((var *) __VA_ARGS__)

t(char) strndup(ctring str, u8 len) {
    const auto obj = new (char, len + 1);

    for (u8 i = 0; i < len; i++) obj[ 0 ][ i ] = str[ i ];

    obj[ 0 ][ len ] = 0;

    return obj;
}

typedef struct linkednode linkednode;

// A typical node in a linked list.
struct linkednode {
    bool        init;
    linkednode *back[ 2 ];  // Double-index back-refernece for the node
    linkednode *front[ 2 ]; // Double-index forward-feference for the node
    var         data;       // The data for the linked list node
};

typedef struct linkedlist {
    u8          size;       // The size of a linked list
    linkednode *head;       // The head pointer for the linked list
    linkednode *tail;       // The tail pointer for the linked list
    u8          pool_size;  // The size of the last memory pool.
    t(t(linkednode)) pools; // Memory pool.
} linkedlist;

// Create an empty linked list.
flatten linkedlist *create_linkedlist() {
    linkedlist *list = obj(linkedlist);
    list->size       = 0;
    list->head       = NULL;
    list->tail       = NULL;

    list->pool_size = 1024;
    list->pools     = new (t(linkednode));
    push(list->pools, new (linkednode, list->pool_size));

    return list;
}

// Push a value to the end of a linked list.
void push_linkedlist(linkedlist *list, var value) {
    // Get object reference from memory pool.
    linkednode *node = obj(linkednode);

    // foreach (pool in list->pools) {
    //     if (!pool) continue;
    //     foreach (candid in pool) {
    //         if (candid.init) continue;
    //         node = &pool[ 0 ][ (u8) candid_index ];
    //         break;
    //     }
    // }

    // if (node == NULL) {
    //     t(linkednode) new_pool = new (linkednode, list->pool_size *= 2);

    //     node = &last(new_pool);
    //     push(list->pools, new_pool);
    // }

    node->init = true;
    node->data = value;

    auto follower      = list->tail;
    auto past_follower = list->tail == NULL ? NULL : list->tail->back[ 0 ];

    node->back[ 0 ]  = follower;
    node->back[ 1 ]  = past_follower;
    node->front[ 0 ] = node->front[ 1 ] = NULL;

    if (follower) {
        follower->front[ 0 ] = node;
        if (past_follower) past_follower->front[ 1 ] = node;
    }

    list->tail = node;
    if (!list->head) list->head = node;
}

linkednode *index_to_ptr(linkedlist *list, u8 index) {
    u8 last = list->size - 1;

    // perform intuitive access checks
    // clang-format off
    if (list->head == NULL) return NULL;
    else if (index == 0) return list->head;
    else if (index == last) return list->tail;
    else if (index == 1) return list->head->front[ 0 ];
    else if (index == 2) return list->head->front[ 1 ];
    else if (index == last - 1) return list->tail->back[ 0 ];
    else if (index == last - 2) return list->tail->back[ 1 ];
    // clang-format on

    linkednode *current;
    u8          current_index;

    // decide whether to start from the head or the tail of the list
    if (index <= (last / 2)) { // start from the head
        current       = list->head;
        current_index = 0;
        while (current && current_index < index) {
            if (current->front[ 1 ] && current_index + 2 <= index) {
                current = current->front[ 1 ];
                current_index += 2;
            } else if (current->front[ 0 ]) {
                current = current->front[ 0 ];
                current_index += 1;
            } else {
                break;
            }
        }
    } else { // start from the tail
        current       = list->tail;
        current_index = list->size - 1;
        while (current && current_index > index) {
            if (current->back[ 1 ] && current_index - 2 >= index) {
                current = current->back[ 1 ];
                current_index -= 2;
            } else if (current->back[ 0 ]) {
                current = current->back[ 0 ];
                current_index -= 1;
            } else {
                break;
            }
        }
    }

    return (current_index == index) ? current : NULL;
}

// This function will return a pointer to the linkednode, if free = false, and a pointer to the data contained inside, if free =
// true.
overload var pop_item(linkedlist *list, bool free, linkednode *item) {
    if (item == NULL) return NULL;

    if (list->tail == item) list->tail = item->back[ 0 ];
    else if (item->front[ 0 ]) {
        item->front[ 0 ]->back[ 0 ] = item->back[ 0 ];
        item->front[ 0 ]->back[ 1 ] = item->back[ 1 ];
        if (item->front[ 1 ]) item->front[ 1 ]->back[ 1 ] = item->back[ 0 ];
    }

    if (list->head == item) list->head = item->front[ 0 ];
    else if (item->back[ 0 ]) {
        item->back[ 0 ]->front[ 0 ] = item->front[ 0 ];
        item->back[ 0 ]->front[ 1 ] = item->front[ 1 ];
        if (item->back[ 1 ]) item->back[ 1 ]->front[ 1 ] = item->front[ 0 ];
    }

    item->front[ 0 ] = item->front[ 1 ] = item->back[ 0 ] = item->back[ 1 ] = NULL;

    list->size--;

    auto ptr = item->data;
    if (free) {
        item->init = false;
        return ptr;
    } else {
        return item;
    }
}

// This function will return a pointer to the linkednode, if free = false, and a pointer to the data contained inside, if free =
// true.
line var pop_item(linkedlist *list, bool free, u8 index) { return pop_item(list, free, index_to_ptr(list, index)); }

// This function removes a node from the linked list, and then places it at the start of the list.
overload void front_insert(linkedlist *list, linkednode *node) {
    pop_item(list, false, node);
    if (list->head) {
        node->front[ 0 ]      = list->head;
        node->front[ 1 ]      = list->head->front[ 0 ];
        list->head->back[ 0 ] = node;
        if (list->head->front[ 0 ]) list->head->front[ 0 ]->back[ 1 ] = node;
    }
    if (list->head == list->tail) list->tail = node;
    list->head = node;
}

// This function removes a node from the linked list, and then places it at the start of the list.
line void front_insert(linkedlist *list, u8 index) { return front_insert(list, index_to_ptr(list, index)); }

typedef u8 (*hash_function)(ctring);

u8 table_hash_1(ctring str) {
    u8   hash = 5381;
    char c;

    while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

// jenkins one-at-a-time hash
u8 table_hash_2(ctring key) {
    u8 hash, i;
    for (hash = i = 0; i < strlen(key); ++i) {
        hash += key[ i ];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

// murmurhash3 implementatin for 64-bit output
u8 table_hash_3(ctring data) {
    const u8  seed    = 0;
    u8        h1      = seed;
    const u8  len     = strlen(data);
    const u8  nblocks = len / 8;
    const u8  c1      = 0xff51afd7ed558ccdULL;
    const u8  c2      = 0xc4ceb9fe1a85ec53ULL;
    const u8 *blocks  = (const u8 *) (data);

    for (size_t i = 0; i < nblocks; i++) {
        u8 k1 = blocks[ i ];

        k1 *= c1;
        k1 = (k1 << 31) | (k1 >> 33);
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 27) | (h1 >> 37);
        h1 = h1 * 5 + 0x52dce729;
    }

    ctring tail = (data + nblocks * 8);
    u8     k1   = 0;

    switch (len & 7) {
        case 7: k1 ^= ((u8) tail[ 6 ]) << 48;
        case 6: k1 ^= ((u8) tail[ 5 ]) << 40;
        case 5: k1 ^= ((u8) tail[ 4 ]) << 32;
        case 4: k1 ^= ((u8) tail[ 3 ]) << 24;
        case 3: k1 ^= ((u8) tail[ 2 ]) << 16;
        case 2: k1 ^= ((u8) tail[ 1 ]) << 8;
        case 1:
            k1 ^= ((u8) tail[ 0 ]);
            k1 *= c1;
            k1 = (k1 << 31) | (k1 >> 33);
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= len;
    h1 ^= h1 >> 33;
    h1 *= c1;
    h1 ^= h1 >> 33;
    h1 *= c2;
    h1 ^= h1 >> 33;

    return h1;
}

typedef struct hashnode {
    u8     hash;
    ctring key;
    car    value;
} hashnode;

typedef struct hashtable {
    hash_function proto;
    bool          use_hash;
    u8            size;
    int8_t        current_recent_access;
    struct hashtable_optimizations {
        bool only_recent_collision_checks; // Fast-ish option? Only checks the recent access table for collisions.
        bool no_collision_checks;          // Fastest option. Disables collision checking altogether.
        bool run_ptr_check; // Faster option. Checks hash equivalency and pointer equivalency. Reduces rate of collisions if
                            // the string pointers used for fetching entries remain persistent. Setting this option to true
                            // will disable collision checks.
        byte collision_character_check_count; // Fast option. Check the first N characters of the inputs together, alongside the
                                              // hash. Setting this option to a non-zero value will disable collision checks.
        bool no_hash; // Slowest option. Always use strcmp to find entries. Does not bother hashing values or checking for
                      // collisions.
    } optimizations;
    struct recent_access {
        u8     hash;
        ctring key;
        car    value;
    } recent_access_nodes[ 16 ]; // Recently accessed table entries.
    linkedlist *entries;         // Linked list of hashnode structs.
} hashtable;

// TODO: Add a rehash function that allows the user to change the hash function, recompute all the hashes and detect collisions.

//
hashtable *create_table(hash_function proto) {
    hashtable  node     = { .use_hash              = true,
                            .size                  = 0,
                            .proto                 = proto,
                            .entries               = create_linkedlist(),
                            .current_recent_access = -1,
                            .recent_access_nodes   = { [0] = { 0, 0, 0 }, [15] = { 0, 0, 0 } },
                            .optimizations         = { // Default optimization options. Check for collisions.
                                                       .only_recent_collision_checks    = false,
                                                       .no_hash                         = false,
                                                       .run_ptr_check                   = false,
                                                       .collision_character_check_count = 0,
                                                       .no_collision_checks             = false } };
    hashtable *node_ref = obj(hashtable);
    *node_ref           = node;

    return node_ref;
}

overload hashtable *create_table(hash_function proto, struct hashtable_optimizations optimizations) {
    auto table           = create_table(proto);
    table->optimizations = optimizations;
    return table;
}

flatten void add_recent_access(hashtable *table, hashnode node) {
    auto ref = &table->recent_access_nodes[ table->current_recent_access = (table->current_recent_access + 1) % 16 ];
    if (!table->optimizations.no_hash) ref->hash = node.hash;
    ref->key   = node.key;
    ref->value = node.value;
}

// TODO: Improve performance

// Sets a value in the hashtable.
void set_hashtable(hashtable *table, ctring key, car value) {
    hashnode *node = obj(hashnode);
    node->hash     = table->proto(key);
    node->key      = key;
    node->value    = value;

    // skip collision detection if optimizations are enabled
    if (table->optimizations.no_hash || table->optimizations.no_collision_checks
        || table->optimizations.collision_character_check_count > 0)
        goto FINISH;

    // maybe there's a better option than checking for collisions all the time?

    table->use_hash = true;

    if (table->optimizations.no_collision_checks) goto FINISH;

    // check for collisions in the recent access
    for (u8 i = 0; i < 16; i++) {
        if (table->recent_access_nodes[ i ].hash == 0 && table->recent_access_nodes[ i ].key == NULL) continue;
        if (table->recent_access_nodes[ i ].hash != node->hash) continue;
        // collision detected, disable usage of hashes
        table->use_hash = false;
        goto FINISH;
    }

    if (table->optimizations.only_recent_collision_checks) goto FINISH;

    // check for collisions in the hash table, maybe slow?
    // improve the speed on this!!
    auto c_head = table->entries->head;
    auto c_tail = table->entries->tail;
    for (; c_head != NULL || c_tail != NULL;) {
        if (c_head) {
            auto head = (hashnode *) c_head->data;
            if (head->hash == node->hash) { // collision detected on head.
                table->use_hash = false;
                goto FINISH;
            }
        }

        if (c_tail) {
            auto tail = (hashnode *) c_tail->data;
            if (tail->hash == node->hash) { // collision detected on tail.
                table->use_hash = false;
                goto FINISH;
            }
        }

        if (c_head) c_head = c_head->front[ 0 ];
        if (c_tail) c_tail = c_tail->back[ 0 ];
    }

FINISH:
    push_linkedlist(table->entries, node);
    // add_recent_access(table, *node);
}

overload car get_hashtable(hashtable *table, u8 hash, ctring key);

// Gets a value from the hashtable using just a key. This approach is not very performant if hashing has been disabled.
overload car get_hashtable(hashtable *table, ctring key) {
    if (table->use_hash && !table->optimizations.no_hash) return get_hashtable(table, table->proto(key), key);

    for (byte i = 0; i < 16; i++) {
        if (table->recent_access_nodes[ i ].hash == 0 && table->recent_access_nodes[ i ].key == NULL) continue;
        if (table->recent_access_nodes[ i ].key == key) return table->recent_access_nodes[ i ].value;
        if (eq(table->recent_access_nodes[ i ].key, key)) return table->recent_access_nodes[ i ].value;
    }

    auto c_head = table->entries->head;
    auto c_tail = table->entries->tail;

    for (; c_head != NULL || c_tail != NULL;) {
        if (c_head) {
            hashnode *head = c_head->data;
            if (head->key != key) goto LAST;
            if (!eq(head->key, key)) goto LAST;
            add_recent_access(table, *(hashnode *) head);
            return head->value;
        }

        if (c_tail) {
            hashnode *tail = c_tail->data;
            if (tail->key != key) goto LAST;
            if (!eq(tail->key, key)) goto LAST;
            add_recent_access(table, *(hashnode *) tail);
            return tail->value;
        }

    LAST:

        if (c_head) c_head = c_head->front[ 0 ];
        if (c_tail) c_tail = c_tail->back[ 0 ];
    }

    return NULL;
}

// Gets a value from the hashtable using a key and a hash. The key is not typically used unless optimization checks are enabled.
overload car get_hashtable(hashtable *table, u8 hash, ctring key) {
    if (!table->use_hash || table->optimizations.no_hash) return get_hashtable(table, key);

    bool        matched        = false;
    linkednode *best_candidate = NULL;

    for (byte i = 0; i < 16; i++) {
        auto node = table->recent_access_nodes[ i ];
        if (node.hash == 0 && node.key == NULL) continue;
        if (node.hash != hash) continue;
        // Hash matches.
        if (table->optimizations.run_ptr_check && node.key == key) return node.value;
        // Check key characters.
        if (table->optimizations.collision_character_check_count
            && strncmp(node.key, key, table->optimizations.collision_character_check_count) == 0)
            return node.value;
        // No hash matches. Set best candidate.
        matched        = true;
        best_candidate = (var) node.value;
    }

    // Best candidate here acts as a holder for the value, not an actual linkednode.
    if (matched) return best_candidate;

    // Use hash
    auto c_head = table->entries->head;
    auto c_tail = table->entries->tail;

    for (; c_head != NULL || c_tail != NULL;) {
        // Bug: Investigate why this happens.
        if (likely(c_head) && unlikely(c_head == c_head->front[ 0 ])) c_head = NULL;
        if (likely(c_tail) && unlikely(c_tail == c_tail->back[ 0 ])) c_tail = NULL;

        if (c_head) {
            hashnode *head = c_head->data;
            if (!head) goto CONT1;
            if (head->hash == hash) { // Check optimization flags.
                // Hash matches and the pointer check succeeded. Return the candidate immediately.
                if (table->optimizations.run_ptr_check && head->key == key) {
                    add_recent_access(table, *(hashnode *) head);
                    return head->value;
                } // Check key characters.
                if (table->optimizations.collision_character_check_count
                    && strncmp(head->key, key, table->optimizations.collision_character_check_count) == 0) {
                    add_recent_access(table, *(hashnode *) head);
                    return head->value;
                }
                // Hash matches? If so, set it as the best candidate.
                best_candidate = c_head;
                goto CONT1;
            }
        }

    CONT1:

        if (c_tail) {
            hashnode *tail = c_tail->data;
            if (!tail) goto CONT2;
            if (tail->hash == hash) { // Check optimization flags.
                // Hash matches and the pointer check succeeded. Return the candidate immediately.
                if (table->optimizations.run_ptr_check && tail->key == key) {
                    add_recent_access(table, *(hashnode *) tail);
                    return tail->value;
                }
                // Check key characters.
                if (table->optimizations.collision_character_check_count
                    && strncmp(tail->key, key, table->optimizations.collision_character_check_count) == 0) {
                    add_recent_access(table, *(hashnode *) tail);
                    return tail->value;
                }
                // Hash matches? If so, set it as the best candidate.
                best_candidate = c_tail;
                goto CONT2;
            }
        }

    CONT2:

        if (c_head) c_head = c_head->front[ 0 ];
        if (c_tail) c_tail = c_tail->back[ 0 ];
    }

    if (best_candidate) front_insert(table->entries, best_candidate);
    else { return NULL; }
    add_recent_access(table, *(hashnode *) best_candidate->data);

    return ((hashnode *) best_candidate->data)->value;
}

string *environ;

#ifdef __MOONSHINE_PROFILER
extern void __llvm_profile_reset_counters(void);
extern int  __llvm_profile_write_file(void);
extern void __llvm_profile_set_filename(const char *name);
#endif

[[noreturn]] __attribute__((used)) void __moonshine_start(int argc, string *argv, string *envp) {
#ifdef __MOONSHINE_PROFILER
    static const char profile_filename[] __attribute__((used)) = "profile.profraw";
    __llvm_profile_reset_counters();
    __llvm_profile_set_filename(profile_filename);
#endif

    global_page_table = PageTable();
    environ           = envp;

    int exit_code;

    __asm__ __volatile__("mov %1, %%rdi      \n"
                         "mov %2, %%rsi      \n"
                         "call main          \n"
                         "mov %%eax, %0      \n"
                         : "=r"(exit_code)
                         : "r"((long) argc), "r"(argv)
                         : "rdi", "rsi", "rax", "memory");

#ifdef __MOONSHINE_PROFILER
    extern int __llvm_profile_write_file(void);
    __llvm_profile_write_file();
#endif

    exit(exit_code);
    __builtin_unreachable();
}

__attribute__((force_align_arg_pointer)) __attribute__((naked)) void _start(void) {
    __asm__ volatile("mov (%rsp), %rdi \n"
                     "lea 8(%rsp), %rsi \n"
                     "mov %rdi, %rax \n"
                     "lea (,%rax,8), %rcx \n"
                     "add $8, %rcx \n"
                     "lea (%rsp, %rcx), %rdx \n"
                     "call __moonshine_start \n"
                     "mov $60, %rax \n"
                     "xor %rdi, %rdi \n"
                     "syscall \n"
                     "hlt \n");
}

// TODO: Check for errors

typedef long ssize_t;
typedef long off_t;

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR   2
#define O_CREAT  64
#define O_TRUNC  512
#define O_APPEND 1024

#define DEFAULT_PERMISSIONS 0666

#define AT_FDCWD (-100)

#define SYS_read   0
#define SYS_write  1
#define SYS_close  3
#define SYS_lseek  8
#define SYS_openat 257

typedef struct FILE {
    int fd;
} FILE;

FILE  *fopen(const char *pathname, const char *mode);
size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
int    fclose(FILE *stream);
void   rewind(FILE *stream);
int    fseek(FILE *stream, long offset, int whence);
size_t fread(void *ptr, size_t size, size_t count, FILE *stream);
long   ftell(FILE *stream);

FILE *fopen(const char *pathname, const char *mode) {
    int flags = 0;
    int perms = DEFAULT_PERMISSIONS;

    if (!pathname || !mode) return 0;

    if (mode[ 0 ] == 'r') {
        if (mode[ 1 ] == '+') flags = O_RDWR;
        else
            flags = O_RDONLY;
    } else if (mode[ 0 ] == 'w') {
        if (mode[ 1 ] == '+') flags = O_RDWR | O_CREAT | O_TRUNC;
        else
            flags = O_WRONLY | O_CREAT | O_TRUNC;
    } else if (mode[ 0 ] == 'a') {
        if (mode[ 1 ] == '+') flags = O_RDWR | O_CREAT | O_APPEND;
        else
            flags = O_WRONLY | O_CREAT | O_APPEND;
    } else {
        return 0;
    }

    int fd = syscall5(SYS_openat, AT_FDCWD, pathname, flags, perms);
    if (fd < 0) return 0;

#ifdef __MOONSHINE_PROFILER
    FILE *file = (FILE *) __bare_alloc(sizeof(FILE));
#else
    FILE *file = (FILE *) alloc(sizeof(FILE));
#endif
    if (!file) {
        (void) syscall2(SYS_close, fd);
        return 0;
    }
    file->fd = fd;
    return file;
}

size_t fwrite(const void *ptr, size_t size, size_t count, FILE *stream) {
    size_t      total         = size * count;
    size_t      total_written = 0;
    const char *buf           = (const char *) ptr;
    while (total_written < total) {
        long ret = syscall4(SYS_write, stream->fd, (void *) (buf + total_written), total - total_written);
        if (ret <= 0) break;
        total_written += ret;
    }
    return total_written / size;
}

int fclose(FILE *stream) {
    if (!stream) return -1;
    int ret = syscall2(SYS_close, stream->fd);
#ifdef __MOONSHINE_PROFILER
    __bare_munmap((var) stream, sizeof(FILE));
#else
    release((var) stream);
#endif
    return (ret < 0) ? -1 : 0;
}

int fseek(FILE *stream, long offset, int whence) {
    long ret = syscall4(SYS_lseek, stream->fd, offset, whence);
    return (ret < 0) ? -1 : 0;
}

void rewind(FILE *stream) { fseek(stream, 0, SEEK_SET); }

size_t fread(void *ptr, size_t size, size_t count, FILE *stream) {
    size_t total      = size * count;
    size_t total_read = 0;
    char  *buf        = (char *) ptr;
    while (total_read < total) {
        long ret = syscall4(SYS_read, stream->fd, (void *) (buf + total_read), total - total_read);
        if (ret <= 0) break;
        total_read += ret;
    }
    return total_read / size;
}

long ftell(FILE *stream) { return syscall4(SYS_lseek, stream->fd, 0, SEEK_CUR); }

#ifdef __MOONSHINE_PROFILER
void *__dso_handle = (void *) 0;
int   atexit(void (*func)(void)) {
    func(); // Call immediately since we don’t buffer exits
    return 0;
}
#endif

#endif
