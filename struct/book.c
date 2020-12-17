#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TITLE_LENGTH 31
#define AUTHOR_NAME_LENGTH 21
#define MAXBKS 100

char *s_gets(char *, int);

typedef struct
{
    char title[TITLE_LENGTH];
    char name[AUTHOR_NAME_LENGTH];
    float value;
} book;

char *get_book_desc(book a_book, char *ret);
char *get_book_desc_(book *a_book, char *ret);

int main(void)
{
    book a_book = {"Title1", "author1", 9.9f};
    printf("a_book pointer: %p\n", &a_book);
    char desc[52] = {'\0'};
    get_book_desc(a_book, desc);
    printf("book: %s\n", desc);
    char desc_[52] = {'\0'};
    get_book_desc_(&a_book, desc_);
    printf("book_: %s\n", desc_);
}
char *get_book_desc(book a_book, char *ret)
{
    printf("inner get_book_desc: %p\n", &a_book);
    return strncat(strncat(ret, a_book.title, TITLE_LENGTH), a_book.name, AUTHOR_NAME_LENGTH);
}
char *get_book_desc_(book *a_book, char *ret)
{
    printf("inner get_book_desc_: %p\n", a_book);
    return strncat(strncat(ret, a_book->title, TITLE_LENGTH), a_book->name, AUTHOR_NAME_LENGTH);
}

int main1(void)
{
    book library[MAXBKS];
    int count = 0;

    printf("Please enter the title\n");
    printf("Press [enter] at the start of a line to stop.\n");
    while (count <= MAXBKS && s_gets(library[count].title, TITLE_LENGTH) != NULL && library[count].title[0] != '\n')
    {
        printf("Now enter the author.\n");
        if (s_gets(library[count].name, AUTHOR_NAME_LENGTH) == NULL)
        {
            printf("Get author of %s failed \n", library[count].title);
            break;
        }
        printf("Now enter the value.\n");
        scanf("%f", &library[count].value);
        while (getchar() != '\n')
            continue;

        count++;
        printf("Please enter the title\n");
    }

    if (count > 0)
    {
        printf("Here is the list of your books:\n");
        int idx = -1;
        while (++idx < count)
        {
            printf("%s by %s: %.2f\n", library[idx].title, library[idx].name, library[idx].value);
        }
    }
    else
    {
        printf("No books? Too bad.\n");
    }

    printf("DONE!\n");
    return 0;
}
char *s_gets(char *sp, int length)
{
    char *return_value;
    return_value = fgets(sp, length, stdin);
    if (return_value)
    {
        char *find = strchr(return_value, '\n');
        if (find)
        {
            *find = '\0';
        }
        else
        {
            while (getchar() != '\n')
                continue;
        }
    }
    return return_value;
}

#define LEN 78

struct names
{ // first structure
    char first[LEN];
    char last[LEN];
};

struct guy
{                        // second structure
    struct names handle; // nested structure
    char favfood[LEN];
    char job[LEN];
    float income;
};

struct names
{
    char first[LEN];
    char last[LEN];
};
struct pnames
{
    char *first;
    char *last;
};