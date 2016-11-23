#include <stdio.h>
#include <stdlib.h>

int test(int i, int c)
{
    i += 3;
    return i;
}

int main()
{
    int res = test(7,99);

    printf("%d\n", res);
    return 0;
}
