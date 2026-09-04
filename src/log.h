#ifndef LOG_H
#define LOG_H

void log_err(const char *fs, ...);
void log_war(const char *fs, ...);
void log_inf(const char *fs, ...);
void log_sys(const char *fs, ...);

#endif // !LOG_H
