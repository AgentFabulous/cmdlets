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
        if (result.size() != 3) return 1;
#ifdef IS_HOST
        const char* _path = result[2].c_str();
#else
        const char *prefix = "/data/extra/";
        char *_path = (char*) malloc(strlen(dst_f) + strlen(prefix) + 1);
        strcpy(_path, prefix);
        strcat(_path, result[2].c_str());
#endif
        cp_file(result[1].c_str(), result[2].c_str());
    } else if (strcmp(_cmd, "chmod") == 0) {
        if (result.size() != 3) return 1;
        do_chmod(result[1].c_str(), result[2].c_str());
    } else if (strcmp(_cmd, "chown") == 0) {
        if (result.size() != 4) return 1;
#ifdef IS_HOST
        const char *_path = result[1].c_str();
#else
        const char *prefix = "/data/extra/";
        char *_path = (char*) malloc(strlen(result[1].c_str()) + strlen(prefix) + 1);
        strcpy(_path, prefix);
        strcat(_path, result[1].c_str());
#endif
        do_chown(_path, result[2].c_str(), result[3].c_str());
    } else if (strcmp(_cmd, "fixperms") == 0) {
#ifdef IS_HOST
        printf("chown and chmod as necessary here.\n");
#else
        system("chown -R root:root /data/extra");
        system("chmod -R 0755 /data/extra");
        system("chmod -R 0644 $(find /data/extra -name *.apk)");
#endif
    } else if (strcmp(_cmd, "del") == 0) {
        if (result.size() != 2) return 1;
        std::string path = result[1];
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
    } else if (strcmp(_cmd, "mkdir") == 0) {
        if (result.size() != 2) return 1;
        std::string path = result[1];
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
    }
    return 0;
}