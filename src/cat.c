#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT 1024

int main(int argc, char *argv[]) {
  FILE *file;
  char filename[MAX_INPUT];

  if (argc < 2) {
    if (fgets(filename, sizeof(filename), stdin) == NULL) {
      fprintf(stderr, "Error reading filename\n");
      return 1;
    }
    // Remove newline character if present
    filename[strcspn(filename, "\n")] = 0;
  } else {
    strncpy(filename, argv[1], sizeof(filename) - 1);
    filename[sizeof(filename) - 1] = '\0';
  }

  file = fopen(filename, "r");
  if (file == NULL) {
    fprintf(stderr, "File not found\n");
    return 1;
  }

  char buffer[MAX_INPUT];
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    fprintf(stdout, "%s", buffer);
  }

  fclose(file);
  return 0;
}
