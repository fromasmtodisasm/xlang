#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "fileutils.h"
#include "interpreter.h"
#include "preprocessor.h"
#include "debug.h"

#if !defined _MSC_VER
#define getline mygetline
#endif

int getline(FILE *fp) {
  int ch = EOF;
  int len = 0;
  int lines = 0;
  while ((ch = getc(fp)) != EOF) {
    if (ch != '\n') {
      len++;
    } else {
      len++;
      break;
    }
  }
  return len;
}

char *loadProgram(char *name) {
  FILE *program;
  char *source = NULL;
  //DEBUG_PROD(__FUNCTION__);
  if ((program = preprocess(name)) != NULL) {
    DEBUG_PROD("Preprocessed %s\n", name);
    source = file2str(program);
  }
  return source;
}

void usage(char *prog_name) { printf("Usage: %s file\n", prog_name); }

char *basename(char *path) {
  int pos = 0;
  for (int i = strlen(path) - 1; i > 0; i--) {
    if (path[i] == '\\') {
      break;
    }
    pos = i;
  }
  printf("%s\n", path + pos);
  return path + pos;
}

int main(int argc, char **argv) {
  char *source = NULL;
  char buf[255];
  char *expression = source;
  FILE *test;

  //SET_DEBUG_LVL(DEBUG_PROD);
  //SET_DEBUG_LVL(DEBUG_TRACE);
  SET_DEBUG_LVL(DEBUG_ALL);
  
  if (argc > 1) {
    expression = buf;
    int cur_file = 1;
    node_t *syntax_tree = NULL;
    for (; cur_file < argc; cur_file++) {
      DEBUG_PROD("Load %s \n", argv[cur_file]);
      if (source = loadProgram(argv[cur_file])) {
        syntax_tree = parse(&source);
        DEBUG_PROD("\nParsed!!!\n");
        interprete(syntax_tree);
      }
      else { DEBUG_PROD("Failed load"); }
    }
  } else {
    int buffer_size = 1024;
    source = malloc(buffer_size);
    usage(basename(argv[0]));
    while (fgets(source, buffer_size, stdin) != NULL) {
      if (parse(&source) == NULL)
        break;
    }
  }
  return 0;
}
