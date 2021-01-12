#include "dependencies.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define CONFIG_FILE_NAME "._yaml_config_.swp"

char *read_local_config(char *local_packages_dir, const int length);
int write_local_config(char *local_packages_dir);

int main(int argc, char **argv) {
  bool restore = false;

  // 设定模式，仅把 传入的 local packages dir 存入当前目录的 CONFIG_FILE_NAME 中
  bool setMode = false;
  char *packages_dir = NULL;
  int c;

  // 参数处理
  while ((c = getopt(argc, argv, "rsp:")) != -1) {
    switch (c) {
    case 'r':
      restore = true;
      break;
    case 's':
      setMode = true;
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
        fprintf(stdout, "%s can be used to change the dependencies of pubspec.yaml in current directory from hosted on private pub server to pathed, and vice versa.\n", argv[0]);
        fprintf(stdout, "Usage: %s -p your_packages_path [-rs]\n", argv[0]);
        fprintf(stdout, "          -p specify path of your local packages, i.e. xxxx/mi_flutter_plugins/packages\n");
        fprintf(stdout, "          -r restore mode. Restore all the changes made\n");
        fprintf(stdout, "          -s set mode. Set the path of your local packages.\n"
                        "             After set, you don't need to sepcify the path of your local packages in following operations\n");
      return 1;
    default:
      fprintf(stderr, "Usage: %s -p your_packages_path [-rs]\n", argv[0]);
      exit(EXIT_FAILURE);
    }
  }
  if (setMode) {
    if (packages_dir == NULL) {
      fprintf(stderr, "Use -p to indicate the location of your packages\n");
      exit(EXIT_FAILURE);
    }
    return write_local_config(packages_dir);
  }
  if (packages_dir == NULL) {
    packages_dir = calloc(MAX_LINE_CHAR_COUNT, sizeof(char));
    if (read_local_config(packages_dir, MAX_LINE_CHAR_COUNT) == NULL) {
      fprintf(stderr, "No local packages found!\n");
      exit(EXIT_FAILURE);
    }
  }
   load_local_dependency_info(packages_dir, restore);
   return hosted_to_pathed(YAML_FILE_NAME, YAML_BACKUP_FILE_NAME, restore);
}

char *read_local_config(char *local_packages_dir, const int length) {
  const char *config_file_name = CONFIG_FILE_NAME;
  FILE *config = fopen(config_file_name, "r");
  if (config == NULL) {
    return NULL;
  } else {
    fgets(local_packages_dir, length, config);
    fclose(config);
    return local_packages_dir;
  }
}

int write_local_config(char *local_packages_dir) {
  const char *config_file_name = CONFIG_FILE_NAME;
  FILE *config = fopen(config_file_name, "w");
  int result;
  if (config == NULL) {
    fprintf(stderr, "Cannot open or create file: %s\n", config_file_name);
    return -1;
  } else {
    result = fputs(local_packages_dir, config);
    fclose(config);
    return result;
  }
}