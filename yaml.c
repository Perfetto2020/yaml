#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_LINE_CHAR_COUNT 100

int replace_hosted_to_pathed(const char *yaml_file_name, const char *path);
bool prefix(const char *pre, const char *str);

int main(int argc, char *argv[])
{
    replace_hosted_to_pathed("pubspec.yaml", "    path: the_path_the_package\n");
}

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

    while (fgets(line, MAX_LINE_CHAR_COUNT, src))
    {
        fprintf(dest, "%s", line);
        if (!strcmp(line, "dependencies:\n"))
            break;
    }
    char *depency = NULL;
    char *depency_detail = NULL;
    size_t detail_length = 0;
    bool is_cur_denpency_private = false;

    while (fgets(line, MAX_LINE_CHAR_COUNT, src))
    {
        size_t line_length = strlen(line);

        if (prefix("  ", line) && !prefix("   ", line) && !prefix("  #", line))
        {
            /// 新的depency 开始了，把旧得写入到dest中
            fprintf(dest, "%s", depency);
            fprintf(dest, "%s", depency_detail);

            if (depency)
                free(depency);
            detail_length = 0;
            depency = calloc(sizeof(char), line_length + 1);
            memcpy(depency, line, line_length + 1);
            is_cur_denpency_private = false;
        }
        else
        {
            if (is_cur_denpency_private)
                continue;

            // todo get and check version
            char *s = calloc(sizeof(char), detail_length + strlen(line) + 1);
            memcpy(s, depency_detail, detail_length);
            memcpy(s + detail_length, line, line_length + 1);
            if (depency_detail)
                free(depency_detail);
            depency_detail = s;
            if (strcmp("      url: *private_pub_server\n", line) == 0)
            {
                /// found!
                is_cur_denpency_private = true;
                if (depency_detail)
                    free(depency_detail);
                depency_detail = calloc(sizeof(char), strlen(path) + 1);
                memcpy(depency_detail, path, strlen(path) + 1);
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