#ifndef __STDLIB_H__
#define __STDLIB_H__

#include <stddef.h>

#define ROUND_UP(num, mult) ((num + mult - 1) / mult) * mult

extern "C" {
void __cxa_pure_virtual();
void *memset(void *dst, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
size_t strlen(const char *s);
}

#endif
