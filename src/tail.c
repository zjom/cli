#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

void print_usage(const char *program_name, FILE *stream) {
  fprintf(stream,
          "Usage: %s [path | -p path string] [-n lines number] "
          "[-H headers] [-f follow] [--help]\n",
          program_name);
}
typedef struct {
  int n;
  int show_headers;
  int follow;
} TailOptions;

int parse_options(int argc, char *argv[], TailOptions *options, char **path) {
  int opt;
  int option_index = 0;
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},        {"path", required_argument, 0, 'p'},
      {"lines", required_argument, 0, 'n'}, {"headers", no_argument, 0, 'H'},
      {"follow", no_argument, 0, 'f'},      {0, 0, 0, 0}};

  *path = NULL;
  options->n = 10;
  options->show_headers = 0;
  options->follow = 0;

  if (argc > 1 && argv[1][0] != '-') {
    *path = argv[1];
    argc--;
    argv++;
  }

  while ((opt = getopt_long(argc, argv, "p:n:hfH", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0], stdout);
      exit(EXIT_SUCCESS);
    case 'p':
      *path = optarg;
      break;
    case 'n':
      options->n = atoi(optarg);
    case 'H':
      options->show_headers = 1;
      break;
    case 'f':
      options->follow = 1;
      break;
    default:
      print_usage(argv[0], stderr);
      return -1;
    }
  }

  if (*path == NULL) {
    print_usage(argv[0], stderr);
    return -1;
  }

  if (strlen(*path) > PATH_MAX) {
    fprintf(stderr, "Provided path is too long\n");
    return -1;
  }

  return 0;
}

void print_last_n_lines(FILE *file, int n) {
  long pos;
  int line_count = 0;
  char buffer[BUFSIZ];

  fseek(file, 0, SEEK_END);
  pos = ftell(file);

  while (pos && line_count < n) {
    fseek(file, --pos, SEEK_SET);
    if (fgetc(file) == '\n') {
      line_count++;
    }
  }

  if (pos != 0) {
    fseek(file, pos + 1, SEEK_SET);
  }

  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    fputs(buffer, stdout);
  }
}

int tail(const char *path, const TailOptions *options) {
  FILE *file = fopen(path, "r");
  if (file == NULL) {
    perror("fopen");
    return -1;
  }

  if (options->show_headers) {
    printf("==> %s <==\n", path);
  }

  print_last_n_lines(file, options->n);
  fflush(stdout);

  if (options->follow) {
    struct stat stat_buf;
    time_t last_modified;

    if (stat(path, &stat_buf) == -1) {
      perror("stat");
      fclose(file);
      return -1;
    }
    last_modified = stat_buf.st_mtime;

    while (1) {
      clearerr(file);
      char buffer[BUFSIZ];
      while (fgets(buffer, sizeof(buffer), file) != NULL) {
        fputs(buffer, stdout);
        fflush(stdout);
      }

      // Check if file has been modified
      if (stat(path, &stat_buf) == -1) {
        perror("stat");
        break;
      }

      if (stat_buf.st_mtime > last_modified) {
        // File has been modified, seek to the end
        fseek(file, 0, SEEK_END);
        last_modified = stat_buf.st_mtime;
      } else {
        // File hasn't been modified, wait before checking again
        struct timespec ts = {0, 250000000}; // 250ms
        nanosleep(&ts, NULL);
      }
    }
  }

  fclose(file);
  return 0;
}

int main(int argc, char *argv[]) {
  char *path;
  TailOptions options;

  if (parse_options(argc, argv, &options, &path) != 0) {
    return EXIT_FAILURE;
  }

  if (tail(path, &options) != 0) {
    return EXIT_FAILURE;
  }

  return 0;
}
