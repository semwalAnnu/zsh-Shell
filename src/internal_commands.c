#include "execute.h"
#include "globals.h"
#include "msgs.h"
#include <ctype.h>
#include <pwd.h>
#include <stdio.h>
#include <string.h>

char prevDir[1024] = ".";

int check_internal_command(int argc, char **argv) {

  char *command = argv[0];
  if (strcmp(command, "exit") == 0) {
    if (argc <= 1) {
      // printf("exiting\n");
      clean_zombies();
      exit(0);
      // printf("exited");
    } else {
      const char *msg = FORMAT_MSG("exit", TMA_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
    }
    return 1;
  } else if (strcmp(command, "pwd") == 0) {
    if (argc <= 1) {

      char cwd[1024];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        write(STDOUT_FILENO, cwd, strlen(cwd));
        write(STDOUT_FILENO, "\n", 1);
      } else {
        const char *msg = FORMAT_MSG("pwd", GETCWD_ERROR_MSG);
        write(STDERR_FILENO, msg, strlen(msg));
      }
    } else {
      const char *msg = FORMAT_MSG("pwd", TMA_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
    }

    return 1;
  } else if (strcmp(command, "cd") == 0) {

    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
      const char *msg = FORMAT_MSG("cwd", GETCWD_ERROR_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
    }

    if (argc <= 2) {
      char *dir = argv[1];

      if (argc == 2) {

        if (argv[1][0] == '~') {

          struct passwd *pw = getpwuid(getuid());
          if (pw != NULL) {
            if (chdir(pw->pw_dir) != 0) {
              const char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
              write(STDERR_FILENO, msg, strlen(msg));
            }
          }

          if (argv[1][1] != '\0') {
            char full_path[1024];
            if (argv[1][1] == '/') {
              snprintf(full_path, sizeof(full_path), "%s%s", pw->pw_dir,
                       argv[1] + 1);

            } else {
              const char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
              write(STDERR_FILENO, msg, strlen(msg));
            }
            if (chdir(full_path) != 0) {
              const char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
              write(STDERR_FILENO, msg, strlen(msg));
            }
          }

          return 1;
        } else if (strcmp(argv[1], "-") == 0) {
          if (chdir(prevDir) != 0) {
            const char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
            write(STDERR_FILENO, msg, strlen(msg));
          }
          return 1;
        }

        if (chdir(dir) != 0) {
          const char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
          write(STDERR_FILENO, msg, strlen(msg));
        }
        strcpy(prevDir, cwd);

      } else {
        if (getHomeDir() != NULL) {
          if (chdir(getHomeDir()) != 0) {
            const char *msg = FORMAT_MSG("cd", CHDIR_ERROR_MSG);
            write(STDERR_FILENO, msg, strlen(msg));
          }
        }
      }
    } else {
      const char *msg = FORMAT_MSG("cd", TMA_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
    }

    // strcpy(prevDir, cwd);
    return 1;
  } else if (strcmp(command, "help") == 0) {
    // write(STDOUT_FILENO, "helping\n", 8);
    if (argc <= 2) {

      char *helpCom = argv[1];
      if (argc == 2) {

        if (strcmp(helpCom, "exit") == 0) {
          const char *msg = FORMAT_MSG("exit", EXIT_HELP_MSG);
          write(STDOUT_FILENO, msg, strlen(msg));
        } else if (strcmp(helpCom, "help") == 0) {
          const char *msg = FORMAT_MSG("help", HELP_HELP_MSG);
          write(STDOUT_FILENO, msg, strlen(msg));
        } else if (strcmp(helpCom, "pwd") == 0) {
          const char *msg = FORMAT_MSG("pwd", PWD_HELP_MSG);
          write(STDOUT_FILENO, msg, strlen(msg));
        } else if (strcmp(helpCom, "cd") == 0) {
          const char *msg = FORMAT_MSG("cd", CD_HELP_MSG);
          write(STDOUT_FILENO, msg, strlen(msg));
        } else if (strcmp(helpCom, "history") == 0) {
          const char *msg4 = FORMAT_MSG("history", HISTORY_HELP_MSG);
          write(STDOUT_FILENO, msg4, strlen(msg4));

        } else {
          const char *msg = FORMAT_MSG("", EXTERN_HELP_MSG);
          write(STDOUT_FILENO, helpCom, strlen(helpCom));
          write(STDOUT_FILENO, msg, strlen(msg));
        }

      } else {
        const char *msg = FORMAT_MSG("exit", EXIT_HELP_MSG);
        write(STDOUT_FILENO, msg, strlen(msg));

        const char *msg2 = FORMAT_MSG("pwd", PWD_HELP_MSG);
        write(STDOUT_FILENO, msg2, strlen(msg2));

        const char *msg3 = FORMAT_MSG("cd", CD_HELP_MSG);
        write(STDOUT_FILENO, msg3, strlen(msg3));

        const char *msg1 = FORMAT_MSG("help", HELP_HELP_MSG);
        write(STDOUT_FILENO, msg1, strlen(msg1));

        const char *msg4 = FORMAT_MSG("history", HISTORY_HELP_MSG);
        write(STDOUT_FILENO, msg4, strlen(msg4));
      }

    } else {
      const char *msg = FORMAT_MSG("cd", TMA_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
    }

    return 1;
  } else if (strcmp(command, "history") == 0) {
    int count;
    if (totalCommands >= 10) {
      count = 10;
    } else {
      count = totalCommands;
    }

    for (int i = 0; i < count; i++) {
      char instance[1024];
      int num = totalCommands - i - 1;
      int index = (curr_index - 1 - i + 10) % 10;
      snprintf(instance, sizeof(instance), "%d\t%s\n", num,
               last_ten_commands[index]);
      write(STDOUT_FILENO, instance, strlen(instance));
    }

    return 1;
  } else if (argv[0][0] == '!') {
    char argument[20];
    strcpy(argument, &argv[0][1]);

    if (argument[0] == '!') {
      if (totalCommands < 1) {
        const char *msg = FORMAT_MSG("history", HISTORY_NO_LAST_MSG);
        write(STDOUT_FILENO, msg, strlen(msg));
      } else {

        char *last_command =
            strdup(last_ten_commands[(totalCommands - 1) % 10]);
        write(STDOUT_FILENO, last_command, strlen(last_command));
        write(STDOUT_FILENO, "\n", 1);
        if (last_command[0] == '!') {
          store_command(last_command);
        }
        execute(last_command);
        free(last_command);
      }
      return 1;
    }

    if (!is_number(argument)) {
      const char *msg = FORMAT_MSG("history", HISTORY_INVALID_MSG);
      write(STDOUT_FILENO, msg, strlen(msg));
      if (last_ten_commands[curr_index] != NULL) {
        free(last_ten_commands[curr_index]);
      }
      last_ten_commands[curr_index] = strdup(argv[0]);
      totalCommands++;
      curr_index = (curr_index + 1) % 10;
      return 1;
    }

    int min_index = 0;
    if (totalCommands > 10) {
      min_index = totalCommands - 10;
    }

    int n = atoi(argument);
    if (n >= totalCommands || n < min_index) {
      const char *msg = FORMAT_MSG("history", HISTORY_INVALID_MSG);
      write(STDOUT_FILENO, msg, strlen(msg));
      if (last_ten_commands[curr_index] != NULL) {
        free(last_ten_commands[curr_index]);
      }
      last_ten_commands[curr_index] = strdup(argv[0]);
      totalCommands++;
      curr_index = (curr_index + 1) % 10;
      return 1;
    }

    char *last_command = strdup(last_ten_commands[(n % 10)]);
    write(STDOUT_FILENO, last_command, strlen(last_command));
    write(STDOUT_FILENO, "\n", 1);
    execute(last_command);
    free(last_command);

    return 1;
  }

  return 0;
}

const char *getHomeDir() {
  struct passwd *pw = getpwuid(getuid());
  if (pw == NULL) {
    return NULL;
  }
  return (const char *)(pw->pw_dir);
}

int is_number(const char *str) {

  if (*str == '\0')
    return 0;

  while (*str) {
    if (!isdigit(*str))
      return 0;
    str++;
  }
  return 1;
}
