// This is the test unit, not the actual code.

#define __MOONSHINE_LIBC_WARNINGS
#include "moonshine.h"

void cleanup_free(var* input) { print("Free that thang ", input); }

#define auto_free __attribute__((cleanup(cleanup_free)))

#define create(name, ...)                                                                                                      \
    auto_free var cat(name, _parent) = __VA_ARGS__;                                                                            \
    auto          name               = cat(name, _parent);

int main() {
    print("Wow ", strdup("Gay smex")[ 0 ]);

    return 0;
}
