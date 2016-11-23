#include <stdio.h>
#include <stdlib.h>

int test(int i, int c)
{
    i += 3;
    return i;
}

int main()
{
    int a = 123;

    int b = a + 1;

    int res = test(a,b);

    printf("%d\n", res);
    return 0;
}
