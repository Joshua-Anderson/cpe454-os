#include <stdint.h>

#include "Process.h"
#include "arch/x86_64/GDT.h"
#include "mm/Page.h"
#include "stdlib.h"

#define NUM_STACK_REGS 15
#define STACK_REGS_SIZE (sizeof(uint64_t) * NUM_STACK_REGS)

Process::Process() {}

Process::Process(kentry_t entry, void* arg) {
  Process::regs.rip = (uint64_t)entry;
  Process::regs.rdx = (uint64_t)arg;
  Process::regs.cs = GDT::CS_SEG;
  Process::initial_stack = Page::AllocKernStackMem();
  Process::regs.rsp = (uint64_t) Process::initial_stack;
  Process::State = RUNABLE;
}

void Process::Load(struct IRQ_Frame* frame) {
  frame->rip = Process::regs.rip;
  frame->cs = Process::regs.cs;
  frame->rflags = Process::regs.rflags;
  frame->rsp = Process::regs.rsp;
  frame->ss = Process::regs.ss;

  void* reg_start = (void*)((intptr_t)frame - STACK_REGS_SIZE);
  memcpy(reg_start, (void*)&(Process::regs), STACK_REGS_SIZE);
  Process::State = RUNNING;
}

void Process::Save(struct IRQ_Frame* frame) {
  Process::regs.rip = frame->rip;
  Process::regs.cs = frame->cs;
  Process::regs.rflags = frame->rflags;
  Process::regs.rsp = frame->rsp;
  Process::regs.ss = frame->ss;
  Process::State = RUNABLE;
}

void Process::Destroy() {
  Page::FreeKernStackMem(Process::initial_stack);
  Process::initial_stack = NULL;
}
