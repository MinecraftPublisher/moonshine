#ifndef moonshine_header
#define moonshine_header

//

#include "moonshine.m4.h"

int errno = 0;

#define NULL ((void *) 0)
#define null ((void *) 0)

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
#define string char *
typedef void    *ptr;
typedef void    *var;
typedef uint64_t u8;
typedef int64_t  i8;
typedef int32_t  i4;
typedef uint32_t u4;
typedef uint8_t bool;

const bool true  = 1;
const bool false = 0;

#define auto __auto_type

enum glob_type {
    type_char,
    type_int,
    type_long,
    type_long_long,
    type_float,
    type_double,
    type_unsigned_char,
    type_unsigned_int,
    type_unsigned_long_long,
    type_string,
    type_bool,
    type_pointer,
    type_u32,
    type_u64,
    type_i64,
    type_i32,
    type_unknown
};

void globprint(const enum glob_type type, const char *value_text, const char *text, const void *value);

#define print_local(value)                                                                                                     \
    ({                                                                                                                         \
        __auto_type obj = value;                                                                                               \
        _Generic(                                                                                                              \
            obj,                                                                                                               \
            char: globprint(type_char, "", "char", &obj),                                                                      \
            int: globprint(type_int, #value, "int", &obj),                                                                     \
            string: globprint(type_string, "", "string", &obj),                                                                \
            bool: globprint(type_bool, "", "bool", &obj),                                                                      \
            u4: globprint(type_u32, "", "u32", &obj),                                                                          \
            float: globprint(type_float, "", "float", &obj),                                                                   \
            double: globprint(type_double, "", "double", &obj),                                                                \
            u8: globprint(type_u64, "", "u64", &obj),                                                                          \
            i8: globprint(type_i64, "", "i64", &obj),                                                                          \
            var: globprint(type_pointer, "", "pointer", &obj),                                                                 \
            default: globprint(type_unknown, #value, "unknown", &obj));                                                        \
    })

#define print(...)                                                                                                             \
    ({                                                                                                                         \
        EXPAND_print_local(__VA_ARGS__);                                                                                       \
        putchar('\n');                                                                                                         \
    })

#define item(value, code)                                                                                                      \
    case value: {                                                                                                              \
        code;                                                                                                                  \
    }; break
#define none(code)                                                                                                             \
    default: {                                                                                                                 \
        code;                                                                                                                  \
    } break

#define decast(type) (*((type *) value))

#define puts(value)        write(value, strlen(value), stdout)
#define puts_static(value) write(value, sizeof(value), stdout)

#define expand(x) x
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
#define demon(code)    /* Fork the current process and creates an async process that runs                                      \
                          in parallel, it also returns the PID of the child process to the                                     \
                          parent, in case you need to wait until the child dies. */                                            \
    __demon(code, expand(__COUNTER__))

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

void exit(const int exit_code) {
    const uint64_t syscall_number = 60;

    syscall2(syscall_number, exit_code);

    __builtin_unreachable();
}

#define stdin  0 /* Standard input.  */
#define stdout 1 /* Standard output.  */
#define stderr 2 /* Standard error output.  */

#define BUFFER_SIZE 4096

static char buffer[ BUFFER_SIZE ];
static int  buffer_index = 0;

int flush(int fd);

int write(const char *ptr, const int size, const int fd) {
    int total_written = 0;
    for (int i = 0; i < size; i++) {
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

int flush(int fd) {
    if (buffer_index == 0) {
        return 0; // Nothing to flush
    }

    int result   = syscall4(1, fd, buffer, buffer_index);
    buffer_index = 0; // Reset buffer index
    return result;
}

#define putchar(x)                                                                                                             \
    ({                                                                                                                         \
        byte y = x;                                                                                                            \
        write((char *) &y, 1, stdout);                                                                                         \
    })

#ifdef __MOONSHINE_MALLOC_WARNINGS
void malloc() { throw("Don't use base malloc!"); }

void realloc() { throw("Don't use base realloc!"); }

void free() { throw("Don't use base free!"); }
#endif

#define MAP_SHARED    0x01 /* Share changes */
#define MAP_PRIVATE   0x02 /* Changes are private */
#define PROT_READ     0x1  /* page can be read */
#define PROT_WRITE    0x2  /* page can be written */
#define PROT_EXEC     0x4  /* page can be executed */
#define MAP_FIXED     0x10 /* Interpret addr exactly */
#define MAP_ANONYMOUS 0x20 /* don't use a file */

byte strcmp(const string left, const string right) {
    while (*left && (*left == *right) && left++ && right++);
    return (byte) (*left) - (byte) (*right);
}

#define eq(a, b) (strcmp(a, b) == 0)

void memcpy(string dest, const string src, const unsigned long n) {
    for (u8 i = 0; i < n; i++) dest[ i ] = src[ i ];
}

var __bare_mmap(const var addr, const u8 len, const int prot, const int flags, const int fd, const int64_t offset) {
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

var __bare_alloc(const uint64_t size) {
    return __bare_mmap(NULL, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
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

#define throw(...)                                                                                                             \
    ({                                                                                                                         \
        print("Error: ", __VA_ARGS__);                                                                                         \
        puts_static("The following is an exception-related crash:");                                                           \
        (void) raise(SIGILL);                                                                                                  \
        __builtin_unreachable();                                                                                               \
        0;                                                                                                                     \
    })

var __bare_mremap(const var ptr, const uint64_t old, const uint64_t new) {
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

#define align_value(value) ((u8) ((((value) + 15) & ~15ULL)))

struct FreeBlock find_free_space(struct Page *page) {
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

        // Check gap before current block
        if (block->location > prev_end) {
            uint64_t gap_size = block->location - prev_end;
            if (gap_size > max_free.size) {
                max_free.start = prev_end;
                max_free.size  = gap_size;
            }
        }

        if (current_end > prev_end) { prev_end = current_end; }
    }

    // Check gap after last block
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

// remove empty pages
void clean_pages() {
    for (u8 i = 0; i < global_page_table.size; i++) {
        auto page = &global_page_table.pages[ i ];

        if (page->start == null) continue;

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

var alloc(const u8 _len) {
    clean_pages();

    const u8 len = align_value(_len);

    // Find the biggest free spot
    i8               biggest_free_page = -1;
    struct FreeBlock max_biggest_free  = { 0, 0 };

    for (u8 i = 0; i < global_page_table.size; i++) {
        if (global_page_table.pages[ i ].start == null) continue;
        struct FreeBlock biggest_free = find_free_space(&global_page_table.pages[ i ]);

        if (biggest_free.size <= max_biggest_free.size) continue;
        max_biggest_free  = biggest_free;
        biggest_free_page = i;
    }

    auto page = &global_page_table.pages[ biggest_free_page ];

    if (max_biggest_free.size < len) {
        // add a page
        u8 place = 0;

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
            global_page_table.pages
                = __bare_mremap(global_page_table.pages, global_page_table.size, global_page_table.capacity *= 2);
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
        page->pointers.data = __bare_mremap(page->pointers.data, page->pointers.size, page->pointers.capacity *= 2);
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
            // Address is in this page

            struct AllocatedPointer *best_ptr = null;

            for (u8 j = 0; j < page->pointers.size; j++) {
                struct AllocatedPointer *ptr = &page->pointers.data[ j ];

                if (ptr->location == location) {
                    if (!ptr->allocated && best_ptr != null) continue;
                    best_ptr = ptr;
                }
            }

            if (!best_ptr->allocated) throw("Unallocated pointer? ");
            if (best_ptr == null) throw("Pointer for location does not exist in page");

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

void release(const var addr) { unmap(find_pointer(addr), addr); }

var remap(const var ptr, const u8 new) {
    const var  new_ptr = alloc(new);
    const auto found   = find_pointer(ptr);
    memcpy(new_ptr, ptr, found.allocation_size);
    unmap(found, ptr);
    return new_ptr;
}

unsigned long strlen(const char *str) {
    unsigned long len = 0;
    while (str[ len ] != '\0') len++;
    return len;
}

int fork() {
    const uint64_t syscall_number = 57;
    const int      pid            = syscall1(syscall_number);

    return pid;
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

    demon({
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

#define cast_index(arr, type, index) ((type *) arr)[ index ]
#define cast_ptr(arr, type, index)   (&cast_index(arr, type, index))

const int   magic_number = 0xB00E;
typedef int magic_type;
const int   __addon_size = 2 * sizeof(u4) + sizeof(string) + sizeof(magic_type);
const int   addon_size   = align_value(__addon_size);
const int   spacer_size  = addon_size - __addon_size;

const var *__new_array(const string type_name, const u4 type_size, const u4 count) {
    // string name, and one place for the special code
    const u8 size = type_size * count + addon_size;

    const var base = alloc(size);
    var       ptr  = base;

    cast_index(ptr, magic_type, 0) = magic_number; // magic number for arrays
    ptr                            = cast_ptr(ptr, magic_type, 1);

    cast_index(ptr, const string, 0) = type_name;
    ptr                              = cast_ptr(ptr, string, 1);

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

void unsafe_extend(
    var      *array_ref,
    const var array_from_u4,
    const var array_base,
    const u4  element_count,
    const u4  element_size,
    const u4  offset) {
    cast_index(array_from_u4, u4, 1) += offset;
    const var    new_base    = remap(array_base, addon_size + element_size * (element_count + offset));
    const string array_place = &((string) new_base)[ addon_size ];
    *array_ref               = (var) array_place;
}

void unsafe_push(var *array, const var ptr, const u4 ptr_size) {
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

    if (element_size != ptr_size && ptr_size != 0) {
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
    memcpy(array[ 0 ] + element_count * element_size, ptr, ptr_size);
}

// IMPROVE
#define safe_push(type, array, ptr)                                                                                            \
    ({                                                                                                                         \
        type **_array = array;                                                                                                 \
        type   _ptr   = ptr;                                                                                                   \
        unsafe_push((var *) _array, (var) & _ptr, sizeof(type));                                                               \
    })
#define push(array, ptr) safe_push(typeof((array)[ 0 ][ 0 ]), array, ptr)
#define address_item(__array, __index)                                                                                         \
    ({                                                                                                                         \
        const auto arr   = __array;                                                                                            \
        const u8   index = __index;                                                                                            \
        if (index >= count(arr)) throw("Requested index address in array exceeds the size of the array!");                     \
        if (index < 0) throw("Cannot address negative offset in array!");                                                      \
        &(arr[ 0 ][ index ]);                                                                                                  \
    })
#define get(__array, __index)                                                                                                  \
    ({                                                                                                                         \
        const auto main        = __array;                                                                                      \
        const auto arr         = main[ 0 ];                                                                                    \
        const u8   __get_index = __index;                                                                                      \
        if (__get_index > count(main))                                                                                         \
            throw("Requested index in array (", __get_index, ") exceeds the size of the array! (", count(main), ")");          \
        if (__get_index < 0) throw("Cannot access negative offset in array!");                                                 \
        arr[ __get_index ];                                                                                                    \
    })
#define count(array) ((u4) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u4, 1))
#define element_size(array)                                                                                                    \
    (u4) cast_index(cast_ptr(cast_ptr(cast_ptr(*array, byte, -addon_size), magic_type, 1), string, 1), u4, 0)
#define last(array) get(array, count(array) - 1)

#define in               ,
#define foreach(...)     foreach_xp(foreach_inner, (__VA_ARGS__))
#define foreach_xp(a, b) a b
#define foreach_inner(item, array)                                                                                             \
    u4 cat(item, _index) = 0;                                                                                                  \
    for (auto item = get(array, cat(item, _index)++); cat(item, _index) <= count(array); item = get(array, cat(item, _index)++))

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

void puts_hex(const unsigned char hex) {
    const char hex_chars[] = "0123456789abcdef";
    putchar(hex_chars[ hex >> 4 ]);
    putchar(hex_chars[ hex & 0x0F ]);
}

void puts_pointer(const void *ptr) {
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

// Massive TODO
void globprint(const enum glob_type type, const char *value_text, const char *text, const void *value) {
    switch (type) {
        item(type_char, putchar(decast(char)));
        item(type_int, {
            if (value_text[ 0 ] == '\'') putchar(decast(char));
            else { puts_number(decast(int), 1); }
        });
        item(type_long, puts_number(decast(long), 1));
        item(type_long_long, puts_number(decast(long long), 1));
        item(type_float, puts_float(decast(float)));
        item(type_double, puts_float(decast(double)));
        item(type_u64, puts_number(decast(u8), 0));
        item(type_u32, puts_number(decast(u4), 0));
        item(type_string, puts(decast(char *)));
        item(type_bool, {
            if (decast(bool)) puts_static("true");
            else
                puts_static("false");
        });
        item(type_pointer, {
            puts_static("<pointer ");
            puts_pointer(decast(var));
            puts_static(" (");
            puts_number(decast(u8), false);
            puts_static(")>");
        });

        none({
            if (mine(decast(var))) {
                if (mine(*decast(var *))) {
                    // handle array
                    var array = *decast(var *);

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
            if (eq(text, "i64")) return puts_number(decast(int), 1);

            puts_static("<unknown:");
            puts(text);
            putchar('(');
            puts_number(type, false);
            puts_static(")[");
            puts(value_text);
            puts_static("]>");
            puts_pointer(value);
        });
    }
}

const auto data  = 243;
const auto array = 247;

#define array(type) type **
#define de(obj)     (*(obj))

#define new(a, ...)         ((a **) new_x(a __VA_OPT__(, ) __VA_ARGS__, new2, new1)(a __VA_OPT__(, ) __VA_ARGS__))
#define new_x(a, b, c, ...) c
#define new1(type)          (__new_array(#type, sizeof(type), 0))
#define new2(type, count)   (__new_array(#type, sizeof(type), count))

string *environ;

__attribute__((noreturn)) void __moonshine_start(int argc, string *argv, string *envp) {
    global_page_table = PageTable();
    environ           = envp;

    int exit_code;

    __asm__ __volatile__("mov %1, %%rdi      \n"
                         "mov %2, %%rsi      \n"       // Move argv into rsi (2nd argument)
                         "call main          \n"       // Call main(argc, argv)
                         "mov %%eax, %0      \n"       // Store return value into exit_code
                         : "=r"(exit_code)             // Output operand
                         : "r"((long) argc), "r"(argv) // Input operands
                         : "rdi", "rsi", "rax", "memory");

    exit(exit_code);
}

__attribute__((force_align_arg_pointer)) __attribute__((naked)) void _start(void) {
    __asm__ volatile(
        // rdi will get argc. The first quadword at rsp is argc.
        "mov (%rsp), %rdi \n" // rdi = argc

        // rsi will get argv, which is at rsp + 8.
        "lea 8(%rsp), %rsi \n" // rsi = &argv[0]

        // Compute envp pointer.
        // We need to add 8*(argc + 1) to rsp.
        // First, copy argc (which is in rdi) into rax.
        "mov %rdi, %rax \n"         // rax = argc
        "lea (,%rax,8), %rcx \n"    // rcx = argc * 8
        "add $8, %rcx \n"           // rcx = 8*(argc + 1)
        "lea (%rsp, %rcx), %rdx \n" // rdx = rsp + 8*(argc+1) -> envp pointer

        // Now call __moonshine_start(argc, argv, envp).
        "call __moonshine_start \n"

        // If __moonshine_start returns, we exit.
        // Exit syscall number 60; exit code 0.
        "mov $60, %rax \n"  // syscall: exit
        "xor %rdi, %rdi \n" // rdi = 0    (exit code 0)
        "syscall \n"

        // No return. Add an infinite loop as a safeguard.
        "hlt \n" // halt the CPU if syscall fails
    );
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
#define O_CREAT  64   /* (0100 octal) */
#define O_TRUNC  512  /* (01000 octal) */
#define O_APPEND 1024 /* (02000 octal) */

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

    FILE *file = (FILE *) alloc(sizeof(FILE));
    if (!file) {
        syscall2(SYS_close, fd);
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
    release((var) stream);
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

#endif
