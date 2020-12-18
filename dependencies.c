#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "dependencies.h"

#define YAML_NAME "pubspec.yaml"
#define YAML_KEY_NAME "name:"
#define YAML_KEY_VERSION "version:"

/**
 * 存放 packages 的根目录: "/Users/wangpan/Work/Mi/mi_flutter_plugins/packages"
 * */
static char *root_packages_dir_path;

static bool local_dependency_loaded = false;
int dependencies_count = 0;
void init_root_packages_dir_path(char *origin_root_path);

int load_local_dependency_info(char *packages_dir_name)
{
    if(packages_dir_name == NULL)
    {
        fprintf(stderr, "packages_dir_name can not be NULL");
        return -1;
    }
    init_root_packages_dir_path(packages_dir_name);

    DIR *dir;
    struct dirent *dir_ent;
    dir = opendir(root_packages_dir_path);
    int count = 0;

    FILE *pubspec_yaml;
    if (dir)
    {
        while ((dir_ent = readdir(dir)) != NULL)
        {
            if (dir_ent->d_type == DT_DIR && strcmp(dir_ent->d_name, ".") && strcmp(dir_ent->d_name, ".."))
            {
                char *dir_name = calloc(sizeof(char), strlen(dir_ent->d_name) + 1);
                memcpy(dir_name, dir_ent->d_name, strlen(dir_ent->d_name));

                dependencies[count] = calloc(sizeof(local_dependency_info), 1);
                dependencies[count]->path = dir_name;

                char *pubspec_file_path;
                asprintf(&pubspec_file_path, "%s%s/%s", root_packages_dir_path, dir_name, YAML_NAME);

                if ((pubspec_yaml = fopen(pubspec_file_path, "r")) != NULL)
                {
                    char line[MAX_LINE_CHAR_COUNT];
                    bool name_found = false;
                    bool version_found = false;
                    size_t line_length;
                    size_t key_name_length = strlen(YAML_KEY_NAME);
                    size_t key_version_length = strlen(YAML_KEY_VERSION);
                    while (fgets(line, MAX_LINE_CHAR_COUNT, pubspec_yaml))
                    {
                        line_length = strlen(line);
                        if (!strncmp(line, YAML_KEY_NAME, strlen(YAML_KEY_NAME)))
                        {
                            char *name;
                            asprintf(&name, "%.*s", line_length - key_name_length - 1, line + key_name_length);
                            remove_spaces(name);
                            dependencies[count]->name = name;
                            name_found = true;
                        }
                        else if (!strncmp(line, YAML_KEY_VERSION, strlen(YAML_KEY_VERSION)))
                        {
                            char *version;
                            asprintf(&version, "%.*s", line_length - key_version_length - 1, line + key_version_length);
                            remove_spaces(version);
                            dependencies[count]->version = version;
                            version_found = true;
                        }
                        if (name_found && version_found)
                            break;
                    }
                    fclose(pubspec_yaml);
                    count++;
                }
                else
                {
                    fprintf(stderr, "Open %s in %s failed! \n", YAML_NAME, dir_name);
                    free(dir_name);
                    free(dependencies[count]);
                }
                free(pubspec_file_path);
            }
        }
    }
    else
    {
        return -1;
    }
    dependencies_count = count;
    local_dependency_loaded = true;
    return dependencies_count;
}

char *get_path_of_dependency(char *denpendency_name)
{
    for (int i = 0; i < dependencies_count; i++)
    {
        if(!strcmp(dependencies[i]->name, denpendency_name))
        {
            return dependencies[i]->path;
        }
    }
    fprintf(stderr, "%s NOT found! \n", denpendency_name);
    return NULL;
}

/**
 * name_line: "  card_ocr:\n"
 * */
char *get_full_path_of_dependency(const char *name_line)
{
    if(!local_dependency_loaded)
    {
        fprintf(stderr, "Please load local denpendency first!\n");
        return NULL;
    }
    // name_line: "  card_ocr:\n"
    char *name;
    asprintf(&name, "%s", name_line);
    remove_spaces(name);
    name[strlen(name) - 2] = '\0';
    char *path = get_path_of_dependency(name);
    if(!path)
    {
        return NULL;
    }
    char *ret;
    asprintf(&ret, "%s%s", root_packages_dir_path, path);
    free(name);
    return ret;
}

void init_root_packages_dir_path(char *origin_root_path)
{
    size_t origin_length = strlen(origin_root_path);
    bool need_slash = origin_root_path[origin_length - 1] != '/';
    asprintf(&root_packages_dir_path, "%s%s", origin_root_path, need_slash ? "/": "");
}

void remove_spaces(char *s)
{
    const char *d = s;
    do
    {
        while (*d == ' ')
        {
            ++d;
        }
    } while (*s++ = *d++);
}