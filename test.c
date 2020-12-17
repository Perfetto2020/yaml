#include <stdio.h>
#include <string.h>


int main(void)
{
    char *test = "hello";
    printf("%lu\n", strlen(test));
    printf("%c\n", test[0]);
    printf("%c\n", test[4]);
    printf("%c\n", test[5]);
}