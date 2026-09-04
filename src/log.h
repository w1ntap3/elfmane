#ifndef LOG_H
#define LOG_H

// ANSI Colors
#define ERROR_COLOR "\e[0;31m"
#define WARNING_COLOR "\e[0;33m"
#define HIGHLIGHT_COLOR "\e[0;32m"
#define RESET_PRINT "\033[0m"

// Macros
// #define ERROR(s) fprintf(stderr, "%s%s%s\n", ERROR_PRINT, s, RESET_PRINT)
#define ERROR(s) ERROR_COLOR s RESET_PRINT
// #define WARNING(s) fprintf(stderr, "%s%s%s\n", WARNING_PRINT, s, RESET_PRINT)
#define WARNING(s) WARNING_COLOR s RESET_PRINT
#define HIGHLIGHT(s) HIGHLIGHT_COLOR s RESET_PRINT

#endif // !LOG_H
