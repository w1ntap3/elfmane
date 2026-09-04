#include "elf.h"
#include "log.h"
#include <stdint.h>

// int e_ident_valid(const struct e_header32 e_head, int *elf_class) {
//   uint8_t return_val = 0; // Return after checking each warning/error case
//
//   if (((*elf_class = e_head.e_ident[EI_CLASS]) != 1) && (*elf_class != 2)) {
//     ERROR("ELF class is neither 32- or 64-bit.");
//     return_val = 1;
//   }
//
//   if (e_head.e_ident[EI_DATA] != 1 && e_head.e_ident[EI_DATA] != 2) {
//     ERROR("Endianness is neither little or big.");
//     return_val = 1;
//   }
//
//   if (e_head.e_ident[EI_VERSION] != 1) {
//     WARNING("ELF Version is not set to 1.");
//   }
//
//   if (e_head.e_ident[EI_OSABI] > 0x12) {
//     WARNING("OS's ABI undefined");
//   }
//
//   // no one cares about ABIVERSION
//
//   uint8_t corrupted = 0; // Padding bytes corruption check
//   for (int i = 0x09; i <= EI_LEN; i++) {
//     if (e_head.e_ident[i] != 0x00)
//       corrupted = 1;
//   }
//
//   if (corrupted)
//     WARNING("File might be corrupted (Padding bytes are not 0)");
//   return return_val;
// }

