/*
 * file:        internal.h
 * description: skeleton code for simple shell
 *
 * Haonan Lu, Northeastern CS5600 Fall 2023
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "internal.h"

int internal_cmd(int n_tokens, char **tokens) {
    if (!strcmp("cd", tokens[0])) {
        /* cd
         */
        return __cd(n_tokens - 1, &tokens[1]);
    } else if (!strcmp("pwd", tokens[0])) {
        /* pwd
         */
        return __pwd(n_tokens - 1, &tokens[1]);
    } else if (!strcmp("exit", tokens[0])) {
        /* exit
         */
        return __exit(n_tokens - 1, &tokens[1]);
    } else {
        printf("%s", tokens[0]);
        return -1;
    }
}

int __cd(int argc, char **argv) {
    if (argc > 1) {
        fprintf(stderr, "cd: wrong number of arguments\n");
        return 1;
    }
    if (chdir(argc == 0 ? getenv("HOME") : argv[0]) == -1) {
        fprintf(stderr, "cd: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}

int __pwd(int argc, char **argv) {
    if (argc != 0) {
        fprintf(stderr, "pwd: wrong number of arguments\n");
        return 1;
    }
    char buf[PATH_MAX];
    getcwd(buf, sizeof(buf));
    printf("%s\n", buf);

    return 0;
}

int __exit(int argc, char **argv) {
    if (argc > 1) {
        fprintf(stderr, "exit: too many arguments\n");
        return 1;
    }

    exit(argc == 0 ? 0 : atoi(argv[0]));
}
