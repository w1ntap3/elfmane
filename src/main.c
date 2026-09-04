#include "elf.h"
#include "log.h"
#include "util.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char const *argv[]) {
  int elf_class = 0;

  if (argc < 2) {
    ERROR("Use \"elfmane [FILE_PATH] [OPTIONS]\" or \"elfmane --help\"");
    return 1;
  }

  if (check_help(argv[1]))
    return 0;

  FILE *target_file = fopen(argv[1], "rb");

  if (target_file == NULL) {
    ERROR("Could not open the file.");
    return 1;
  }
  fread(e_head32.e_ident, sizeof(uint8_t), EI_LEN, target_file);
  // Check validity of e_ident to identify the class

  if (e_ident_valid(e_head32, &elf_class))
    return 1;

  for (int arg_n = 2; arg_n < argc; arg_n++) {
    if (argv[arg_n][0] == '-') {
      switch (argv[arg_n][1]) {
      case 'a':
        e_header_info(target_file, elf_class);
        p_header_info(target_file, elf_class);
        s_header_info(target_file, elf_class);
        break;
      case 'e':
        e_header_info(target_file, elf_class);
        break;
      case 'p':
        p_header_info(target_file, elf_class);
        break;
      case 's':
        s_header_info(target_file, elf_class);
        break;
      case 'f':
        break;
      default:
        ERROR("Use \"elfmane [FILE_PATH] [OPTIONS]\" or \"elfmane --help\"");
        break;
      }
    }
  }

  fclose(target_file);
  return 0;
}
