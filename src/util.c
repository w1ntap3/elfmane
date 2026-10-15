#include "util.h"
#include <stdio.h>

// Column width for printing tables
#define COL_W 10

// Flags for program header table
#define PF_R 0x4
#define PF_W 0x2
#define PF_X 0x1

struct Help_entry {
  char *key;
  char *value;
};

// const struct Help_entry help_table[] = {{"salam", "aleykum"}};
const struct Help_entry help_table[] = {
    {"--version", "the current version of the program on your system"},
    {"--help", "list of all subcommands and flags"},
};

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

char *padding_bytes_check(uint8_t e_ident[]) {
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

char *s_name(uint32_t sh_name) { return "temp_name"; }

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

void util_help(void) {
  printf("List of all help:\n");
  const int entries = (int)(sizeof(help_table) / (int)sizeof(help_table[0]));
  for (int entry = 0; entry < entries; entry++) {
    printf("\t%-20s %s\n", help_table[entry].key, help_table[entry].value);
  }
}
