#include "printk_generic.h"
#include <stdarg.h>
#include "stdlib.h"

void reverse(char s[]) {
  for (int i = 0, j = strlen(s)-1; i<j; i++, j--) {
    char c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

static void printk_int(Display *disp, int i) {
  if(i == 0) {
    disp->PrintChar('0');
    return;
  }

  if(i < 0) {
    disp->PrintChar('-');
    i *= -1;
  }

  int pos = 0;
  char out[32];
  while(i > 0) {
    out[pos] = '0' + i % 10;
    i /= 10;
    pos++;
  }
  out[pos] = '\0';
  reverse(out);

  char *tmp = &out[0];
  while(*tmp != 0) {
    disp->PrintChar(*tmp);
    tmp++;
  }
}

int printk_generic(Display *disp, const char *fmt, va_list* argp) {
  const char *tmp = fmt;

  while(*tmp != 0) {
    switch(*tmp) {
    case '%':
      tmp++;
      switch(*tmp) {
      case 'd':
        printk_int(disp, va_arg(*argp, int));
        break;
      default:
        disp->PrintChar('?');
      }
      break;
    default:
      disp->PrintChar(*tmp);
    }
    tmp++;
  }
  return 0;
}
