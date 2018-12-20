#pragma once
#include "config.h"

extern int debug_level;
#ifdef DEBUG_BUILD

typedef enum DEBUG_LEVEL {
  DEBUG_ALL,
  DEBUG_TRACE,
  DEBUG_LOG,
  DEBUG_PROD
}DEBUG_LEVEL;

#define SET_DEBUG_LVL(lvl) debug_level = lvl;
#define DEBUG(lvl,...) (debug_level <= lvl) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_ALL(...) (debug_level <= DEBUG_ALL) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_TRACE(...) if (debug_level <= DEBUG_TRACE) \
  { fprintf(stderr,"=>%s:%s:%d\t", __FILE__, __FUNCTION__, __LINE__); fprintf(stderr, __VA_ARGS__); }
#define DEBUG_LOG(...)  (debug_level <= DEBUG_LOG) ? fprintf(stderr, __VA_ARGS__) : (void)0
#define DEBUG_PROD(...) if (debug_level <= DEBUG_PROD) fprintf(stderr, __VA_ARGS__)

#else

#define DEBUG(...) 
#define DEBUG_LOG(...) 

#endif
