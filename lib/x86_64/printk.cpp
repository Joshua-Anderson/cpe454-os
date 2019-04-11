#include "printk.h"
#include "printk_generic.h"
#include "../../drivers/display/x86_64/VgaConsole.h"

int printk(const char *fmt, ...) {
   static VgaConsole disp = VgaConsole();
   return printk_generic(&disp, fmt);
}
