#define _POSIX_C_SOURCE 200809L // fileno(), isatty()

#include "elf.h"
#include "log.h"
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

// Flags for program header table entries
#define PF_R 0x4
#define PF_W 0x2
#define PF_X 0x1

// Byte-level readers: decode explicit widths honoring the file's byte order.
// Return 0 on success, 1 on short read.

static int read_bytes(FILE *f, uint8_t *buf, size_t n) {
  return fread(buf, 1, n, f) == n ? 0 : 1;
}

static int read_u16(FILE *f, int byte_order, uint16_t *out) {
  uint8_t b[2];
  if (read_bytes(f, b, sizeof(b)))
    return 1;
  *out = (uint16_t)(byte_order == ELFDATA2MSB ? (b[0] << 8) | b[1]
                                       : (b[1] << 8) | b[0]);
  return 0;
}

static int read_u32(FILE *f, int byte_order, uint32_t *out) {
  uint8_t b[4];
  if (read_bytes(f, b, sizeof(b)))
    return 1;
  if (byte_order == ELFDATA2MSB) {
    *out = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
           ((uint32_t)b[2] << 8) | b[3];
  } else {
    *out = ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) |
           ((uint32_t)b[1] << 8) | b[0];
  }
  return 0;
}

static int read_u64(FILE *f, int byte_order, uint64_t *out) {
  uint8_t b[8];
  if (read_bytes(f, b, sizeof(b)))
    return 1;
  uint64_t v = 0;
  if (byte_order == ELFDATA2MSB) {
    for (int i = 0; i < 8; i++)
      v = (v << 8) | b[i];
  } else {
    for (int i = 7; i >= 0; i--)
      v = (v << 8) | b[i];
  }
  *out = v;
  return 0;
}

int elf_read_header(FILE *f, Elf_Header *out) {
  memset(out, 0, sizeof(*out));

  if (fseek(f, 0, SEEK_SET) != 0) {
    log_err("Failed to seek to the start of the file.");
    return 1;
  }
  if (read_bytes(f, out->e_ident, EI_LEN)) {
    log_err("File is too small to contain an ELF header.");
    return 1;
  }

  if (memcmp(out->e_ident, "\x7F" "ELF", 4) != 0) {
    log_err("Not an ELF file (bad magic).");
    return 1;
  }

  out->elf_class = out->e_ident[EI_CLASS];
  out->encoding = out->e_ident[EI_DATA];

  if (out->elf_class != ELFCLASS32 && out->elf_class != ELFCLASS64) {
    log_err("ELF class is neither 32- nor 64-bit.");
    return 1;
  }
  if (out->encoding != ELFDATA2LSB && out->encoding != ELFDATA2MSB) {
    log_err("ELF byte order (endianness) is neither little nor big.");
    return 1;
  }

  const int byte_order = out->encoding;
  const int is_64bit = (out->elf_class == ELFCLASS64);

  if (read_u16(f, byte_order, &out->e_type) ||
      read_u16(f, byte_order, &out->e_machine) ||
      read_u32(f, byte_order, &out->e_version)) {
    log_err("Truncated ELF header.");
    return 1;
  }

  if (is_64bit) {
    if (read_u64(f, byte_order, &out->e_entry) ||
        read_u64(f, byte_order, &out->e_phoff) ||
        read_u64(f, byte_order, &out->e_shoff)) {
      log_err("Truncated ELF header.");
      return 1;
    }
  } else {
    uint32_t entry32, phoff32, shoff32;
    if (read_u32(f, byte_order, &entry32) ||
        read_u32(f, byte_order, &phoff32) ||
        read_u32(f, byte_order, &shoff32)) {
      log_err("Truncated ELF header.");
      return 1;
    }
    out->e_entry = entry32;
    out->e_phoff = phoff32;
    out->e_shoff = shoff32;
  }

  if (read_u32(f, byte_order, &out->e_flags) ||
      read_u16(f, byte_order, &out->e_ehsize) ||
      read_u16(f, byte_order, &out->e_phentsize) ||
      read_u16(f, byte_order, &out->e_phnum) ||
      read_u16(f, byte_order, &out->e_shentsize) ||
      read_u16(f, byte_order, &out->e_shnum) ||
      read_u16(f, byte_order, &out->e_shstrndx)) {
    log_err("Truncated ELF header.");
    return 1;
  }
  return 0;
}

