#ifndef _DEBUG_H_
#define _DEBUG_H_

/***************************************************************************/
/************************** Other Includes *********************************/
/***************************************************************************/
#include <stdio.h>
#include "config.h"

/***************************************************************************/
/***************************** Defines *************************************/
/***************************************************************************/
#define mkstr(s)  # s
#define DEBUG_PRINT(file,...) fprintf(file, __VA_ARGS__)
#define DEBUG_FILE debug_file
#define OPEN_DEBUG_FILE(file) DEBUG_FILE = freopen(file, "w", stderr)

#ifdef DEBUG_BUILD

#define SET_DEBUG_LVL(lvl) debug_level = lvl;
#define DEBUG(lvl,...) (debug_level <= lvl) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0
#define DEBUG_NONE(...) (debug_level == DEBUG_PROD) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0
#define DEBUG_ALL(...) (debug_level <= DEBUG_ALL) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0
#define DEBUG_TRACE(...) ( (debug_level <= DEBUG_TRACE) \
  ? DEBUG_PRINT(DEBUG_FILE, "=>%s:%d\t-- ", __FUNCTION__, __LINE__), DEBUG_PRINT(DEBUG_FILE, mkstr(\n\t) __VA_ARGS__) \
  : (void)0 ) 
#define DEBUG_DEVELOP(...) (debug_level <= DEBUG_DEVELOP) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0
#define DEBUG_LOG(...)  (debug_level <= DEBUG_LOG) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0
#define DEBUG_PROD(...) if (debug_level <= DEBUG_PROD) DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__)

#else

#define SET_DEBUG_LVL(lvl) debug_level = DEBUG_PROD;
#define DEBUG(lvl,...) ((debug_level <= lvl) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0)
#define DEBUG_ALL(...) (void)0
#define DEBUG_NONE(...) (void)0
#define DEBUG_TRACE(...) (void)0
#define DEBUG_DEVELOP(...) (void)0
#define DEBUG_LOG(...)  (debug_level <= DEBUG_LOG) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0
#define DEBUG_PROD(...) (debug_level <= DEBUG_PROD) ? DEBUG_PRINT(DEBUG_FILE, __VA_ARGS__) : (void)0

#endif
/***************************************************************************/
/*********************** Typedefs/Structures *******************************/
/***************************************************************************/
extern int debug_level;
typedef enum DEBUG_LEVEL {
  DEBUG_NONE,
  DEBUG_ALL,
  DEBUG_TRACE,
  DEBUG_DEVELOP,
  DEBUG_LOG,
  DEBUG_PROD
}DEBUG_LEVEL;

/***************************************************************************/
/********************* Externally Defined Globals **************************/
/***************************************************************************/
extern FILE *debug_file;

#endif /* _DEBUG_H_ */
