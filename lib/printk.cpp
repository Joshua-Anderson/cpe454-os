#include "printk.h"
#include "../drivers/display/Display.h"
#include "../drivers/display/x86_64/VgaConsole.h"


int printk(Display *disp, const char *fmt, ...) {
    const char *tmp = fmt;

    while(*tmp != 0) {
        disp->PrintChar(*tmp);
        tmp++;
    }

    return 0;
}

int printk(const char *fmt, ...) {
   static VgaConsole disp = VgaConsole();
   return printk(&disp, fmt);
}
