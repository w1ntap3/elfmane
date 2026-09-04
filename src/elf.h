#ifndef ELF_H
#define ELF_H

#include "util.h"
#include <stdint.h>
#include <stdio.h>
// e_ident indexing
#define EI_LEN 16
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9

// Offsets for header tables
#define E_PHOFF_OFF32 0x1C
#define E_PHENTSIZE_OFF32 0x2A
#define E_PHOFF_OFF64 0x20
#define E_PHENTSIZE_OFF64 0x36

#define E_SHOFF_OFF32 0x20
#define E_SHENTSIZE_OFF32 0x2E
#define E_SHOFF_OFF64 0x28
#define E_SHENTSIZE_OFF64 0x3A

struct e_header32 {
  uint8_t e_ident[EI_LEN];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version; // always 1
  uint32_t e_entry;
  uint32_t e_phoff;
  uint32_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct e_header64 {
  uint8_t e_ident[EI_LEN];
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version; // always 1
  uint64_t e_entry;
  uint64_t e_phoff;
  uint64_t e_shoff;
  uint32_t e_flags;
  uint16_t e_ehsize;
  uint16_t e_phentsize;
  uint16_t e_phnum;
  uint16_t e_shentsize;
  uint16_t e_shnum;
  uint16_t e_shstrndx;
};

struct p_header32 {
  uint32_t p_type;
  uint32_t p_offset;
  uint32_t p_vaddr;
  uint32_t p_paddr;
  uint32_t p_filesz;
  uint32_t p_memsz;
  uint32_t p_flags;
  uint32_t p_align;
};

struct p_header64 {
  uint32_t p_type;
  uint32_t p_flags;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint64_t p_align;
};

struct s_header32 {
  uint32_t sh_name;
  uint32_t sh_type;
  uint32_t sh_flags;
  uint32_t sh_addr;
  uint32_t sh_offset;
  uint32_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint32_t sh_addralign;
  uint32_t sh_entsize;
};

struct s_header64 {
  uint32_t sh_name;
  uint32_t sh_type;
  uint64_t sh_flags;
  uint64_t sh_addr;
  uint64_t sh_offset;
  uint64_t sh_size;
  uint32_t sh_link;
  uint32_t sh_info;
  uint64_t sh_addralign;
  uint64_t sh_entsize;
};

int e_ident_valid(const struct e_header32 e_head, int *elf_class);
int e_header_info(FILE *target_file, const int elf_class);
int p_header_info(FILE *target_file, const int elf_class);
int s_header_info(FILE *target_file, const int elf_class);

#endif // !ELF_H
