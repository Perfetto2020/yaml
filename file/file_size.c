#include <stdio.h>
#include <stdlib.h>
void humanReadableSize(char *, long);
int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Usage: %s file name\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE *file;
    if((file = fopen(argv[1], "r")) == NULL)
    {
        printf("Open file %s failed \n", argv[1]);
        exit(EXIT_FAILURE);
    }
    if(fseek(file, 0, SEEK_END) != 0) {
        printf("Seek file %s to end failed\n", argv[1]);
        exit(EXIT_FAILURE);
    }
    long length = ftell(file);
    printf("The length of file \"%s\" is %ld\n", argv[1], length);
    char *readalbe = malloc(5);
    humanReadableSize(readalbe, length);
    printf("The human readable length of file \"%s\" is %s\n", argv[1], readalbe);
    free(readalbe);
    return 0;
}

void humanReadableSize(char *readable, long size)
{
    if(size < 0) return;

    char *units[] = {"B", "KB", "MB", "GB", "TB"};
    unsigned short length = sizeof(units)/sizeof(units[0]);
    int idx = -1;
    while (++idx < length && size > 1024) {
        size /= 1024;
    }
    sprintf(readable, "%ld%s", size, units[idx]);
}