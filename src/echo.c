#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

void print_help() {
  printf("Usage: echo [OPTION]... [STRING]...\n");
  printf("Echo the STRING(s) to standard output.\n\n");
  printf("  -n             do not output the trailing newline\n");
  printf("  -h, --help     display this help and exit\n\n");
  printf("If no arguments are provided, echo reads from standard input.\n");
  exit(0);
}

int main(int argc, char *argv[]) {
  int i;
  int newline = 1; // By default, print a newline
  char buffer[BUFFER_SIZE];

  // Check for help flag
  for (i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
      print_help();
    }
  }

  // Check if the first argument is -n
  if (argc > 1 && strcmp(argv[1], "-n") == 0) {
    newline = 0; // Don't print a newline
    i = 2;       // Start from the third argument
  } else {
    i = 1; // Start from the second argument
  }

  // Check if there's input from a pipe
  if (!isatty(STDIN_FILENO)) {
    // Read from stdin (pipe)
    while (fgets(buffer, BUFFER_SIZE, stdin) != NULL) {
      // Remove trailing newline if -n option is set
      if (!newline && buffer[strlen(buffer) - 1] == '\n') {
        buffer[strlen(buffer) - 1] = '\0';
      }
      printf("%s", buffer);
    }
  } else if (i < argc) {
    // Print command-line arguments
    for (; i < argc; i++) {
      printf("%s", argv[i]);
      if (i < argc - 1) {
        printf(" "); // Print a space between arguments
      }
    }
  } else {
    // No arguments and no piped input, print help
    print_help();
  }

  // Print a newline unless -n was specified
  if (newline) {
    printf("\n");
  }

  return 0;
}
