// Compile any program that uses this library with `-nostartfiles -nostdlib` :)
// This library probably only works with clang/llvm, so don't try gcc...

#ifndef moonshine_header
#define moonshine_header

#pragma clang diagnostic ignored "-Wincompatible-library-redeclaration"

//

#include "moonshine.expander.h"

#define fn_overload __attribute__((overloadable))
#define fn_line     fn_overload flatten
#define attr_unused __attribute__((unused))

#define NULL ((void *) 0)

#if defined __x86_64__ && !defined __ILP32__
    #define __WORDSIZE 64
#else
    #define __WORDSIZE                32
    #define __WORDSIZE32_SIZE_ULONG   0
    #define __WORDSIZE32_PTediFF_LONG 0
#endif

#define __WORDSIZE_TIME64_COMPAT32 1
typedef long unsigned int size_t;

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

// algebraic data type macros

#define exp(a, ...)                         a(__VA_ARGS__)
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
    _Pragma("unroll") for (auto name attr_unused = __VA_ARGS__, datatype_break = (typeof(name)) 0;                             \
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
#define clone(value)                                                                                                           \
    ({                                                                                                                         \
        auto _val = value;                                                                                                     \
        auto _ptr = alloc(sizeof(_val));                                                                                       \
        memcpy(_ptr, &_val, sizeof(_val));                                                                                     \
        _ptr;                                                                                                                  \
    })

#define t(type)             type **
#define new(a, ...)         ((a **) new_x(a __VA_OPT__(, ) __VA_ARGS__, new2, new1)(a __VA_OPT__(, ) __VA_ARGS__))
#define new_x(a, b, c, ...) c
#define new1(type)          (__new_array(#type, sizeof(type), 0))
#define new2(type, count)   (__new_array(#type, sizeof(type), count))
#define free_array(array)                                                                                                      \
    ({                                                                                                                         \
        release(cast_ptr(*array, byte, -addon_size));                                                                          \
        release(array);                                                                                                        \
    })

#define fill(a, ...)                                                                                                           \
    ({                                                                                                                         \
        typeof(a) __fill_values[] = { a __VA_OPT__(, ) __VA_ARGS__ };                                                          \
        u8        __fill_count    = sizeof(__fill_values) / sizeof(__fill_values[ 0 ]);                                        \
        auto      __fill_arr      = new (typeof(a), __fill_count);                                                             \
        for (u8 __i = 0; __i < __fill_count; __i++) { de(__fill_arr)[ __i ] = __fill_values[ __i ]; }                          \
        __fill_arr;                                                                                                            \
    })

#define single(...) fill(__VA_ARGS__)

#define capacity(array)                                                                                                        \
    ((u8) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u8, 2))
#define count(array) ((u8) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u8, 1))
#define element_size(array)                                                                                                    \
    (u8) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u8, 0)
#define last(array)  ((array)[ 0 ][ count(array) ])
#define first(array) get(array, 0)
#define safe_push(type, array, ptr, ...)                                                                                       \
    ({                                                                                                                         \
        type **_array = array;                                                                                                 \
        type _ptr     = ptr __VA_OPT__(, ) __VA_ARGS__;                                                                        \
        unsafe_push((var *) _array, (var) & (_ptr), sizeof(type));                                                             \
    })
#define push(array, ptr, ...) safe_push(typeof((array)[ 0 ][ 0 ]), array, ptr __VA_OPT__(, ) __VA_ARGS__)
void unsafe_push(var *array, const var ptr, const u8 ptr_size);
#define cast_index(arr, type, index) ((type *) arr)[ index ]
#define cast_ptr(arr, type, index)   (&cast_index(arr, type, index))

int errno = 0;

#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#define unless(x) while (!(x))
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

__attribute__((diagnose_as_builtin(__builtin_memcpy, 1, 2, 3))) void *memcpy(var _dest, car _src, const unsigned long n) {
    string dest = _dest;
    ctring src  = _src;

    for (u8 i = 0; i < n; i++) dest[ i ] = src[ i ];
    return dest;
}

#define stdin  0
#define stdout 1
#define stderr 2

#define BUFFER_SIZE 4096

char stdout_buffer[ BUFFER_SIZE ];
int  stdout_buffer_index = 0;

int flush(int fd) {
    if (stdout_buffer_index == 0) return 0;

    int total_written = 0;
    while (total_written < stdout_buffer_index) {
        int result = syscall4(1, fd, stdout_buffer + total_written, stdout_buffer_index - total_written);
        if (result < 0) return -1;
        total_written += result;
    }
    stdout_buffer_index = 0;
    return total_written;
}

int write(const char *ptr, const int size, const int fd) {
    if (size <= 0) return 0;

    if (fd == 1) {
        for (int i = 0; i < size; i++) {
            if(ptr[i] == 0) return flush(fd);
            stdout_buffer[ stdout_buffer_index++ ] = ptr[ i ];
            if (stdout_buffer_index == BUFFER_SIZE || ptr[ i ] == '\n') {
                int result = flush(fd);
                if (result < 0) return -1;
            }
        }
        return size;
    } else {
        int total_written = 0;
        while (total_written < size) {
            int result = syscall4(1, fd, (void *) (ptr + total_written), size - total_written);
            if (result < 0) { return -1; }
            total_written += result;
        }
        return total_written;
    }
}

void release(const var addr);

#define puts_size(value, size) write(value, size, stdout)
#define puts(value)                                                                                                            \
    ({                                                                                                                         \
        auto v = value;                                                                                                        \
        puts_size(v, strlen(v));                                                                                               \
    })
#define puts_static(value) puts_size(value, sizeof(value))

void puts_static_ptr(car value, u8 size) { puts_size(*(string *) value, size); }

