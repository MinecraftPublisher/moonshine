// The moonshine library build and installation script.

// ----- This part is a bit of boilerplate to establish basic functionality and get to work. -----

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
            if (ptr[ i ] == 0) return flush(fd);
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

#define staputs(txt)                                                                                                           \
    ({                                                                                                                         \
        const char __txt[] = "" txt;                                                                                           \
        write(__txt, sizeof(__txt), stdout);                                                                                   \
    })
#define puts(txt)                                                                                                              \
    ({                                                                                                                         \
        const char *__txt = txt;                                                                                               \
        write(__txt, strlen(__txt), stdout);                                                                                   \
    })

__attribute__((diagnose_as_builtin(__builtin_strlen, 1))) unsigned long strlen(const char *str) {
    unsigned long len = 0;
    while (str[ len ] != '\0') len++;
    return len;
}

void exit(const int exit_code) {
    const unsigned long syscall_number = 60;

    flush(stdout);
    (void) syscall2(syscall_number, exit_code);

    __builtin_unreachable();
}

int fork() {
    const unsigned long syscall_number = 57;
    const int           pid            = syscall1(syscall_number);

    return pid;
}

char **environ;

[[noreturn]] __attribute__((used)) void __moonshine_builder_start(int argc, char **argv, char **envp) {
    int exit_code;

    environ = envp;

    __asm__ __volatile__("mov %1, %%rdi      \n"
                         "mov %2, %%rsi      \n"
                         "call main          \n"
                         "mov %%eax, %0      \n"
                         : "=r"(exit_code)
                         : "r"((long) argc), "r"(argv)
                         : "rdi", "rsi", "rax", "memory");

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
                     "call __moonshine_builder_start \n"
                     "mov $60, %rax \n"
                     "xor %rdi, %rdi \n"
                     "syscall \n"
                     "hlt \n");
}

#define NULL ((void *) 0)

int execl(const char *path, const char *arg0, const char *arg1, const char *arg2, char **envp) {
    char *argv[] = { (char *) arg0, (char *) arg1, (char *) arg2, (char *) 0 };
    int   ret    = (int) syscall4(59, path, argv, envp);
    return ret;
}

int waitpid(int pid, int *status, int options) { return syscall5(61, pid, status, options, 0); }

int system(const char *command) {
    int pid;
    int status;

    if (command == NULL) { return 1; }

    pid = fork();
    if (pid == -1) { return -1; }
    if (pid == 0) {
        execl("/bin/sh", "sh", "-c", command, environ);
        exit(127);
    }
    waitpid(pid, &status, 0);
    return status;
}

int run(const char *command) {
    staputs("> ");
    puts(command);
    staputs("\n");
    return system(command);
}

__attribute__((diagnose_as_builtin(__builtin_memcpy, 1, 2, 3))) void *
memcpy(void *_dest, const void *_src, const unsigned long n) {
    char       *dest = _dest;
    const char *src  = _src;

    for (unsigned long i = 0; i < n; i++) dest[ i ] = src[ i ];
    return dest;
}

__attribute__((diagnose_as_builtin(__builtin_strcmp, 1, 2))) int strcmp(const char *left, const char *right) {
    while (*left && (*left == *right) && left++ && right++);
    return (unsigned char) (*left) - (unsigned char) (*right);
}

__attribute__((diagnose_as_builtin(__builtin_strncmp, 1, 2, 3))) int
strncmp(const char *left, const char *right, unsigned long n) {
    while (n && *left && (*left == *right)) {
        left++;
        right++;
        n--;
    }
    return (n == 0) ? 0 : ((char) (*left) - (char) (*right));
}

#define putchar(c)                                                                                                             \
    ({                                                                                                                         \
        char f = c;                                                                                                            \
        write(&f, 1, stdout);                                                                                                  \
    })

