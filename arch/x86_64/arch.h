#ifndef __ARCH_H__
#define __ARCH_H__

#include "stdint.h"

#define DF_FAULT 0x8
#define GP_FAULT 0xD
#define PF_FAULT 0xE

#define PS2_IRQ 33

extern "C" {

inline void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

static inline void io_wait(void) {
  /* Port 0x80 is used for 'checkpoints' during POST. */
  /* The Linux kernel seems to think it is free for use :-/ */
  asm volatile("outb %%al, $0x80" : : "a"(0));
  /* %%al instead of %0 makes no difference.  TODO: does the register need to be
   * zeroed? */
}
}

#endif
