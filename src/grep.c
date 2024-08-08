// grep.c
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

void grep(FILE *file, char *pattern, int show_line_numbers) {
  char line[MAX_LINE_LENGTH];
  int line_number = 0;
  while (fgets(line, MAX_LINE_LENGTH, file) != NULL) {
    line_number++;
    if (strstr(line, pattern) != NULL) {
      if (show_line_numbers) {
        fprintf(stdout, "%d: %s", line_number, line);
      } else {
        fprintf(stdout, "%s", line);
      }
    }
  }

  if (file != stdin) {
    fclose(file);
  }
}

int main(int argc, char *argv[]) {
  int opt;
  int opt_idx = 0;
  int show_line_numbers = 0;
  char *pattern = NULL;
  FILE *file = stdin;

  static struct option long_opts[] = {{"enumerate", no_argument, 0, 'n'},
                                      {"help", no_argument, 0, 'h'},
                                      {0, 0, 0, 0}};

  while ((opt = getopt_long(argc, argv, "nh", long_opts, &opt_idx)) != -1) {
    switch (opt) {
    case 'n':
      show_line_numbers = 1;
      break;
    case 'a':
      fprintf(stderr, "Option not implemented\n");
      break;
    case 'h':
      fprintf(stderr,
              "Usage: %s <pattern> [<filename>] [--help] [--enumerate]\n",
              argv[0]);
      return 0;
    default:
      fprintf(stderr,
              "Usage: %s <pattern> [<filename>] [--help] [--enumerate]\n",
              argv[0]);
      return 1;
    }
  }

  if (optind < argc) {
    pattern = argv[optind++];
  } else {
    fprintf(stderr,
            "Pattern not provided.\nUsage: %s <pattern> [<filename>] [--help] "
            "[--enumerate]\n",
            argv[0]);
    return 1;
  }

  if (optind < argc) {
    file = fopen(argv[optind], "r");
    if (file == NULL) {
      perror("fopen");
      return 1;
    }
  }

  grep(file, pattern, show_line_numbers);
}
