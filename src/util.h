#ifndef LOOKUP_H
#define LOOKUP_H
#include <stdint.h>

#define EI_PAD 9
#define EI_LEN 16
char *os_abi_lookup(int osabi);
char *padding_bytes_check(uint8_t e_ident[]);
char *file_type_lookup(uint16_t e_type);
char *target_isa_lookup(uint16_t e_machine);
char *p_type_lookup(uint32_t p_type);
void p_flag_string(char *flag_str, uint32_t p_flags);
char *s_name(uint32_t sh_name);
char *s_type_lookup(uint32_t sh_type);
int check_help(const char *arg);

#endif // !LOOKUP_H
#define LOOKUP_H
