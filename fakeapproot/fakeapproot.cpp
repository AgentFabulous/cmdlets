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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/unistd.h>
#include <vector>

#include "constants.h"
#include "cmd.h"
#include "socket.h"
#include "utils.h"


int client(int argc, char **argv) {
    int s, t;
    char str_recv[1024];

    s = create_socket();
    connect_socket(s);

    int i, v = 0, size = argc - 1;

    char *str = (char *) malloc(v);

    for(i = 1; i <= size; i++) {
        v += strlen(argv[i]) + 1;
        str = (char *) realloc(str, v);
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

int server() {
    int s, s2;
    socklen_t t;
    struct sockaddr_un remote;
    char str[1024];

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
            n = recv(s2, str, 1024, 0);
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