int elf_validate_ident(const Elf_Header *h) {
  if (h->e_ident[EI_VERSION] != 1) {
    log_warn("ELF version is not set to 1.");
  }
  if (h->e_ident[EI_OSABI] > 0x12) {
    log_warn("Target OS ABI is undefined.");
  }
  // ABI version: nobody cares.
  for (int i = EI_PAD; i < EI_LEN; i++) {
    if (h->e_ident[i] != 0) {
      log_warn("File might be corrupted (padding bytes are not 0).");
      break;
    }
  }
  return 0;
}

static const char *highlight_code(void) {
  return isatty(fileno(stdout)) ? "\033[32m" : "";
}
static const char *reset_code(void) {
  return isatty(fileno(stdout)) ? "\033[0m" : "";
}

void elf_print_header(const Elf_Header *h) {
  const int is64 = (h->elf_class == ELFCLASS64);

  printf("───── ELF Header ─────\n");
  printf("Magic: %c %c %c (0x%02x %02x %02x %02x)\n", h->e_ident[1],
         h->e_ident[2], h->e_ident[3], h->e_ident[0], h->e_ident[1],
         h->e_ident[2], h->e_ident[3]);
  printf("%sClass: %s (%d-bit)%s\n", highlight_code(),
         h->elf_class == ELFCLASS64 ? "ELF64" : "ELF32", is64 ? 64 : 32,
         reset_code());
  printf("%sEndianness: %s endian%s\n", highlight_code(),
         h->encoding == ELFDATA2MSB ? "Big" : "Little", reset_code());
  printf("Version: %d\n", h->e_ident[EI_VERSION]);
  printf("Target OS ABI: %s (0x%x)\n", os_abi_lookup(h->e_ident[EI_OSABI]),
         h->e_ident[EI_OSABI]);
  printf("ABI Version: %d\n", h->e_ident[EI_ABIVERSION]);
  printf("Padding bytes status: %s\n", padding_bytes_check(h->e_ident));
  printf("%sFile type: %s%s\n", highlight_code(), file_type_lookup(h->e_type), reset_code());
  printf("Target ISA: %s\n", target_isa_lookup(h->e_machine));
  printf("Process entry: 0x%" PRIx64 "\n", h->e_entry);
  printf("Program table: 0x%" PRIx64 "\n", h->e_phoff);
  printf("Section table: 0x%" PRIx64 "\n", h->e_shoff);
  printf("Flags: 0x%x\n", h->e_flags);
  printf("ELF Header Size: %d bytes (%s)\n", h->e_ehsize,
         is64 ? "normally 64" : "normally 52");
  printf("Program table entry size: 0x%x (%s)\n", h->e_phentsize,
         is64 ? "typically 0x38" : "typically 0x20");
  printf("%sEntries in the program header: %d%s\n", highlight_code(), h->e_phnum, reset_code());
  printf("Section table entry size: 0x%x (%s)\n", h->e_shentsize,
         is64 ? "typically 0x40" : "typically 0x28");
  printf("%sEntries in the section header: %d%s\n", highlight_code(), h->e_shnum, reset_code());
  printf("Index of section names: %d\n", h->e_shstrndx);
}

char *os_abi_lookup(int osabi) {
  switch (osabi) {
  case 0x00:
    return "System V";
  case 0x01:
    return "HP-UX";
  case 0x02:
    return "NetBSD";
  case 0x03:
    return "Linux";
  case 0x04:
    return "GNU Hurd";
  case 0x06:
    return "Solaris";
  case 0x07:
    return "AIX (Monterey)";
  case 0x08:
    return "IRIX";
  case 0x09:
    return "FreeBSD";
  case 0x0A:
    return "Tru64";
  case 0x0B:
    return "Novell Modesto";
  case 0x0C:
    return "OpenBSD";
  case 0x0D:
    return "OpenVMS";
  case 0x0E:
    return "NonStop Kernel";
  case 0x0F:
    return "AROS";
  case 0x10:
    return "FenixOS";
  case 0x11:
    return "Nuxi CloudABI";
  case 0x12:
    return "Stratus Technologies OpenVOS";
  default:
    return "Undefined";
  }
}

