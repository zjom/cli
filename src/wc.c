// The wc utility displays the number of lines, words, and bytes contained
// in each input file, or standard input (if no file is specified) to the
// standard output.  A line is defined as a string of characters delimited
// by a ⟨newline⟩ character.  Characters beyond the final ⟨newline⟩
// character will not be included in the line count.
//
// A word is defined as a string of characters delimited by white space
// characters.  White space characters are the set of characters for which
// the iswspace(3) function returns true.  If more than one input file is
// specified, a line of cumulative counts for all the files is displayed on
// a separate line after the output for the last file.
//
//
// -L      Write the length of the line containing the most bytes (default)
// or characters (when -m is provided) to standard output.  When more than
// one
//        file argument is specified, the longest input line of all files is
//        reported as the value of the final “total”.
//
// -c      The number of bytes in each input file is written to the standard
// output.  This will cancel out any prior usage of the -m option.
//
// -l      The number of lines in each input file is written to the standard
// output.
//
// -m      The number of characters in each input file is written to the
// standard output.  If the current locale does not support multibyte
// characters,
//        this is equivalent to the -c option.  This will cancel out any
//        prior usage of the -c option.
//
// -w      The number of words in each input file is written to the standard
// output.
//
// When an option is specified, wc only reports the information requested by
// that option.  The order of output always takes the form of line, word,
// byte, and file name.  The default action is equivalent to specifying the
// -c, -l and -w options.

// If no files are specified, the standard input is used and no file name is
// displayed.  The prompt will accept input until receiving EOF, or [^D] in
// most environments.
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  int lines;
  int words;
  int bytes;
  int max_line_length;
} wc_result;

typedef struct {
  int show_lines;
  int show_words;
  int show_bytes;
  int show_max_line_length;
} wc_show_options;

void print_options(wc_result result, wc_show_options options, char path[]) {
  if (options.show_lines)
    printf("%7d", result.lines);
  if (options.show_words)
    printf("%7d", result.words);
  if (options.show_bytes)
    printf("%7d", result.bytes);
  if (options.show_max_line_length)
    printf("%7d", result.max_line_length);
  printf(" %s\n", path);
}

void parse_options(int argc, char *argv[], wc_show_options *options) {
  options->show_lines = 1;
  options->show_words = 1;
  options->show_bytes = 1;
  options->show_max_line_length = 1;

  int opt;
  int opt_idx = 0;

  static struct option long_opts[] = {
      {"bytes", no_argument, 0, 'c'},
      {"lines", no_argument, 0, 'l'},
      {"words", no_argument, 0, 'w'},
      {"max-line-length", no_argument, 0, 'L'},
      {0, 0, 0, 0},
  };

  while ((opt = getopt_long(argc, argv, "clwL", long_opts, &opt_idx)) != -1) {
    switch (opt) {
    case 'c':
      options->show_bytes = 0;
      break;
    case 'l':
      options->show_lines = 0;
      break;
    case 'w':
      options->show_words = 0;
      break;
    case 'L':
      options->show_max_line_length = 0;
      break;
    default:
      fprintf(stderr, "Usage: %s [-c | -l | -w | -L] path...\n", argv[0]);
      exit(1);
    }
  }
}

wc_result wc(FILE *f) {
  long byte_count = 0;
  long line_count = 0;
  long word_count = 0;
  long max_line_length = 0;

  int in_word = 0;
  int line_length = 0;
  int c;

  while ((c = fgetc(f)) != EOF) {
    byte_count++;

    if (c == '\n') {
      line_count++;
      if (line_length > max_line_length)
        max_line_length = line_length;
      line_length = 0;
    } else {
      line_length++;
    }

    if (c == ' ' || c == '\n' || c == '\t') {
      in_word = 0;
    } else if (!in_word) {
      in_word = 1;
      word_count++;
    }
  }

  wc_result result = {.bytes = byte_count,
                      .lines = line_count,
                      .words = word_count,
                      .max_line_length = max_line_length};
  return result;
}

int main(int argc, char *argv[]) {
  wc_show_options options;
  parse_options(argc, argv, &options);

  wc_result total = {0, 0, 0, 0};
  int file_count = 0;

  for (int i = optind; i < argc; i++) {
    FILE *file = fopen(argv[i], "r");
    if (file == NULL) {
      fprintf(stderr, "Error: Cannot open file '%s'\n", argv[i]);
      continue;
    }

    wc_result result = wc(file);
    fclose(file);

    print_options(result, options, argv[i]);

    // Add to total
    total.lines += result.lines;
    total.words += result.words;
    total.bytes += result.bytes;
    if (result.max_line_length > total.max_line_length)
      total.max_line_length = result.max_line_length;

    file_count++;
  }

  // Print total if more than one file was processed
  if (file_count > 1) {
    if (options.show_lines)
      printf("%7d", total.lines);
    if (options.show_words)
      printf("%7d", total.words);
    if (options.show_bytes)
      printf("%7d", total.bytes);
    if (options.show_max_line_length)
      printf("%7d", total.max_line_length);
    printf(" total\n");
  }
}
