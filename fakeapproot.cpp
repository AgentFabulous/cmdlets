#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/unistd.h>

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
            printf("Processing cmd: %s\n", str);
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
