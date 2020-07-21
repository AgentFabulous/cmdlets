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

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/unistd.h>

#include "constants.h"
#include "socket.h"

int create_socket() {
    int sockfd;
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    return sockfd;
}

struct sockaddr_un get_sockaddr() {
    struct sockaddr_un _sockaddr;
    _sockaddr.sun_family = AF_UNIX;
    strcpy(_sockaddr.sun_path, SOCK_PATH);
    return _sockaddr;
}

void connect_socket(int sockfd) {
    struct sockaddr_un remote = get_sockaddr();
    int len = strlen(remote.sun_path) + sizeof(remote.sun_family);
    if (connect(sockfd, (struct sockaddr *)&remote, len) == -1) {
        perror("connect");
        exit(1);
    }
}

void bind_socket(int sockfd) {
    struct sockaddr_un local = get_sockaddr();
    unlink(local.sun_path);
    int len = strlen(local.sun_path) + sizeof(local.sun_family);
    if (bind(sockfd, (struct sockaddr *)&local, len) == -1) {
        perror("bind");
        exit(1);
    }
}