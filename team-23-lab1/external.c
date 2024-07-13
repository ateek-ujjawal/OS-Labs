#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "external.h"

int external_cmd(int n_tokens, char **token) {
    int start = 0, pid_count = 0, exit_status = 0, status;
    pid_t pids[9];
    int infd = 0, outfd, pipefd[8][2], cmd = 0;
    for (int i = 0; i <= n_tokens; i++) {
        if (token[i] == NULL) {
            if (pipe(pipefd[cmd]) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
                exit(EXIT_FAILURE);
            }
            cmd++;
        }
    }
    for (int i = 0; i <= n_tokens; i++) {
        if (token[i] == NULL) {
            if (pid_count != 0)
                infd = pipefd[pid_count - 1][0];

            if (i != n_tokens)
                outfd = pipefd[pid_count][1];
            else
                outfd = 1;

            if (token[start] != NULL) {
                pids[pid_count] =
                    fork_process(&token[start], infd, outfd, pipefd, cmd);
                if (pids[pid_count] == -1) {
                    fprintf(stderr, "%s\n", strerror(errno));
                }
                pid_count++;
            }
            start = i + 1;
        }
    }
    for (int i = 0; i < pid_count; i++) {
        close(pipefd[i][0]);
        close(pipefd[i][1]);
        do {
            waitpid(pids[i], &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        exit_status = WEXITSTATUS(status);
    }

    return exit_status;
}

int fork_process(char **token, int infd, int outfd, int pipefd[8][2], int cmd) {
    pid_t child = fork();
    if (child == 0) {
        signal(SIGINT, SIG_DFL);
        
        // check file redirect
        int inre = -1, outre = -1;
        int idx = 0;
        while (token[idx] != NULL) {
            if (!strcmp("<", token[idx])) {
                if (inre == -1)
                    inre = idx;
                token[idx] = NULL;
            } else if (!strcmp(">", token[idx])) {
                if (outre == -1)
                    outre = idx;
                token[idx] = NULL;
            }
            idx++;
            
            if((idx-1 == 0 && token[idx-1] == NULL) || (token[idx] == NULL && token[idx-1] == NULL)) {
                for (int i = 0; i < cmd; i++) {
            		close(pipefd[i][0]);
            		close(pipefd[i][1]);
        	}
        	exit(0);	
            }
        }

        if (inre != -1) {
            char *infile = token[inre + 1];
            if (infile != NULL)
                infd = open(infile, O_RDONLY);
        }
        if (outre != -1) {
            char *outfile = token[outre + 1];
            if (outfile != NULL)
                outfd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC,
                             S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
        }
        
        dup2(infd, 0);
        dup2(outfd, 1);
        for (int i = 0; i < cmd; i++) {
            close(pipefd[i][0]);
            close(pipefd[i][1]);
        }
        if (execvp(token[0], token) == -1) {
            fprintf(stderr, "%s: %s\n", token[0], strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    return child;
}
