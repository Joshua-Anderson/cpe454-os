#ifndef __PRINTK_GENERIC_H__
#define __PRINTK_GENERIC_H__

#include "../drivers/display/Display.h"

extern "C" {

int printk_generic(Display* disp, const char *fmt, ...);

}

#endif
