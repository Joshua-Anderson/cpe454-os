#ifndef __MULTIBOOT_H__
#define __MULTIBOOT_H__

#include <stdint.h>

extern "C" {
void parse_multiboot(uint32_t magic, uint8_t* header);
}


#endif
