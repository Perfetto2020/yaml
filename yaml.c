#include "dependencies.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char **argv) {
  bool restore = false;
  char *packages_dir;
  int c;

  if(argc <= 1) {
    fprintf(stderr, "Usage: %s -p your_packages_path [-r]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
  while ((c = getopt(argc, argv, "rp:")) != -1) {
    switch (c) {
    case 'r':
      restore = true;
      break;
    case 'p':
      packages_dir = optarg;
      break;
    case '?':
      if (optopt == 'p')
        fprintf(stderr,
                "Option -%c requires an argument to indicate the location of your packages.\n i.e. "
                "path/to/mi_flutter_plugins/packages",
                optopt);
      else if (isprint(optopt))
        fprintf(stderr, "Unknown option `-%c'.\n", optopt);
      else
        fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
      return 1;
    default:
      fprintf(stderr, "Usage: %s -p your_packages_path [-r]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  load_local_dependency_info(packages_dir, restore);
  return hosted_to_pathed(YAML_FILE_NAME, YAML_BACKUP_FILE_NAME, restore);
}