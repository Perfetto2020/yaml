#include "dependencies.h"
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

int main(int argc, char *argv[]) {
  char *packages_dir = "/Users/wangpan/Work/Mi/mi_flutter_plugins/packages";
  load_local_dependency_info(packages_dir);

  printf("-------------- begin work -----------------\n");

  replace_hosted_to_pathed("pubspec.yaml", "pubspec.y_a_m_l.bak");
}

bool is_cur_line_dependencies_begin(const char *line);
bool is_cur_line_other_begin(const char *line);
void write_and_clear_dependency(FILE *file, dependency *d);

/**
 *  把 yaml_file_name 中 host 在 private pub server 的 dependency 改成 path 的。
 *  path 的路径：根据每个 dependency 的名字去 load_local_dependency_info 中 load 出来的本地 dependency
 *  信息表中查找。
 *  同时会把修改前的 yaml_file_name 保存到 bak_file_name 中。
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