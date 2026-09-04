#include "util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define ELFMANE_VERSION "v1.2.0 (enhanced cli)"

int handle_header(int argc, const char *argv[]);

static const struct {
  const char *cmd_name;
  int (*cmd_handler)(int, const char **);
} cmd_table[] = {{"header", handle_header}};

int main(int argc, char const *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Usage: elfmane --help\n");
    // TODO: explain usage by error from log.h
    return 1;
  }

  if (strcmp(argv[1], "--help") == 0) {
    printf("For now no implementation for help..\n");
    // TODO: help function
    return 0;
  }

  if (strcmp(argv[1], "--version") == 0) {
    printf("nscli Version: %s\n", ELFMANE_VERSION);
    return 0;
  }

  const char *cmd = argv[1];

  const size_t table_entries = sizeof(cmd_table) / sizeof(cmd_table[0]);
  for (size_t entry = 0; entry < table_entries; entry++) {
    if (strcmp(cmd, cmd_table[entry].cmd_name) == 0) {
      int result = cmd_table[entry].cmd_handler(argc - 2, argv + 2);
      // TODO: handle bad results
      return result;
    }
  }
  // TODO: error from log.h
  fprintf(stderr, "Error: No such command as \"%s\" (Use elfmane --help)\n",
          cmd);
  return 1;
}

int handle_header(int argc, const char *argv[]) { return 0; }