char *padding_bytes_check(const uint8_t e_ident[]) {
  for (int i = EI_PAD; i < EI_LEN; i++) {
    if (e_ident[i] != 0) {
      return "Unexpected values (File might be corrupted)";
    }
  }
  return "As expected";
}

char *file_type_lookup(uint16_t e_type) {
  switch (e_type) {
  case 0x00:
    return "Unknown (NONE)";
  case 0x01:
    return "Relocatable file (REL)";
  case 0x02:
    return "Executable file (EXEC)";
  case 0x03:
    return "Shared object (DYN)";
  case 0x04:
    return "Core file (CORE)";
  case 0xFE00:
    return "Reserved inclusive range (LOOS)";
  case 0xFEFF:
    return "Reserved inclusive range (HIOS)";
  case 0xFF00:
    return "Reserved inclusive range (LOPROC)";
  case 0xFFFF:
    return "Reserved inclusive range (HIPROC)";
  default:
    return "Unexpected";
  }
}

char *target_isa_lookup(uint16_t e_machine) {
  switch (e_machine) {
  case 0x00:
    return "No specific instruction set";
  case 0x01:
    return "AT&T WE 32100";
  case 0x02:
    return "SPARC";
  case 0x03:
    return "x86";
  case 0x04:
    return "Motorola 68000 (M68k)";
  case 0x05:
    return "Motorola 88000 (M88k)";
  case 0x06:
    return "Intel MCU";
  case 0x07:
    return "Intel 80860";
  case 0x08:
    return "MIPS";
  case 0x09:
    return "IBM System/370";
  case 0x0A:
    return "MIPS RS3000 Little-endian";
  case 0x0F:
    return "Hewlett-Packard PA-RISC";
  case 0x13:
    return "Intel 80960";
  case 0x14:
    return "PowerPC";
  case 0x15:
    return "PowerPC (64-bit)";
  case 0x16:
    return "S390, including S390x";
  case 0x17:
    return "IBM SPU/SPC";
  case 0x24:
    return "NEC V800";
  case 0x25:
    return "Fujitsu FR20";
  case 0x26:
    return "TRW RH-32";
  case 0x27:
    return "Motorola RCE";
  case 0x28:
    return "Arm (up to Armv7/AArch32)";
  case 0x29:
    return "Digital Alpha";
  case 0x2A:
    return "SuperH";
  case 0x2B:
    return "SPARC Version 9";
  case 0x2C:
    return "Siemens TriCore embedded processor";
  case 0x2D:
    return "Argonaut RISC Core";
  case 0x2E:
    return "Hitachi H8/300";
  case 0x2F:
    return "Hitachi H8/300H";
  case 0x30:
    return "Hitachi H8S";
  case 0x31:
    return "Hitachi H8/500";
  case 0x32:
    return "IA-64";
  case 0x33:
    return "Stanford MIPS-X";
  case 0x34:
    return "Motorola ColdFire";
  case 0x35:
    return "Motorola M68HC12";
  case 0x36:
    return "Fujitsu MMA Multimedia Accelerator";
  case 0x37:
    return "Siemens PCP";
  case 0x38:
    return "Sony nCPU embedded RISC processor";
  case 0x39:
    return "Denso NDR1 microprocessor";
  case 0x3A:
    return "Motorola Star*Core processor";
  case 0x3B:
    return "Toyota ME16 processor";
  case 0x3C:
    return "STMicroelectronics ST100 processor";
  case 0x3D:
    return "Advanced Logic Corp. TinyJ embedded processor family";
  case 0x3E:
    return "AMD x86-64";
  case 0x3F:
    return "Sony DSP Processor";
  case 0x40:
    return "Digital Equipment Corp. PDP-10";
  case 0x41:
    return "Digital Equipment Corp. PDP-11";
  case 0x42:
    return "Siemens FX66 microcontroller";
  case 0x43:
    return "STMicroelectronics ST9+ 8/16-bit microcontroller";
  case 0x44:
    return "STMicroelectronics ST7 8-bit microcontroller";
  case 0x45:
    return "Motorola MC68HC16 Microcontroller";
  case 0x46:
    return "Motorola MC68HC11 Microcontroller";
  case 0x47:
    return "Motorola MC68HC08 Microcontroller";
  case 0x48:
    return "Motorola MC68HC05 Microcontroller";
  case 0x49:
    return "Silicon Graphics SVx";
  case 0x4A:
    return "STMicroelectronics ST19 8-bit microcontroller";
  case 0x4B:
    return "Digital VAX";
  case 0x4C:
    return "Axis Communications 32-bit embedded processor";
  case 0x4D:
    return "Infineon Technologies 32-bit embedded processor";
  case 0x4E:
    return "Element 14 64-bit DSP Processor";
  case 0x4F:
    return "LSI Logic 16-bit DSP Processor";
  case 0x8C:
    return "TMS320C6000 Family";
  case 0xAF:
    return "MCST Elbrus e2k";
  case 0xB7:
    return "Arm 64-bits (Armv8/AArch64)";
  case 0xDC:
    return "Zilog Z80";
  case 0xF3:
    return "RISC-V";
  case 0xF7:
    return "Berkeley Packet Filter";
  case 0x101:
    return "WDC 65C816";
  case 0x102:
    return "LoongArch";
  default:
    return "Undefined";
  }
}

