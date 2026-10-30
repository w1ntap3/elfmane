#ifndef ELF_H
#define ELF_H

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

// e_ident values
#define ELFCLASS32 1
#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define ELFDATA2MSB 2

// Offsets for program/section header tables (used by the phdr/shdr readers)
#define E_PHOFF_OFF32 0x1C
#define E_PHENTSIZE_OFF32 0x2A
#define E_PHOFF_OFF64 0x20
#define E_PHENTSIZE_OFF64 0x36

#define E_SHOFF_OFF32 0x20
#define E_SHENTSIZE_OFF32 0x2E
#define E_SHOFF_OFF64 0x28
#define E_SHENTSIZE_OFF64 0x3A

// Canonical, class-agnostic ELF header.
// 32-bit fields are widened during parsing; both classes parse into this.
typedef struct {
  uint8_t e_ident[EI_LEN];
  int elf_class;  // ELFCLASS32 / ELFCLASS64 (EI_CLASS raw value)
  int encoding;   // ELFDATA2LSB / ELFDATA2MSB (EI_DATA raw value)
  uint16_t e_type;
  uint16_t e_machine;
  uint32_t e_version;
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
} Elf_Header;

// Reads and parses the ELF header. Returns 0 on success, 1 on failure
// (diagnostics via log.h). The file is left positioned at the end of
// the header on success.
int elf_read_header(FILE *f, Elf_Header *out);

// Sanity-checks e_ident: ELF version, OS ABI range, zero padding.
// Returns non-zero on hard error, logs oddities as warnings and continues.
int elf_validate_ident(const Elf_Header *h);

// Prints the ELF header; key fields highlighted when stdout is a terminal.
void elf_print_header(const Elf_Header *h);

// Canonical, class-agnostic program header.
// Field order follows the 32-bit layout; the reader maps 64-bit
// entries (which move p_flags earlier) into this.
typedef struct {
  uint32_t p_type;
  uint64_t p_offset;
  uint64_t p_vaddr;
  uint64_t p_paddr;
  uint64_t p_filesz;
  uint64_t p_memsz;
  uint32_t p_flags;
  uint64_t p_align;
} Elf_Phdr;

// Reads the program header table. Allocates *entries_out (caller frees)
// and stores the entry count in *count_out. If e_phnum claims more
// entries than the file contains, the count is clamped to what fits and
// a warning is logged. Returns 0 on success, 1 on failure.
int elf_read_phdrs(FILE *f, const Elf_Header *header, Elf_Phdr **entries_out,
                   uint16_t *count_out);

void elf_print_phdrs(const Elf_Phdr *entries, uint16_t count);

// Lookup helpers: map raw ELF field values to human-readable strings.
char *os_abi_lookup(int osabi);
char *padding_bytes_check(const uint8_t e_ident[]);
char *file_type_lookup(uint16_t e_type);
char *target_isa_lookup(uint16_t e_machine);
char *p_type_lookup(uint32_t p_type);
void p_flag_string(char flag_str[static 4], uint32_t p_flags);
char *s_name(uint32_t sh_name);
char *s_type_lookup(uint32_t sh_type);

#endif // !ELF_H
