// This is the test unit, not the actual code.

#include "moonshine.h"

datatype(binarytree, ((leaf, int), (node, binarytree *, int, binarytree *) ));

int main() {
    const auto tree = instance(binarytree, leaf, 3);

    print(&tree);

    match(tree) {
        of(leaf, v) print("Leafy ", v);
        of(node, _, v, _) print("Nodey ", v);
        other() print("Idk man");
    }

    return 0;
}
