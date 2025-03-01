// This is the test unit, not the actual code.

#include "moonshine.h"

const enum { test_datatype, test_linkedlist, test_hashtable, test_alloc } current_test = test_hashtable;

int main() {
    if(current_test == test_alloc) {
        auto v1 = obj(i4);
        auto v2 = obj(u8);

        for(u8 i = 1; i <= 1000; i++) print(alloc(i * 1000));

        *v1 = 2;
        *v2 = 99;

        print(*v1, *v2);
        release(v1);

        print(*v2);
    } else if (current_test == test_datatype) {
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
    } else if (current_test == test_hashtable) {
        const auto ht = create_table(table_hash_2, (struct hashtable_optimizations) { .run_ptr_check = true, .collision_character_check_count = 10 });

        for (u8 i = 0; i < 10000; i++) {
            if(i % 1000 == 0) print("I ", i);
            set_hashtable(ht, "hello", "world");
            set_hashtable(ht, "how", "are you");
        }

        print((string)get_hashtable(ht, "hello"), ' ', (string)get_hashtable(ht, "how"), ' ', get_hashtable(ht, "wowie"));
    }

    else {
        print("Unknown test.");
    }

    return 0;
}
