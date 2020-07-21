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

#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#include "cmd.h"
#include "utils.h"

int cmd_processor(char *cmd) {
    printf("%s: %s\n", __func__, cmd);
    std::istringstream iss(cmd);
    std::vector<std::string> result{
        std::istream_iterator<std::string>(iss), {}
    };

    const char *_cmd = result[0].c_str();

    if (strcmp(_cmd, "cp") == 0) {
        return cmd_cp(result);
    } else if (strcmp(_cmd, "chmod") == 0) {
        return cmd_chmod(result);
    } else if (strcmp(_cmd, "chown") == 0) {
        return cmd_chown(result);
    } else if (strcmp(_cmd, "fixperms") == 0) {
        return cmd_fixperms(result);
    } else if (strcmp(_cmd, "del") == 0) {
        return cmd_del(result);
    } else if (strcmp(_cmd, "mkdir") == 0) {
        return cmd_mkdir(result);
    }

    return -1;
}

int cmd_cp(std::vector<std::string>& input) {
    if (input.size() != 3) return 1;
#ifdef IS_HOST
    const char* _path = input[2].c_str();
#else
    const char *prefix = "/data/extra/";
    char *_path = (char*) malloc(strlen(input[2].c_str()) + strlen(prefix) + 1);
    strcpy(_path, prefix);
    strcat(_path, input[2].c_str());
#endif
    cp_file(input[1].c_str(), _path);
    return 0;
}

int cmd_chmod(std::vector<std::string>& input) {
    if (input.size() != 3) return 1;
#ifdef IS_HOST
    const char* _path = input[2].c_str();
#else
    const char *prefix = "/data/extra/";
    char *_path = (char*) malloc(strlen(input[2].c_str()) + strlen(prefix) + 1);
    strcpy(_path, prefix);
    strcat(_path, input[2].c_str());
#endif
    do_chmod(input[1].c_str(), _path);
    return 0;
}

int cmd_chown(std::vector<std::string>& input) {
    if (input.size() != 4) return 1;

#ifdef IS_HOST
    const char *_path = input[1].c_str();
#else
    const char *prefix = "/data/extra/";
    char *_path = (char*) malloc(strlen(input[1].c_str()) + strlen(prefix) + 1);
    strcpy(_path, prefix);
    strcat(_path, input[1].c_str());
#endif

    do_chown(_path, input[2].c_str(), input[3].c_str());
    return 0;
}

int cmd_fixperms(std::vector<std::string>& input) {
#ifdef IS_HOST
    printf("chown and chmod as necessary here.\n");
#else
    system("chown -R root:root /data/extra");
    system("chmod -R 0755 /data/extra");
    system("chmod -R 0644 $(find /data/extra -name *.apk)");
#endif

    return 0;
}

int cmd_del(std::vector<std::string>& input) {
    if (input.size() != 2) return 1;
    std::string path = input[1];
    if (validatePath(path)) {
#ifdef IS_HOST
        const char *_del = "rm -rf ";
#else
        const char *_del = "rm -rf /data/extra/";
#endif
        char* del_cmd = (char*) malloc(strlen(_del) + strlen(path.c_str()) + 1);
        strcpy(del_cmd, _del);
        strcat(del_cmd, path.c_str());
        system(del_cmd); // consider using exec family here
    }
    return 0;
}

int cmd_mkdir(std::vector<std::string>& input) {
    if (input.size() != 2) return 1;
    std::string path = input[1];
    if (validatePath(path)) {
#ifdef IS_HOST
        const char * _mkdir = "mkdir -p ";
#else
        const char * _mkdir = "mkdir -p /data/extra/";
#endif
        char* mkdir_cmd = (char*) malloc(strlen(_mkdir) + strlen(path.c_str()) + 1);
        strcpy(mkdir_cmd, + _mkdir);
        strcat(mkdir_cmd, path.c_str());
        system(mkdir_cmd); // consider using exec family here
    }
    return 0;
}