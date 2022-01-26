#pragma once

#include <stdio.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stdlib.h>

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <string.h>

#define MAX_STRING_LENGTH 1000

#define STRINGS_EQUAL(A,B)(strcmp(A, B) == 0)
#define FREE_AND_NULL(A) if(A != NULL) {free(A); A = NULL;}
