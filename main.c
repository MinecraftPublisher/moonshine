// This is the test unit, not the actual code.

#include "moonshine.h"

int main() {
    array(int) test = new (int);

    int j = 1;
    for(int i = 100; i >= 0; i--) push(test, i % (j++));

    foreach(numba in test) {
        print("Item: ", numba, " index ", numba_index);
    }

    return 0;
}
