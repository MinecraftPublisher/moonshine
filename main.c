// This is the test unit, not the actual code.

#define __MOONSHINE_LIBC_WARNINGS
#include "moonshine.h"

void cleanup_free(var* input) { print("Deallocation logic goes here ", input); }

#define auto_free __attribute__((cleanup(cleanup_free)))

#define create(name, ...)                                                                                                      \
    auto_free var cat(name, _parent) = __VA_ARGS__;                                                                            \
    auto          name               = cat(name, _parent);

int main() {
    create(proof_of_concept, new(int));

    print("Neat, right? ", proof_of_concept);

    return 0;
}
