#include <stdio.h>
#include <stdlib.h>
 int main(int argc, char *argv [])
 {
     char ch;
     FILE *file;
     unsigned int count;
     if(argc != 2)
     {
         printf("Usage: %s filename \n", argv[0]);
         exit(EXIT_FAILURE);
     }
     if((file = fopen(argv[1], "rb")) == NULL)
     {
         printf("Open file failed \n");
         exit(EXIT_FAILURE);
     }
     while ((ch = getc(file)) != EOF)
     {
         putchar(ch);
         count++;
     }
     fclose(file);
     printf("\nfile %s has %u characters\n", argv[1], count);
     return 0;
 }