void puts_number(const long number, const unsigned char is_signed) {
    char temp[ 20 ];
    int  len = 0;
    long n   = number;

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

unsigned char eq(const char *l, const char *r) { return strcmp(l, r) == 0; }

// ----- The macro processor starts here. -----

#define type(...)                                                                                                              \
    ({                                                                                                                         \
        staputs(__VA_ARGS__);                                                                                                  \
        putchar('\n');                                                                                                         \
    })

void macro_define_part(char *name, int index, int count) {
    staputs("#define EXPAND_");
    puts(name);
    staputs("_");
    puts_number(index, 0);
    staputs("(func, extra, x, ...) func(extra, ");
    puts_number(count - index + 1, 0);
    staputs(", x)");
    if (index == 1) {
        putchar('\n');
        return;
    }
    staputs(" cat(dump_or_continue_0, __VA_OPT__(1))(EXPAND_");
    puts(name);
    staputs("_");
    puts_number(index - 1, 0);
    type("(func, extra, __VA_ARGS__))");
}

void macro_define_expander(char *name, int count) {
    staputs("#define EXPAND_");
    puts(name);
    staputs("(func, extra, ...) EXPAND_");
    puts(name);
    staputs("_");
    puts_number(count, 0);
    type("(func, extra, __VA_ARGS__)");

    for (int i = 0; i < count; i++) { macro_define_part(name, count - i, count); }
}

void macro_processor() {
    type("#ifndef moonshine_m4_header\n"
         "#define moonshine_m4_header\n"
         "\n"
         "// Autogenerated by the moonshine builder.\n"
         "// If you wanna make changes, good luck :P\n");

    type("#define p_cat(a, b) a##b\n"
         "#define cat(a, b) p_cat(a, b)\n"
         "#define cat3(a, b, c) cat(a, cat(b, c))\n"
         "#define cat4(a, b, c, d) cat(cat(a, b), cat(c, d))\n");

    type("#define dump_or_continue_0(...)\n"
         "#define dump_or_continue_01(...) __VA_ARGS__\n");

    macro_define_expander("general", 1024);
    macro_define_expander("general2", 1024);

    type("\n#endif");
}

// ----- The real builder code starts here. -----

#define FLAGS "-nostartfiles -nostdlib"
#define SIZE_FLAGS                                                                                                             \
    "-Wl,--undefined=main -flto -Oz -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,--strip-all "                    \
    "-finline-hint-functions -fno-builtin -Wl,--print-gc-sections -fno-unwind-tables -fomit-frame-pointer "                    \
    "-fno-asynchronous-unwind-tables -fno-ident -fvisibility=hidden -march=native -Wl,--build-id=none"

unsigned char code_ran = 0;

void run_dep_code() {
    if (code_ran) return;
    code_ran = 1;
    run("clang main.c -o out.bin " FLAGS /* " -O3" */);
}

unsigned char macro_ran = 0;

void run_dep_macro(char *name) {
    if (macro_ran) return;
    macro_ran = 1;

    int  self_len = strlen(name);
    char addon[]  = " __internal_macro_processor__ > moonshine.expander.h";
    char self[ self_len + sizeof(addon) + 1 ];

    memcpy(self, name, self_len);
    memcpy(self + self_len, addon, sizeof(addon));
    self[ sizeof(self) - 1 ] = '\0';

    run("rm -f moonshine.expander.h");
    system(self);
}

unsigned char install_ran = 0;

void run_dep_install() {
    if (install_ran) return;
    install_ran = 1;

    type("Make sure your CPATH environment variable is set to recognize ~/include/ as a valid header directory.");
    type("Otherwise, clang will not recognize moonshine as a valid library.");

    run("cp moonshine.h ~/include/moonshine.h");
    run("cp moonshine.expander.h ~/include/moonshine.expander.h");
}

int main(int argc, char **argv) {
    if (argc <= 1) {
        staputs("Usage:\n    ");
        puts(argv[ 0 ]);
        staputs(
            " [ clean | macro | code | run | profile | debug | install ]+ | <all>\n\n"
            "This is a bootstrapped lightweight make alternative designed specifically to work with the moonshine library.\n"
            "Clean residue files with:\n$ ");
        puts(argv[ 0 ]);
        staputs(" clean\n"
                "Run the tests with:\n$ ");
        puts(argv[ 0 ]);
        staputs(" run\n"
                "Install the header files for use within your projects:\n$ ");
        puts(argv[ 0 ]);
        staputs(" install\n"
                "Perform preprocessing and install moonshine:\n$ ");
        puts(argv[ 0 ]);
        staputs(" all\n");
    }

    int tracker = 0;
    while (++tracker < argc) {
        char *argument = argv[ tracker ];

        if (eq(argument, "clean")) {
            type("-- clean");
            run("rm -f out.*");
            run("rm -f *.expander.h");
            run("rm -f perf.data*");
        } else if (eq(argument, "macro")) {
            type("-- macro");
            run_dep_macro(argv[ 0 ]);
        } else if (eq(argument, "code")) {
            type("-- code");
            run_dep_code();
        } else if (eq(argument, "run")) {
            type("-- run");
            run_dep_code();
            run("./out.bin");
        } else if (eq(argument, "profile")) {
            type("-- profile");
            run_dep_code();
            run("perf record ./out.bin");
            run("perf report > ./out.profile.txt");
        } else if (eq(argument, "debug")) {
            type("-- debug");
            run("clang main.c -o out.bin " FLAGS " -g");
            run("lldb ./out.bin -o run");
        } else if (eq(argument, "install")) {
            type("-- install");
            run_dep_install();
        } else if (eq(argument, "all")) {
            type("-- all");
            run_dep_macro(argv[ 0 ]);
            run_dep_install();
        } else if (eq(argument, "__internal_macro_processor__")) {
            macro_processor();
            exit(0);
        }
    }
}
