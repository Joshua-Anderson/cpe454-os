#include "stdlib.h"

void *memset(void *dst, int c, size_t n) {
  char *tmp = (char *)dst;
  for (int i = 0; i < n; i++) {
    tmp[i] = (char) c;
  }

  return dst;
}

void *memcpy(void *dst, const void *src, size_t n) {
  for (int i = 0; i < n; i++) {
    ((char *)dst)[i] = ((char *)src)[i];
  }

  return dst;
}

size_t strlen(const char *s) {
  size_t len = 0;
  while(*s != NULL) {
    len++;
    s += 1;
  }

  return len;
}