// int e_header_info(FILE *target_file, const int elf_class) {
//   rewind(target_file);
//   if (elf_class == 1) {
//     fread(&e_head32, sizeof(e_head32), 1, target_file);
//     uint8_t elf_class_bits = 32;
//     printf("\n───── ELF Header ─────\n");
//
//     printf("%s(Important fields are highlighted like this)%s\n",
//            HIGHLIGHT_PRINT, RESET_PRINT);
//     printf("Magic: %c %c %c (0x%x %x %x %x)\n"
//            "%sClass: %d (%d-bit)%s\n"
//            "%sEndianness: %d (%s endian)%s\n"
//            "Version: %d\n"
//            "Target OS ABI: %s (0x%x)\n"
//            "ABI Version: %d\n"
//            "Padding bytes status: %s\n"
//            "%sFile types: %s%s\n"
//            "Target ISA: %s\n"
//            "Process entry: 0x%x\n"
//            "Program table: 0x%x\n"
//            "Section table: 0x%x\n"
//            "Flags: 0x%x\n"
//            "ELF Header Size: %d bytes (%s)\n"
//            "Program table entry size: 0x%x (%s)\n"
//            "%sEntries in the program header: %d%s\n"
//            "Section table entry size: 0x%x (%s)\n"
//            "%sEntries in the section header: %d%s\n"
//            "Index of section names: %d\n",
//            e_head32.e_ident[1], e_head32.e_ident[2], e_head32.e_ident[3],
//            e_head32.e_ident[0], e_head32.e_ident[1], e_head32.e_ident[2],
//            e_head32.e_ident[3], HIGHLIGHT_PRINT, elf_class, elf_class_bits,
//            RESET_PRINT, HIGHLIGHT_PRINT, e_head32.e_ident[EI_DATA],
//            ENDIANNESS_STRING(e_head32.e_ident[EI_DATA] - 1), RESET_PRINT,
//            e_head32.e_ident[EI_VERSION],
//            os_abi_lookup(e_head32.e_ident[EI_OSABI]),
//            e_head32.e_ident[EI_OSABI], e_head32.e_ident[EI_ABIVERSION],
//            padding_bytes_check(e_head32.e_ident), HIGHLIGHT_PRINT,
//            file_type_lookup(e_head32.e_type), RESET_PRINT,
//            target_isa_lookup(e_head32.e_machine), e_head32.e_entry,
//            e_head32.e_phoff, e_head32.e_shoff, e_head32.e_flags,
//            e_head32.e_ehsize, "Normally 52 bytes", e_head32.e_phentsize,
//            "Typically 0x20", HIGHLIGHT_PRINT, e_head32.e_phnum, RESET_PRINT,
//            e_head32.e_shentsize, "Typically 0x28", HIGHLIGHT_PRINT,
//            e_head32.e_shnum, RESET_PRINT, e_head32.e_shstrndx);
//   } else if (elf_class == 2) {
//     fread(&e_head64, sizeof(e_head64), 1, target_file);
//     uint8_t elf_class_bits = 64;
//     printf("\n───── ELF Header ─────\n");
//
//     printf("\tMagic: %c %c %c (0x%x %x %x %x)\n"
//            "%s\tClass: %d (%d-bit)%s\n"
//            "%s\tEndianness: %d (%s endian)%s\n"
//            "\tVersion: %d\n"
//            "\tTarget OS ABI: %s (0x%x)\n"
//            "\tABI Version: %d\n"
//            "\tPadding bytes status: %s\n"
//            "%s\tFile type: %s%s\n"
//            "\tTarget ISA: %s\n"
//            "\tProcess entry: 0x%lx\n"
//            "\tProgram table: 0x%lx\n"
//            "\tSection table: 0x%lx\n"
//            "\tFlags: 0x%x\n"
//            "\tELF Header Size: %d bytes (%s)\n"
//            "\tProgram table entry size: 0x%x (%s)\n"
//            "%s\tEntries in the program header: %d%s\n"
//            "\tSection table entry size: 0x%x (%s)\n"
//            "%s\tEntries in the section header: %d%s\n"
//            "\tIndex of section names: %d\n",
//            e_head64.e_ident[1], e_head64.e_ident[2], e_head64.e_ident[3],
//            e_head64.e_ident[0], e_head64.e_ident[1], e_head64.e_ident[2],
//            e_head64.e_ident[3], HIGHLIGHT_PRINT, elf_class, elf_class_bits,
//            RESET_PRINT, HIGHLIGHT_PRINT, e_head64.e_ident[EI_DATA],
//            ENDIANNESS_STRING(e_head64.e_ident[EI_DATA] - 1), RESET_PRINT,
//            e_head64.e_ident[EI_VERSION],
//            os_abi_lookup(e_head64.e_ident[EI_OSABI]),
//            e_head64.e_ident[EI_OSABI], e_head64.e_ident[EI_ABIVERSION],
//            padding_bytes_check(e_head64.e_ident), HIGHLIGHT_PRINT,
//            file_type_lookup(e_head64.e_type), RESET_PRINT,
//            target_isa_lookup(e_head64.e_machine), e_head64.e_entry,
//            e_head64.e_phoff, e_head64.e_shoff, e_head64.e_flags,
//            e_head64.e_ehsize, "Normally 64 bytes", e_head64.e_phentsize,
//            "Typically 0x38", HIGHLIGHT_PRINT, e_head64.e_phnum, RESET_PRINT,
//            e_head64.e_shentsize, "Typically 0x40", HIGHLIGHT_PRINT,
//            e_head64.e_shnum, RESET_PRINT, e_head64.e_shstrndx);
//   }
//   return 0;
// }
//
// int p_header_info(FILE *target_file, const int elf_class) {
//   if (elf_class == 1) {
//     uint32_t e_phoff;
//     uint16_t e_phentsize;
//     uint16_t e_phnum;
//
//     fseek(target_file, E_PHOFF_OFF32, SEEK_SET);
//     fread(&e_phoff, sizeof(uint32_t), 1, target_file);
//     fseek(target_file, E_PHENTSIZE_OFF32, SEEK_SET);
//     fread(&e_phentsize, sizeof(uint16_t), 1, target_file);
//     fread(&e_phnum, sizeof(uint16_t), 1, target_file);
//
//     printf("\n───── Program Header Table ─────\n");
//
//     printf("%-4s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s\n", "N", COL_W,
//     "Type",
//            COL_W, "Off", COL_W, "VAddress", COL_W, "PAddress", COL_W,
//            "SizeFile", COL_W, "SizeMem", COL_W, "Flags", COL_W, "Align");
//     fseek(target_file, e_phoff, SEEK_SET);
//
//     for (int i = 0; i < e_phnum; i++) {
//       struct p_header32 p_head;
//       char flag_str[4]; // {'R', 'W', 'X', '\0'} (len = 4)
//       fread(&p_head, sizeof(p_head), 1, target_file);
//       p_flag_string(flag_str, p_head.p_flags);
//
//       printf("[%02d] %-*s 0x%-*x 0x%-*x 0x%-*x 0x%-*x 0x%-*x %-*s %-*d\n", i,
//              COL_W, p_type_lookup(p_head.p_type), COL_W - 2, p_head.p_offset,
//              COL_W - 2, p_head.p_vaddr, COL_W - 2, p_head.p_paddr, COL_W - 2,
//              p_head.p_filesz, COL_W - 2, p_head.p_memsz, COL_W, flag_str,
//              COL_W, p_head.p_align);
//     }
//   } else if (elf_class == 2) {
//     uint64_t e_phoff;
//     uint16_t e_phentsize;
//     uint16_t e_phnum;
//
//     fseek(target_file, E_PHOFF_OFF64, SEEK_SET);
//     fread(&e_phoff, sizeof(uint64_t), 1, target_file);
//     fseek(target_file, E_PHENTSIZE_OFF64, SEEK_SET);
//     fread(&e_phentsize, sizeof(uint16_t), 1, target_file);
//     fread(&e_phnum, sizeof(uint16_t), 1, target_file);
//
//     printf("\n───── Program Header Table ─────\n");
//
//     printf("%-4s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s\n", "N", COL_W,
//     "Type",
//            COL_W, "Off", COL_W, "VAddress", COL_W, "PAddress", COL_W,
//            "SizeFile", COL_W, "SizeMem", COL_W, "Flags", COL_W, "Align");
//     fseek(target_file, e_phoff, SEEK_SET);
//     for (int i = 0; i < e_phnum; i++) {
//       struct p_header64 p_head;
//       char flag_str[4];
//       fread(&p_head, sizeof(p_head), 1, target_file);
//       p_flag_string(flag_str, p_head.p_flags);
//
//       printf("[%02d] %-*s 0x%-*lx 0x%-*lx 0x%-*lx 0x%-*lx 0x%-*lx %-*s
//       %-*d\n",
//              i, COL_W, p_type_lookup(p_head.p_type), COL_W - 2,
//              p_head.p_offset, COL_W - 2, p_head.p_vaddr, COL_W - 2,
//              p_head.p_paddr, COL_W - 2, p_head.p_filesz, COL_W - 2,
//              p_head.p_memsz, COL_W, flag_str, COL_W, p_head.p_align);
//     }
//   }
//   return 0;
// }
//
// int s_header_info(FILE *target_file, const int elf_class) {
//   if (elf_class == 1) {
//     uint32_t e_shoff;
//     uint16_t e_shentsize;
//     uint16_t e_shnum;
//
//     fseek(target_file, E_SHOFF_OFF32, SEEK_SET);
//     fread(&e_shoff, sizeof(uint32_t), 1, target_file);
//     fseek(target_file, E_SHENTSIZE_OFF32, SEEK_SET);
//     fread(&e_shentsize, sizeof(uint16_t), 1, target_file);
//     fread(&e_shnum, sizeof(uint16_t), 1, target_file);
//
//     printf("\n───── Section Header Table ─────\n");
//
//     printf("%-4s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s\n", "N",
//            COL_W, "Name", COL_W, "Type", COL_W, "Address", COL_W, "Off",
//            COL_W, "Size", COL_W, "Link", COL_W, "Info", COL_W, "Align",
//            COL_W, "EntSize", COL_W, "Flags");
//     fseek(target_file, e_shoff, SEEK_SET);
//
//     for (int i = 0; i < e_shnum; i++) {
//       struct s_header32 s_head;
//       fread(&s_head, sizeof(s_head), 1, target_file);
//       char *section_name = "temp_name";
//
//       printf("[%02d] %-*s %-*d 0x%-*x 0x%-*x %-*d %-*d %-*d 0x%-*x %-*d
//       %-*d\n",
//              i, COL_W, section_name, COL_W, s_head.sh_type, COL_W - 2,
//              s_head.sh_addr, COL_W - 2, s_head.sh_offset, COL_W,
//              s_head.sh_size, COL_W, s_head.sh_link, COL_W, s_head.sh_info,
//              COL_W - 2, s_head.sh_addralign, COL_W, s_head.sh_entsize, COL_W,
//              s_head.sh_flags);
//     }
//   } else if (elf_class == 2) {
//     uint64_t e_shoff;
//     uint16_t e_shentsize;
//     uint16_t e_shnum;
//
//     fseek(target_file, E_SHOFF_OFF64, SEEK_SET);
//     fread(&e_shoff, sizeof(uint64_t), 1, target_file);
//     fseek(target_file, E_SHENTSIZE_OFF64, SEEK_SET);
//     fread(&e_shentsize, sizeof(uint16_t), 1, target_file);
//     fread(&e_shnum, sizeof(uint16_t), 1, target_file);
//
//     printf("\n───── Section Header Table ─────\n");
//
//     printf("%-4s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s %-*s\n", "N",
//            COL_W, "Name", COL_W, "Type", COL_W, "Address", COL_W, "Off",
//            COL_W, "Size", COL_W, "Link", COL_W, "Info", COL_W, "Align",
//            COL_W, "EntSize", COL_W, "Flags");
//
//     fseek(target_file, e_shoff, SEEK_SET);
//
//     for (int i = 0; i < e_shnum; i++) {
//       struct s_header64 s_head;
//       fread(&s_head, sizeof(s_head), 1, target_file);
//       char *section_name = "temp_name";
//
//       printf(
//           "[%02d] %-*s %-*d 0x%-*lx 0x%-*lx %-*d %-*d %-*d 0x%-*lx %-*d
//           %-*d\n", i, COL_W, section_name, COL_W, s_head.sh_type, COL_W - 2,
//           s_head.sh_addr, COL_W - 2, s_head.sh_offset, COL_W, s_head.sh_size,
//           COL_W, s_head.sh_link, COL_W, s_head.sh_info, COL_W - 2,
//           s_head.sh_addralign, COL_W, s_head.sh_entsize, COL_W,
//           s_head.sh_flags);
//     }
//   }
//   return 0;
// }
