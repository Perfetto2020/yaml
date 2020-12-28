#include "dependencies.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  bool restore;
  char *packages_dir;
  int c;
  while ((c = getopt(argc, argv, "rp:") != -1)) {
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
      fprintf(stderr, "Usage: %s [-r] [-p your_packages_path]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }

  const *pub_spec_file_name = YAML_FILE_NAME;

  if (access(pub_spec_file_name, F_OK | R_OK) != 0) {
    fprintf(stderr, "File %s NOT found or NOT readable in current directory\n", pub_spec_file_name);
    exit(EXIT_FAILURE);
  }
  if (restore) {
    if (access(YAML_BACKUP_FILE_NAME, F_OK | R_OK) != 0) {
      fprintf(stderr, "Backup file NOT found or NOT readable in current directory\n");
      exit(EXIT_FAILURE);
    }
    
  } else {
    if (access(YAML_BACKUP_FILE_NAME, F_OK) == 0) {
      fprintf(stderr,
              "Backup file found in current directory. Restore it if you have done something\n");
      exit(EXIT_FAILURE);
    }
    load_local_dependency_info(packages_dir);

    replace_hosted_to_pathed(YAML_FILE_NAME, YAML_BACKUP_FILE_NAME);
  }
}