#ifndef LOG_H
#define LOG_H

void log_err(const char *format, ...);
void log_warn(const char *format, ...);
void log_inf(const char *format, ...);
void log_sys(const char *format, ...);

#endif // !LOG_H
