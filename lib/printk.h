#ifndef __PRINTK_H__
#define __PRINTK_H__

#include <stdarg.h>
#include "arch/Platform.h"

#define printk(fmt, ...) print_disp(Platform::GetDflDisplay(), fmt, ##__VA_ARGS__)
#define printc(fmt, ...) print_disp(Platform::GetDflConsole(), fmt, ##__VA_ARGS__)
#define printa(fmt, ...) \
  printk(fmt, ##__VA_ARGS__); \
  printc(fmt, ##__VA_ARGS__)

extern "C" {
int print_disp(Display *disp, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
}


#endif
