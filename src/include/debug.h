#pragma once
#include "config.h"

extern int debug_level;
typedef enum DEBUG_LEVEL {
  DEBUG_ALL,
  DEBUG_TRACE,
  DEBUG_DEVELOP,
  DEBUG_LOG,
  DEBUG_PROD
}DEBUG_LEVEL;
#ifdef DEBUG_BUILD

#define SET_DEBUG_LVL(lvl) debug_level = lvl;
#define DEBUG(lvl,...) (debug_level <= lvl) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_NONE(...) (debug_level == DEBUG_PROD) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_ALL(...) (debug_level <= DEBUG_ALL) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_TRACE(...) ( (debug_level <= DEBUG_TRACE) \
  ? fprintf(stderr,"=>%s:%d\t-- ", __FUNCTION__, __LINE__), fprintf(stderr, __VA_ARGS__) \
  : (void)0 ) 
#define DEBUG_DEVELOP(...) (debug_level <= DEBUG_DEVELOP) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_LOG(...)  (debug_level <= DEBUG_LOG) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_PROD(...) if (debug_level <= DEBUG_PROD) fprintf(stderr, __VA_ARGS__)

#else

#define SET_DEBUG_LVL(lvl) debug_level = DEBUG_PROD;
#define DEBUG(lvl,...) ((debug_level <= lvl) ? fprintf(stderr, __VA_ARGS__) : (void)0)
#define DEBUG_ALL(...) (void)0
#define DEBUG_NONE(...) (void)0
#define DEBUG_TRACE(...) (void)0
#define DEBUG_DEVELOP(...) (void)0
#define DEBUG_LOG(...)  (debug_level <= DEBUG_LOG) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_PROD(...) (debug_level <= DEBUG_PROD) ? fprintf(stderr, __VA_ARGS__) : (void)0

#endif
