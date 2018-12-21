/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "fileutils.h"
#include "interpreter.h"
#include "preprocessor.h"
#include "debug.h"

/***************************************************************************/
/***************** Defines used by this module only ************************/
/***************************************************************************/
#if !defined _MSC_VER
#define getline mygetline
#endif

#define DEBUGING_LEVEL DEBUG_TRACE

/***************************************************************************/
/************************* Global Variables ********************************/
/***************************************************************************/
struct globalArgs_t {
    int noIndex;                /* параметр -I */
    char *langCode;             /* параметр -l */
    const char *outFileName;    /* параметр -o */
    FILE *outFile;
    int verbosity;              /* параметр -v */
    char **inputFiles;          /* входные файлы */
    int numInputFiles;          /* число входных файлов */
} globalArgs;

/***************************************************************************/
/********** Global Variables defined for this module only ******************/
/************************ (should be static) *******************************/
/***************************************************************************/
static const char *optString = "Il:o:vh?";

int process_args(int argc, char **argv) {
  int res;

  while (getopt(argc, argv, optString) != -1)
  {
    printf("Here\n"); 
  }

  return res;
}

int getline(FILE *fp) {
  int ch = EOF;
  int len = 0;
  int lines = 0;
  
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
  int opt = 0;

  globalArgs.noIndex = 0;     /* false */
  globalArgs.langCode = NULL;
  globalArgs.outFileName = NULL;
  globalArgs.outFile = NULL;
  globalArgs.verbosity = 0;
  globalArgs.inputFiles = NULL;
  globalArgs.numInputFiles = 0;

  SET_DEBUG_LVL(DEBUGING_LEVEL);
  
  process_args(argc, argv);
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
