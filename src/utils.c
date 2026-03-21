#include <stdint.h>

#define ERROR_PRINT "\e[0;31m"
#define WARNING_PRINT "\e[0;33m"
#define HIGHLIGHT_PRINT "\e[0;32m"
#define RESET_PRINT "\033[0m"

#define ENDIANNESS_STRING(n) n ? "Big" : "Little"

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

char* os_abi_lookup(int osabi) {
        switch (osabi)
        {
        case 0x00:
                return "System V";
                break;
        case 0x01:
                return "HP-UX";
                break;
        case 0x02:
                return "NetBSD";
                break;
        case 0x03:
                return "Linux";
                break;
        case 0x04:
                return "GNU Hurd";
                break;
        case 0x06:
                return "Solaris";
                break;
        case 0x07:
                return "AIX (Monterey)";
                break;
        case 0x08:
                return "IRIX";
                break;
        case 0x09:
                return "FreeBSD";
                break;
        case 0x0A:
                return "Tru64";
                break;
        case 0x0B:
                return "Novell Modesto";
                break;
        case 0x0C:
                return "OpenBSD";
                break;
        case 0x0D:
                return "OpenVMS";
                break;
        case 0x0E:
                return "NonStop Kernel";
                break;
        case 0x0F:
                return "AROS";
                break;
        case 0x10:
                return "FenixOS";
                break;
        case 0x11:
                return "Nuxi CloudABI";
                break;
        case 0x12:
                return "Stratus Technologies OpenVOS";
                break;
        default:
                return "Undefined";
                break;
        }
}

char* padding_bytes_check(uint8_t e_ident[]) {
        if (e_ident[9] == e_ident[10] == e_ident[11] == e_ident[12] == e_ident[13] == e_ident[14] == e_ident[15] == 0) {
                return "As expected";
        } else {
                return "Unexpected values (File might be corrupted)";
        }
}

char* file_type_lookup(uint16_t e_type) {
        switch (e_type)
        {
        case 0x00:
                return "Unknown (ET_NONE)";
                break;
        case 0x01:
                return "Relocatable file (ET_REL)";
                break;
        case 0x02:
                return "Executable file (ET_EXEC)";
                break;
        case 0x03:
                return "Shared object (ET_DYN)";
                break;
        case 0x04:
                return "Core file (ET_CORE)";
                break;
        case 0xFE00:
                return "Reserved inclusive range (ET_LOOS)";
                break;
        case 0xFEFF:
                return "Reserved inclusive range (ET_HIOS)";
                break;
        case 0xFF00:
                return "Reserved inclusive range (ET_LOPROC)";
                break;
        case 0xFFFF:
                return "Reserved inclusive range (ET_HIPROC)";
                break;
        default:
                break;
        }
}

