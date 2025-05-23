#include "execute.h"
#include "globals.h"
#include "msgs.h"
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int curr_index = 0;
int totalCommands = 0;
char *last_ten_commands[10] = {NULL};
int CTRL_C = 0;

void print_cwd() {

  char cwd[1024];

  if (getcwd(cwd, sizeof(cwd)) != NULL) {

    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, "$ ", 2);
  } else {
    write(STDERR_FILENO, "shell: unable to get current directory\n", 39);
  }
}

void handle_sigint(int signum) {
  const char *msg = FORMAT_MSG("\nexit", EXIT_HELP_MSG);
  write(STDOUT_FILENO, msg, strlen(msg));

  const char *msg2 = FORMAT_MSG("pwd", PWD_HELP_MSG);
  write(STDOUT_FILENO, msg2, strlen(msg2));

  const char *msg3 = FORMAT_MSG("cd", CD_HELP_MSG);
  write(STDOUT_FILENO, msg3, strlen(msg3));

  const char *msg1 = FORMAT_MSG("help", HELP_HELP_MSG);
  write(STDOUT_FILENO, msg1, strlen(msg1));

  const char *msg4 = FORMAT_MSG("history", HISTORY_HELP_MSG);
  write(STDOUT_FILENO, msg4, strlen(msg4));
  CTRL_C = 1;
  // write(STDOUT_FILENO, "\n", 1);
}

void store_command(char *command) {

  if (last_ten_commands[curr_index] != NULL) {
    free(last_ten_commands[curr_index]);
  }
  last_ten_commands[totalCommands % 10] = strdup(command);
  totalCommands++;
  curr_index = (curr_index + 1) % 10;
}

void registerSignal() {
  struct sigaction act;
  act.sa_handler = handle_sigint;
  act.sa_flags = 0;
  sigemptyset(&act.sa_mask);

  int ret = sigaction(SIGINT, &act, NULL);
  if (ret == -1) {
    perror("FAILED SIGACTION()");
    exit(EXIT_FAILURE);
  }
}

int main() {

  char input[1024];
  registerSignal();
  curr_index = 0;
  totalCommands = 0;
  while (1) {
    print_cwd();
    ssize_t size = read(STDIN_FILENO, input, 1024);

    if (size <= 0) {
      if (errno != EINTR) {
        write(STDERR_FILENO, "shell: unable to read command\n", 30);
        exit(EXIT_FAILURE);
      }
    }
    input[size - 1] = '\0'; //

    if (CTRL_C == 1) {
      CTRL_C = 0;
      continue;
    }

    execute(input);
    clean_zombies();
  }

  return 0;
}
