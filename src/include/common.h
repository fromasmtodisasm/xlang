#pragma once

#include "debug.h"
#include "error.h"
#include <assert.h>

#define TRUE 1
#define FALSE 0
#define MALLOC(t,s) (t*)malloc(s)

#if defined _MSC_VER
#define strdup _strdup
//#define strndup(str) 
#endif


char *strndup(const char *str, size_t n);