char* target_isa_lookup(uint16_t e_machine) {
	switch (e_machine)
	{
		case 0x00:
			return "No specific instruction set";
			break;
		case 0x01:
			return "AT&T WE 32100";
			break;
		case 0x02:
			return "SPARC";
			break;
		case 0x03:
			return "x86";
			break;
		case 0x04:
			return "Motorola 68000 (M68k)";
			break;
		case 0x05:
			return "Motorola 88000 (M88k)";
			break;
		case 0x06:
			return "Intel MCU";
			break;
		case 0x07:
			return "Intel 80860";
			break;
		case 0x08:
			return "MIPS";
			break;
		case 0x09:
			return "IBM System/370";
			break;
		case 0x0A:
			return "MIPS RS3000 Little-endian";
			break;
		case 0x0F:
			return "Hewlett-Packard PA-RISC";
			break;
		case 0x13:
			return "Intel 80960";
			break;
		case 0x14:
			return "PowerPC";
			break;
		case 0x15:
			return "PowerPC (64-bit)";
			break;
		case 0x16:
			return "S390, including S390x";
			break;
		case 0x17:
			return "IBM SPU/SPC";
			break;
		case 0x24:
			return "NEC V800";
			break;
		case 0x25:
			return "Fujitsu FR20";
			break;
		case 0x26:
			return "TRW RH-32";
			break;
		case 0x27:
			return "Motorola RCE";
			break;
		case 0x28:
			return "Arm (up to Armv7/AArch32)";
			break;
		case 0x29:
			return "Digital Alpha";
			break;
		case 0x2A:
			return "SuperH";
			break;
		case 0x2B:
			return "SPARC Version 9";
			break;
		case 0x2C:
			return "Siemens TriCore embedded processor";
			break;
		case 0x2D:
			return "Argonaut RISC Core";
			break;
		case 0x2E:
			return "Hitachi H8/300";
			break;
		case 0x2F:
			return "Hitachi H8/300H";
			break;
		case 0x30:
			return "Hitachi H8S";
			break;
		case 0x31:
			return "Hitachi H8/500";
			break;
		case 0x32:
			return "IA-64";
			break;
		case 0x33:
			return "Stanford MIPS-X";
			break;
		case 0x34:
			return "Motorola ColdFire";
			break;
		case 0x35:
			return "Motorola M68HC12";
			break;
		case 0x36:
			return "Fujitsu MMA Multimedia Accelerator";
			break;
		case 0x37:
			return "Siemens PCP";
			break;
		case 0x38:
			return "Sony nCPU embedded RISC processor";
			break;
		case 0x39:
			return "Denso NDR1 microprocessor";
			break;
		case 0x3A:
			return "Motorola Star*Core processor";
			break;
		case 0x3B:
			return "Toyota ME16 processor";
			break;
		case 0x3C:
			return "STMicroelectronics ST100 processor";
			break;
		case 0x3D:
			return "Advanced Logic Corp. TinyJ embedded processor family";
			break;
		case 0x3E:
			return "AMD x86-64";
			break;
		case 0x3F:
			return "Sony DSP Processor";
			break;
		case 0x40:
			return "Digital Equipment Corp. PDP-10";
			break;
		case 0x41:
			return "Digital Equipment Corp. PDP-11";
			break;
		case 0x42:
			return "Siemens FX66 microcontroller";
			break;
		case 0x43:
			return "STMicroelectronics ST9+ 8/16-bit microcontroller";
			break;
		case 0x44:
			return "STMicroelectronics ST7 8-bit microcontroller";
			break;
		case 0x45:
			return "Motorola MC68HC16 Microcontroller";
			break;
		case 0x46:
			return "Motorola MC68HC11 Microcontroller";
			break;
		case 0x47:
			return "Motorola MC68HC08 Microcontroller";
			break;
		case 0x48:
			return "Motorola MC68HC05 Microcontroller";
			break;
		case 0x49:
			return "Silicon Graphics SVx";
			break;
		case 0x4A:
			return "STMicroelectronics ST19 8-bit microcontroller";
			break;
		case 0x4B:
			return "Digital VAX";
			break;
		case 0x4C:
			return "Axis Communications 32-bit embedded processor";
			break;
		case 0x4D:
			return "Infineon Technologies 32-bit embedded processor";
			break;
		case 0x4E:
			return "Element 14 64-bit DSP Processor";
			break;
		case 0x4F:
			return "LSI Logic 16-bit DSP Processor";
			break;
		case 0x8C:
			return "TMS320C6000 Family";
			break;
		case 0xAF:
			return "MCST Elbrus e2k";
			break;
		case 0xB7:
			return "Arm 64-bits (Armv8/AArch64)";
			break;
		case 0xDC:
			return "Zilog Z80";
			break;
		case 0xF3:
			return "RISC-V";
			break;
		case 0xF7:
			return "Berkeley Packet Filter";
			break;
		case 0x101:
			return "WDC 65C816";
			break;
		case 0x102:
			return "LoongArch";
			break;
		default:
			return "Undefined";
			break;
	}
}