#define format_local(extra, time, value)                                                                                       \
    ({                                                                                                                         \
        if (#value[ 0 ] == '"') {                                                                                              \
            auto obj = value;                                                                                                  \
            join_static_buf_buddy((var *) extra, (string *) &obj, sizeof(value) - 1);                                          \
        } else {                                                                                                               \
            formatter(extra, value, #value);                                                                                   \
        }                                                                                                                      \
    });

const var *__new_array(const string type_name, const u8 type_size, const u8 count);
#define format(...)                                                                                                            \
    ({                                                                                                                         \
        auto format_buffer = new (char);                                                                                       \
        EXPAND_general(format_local, format_buffer, __VA_ARGS__);                                                              \
        format_buffer;                                                                                                         \
    })

#define align_value(value)          ((u8) ((((value) + 15) & ~15ULL)))
#define align_custom(offset, value) ((u8) ((((value) + offset) & ~cat(offset, ULL))))
const int   magic_number = 0xB00E;
typedef int magic_type;
const int   __addon_size = 3 * sizeof(u8) + sizeof(string) + sizeof(magic_type);
const int   addon_size   = align_value(__addon_size);
const int   spacer_size  = addon_size - __addon_size;

void null_terminate(t(char) buf) {
    u8 cnt = count(buf);
    push(buf, '\0');
    cast_index(cast_ptr(*buf, byte, -addon_size), magic_type, 1) = cnt;
}

#define print(...)                                                                                                             \
    ({                                                                                                                         \
        auto buf = format(__VA_ARGS__);                                                                                        \
        null_terminate(buf);                                                                                                   \
        puts_size(buf[ 0 ], count(buf));                                                                                       \
        free_array(buf);                                                                                                       \
    })
#define println(...)                                                                                                           \
    ({                                                                                                                         \
        print(__VA_ARGS__);                                                                                                    \
        putchar('\n');                                                                                                         \
        flush(stdout);                                                                                                         \
    })

void join_static_buf(var *array, string ptr, u8 size);
#define join_text(buf, txt)                                                                                                    \
    ({                                                                                                                         \
        char x[] = "" txt;                                                                                                     \
        join_static_buf((var *) buf, x, sizeof(x));                                                                            \
    })

void writeout_number(const int64_t number, const bool is_signed) {
    char    temp[ 20 ];
    int     len = 0;
    int64_t n   = number;

    if (is_signed && number < 0) {
        putchar('-');
        n = -n;
    }

    do {
        temp[ len++ ] = '0' + (n % 10);
        n /= 10;
    } while (n > 0);

    for (int i = len - 1; i >= 0; i--) putchar(temp[ i ]);
}

void puts_number(t(char) buf, const int64_t number, const bool is_signed) {
    char    temp[ 20 ];
    int     len = 0;
    int64_t n   = number;

    if (is_signed && number < 0) {
        push(buf, '-');
        n = -n;
    }

    do {
        temp[ len++ ] = '0' + (n % 10);
        n /= 10;
    } while (n > 0);

    for (int i = len - 1; i >= 0; i--) push(buf, temp[ i ]);
}

int sign_bit(float f) {
    u4 bits;
    memcpy(&bits, &f, sizeof(bits));
    return (bits >> 31) & 1;
}

bool is_infinity(float f) {
    u4 bits;
    memcpy(&bits, &f, sizeof(bits));
    return (bits & 0x7fffffff) == 0x7f800000;
}

bool is_nan(float f) {
    u4 bits;
    memcpy(&bits, &f, sizeof(bits));
    return (bits & 0x7f800000) == 0x7f800000 && (bits & 0x007fffff) != 0;
}

void puts_float(t(char) buf, const double number) {
    if (is_nan(number)) {
        join_text(buf, "nan");
        return;
    }
    if (is_infinity(number)) {
        if (sign_bit(number)) push(buf, '-');
        join_text(buf, "inf");
        return;
    }

    double num = number;

    bool is_negative = number < 0;
    if (is_negative) num = -num;

    int64_t int_part  = (int64_t) num;
    double  frac_part = num - (double) int_part;

    int64_t frac_scaled = (int64_t) (frac_part * 1000000.0 + 0.5);
    if (frac_scaled >= 1000000) {
        int_part++;
        frac_scaled = 0;
    }

    if (is_negative) push(buf, '-');

    puts_number(buf, int_part, false);

    if (frac_scaled == 0) return;

    char frac_digits[ 6 ];
    for (int i = 5; i >= 0; i--) {
        frac_digits[ i ] = (frac_scaled % 10) + '0';
        frac_scaled /= 10;
    }

    int last_non_zero = -1;
    for (int i = 5; i >= 0; i--) {
        if (frac_digits[ i ] != '0') {
            last_non_zero = i;
            break;
        }
    }

    if (last_non_zero >= 0) {
        push(buf, '.');
        for (int i = 0; i <= last_non_zero; i++) { push(buf, frac_digits[ i ]); }
    }
}

#define flatten __attribute__((flatten))

flatten void puts_hex(t(char) buf, const unsigned char hex) {
    const char hex_chars[] = "0123456789abcdef";
    push(buf, hex_chars[ hex >> 4 ]);
    push(buf, hex_chars[ hex & 0x0F ]);
}

flatten void puts_pointer(t(char) buf, const void *ptr) {
    join_text(buf, "0x");

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

    while (i > 0) { push(buf, hex_buffer[ --i ]); }
}

__attribute__((diagnose_as_builtin(__builtin_strlen, 1))) unsigned long strlen(ctring str) {
    unsigned long len = 0;
    while (str[ len ] != '\0') len++;
    return len;
}

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

    flush(stdout);
    (void) syscall2(syscall_number, exit_code);

    __builtin_unreachable();
}

int wait(const int *status) {
    const uint64_t syscall_number = 9;
    const int      result         = syscall3(syscall_number, -1, status);

    return result;
}

struct FoundPointer {
    i8                       page_index;
    i8                       allocation_size;
    struct AllocatedPointer *pointer_ref;
    struct Page             *page_ref;
    enum { ptr_owner_mmap, ptr_owner_sbrk } owner;
};

fn_line struct FoundPointer find_pointer(const var addr, bool throw_on_error);

bool mine(const ptr pointer);

enum human_readable_size { hrs_byte, hrs_kilobyte, hrs_megabyte, hrs_gigabyte };
string human_readable_size_names[] = { "byte", "kilobyte", "megabyte", "gigabyte" };

struct human_readable_size_layout {
    enum human_readable_size size;
    float                    count;
};

struct human_readable_size_layout calculate_human_readable_size(u8 size) {
    if (size < 1000) return (struct human_readable_size_layout) { .count = size, .size = hrs_byte };
    if (size < 1000000) {
        return (struct human_readable_size_layout) { .count = (float) ((u8) size / 10) / 100, .size = hrs_kilobyte };
    }
    if (size < 1000000000) {
        return (struct human_readable_size_layout) { .count = (float) ((u8) size / 10000) / 100, .size = hrs_megabyte };
    }
    return (struct human_readable_size_layout) { .count = (float) ((u8) size / 10000000) / 100, .size = hrs_gigabyte };
}

struct moonshine_float {
    u8   number;
    byte decimal;
};

void join_static_buf_buddy(var *buf, string *ptr, u8 size);

