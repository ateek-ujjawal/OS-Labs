/*
 * file:        shell56.c
 * description: skeleton code for simple shell
 *
 * Peter Desnoyers, Northeastern CS5600 Fall 2023
 */

/* <> means don't check the local directory */
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* "" means check the local directory */
#include "external.h"
#include "internal.h"
#include "parser.h"

/* you'll need these includes later: */
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
    bool interactive = isatty(STDIN_FILENO); /* see: man 3 isatty */
    FILE *fp = stdin;

    if (argc == 2) {
        interactive = false;
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: %s\n", argv[1], strerror(errno));
            exit(EXIT_FAILURE); /* see: man 3 exit */
        }
    }
    if (argc > 2) {
        fprintf(stderr, "%s: too many arguments\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char line[1024], linebuf[1024];
    const int max_tokens = 32;
    char *tokens[max_tokens];
    char qbuf[16];

    /* ignore SIGINT=^C
     */
    signal(SIGINT, SIG_IGN);

    /* loop:
     *   if interactive: print prompt
     *   read line, break if end of file
     *   tokenize it
     *   print it out <-- your logic goes here
     */
    while (true) {
        if (interactive) {
            /* print prompt. flush stdout, since normally the tty driver doesn't
             * do this until it sees '\n'
             */
            printf("$ ");
            fflush(stdout);
        }

        /* see: man 3 fgets (fgets returns NULL on end of file)
         */
        if (!fgets(line, sizeof(line), fp))
            break;

        /* read a line, tokenize it, and print it out
         */
        int n_tokens =
            parse(line, max_tokens, tokens, linebuf, sizeof(linebuf));
        /* replace the code below with your shell:
         */
        // printf("line:");
        // for (int i = 0; i < n_tokens; i++)
        //     printf(" '%s'", tokens[i]);
        // printf("\n");

        for (int i = 0; i < n_tokens; i++) {
            if (!strcmp("$?", tokens[i]))
                tokens[i] = qbuf;
            else if (!strcmp("|", tokens[i]))
                tokens[i] = NULL;
            
            if((i == 0 && tokens[i] == NULL) || (tokens[i] == NULL && tokens[i-1] == NULL))
    		n_tokens = 0;
        }

        int status = 0;

        if (n_tokens) {
            if (!strcmp("cd", tokens[0]) || !strcmp("pwd", tokens[0]) ||
                !strcmp("exit", tokens[0]))
                status = internal_cmd(n_tokens, tokens);
            else
                status = external_cmd(n_tokens, tokens);
        }
        sprintf(qbuf, "%d", status);
    }

    if (interactive)  /* make things pretty */
        printf("\n"); /* try deleting this and then quit with ^D */
}
