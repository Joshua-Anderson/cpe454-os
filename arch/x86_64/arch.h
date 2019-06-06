#ifndef __ARCH_H__
#define __ARCH_H__

#include "stdint.h"

#define DF_FAULT 0x8
#define GP_FAULT 0xD
#define PF_FAULT 0xE

#define PS2_IRQ 33
#define SERIAL_IRQ 36

extern "C" {

inline void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

#define get_reg(reg, var) asm volatile("movq %%" reg ", %0" : "=r"(var) :)

#define set_reg(reg, var) asm ( "movq %0, %%" reg : : "r" (var));

inline void hlt() {
  asm volatile("hlt" : : );
}
}

#endif
