/***************************************************************************/
/****************************** Includes ***********************************/
/***************************************************************************/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
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
    int noIndex;                /* parametr -I */
    const char *debugFileName;  /* parametr -d */
    FILE *outFile;
    int verbosity;              /* parametr -v */
    char **inputFiles;          /* input files */
    int numInputFiles;          /* number input files  */
    int usage;
    int isCLI;                  /* Command line interface mode */
} globalArgs;

FILE *debug_file;

/***************************************************************************/
/*********************** Function Prototypes *******************************/
/******  (should be static, if not they should be in '.h' file) ************/
/***************************************************************************/
static void usage(char *prog_name);
static int process_args(int argc, char **argv);
static int getline(FILE *fp);
static char *basename(char *path);

/***************************************************************************/
/********** Global Variables defined for this module only ******************/
/************************ (should be static) *******************************/
/***************************************************************************/
static const char *optString = "Il:o:v:ch?";

int process_args(int argc, char **argv) {
  int res;
  int opt;
  int d_level=DEBUGING_LEVEL;
    
  struct {
    char *text;
    DEBUG_LEVEL level; 
  }verbosity[] = {
    "NONE",     DEBUG_NONE,
    "ALL",      DEBUG_ALL,
    "TRACE",    DEBUG_TRACE,
    "DEVELOP",  DEBUG_DEVELOP, 
    "LOG",      DEBUG_LOG,
    "PROD",     DEBUG_PROD,
    NULL 
  };
  opt = getopt( argc, argv, optString );
  while( opt != -1 ) {
    switch( opt ) {
      case 'o':
          globalArgs.debugFileName = optarg;
          break;
      case 'v':
      {
        int i;
        for (i = 0; verbosity[i].text != NULL; i++)
        {
          if (!strcmp(verbosity[i].text, optarg))
          {
            d_level = verbosity[i].level;
            DEBUG_PROD("Debug level: [%s]\n", verbosity[i].text);
            break;
          }
        }
        break;
      }
      case 'c':
        globalArgs.isCLI = TRUE;
        break;
      case 'h':   /* intentional passage to the next case-блок */
      case '?':
          globalArgs.usage = TRUE;
          usage(basename(argv[0]));
          break;
      default:
          /* it's really impossible to get here */
          break;
    }

    opt = getopt( argc, argv, optString );
  }

  globalArgs.inputFiles = argv + optind;
  globalArgs.numInputFiles = argc - optind;

  DEBUG_PROD("Arg processed\n");


  SET_DEBUG_LVL(d_level);

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

void usage(char *prog_name) { 
  printf("Usage: %s [OPTIONS] FILES\n", prog_name); 
}

char *basename(char *path) {
  int pos = 0;
  for (int i = strlen(path) - 1; i >= 0; i--) {
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
  FILE *test;
  int opt = 0;
  int exit_code = EXIT_SUCCESS;

  debug_file = stderr;
  globalArgs.debugFileName = NULL;
  globalArgs.outFile = NULL;
  globalArgs.verbosity = 0;
  globalArgs.inputFiles = NULL;
  globalArgs.numInputFiles = 0;
  globalArgs.usage = 0;

  //DEBUG_PROD("OPEN DEBUG FILE\n");
  DEBUG_TRACE("Open debug file\n");
  OPEN_DEBUG_FILE("debug.out");
  SET_DEBUG_LVL(DEBUG_ALL);
  
  DEBUG_PROD("PROCESS ARGS\n");
  process_args(argc, argv);
  if (!globalArgs.usage) {
    if (!globalArgs.isCLI) {
      if (globalArgs.numInputFiles > 0) {
        int cur_file = 0;
        node_t *syntax_tree = NULL;
        for (; cur_file < globalArgs.numInputFiles; cur_file++) {
          DEBUG_PROD("Load %s \n", argv[cur_file]);
          if (source = loadProgram(globalArgs.inputFiles[cur_file])) {
            syntax_tree = parse(&source);
            DEBUG_PROD("\nParsed!!!\n");
            interprete(syntax_tree);
          }
          else { DEBUG_PROD("Failed load"); }
        }
      } else { DEBUG_PROD("NO INPUTS FILES"); }
    } else {
      int buffer_size = 1024;
      source = malloc(buffer_size);
      node_t *syntax_tree = NULL;
      while (printf(">"), fgets(source, buffer_size, stdin) != NULL) {
        if ((syntax_tree = parse(&source)) != NULL){
          DEBUG_PROD("\nPARSED\n"); 
          interprete(syntax_tree);
        }
        else  break;
      }
    }
  }
  /* Go to new line */
  puts("");
  return 0;
}
