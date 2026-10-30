#ifndef CLI_H
#define CLI_H

// Header-part mask for `elfmane header FILE [-e|-p|-s] [-f]`.
#define PART_EHDR (1u << 0)
#define PART_PHDR (1u << 1)
#define PART_SHDR (1u << 2)
#define PART_ALL (PART_EHDR | PART_PHDR | PART_SHDR)

typedef struct {
  const char *path; // positional argument: the ELF file
  unsigned parts;   // PART_* bitmask; 0 after parsing means PART_ALL
  int force;        // -f: continue after validation errors
} Cli_HeaderOpts;

// Parses argv for the "header" subcommand. Returns 0 and fills `out` on
// success; on usage error logs it via log.h and returns non-zero.
int cli_parse_header_opts(int argc, const char **argv, Cli_HeaderOpts *out);

void cli_print_header_usage(void);

#endif // !CLI_H
