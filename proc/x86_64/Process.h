#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <stddef.h>
#include <stdint.h>

#include "syscall/x86_64/syscall_arch.h"

typedef void (*kentry_t)(void *);

struct Regs {
  // Registers in order they are pushed to the stack, don't reorder
  uint64_t r15, r14, r13, r12, r11, r10, r9, r8, rbp, rcx, rbx, rax, rdx, rsi,
      rdi, rsp, rip, rflags;
  uint16_t cs, ss, ds;
} __attribute__((packed));

enum ProcState { RUNNING, RUNABLE, EXITED, BLOCKED };

class Process {
 public:
  // Generic Process Interface (to be implemented by every arch)
  // Ideally there would be an generic abstract process class and a arch
  // specific implmentation Virtual classes make the scheduler harder to work
  // with though, since it makes classes harder to instanciate and genericise.
  Process(unsigned pid);
  Process(unsigned pid, kentry_t, void *);
  unsigned GetPid() { return pid; }
  int CanRun() { return (State == RUNNING || State == RUNABLE); }
  ProcState State;

  // x86_64 Specific Functions
  void Save(struct IRQ_Frame *);
  void Load(struct IRQ_Frame *);
  void Destroy();

 private:
  // Generic Variables
  unsigned pid;
  void *initial_stack = NULL;

  // x86_64 Specific Variables
  struct Regs regs = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

#endif
