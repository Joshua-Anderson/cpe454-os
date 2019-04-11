#include "printk_generic.h"

int printk_generic(Display *disp, const char *fmt, ...) {
  const char *tmp = fmt;

  while(*tmp != 0) {
    disp->PrintChar(*tmp);
    tmp++;
  }

  return 0;
}
