#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void print_cwd() {

  char cwd[1024];

  if (getcwd(cwd, sizeof(cwd)) != NULL) {

    write(STDOUT_FILENO, cwd, strlen(cwd));
    write(STDOUT_FILENO, "$ ", 2);
  } else {
    write(STDERR_FILENO, "shell: unable to get current directory\n", 39);
  }
}

int main() {

  char input[1024];

  while (1) {
    print_cwd();
  }

  return 0;
}
