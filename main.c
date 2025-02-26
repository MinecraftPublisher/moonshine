// This is the test unit, not the actual code.

#define __MOONSHINE_LIBC_WARNINGS
#include "moonshine.h"

void cleanup_free(var* input) { print("Deallocation logic goes here ", input); }

#define auto_free __attribute__((cleanup(cleanup_free)))

#define create(type, name, ...)                                                                                                      \
    auto_free var cat(name, _parent) = __VA_ARGS__;                                                                            \
    type          name               = cat(name, _parent);

int main() {
    create(t(int), proof_of_concept, new(int));

    print("Neat, right? ", proof_of_concept);

    return 0;
}
