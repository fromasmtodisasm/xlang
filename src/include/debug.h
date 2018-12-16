#pragma once

#ifdef DEBUG_BUILD

#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

#else

#define DEBUG(...) 

#endif
