#include "printk.h"
#include "printk_generic.h"
#include "../../drivers/display/x86_64/VgaConsole.h"
#include <stdarg.h>

int printk(const char *fmt, ...) {
   va_list argp;
   static VgaConsole disp = VgaConsole();

   va_start(argp, fmt);
   int i = printk_generic(&disp, fmt, &argp);
   va_end(argp);
   return i;
}
