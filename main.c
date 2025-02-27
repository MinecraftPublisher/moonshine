// This is the test unit, not the actual code.

#include "moonshine.h"

const enum { test_datatype, test_linkedlist } current_test = test_linkedlist;

int main() {
    if (current_test == test_datatype) {
        datatype(binarytree, ((leaf, int), (node, binarytree *, int, binarytree *) ));
        const auto tree = instance(binarytree, leaf, 3);

        print(&tree);

        match(tree) {
            of(leaf, v) print("Leafy ", v);
            of(node, _, v, _) print("Nodey ", v);
            other() print("Idk man");
        }
    } else if (current_test == test_linkedlist) {
        const auto ll = create_linkedlist();

        for (u8 i = 0; i < 100; i++) push_linkedlist(ll, (var) i);

        print((u8) index_to_ptr(ll, 12)->data);
    }

    else {
        print("Unknown test.");
    }

    return 0;
}
