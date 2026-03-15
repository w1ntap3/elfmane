#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define ERROR_PRINT "\e[0;31m"
#define WARNING_PRINT "\e[0;33m"
#define RESET_PRINT "\033[0m"
#define ERROR(s) fprintf(stderr, "%sERROR: %s%s\n", ERROR_PRINT, s, RESET_PRINT)
#define WARNING(s) fprintf(stderr, "%sWARNING: %s%s\n", WARNING_PRINT, s, RESET_PRINT)

// Elf header identification indexing
#define EI_LEN 16
#define EI_CLASS 4
#define EI_DATA 5
#define EI_VERSION 6
#define EI_OSABI 7
#define EI_ABIVERSION 8
#define EI_PAD 9


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
int e_ident_valid(struct e_header32 e_head, int *elf_class);

int main(int argc, char const *argv[])
{
        int elf_class = 0;

        if (argc < 2) {
                ERROR("Use \"elfmane [file] [options]\" or \"elfmane --help\"");
                return 1;
        }
        const char *target_name = argv[1];
        // Handle argv[1] being --help

        if (check_help(target_name)) return 0;

        FILE *target_file = fopen(target_name, "rb");

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
                        case 'h':
                                printf("elf header");
                                break;
                        case 'p':
                                printf("program header");
                                break;
                        case 's':
                                printf("section header");
                                break;
                        case 'q':
                                print("quiet (no error/warnings) mode");
                        default:
                                printf("general essentials");
                                break;
                        }
                }
        }

        fclose(target_file);
        return 0;
}

int check_help(const char *arg) {
        if (!strcmp(arg, "--help")) {
                printf("9vicious is a weirdo");
                return 1;
        }
        return 0;
}

int e_ident_valid(struct e_header32 e_head, int *elf_class) {
        uint8_t return_val = 0; // Done so that all errors and warning can be seen before program aborts

        if (((*elf_class = e_head.e_ident[EI_CLASS]) != 1) && (*elf_class != 2)) { // 0x04 - Class
                ERROR("ELF class is neither 32- or 64-bit.");
                return_val = 1;
        }

        if (e_head.e_ident[EI_DATA] != 1 && e_head.e_ident[EI_DATA] != 2) { // 0x05 - Endianness
                ERROR("Endianness is neither little or big.");
                return_val = 1;
        }

        if (e_head.e_ident[EI_VERSION] != 1) { // 0x06 - ELF Version
                WARNING("ELF Version is not set to 1.");
        }

        if (e_head.e_ident[EI_OSABI] > 0x12) { // 0x07 - ABI target OS
                WARNING("OS's ABI undefined");
        }

        // no one cares about ABIVERSION lmao

        uint8_t corrupted = 0;
        for (int i = 0x09; i <= EI_LEN; i++) {
                if (e_head.e_ident[i] != 0x00) corrupted = 1;
        }

        if (corrupted) WARNING("File might be corrupted (Padding bytes are not 0)");
        return return_val;
}