void formatter(t(char) buf, struct human_readable_size_layout x, ctring attr_unused text) {
    puts_float(buf, x.count);
    push(buf, ' ');
    join_static_buf((var) buf, human_readable_size_names[ x.size ], strlen(human_readable_size_names[ x.size ]));
    if (x.count != 1.0) push(buf, 's');
}
fn_line void formatter(t(char) buf, struct moonshine_float x, ctring attr_unused text) {
    u8   value = x.number;
    char buffer[ 20 ];
    byte index   = 20;
    byte index_1 = 0;

    while (value > 0) {
        buffer[ --index ] = value % 10;
        value /= 10;
    }

    for (byte i = index; i < 20; i++) {
        if (i == index && (20 - index) - x.decimal == 0) push(buf, '0');
        if (index_1++ == (20 - index) - x.decimal) push(buf, '.');
        push(buf, buffer[ i ] + '0');
    }
}
fn_line void formatter(t(char) buf, const char x, ctring attr_unused text) { push(buf, x); }
fn_line void formatter(t(char) buf, ctring x, ctring attr_unused text) { join_static_buf((var) buf, (string) x, strlen(x)); }
fn_line void formatter(t(char) buf, const int x, ctring text) {
    if (text[ 0 ] == '\'') push(buf, x);
    else { puts_number(buf, x, 1); }
}
fn_line void formatter(t(char) buf, const long x, ctring attr_unused text) { puts_number(buf, x, true); }
fn_line void formatter(t(char) buf, const unsigned long x, ctring attr_unused text) { puts_number(buf, x, false); }
fn_line void formatter(t(char) buf, const u4 x, ctring attr_unused text) { puts_number(buf, x, false); }
fn_line void formatter(t(char) buf, const short x, ctring attr_unused text) { puts_number(buf, x, false); }
fn_line void formatter(t(char) buf, const unsigned short x, ctring attr_unused text) { puts_number(buf, x, false); }
fn_line void formatter(t(char) buf, const float x, ctring attr_unused text) { puts_float(buf, x); }
fn_line void formatter(t(char) buf, const double x, ctring attr_unused text) { puts_float(buf, x); }
fn_line void formatter(t(char) buf, const bool x, ctring attr_unused text) {
    if (x) join_text(buf, "true");
    else if (x == 0) {
        join_text(buf, "false");
    } else {
        push(buf, x);
    }
}
fn_line void formatter(t(char) buf, var x, ctring attr_unused text) {
    goto UNKNOWN;

    if (mine(x)) {
        if (mine(*(var *) x)) {
            // handle array
            var array = *(var *) x;

            array = cast_ptr(array, byte, -addon_size);

            if (cast_index(array, magic_type, 0) != magic_number) goto UNKNOWN;
            array = cast_ptr(array, magic_type, 1);

            string array_type = cast_index(array, string, 0);

            array = cast_ptr(array, string, 1);

            u8 element_count = cast_index(array, u8, 1);

            join_text(buf, "<(");
            join_static_buf((var) buf, array_type, strlen(array_type));
            join_text(buf, ")_array[");
            puts_number(buf, element_count, false);
            join_text(buf, "]>");

            return;
        }
    }

UNKNOWN:;
    join_text(buf, "<pointer ");
    puts_pointer(buf, x);
    join_text(buf, " (");
    puts_number(buf, (u8) x, false);
    join_text(buf, ")>");
}
fn_line void formatter(t(char) buf, car x, ctring attr_unused text) { formatter(buf, (var) x, text); }

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

