#include "moonshine.h"

// Free variable when it goes out of scope

void cleanup_free(var *input) { print("Deallocation logic goes here ", input); }

#define auto_free __attribute__((cleanup(cleanup_free)))

#define create(type, name, ...)                                                                                                \
    auto_free var cat(name, _parent) = __VA_ARGS__;                                                                            \
    type          name               = cat(name, _parent);
