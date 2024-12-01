#include "main.h"

int main()
{

    // test code
    for (int i = 0; i < 22; i++)
    {
        container_add({N - 1, M - 1}, 1);
        auto test = container_organize();
        apply_commands(test, false);
    }
    print_grid();
    container_remove({0, 2});
    container_remove({0, 3});
    container_remove({0, 4});
    container_remove({1, 4});
    container_remove({2, 4});
    print_grid();
    apply_commands(container_move(5, {N - 1, M - 1}), false);
    print_grid();
    apply_commands(container_make_empty({4, 0}), false);
    print_grid();
    container_all_remove();
}