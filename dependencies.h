#define MAX_LINE_CHAR_COUNT 200
typedef struct
{
    char *name;
    char *path;
    char *version;
} local_dependency_info;

void remove_spaces(char *s);

local_dependency_info *dependencies[32];

int load_local_dependency_info(char *packages_dir_name);

char *get_full_path_of_dependency(const char *name_line);