#ifndef __PRINTK_GENERIC_H__
#define __PRINTK_GENERIC_H__

#include <stdarg.h>
#include "../drivers/display/Display.h"

extern "C" {

int printk_generic(Display* disp, const char* fmt, va_list* argp);
}

#endif
