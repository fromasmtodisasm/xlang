#include "fileutils.h"
#include <malloc.h>
#include <string.h>
#include "common.h"
#include "debug.h"

char *file2str(FILE *file) {
  char *str = NULL;
  int len = 0;
  int real_len = 0;
  int lines = 0;

  DEBUG_LOG("In function %s\n", __FUNCTION__);
  fseek(file, 0, SEEK_END);
  len = ftell(file);
  fseek(file, 0, SEEK_SET);
  DEBUG_LOG("Allocate %d bytes for program\n", len);
  str = (char *)malloc(len + 1);
  fread(str, 1, len, file);
  str[len] = '\0';
  fclose(file);

  return str;
}


char *strndup(const char *str, size_t n) {
  char *out;
  size_t len = strlen(str);
  if (len > n) len = n;
  out = (char*)malloc(len);
  strncpy(out, str, len);
  return out;
}
  
