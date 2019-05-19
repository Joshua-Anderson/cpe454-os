#include <stdarg.h>

#include "printk.h"
#include "stdlib.h"
#include "arch/ArchInit.h"

static void reverse(char s[]) {
  for (int i = 0, j = strlen(s) - 1; i < j; i++, j--) {
    char c = s[i];
    s[i] = s[j];
    s[j] = c;
  }
}

static inline char itoc(int i, int base) {
  if (base <= 10) {
    return '0' + i % base;
  }

  if (base == 16) {
    if (i % base < 10) {
      return '0' + i % base;
    }

    return 'a' + (i % base) - 10;
  }

  return '?';
}

static void printk_str(Display *disp, char *str) {
  while (*str != 0) {
    disp->PrintChar(*str);
    str++;
  }
}

static void printk_uint(Display *disp, unsigned long i, int base) {
  if (i == 0) {
    disp->PrintChar('0');
    return;
  }

  int pos = 0;
  char out[32];
  while (i > 0) {
    out[pos] = itoc(i, base);
    i /= base;
    pos++;
  }
  out[pos] = '\0';
  reverse(out);

  printk_str(disp, &out[0]);
}

static void printk_int(Display *disp, long i, int base) {
  if (i < 0) {
    disp->PrintChar('-');
    i *= -1;
  }

  printk_uint(disp, (unsigned long)i, base);
}

int print_disp(Display *disp, const char *fmt, ...) {
  va_list argp;
  va_start(argp, fmt);
  const char *tmp = fmt;

  while (*tmp != 0) {
    switch (*tmp) {
      case '%':
        tmp++;
        switch (*tmp) {
          case '%':
            disp->PrintChar('%');
            break;
          case 'c':
            disp->PrintChar((char)va_arg(argp, int));
            break;
          case 'd':
            printk_int(disp, va_arg(argp, int), 10);
            break;
          case 'h':
            tmp++;
            switch (*tmp) {
              case 'd':
                printk_int(disp, (short)va_arg(argp, int), 10);
                break;
              case 'u':
                printk_uint(disp, (unsigned short)va_arg(argp, unsigned int),
                            10);
                break;
              case 'x':
                printk_uint(disp, (unsigned short)va_arg(argp, unsigned int),
                            16);
                break;
              default:
                disp->PrintChar('?');
            }
            break;
          case 'l':
            tmp++;
            switch (*tmp) {
              case 'd':
                printk_int(disp, va_arg(argp, long), 10);
                break;
              case 'u':
                printk_uint(disp, va_arg(argp, unsigned long), 10);
                break;
              case 'x':
                printk_uint(disp, va_arg(argp, unsigned long), 16);
                break;
              default:
                disp->PrintChar('?');
            }
            break;
          case 'p':
            printk_uint(disp, (intptr_t )va_arg(argp, void *), 16);
            break;
          case 's':
            printk_str(disp, va_arg(argp, char *));
            break;
          case 'u':
            printk_uint(disp, va_arg(argp, unsigned int), 10);
            break;
          case 'x':
            printk_uint(disp, va_arg(argp, unsigned int), 16);
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

  va_end(argp);
  return 0;
}

