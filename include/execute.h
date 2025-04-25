#define _POSIX_C_SOURCE 200809L
#ifndef EXECUTE_H
#define EXECUTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void execute(char *input);
void clean_zombies();
int check_internal_command(int argc, char **argv);
const char *getHomeDir();
void handle_sigint(int signum);
void registerSignal();
int is_number(const char *str);
void store_command(char *command);
#endif