char *p_type_lookup(uint32_t p_type) {
  switch (p_type) {
  case 0x00000000:
    return "NULL";
  case 0x00000001:
    return "LOAD";
  case 0x00000002:
    return "DYNAMIC";
  case 0x00000003:
    return "INTERP";
  case 0x00000004:
    return "NOTE";
  case 0x00000005:
    return "SHLIB";
  case 0x00000006:
    return "PHDR";
  case 0x00000007:
    return "TLS";
  case 0x60000000:
    return "LOOS";
  case 0x6FFFFFFF:
    return "HIOS";
  case 0x70000000:
    return "LOPROC";
  case 0x7FFFFFFF:
    return "HIPROC";
  default:
    return "[undef]";
  }
}

void p_flag_string(char *flag_str, uint32_t p_flags) {
  flag_str[0] = (p_flags & PF_R) ? 'R' : '-';
  flag_str[1] = (p_flags & PF_W) ? 'W' : '-';
  flag_str[2] = (p_flags & PF_X) ? 'X' : '-';
}

// Phase 1 stub: resolves against the shstrtab once elf_read_shdrs() exists.
char *s_name(uint32_t sh_name) {
  (void)sh_name;
  return "temp_name";
}

char *s_type_lookup(uint32_t sh_type) {
  switch (sh_type) {
  case 0x0:
    return "NULL";
  case 0x1:
    return "PROGBITS";
  case 0x2:
    return "SYMTAB";
  case 0x3:
    return "STRTAB";
  case 0x4:
    return "RELA";
  case 0x5:
    return "HASH";
  case 0x6:
    return "DYNAMIC";
  case 0x7:
    return "NOTE";
  case 0x8:
    return "NOBITS";
  case 0x9:
    return "REL";
  case 0x0A:
    return "SHLIB";
  case 0x0B:
    return "DYNSYM";
  case 0x0E:
    return "INIT_ARRAY";
  case 0x0F:
    return "FINI_ARRAY";
  case 0x10:
    return "PREINIT_ARRAY";
  case 0x11:
    return "GROUP";
  case 0x12:
    return "SYMTAB_SHNDX";
  case 0x13:
    return "NUM";
  case 0x60000000:
    return "LOOS";
  default:
    return "Undefined";
  }
}

