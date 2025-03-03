// This is the test unit, not the actual code.

#include "moonshine.h"

const enum { test_datatype, test_linkedlist, test_hashtable, test_alloc, test_array, test_hashtable_2 } current_test = test_hashtable;

int main() {
    if (current_test == test_alloc) {
        auto v1 = obj(i4);
        auto v2 = obj(u8);

        for (u8 i = 1; i <= 1000; i++) print(alloc(i * 1000));

        *v1 = 2;
        *v2 = 99;

        println(*v1, *v2);
        release(v1);

        println(*v2);
    } else if (current_test == test_array) {
        auto arr = new (int);
        push(arr, 4);
        push(arr, 9);
        push(arr, 3);
        push(arr, 5);
        set(arr, 0, 2);

        println("Original");

        foreach(i in arr) println("[", (u8)i_index, "] ", i);

        println("Reversed");

        reverse_array(arr);

        foreach(i in arr) println("[", (u8)i_index, "] ", i);
    } else if (current_test == test_datatype) {
        datatype(binarytree, ((leaf, int), (node, binarytree *, int, binarytree *) ));
        const auto tree = instance(binarytree, leaf, 3);

        println(&tree);

        match(tree) {
            of(leaf, v) println("Leafy ", v);
            of(node, _, v, _) println("Nodey ", v);
            other() println("Idk man");
        }
    } else if (current_test == test_linkedlist) {
        const auto ll = create_linkedlist();

        for (u8 i = 0; i < 100; i++) push_linkedlist(ll, (var) i);

        print((u8) index_to_ptr(ll, 12)->data);
    } else if (current_test == test_hashtable) {
        const auto ht = create_hasharray(
            table_hash_2);

        for (u8 i = 0; i < 10000000; i++) {
            if (i % 1000000 == 0) println("I ", i);
            set_hasharray(ht, "hello", "world");
            set_hasharray(ht, "how", "are you");
        }

        println((string) get_hasharray(ht, "hello"), ' ', (string) get_hasharray(ht, "how"), ' ', get_hasharray(ht, "wowie"));
    } else if (current_test == test_hashtable_2) {
        const auto ht = create_hasharray(table_hash_2);

        set_hasharray(ht, "hello", "hi");
        println("Value: ", (string)get_hasharray(ht, "hello"));
        set_hasharray(ht, "hello", "meow");
        println("Value after: ", (string)get_hasharray(ht, "hello"));
    }

    else {
        throw("Unknown test.");
    }

    return 0;
}
