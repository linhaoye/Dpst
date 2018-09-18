#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEDUG
#define ph_debug(M, ...)
#endif

#endif