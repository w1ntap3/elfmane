#include "cli.h"
#include "elf.h"
#include "log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// One-word codename describes the current work state.
#define ELFMANE_VERSION "2.1.0-headers"

#ifndef ELFMANE_GIT_HASH
#define ELFMANE_GIT_HASH "unknown"
#endif

int handle_header(int argc, const char *argv[]);

static const struct {
  const char *cmd_name;
  int (*cmd_handler)(int, const char **);
} cmd_table[] = {{"header", handle_header}};

static const struct {
  const char *key;
  const char *value;
} help_table[] = {
    {"--version", "the current version of the program on your system"},
    {"--help", "list of all subcommands and flags"},
    {"header", "info about ELF, program and section headers"},
};

static void print_help(void) {
  const size_t entries = sizeof(help_table) / sizeof(help_table[0]);
  printf("List of all help:\n");
  for (size_t entry = 0; entry < entries; entry++) {
    printf("\t%-16s %s\n", help_table[entry].key, help_table[entry].value);
  }
}

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    log_sys("Usage: elfmane --help");
    return 1;
  }

  if (strcmp(argv[1], "--help") == 0) {
    print_help();
    return 0;
  }

  if (strcmp(argv[1], "--version") == 0) {
    printf("elfmane version: %s (%s)\n", ELFMANE_VERSION, ELFMANE_GIT_HASH);
    return 0;
  }

  const char *cmd = argv[1];

  const size_t table_entries = sizeof(cmd_table) / sizeof(cmd_table[0]);
  for (size_t entry = 0; entry < table_entries; entry++) {
    if (strcmp(cmd, cmd_table[entry].cmd_name) == 0) {
      return cmd_table[entry].cmd_handler(argc - 2, argv + 2);
    }
  }
  log_err("No such command as \"%s\". (Refer to elfmane --help)", cmd);
  return 1;
}

int handle_header(int argc, const char *argv[]) {
  Cli_HeaderOpts opts;
  if (cli_parse_header_opts(argc, argv, &opts) != 0)
    return 1;

  FILE *f = fopen(opts.path, "rb");
  if (f == NULL) {
    log_err("Could not open \"%s\".", opts.path);
    return 1;
  }

  int rc = 0;
  Elf_Header header;
  if (elf_read_header(f, &header) != 0) {
    rc = 1;
  } else if (elf_validate_ident(&header) != 0 && !opts.force) {
    rc = 1;
  } else {
    if (opts.parts & PART_EHDR)
      elf_print_header(&header);
    if (opts.parts & PART_PHDR) {
      Elf_Phdr *phdrs = NULL;
      uint16_t phdr_count = 0;
      if (elf_read_phdrs(f, &header, &phdrs, &phdr_count) == 0) {
        elf_print_phdrs(phdrs, phdr_count);
        free(phdrs);
      } else {
        rc = 1;
      }
    }
    if (opts.parts & PART_SHDR)
      log_sys("Section header table is not implemented yet.");
  }

  fclose(f);
  return rc;
}
