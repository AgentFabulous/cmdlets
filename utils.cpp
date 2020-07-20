#include <grp.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/unistd.h>

#include "utils.h"

void do_chown (const char *file_path,
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
