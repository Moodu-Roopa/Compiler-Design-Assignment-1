#include <stdio.h>
// This is the
// input code
int main() {
    int x = 10;
    float y = 3.14f;
    char c = 'A';
    double z = .5e2;

    x = x + y - 2 * (x / 5 % 3);

    x++;
    --y;

    if (x > y && x != 0 || !(x <= 100)) {
        printf("x is valid\n");
    }

    x = x << 2;
    y += 5;
    x &= x ^ ~x;

    char *str = "Hello\tWorld\n";

    return 0;
}
