#include "dependencies.h"
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


typedef struct {
  char *name;
  char *properties;
  size_t properties_length;
  bool private; // hosted on private pub server
  bool valid;
} dependency;

int replace_hosted_to_pathed(const char *yaml_file_name, const char *path);
bool prefix(const char *pre, const char *str);

typedef struct {
  char *name;
  char *path;
  char *version;
} local_dependency_info;

void remove_spaces(char *s);

local_dependency_info *dependencies[32];


/**
 * 存放 packages 的根目录: "/Users/wangpan/Work/Mi/mi_flutter_plugins/packages"
 * */
static char *root_packages_dir_path;

static bool local_dependency_loaded = false;
int dependencies_count = 0;
void init_root_packages_dir_path(char *origin_root_path);
void print_loaded_local_dependencies();

int replace_all_local_hosted_to_pathed();

int load_local_dependency_info(char *packages_dir_name) {
  if (packages_dir_name == NULL) {
    fprintf(stderr, "packages_dir_name can not be NULL");
    return -1;
  }
  init_root_packages_dir_path(packages_dir_name);

  DIR *dir;
  struct dirent *dir_ent;
  dir = opendir(root_packages_dir_path);
  int count = 0;

  FILE *pubspec_yaml;
  if (dir) {
    while ((dir_ent = readdir(dir)) != NULL) {
      if (dir_ent->d_type == DT_DIR && strcmp(dir_ent->d_name, ".") &&
          strcmp(dir_ent->d_name, "..")) {
        char *dir_name = calloc(sizeof(char), strlen(dir_ent->d_name) + 1);
        memcpy(dir_name, dir_ent->d_name, strlen(dir_ent->d_name));

        dependencies[count] = calloc(sizeof(local_dependency_info), 1);
        dependencies[count]->path = dir_name;

        char *pubspec_file_path;
        asprintf(&pubspec_file_path, "%s%s/%s", root_packages_dir_path, dir_name, YAML_FILE_NAME);

        if ((pubspec_yaml = fopen(pubspec_file_path, "r")) != NULL) {
          char line[MAX_LINE_CHAR_COUNT];
          bool name_found = false;
          bool version_found = false;
          size_t line_length;
          size_t key_name_length = strlen(YAML_KEY_NAME);
          size_t key_version_length = strlen(YAML_KEY_VERSION);
          while (fgets(line, MAX_LINE_CHAR_COUNT, pubspec_yaml)) {
            line_length = strlen(line);
            if (!strncmp(line, YAML_KEY_NAME, strlen(YAML_KEY_NAME))) {
              char *name;
              asprintf(&name, "%.*s", line_length - key_name_length - 1, line + key_name_length);
              remove_spaces(name);
              dependencies[count]->name = name;
              name_found = true;
            } else if (!strncmp(line, YAML_KEY_VERSION, strlen(YAML_KEY_VERSION))) {
              char *version;
              asprintf(&version, "%.*s", line_length - key_version_length - 1,
                       line + key_version_length);
              remove_spaces(version);
              dependencies[count]->version = version;
              version_found = true;
            }
            if (name_found && version_found)
              break;
          }
          fclose(pubspec_yaml);
          count++;
        } else {
          fprintf(stderr, "Open %s in %s failed! \n", YAML_FILE_NAME, dir_name);
          free(dir_name);
          free(dependencies[count]);
        }
        free(pubspec_file_path);
      }
    }
  } else {
    return -1;
  }
  dependencies_count = count;
  local_dependency_loaded = true;
  print_loaded_local_dependencies();
  replace_all_local_hosted_to_pathed(); // todo 返回值
  return dependencies_count;
}

char *get_path_of_dependency(char *denpendency_name) {
  for (int i = 0; i < dependencies_count; i++) {
    if (!strcmp(dependencies[i]->name, denpendency_name)) {
      return dependencies[i]->path;
    }
  }
  fprintf(stderr, "%s NOT found! \n", denpendency_name);
  return NULL;
}

/**
 * name_line: "  card_ocr:\n"
 * */
char *full_path_of_dependency(const char *name_line) {
  if (!local_dependency_loaded) {
    fprintf(stderr, "Please load local denpendency first!\n");
    return NULL;
  }
  // name_line: "  card_ocr:\n"
  char *name;
  asprintf(&name, "%s", name_line);
  remove_spaces(name);
  name[strlen(name) - 2] = '\0';
  char *path = get_path_of_dependency(name);
  if (!path) {
    return NULL;
  }
  char *ret;
  asprintf(&ret, "%s%s", root_packages_dir_path, path);
  free(name);
  return ret;
}

void init_root_packages_dir_path(char *origin_root_path) {
  size_t origin_length = strlen(origin_root_path);
  bool need_slash = origin_root_path[origin_length - 1] != '/';
  asprintf(&root_packages_dir_path, "%s%s", origin_root_path, need_slash ? "/" : "");
}

void print_loaded_local_dependencies() {
  if (!DEBUG)
    return;

  printf("there are %d packages loaded:\n", dependencies_count);
  local_dependency_info *info;
  for (int i = 0; i < dependencies_count; i++) {
    info = dependencies[i];
    printf("name: %s:\n  version: %s\n  path: %s\n", info->name, info->version, info->path);
  }
}

void remove_spaces(char *s) {
  const char *d = s;
  do {
    while (*d == ' ') {
      ++d;
    }
  } while (*s++ = *d++);
}

/// yaml.c
bool is_cur_line_dependencies_begin(const char *line);
bool is_cur_line_other_begin(const char *line);
void write_and_clear_dependency(FILE *file, dependency *d);

