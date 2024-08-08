#include <limits.h>
#include <stdio.h>
#include <string.h>

int main() {

  char input[MAX_INPUT];
  memset(input, 0, MAX_INPUT);

  while (fgets(input, MAX_INPUT, stdin) != NULL) {
    fprintf(stdout, "%s", input);
  }

  return 0;
}
