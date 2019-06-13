#include <stdint.h>

#include "../syscall.h"
#include "arch/x86_64/arch.h"
#include "printk.h"
#include "stdlib.h"
#include "syscall_arch.h"

void thread_yield() {
  set_reg("rdi", 1L);
  trap();
}

static inline uint64_t get_param(struct IRQ_Frame* frame, int num) {
  return *((uint64_t*)((intptr_t)frame - (8 * (num + 1))));
}

void syscall_handler(uint32_t, uint32_t, struct IRQ_Frame* frame) {
  // RDI holds the system call number and is on the stack before the frame
  uint64_t num = get_param(frame, 0);
  printk("Syscall Num %lx\n", num);
}
