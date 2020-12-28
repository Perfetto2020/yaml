#define MAX_LINE_CHAR_COUNT 200
#define DEBUG true
#define YAML_FILE_NAME "pubspec.yaml"
#define YAML_BACKUP_FILE_NAME ".pubspec_y_a_m_l.swp"
#define YAML_KEY_NAME "name:"
#define YAML_KEY_VERSION "version:"

int load_local_dependency_info(char *packages_dir_name);

int replace_hosted_to_pathed(const char *yaml_file_name, const char *bak_file_name);