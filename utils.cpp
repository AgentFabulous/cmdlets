/*
 * Copyright (C) 2020 The Potato Open Sauce Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <cstring>
#include <fstream>
#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "utils.h"

void do_chown(const char *file_path,
               const char *user_name,
               const char *group_name) {
    uid_t          uid;
    gid_t          gid;
    struct passwd *pwd;
    struct group  *grp;

    pwd = getpwnam(user_name);
    if (pwd == NULL) {
        printf("Failed to get uid");
        exit(1);
    }
    uid = pwd->pw_uid;

    grp = getgrnam(group_name);
    if (grp == NULL) {
        printf("Failed to get gid");
        exit(1);
    }
    gid = grp->gr_gid;

    if (chown(file_path, uid, gid) == -1) {
        perror("chown");
        exit(1);
    }
}

void do_chmod(const char *mode, const char *file_path) {
    if (!is_number(mode)) { printf("u nub"); return; }
    if (chmod(file_path,strtol(mode, 0, 8)) < 0) {
        perror("chmod");
        exit(1);
    }
}

void cp_file(const char *src_f, const char *dst_f) {
    std::ifstream src(src_f, std::ios::binary);
    std::ofstream dst(dst_f, std::ios::binary);
    dst << src.rdbuf();
}

int validate_path(std::string& str) {
    std::string root("/");
    std::string parent("..");
    std::string term(";");
    std::string _and("&&");
    std::string _or("||");
    std::string dollar("$");
    std::string b1("(");
    std::string b2(")");

    return !(str.compare("/") == 0 ||
                str.find(parent) != std::string::npos ||
                str.find(term) != std::string::npos ||
                str.find(_and) != std::string::npos ||
                str.find(_or) != std::string::npos ||
                str.find(dollar) != std::string::npos ||
                str.find(b1) != std::string::npos ||
                str.find(b2) != std::string::npos);
}

bool is_number(const char* str) {
    return(strspn(str, "0123456789") == strlen(str));
}
