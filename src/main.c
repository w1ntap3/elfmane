#include <stdio.h>
#include <stdint.h>
#include "utils.c"

#define ERROR_PRINT "\e[0;31m"
#define WARNING_PRINT "\e[0;33m"
#define HIGHLIGHT_PRINT "\e[0;32m"
#define RESET_PRINT "\033[0m"

#define ERROR(s) fprintf(stderr, "%s%s%s\n", ERROR_PRINT, s, RESET_PRINT)
#define WARNING(s) fprintf(stderr, "%s%s%s\n", WARNING_PRINT, s, RESET_PRINT)

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

struct e_header32{
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

struct e_header64{
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

struct p_header32{
        uint32_t p_type;
        uint32_t p_offset;
        uint32_t p_vaddr;
        uint32_t p_paddr;
        uint32_t p_filesz;
        uint32_t p_memsz;
        uint32_t p_flags;
        uint32_t p_align;
};

struct p_header64{
        uint32_t p_type;
        uint32_t p_flags;
        uint64_t p_offset;
        uint64_t p_vaddr;
        uint64_t p_paddr;
        uint64_t p_filesz;
        uint64_t p_memsz;
        uint64_t p_align;
};

struct s_header32{
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

struct s_header64{
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

struct e_header32 e_head32;
struct e_header64 e_head64;
struct p_header32 p_head32;
struct p_header64 p_head64;
struct s_header32 s_head32;
struct s_header64 s_head64;

int check_help(const char *arg);
int e_ident_valid(const struct e_header32 e_head, int *elf_class);
int e_header_info(FILE *target_file, const int elf_class);
int p_header_info(FILE *target_file, const int elf_class);
int s_header_info(FILE *target_file, const int elf_class);

int main(int argc, char const *argv[])
{
        int elf_class = 0;

        if (argc < 2) {
                ERROR("Use \"elfmane [FILE_PATH] [OPTIONS]\" or \"elfmane --help\"");
                return 1;
        }

        if (check_help(argv[1])) return 0;

        FILE *target_file = fopen(argv[1], "rb");

        if (target_file == NULL) {
                ERROR("Could not open the file.");
                return 1;
        }
        fread(e_head32.e_ident, sizeof(uint8_t), EI_LEN, target_file);
        // Check validity of e_ident to identify the class

        if (e_ident_valid(e_head32, &elf_class)) return 1;

        for (int arg_n = 2; arg_n < argc; arg_n++) {
                if (argv[arg_n][0] == '-') {
                        switch (argv[arg_n][1])
                        {
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

int check_help(const char *arg) {
        if (!strcmp(arg, "--help") || !strcmp(arg, "-h")) {
                printf("Usage: elfmane [FILE_PATH] [OPTIONS]\n"
                        "%-20s %s\n"
                        "OPTIONS:\n"
                        "%-20s %s\n"
                        "%-20s %s\n"
                        "%-20s %s\n"
                        "%-20s %s\n"
                        "FLAGS:\n"
                        "%-20s %s\n"
                        "COLOR CODES:\n"
                        "%sRED: Error occurred%s\n"
                        "%sYELLOW: Warnings for unusual information in the file%s\n"
                        "%sGREEN: Highlight important (commonly looked for) fields%s\n",
                        "-h, --help", "Show the help list",
                        "[NO OPTIONS]", "All information elfmane can give",
                        "-e", "ELF header fields",
                        "-p", "Program header fields",
                        "-s", "Section header fields",
                        "-f", "Force read (ignore warnings/errors)",
                        ERROR_PRINT, RESET_PRINT,
                        WARNING_PRINT, RESET_PRINT,
                        HIGHLIGHT_PRINT, RESET_PRINT
                        );
                return 1;
        }
        return 0;
}

int e_ident_valid(const struct e_header32 e_head, int *elf_class) {
        uint8_t return_val = 0; // Return after checking each warning/error case

        if (((*elf_class = e_head.e_ident[EI_CLASS]) != 1) && (*elf_class != 2)) {
                ERROR("ELF class is neither 32- or 64-bit.");
                return_val = 1;
        }

        if (e_head.e_ident[EI_DATA] != 1 && e_head.e_ident[EI_DATA] != 2) {
                ERROR("Endianness is neither little or big.");
                return_val = 1;
        }

        if (e_head.e_ident[EI_VERSION] != 1) {
                WARNING("ELF Version is not set to 1.");
        }

        if (e_head.e_ident[EI_OSABI] > 0x12) {
                WARNING("OS's ABI undefined");
        }

        // no one cares about ABIVERSION

        uint8_t corrupted = 0; // Padding bytes corruption check
        for (int i = 0x09; i <= EI_LEN; i++) {
                if (e_head.e_ident[i] != 0x00) corrupted = 1;
        }

        if (corrupted) WARNING("File might be corrupted (Padding bytes are not 0)");
        return return_val;
}

int e_header_info(FILE *target_file, const int elf_class) {
        rewind(target_file);

        if (elf_class == 1) {
                fread(&e_head32, sizeof(e_head32), 1, target_file);
                uint8_t elf_class_bits = 32;
                printf("\n───── ELF Header ─────\n");

                printf("%s(Important fields are highlighted like this)%s\n", HIGHLIGHT_PRINT, RESET_PRINT);
                printf("Magic: %c %c %c (0x%x %x %x %x)\n"
                        "%sClass: %d (%d-bit)%s\n"
                        "%sEndianness: %d (%s endian)%s\n"
                        "Version: %d\n"
                        "Target OS ABI: %s (0x%x)\n"
                        "ABI Version: %d\n"
                        "Padding bytes status: %s\n"
                        "%sFile types: %s%s\n"
                        "Target ISA: %s\n"
                        "Process entry: 0x%x\n"
                        "Program table: 0x%x\n"
                        "Section table: 0x%x\n"
                        "Flags: 0x%x\n"
                        "ELF Header Size: %d bytes (%s)\n"
                        "Program table entry size: 0x%x (%s)\n"
                        "%sEntries in the program header: %d%s\n"
                        "Section table entry size: 0x%x (%s)\n"
                        "%sEntries in the section header: %d%s\n"
                        "Index of section names: %d\n",
                        e_head32.e_ident[1],e_head32.e_ident[2],e_head32.e_ident[3],e_head32.e_ident[0],e_head32.e_ident[1],e_head32.e_ident[2],e_head32.e_ident[3],
                        HIGHLIGHT_PRINT, elf_class, elf_class_bits, RESET_PRINT,
                        HIGHLIGHT_PRINT, e_head32.e_ident[EI_DATA], ENDIANNESS_STRING(e_head32.e_ident[EI_DATA]-1), RESET_PRINT,
                        e_head32.e_ident[EI_VERSION],
                        os_abi_lookup(e_head32.e_ident[EI_OSABI]), e_head32.e_ident[EI_OSABI],
                        e_head32.e_ident[EI_ABIVERSION],
                        padding_bytes_check(e_head32.e_ident),
                        HIGHLIGHT_PRINT, file_type_lookup(e_head32.e_type), RESET_PRINT,
                        target_isa_lookup(e_head32.e_machine),
                        e_head32.e_entry,
                        e_head32.e_phoff,
                        e_head32.e_shoff,
                        e_head32.e_flags,
                        e_head32.e_ehsize, "Normally 52 bytes",
                        e_head32.e_phentsize, "Typically 0x20",
                        HIGHLIGHT_PRINT, e_head32.e_phnum, RESET_PRINT,
                        e_head32.e_shentsize, "Typically 0x28",
                        HIGHLIGHT_PRINT, e_head32.e_shnum, RESET_PRINT,
                        e_head32.e_shstrndx
                );
        } else if (elf_class == 2) {
                fread(&e_head64, sizeof(e_head64), 1, target_file);
                uint8_t elf_class_bits = 64;
                printf("\n───── ELF Header ─────\n");

                printf("\tMagic: %c %c %c (0x%x %x %x %x)\n"
                        "%s\tClass: %d (%d-bit)%s\n"
                        "%s\tEndianness: %d (%s endian)%s\n"
                        "\tVersion: %d\n"
                        "\tTarget OS ABI: %s (0x%x)\n"
                        "\tABI Version: %d\n"
                        "\tPadding bytes status: %s\n"
                        "%s\tFile type: %s%s\n"
                        "\tTarget ISA: %s\n"
                        "\tProcess entry: 0x%lx\n"
                        "\tProgram table: 0x%lx\n"
                        "\tSection table: 0x%lx\n"
                        "\tFlags: 0x%x\n"
                        "\tELF Header Size: %d bytes (%s)\n"
                        "\tProgram table entry size: 0x%x (%s)\n"
                        "%s\tEntries in the program header: %d%s\n"
                        "\tSection table entry size: 0x%x (%s)\n"
                        "%s\tEntries in the section header: %d%s\n"
                        "\tIndex of section names: %d\n",
                        e_head64.e_ident[1],e_head64.e_ident[2],e_head64.e_ident[3],e_head64.e_ident[0],e_head64.e_ident[1],e_head64.e_ident[2],e_head64.e_ident[3],
                        HIGHLIGHT_PRINT, elf_class, elf_class_bits, RESET_PRINT,
                        HIGHLIGHT_PRINT, e_head64.e_ident[EI_DATA], ENDIANNESS_STRING(e_head64.e_ident[EI_DATA]-1), RESET_PRINT,
                        e_head64.e_ident[EI_VERSION],
                        os_abi_lookup(e_head64.e_ident[EI_OSABI]), e_head64.e_ident[EI_OSABI],
                        e_head64.e_ident[EI_ABIVERSION],
                        padding_bytes_check(e_head64.e_ident),
                        HIGHLIGHT_PRINT, file_type_lookup(e_head64.e_type), RESET_PRINT,
                        target_isa_lookup(e_head64.e_machine),
                        e_head64.e_entry,
                        e_head64.e_phoff,
                        e_head64.e_shoff,
                        e_head64.e_flags,
                        e_head64.e_ehsize, "Normally 64 bytes",
                        e_head64.e_phentsize, "Typically 0x38",
                        HIGHLIGHT_PRINT, e_head64.e_phnum, RESET_PRINT,
                        e_head64.e_shentsize, "Typically 0x40",
                        HIGHLIGHT_PRINT, e_head64.e_shnum, RESET_PRINT,
                        e_head64.e_shstrndx
                );
        }
        return 0;
}

int p_header_info(FILE *target_file, const int elf_class) {
        if (elf_class == 1) {
                uint32_t e_phoff;
                uint16_t e_phentsize;
                uint16_t e_phnum;

                fseek(target_file, E_PHOFF_OFF32, SEEK_SET);
                fread(&e_phoff, sizeof(uint32_t), 1, target_file);
                fseek(target_file, E_PHENTSIZE_OFF32, SEEK_SET);
                fread(&e_phentsize, sizeof(uint16_t), 1, target_file);
                fread(&e_phnum, sizeof(uint16_t), 1, target_file);

                printf("\n───── Program Header Table ─────\n");
                
                printf("%-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s\n",
                        "Type",
                        "Off",
                        "VAddress",
                        "PAddress",
                        "SizeFile",
                        "SizeMem",
                        "Flags",
                        "Align");
                fseek(target_file, e_phoff, SEEK_SET);

                for (int i = 0; i < e_phnum; i++) {
                        struct p_header32 p_head;
                        char flag_str[4]; // {'R', 'W', 'X', '\0'} (len = 4)
                        fread(&p_head, sizeof(p_head), 1, target_file);
                        p_flag_string(flag_str, p_head.p_flags);

                        printf("%d %-10s 0x%-10x 0x%-10x 0x%-10x %-10d %-10d %-10s %-10d\n",
                                i,
                                p_type_lookup(p_head.p_type),
                                p_head.p_offset,
                                p_head.p_vaddr,
                                p_head.p_paddr,
                                p_head.p_filesz,
                                p_head.p_memsz,
                                flag_str,
                                p_head.p_align);
                }
        } else if (elf_class == 2) {
                uint64_t e_phoff;
                uint16_t e_phentsize;
                uint16_t e_phnum;

                fseek(target_file, E_PHOFF_OFF64, SEEK_SET);
                fread(&e_phoff, sizeof(uint64_t), 1, target_file);
                fseek(target_file, E_PHENTSIZE_OFF64, SEEK_SET);
                fread(&e_phentsize, sizeof(uint16_t), 1, target_file);
                fread(&e_phnum, sizeof(uint16_t), 1, target_file);

                printf("\n───── Program Header Table ─────\n");

                printf("%-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s\n",
                        "Type",
                        "Off",
                        "VAddress",
                        "PAddress",
                        "SizeFile",
                        "SizeMem",
                        "Flags",
                        "Align");
                fseek(target_file, e_phoff, SEEK_SET);
                for (int i = 0; i < e_phnum; i++) {
                        struct p_header64 p_head;
                        char flag_str[4];
                        fread(&p_head, sizeof(p_head), 1, target_file);
                        p_flag_string(flag_str, p_head.p_flags);

                        printf("%d %-10s 0x%-10lx 0x%-10lx 0x%-10lx %-10d %-10d %-10s %-10d\n",
                                i,
                                p_type_lookup(p_head.p_type),
                                p_head.p_offset,
                                p_head.p_vaddr,
                                p_head.p_paddr,
                                p_head.p_filesz,
                                p_head.p_memsz,
                                flag_str,
                                p_head.p_align);
                }
        }
        return 0;
}

int s_header_info(FILE *target_file, const int elf_class) {
        if (elf_class == 1) {
                uint32_t e_shoff;
                uint16_t e_shentsize;
                uint16_t e_shnum;

                fseek(target_file, E_SHOFF_OFF32, SEEK_SET);
                fread(&e_shoff, sizeof(uint32_t), 1, target_file);
                fseek(target_file, E_SHENTSIZE_OFF32, SEEK_SET);
                fread(&e_shentsize, sizeof(uint16_t), 1, target_file);
                fread(&e_shnum, sizeof(uint16_t), 1, target_file);

                printf("\n───── Section Header Table ─────\n");
                
                printf("%-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s\n",
                        "Name",
                        "Type",
                        "Address",
                        "Off",
                        "Size",
                        "Link",
                        "Info",
                        "Align",
                        "EntSize",
                        "Flags");
                fseek(target_file, e_shoff, SEEK_SET);

                for (int i = 0; i < e_shnum; i++) {
                        struct s_header32 s_head;
                        fread(&s_head, sizeof(s_head), 1, target_file);
                        char* section_name = "temp_name";

                        printf("%-10s %-10d 0x%-10x 0x%-10x %-10d %-10d %-10d 0x%-10x %-10d %-10d\n",
                        section_name,
                        s_head.sh_type, //s_type_lookup(s_head.sh_type),
                        s_head.sh_addr,
                        s_head.sh_offset,
                        s_head.sh_size,
                        s_head.sh_link,
                        s_head.sh_info,
                        s_head.sh_addralign,
                        s_head.sh_entsize,
                        s_head.sh_flags);
                }
        } else if (elf_class == 2) {
                uint64_t e_shoff;
                uint16_t e_shentsize;
                uint16_t e_shnum;

                fseek(target_file, E_SHOFF_OFF64, SEEK_SET);
                fread(&e_shoff, sizeof(uint64_t), 1, target_file);
                fseek(target_file, E_SHENTSIZE_OFF64, SEEK_SET);
                fread(&e_shentsize, sizeof(uint16_t), 1, target_file);
                fread(&e_shnum, sizeof(uint16_t), 1, target_file);

                printf("\n───── Section Header Table ─────\n");
                
                printf("\tN  %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s %-10s\n",
                        "Name",
                        "Type",
                        "Address",
                        "Off",
                        "Size",
                        "Link",
                        "Info",
                        "Align",
                        "EntSize",
                        "Flags");
                
                fseek(target_file, e_shoff, SEEK_SET);

                for (int i = 0; i < e_shnum; i++) {
                        struct s_header64 s_head;
                        fread(&s_head, sizeof(s_head), 1, target_file);
                        char* section_name = "temp_name";

                        printf("%2d %-10s %-10s 0x%-10x 0x%-10x %-10d %-10d %-10d 0x%-10x %-10d %-10d\n",
                        i,
                        section_name,
                        s_type_lookup(s_head.sh_type),
                        s_head.sh_addr,
                        s_head.sh_offset,
                        s_head.sh_size,
                        s_head.sh_link,
                        s_head.sh_info,
                        s_head.sh_addralign,
                        s_head.sh_entsize,
                        s_head.sh_flags);
                }
        }
        return 0;
}