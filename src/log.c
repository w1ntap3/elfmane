#include "log.h"
#include "stdarg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define MAX_LOG_LEN 2048

#define ERR_COL "\033[31m"
#define WAR_COL "\033[33m"
#define INF_COL "\033[32m"
#define SYS_COL "\033[34m"
#define RES_COL "\033[0m"

#define ERR_TAG "ERROR"
#define WAR_TAG "WARNING"
#define INF_TAG "INFO"
#define SYS_TAG "ELFMANE"

static inline int validate_fs(const char *fs) {
  if (fs == NULL || (strlen(fs) == 0)) {
    log_sys("Received an empty request to log.");
    return 1;
  }
  if (strlen(fs) > MAX_LOG_LEN) {
    log_sys("Dropped a log with length %d. (Max is %d)", strlen(fs),
            MAX_LOG_LEN);
    return 1;
  }
  return 0;
}

static void log_skeleton(const char *tag, const char *color, const char *fs,
                         va_list args, FILE *__restrict stream) {
  char buf[MAX_LOG_LEN];

  vsnprintf(buf, sizeof(buf), fs, args);
  fprintf(stream, "%s%s: %s\n" RES_COL, color, tag, buf);
}

void log_err(const char *fs, ...) {
  int err = validate_fs(fs);
  if (err)
    return;
  va_list args;
  va_start(args, fs);

  log_skeleton(ERR_TAG, ERR_COL, fs, args, stderr);
  va_end(args);
}

void log_war(const char *fs, ...) {

  int err = validate_fs(fs);
  if (err)
    return;
  va_list args;
  va_start(args, fs);

  log_skeleton(WAR_TAG, WAR_COL, fs, args, stderr);
  va_end(args);
}
void log_inf(const char *fs, ...) {

  int err = validate_fs(fs);
  if (err)
    return;
  va_list args;
  va_start(args, fs);

  log_skeleton(INF_TAG, INF_COL, fs, args, stderr);
  va_end(args);
}
void log_sys(const char *fs, ...) {

  int err = validate_fs(fs);
  if (err)
    return;
  va_list args;
  va_start(args, fs);

  log_skeleton(SYS_TAG, SYS_COL, fs, args, stderr);
  va_end(args);
}
