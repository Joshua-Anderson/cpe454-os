#include "stdlib.h"

void *memset(void *dst, int c, size_t n) {
  int *tmp = (int *)dst;
  for (int i = 0; i < n; i++) {
    tmp[i] = c;
  }

  return dst;
}
