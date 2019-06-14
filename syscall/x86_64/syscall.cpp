#include <stdint.h>

#include "../SysCall.h"
#include "arch/x86_64/arch.h"
#include "printk.h"
#include "proc/Scheduler.h"
#include "stdlib.h"
#include "syscall_arch.h"

#define SYSCALL_YIELD_NUM 1L
#define SYSCALL_EXIT_NUM 2L
#define SYSCALL_RUN_NUM 3L

static void syscall_yield(struct IRQ_Frame* frame) {
  Scheduler::GetCurProc()->Save(frame);
  Process* proc = Scheduler::Reschedule();

  // If there's no process, return to parent
  if (!proc) {
    Scheduler::ParentProc.Load(frame);
    return;
  }

  proc->Load(frame);
}

static void syscall_exit(struct IRQ_Frame* frame) {
  // Remove and resechedule the next thread
  Scheduler::GetCurProc()->State = EXITED;
  Process* new_proc = Scheduler::Reschedule();
  if (new_proc) {
    new_proc->Load(frame);
    return;
  }

  // Return to the parent
  Scheduler::ParentProc.Load(frame);
}

static void syscall_run(struct IRQ_Frame* frame) {
  // Save Parent Context

  Scheduler::ParentProc.Save(frame);
  Process* new_proc = Scheduler::Reschedule();
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
      FATAL("Unknown System Call %lu", num);
      break;
  }
}

void SysCall::ProcYield() {
  set_reg("rdi", SYSCALL_YIELD_NUM);
  trap();
}

void SysCall::ProcExit() {
  set_reg("rdi", SYSCALL_EXIT_NUM);
  trap();
}

void SysCall::ProcRun() {
  set_reg("rdi", SYSCALL_RUN_NUM);
  trap();
}
