#ifndef __STDLIB_H__
#define __STDLIB_H__

//TODO: Is this the right call?
#define size_t int
#define NULL 0
void *memset(void *dst, int c, size_t n);
void *memcpy(void *dst, const void *src, size_t n);
size_t strlen(const char *s);

#endif
