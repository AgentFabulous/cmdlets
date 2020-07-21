#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <vector>

#include "constants.h"
#include "socket.h"
#include "utils.h"


int client(int argc, char **argv) {
    int s, t;
    char str_recv[100];

    s = create_socket();
    connect_socket(s);

    int i, v = 0, size = argc - 1;

    char *str = (char *) malloc(v);

    for(i = 1; i <= size; i++) {
        str = (char *) realloc(str, (v + strlen(argv[i])));
        strcat(str, argv[i]);
        strcat(str, " ");
    }

    if (send(s, str, strlen(str)+1, 0) == -1) {
        perror("send");
        exit(1);
    }
    if ((t=recv(s, str_recv, strlen(str)+1, 0)) > 0) {
        int check = strcmp(str, str_recv);
        if (check != 0) exit(1);
    } else {
        if (t < 0) perror("recv");
        exit(1);
    }

    close(s);

    return 0;
}

int cmd_processor(char *cmd) {
    printf("%s: %s\n", __func__, cmd);
    std::istringstream iss(cmd);
    std::vector<std::string> result{
        std::istream_iterator<std::string>(iss), {}
    };

    const char *_cmd = result[0].c_str();
    if (strcmp(_cmd, "cp") == 0) {
        if (result.size() != 3) return 1;
        cp_file(result[1].c_str(), result[2].c_str());
    } else if (strcmp(_cmd, "chmod") == 0) {
        if (result.size() != 3) return 1;
        do_chmod(result[1].c_str(), result[2].c_str());
    } else if (strcmp(_cmd, "chown") == 0) {
        if (result.size() != 4) return 1;
        do_chown(result[1].c_str(), result[2].c_str(), result[3].c_str());
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

int server() {
    int s, s2;
    socklen_t t;
    struct sockaddr_un remote;
    char str[100];

    s = create_socket();
    bind_socket(s);
#ifndef IS_HOST
    do_chown(SOCK_PATH, "system", "system");
    chmod(SOCK_PATH, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
#endif
    if (listen(s, 5) == -1) {
        perror("listen");
        exit(1);
    }

    while(1) {
        int done, n;
        t = sizeof(remote);
        if ((s2 = accept(s, (struct sockaddr *)&remote, &t)) == -1) {
            perror("accept");
            exit(1);
        }
        done = 0;
        while (!done) {
            n = recv(s2, str, 100, 0);
            if (n <= 0) {
                if (n < 0) perror("recv");
                done = 1;
                continue;
            }
            if (send(s2, str, n, 0) < 0) {
                perror("send");
                done = 1;
            }
            cmd_processor(str);
        }
        close(s2);
    }
    return 0;
}

int main(int argc, char **argv) {
    if (strcmp(argv[1], "--server") == 0) {
        return server();
    } else {
        return client(argc, argv);
    }
}
