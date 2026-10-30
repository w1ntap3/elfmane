#include "cli.h"
#include "log.h"
#include <stdio.h>
#include <string.h>

void cli_print_header_usage(void) {
  printf("Usage: elfmane header FILE [OPTIONS]\n"
         "OPTIONS:\n"
         "\t%-12s %s\n"
         "\t%-12s %s\n"
         "\t%-12s %s\n"
         "\t%-12s %s\n"
         "\t%-12s %s\n",
         "-h", "show this help",
         "-e", "ELF header",
         "-p", "program header table",
         "-s", "section header table",
         "-f", "force read (ignore validation errors)\n"
         "(no -e/-p/-s) prints everything");
}

static void usage_error(const char *msg, const char *arg) {
  log_err(msg, arg);
  cli_print_header_usage();
}

int cli_parse_header_opts(int argc, const char **argv, Cli_HeaderOpts *out) {
  if (argc < 1) {
    log_err("No file provided.");
    cli_print_header_usage();
    return 1;
  }

  out->path = NULL;
  out->parts = 0;
  out->force = 0;

  for (int i = 0; i < argc; i++) {
    const char *arg = argv[i];

    if (arg[0] == '-' && arg[1] != '\0') {
      // Bundled short options allowed: "-ef" == "-e -f".
      for (const char *opt = arg + 1; *opt != '\0'; opt++) {
        switch (*opt) {
        case 'e':
          out->parts |= PART_EHDR;
          break;
        case 'p':
          out->parts |= PART_PHDR;
          break;
        case 's':
          out->parts |= PART_SHDR;
          break;
        case 'f':
          out->force = 1;
          break;
        case 'h':
          cli_print_header_usage();
          return 1;
        default: {
          char token[2] = {'-', *opt};
          usage_error("Unknown option \"%s\"", token);
          return 1;
        }
        }
      }
    } else if (out->path == NULL) {
      out->path = arg;
    } else {
      usage_error("Unexpected extra argument \"%s\"", arg);
      return 1;
    }
  }

  if (out->path == NULL) {
    log_err("No file provided.");
    cli_print_header_usage();
    return 1;
  }
  if (out->parts == 0) {
    out->parts = PART_ALL;
  }
  return 0;
}
