#ifndef __SYSCALL_ARCH_H__
#define __SYSCALL_ARCH_H__

#include <stdint.h>

#include "arch/x86_64/arch.h"
#include "irq/IRQ.h"

extern "C" {

struct IRQ_Frame {
  uint64_t rip;
  uint16_t cs;
  uint64_t res_2 : 48;
  uint64_t rflags;
  uint64_t rsp;
  uint16_t ss;
  uint64_t res_1 : 48;
} __attribute__((packed));

void syscall_handler(uint32_t, uint32_t, struct IRQ_Frame*);
}
#endif
