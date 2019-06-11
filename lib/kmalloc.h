#ifndef __KMALLOC_H__
#define __KMALLOC_H__

#include <stdint.h>

extern "C" {
void* kmalloc(uint32_t);
void kfree(void*);
}

#endif
