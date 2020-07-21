void do_chown(const char *file_path,
               const char *user_name,
               const char *group_name);

void do_chmod(const char *mode, const char *file_path);

void cp_file(const char *src_f, const char *dst_f);

int validate_path(std::string& str);

bool is_number(const char* str);