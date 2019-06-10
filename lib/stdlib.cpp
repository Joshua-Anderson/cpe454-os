#include "stdlib.h"
#include <stdint.h>

void __cxa_pure_virtual() {
  while (1)
    ;
}

void *memset(void *dst, int c, size_t n) {
  char *tmp = (char *)dst;
  for (size_t i = 0; i < n; i++) {
    tmp[i] = (char)c;
  }

  return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
  for (size_t i = 0; i < n; i++) {
    ((uint8_t *)dst)[i] = ((uint8_t *)src)[i];
  }

  return dst;
}

size_t strlen(const char *s) {
  size_t len = 0;
  while (*s != 0) {
    len++;
    s += 1;
  }

  return len;
}
