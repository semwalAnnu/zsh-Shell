#include "execute.h"
#include "msgs.h"

int check_internal_command(int argc, char **argv) {

  char *command = argv[0];
  if (strcmp(command, "exit") == 0) {
    if (argc <= 1) {
      exit(0);
    } else {
      const char *msg = FORMAT_MSG("exit", TMA_MSG);
      write(STDERR_FILENO, msg, strlen(msg));
    }
    return 1;
  } else if (strcmp(command, "pwd")) {
  }

  return 0;
}
