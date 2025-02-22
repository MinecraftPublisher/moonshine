// This is the test unit, not the actual code.

#include "moonshine.h"

int main() {
    array(int) test = new (int);

    push(test, 2);
    for(int i = 45; i > 7; i--) push(test, i % 3);

    print("Wowie: ", get(test, 30));

    return 0;
}
