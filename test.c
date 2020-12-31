#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv) {
  int *a = calloc(10, sizeof(int));
  for (int i = 0; i < 10; i++) {
    a[i] = i + 1;
  }

  for (int i = 0; i < 10; i++) {
    printf("%d, ", a[i]);
  }
  printf("\n");
  a = realloc(a, 20 * sizeof(int));
  for (int i = 0; i < 20; i++) {
    a[i] += 1;
  }
  for (int i = 0; i < 20; i++) {
    printf("%d, ", a[i]);
  }
  printf("\n");
}
// 1, 2, 3, 4, 5, 6, 7, 8, 9, 10,
// 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,