__attribute__((diagnose_as_builtin(__builtin_strcmp, 1, 2))) int strcmp(ctring left, ctring right) {
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

fn_line bool eq(ctring l, ctring r) { return strcmp(l, r) == 0; }
fn_line bool eq(t(char) l, t(char) r) { return strcmp(l[ 0 ], r[ 0 ]) == 0; }
fn_line bool eq(int l, int r) { return l == r; }

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

#define SYS_getpid 39
#define SYS_kill   62

#define SIGTRAP 5
#define SIGILL  4

#define getpid()       syscall1(SYS_getpid)
#define kill(pid, sig) syscall3(SYS_kill, pid, sig)
#define raise(sig)     kill(getpid(), sig)

#define breakpoint(str)                                                                                                        \
    ({                                                                                                                         \
        flush(stdout);                                                                                                         \
        (void) raise(SIGTRAP);                                                                                                 \
        const string wowie = "" str;                                                                                           \
        (void) wowie;                                                                                                          \
    })

#define fg_ansi(r, g, b) "\e[38;2;" #r ";" #g ";" #b "m"
#define bg_ansi(r, g, b) "\e[48;2;" #r ";" #g ";" #b "m"
#define no_ansi()        "\e[0m"

#define __MOONSHINE_EXCEPTION_DEBUGGER
string global_throw_message_format = NULL;
#ifdef __MOONSHINE_EXCEPTION_DEBUGGER
    #define throw(...)                                                                                                         \
        ({                                                                                                                     \
            if (global_throw_message_format != NULL) {                                                                         \
                puts(fg_ansi(255, 90, 90));                                                                                    \
                puts("Internal moonshine exception: ");                                                                        \
                puts(global_throw_message_format);                                                                             \
                puts(no_ansi());                                                                                               \
                putchar('\n');                                                                                                 \
                (void) raise(SIGILL);                                                                                          \
            }                                                                                                                  \
            global_throw_message_format = #__VA_ARGS__;                                                                        \
            println(fg_ansi(255, 90, 90) "Error: ", __VA_ARGS__, no_ansi());                                                   \
            puts_static(fg_ansi(235, 235, 50) "The following is an exception-related crash: " no_ansi());                      \
            (void) raise(SIGILL);                                                                                              \
            __builtin_unreachable();                                                                                           \
            0;                                                                                                                 \
        })
#else
    #define throw(...)                                                                                                         \
        ({                                                                                                                     \
            if (global_throw_message_format != NULL) {                                                                         \
                puts(fg_ansi(255, 90, 90));                                                                                    \
                puts("Internal moonshine exception: ");                                                                        \
                puts(global_throw_message_format);                                                                             \
                puts(no_ansi());                                                                                               \
                putchar('\n');                                                                                                 \
                exit(1);                                                                                                       \
            }                                                                                                                  \
            global_throw_message_format = #__VA_ARGS__;                                                                        \
            println(fg_ansi(255, 90, 90) "Error: ", __VA_ARGS__, no_ansi());                                                   \
            exit(1);                                                                                                           \
            __builtin_unreachable();                                                                                           \
            0;                                                                                                                 \
        })
#endif

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
    u8           pointer_count;
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
    ((struct PageTable) { 0, BASE_PAGE_TABLE_CAPACITY, __bare_alloc(sizeof(struct Page) * BASE_PAGE_TABLE_CAPACITY), 0 })

struct PageTable global_page_table;

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

struct FreeBlock mmapocator_find_free_space(struct Page *page) {
    if (!page->dirty) return page->last_result;

    struct FreeBlock max_free = { 0, 0 };
    if (page == NULL || page->pointers.size == 0) {
        max_free.start    = 0;
        max_free.size     = page->size;
        page->dirty       = false;
        page->last_result = max_free;
        return max_free;
    }

    struct AllocatedPointer sorted_pointers[ sizeof(struct AllocatedPointer) * page->pointers.size ];
    for (u8 i = 0; i < page->pointers.size; i++) { sorted_pointers[ i ] = page->pointers.data[ i ]; }

    qsort(sorted_pointers, page->pointers.size, sizeof(struct AllocatedPointer), compare_locations);

    u8 prev_end = 0;
    for (u8 i = 0; i < page->pointers.size; i++) {
        struct AllocatedPointer *block = &sorted_pointers[ i ];
        if (!block->allocated) continue;

        u8 block_start = block->location;
        u8 block_end   = block_start + block->size;

        if (block_start > prev_end) {
            u8 gap_size = block_start - prev_end;
            if (gap_size > max_free.size) {
                max_free.start = prev_end;
                max_free.size  = gap_size;
            }
        }
        if (block_end > prev_end) { prev_end = block_end; }
    }

    if (prev_end < page->size) {
        u8 gap_size = page->size - prev_end;
        if (gap_size > max_free.size) {
            max_free.start = prev_end;
            max_free.size  = gap_size;
        }
    }

    if (max_free.size > 0) {
        u8 aligned_start = (max_free.start + 15) & ~15ULL;
        u8 adjustment    = aligned_start - max_free.start;
        if (adjustment < max_free.size) {
            max_free.start = aligned_start;
            max_free.size -= adjustment;
        } else {
            max_free.size = 0; // No space after alignment
        }
    }

    page->dirty       = false;
    page->last_result = max_free;

    return max_free;
}

void mmapocator_clean_pages() {
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

#define max(a, b) ((a) > (b) ? (a) : (b))
#define min(a, b) ((a) < (b) ? (a) : (b))

// TODO: Place page pointer and pointer size behind the allocated pointer

void                                                                                  collect_garbage(bool debug);
__attribute__((diagnose_as_builtin(__builtin_malloc, 1))) __attribute__((malloc)) var mmapocator_alloc(const u8 len)
    [[clang::allocating]] {
    static int alloc_count = 0;
    if (unlikely(alloc_count++ == 4096)) {
        alloc_count = 0;
        collect_garbage(false);
        mmapocator_clean_pages();
    }

    i8               biggest_free_page = -1;
    struct FreeBlock max_biggest_free  = { 0, 0 };
    struct Page     *page;

    for (u8 i = 0; i < global_page_table.size; i++) {
        if (global_page_table.pages[ i ].start == NULL) continue;
        struct FreeBlock biggest_free = mmapocator_find_free_space(&global_page_table.pages[ i ]);

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

        global_page_table.pages[ place ] = Page(max(len, 4096));
        page                             = &global_page_table.pages[ place ];
        biggest_free_page                = place;
        max_biggest_free.size            = max(len, 4096);
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

    global_page_table.pointer_count++;
    return (var) new_location;
}

// Finds a pointer's reference in the page table.
struct FoundPointer mmapocator_find_pointer(const var addr, bool throw_on_error) {
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

            if (best_ptr == NULL) {
                if (!throw_on_error) {
                    return (struct FoundPointer) {
                        .page_index = -1, .page_ref = NULL, .pointer_ref = NULL, .allocation_size = -1, .owner = ptr_owner_mmap
                    };
                }
                throw("Pointer for location does not exist in page");
            }
            if (!best_ptr->allocated) {
                if (!throw_on_error) {
                    return (struct FoundPointer) {
                        .page_index = -1, .page_ref = NULL, .pointer_ref = NULL, .allocation_size = -1, .owner = ptr_owner_mmap
                    };
                }
                throw("Unallocated pointer? ", addr);
            }

            return (struct FoundPointer) { .allocation_size = best_ptr->size,
                                           .pointer_ref     = best_ptr,
                                           .page_ref        = page,
                                           .page_index      = i,
                                           .owner           = ptr_owner_mmap };
        }
    }

    if (!throw_on_error) {
        return (struct FoundPointer) {
            .page_index = -1, .page_ref = NULL, .pointer_ref = NULL, .allocation_size = -1, .owner = ptr_owner_mmap
        };
    }
    throw("Invalid pointer!");
}

void mmapocator_unmap(const struct FoundPointer ptr, const var addr) {
    if (!ptr.pointer_ref->allocated) throw("unmap double-free for pointer ", addr);

    ptr.pointer_ref->allocated = false;
    ptr.page_ref->dirty        = true;
    global_page_table.pointer_count--;
}

__attribute__((diagnose_as_builtin(__builtin_free, 1))) void mmapocator_release(const var addr) {
    if (addr == 0) return;
    mmapocator_unmap(mmapocator_find_pointer(addr, true), addr);
}

#define SYS_brk 12
var current_break = NULL;
var first_break   = NULL;

typedef __PTRDIFF_TYPE__ ptrdiff_t;

const u8        sbrk_increment = 4096;
fn_overload var sbrk(u8 increment) {
    if (current_break == NULL) {
        current_break = (var) syscall2(SYS_brk, 0);
        if (current_break == (var) -1) return (var) -1;
    }

    if (increment == 0) return current_break;
    if (increment > 0 && increment < sbrk_increment) increment = sbrk_increment;

    var new_break = (var) ((string) current_break + increment);

    long result = syscall2(SYS_brk, new_break);
    if (result == -1) return (var) -1;

    var old_break = current_break;
    current_break = new_break;
    return old_break;
}

auto                              sbrk_magic_number = 0x991010FC;
typedef typeof(sbrk_magic_number) sbrk_magic;

typedef struct block {
    sbrk_magic    magic;
    u8            size;
    var           ptr;
    bool          free;
    struct block *next;
    struct block *prev;
} sbrk_block;

sbrk_block *sbrk_block_list_head = NULL;
sbrk_block *sbrk_block_list_tail = NULL;

sbrk_block *sbrkocator_find_free(u8 size) {
    if (sbrk_block_list_head == NULL) return NULL;
    sbrk_block *cur = sbrk_block_list_head;

    while (cur != NULL) {
        if (cur->free && cur->size >= size) return cur;
        cur = cur->next;
    }

    return NULL;
}

var sbrkocator_alloc_new(u8 size) {
    sbrk_block *new_block = sbrk(size + sizeof(sbrk_block));

    // Out of sbrk memory.
    if (current_break < (var) new_block + size) return NULL;

    if (new_block == (sbrk_block *) -1) return NULL;
    new_block->magic = sbrk_magic_number;
    new_block->ptr   = (void *) (new_block + 1);
    new_block->size  = size;
    new_block->free  = false;
    new_block->next  = NULL;

    if (sbrk_block_list_head == NULL) {
        new_block->prev      = NULL;
        sbrk_block_list_head = new_block;
        sbrk_block_list_tail = new_block;
    } else {
        new_block->prev            = sbrk_block_list_tail;
        sbrk_block_list_tail->next = new_block;
        sbrk_block_list_tail       = new_block;
    }

    return (void *) (new_block + 1);
}

__attribute__((malloc)) var sbrkocator_alloc(u8 size) {
    auto free_block = sbrkocator_find_free(size);

    if (free_block == NULL) return sbrkocator_alloc_new(size);

    free_block->free = false;

    if (free_block->size > size + sizeof(sbrk_block)) {
        auto new_free_block   = (sbrk_block *) ((char *) free_block + sizeof(sbrk_block) + size);
        new_free_block->free  = true;
        new_free_block->ptr   = (void *) (new_free_block + 1);
        new_free_block->magic = sbrk_magic_number;
        new_free_block->size  = free_block->size - sizeof(sbrk_block) - size;
        new_free_block->prev  = free_block;
        new_free_block->next  = free_block->next;
        if (free_block->next) free_block->next->prev = new_free_block;
        free_block->next = new_free_block;
    }

    free_block->size = size;

    return (void *) (free_block + 1);
}

void sbrkocator_release(car ptr) {
    if (ptr == NULL) return;

    auto block_ptr = (sbrk_block *) ptr - 1;
    if (block_ptr->magic != sbrk_magic_number) return;
    if (block_ptr->free) return;

    block_ptr->free = true;

    if (block_ptr->prev && block_ptr->prev->free) {
        auto prev  = block_ptr->prev;
        prev->next = block_ptr->next;
        if (block_ptr->next) block_ptr->next->prev = prev;
        prev->size += block_ptr->size + sizeof(sbrk_block);
        block_ptr = prev;
    }

    if (block_ptr->next && block_ptr->next->free) {
        auto next       = block_ptr->next;
        block_ptr->next = next->next;
        if (next->next) next->next->prev = block_ptr;
        block_ptr->size += next->size + sizeof(sbrk_block);
    }
}

void sbrkocator_unmap(struct FoundPointer attr_unused ptr, car addr) { sbrkocator_release(addr); }

struct FoundPointer sbrkocator_find_pointer(car addr) {
    if (addr == NULL) {
        return (struct FoundPointer) {
            .page_index = -1, .allocation_size = -1, .pointer_ref = NULL, .page_ref = NULL, .owner = ptr_owner_sbrk
        };
    }

    if (addr < first_break || addr > current_break) {
        return (struct FoundPointer) {
            .page_index = -1, .allocation_size = -1, .pointer_ref = NULL, .page_ref = NULL, .owner = ptr_owner_sbrk
        };
    }

    sbrk_block *block = (sbrk_block *) addr - 1;

    if (block->magic != sbrk_magic_number) {
        return (struct FoundPointer) {
            .page_index = -1, .allocation_size = -1, .pointer_ref = NULL, .page_ref = NULL, .owner = ptr_owner_sbrk
        };
    }

    return (struct FoundPointer) { .page_index      = -1,
                                   .allocation_size = block->size,
                                   .pointer_ref     = (struct AllocatedPointer *) block,
                                   .page_ref        = NULL,
                                   .owner           = ptr_owner_sbrk };
}

const u8 MOONSHINE_ALLOCATOR_SWITCH_THRESHOLD = 1024 * 16;

var alloc(u8 size) {
    if (size > MOONSHINE_ALLOCATOR_SWITCH_THRESHOLD) {
        var mmap_ptr = mmapocator_alloc(size);
        if (mmap_ptr == NULL) goto SBRK_ALLOC;
        return mmap_ptr;
    }
SBRK_ALLOC:;
    var sbrk_ptr = sbrkocator_alloc(size);
    if (sbrk_ptr == NULL) return mmapocator_alloc(size);
    return sbrk_ptr;
}

void release(var ptr) {
    auto mmap_found = mmapocator_find_pointer(ptr, false);
    if (mmap_found.allocation_size != -1) return mmapocator_unmap(mmap_found, ptr);
    auto sbrk_found = sbrkocator_find_pointer(ptr);
    if (sbrk_found.allocation_size != -1) return sbrkocator_release(ptr);

    throw("Pointer was not owned by any of the moonshine allocators.");
}

void unmap(struct FoundPointer ptr, const var addr) {
    if (ptr.owner == ptr_owner_mmap) mmapocator_unmap(ptr, addr);
    sbrkocator_unmap(ptr, addr);
}

fn_line struct FoundPointer find_pointer(const var ptr, bool throw_on_error) {
    auto mmap_found = mmapocator_find_pointer(ptr, false);
    if (mmap_found.allocation_size != -1) return mmap_found;
    auto sbrk_found = sbrkocator_find_pointer(ptr);
    if (unlikely(!throw_on_error)) return sbrk_found;
    if (sbrk_found.allocation_size != -1) return sbrk_found;
    throw("Invalid pointer!");
}

enum GC_STATUS { GC_ALIVE, GC_DEAD };

__attribute__((diagnose_as_builtin(__builtin_realloc, 1, 2))) var remap(const var ptr, const u8 new_size) {
    const var new_ptr = alloc(new_size);
    if (ptr == 0) return new_ptr;
    const auto found = find_pointer(ptr, true);
    memcpy(new_ptr, ptr, found.allocation_size);
    unmap(found, ptr);
    return new_ptr;
}

// Check if the current process tree owns a pointer. Expensive, don't use it often.
bool mine(const ptr pointer) {
    if (pointer == NULL) return false;

    for (u8 i = 0; i < global_page_table.size; i++) {
        struct Page *page = &global_page_table.pages[ i ];
        if (page->start == NULL) continue;

        u8 page_start = (u8) page->start;
        u8 page_end   = page_start + page->size;

        if ((u8) pointer < page_start || (u8) pointer >= page_end) continue;

        for (u8 j = 0; j < page->pointers.size; j++) {
            struct AllocatedPointer *alloc = &page->pointers.data[ j ];
            if (!alloc->allocated) continue;

            u8 alloc_start = page_start + alloc->location;
            u8 alloc_end   = alloc_start + alloc->size;

            if ((u8) pointer >= alloc_start && (u8) pointer < alloc_end) return true;
        }
    }

    sbrk_block *block = sbrk_block_list_head;
    while (block != NULL) {
        if (!block->free && block->magic == sbrk_magic_number) {
            u8 block_start = (u8) block->ptr;
            u8 block_end   = block_start + block->size;

            if ((u8) pointer >= block_start && (u8) pointer < block_end) return true;
        }
        block = block->next;
    }

    return false;
}

u8 start_of_stack;

// Basic Mark & Sweep garbage collector. Slow, use with caution.
void collect_garbage(bool debug) {
    u8 end_of_stack = (u8) &debug;
    u8 start        = min(start_of_stack, end_of_stack);
    u8 end          = max(start_of_stack, end_of_stack);

    struct GC_Ptr {
        enum GC_STATUS status;
        var            ptr;
        u8             size;
        bool           is_sbrk;
    };

    u8          total_pointers = global_page_table.pointer_count;
    sbrk_block *block          = sbrk_block_list_head;
    while (block != NULL) {
        if (!block->free) total_pointers++;
        block = block->next;
    }

    struct GC_Ptr marks[ total_pointers ];
    memset(marks, 0, total_pointers * sizeof(struct GC_Ptr));
    u8 marks_index = 0;

    struct GC_Page_Range {
        var start_location;
        var end_location;
        u8  marks_position;
        u8  marks_end;
    } page_ranges[ global_page_table.size ];
    u8 ranges_index = 0;

    u8 min_page_start = ~(u8) NULL;
    u8 max_page_end   = 0;

    for (u8 page_index = 0; page_index < global_page_table.size; page_index++) {
        auto page = global_page_table.pages[ page_index ];
        if (page.start == NULL) continue;

        if ((u8) page.start < min_page_start) min_page_start = (u8) page.start;
        if ((u8) (page.start + page.size) > max_page_end) max_page_end = (u8) page.start + page.size;

        u8 marks_start = marks_index;
        for (u8 ptr_index = 0; ptr_index < page.pointers.size; ptr_index++) {
            auto ptr = page.pointers.data[ ptr_index ];
            if (!ptr.allocated) continue;
            marks[ marks_index++ ]
                = (struct GC_Ptr) { .ptr = page.start + ptr.location, .size = ptr.size, .status = GC_DEAD, .is_sbrk = false };
        }
        page_ranges[ ranges_index++ ] = (struct GC_Page_Range) { .start_location = page.start,
                                                                 .end_location   = page.start + page.size,
                                                                 .marks_position = marks_start,
                                                                 .marks_end      = marks_index };
    }

    u8 sbrk_min = (u8) first_break;
    u8 sbrk_max = (u8) current_break;
    block       = sbrk_block_list_head;
    while (block != NULL) {
        if (!block->free) {
            marks[ marks_index++ ]
                = (struct GC_Ptr) { .ptr = block->ptr, .size = block->size, .status = GC_DEAD, .is_sbrk = true };
        }
        block = block->next;
    }

    for (u8 st = start; st < end; st += sizeof(var)) {
        auto ptr   = (var) st;
        auto value = *(u8 *) ptr;

        if (value < min_page_start || value > max_page_end) {
            if (sbrk_min <= value && value <= sbrk_max) goto CHECK_SBRK;
            continue;
        }

        for (u8 page_idx = 0; page_idx < ranges_index; page_idx++) {
            auto page = page_ranges[ page_idx ];
            if (value < (u8) page.start_location || value > (u8) page.end_location) continue;

            for (u8 marks_idx = page.marks_position; marks_idx < page.marks_end; marks_idx++) {
                auto mark = marks[ marks_idx ];
                if ((var) value < mark.ptr || (var) value >= (mark.ptr + mark.size)) continue;
                marks[ marks_idx ].status = GC_ALIVE;
                goto TOP_LEVEL_CONTINUE;
            }
        }

    CHECK_SBRK:
        block = sbrk_block_list_head;
        while (block != NULL) {
            if (!block->free && value >= (u8) block->ptr && value < (u8) (block->ptr + block->size)) {
                for (u8 m = 0; m < marks_index; m++) {
                    if (marks[ m ].ptr == block->ptr && marks[ m ].is_sbrk) {
                        marks[ m ].status = GC_ALIVE;
                        goto TOP_LEVEL_CONTINUE;
                    }
                }
            }
            block = block->next;
        }

    TOP_LEVEL_CONTINUE:;
    }

    bool all_marked = false;
    unless(all_marked) {
        all_marked = true;
        for (u8 ptr_idx = 0; ptr_idx < marks_index; ptr_idx++) {
            if (marks[ ptr_idx ].status == GC_DEAD) continue;

            auto ptr = marks[ ptr_idx ];
            for (u8 ptr_var = 0; ptr_var < ptr.size; ptr_var += sizeof(var)) {
                u8 *pointer = ptr.ptr + ptr_var;
                u8  value   = *(u8 *) pointer;

                if (value < min_page_start || value > max_page_end) {
                    if (sbrk_min <= value && value <= sbrk_max) goto CHECK_SBRK_HEAP;
                    continue;
                }

                for (u8 page_idx = 0; page_idx < ranges_index; page_idx++) {
                    auto page = page_ranges[ page_idx ];
                    if (value < (u8) page.start_location || value > (u8) page.end_location) continue;

                    for (u8 marks_idx = page.marks_position; marks_idx < page.marks_end; marks_idx++) {
                        auto mark = marks[ marks_idx ];
                        if ((var) value < mark.ptr || (var) value >= (mark.ptr + mark.size)) continue;
                        if (mark.status != GC_ALIVE) {
                            marks[ marks_idx ].status = GC_ALIVE;
                            all_marked                = false;
                        }
                        goto NEXT_CONTINUE;
                    }
                }

            CHECK_SBRK_HEAP:
                block = sbrk_block_list_head;
                while (block != NULL) {
                    if (!block->free && value >= (u8) block->ptr && value < (u8) (block->ptr + block->size)) {
                        for (u8 m = 0; m < marks_index; m++) {
                            if (marks[ m ].ptr == block->ptr && marks[ m ].is_sbrk && marks[ m ].status != GC_ALIVE) {
                                marks[ m ].status = GC_ALIVE;
                                all_marked        = false;
                            }
                        }
                    }
                    block = block->next;
                }

            NEXT_CONTINUE:;
            }
        }
    }

    u8 total_saved = 0;
    for (u8 m = 0; m < marks_index; m++) {
        auto mark = marks[ m ];
        if (mark.status == GC_DEAD) {
            if (mark.is_sbrk) {
                sbrkocator_release(mark.ptr);
            } else {
                auto found = mmapocator_find_pointer(mark.ptr, false);
                if (found.allocation_size != -1) mmapocator_unmap(found, mark.ptr);
            }
            total_saved += mark.size;
        }
    }

    if (debug) println("[GC] Saved ", calculate_human_readable_size(total_saved), " of memory");
}

#define de(obj) (*(obj))

const var *__new_array(const string type_name, const u8 type_size, const u8 count) [[clang::allocating]] {
    // string name, and one place for the special code
    const u8 size = type_size * count + addon_size;

    const var base = alloc(size);
    var       ptr  = base;

    cast_index(ptr, magic_type, 0) = magic_number; // magic number for arrays
    ptr                            = cast_ptr(ptr, magic_type, 1);

    cast_index(ptr, string, 0) = type_name;
    ptr                        = cast_ptr(ptr, string, 1);

    cast_index(ptr, u8, 0) = type_size;
    cast_index(ptr, u8, 1) = count;
    cast_index(ptr, u8, 2) = align_custom(63, count); // Align the capacity to 64

    var *holder = alloc(sizeof(var));

    *holder = cast_ptr(base, byte, addon_size);

    return holder;
}

// TODO: Add a demand level for the capacity checker, to modify the alignment amount the more the array increases in size.

flatten void unsafe_extend(
    var      *array_ref,
    const var array_from_u8,
    const var array_base,
    const u8  element_count,
    const u8  element_size,
    const u8  offset) [[clang::allocating]] {
    cast_index(array_from_u8, u8, 1) += offset;
    if (element_count + offset >= capacity(array_ref)) {
        const var    new_base    = remap(array_base, addon_size + element_size * align_custom(63, element_count + offset + 1));
        const string array_place = &((string) new_base)[ addon_size ];
        *array_ref               = (var) array_place;
    }
}

void extend(var *array, u8 count) [[clang::allocating]] {
    const var main_wrap  = cast_ptr(*array, byte, -addon_size);
    var       array_wrap = main_wrap;

    if (cast_index(array_wrap, magic_type, 0) != magic_number) {
        throw("Invalid array magic number: ", cast_index(array_wrap, magic_type, 0), ", expected ", magic_number);
    }

    array_wrap = cast_ptr(array_wrap, magic_type, 1);
    array_wrap = cast_ptr(array_wrap, string, 1);

    const u8 element_size  = cast_index(array_wrap, u8, 0);
    const u8 element_count = cast_index(array_wrap, u8, 1);
    // const u8 capacity      = cast_index(array_wrap, u8, 2);

    unsafe_extend(array, array_wrap, main_wrap, align_custom(63, element_count + count + 1), element_size, count);
}

void unsafe_push(var *array, const var ptr, const u8 ptr_size) [[clang::allocating]] {
    const var main_wrap  = cast_ptr(*array, byte, -addon_size);
    var       array_wrap = main_wrap;

    if (cast_index(array_wrap, magic_type, 0) != magic_number) {
        // auto mnum = cast_index(array_wrap, magic_type, 0);
        throw("Invalid array magic number!");
    }

    array_wrap       = cast_ptr(array_wrap, magic_type, 1);
    string type_name = array_wrap;
    array_wrap       = cast_ptr(array_wrap, string, 1);

    const u8 element_count = cast_index(array_wrap, u8, 1);
    const u8 element_size  = cast_index(array_wrap, u8, 0);

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

void join_static_buf(var *array, string ptr, u8 size) {
    const var main_wrap  = cast_ptr(*array, byte, -addon_size);
    var       array_wrap = main_wrap;

    if (cast_index(array_wrap, magic_type, 0) != magic_number) {
        throw("Invalid array magic number: ", cast_index(array_wrap, magic_type, 0), ", expected ", magic_number);
    }

    array_wrap = cast_ptr(array_wrap, magic_type, 1);
    array_wrap = cast_ptr(array_wrap, string, 1);

    const u8 element_count = cast_index(array_wrap, u8, 1);

    if (element_count + size > capacity(array)) extend(array, size);

    for (u8 i = 0; i < size; i++) ((t(char)) array)[ 0 ][ element_count + i ] = ptr[ i ];

    cast_index(array_wrap, u8, 1) += size;
}

void join_static_buf_buddy(var *array, string *ptr, u8 size) { join_static_buf(array, ptr[ 0 ], size); }

var return_item(var **arr, i8 index) {
    if (unlikely((u8) index >= count(arr))) {
        throw("Requested index in array (", index, ") exceeds the size of the array! (", count(arr), ")");
    }
    if (unlikely(index < 0)) throw("Cannot access negative offset ", index, " in array!");

    return &((byte **) arr)[ 0 ][ index * element_size(arr) ];
}

var return_item_unbound(var **arr, i8 index) { return &((byte **) arr)[ 0 ][ index * element_size(arr) ]; }

void set_item(var **arr, i8 index, var value) {
    if (unlikely((u8) index >= count(arr))) {
        throw("Target index in array (", index, ") exceeds the size of the array! (", count(arr), ")");
    }
    if (unlikely(index < 0)) throw("Cannot write to negative offset ", index, " in array!");

    memcpy(&((byte **) arr)[ 0 ][ index * element_size(arr) ], value, element_size(arr));
}

#define branchless(c, a, b) !!c * a + !!!c * b

#define address_item(__array, __index) (typeof(__array[ 0 ])) return_item((var **) __array, (u8) __index)
#define get(__array, __index)          (*(typeof(__array[ 0 ])) return_item((var **) __array, (u8) __index))
#define get_unbound(__array, __index)  (*(typeof(__array[ 0 ])) return_item_unbound((var **) __array, (u8) __index))
#define set(__array, __index, __value)                                                                                         \
    ({                                                                                                                         \
        typeof(__array[ 0 ][ 0 ]) v = __value;                                                                                 \
        set_item((var **) __array, __index, &v);                                                                               \
    })

#define in                         ,
#define foreach(...)               foreach_inner(__VA_ARGS__)
#define foreach_inner(item, array) foreach_inner_2(item, cat(item, _index), array)
#define foreach_inner_2(item, index, array)                                                                                    \
    scope(                                                                                                                     \
        index, (var) 0) for (auto attr_unused item = get_unbound(array, (u8) 0); (u8) index < count(array);                    \
                             item                  = get_unbound(                                                              \
                                 array, (u8) index == count(array) - 1 ? count(array) - 1 + 0 * (u8) ++index : (u8) ++index))

#define map(tuple, ...) exp(map__inner, expand tuple, __VA_ARGS__)
#define map__inner(out_type, array, item, ...)                                                                                 \
    ({                                                                                                                         \
        auto __original_array      = array;                                                                                    \
        t(out_type) __result_array = new (out_type, count(array));                                                             \
        foreach (item in __original_array) {                                                                                   \
            out_type result = __VA_ARGS__;                                                                                     \
            set(__result_array, (u8) cat(item, _index), result);                                                               \
        }                                                                                                                      \
        __result_array;                                                                                                        \
    })
#define reduce(array, tuple, ...) exp(reduce__inner, array, expand tuple, __VA_ARGS__)
#define reduce__inner(array, initial, item, ...)                                                                               \
    ({                                                                                                                         \
        auto __original_array = array;                                                                                         \
        auto output           = initial;                                                                                       \
        foreach (item in __original_array) { output = __VA_ARGS__; };                                                          \
        output;                                                                                                                \
    })

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
    const u8 element_size = element_size(array);
    const u8 count_val    = count(array);
    if (count_val <= 1) return array;
    byte temp[ element_size ];

    for (u8 i = 0; i < count_val / 2; i++) {
        u8 left_idx  = i;
        u8 right_idx = count_val - 1 - i;

        memcpy(temp, address_item(array, left_idx), element_size);
        memcpy(address_item(array, left_idx), address_item(array, right_idx), element_size);
        memcpy(address_item(array, right_idx), temp, element_size);
    }

    return array;
}

#define reverse_array(...) (typeof(__VA_ARGS__)) __reverse_array((var **) __VA_ARGS__)

t(char) strndup(ctring str, u8 len) {
    const auto obj = new (char, len + 1);

    for (u8 i = 0; i < len; i++) obj[ 0 ][ i ] = str[ i ];

    obj[ 0 ][ len ] = 0;

    return obj;
}

typedef struct linkednode linkednode;

// A typical node in a linked list.
struct linkednode {
    linkednode *back[ 2 ];  // Double-index back-refernece for the node
    linkednode *front[ 2 ]; // Double-index forward-feference for the node
    var         data;       // The data for the linked list node
};

typedef struct linkedlist {
    u8          size; // The size of a linked list
    linkednode *head; // The head pointer for the linked list
    linkednode *tail; // The tail pointer for the linked list
} linkedlist;

// Create an empty linked list.
flatten linkedlist *create_linkedlist() {
    linkedlist *list = obj(linkedlist);
    list->size       = 0;
    list->head       = NULL;
    list->tail       = NULL;

    return list;
}

// Push a value to the end of a linked list.
linkednode *push_linkedlist(linkedlist *list, var value) {
    linkednode *node = obj(linkednode);

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

    return node;
}

// Push a value to the front of a linked list.
linkednode *push_front_linkedlist(linkedlist *list, var value) {
    linkednode *node = obj(linkednode);

    node->data = value;

    auto next        = list->head;
    auto double_next = list->head == NULL ? NULL : list->head->front[ 0 ];

    node->front[ 0 ] = next;
    node->front[ 1 ] = double_next;
    node->back[ 0 ] = node->back[ 1 ] = NULL;

    if (next) {
        next->back[ 0 ] = node;
        if (double_next) double_next->back[ 1 ] = node;
    }

    list->head = node;
    if (!list->tail) list->tail = node;

    return node;
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
fn_overload var pop_item(linkedlist *list, bool free, linkednode *item) {
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
        return ptr;
    } else {
        return item;
    }
}

// This function will return a pointer to the linkednode, if free = false, and a pointer to the data contained inside, if free =
// true.
fn_line var pop_item(linkedlist *list, bool free, u8 index) { return pop_item(list, free, index_to_ptr(list, index)); }

// This function removes a node from the linked list, and then places it at the start of the list.
fn_overload void front_insert(linkedlist *list, linkednode *node) {
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
fn_line void front_insert(linkedlist *list, u8 index) { return front_insert(list, index_to_ptr(list, index)); }

linkedlist *clone_linkedlist(linkedlist *original) {
    if (original == NULL) { return NULL; }

    linkedlist *new_list = create_linkedlist();
    new_list->size       = original->size;

    linkednode *current = original->head;
    while (current != NULL) {
        push_linkedlist(new_list, current->data);
        current = current->front[ 0 ];
    }

    return new_list;
}

void free_linkedlist(linkedlist *list) {
    if (list == NULL) return;

    linkednode *current = list->head;
    while (current != NULL) {
        linkednode *next = current->front[ 0 ];
        release(current);
        current = next;
    }

    release(list);
}

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

#define LOAD_FACTOR_THRESHOLD 0.7

typedef enum entry_state { EMPTY, OCCUPIED, TOMBSTONE } entry_state;

typedef struct hashentry {
    entry_state state;
    ctring      key;
    car         value;
} hashentry;

typedef struct hasharray {
    hash_function proto;
    u8            size;
    u8            capacity;
    hashentry    *entries;
} hasharray;

hasharray *create_hasharray(hash_function proto) {
    hasharray *ha = obj(hasharray);
    ha->proto     = proto;
    ha->size      = 0;
    ha->capacity  = 16;
    ha->entries   = (hashentry *) alloc(sizeof(hashentry) * ha->capacity);
    for (u8 i = 0; i < ha->capacity; i++) {
        ha->entries[ i ].state = EMPTY;
        ha->entries[ i ].key   = NULL;
        ha->entries[ i ].value = NULL;
    }
    return ha;
}

void resize_hasharray(hasharray *ha) {
    u8         new_capacity = ha->capacity * 2;
    hashentry *new_entries  = (hashentry *) alloc(sizeof(hashentry) * new_capacity);
    for (u8 i = 0; i < new_capacity; i++) {
        new_entries[ i ].state = EMPTY;
        new_entries[ i ].key   = NULL;
        new_entries[ i ].value = NULL;
    }

    for (u8 i = 0; i < ha->capacity; i++) {
        if (ha->entries[ i ].state == OCCUPIED) {
            ctring key   = ha->entries[ i ].key;
            car    value = ha->entries[ i ].value;
            u8     hash  = ha->proto(key);
            u8     index = hash % new_capacity;
            while (new_entries[ index ].state == OCCUPIED) { index = (index + 1) % new_capacity; }
            new_entries[ index ].state = OCCUPIED;
            new_entries[ index ].key   = key;
            new_entries[ index ].value = value;
        }
    }

    release(ha->entries);
    ha->entries  = new_entries;
    ha->capacity = new_capacity;
}

void set_hasharray(hasharray *ha, ctring key, car value) {
    if (key == NULL) throw("Cannot insert NULL key into hasharray");
    if (ha->size >= (u8) (ha->capacity * LOAD_FACTOR_THRESHOLD)) { resize_hasharray(ha); }

    u8 hash            = ha->proto(key);
    u8 index           = hash % ha->capacity;
    u8 original_index  = index;
    u8 first_tombstone = -1;

    while (true) {
        hashentry *entry = &ha->entries[ index ];
        if (entry->state == EMPTY) {
            u8         insert_index = (first_tombstone != (u8) -1) ? first_tombstone : index;
            hashentry *insert_entry = &ha->entries[ insert_index ];
            insert_entry->state     = OCCUPIED;
            insert_entry->key       = key;
            insert_entry->value     = value;
            ha->size++;
            return;
        } else if (entry->state == OCCUPIED && eq(entry->key, key)) {
            entry->value = value;
            return;
        } else if (entry->state == TOMBSTONE && first_tombstone == (u8) -1) {
            first_tombstone = index;
        }
        index = (index + 1) % ha->capacity;
        if (index == original_index) { throw("Hasharray is full!"); }
    }
}

car get_hasharray(hasharray *ha, ctring key) {
    if (key == NULL) { throw("Cannot retrieve NULL key from hasharray"); }
    u8 hash           = ha->proto(key);
    u8 index          = hash % ha->capacity;
    u8 original_index = index;

    while (true) {
        hashentry *entry = &ha->entries[ index ];
        if (entry->state == EMPTY) {
            return NULL;
        } else if (entry->state == OCCUPIED && eq(entry->key, key)) {
            return entry->value;
        }
        index = (index + 1) % ha->capacity;
        if (index == original_index) return NULL;
    }
}

void delete_hasharray(hasharray *ha, ctring key) {
    if (key == NULL) { throw("Cannot delete NULL key from hasharray"); }
    u8 hash           = ha->proto(key);
    u8 index          = hash % ha->capacity;
    u8 original_index = index;

    while (true) {
        hashentry *entry = &ha->entries[ index ];
        if (entry->state == EMPTY) return;
        else if (entry->state == OCCUPIED && eq(entry->key, key)) {
            entry->state = TOMBSTONE;
            ha->size--;
            return;
        }
        index = (index + 1) % ha->capacity;
        if (index == original_index) return;
    }
}

void free_hasharray(hasharray *ha) {
    release(ha->entries);
    release(ha);
}

hasharray *clone_hasharray(hasharray *original) {
    if (original == NULL) { return NULL; }

    hasharray *clone = obj(hasharray);
    if (clone == NULL) { throw("Failed to allocate memory for cloned hasharray"); }

    clone->proto    = original->proto;
    clone->size     = original->size;
    clone->capacity = original->capacity;

    clone->entries = (hashentry *) alloc(sizeof(hashentry) * clone->capacity);
    if (clone->entries == NULL) {
        release(clone);
        throw("Failed to allocate memory for cloned hasharray entries");
    }

    for (u8 i = 0; i < original->capacity; i++) {
        clone->entries[ i ].state = original->entries[ i ].state;
        clone->entries[ i ].key   = original->entries[ i ].key;
        clone->entries[ i ].value = original->entries[ i ].value;
    }

    return clone;
}

string *environ;

#ifdef __MOONSHINE_PROFILER
extern void __llvm_profile_reset_counters(void);
extern int  __llvm_profile_write_file(void);
extern void __llvm_profile_set_filename(const char *name);
#endif

[[noreturn]] __attribute__((used)) void __moonshine_start(int argc, string *argv, string *envp) {
    start_of_stack = (u8) &argc;
    current_break  = sbrk(0);
    first_break    = current_break;

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
                     "add $16, %rcx \n"
                     "lea (%rsp, %rcx), %rdx \n"
                     "call __moonshine_start \n"
                     "mov $60, %rax \n"
                     "xor %rdi, %rdi \n"
                     "syscall \n"
                     "hlt \n");
}

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
