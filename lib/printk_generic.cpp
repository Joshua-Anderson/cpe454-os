#include "printk_generic.h"
#include <stdarg.h>
#include "stdlib.h"

static void reverse(char s[]) {
  for (int i = 0, j = strlen(s)-1; i<j; i++, j--) {
    char c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

static inline char itoc(int i, int base) {
  if(base <= 10) {
    return '0' + i % base;
  }

  if(base == 16) {
    if(i % base < 10) {
      return '0' + i % base;
    }

    return 'a' + (i%base) - 10;
  }

  return '?';
}

static void printk_str(Display *disp, char *str) {
  while(*str != 0) {
    disp->PrintChar(*str);
    str++;
  }
}

static void printk_int(Display *disp, int i, int base) {
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
    out[pos] = itoc(i, base);
    i /= base;
    pos++;
  }
  out[pos] = '\0';
  reverse(out);

  printk_str(disp, &out[0]);
}

int printk_generic(Display *disp, const char *fmt, va_list* argp) {
  const char *tmp = fmt;

  while(*tmp != 0) {
    switch(*tmp) {
    case '%':
      tmp++;
      switch(*tmp) {
      case '%':
        disp->PrintChar('%');
        break;
      case 'c':
        disp->PrintChar((char) va_arg(*argp, int));
        break;
      case 'd':
        printk_int(disp, va_arg(*argp, int), 10);
        break;
      case 's':
        printk_str(disp, va_arg(*argp, char *));
        break;
      case 'x':
        printk_int(disp, va_arg(*argp, int), 16);
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
