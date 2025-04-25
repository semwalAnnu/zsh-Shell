#include "execute.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
void execute(char *input) {

  char *token;
  char *saveptr;

  if ((input[0] != '!')) {
    store_command(input);
  }

  int mode = 0; // 0 for foreground, 1 for background

  token = strtok_r(input, " ", &saveptr);

  int argc = 0;
  char *argv[100];
  while (token != NULL) {
    argv[argc++] = token;
    token = strtok_r(NULL, " ", &saveptr);
  }

  if (argc > 0 && strcmp(argv[argc - 1], "&") == 0) {
    mode = 1;
    argv[argc - 1] = NULL;
  } else {
    argv[argc] = NULL;
  }

  if (argv[0] == NULL) {
    return;
  }

  int executed = check_internal_command(argc, argv);

  if (executed == 0) {
    pid_t pid = fork();

    if (pid < 0) {
      if (errno != EINTR) {
        write(STDERR_FILENO, "shell: unable to fork\n", 22);
        return;
      }
    }

    if (pid) {
      int wstatus = 0;
      // foreground
      if (mode == 0) {
        if (waitpid(pid, &wstatus, 0) == -1) {
          if (errno != EINTR) {
            write(STDERR_FILENO, "shell: unable to wait for children\n", 32);
            exit(EXIT_FAILURE);
          }
        }
      }

    } else {

      if (execvp(argv[0], argv) == -1) {
        if (errno != EINTR) {
          write(STDERR_FILENO, "shell: unable to execute command\n", 33);
          exit(EXIT_FAILURE);
        }
      }
    }
  }
}

void clean_zombies() {
  while (waitpid(-1, NULL, WNOHANG) > 0)
    ;
}
