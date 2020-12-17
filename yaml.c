#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LINE_CHAR_COUNT 200

int replace_hosted_to_pathed(const char *yaml_file_name, const char *path);
bool prefix(const char *pre, const char *str);

int main(int argc, char *argv[])
{
    replace_hosted_to_pathed("pubspec.yaml", "    path: the_path_to_the_package\n\n");
}

bool is_cur_line_dependencies_begin(const char *line);
bool is_cur_line_other_begin(const char *line);
int replace_hosted_to_pathed(const char *yaml_file_name, const char *path)
{
    FILE *src;
    FILE *dest;
    src = fopen(yaml_file_name, "r");
    if (src == NULL)
    {
        printf("Read yaml file failed: %s\n", yaml_file_name);
        return -1;
    }
    dest = fopen("yaml_file_.bak", "w");
    if (dest == NULL)
    {
        printf("Create new yaml file failed\n");
        return -1;
    }

    char line[MAX_LINE_CHAR_COUNT];

    bool is_cur_line_in_dependency = false; // 当前处理的行是不是 dependency 中的一行
    char *depency = NULL;                   // dependency 的名字
    char *depency_detail = NULL;            // dependency 的属性
    size_t detail_length = 0;               // dependency 的属性的字符串大小
    bool is_cur_denpency_private = false;

    while (fgets(line, MAX_LINE_CHAR_COUNT, src))
    {

        size_t line_length = strlen(line);

        if (is_cur_line_in_dependency)
        {
            is_cur_line_in_dependency = !is_cur_line_other_begin(line);
        }
        else
        {
            is_cur_line_in_dependency = is_cur_line_dependencies_begin(line);
        }
        printf("%s is_cur_line_in_dependency: %s\n", line, is_cur_line_in_dependency ? "true" : "false");
        printf("is_cur_line_other_begin: %s\n", is_cur_line_other_begin(line) ? "true" : "false");
        printf("is_cur_line_dependencies_begin: %s\n", is_cur_line_dependencies_begin(line) ? "true" : "false");

        if (!is_cur_line_in_dependency || is_cur_line_dependencies_begin(line))
        {
            if (is_cur_line_other_begin(line) && depency)
            {
                fprintf(dest, "%s", depency);
                fprintf(dest, "%s", depency_detail);
                depency = NULL;
                depency_detail = NULL;
                detail_length = 0;
            }
            fprintf(dest, "%s", line);
            continue;
        }

        if (prefix("  ", line) && !prefix("   ", line)) // 开始一个新的 dependency 或开始一行注释："  #"
        {
            /// 把旧得写入到dest中
            if (depency)
            {
                fprintf(dest, "%s", depency);
                free(depency);
                depency = NULL;
            }
            if(depency_detail)
            {
                fprintf(dest, "%s", depency_detail);
                free(depency_detail);
                depency_detail = NULL;
            }

            detail_length = 0;
            depency = calloc(sizeof(char), line_length + 1);
            memcpy(depency, line, line_length + 1);
            is_cur_denpency_private = false;
        }
        else // 处理一个 dependency 的属性：depency_detail
        {
            if (is_cur_denpency_private)
                continue;

            // todo get and check version

            if (strcmp("      url: *private_pub_server\n", line) == 0)
            {
                /// found!
                is_cur_denpency_private = true;
                if (depency_detail)
                    free(depency_detail);
                depency_detail = calloc(sizeof(char), strlen(path) + 1);
                memcpy(depency_detail, path, strlen(path) + 1);
            }
            else
            {
                // 把当前行 append 到 depency_detail 中
                char *s = calloc(sizeof(char), detail_length + line_length + 1);
                if (detail_length > 0)
                {
                    memcpy(s, depency_detail, detail_length);
                }
                memcpy(s + detail_length, line, line_length + 1);
                if (depency_detail)
                    free(depency_detail);
                depency_detail = s;
                detail_length += line_length;
            }
        }
    }

    fflush(dest);
    fclose(dest);
    fclose(src);
    return 0;
}

bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

bool is_cur_line_dependencies_begin(const char *line)
{
    return !strcmp(line, "dependencies:\n") || !strcmp(line, "dev_dependencies:\n");
}

bool is_cur_line_other_begin(const char *line)
{
    return !prefix(" ", line) && !prefix("\n", line) && !is_cur_line_dependencies_begin(line);
}