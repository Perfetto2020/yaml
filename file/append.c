#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLEN 81
char *s_gets(char *, int);
int main(void)
{
    puts("Pls input the destination file name:");
    char file_des_name[SLEN];
    if (s_gets(file_des_name, SLEN) == NULL)
    {
        puts("Get destination file name failed");
        exit(EXIT_FAILURE);
    }
    FILE *file_destination = fopen(file_des_name, "a");
    if (file_destination == NULL)
    {
        puts("Open destination file failed");
        exit(EXIT_FAILURE);
    }
    FILE *fp_src;
    char file_name_src[SLEN];
    file_name_src[0] = 'A';
    puts("Pls input the source file name:");
    while (s_gets(file_name_src, SLEN) != NULL && file_name_src[0] != '\0')
    {
        if ((fp_src = fopen(file_name_src, "r")) == NULL)
        {
            printf("Open source file failed: %s\n", file_name_src);
        }
        else
        {
            char cache[16];
            while (fgets(cache, SLEN, fp_src) != NULL)
            {
                if (!fputs(cache, file_destination))
                {
                    printf("Write data to file %s failed\n", file_des_name);
                    break;
                }
            }
            if (fprintf(file_destination, "\n--------- %s appended ----------\n", file_name_src) < 0)
            {
                printf("Write data to file %s failed\n", file_des_name);
            }
            if (feof(fp_src))
            {
                printf("Append %s to %s successed!\n", file_name_src, file_des_name);
            }
            fflush(file_destination);
            fclose(fp_src);
        }
        puts("Input next src file name, or Enter to exit");
    }
    if (file_name_src[0] == 'A')
    {
        puts("Get source file name failed");
    }
    else
    {
        puts("Append word Done!");
    }
    fclose(file_destination);
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