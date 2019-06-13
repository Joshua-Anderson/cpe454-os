#include <stdint.h>

#include "../syscall.h"
#include "arch/x86_64/arch.h"
#include "printk.h"
#include "proc/Scheduler.h"
#include "stdlib.h"
#include "syscall_arch.h"

#define SYSCALL_YIELD_NUM 1L
#define SYSCALL_EXIT_NUM 2L
#define SYSCALL_RUN_NUM 3L

void thread_yield() {
  set_reg("rdi", SYSCALL_YIELD_NUM);
  trap();
}

static void syscall_yield(struct IRQ_Frame* frame) {
  Process* cur_proc = Scheduler::CurProc();
  Process* new_proc = Scheduler::Reschedule(0);

  if (cur_proc == new_proc) {
    return;
  }

  cur_proc->Save(frame);
  new_proc->Load(frame);
}

void thread_exit() {
  set_reg("rdi", SYSCALL_EXIT_NUM);
  trap();
}

static void syscall_exit(struct IRQ_Frame* frame) {
  // Remove and resechedule the next thread
  Process* new_proc = Scheduler::Reschedule(1);
  if (new_proc) {
    new_proc->Load(frame);
    return;
  }

  // Return to the parent
  Scheduler::parent_proc.Load(frame);
}

void thread_run() {
  set_reg("rdi", SYSCALL_RUN_NUM);
  trap();
}

static void syscall_run(struct IRQ_Frame* frame) {
  // Save Parent Context

  // Return to the parent
  Scheduler::parent_proc.Save(frame);
  Process* new_proc = Scheduler::Reschedule(0);
  if (!new_proc) {
    return;
  }

  new_proc->Load(frame);
}

static inline uint64_t get_param(struct IRQ_Frame* frame, int num) {
  return *((uint64_t*)((intptr_t)frame - (sizeof(uint64_t) * (num + 1))));
}

void syscall_handler(uint32_t, uint32_t, struct IRQ_Frame* frame) {
  // RDI holds the system call number and is on the stack before the frame
  uint64_t num = get_param(frame, 0);
  printk("Syscall Num %lx\n", num);

  switch (num) {
    case SYSCALL_YIELD_NUM:
      syscall_yield(frame);
      break;
    case SYSCALL_EXIT_NUM:
      syscall_exit(frame);
      break;
    case SYSCALL_RUN_NUM:
      syscall_run(frame);
      break;
    default:
      break;
  }
}
