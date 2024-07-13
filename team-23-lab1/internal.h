/*
 * file:        internal.h
 * description: skeleton code for simple shell
 *
 * Haonan Lu, Northeastern CS5600 Fall 2023
 */

#ifndef __INTERNAL_H__
#define __INTERNAL_H__

/* function declarations:
 */

int internal_cmd(int n_tokens, char **tokens);
int __cd(int argc, char **argv);
int __pwd(int argc, char **argv);
int __exit(int argc, char **argv);

#endif
