#include <stdbool.h>
#include <string.h>
#include "string_utils.h"

bool prefix(const char *pre, const char *str) { return strncmp(pre, str, strlen(pre)) == 0; }

void remove_spaces(char *s) {
  const char *d = s;
  do {
    while (*d == ' ') {
      ++d;
    }
  } while ((*s++ = *d++) != '\0');
}