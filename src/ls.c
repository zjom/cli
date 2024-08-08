#include <dirent.h>
#include <getopt.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_PATH_LENGTH PATH_MAX

typedef struct {
  int show_all;
  int recursive;
  int absolute_path;
} ListOptions;

void print_usage(const char *program_name) {
  printf("Usage: %s [path] [--path <path>] [--help] [--all] [--recursive] "
         "[--Absolute]\n",
         program_name);
}

void print_indentation(int level) {
  for (int i = 0; i < level; i++) {
    printf("  ");
  }
}

int is_special_directory(const char *name) {
  return strcmp(name, ".") == 0 || strcmp(name, "..") == 0;
}

int should_skip_file(const ListOptions *options, const char *name) {
  return !options->show_all && name[0] == '.';
}

char *get_absolute_path(const char *path) {
  char *abs_path = realpath(path, NULL);
  if (abs_path == NULL) {
    perror("Failed to get absolute path");
    return NULL;
  }
  return abs_path;
}

char *build_next_path(const char *current_path, const char *item_name) {
  size_t current_len = strlen(current_path);
  size_t item_len = strlen(item_name);
  char *next_path = malloc(current_len + item_len + 2);

  if (!next_path) {
    perror("Memory allocation failed");
    exit(EXIT_FAILURE);
  }

  strcpy(next_path, current_path);
  if (next_path[current_len - 1] != '/') {
    strcat(next_path, "/");
  }
  strcat(next_path, item_name);

  return next_path;
}

void print_item(const char *base_path, const char *name, unsigned char type,
                const ListOptions *options) {
  if (options->absolute_path) {
    char *full_path = build_next_path(base_path, name);
    char *abs_path = get_absolute_path(full_path);
    if (abs_path) {
      if (type == DT_DIR) {
        printf("%s/\n", abs_path);
      } else {
        printf("%s\n", abs_path);
      }
      free(abs_path);
    }
    free(full_path);
  } else {
    if (type == DT_DIR) {
      printf("%s/\n", name);
    } else {
      printf("%s\n", name);
    }
  }
}

int list_directory(const char *path, const ListOptions *options,
                   int indentation) {
  DIR *dir = opendir(path);
  if (dir == NULL) {
    perror("Failed to open directory");
    return -1;
  }

  struct dirent *entry;
  while ((entry = readdir(dir)) != NULL) {
    if (is_special_directory(entry->d_name) ||
        should_skip_file(options, entry->d_name)) {
      continue;
    }

    print_indentation(indentation);
    print_item(path, entry->d_name, entry->d_type, options);

    if (options->recursive && entry->d_type == DT_DIR) {
      char *next_path = build_next_path(path, entry->d_name);
      list_directory(next_path, options, indentation + 1);
      free(next_path);
    }
  }

  closedir(dir);
  return 0;
}

int parse_options(int argc, char *argv[], ListOptions *options, char **path) {
  int opt;
  int option_index = 0;
  static struct option long_options[] = {
      {"help", no_argument, 0, 'h'},     {"path", required_argument, 0, 'p'},
      {"all", no_argument, 0, 'a'},      {"recursive", no_argument, 0, 'r'},
      {"absolute", no_argument, 0, 'A'}, {0, 0, 0, 0}};

  *path = NULL;
  options->show_all = 0;
  options->recursive = 0;
  options->absolute_path = 0;

  if (argc > 1 && argv[1][0] != '-') {
    *path = argv[1];
    argc--;
    argv++;
  }

  while ((opt = getopt_long(argc, argv, "p:harA", long_options,
                            &option_index)) != -1) {
    switch (opt) {
    case 'h':
      print_usage(argv[0]);
      exit(EXIT_SUCCESS);
    case 'a':
      options->show_all = 1;
      break;
    case 'p':
      *path = optarg;
      break;
    case 'r':
      options->recursive = 1;
      break;
    case 'A':
      options->absolute_path = 1;
      break;
    default:
      print_usage(argv[0]);
      return -1;
    }
  }

  if (*path == NULL) {
    *path = "./";
  }

  if (strlen(*path) > MAX_PATH_LENGTH) {
    fprintf(stderr, "Provided path is too long\n");
    return -1;
  }

  return 0;
}

int main(int argc, char *argv[]) {
  ListOptions options;
  char *path;

  if (parse_options(argc, argv, &options, &path) != 0) {
    return EXIT_FAILURE;
  }

  return list_directory(path, &options, 0) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
