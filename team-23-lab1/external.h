/*
 * file:        external.h
 * description: skeleton code for simple shell
 *
 * Ateek Ujjawal, Northeastern CS5600 Fall 2023
 */

#ifndef __EXTERNAL_H__
#define __EXTERNAL_H__

/* function declarations:
 */

int external_cmd(int n_tokens, char **token);
int fork_process(char **token, int infd, int outfd, int pipefd[8][2], int cmd);

#endif
