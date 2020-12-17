#include <stdio.h>

union hold
{
    int digit;
    double bigf1;
    char letter;
};
int main(void)
{
    union hold h1;
    h1.letter = 'a'; // letter 被初始化为 a

    union hold h2 = h1; // 用 h1 初始化 h2

    union hold h3 = {88}; // digit 初始化为88

    union hold h = {.letter = 'a'};
    union hold *hp;
    hp = &h;
    printf("bigf1 = %.2f\n", hp->bigf1);
    printf("digit = %d\n", hp->digit);
    printf("letter = %c\n", hp->letter);
}

struct owner
{
    char id[10];
    //...
};

struct company
{
    char name[10];
    char headquarters[20];
    //...
};

struct car
{
    char sku[10];
    union
    {
        struct owner owncar;
        struct company leasecar;
    } owninfo;
};

enum spectrum
{
    red,
    orange,
    yellow,
    green
};
enum spectrum color = red;
int c = orange;
void test()
{
    for (color = red; color <= green; color++)
    {
        // ...
    }
}
enum level
{
    low = 100,
    medium = 500,
    high = 1000
};
