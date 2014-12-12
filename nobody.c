#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <grp.h>

#define ERR_ARG (1)
#define ERR_SYS (2)
#define ERR_UID (3)

#define DEF_NOBODY (65534)

static void get_nobody(uid_t *puid, gid_t *pgid)
{
    struct passwd *pw;

    if ((pw = getpwnam("nobody"))) {
        *puid = pw->pw_uid;
        *pgid = pw->pw_gid;
        return;
    }

    *puid = *pgid = DEF_NOBODY;
}

static int set_nobody(void)
{
    uid_t uid;
    gid_t gid;

    get_nobody(&uid, &gid);

    if (setgid(gid)) {
        perror("setgid");
        return ERR_SYS;
    }

    if (setgroups(1, &gid)) {
        perror("setgroups");
        return ERR_SYS;
    }

    if (setuid(uid) == -1) {
        perror("setuid");
        return ERR_SYS;
    }

    if (getuid() != uid  ||  geteuid() != uid) {
        return ERR_UID;
    }

    return EXIT_SUCCESS;
}

static int exec_proxy(int argc, char **argv)
{
    char **exec_argv = (char**) malloc(sizeof (char*) * argc);
    int i;

    for (i = 1; i < argc; i++) {
        exec_argv[i - 1] = argv[i];
    }

    exec_argv[argc - 1] = NULL;

    if (execvp(argv[1], exec_argv) == -1) {
        perror("execv");
        free(exec_argv);
        return ERR_SYS;
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    int ret;

    if (argc < 2) {
        return ERR_ARG;
    }

    if ((ret = set_nobody())) {
        return ret;
    }

    if ((ret = exec_proxy(argc, argv))) {
        return ret;
    }

    return EXIT_SUCCESS;
}
