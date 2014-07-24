#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

static void get_nobody(uid_t *puid, gid_t *pgid)
{
    struct passwd *pw;

    if ((pw = getpwnam("nobody"))) {
        *puid = pw->pw_uid;
        *pgid = pw->pw_gid;
        return;
    }

    *puid = *pgid = 65534;
}

int main(int argc, char **argv)
{
    if (argc != 2) {
        return 64;
    }

    uid_t uid;
    gid_t gid;

    get_nobody(&uid, &gid);

    if (setgid(gid)) {
        perror("setgid");
        return 1;
    }

    if (setgroups(1, &gid)) {
        perror("setgroups");
        return 1;
    }

    if (setuid(uid) == -1) {
        perror("setuid");
        return 1;
    }

    if (getuid() != uid  ||  geteuid() != uid) {
        return 32;
    }

    int ret = system(argv[1]);

    if (ret == -1) {
        perror("system");
    }

    return ret;
}