/**
 *  把 yaml_file_name 中 host 在 private pub server 的 dependency 改成 path 的。
 *  path 的路径：根据每个 dependency 的名字去 load_local_dependency_info 中 load 出来的本地
 * dependency 信息表中查找。 同时会把修改前的 yaml_file_name 保存到 bak_file_name 中。
 * */
int replace_hosted_to_pathed(const char *yaml_file_name, const char *bak_file_name) {
  FILE *src;
  FILE *dest;
  src = fopen(yaml_file_name, "r");
  if (src == NULL) {
    printf("Read yaml file failed: %s\n", yaml_file_name);
    return -1;
  }
  dest = fopen(bak_file_name, "w");
  if (dest == NULL) {
    printf("Create new yaml file failed\n");
    return -1;
  }

  char line[MAX_LINE_CHAR_COUNT];

  bool is_cur_line_in_dependency = false; // 当前处理的行是不是 dependency 中的一行

  dependency cur_dependency = {NULL, NULL, 0, false, false};

  while (fgets(line, MAX_LINE_CHAR_COUNT, src)) {

    size_t line_length = strlen(line);

    if (is_cur_line_in_dependency) {
      is_cur_line_in_dependency = !is_cur_line_other_begin(line);
    } else {
      is_cur_line_in_dependency = is_cur_line_dependencies_begin(line);
    }

    if (!is_cur_line_in_dependency || is_cur_line_dependencies_begin(line)) {
      // dependencies 结束了，把最后一个 dependency 写入 dest
      write_and_clear_dependency(dest, &cur_dependency);

      fprintf(dest, "%s", line);
      continue;
    }

    if (prefix("  ", line) && !prefix("   ", line)) // 开始一个新的 dependency 或开始一行注释："  #"
    {
      /// 把上一个 dependency 写入到 dest 中
      write_and_clear_dependency(dest, &cur_dependency);

      cur_dependency.name = calloc(sizeof(char), line_length + 1);
      memcpy(cur_dependency.name, line, line_length + 1);
      cur_dependency.valid = true;
    } else // 处理一个 dependency 的属性：depency_detail
    {
      if (cur_dependency.private)
        continue;

      // todo: check version
      if (strcmp("      url: *private_pub_server\n", line) == 0) {
        // found!
        cur_dependency.private = true;
        if (cur_dependency.properties)
          free(cur_dependency.properties);

        char *dependency_path = full_path_of_dependency(cur_dependency.name);
        asprintf(&cur_dependency.properties, "%s%s%s", "    path: ", dependency_path, "\n\n");
        free(dependency_path);
      } else {
        // 把当前行 append 到 depency_detail 中
        char *s = calloc(sizeof(char), cur_dependency.properties_length + line_length + 1);
        if (cur_dependency.properties_length > 0) {
          memcpy(s, cur_dependency.properties, cur_dependency.properties_length);
        }
        memcpy(s + cur_dependency.properties_length, line, line_length + 1);
        if (cur_dependency.properties_length > 0)
          free(cur_dependency.properties);
        cur_dependency.properties = s;
        cur_dependency.properties_length += line_length;
      }
    }
  }

  write_and_clear_dependency(dest, &cur_dependency);

  fflush(dest);
  fclose(dest);
  fclose(src);
  char *temp_bak_file_name;
  asprintf(&temp_bak_file_name, "%s._b_a_k", bak_file_name);

  if (rename(yaml_file_name, temp_bak_file_name) != 0 ||
      rename(bak_file_name, yaml_file_name) != 0 ||
      rename(temp_bak_file_name, bak_file_name) != 0) {
    fprintf(stderr, "Rename failed. errno = %d: %s\n", errno, strerror(errno));
    free(temp_bak_file_name);
    return -1;
  }
  free(temp_bak_file_name);
  return 0;
}

int replace_all_local_hosted_to_pathed() {
  local_dependency_info *info;
  for (int i = 0; i < dependencies_count; i++) {
    info = dependencies[i];
    char *pubspec_name;
    char *bak_name;
    asprintf(&pubspec_name, "%s%s/%s", root_packages_dir_path, info->path, YAML_FILE_NAME);
    asprintf(&bak_name, "%s%s/%s", root_packages_dir_path, info->path, YAML_BACKUP_FILE_NAME);
    replace_hosted_to_pathed(pubspec_name, bak_name);
    free(pubspec_name);
    free(bak_name);
  }
  return 0;
}

bool prefix(const char *pre, const char *str) { return strncmp(pre, str, strlen(pre)) == 0; }

// 当前行是依赖声明的开始
bool is_cur_line_dependencies_begin(const char *line) {
  return !strcmp(line, "dependencies:\n") || !strcmp(line, "dev_dependencies:\n") ||
         !strcmp(line, "dependency_overrides:\n");
}

// 当前行是其他声明的开始，例如："flutter_icons:"
bool is_cur_line_other_begin(const char *line) {
  return !prefix(" ", line) && !prefix("\n", line) && !is_cur_line_dependencies_begin(line);
}

void write_and_clear_dependency(FILE *file, dependency *d) {
  if (!d->valid)
    return;
  if (d->name) {
    fprintf(file, "%s", d->name);
    free(d->name);
  }
  if (d->properties) {
    fprintf(file, "%s", d->properties);
    free(d->properties);
  }
  d->name = NULL;
  d->properties = NULL;
  d->properties_length = 0;
  d->valid = false;
  d->private = false;
}