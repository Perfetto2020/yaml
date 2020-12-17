#include <stdio.h>
#include <stdlib.h>

struct flex
{
    int count;
    double average;
    double scores[];
};
void showFlex(struct flex *);
int main(void)
{
    struct flex *fp;
    int n = 5;
    int total = 0;

    fp = malloc(sizeof(struct flex) + n * sizeof(double));
    fp->count = n;
    for (int i = 0; i < 5; i++)
    {
        fp->scores[i] = 99.9 - i;
        total += fp->scores[i];
    }
    fp->average = total / n;
    showFlex(fp);
}

void showFlex(struct flex *p)
{
    int i;
    printf("Scores : ");
    for (i = 0; i < p->count; i++)
        printf("%g ", p->scores[i]);
    printf("\nAverage: %g\n", p->average);
}

struct person
{
    int id;
    struct
    {
        char first[20];
        char last[20];
    }; // anonymous structure
};
struct person ted = {8483, {"Ted", "Grass"}};
char *t = ted.first;
